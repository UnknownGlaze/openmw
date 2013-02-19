#ifndef DATAFILESPAGE_H
#define DATAFILESPAGE_H

#include <QWidget>
#include <QModelIndex>

class QTableView;
class QSortFilterProxyModel;
class QAction;
class QToolBar;
class QSplitter;
class QMenu;

class ProfilesComboBox;
class DataFilesModel;

class TextInputDialog;
class ProfilesComboBox;
class GameSettings;
class LauncherSettings;
class PluginsProxyModel;

namespace Files { struct ConfigurationManager; }

class DataFilesPage : public QWidget
{
    Q_OBJECT

public:
    DataFilesPage(Files::ConfigurationManager &cfg, GameSettings &gameSettings, LauncherSettings &launcherSettings, QWidget *parent = 0);

    ProfilesComboBox *mProfilesComboBox;

    void writeConfig(QString profile = QString());
    void saveSettings();


public slots:
    void setCheckState(QModelIndex index);

    void filterChanged(const QString filter);
    void showContextMenu(const QPoint &point);
    void profileChanged(const QString &previous, const QString &current);
    void profileRenamed(const QString &previous, const QString &current);
    void updateOkButton(const QString &text);
    void updateSplitter();
    void updateViews();

    // Action slots
    void newProfile();
    void deleteProfile();
//    void moveUp();
//    void moveDown();
//    void moveTop();
//    void moveBottom();
    void check();
    void uncheck();
    void refresh();

private:
    DataFilesModel *mDataFilesModel;

    PluginsProxyModel *mPluginsProxyModel;
    QSortFilterProxyModel *mMastersProxyModel;

    QSortFilterProxyModel *mFilterProxyModel;

    QTableView *mMastersTable;
    QTableView *mPluginsTable;


    QToolBar *mProfileToolBar;
    QMenu *mContextMenu;
    QSplitter *mSplitter;

    QAction *mNewProfileAction;
    QAction *mDeleteProfileAction;
    QAction *mCheckAction;
    QAction *mUncheckAction;

//    QAction *mMoveUpAction;
//    QAction *mMoveDownAction;
//    QAction *mMoveTopAction;
//    QAction *mMoveBottomAction;

    Files::ConfigurationManager &mCfgMgr;

    GameSettings &mGameSettings;
    LauncherSettings &mLauncherSettings;

    TextInputDialog *mNewProfileDialog;

//    const QStringList checkedPlugins();
//    const QStringList selectedMasters();

    void createActions();
    void setupDataFiles();
    void setupConfig();
    void readConfig();

    void loadSettings();

};

#endif
