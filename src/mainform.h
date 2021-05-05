#ifndef MAINFORM_H
#define MAINFORM_H

#include <QDate>
#include <QList>
#include <QFile>
#include <QUrl>
#include <QSystemTrayIcon>
#include <QJsonDocument>
#include <QDragEnterEvent>
#include <QMessageBox>
#include <QSizeGrip>
#include <QTranslator>

#include "tablefilesmodel.h"
#include "tableusbmodel.h"
#include "tableviewdelegate.h"
#include "settings.h"
#include "widgetabout.h"
#include "frameless.h"
#include "switchcheckbox.h"

#include "popupmessagetab.h"
#include "bellindicator.h"

#include "localclient.h"

namespace Ui {
class MainForm;
}

class MainForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainForm(QWidget *parent = nullptr);
    ~MainForm();

    void connectionStatus_web(std::string status);
    void setStatusLicense(std::string status);

    void parseDevice(Device& device, std::string json);

    void check();

    void addFiles();
    void setSettings();

    QStringList getAvailableLanguages(bool isTwoCharFormat);
protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent * event) override;

    void showEvent(QShowEvent *event);

    void closeEvent(QCloseEvent * event);

    void changeEvent(QEvent *event) override;
signals:
    void openAboutSignal();

public slots:
    void connectionStatus_socket(bool);

    void setPluginInfo(QString pluginName, short pluginState);
    void setInfo(QString pluginName, QString json);
    void addNewFile(File file);

    void activateTray(QSystemTrayIcon::ActivationReason reason);

    void showDocLicense();
    void showDocHelp();

    void showSysMsgs();

    void SetViewPage();
    void OpenAbout();
    void trayOpen();
    void changeUITheme();
    void actNotificationState();

    void toggleFullscreen();

    void winMove();
    void winResize();

    void tableFilesColumnResize(int columnIndex, int oldSize, int newSize);
    void tableUSBColumnResize(int columnIndex, int oldSize, int newSize);

    void languageChanged(const QString &lang);

    void bellClicked(bool isOn);

    void slotEnableScanBtn(bool);

private:

    Ui::MainForm *ui;

    QTranslator translator;

    QSystemTrayIcon * tray;

    Settings* settings;

    PopupMessageTab* msgTab;
    BellIndicator* wdgBell;

    void loadDocuments();
    int loadDocument(const QString& doc_name);

    SwitchCheckBox* notificationState;
    SwitchCheckBox* lightThemeEnable;
    SwitchCheckBox* colorVerdict;
    QComboBox* languages;

    QString filename;

    QList<QUrl> tempUrls;
//    QList<File> files;
    /// TODO: либо оставлять файлы (но тащить их из соседнего каталога?)
    ///     либо - просто массив строк (а там уж их парсить)

//    WidgetAbout* aboutWidget;

    /// NOTE: таблица у каждого клиента своя?.. по идее да
    TableFilesModel* filesModel;
    TableUSBModel* deviceModel;

    FrameLess* aboutFrame;
    TableViewDelegate* delegate;

    void customizeFilesTable();
    void setTableFilesHeaderText(const int n_column, const short arrow);
    void customizeDevicesTable();
    void setTableUSBHeaderText(const int n_column, const short arrow);

    void clearPluginInfoLayout(QLayout *layout);

    // Сортировка одна, а фильтры отдельный на каждую колонку
    // FILES
    QSortFilterProxyModel *sortFilesModel;
    QSortFilterProxyModel *filterFilesModel_0;
    QSortFilterProxyModel *filterFilesModel_1;
    QSortFilterProxyModel *filterFilesModel_2;
    QSortFilterProxyModel *filterFilesModel_3;
    QSortFilterProxyModel *filterFilesModel_4;
    QSortFilterProxyModel *filterFilesModel_5;

    QLineEdit* leFilesFilter_0;
    QLineEdit* leFilesFilter_1;
    QLineEdit* leFilesFilter_2;
    QLineEdit* leFilesFilter_3;
    QLineEdit* leFilesFilter_4;
    QLineEdit* leFilesFilter_5;

    // USB
    QSortFilterProxyModel *sortUSBModel;
    QSortFilterProxyModel *filterUSBModel_0;
    QSortFilterProxyModel *filterUSBModel_1;
    QSortFilterProxyModel *filterUSBModel_2;
    QSortFilterProxyModel *filterUSBModel_3;
    QSortFilterProxyModel *filterUSBModel_4;
    QSortFilterProxyModel *filterUSBModel_5;

    QLineEdit* leUSBFilter_0;
    QLineEdit* leUSBFilter_1;
    QLineEdit* leUSBFilter_2;
    QLineEdit* leUSBFilter_3;
    QLineEdit* leUSBFilter_4;
    QLineEdit* leUSBFilter_5;

    QAction* actMaximize;
    QAction* actMinimize;
    QAction* actMove;
    QAction* actResize;
    QAction* actClose;

    QAction* actOpen;
    QAction* actAbout;
    QAction* actNotifications;

    bool drag;
    bool isFirstClose;

    enum ConnectionState {
        // эти статусы "локальные" - соединение между приложениями через сокет
        SOCKET_NOT_CONNETCTED,  // нет подключения к сокет-серверу
        SOCKET_CONNECTED,       // подключились к сокет-серверу
        // эти статусы приходят по сети
        WEB_NOT_CONNECTED,  // подключение к вебу выдаёт ошибку
        WEB_NOT_CONFIRMED,  // подключились к вебу, но Агент "не подтверждён"
        WEB_CONFIRMED       // подключились к вебу, Агент "подтверждён"
    };
    ConnectionState connectionState;
    void updateConnectionState(ConnectionState, bool lngChUpdate = false);

    LocalClient* socketClient;
};

#endif // MAINFORM_H
