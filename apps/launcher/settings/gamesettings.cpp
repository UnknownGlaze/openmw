#include <QTextStream>
#include <QDir>
#include <QString>
#include <QRegExp>
#include <QMap>

#include <QDebug>

#include <components/files/configurationmanager.hpp>

#include "gamesettings.hpp"

GameSettings::GameSettings(Files::ConfigurationManager &cfg)
    : mCfgMgr(cfg)
{
}

GameSettings::~GameSettings()
{
}

void GameSettings::validatePaths()
{
    if (mSettings.isEmpty())
        return;

    QStringList paths = mSettings.values(QString("data"));
    Files::PathContainer dataDirs;

    foreach (const QString &path, paths) {
        dataDirs.push_back(Files::PathContainer::value_type(path.toStdString()));
    }

    // Parse the data dirs to convert the tokenized paths
    mCfgMgr.processPaths(dataDirs);
    mDataDirs.clear();

    for (Files::PathContainer::iterator it = dataDirs.begin(); it != dataDirs.end(); ++it) {
        QString path = QString::fromStdString(it->string());
        path.remove(QChar('\"'));

        QDir dir(path);
        if (dir.exists())
            mDataDirs.append(path);
    }

    // Do the same for data-local
    QString local = mSettings.value(QString("data-local"));

    if (local.isEmpty())
        return;

    dataDirs.clear();
    dataDirs.push_back(Files::PathContainer::value_type(local.toStdString()));

    mCfgMgr.processPaths(dataDirs);

    if (!dataDirs.empty()) {
        QString path = QString::fromStdString(dataDirs.front().string());
        path.remove(QChar('\"'));

        QDir dir(path);
        if (dir.exists())
            mDataLocal = path;
    }
    qDebug() << mSettings;


}

QStringList GameSettings::values(const QString &key, const QStringList &defaultValues)
{
    if (!mSettings.values(key).isEmpty())
        return mSettings.values(key);
    return defaultValues;
}

bool GameSettings::readFile(QTextStream &stream)
{
    QMap<QString, QString> cache;
    QRegExp keyRe("^([^=]+)\\s*=\\s*(.+)$");

    while (!stream.atEnd()) {
        QString line = stream.readLine().simplified();

        if (line.isEmpty() || line.startsWith("#"))
            continue;

        if (keyRe.indexIn(line) != -1) {

            QString key = keyRe.cap(1).simplified();
            QString value = keyRe.cap(2).simplified();

            // There can be multiple keys
            if (key == QLatin1String("data") ||
                    key == QLatin1String("master") ||
                    key == QLatin1String("plugin"))
            {
                // Remove keys from previous config and overwrite them
                mSettings.remove(key);
                QStringList values = cache.values(key);
                if (!values.contains(value)) // Do not insert duplicate values
                    cache.insertMulti(key, value);
            } else {
                cache.insert(key, value);
            }
        }
    }

    if (mSettings.isEmpty()) {
        mSettings = cache; // This is the first time we read a file
        validatePaths();
        return true;
    }

    // Replace values from previous settings
    QMapIterator<QString, QString> i(cache);
    while (i.hasNext()) {
        i.next();

        // Don't remove existing data entries
        if (i.key() == QLatin1String("data"))
            continue;

        if (mSettings.contains(i.key()))
            mSettings.remove(i.key());
    }

    // Merge the changed keys with those which didn't
    mSettings.unite(cache);
    validatePaths();

    return true;
}

bool GameSettings::writeFile(QTextStream &stream)
{
    // Iterate in reverse order to preserve insertion order
    QMapIterator<QString, QString> i(mSettings);
    i.toBack();

    while (i.hasPrevious()) {
        i.previous();

        if (i.key() == QLatin1String("master") || i.key() == QLatin1String("plugin"))
            continue;

        // Quote paths with spaces
        if (i.key() == QLatin1String("data") || i.key() == QLatin1String("data")) {
            if (i.value().contains(" ")) {
                stream << i.key() << "=\"" << i.value() << "\"\n";
                continue;
            }
        }

        stream << i.key() << "=" << i.value() << "\n";

    }

    QStringList masters = mSettings.values(QString("master"));
    for (int i = masters.count(); i--;) {
        stream << "master=" << masters.at(i) << "\n";
    }

    QStringList plugins = mSettings.values(QString("plugin"));
    for (int i = plugins.count(); i--;) {
        stream << "plugin=" << plugins.at(i) << "\n";
    }

    return true;
}
