#include "mainform.h"
#include "ui_mainform.h"

//#include <QFileDialog>
//#include <QMimeData>
//#include <iostream>
//#include <QThread>
//#include <QSplashScreen>
//#include <QCryptographicHash>
//#include <QWindow>

//#include <QDebug>

#include "tabletooltip.h"

#include "plugininfolabel.h"

#include "Core/file.h"

// Для tray->showMessage время в milliseconds (1000)
#define TRAY_MSG_TIME_SHORT 1000
#define TRAY_MSG_TIME_NORM  1500
#define TRAY_MSG_TIME_LONG  2000

MainForm::MainForm(QWidget *parent):  QWidget(parent),  ui(new Ui::MainForm),
  connectionState(SOCKET_NOT_CONNETCTED)
{
    qDebug() << " Constructor of MainForm !";
    ui->setupUi(this);
    setlocale(LC_ALL, "ru_Ru.UTF-8");

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8")); //изменения

    QApplication::setOrganizationName("AVSoft");
    QApplication::setOrganizationDomain("avsw.ru");
    QApplication::setApplicationName("AgentGUI");

    msgTab = new PopupMessageTab(this);

    wdgBell = new BellIndicator(this);
    ui->hl_topRightCornerPanel->insertWidget(0, wdgBell);
    connect(wdgBell, &BellIndicator::signalClicked, this, &MainForm::bellClicked);

    connect(msgTab, &PopupMessageTab::signalHidden, wdgBell, &BellIndicator::click);
    connect(msgTab, &PopupMessageTab::signalAllMsgsRead, wdgBell, &BellIndicator::setMsgStatus);

    /*Set and run splash screen*/
    QPixmap pixmap(":/resources/SplashMin.png");
    QSplashScreen splash(pixmap);
    splash.show();
    QThread::sleep(1);
    splash.close();

    this->setWindowIcon(QIcon(":/resources/trayIcon.png"));

    this->setSettings();

    this->setWindowTitle("AthenaAgent");
    this->setAcceptDrops(true);
    this->setMouseTracking(true);

    this->setWindowFlag(Qt::WindowMinMaxButtonsHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    this->filesModel = new TableFilesModel(this);
    this->deviceModel = new TableUSBModel(this);

//    this->aboutWidget = new WidgetAbout();
//    aboutFrame = new FrameLess(aboutWidget);

    delegate = new TableViewDelegate(this);

    //------------------------------------------------
    //Table files view
    customizeFilesTable();

    //------------------------------------------------
    //Table USB view
    customizeDevicesTable();

    //--------------------------------------------------------------
    // WinControlMenu
    QMenu* controlMenu = new QMenu(this);

    actMaximize = new QAction(QIcon(":/resources/window-maximize-black.png"), tr("Развернуть")); // Восстановить
    actMinimize = new QAction(QIcon(":/resources/minimizeBlack.png"), tr("Свернуть"));
    actMove = new QAction(tr("Переместить"));
    actResize = new QAction(tr("Размер"));
    actClose = new QAction(QIcon(":/resources/CloseBlack.png"), tr("Закрыть"));

    controlMenu->addAction(actMaximize); // + actUnmaximize / actRestore
    controlMenu->addAction(actMinimize);
// This function was introduced in Qt 5.15. (а мы на 5.14, так что пока без этого)
//    controlMenu->addAction(actMove);
//    controlMenu->addAction(actResize);
    controlMenu->addSeparator();
    controlMenu->addAction(actClose);

    connect(actMaximize, &QAction::triggered, this, &MainForm::toggleFullscreen); // showMaximized
    connect(actMinimize, &QAction::triggered, this, &MainForm::showMinimized);
    connect(actMove, &QAction::triggered, this, &MainForm::winMove);
    connect(actResize, &QAction::triggered, this, &MainForm::winResize);
    connect(actClose, &QAction::triggered, this, &MainForm::close);

    ui->btnIconAgent->setPopupMode(QToolButton::InstantPopup);
    ui->btnIconAgent->setMenu(controlMenu);


    //--------------------------------------------------------------
    //System tray setings
    this->tray = new QSystemTrayIcon(this);
    this->tray->setToolTip("Athena Agent");
    this->tray->setIcon(QIcon(":/resources/trayIcon.png"));

    QMenu* trayMenu = new QMenu(this);

    actOpen = new QAction(tr("Открыть"));
    actAbout = new QAction(tr("Справка"));
/*
    actNotifications = new QAction();
    if(settings->getDisableNotification())
    {
        actNotifications->setText(tr("Включить уведомления"));
    }
    else
    {
        actNotifications->setText(tr("Отключить уведомления"));
    }
*/
    trayMenu->addAction(actOpen);
   // trayMenu->addAction(actNotifications);
    trayMenu->addAction(actAbout); // убираем

    connect(actOpen, &QAction::triggered, this, &MainForm::trayOpen);
    connect(actAbout, &QAction::triggered, this, &MainForm::OpenAbout);
  //  connect(actNotifications, &QAction::triggered, this, &MainForm::actNotificationState);

    tray->setContextMenu(trayMenu);
    this->tray->show();
    this->isFirstClose = true;

    //Connects
    connect(tray, &QSystemTrayIcon::activated, this, &MainForm::activateTray);

    connect(ui->btnClose, &QToolButton::pressed, this, &MainForm::close);
    connect(ui->btnMinimize, &QToolButton::pressed, this, &MainForm::showMinimized);

    connect(ui->btnFullScreenToggle, &QToolButton::pressed, this, &MainForm::toggleFullscreen);

    connect(ui->btnAddFile, &QPushButton::pressed, this, &MainForm::addFiles);

    connect(ui->toolBtnHelp, &QToolButton::pressed, this, &MainForm::showDocHelp);

    /// TODO: это всё надо будет удалить (toolBtnBell)
    ///         сейчас это чисто для визуальной корректировки с макетом
    ui->toolBtnBell->hide();
//    connect(ui->toolBtnBell, &QToolButton::pressed, this, &MainForm::showSysMsgs);

    connect(ui->btnMain, &QPushButton::pressed, this, &MainForm::SetViewPage);
    connect(ui->btnFiles, &QPushButton::pressed, this, &MainForm::SetViewPage);
    connect(ui->btnUSB, &QPushButton::pressed, this, &MainForm::SetViewPage);
    connect(ui->btnSettings, &QPushButton::pressed, this, &MainForm::SetViewPage);
    connect(ui->btnDocuments, &QPushButton::pressed, this, &MainForm::SetViewPage);

    connect(ui->pbLicenseDoc, &QPushButton::pressed, this, &MainForm::showDocLicense);
    connect(ui->pbHelpDoc, &QPushButton::pressed, this, &MainForm::showDocHelp);

    ui->btnMain->setChecked(true);
    ui->stackedWidget->setCurrentWidget(ui->mainPage);

    loadDocuments();

    //-----------------------------------------------------------
    // Подключение к локальному Агенту через сокет
    socketClient = new LocalClient("AthenaLocalServer", this);
    // по статусу подключения к PluginUI (AthenaAgent)
    connect(socketClient, &LocalClient::signalConnected, this, &MainForm::connectionStatus_socket);
    // + коннекты на приём/отправку
    connect(socketClient, &LocalClient::signalNewPluginInfo, this, &MainForm::setPluginInfo);
    connect(socketClient, &LocalClient::signalNewInfo, this, &MainForm::setInfo);
    connect(socketClient, &LocalClient::signalNewFile, this, &MainForm::addNewFile);

    //-----------------------------------------------------------
    // Подключаем кнопку сканирования файловой системы
    // * пока Агент не будет в состоянии АКТИВЕН+ПОДТВЁРЖДЁН
    // слать файлы на анализ нельзя - соотв.и скнировать систему смысла нет
    ui->pbScanSystem->setEnabled(false);
    connect(ui->pbScanSystem, &QPushButton::pressed,
            socketClient, &LocalClient::slotScanBtnPressed);
    connect(socketClient, &LocalClient::signalEnableScanBtn,
            this, &MainForm::slotEnableScanBtn);
    // с этой стороны не даём нажать кнопку несколько раз
    connect(ui->pbScanSystem, &QPushButton::pressed,
            this, [=](){ ui->pbScanSystem->setEnabled(false); });
}

void MainForm::showEvent(QShowEvent *)
{
    // Пришлось сделать - для общего(нормального) случая, когда на старте
    // мы сразу успешно подключаемся (сокет уже создан и подключён, а только
    // потом мы делаем конект не него - на отлавливание состояния)
    connectionStatus_socket(socketClient->isConnected());
}

MainForm::~MainForm()
{
    delete aboutFrame;
    delete ui;
}


/*!
 * \brief slotToggleFullscreen - переключение полноэкранного режима
 *
 * Помимо переключения нужен для смены иконки-состояния кнопки (в styleSheet).
 * (помимо "light" используем checked/unchecked)
 * + переключение текста в controlMenu
*/
void MainForm::toggleFullscreen()
{
    if(isFullScreen()) {
        ui->btnFullScreenToggle->setChecked(false);
        if(isMaximized()) {
//            actMaximize->setText(tr("Восстановить"));
            showMaximized();
        }
        else {
            actMaximize->setText(tr("Развернуть"));
            actMaximize->setIcon(QIcon(":/resources/window-maximize-black.png"));
            showNormal();
        }
    }
    else {
        actMaximize->setText(tr("Восстановить"));
        actMaximize->setIcon(QIcon(":/resources/window-restore-black.png"));
        ui->btnFullScreenToggle->setChecked(true);
        showFullScreen();
    }
}

// This function was introduced in Qt 5.15.
void MainForm::winMove() {
//    windowHandle()->startSystemMove();
}
void MainForm::winResize() {
//    windowHandle()->startSystemResize();
}

void MainForm::SetViewPage()
{
    QString objName = sender()->objectName();

    if(objName == "btnMain")
    {
        ui->stackedWidget->setCurrentWidget(ui->mainPage);
    }
    else if(objName == "btnFiles")
    {
        delegate->setMode(Mode::files);
        ui->stackedWidget->setCurrentWidget(ui->tableFilesPage);
        ui->tableFilesView->update();
    }
    else if(objName == "btnUSB")
    {
        delegate->setMode(Mode::devices);
        ui->stackedWidget->setCurrentWidget(ui->tableUSBPage);
        ui->tableUSBView->update();
    }
    else if(objName == "btnSettings")
    {
        ui->stackedWidget->setCurrentWidget(ui->settingsPage);
    }
    else if(objName == "btnDocuments")
    {
        ui->stackedWidget->setCurrentWidget(ui->documentsPage);
    }
}

void MainForm::OpenAbout()
{
//    aboutWidget->show();
    trayOpen();
    ui->stackedWidget->setCurrentWidget(ui->documentsPage);
    ui->btnDocuments->setChecked(true);
}

void MainForm::trayOpen()
{
    if(!this->isVisible())
    {
        this->showNormal();    }
    else
    {
        this->showMinimized();
        this->showNormal();
    }
}

void MainForm::changeUITheme()
{
    QString theme = "";

    if(settings->getEnableLightTheme())
    {
        theme = "light";
    }

    /*Labels(orange)*/
    ui->labelName->setProperty("style", theme);
    ui->labelName->style()->unpolish(ui->labelName);
    ui->labelName->style()->polish(ui->labelName);

    ui->nameLabel->setProperty("style", theme);
    ui->nameLabel->style()->unpolish(ui->nameLabel);
    ui->nameLabel->style()->polish(ui->nameLabel);

    /*Menu panel*/
    ui->widgetInterface->setProperty("style", theme);
    ui->widgetInterface->style()->unpolish(ui->widgetInterface);
    ui->widgetInterface->style()->polish(ui->widgetInterface);

    ui->widgetMenu->setProperty("style", theme);
    ui->widgetMenu->style()->unpolish(ui->widgetMenu);
    ui->widgetMenu->style()->polish(ui->widgetMenu);

    ui->btnMain->setProperty("style", theme);
    ui->btnMain->style()->unpolish(ui->btnMain);
    ui->btnMain->style()->polish(ui->btnMain);

    ui->btnFiles->setProperty("style", theme);
    ui->btnFiles->style()->unpolish(ui->btnFiles);
    ui->btnFiles->style()->polish(ui->btnFiles);

    ui->btnUSB->setProperty("style", theme);
    ui->btnUSB->style()->unpolish(ui->btnUSB);
    ui->btnUSB->style()->polish(ui->btnUSB);

    ui->btnSettings->setProperty("style", theme);
    ui->btnSettings->style()->unpolish(ui->btnSettings);
    ui->btnSettings->style()->polish(ui->btnSettings);

    ui->btnDocuments->setProperty("style", theme);
    ui->btnDocuments->style()->unpolish(ui->btnDocuments);
    ui->btnDocuments->style()->polish(ui->btnDocuments);

    /*Toolbar panel*/
    ui->btnClose->setProperty("style", theme);
    ui->btnClose->style()->unpolish(ui->btnClose);
    ui->btnClose->style()->polish(ui->btnClose);

    ui->btnFullScreenToggle->setProperty("style", theme);
    ui->btnFullScreenToggle->style()->unpolish(ui->btnFullScreenToggle);
    ui->btnFullScreenToggle->style()->polish(ui->btnFullScreenToggle);

    ui->btnMinimize->setProperty("style", theme);
    ui->btnMinimize->style()->unpolish(ui->btnMinimize);
    ui->btnMinimize->style()->polish(ui->btnMinimize);

//    ui->toolBtnBell->setProperty("style", theme);
//    ui->toolBtnBell->style()->unpolish(ui->toolBtnBell);
//    ui->toolBtnBell->style()->polish(ui->toolBtnBell);

    ui->toolBtnHelp->setProperty("style", theme);
    ui->toolBtnHelp->style()->unpolish(ui->toolBtnHelp);
    ui->toolBtnHelp->style()->polish(ui->toolBtnHelp);

    ui->btnAddFile->setProperty("style", theme);
    ui->btnAddFile->style()->unpolish(ui->btnAddFile);
    ui->btnAddFile->style()->polish(ui->btnAddFile);

    /*Main page*/
    ui->mainWidget->setProperty("style", theme);
    ui->mainWidget->style()->unpolish(ui->mainWidget);
    ui->mainWidget->style()->polish(ui->mainWidget);

    ui->licenseLabel->setProperty("style", theme);
    ui->licenseLabel->style()->unpolish(ui->licenseLabel);
    ui->licenseLabel->style()->polish(ui->licenseLabel);

    ui->connectionLabel->setProperty("style", theme);
    ui->connectionLabel->style()->unpolish(ui->connectionLabel);
    ui->connectionLabel->style()->polish(ui->connectionLabel);

    /*Table file page*/
    ui->tableFilesView->setProperty("style", theme);
    ui->tableFilesView->style()->unpolish(ui->tableFilesView);
    ui->tableFilesView->style()->polish(ui->tableFilesView);

    ui->tableFilesView->verticalHeader()->setProperty("style", theme);
    ui->tableFilesView->style()->unpolish(ui->tableFilesView->verticalHeader());
    ui->tableFilesView->style()->polish(ui->tableFilesView->verticalHeader());

    ui->tableFilesView->horizontalHeader()->setProperty("style", theme);
    ui->tableFilesView->style()->unpolish(ui->tableFilesView->horizontalHeader());
    ui->tableFilesView->style()->polish(ui->tableFilesView->horizontalHeader());

    /* ui->tableFilesView->cornerWidget()->style()->unpolish(ui->tableFilesView->cornerWidget());
    ui->tableFilesView->cornerWidget()->style()->polish(ui->tableFilesView->cornerWidget());*/

    /*Table USB page*/
    ui->tableUSBView->setProperty("style", theme);
    ui->tableUSBView->style()->unpolish(ui->tableUSBView);
    ui->tableUSBView->style()->polish(ui->tableUSBView);

    ui->tableUSBView->verticalHeader()->setProperty("style", theme);
    ui->tableUSBView->style()->unpolish(ui->tableUSBView->verticalHeader());
    ui->tableUSBView->style()->polish(ui->tableUSBView->verticalHeader());

    ui->tableUSBView->horizontalHeader()->setProperty("style", theme);
    ui->tableUSBView->style()->unpolish(ui->tableUSBView->horizontalHeader());
    ui->tableUSBView->style()->polish(ui->tableUSBView->horizontalHeader());

    /* ui->tableUSBView->cornerWidget()->style()->unpolish(ui->tableUSBView->cornerWidget());
    ui->tableUSBView->cornerWidget()->style()->polish(ui->tableUSBView->cornerWidget());*/

    /*Settings*/
    ui->widgetSettings->setProperty("style", theme);
    ui->widgetSettings->style()->unpolish(ui->widgetSettings);
    ui->widgetSettings->style()->polish(ui->widgetSettings);

    ui->settingsLanguage->setProperty("style", theme);
    ui->settingsLanguage->style()->unpolish(ui->settingsLanguage);
    ui->settingsLanguage->style()->polish(ui->settingsLanguage);

    ui->settingsNotification->setProperty("style", theme);
    ui->settingsNotification->style()->unpolish(ui->settingsNotification);
    ui->settingsNotification->style()->polish(ui->settingsNotification);

    ui->settingsTheme->setProperty("style", theme);
    ui->settingsTheme->style()->unpolish(ui->settingsTheme);
    ui->settingsTheme->style()->polish(ui->settingsTheme);

    ui->settingsPluginsInfo->setProperty("style", theme);
    ui->settingsPluginsInfo->style()->unpolish(ui->settingsPluginsInfo);
    ui->settingsPluginsInfo->style()->polish(ui->settingsPluginsInfo);

    ui->labelLanguage->setProperty("style", theme);
    ui->labelLanguage->style()->unpolish(ui->labelLanguage);
    ui->labelLanguage->style()->polish(ui->labelLanguage);

    ui->labelNotification->setProperty("style", theme);
    ui->labelNotification->style()->unpolish(ui->labelNotification);
    ui->labelNotification->style()->polish(ui->labelNotification);

    ui->labelTheme->setProperty("style", theme);
    ui->labelTheme->style()->unpolish(ui->labelTheme);
    ui->labelTheme->style()->polish(ui->labelTheme);

    ui->labelPluginsInfo->setProperty("style", theme);
    ui->labelPluginsInfo->style()->unpolish(ui->labelPluginsInfo);
    ui->labelPluginsInfo->style()->polish(ui->labelPluginsInfo);

    /*Documents*/
    ui->widgetDocuments->setProperty("style", theme);
    ui->widgetDocuments->style()->unpolish(ui->widgetDocuments);
    ui->widgetDocuments->style()->polish(ui->widgetDocuments);

    ui->lbDocuments->setProperty("style", theme);
    ui->lbDocuments->style()->unpolish(ui->lbDocuments);
    ui->lbDocuments->style()->polish(ui->lbDocuments);

    ui->pbLicenseDoc->setProperty("style", theme);
    ui->pbLicenseDoc->style()->unpolish(ui->pbLicenseDoc);
    ui->pbLicenseDoc->style()->polish(ui->pbLicenseDoc);

    ui->pbHelpDoc->setProperty("style", theme);
    ui->pbHelpDoc->style()->unpolish(ui->pbHelpDoc);
    ui->pbHelpDoc->style()->polish(ui->pbHelpDoc);
/*
    if(settings->getEnableLightTheme())
        ui->textEdit->setStyleSheet("QScrollBar::handle {background-color: rgb(255, 150, 110);border-radius: 6px;}");
    else
        ui->textEdit->setStyleSheet("QScrollBar::handle {background-color: rgb(230, 120, 75);border-radius: 6px;}");
*/
    /*SwitchCheckBox*/
    if(settings->getEnableLightTheme())
    {
        notificationState->setStyleSheet("QAbstractButton{color:black}");
        lightThemeEnable->setStyleSheet("QAbstractButton{color:black}");
        colorVerdict->setStyleSheet("QAbstractButton{color:black}");

        languages->setStyleSheet(
        {
                        "QComboBox{"
                        "color: black;"
                        "background-color: #c7c7c7;"
                        "border-style: outset;"
                        "border-width: 2px;"
                        "border-radius: 6px;"
                        "border-color: black;"
                        "selection-background-color:#e6784b; "
                        "font: 400 12pt \"Roboto\";"
                        "min-width: 8em;"
                        "padding:  6px;}"
                        "QComboBox QAbstractItemView{"
                        "color: black;"
                        "background-color:  #c7c7c7;"
                        "selection-background-color:#e6784b;"
                        "border-width: 1px;"
                        "border-style: outset;"
                        "border-color: white;"
                        "padding: 4px;"
                        "font: 400 12pt \"Roboto\";}"
                    });

        ui->settingsPluginsInfo->setStyleSheet("QLabel{color:black}");
    }
    else
    {
        notificationState->setStyleSheet("QAbstractButton{color:white}");
        lightThemeEnable->setStyleSheet("QAbstractButton{color:white}");
        colorVerdict->setStyleSheet("QAbstractButton{color:white}");

        languages->setStyleSheet(
        {
                        "QComboBox{"
                        "color: white;"
                        "background-color: #666666;"
                        "border-style: outset;"
                        "border-width: 2px;"
                        "border-radius: 6px;"
                        "border-color: white;"
                        "selection-background-color:#e6784b;"
                        "font: 400 12pt \"Roboto\";"
                        "min-width: 8em;"
                        "padding: 6px;}"
                        "QComboBox QAbstractItemView{"
                        "color: white;"
                        "background-color: #666666;"
                        "selection-background-color: #e6784b;"
                        "border-width: 1px;"
                        "border-color: white;"
                        "border-style: outset;"
                        "padding: 4px;"
                        "font: 400 12pt \"Roboto\";}"

                    });

        ui->settingsPluginsInfo->setStyleSheet("QLabel{color:white}");
    }

    wdgBell->setTheme(theme);
    msgTab->setTheme(theme);
}

void MainForm::actNotificationState()
{
    if(settings->getDisableNotification())
    {
        settings->changeNotificationState(1);
        actNotifications->setText(tr("Отключить уведомления"));
    }
    else
    {
        settings->changeNotificationState(0);
        actNotifications->setText(tr("Включить уведомления"));
    }
}

void MainForm::addFiles()
{
    if(dynamic_cast<QPushButton*>(sender()))
    {
        tempUrls << QFileDialog::getOpenFileUrls();
    }

    ui->stackedWidget->setCurrentWidget(ui->tableFilesPage);

    ui->btnFiles->setChecked(true);
//    ui->tableFilesView->setFocus();
    delegate->setMode(Mode::files);

    check();
/*
    std::thread th(&MainForm::check, this); // в отдельном потоке...
    th.detach();
*/
}

void MainForm::setSettings()
{
    settings = Settings::getSettings();

    QBrush brush(QColor(230, 120, 75));

    bool tmp = settings->getDisableNotification();
    bool tmp2 = settings->getEnableLightTheme();
    bool tmp3 = settings->getDisableColorVerdict();
    QString tmp4 = settings->getCurrentLanguage();

    QLocale::setDefault(tmp4);

    translator.load(":/languages/agentui_" + tmp4  +".qm");
    qApp->installTranslator(&translator);

//    SwitchCheckBox*
    notificationState = new SwitchCheckBox(tmp, tr("Отключить уведомления","create"), brush, this);
    notificationState->setLayoutDirection(Qt::RightToLeft);
    notificationState->setStyleSheet("QAbstractButton{font: 400 10pt \"Roboto\"; margin-left:20px;}");

//    SwitchCheckBox*
    lightThemeEnable = new SwitchCheckBox(tmp2, tr("Светлая тема","create"), brush, this);
    lightThemeEnable->setLayoutDirection(Qt::RightToLeft);
    lightThemeEnable->setStyleSheet("QAbstractButton{font: 400 10pt \"Roboto\"; margin-left:20px;}");

//    SwitchCheckBox*
    colorVerdict = new SwitchCheckBox(tmp3, tr("Отключить подсветку вердиктов","create"), brush, this);
    colorVerdict->setLayoutDirection(Qt::RightToLeft);
    colorVerdict->setStyleSheet("QAbstractButton{font: 400 10pt \"Roboto\"; margin-left:20px;}");

//    ComboBox*
    languages = new QComboBox();
    for(auto lang : getAvailableLanguages(false))
    {
        languages->addItem(lang);
    }

    languages->setEditable(false);
    languages->setCurrentText(QLocale::languageToString(QLocale(tmp4).language()));

    ui->settingsNotification->layout()->addWidget(notificationState);
    ui->settingsNotification->layout()->setContentsMargins(10,0,10,10);

    ui->settingsTheme->layout()->addWidget(lightThemeEnable);
    ui->settingsTheme->layout()->setContentsMargins(10,0,10,10);

    ui->settingsLanguage->layout()->addWidget(languages);
    ui->settingsLanguage->layout()->setContentsMargins(10,0,10,10);

    ui->settingsPluginsInfo->layout()->setContentsMargins(10,0,10,10);

/// NOTE: в текущей реализации убираем
//    ui->settingsTheme->layout()->addWidget(colorVerdict);
    colorVerdict->hide();

    connect(languages, QOverload<const QString &>::of(&QComboBox::currentIndexChanged),
         [=](const QString &lang)
    {
        this->languageChanged(lang);
    });
    connect(settings, &Settings::changeUITheme, this, &MainForm::changeUITheme);
    connect(notificationState, &SwitchCheckBox::stateChanged, settings, &Settings::changeNotificationState);
    connect(lightThemeEnable, &SwitchCheckBox::stateChanged, settings, &Settings::changeLightThemeState);
    connect(colorVerdict, &SwitchCheckBox::stateChanged, settings, &Settings::changeColorVerdictState);

    this->changeUITheme();
}

// Все, что нужно сделать для добавления новых языков - добавить .qm файлы в res.qrc -> languages
// После этого добавленные переводы будут доступны для выбора автоматически, ничего в коде менять не нужно
// единственное условие - файл должен обязательно называться agentui_**.qm, где ** - 2 char language code, валидный для создания QLocale
QStringList MainForm::getAvailableLanguages(bool isTwoCharFormat)
{
    QStringList langs;

    QStringList fileNames;
    QDirIterator it(":/languages",QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        if(it.fileInfo().isFile())
            fileNames << it.fileName();

        it.next();
    }

    //qDebug() << fileNames;

    for (int i = 0; i < fileNames.size(); ++i) {
     // получаем список доступных локализаций исходя из qm файлов
     QString locale;
     locale = fileNames[i]; // "TranslationExample_de.qm"
     locale.truncate(locale.lastIndexOf('.')); // "TranslationExample_de"
     locale.remove(0, locale.lastIndexOf('_') + 1); // "de"

     QString lang = isTwoCharFormat ? locale : QLocale::languageToString(QLocale(locale).language());
     if(!langs.contains(lang))
        langs.push_back(lang);
    }

    return langs;
}

void MainForm::parseDevice(Device& device, std::string json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json.c_str());

    device.id = doc["id"].toString().toStdString();
    device.vid = doc["vid"].toString().toStdString();
    device.pid = doc["pid"].toString().toStdString();
    device.name = doc["name"].toString().toStdString();
    device.serial = doc["serial"].toString().toStdString();
    device.date =  doc["time"].toString().toStdString();
    device.block_state = doc["verdict"].toString().toStdString();
}

/// NOTE: есть такой нюанс - можно добавлять файлы скопом
///     тогда тут будет обрабатываться список tmpUIrl
///     а вот слать на сервер  - по одному
void MainForm::check()
{
    File file;
    QFileInfo info;

    for(int i = 0; i < tempUrls.size(); i++)
    {
        info.setFile(tempUrls[i].toLocalFile());

        // Отправлять можем только файлы
        // закидывать каталоги с вложениями - не пролучится
        if(info.isDir()) {
            tray->showMessage(tr("Невозможно отправить каталог (только файлы)"),
                              info.fileName(),
                              QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information),
                              TRAY_MSG_TIME_SHORT);
            continue;
        }

        if(info.size() >= 300*1024*1024)
        {
            if(!settings->getDisableNotification())
            {
                tray->showMessage(tr("Файл недопустимого размера"),
                                  tr("Файл ") + info.fileName()
                                  + tr(" слишком большой. Максимальный размер 300мб"),
                                  QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information),
                                  TRAY_MSG_TIME_SHORT);
            }
            continue;
        }
        else if(info.size() <= 0)
        {
            if(!settings->getDisableNotification())
            {
                tray->showMessage(tr("Файл недопустимого размера"),
                                  tr("Файл ") + info.fileName()
                                  + tr(" пуст."),
                                  QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information),
                                  TRAY_MSG_TIME_SHORT);
            }
            continue;
        }
        else
        {
//            file.path = tempUrls[i].toLocalFile().toUtf8().data();
            file.path = tempUrls[i].toLocalFile().toStdString();
            QFile tmpfile(tempUrls[i].toLocalFile());

            if(tmpfile.open(QFile::ReadOnly))
            {
                QCryptographicHash h(QCryptographicHash::Sha256);

                if(h.addData(&tmpfile))
                {
//                    file.hash = QString(h.result().toHex()).toUtf8().data();
                    file.hash = QString(h.result().toHex()).toStdString();
                }
            }

            //file.hash = agentAPI->getSHA256(file.path.data());
//            file.name = tempUrls[i].fileName().toUtf8().data();
            file.name = tempUrls[i].fileName().toStdString();

            file.status = 1;
            file.verdict = "Undefined";
//            file.time = QDateTime::currentDateTime().toString(Qt::DateFormat::ISODateWithMs).toUtf8().data();
            file.time = QDateTime::currentDateTime().toString(Qt::DateFormat::ISODateWithMs).toStdString();

            socketClient->sendFileToServer(file);

            filesModel->addFile(file);
        }
    }
    tempUrls.clear();

    ui->tableFilesView->update();
}

// отрабатывает по одному файлу при стартовой выгрузке всего что есть из БД
void MainForm::addNewFile(File file)
{
    filesModel->addFile(file);
}


// При дисконекте чистим эту табу, потому что при
// новом подключении - повторно добавляются те же самые строки
// (инфо по плагинам)
//
// NOTE: Можно придумать комманды для управления UI
//     - зарезервировать что-то типа pluginName="CLEAR_CMD", pluginState=-1
//     по этому сочетанию явно вызывать clearPluginInfoLayout()
void MainForm::setPluginInfo(QString pluginName, short pluginState)
{
qDebug() << "\nsetPluginInfo(): pluginName " << pluginName
         << "  pluginState=" << pluginState;

    PluginInfoLabel* pluginInfo = new PluginInfoLabel(this);
    pluginInfo->setText(pluginName);
    pluginInfo->setState(pluginState);
    ui->settingsPluginsInfo->layout()->addWidget(pluginInfo);
}
// Тут разбираем информацию, что нам прислали и раскидываем её по GUI
void MainForm::setInfo(QString pluginName, QString json_q)
//void MainForm::setInfo(std::string pluginName, std::string json)
{
qDebug() << "\nsetInfo(): pluginName " << pluginName << " json: " << json_q;

    std::string json = json_q.toStdString();

    if(pluginName == "device_info")
    {
        Device dev;
        parseDevice(dev, json);
        dev.date = QDateTime::currentDateTimeUtc().toString(Qt::DateFormat::ISODateWithMs).toStdString();
        deviceModel->addDevice(dev);

        if(!settings->getDisableNotification())
        {
            if(dev.block_state == "Blocked")
            {
                tray->showMessage(tr("Обнаружено устройство"),
                                  tr("Устройство ") + QString::fromStdString(dev.name) + tr(" заблокировано!"),
                                  QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Critical),
                                  TRAY_MSG_TIME_LONG);
            }
            else if(dev.block_state == "Allowed")
            {
                tray->showMessage(tr("Обнаружено устройство"),
                                  tr("Устройство ") + QString::fromStdString(dev.name) + tr(" разрешено!"),
                                  QIcon(":/resources/benignNotificate.png"),
                                  TRAY_MSG_TIME_LONG);
            }
        }
        ui->tableUSBView->update();
    }
    else if(pluginName == "file")
    {
        File file;

        file.fillFromJson(json);

        filesModel->setStatus(file);

        if(file.status == 11 || file.status == -1)
        {
            if (!settings->getDisableNotification() && file.status == -1 && file.webid == -1)
            {
                tray->showMessage(tr("Ошибка отправки файла"),
                                  QString::fromStdString(file.name) + " " + QString::fromStdString(filesModel->verdictEnum[file.verdict]),
                        QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Warning),
                        TRAY_MSG_TIME_NORM);
            }
            else
            {
                QIcon icon;

                if(!settings->getDisableNotification())
                {
                    if(file.verdict == "Benign")
                    {
                        icon.addFile(":/resources/benignNotificate.png");
                    }
                    else if(file.verdict == "Grayware")
                    {
                        //icon.addFile("::/resources/.png");
                    }
                    else if(file.verdict == "Malware")
                    {
                        //icon.addFile(":/resources/.png");
                    }
                    tray->showMessage(tr("Файл проверен"),
                                    QString::fromStdString(file.name) + " " + QString::fromStdString(filesModel->verdictEnum[file.verdict]),
                                    icon,
                                    TRAY_MSG_TIME_LONG);
                }

            }
            ui->tableFilesView->update();
        }
    }
    // статус соединения с web'ом
    else if(pluginName == "agent")
        connectionStatus_web(json);

    /// NOTE: ещё должна быть информация по лицензии (когда будет лицензия)
    else if(pluginName == "license")
        setStatusLicense(json);
}


void MainForm::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void MainForm::dropEvent(QDropEvent *event)
{
    tempUrls << event->mimeData()->urls();
    drag = true;
    addFiles();
}

void MainForm::closeEvent(QCloseEvent *event)
{
    if(this->isVisible())
    {
        event->ignore();
        this->hide();
/* пока убрал
        if(isFirstClose && !settings->getDisableNotification())
        {
            QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
            tray->showMessage("Athena Agent",
                              tr("Чтобы развернуть окно приложения, щелкните двойным кликом по иконке"),
                              icon,
                              TRAY_MSG_TIME_LONG);
            isFirstClose = false;
        }
*/
    }
    /*else
            {
                //this->close();
                //agentAPI->notify("exit","exit");// реализовать выход из агента
            }*/
}
void MainForm::clearPluginInfoLayout(QLayout *layout)
{
    QLayoutItem *item;
    while((item = layout->takeAt(1))) // 0 - это заголовок фрейма ("Компоненты")
    {
        // Чтобы не было артефактов, Widget'ы надо корректно закрывать
        // (со всеми положенными событиями)
        if(item->widget())
            item->widget()->close();
        // NOTE: можно смотреть что это точно объект класса PlugIninfolabel
        delete item;
    }
}
void MainForm::changeEvent(QEvent *event)
{
    if(event != nullptr)
    {
     switch(event->type())
     {
      // если translator был изменен
      case QEvent::LanguageChange:
         //элементы, добавляемые на форму из кода, не обновляются через retranslateUi
         //поэтому надо обновлять их вручную тут

         notificationState->setText(tr("Отключить уведомления","retranslate"));
         lightThemeEnable->setText(tr("Светлая тема","retranslate"));
         colorVerdict->setText(tr("Отключить подсветку вердиктов","retranslate"));

         actAbout->setText(tr("Справка"));
         actOpen->setText(tr("Открыть"));

         actMaximize->setText(tr("Развернуть")); // Восстановить
         actMinimize->setText(tr("Свернуть"));
         actMove->setText(tr("Переместить"));
         actResize->setText(tr("Размер"));
         actClose->setText(tr("Закрыть"));

         msgTab->retranslate();

         ui->retranslateUi(this);

         // Явно обновляем поле "connection"
         updateConnectionState(connectionState, true);
         break;

//      // если была изменена локаль
//      case QEvent::LocaleChange:
//         qDebug() << "locale changed";
//         QString locale = QLocale::system().name();
//         locale.truncate(locale.lastIndexOf('_'));
//         languageChanged(locale);
//         break;
     }
    }

    QWidget::changeEvent(event);
}

// Состояние подключения к сокет-серверу
// - по умолчанию висит "не подключён" (веб пока не ответил)
// - при подключении "подключен" (по сигналу)
// - при разрыве "не подключён" (по сигналу)
void MainForm::connectionStatus_socket(bool cn)
{
    if(cn) // подключены
        updateConnectionState(SOCKET_CONNECTED);
    else // не подключены/отключили
        updateConnectionState(SOCKET_NOT_CONNETCTED);
}

// эти статусы приходят только, когда мы подключились к сервер-сокету (логично)
// - "нет соединения"
// - "не подтверждён"   подключились к вебу (но не подтверждён)
// - "активно"          подключились к вебу + подтверждён (можно работать)
void MainForm::connectionStatus_web(std::string status)
{
    if(status == "disconnect")
        updateConnectionState(WEB_NOT_CONNECTED);
    else if(status == "not_confirmed")
        updateConnectionState(WEB_NOT_CONFIRMED);
    else if(status == "connect")
        updateConnectionState(WEB_CONFIRMED);
}

/// NOTE: Тут надо отображать статус лицензии
///  + msgTab->addNewMessage()
void MainForm::setStatusLicense(std::string status)
{
    QString st_status = "";

    /// статусы лицензии > st_status > styleSheet

    ui->license->setProperty("status", st_status);
    ui->license->style()->unpolish(ui->license);
    ui->license->style()->polish(ui->license);
}

void MainForm::activateTray(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
/* активация по левому клику - она перекрывает DoubleClick
    case QSystemTrayIcon::Trigger: // вызов меню по левому клику
        tray->contextMenu()->popup(QCursor::pos());
        break;*/
    case QSystemTrayIcon::DoubleClick:
    {
        if (!this->isVisible())
        {
            this->showMinimized();
            this->showNormal();
        }
        this->setWindowState(Qt::WindowState::WindowActive);

        break;
    }
    default:
        break;
    }
}

/*!
 * \brief MainForm::loadDocuments - загрузка документов
 *
 * Создаём список всех необходимых.
 * Они есть в ресурсах, но их нет после установки (вдруг?).
 * При первом старте - просто из ресурсов закидываем их в каталог
 *
 * NOTE: в сборке Win они просто лежат в корне, так что из *.qrc их выкидывать и всё
 * (посмотреть что там с deb пакетом...)
 */
void MainForm::loadDocuments()
{
    QStringList docList;
    docList << "ATHENA_AGENT_HELP.pdf";
    docList << "ATHENA_AGENT_EULA.pdf";

    for (int i = 0; i < docList.size(); ++i)
        loadDocument(docList.at(i));
}

/// NOTE: храним в локальной директории !!
int MainForm::loadDocument(const QString& doc_name)
{
    // по нормальному - такие файлы должны развёртываться при установке
    if(QFile(QDir::currentPath() + "/" + doc_name).exists())
        return 0; // уже есть

    QString from_res = "://resources/" + doc_name;
    QString to_local = doc_name; // ещё раз - храним локально

    return QFile::copy(from_res, to_local);
}


void MainForm::showDocLicense()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath()+"/ATHENA_AGENT_EULA.pdf"));
}
void MainForm::showDocHelp()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath()+"/ATHENA_AGENT_HELP.pdf"));
}

/// TODO: отображать выпадающий виджет с сообщениями
void MainForm::showSysMsgs()
{
}

/*!
 * \brief MainForm::setTableFilesHeaderText - устанавливаем текст в шапке таблицы файлов
 *
 * Обновляем текст при сортировке - и к текущему столбцу на сортировке ставим символ
 * "↑"/"↓" в зависимости от того, сортируем мы столбец по возрастанию/убыванию
 *
 * \param n_column - номер столбца для сортировки
 * \param arrow - 0 по возрастанию(↑), 1 по убыванию(↓)
 */
void MainForm::setTableFilesHeaderText(const int n_column, const short arrow)
{
    QStringList strLHeader = {
        tr("Дата и время"),
        tr("Имя файла"),
        tr("Путь к файлу"),
        tr("Статус"),
        tr("Вердикт"),
        tr("Контрольная сумма")};

    QString str_arrow = arrow ? "↑" : "↓";
    strLHeader[n_column] = strLHeader[n_column] + str_arrow;

    ui->twFilesHeader->setHorizontalHeaderLabels(strLHeader);
}
void MainForm::customizeFilesTable()
{
    ui->tableFilesView->setShowGrid(true);
    ui->tableFilesView->setCornerButtonEnabled(false);

    sortFilesModel = new QSortFilterProxyModel(this);
    sortFilesModel->setSourceModel(filesModel);

    filterFilesModel_0 = new QSortFilterProxyModel(this);
    filterFilesModel_0->setSourceModel(sortFilesModel);
    filterFilesModel_1 = new QSortFilterProxyModel(this);
    filterFilesModel_1->setSourceModel(filterFilesModel_0);
    filterFilesModel_2 = new QSortFilterProxyModel(this);
    filterFilesModel_2->setSourceModel(filterFilesModel_1);
    filterFilesModel_3 = new QSortFilterProxyModel(this);
    filterFilesModel_3->setSourceModel(filterFilesModel_2);
    filterFilesModel_4 = new QSortFilterProxyModel(this);
    filterFilesModel_4->setSourceModel(filterFilesModel_3);
    filterFilesModel_5 = new QSortFilterProxyModel(this);
    filterFilesModel_5->setSourceModel(filterFilesModel_4);

    ui->tableFilesView->setModel(filterFilesModel_5);
    ui->tableFilesView->setSortingEnabled(true);

    ui->tableFilesView->setItemDelegate(delegate);

    ui->tableFilesView->horizontalHeader()->setStretchLastSection(true);
    ui->tableFilesView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

    ui->tableFilesView->verticalHeader()->hide();
    ui->tableFilesView->horizontalHeader()->hide(); // убираем

    //-------------------------------------------------------------
    ui->twFilesHeader->horizontalHeader()->setVisible(true);
    connect(ui->twFilesHeader->horizontalHeader(),SIGNAL(sectionResized(int, int, int)),
             this, SLOT(tableFilesColumnResize(int, int, int)));

    leFilesFilter_0 = new QLineEdit(this);
    leFilesFilter_1 = new QLineEdit(this);
    leFilesFilter_2 = new QLineEdit(this);
    leFilesFilter_3 = new QLineEdit(this);
    leFilesFilter_4 = new QLineEdit(this);
    leFilesFilter_5 = new QLineEdit(this);

    ui->twFilesHeader->setCellWidget(0, 0, leFilesFilter_0);
    ui->twFilesHeader->setCellWidget(0, 1, leFilesFilter_1);
    ui->twFilesHeader->setCellWidget(0, 2, leFilesFilter_2);
    ui->twFilesHeader->setCellWidget(0, 3, leFilesFilter_3);
    ui->twFilesHeader->setCellWidget(0, 4, leFilesFilter_4);
    ui->twFilesHeader->setCellWidget(0, 5, leFilesFilter_5);

    ui->twFilesHeader->setColumnWidth(0, 130); // 120
    ui->tableFilesView->setColumnWidth(0, 130);
    ui->twFilesHeader->setColumnWidth(1, 150);
    ui->tableFilesView->setColumnWidth(1, 150);
    ui->twFilesHeader->setColumnWidth(2, 120);
    ui->tableFilesView->setColumnWidth(2, 120);
    ui->twFilesHeader->setColumnWidth(3, 105); //105
    ui->tableFilesView->setColumnWidth(3, 105); //105
    ui->twFilesHeader->setColumnWidth(4, 120); // 90
    ui->tableFilesView->setColumnWidth(4, 120); // 90

    //-------------------------------------------------------------
    // Сортировка таблицы файлов
    QHeaderView *filesHeader = qobject_cast<QTableView *>(ui->twFilesHeader)->horizontalHeader();
    connect(filesHeader, &QHeaderView::sectionClicked, [this](int logicalIndex)
    {
        if(sortFilesModel->sortOrder() == Qt::AscendingOrder) {
            sortFilesModel->sort(logicalIndex, Qt::DescendingOrder);
            setTableFilesHeaderText(logicalIndex, 1); // по убыванию
        }
        else {
            sortFilesModel->sort(logicalIndex);
            setTableFilesHeaderText(logicalIndex, 0); // по возрастанию
        }
    });

    //-------------------------------------------------------------
    // Фильтр для таблицы файлов
    // отрабатывается по editingFinished (enter по строке ввода)
    // для каждого поля можно своё выражение прописать (как и источник сигнала)
    connect(leFilesFilter_0, &QLineEdit::editingFinished, [this]() {
        filterFilesModel_0->setFilterKeyColumn(0);
        QRegExp rx(leFilesFilter_0->text(), Qt::CaseInsensitive);
        filterFilesModel_0->setFilterRegExp(rx);
    });
    connect(leFilesFilter_1, &QLineEdit::editingFinished, [this]() {
        filterFilesModel_1->setFilterKeyColumn(1);
        QRegExp rx(leFilesFilter_1->text(), Qt::CaseInsensitive);
        filterFilesModel_1->setFilterRegExp(rx);
    });
    connect(leFilesFilter_2, &QLineEdit::editingFinished, [this]() {
        filterFilesModel_2->setFilterKeyColumn(2);
        QRegExp rx(leFilesFilter_2->text(), Qt::CaseInsensitive);
        filterFilesModel_2->setFilterRegExp(rx);
    });
    connect(leFilesFilter_3, &QLineEdit::editingFinished, [this]() {
        filterFilesModel_3->setFilterKeyColumn(3);
        QRegExp rx(leFilesFilter_3->text(), Qt::CaseInsensitive);
        filterFilesModel_3->setFilterRegExp(rx);
    });
    connect(leFilesFilter_4, &QLineEdit::editingFinished, [this]() {
        filterFilesModel_4->setFilterKeyColumn(4);
        QRegExp rx(leFilesFilter_4->text(), Qt::CaseInsensitive);
        filterFilesModel_4->setFilterRegExp(rx);
    });
    connect(leFilesFilter_5, &QLineEdit::editingFinished, [this]() {
        filterFilesModel_5->setFilterKeyColumn(5);
        QRegExp rx(leFilesFilter_5->text(), Qt::CaseInsensitive);
        filterFilesModel_5->setFilterRegExp(rx);
    });

    ui->tableFilesView->viewport()->installEventFilter(new TableToolTip(ui->tableFilesView));
    ui->tableFilesView->update();
}

void MainForm::setTableUSBHeaderText(const int n_column, const short arrow)
{
    QStringList strLHeader = {
        tr("Дата и время"),
        tr("Имя устройства"),
        tr("VID"),
        tr("PID"),
        tr("Серийный номер"),
        tr("Вердикт")};

    QString str_arrow = arrow ? "↑" : "↓";
    strLHeader[n_column] = strLHeader[n_column] + str_arrow;

    ui->twUSBHeader->setHorizontalHeaderLabels(strLHeader);
}
void MainForm::customizeDevicesTable()
{
    // Можно сделать по двойному клику - копирование содержимого ячейки
    //  в буфер обмена (или всей строки)

    ui->tableUSBView->setShowGrid(false);
    ui->tableUSBView->setCornerButtonEnabled(false);

    sortUSBModel = new QSortFilterProxyModel(this);
    sortUSBModel->setSourceModel(deviceModel);

    filterUSBModel_0 = new QSortFilterProxyModel(this);
    filterUSBModel_0->setSourceModel(sortUSBModel);
    filterUSBModel_1 = new QSortFilterProxyModel(this);
    filterUSBModel_1->setSourceModel(filterUSBModel_0);
    filterUSBModel_2 = new QSortFilterProxyModel(this);
    filterUSBModel_2->setSourceModel(filterUSBModel_1);
    filterUSBModel_3 = new QSortFilterProxyModel(this);
    filterUSBModel_3->setSourceModel(filterUSBModel_2);
    filterUSBModel_4 = new QSortFilterProxyModel(this);
    filterUSBModel_4->setSourceModel(filterUSBModel_3);
    filterUSBModel_5 = new QSortFilterProxyModel(this);
    filterUSBModel_5->setSourceModel(filterUSBModel_4);

    ui->tableUSBView->setModel(filterUSBModel_5);
    ui->tableUSBView->setSortingEnabled(true);

    ui->tableUSBView->setItemDelegate(delegate);

//    ui->tableUSBView->horizontalHeader()->setSectionsMovable(true);
    ui->tableUSBView->horizontalHeader()->setStretchLastSection(true);
    ui->tableUSBView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

    ui->tableUSBView->verticalHeader()->hide();
    ui->tableUSBView->horizontalHeader()->hide(); // убираем

    //-------------------------------------------------------------
    ui->twUSBHeader->horizontalHeader()->setVisible(true);
    connect(ui->twUSBHeader->horizontalHeader(),SIGNAL(sectionResized(int, int, int)),
             this, SLOT(tableUSBColumnResize(int, int, int)));

    leUSBFilter_0 = new QLineEdit(this);
    leUSBFilter_1 = new QLineEdit(this);
    leUSBFilter_2 = new QLineEdit(this);
    leUSBFilter_3 = new QLineEdit(this);
    leUSBFilter_4 = new QLineEdit(this);
    leUSBFilter_5 = new QLineEdit(this);

    ui->twUSBHeader->setCellWidget(0, 0, leUSBFilter_0);
    ui->twUSBHeader->setCellWidget(0, 1, leUSBFilter_1);
    ui->twUSBHeader->setCellWidget(0, 2, leUSBFilter_2);
    ui->twUSBHeader->setCellWidget(0, 3, leUSBFilter_3);
    ui->twUSBHeader->setCellWidget(0, 4, leUSBFilter_4);
    ui->twUSBHeader->setCellWidget(0, 5, leUSBFilter_5);

    ui->twUSBHeader->setColumnWidth(0, 120);
    ui->tableUSBView->setColumnWidth(0, 120);
    ui->tableUSBView->setColumnWidth(1, 150);
    ui->twUSBHeader->setColumnWidth(1, 150);
    ui->tableUSBView->setColumnWidth(2, 120);
    ui->twUSBHeader->setColumnWidth(2, 120);
    ui->tableUSBView->setColumnWidth(3, 105);
    ui->twUSBHeader->setColumnWidth(3, 105);
    ui->tableUSBView->setColumnWidth(4, 150);
    ui->twUSBHeader->setColumnWidth(4, 150);

    //-------------------------------------------------------------
    // Сортировка таблицы устройств
    QHeaderView *devicesHeader = qobject_cast<QTableView *>(ui->twUSBHeader)->horizontalHeader();
    connect(devicesHeader, &QHeaderView::sectionClicked, [this](int logicalIndex)
    {
        if(sortUSBModel->sortOrder() == Qt::AscendingOrder) {
            sortUSBModel->sort(logicalIndex, Qt::DescendingOrder);
            setTableUSBHeaderText(logicalIndex, 1); // по убыванию
        }
        else {
            sortUSBModel->sort(logicalIndex);
            setTableUSBHeaderText(logicalIndex, 0); // по возрастанию
        }
    });

    //-------------------------------------------------------------
    // Фильтр для таблицы файлов
    // отрабатывается по editingFinished (enter по строке ввода)
    // для каждого поля можно своё выражение прописать (как и источник сигнала)
    connect(leUSBFilter_0, &QLineEdit::editingFinished, [this]() {
        filterUSBModel_0->setFilterKeyColumn(0);
        QRegExp rx(leUSBFilter_0->text(), Qt::CaseInsensitive);
        filterUSBModel_0->setFilterRegExp(rx);
    });
    connect(leUSBFilter_1, &QLineEdit::editingFinished, [this]() {
        filterUSBModel_1->setFilterKeyColumn(1);
        QRegExp rx(leUSBFilter_1->text(), Qt::CaseInsensitive);
        filterUSBModel_1->setFilterRegExp(rx);
    });
    connect(leUSBFilter_2, &QLineEdit::editingFinished, [this]() {
        filterUSBModel_2->setFilterKeyColumn(2);
        QRegExp rx(leUSBFilter_2->text(), Qt::CaseInsensitive);
        filterUSBModel_2->setFilterRegExp(rx);
    });
    connect(leUSBFilter_3, &QLineEdit::editingFinished, [this]() {
        filterUSBModel_3->setFilterKeyColumn(3);
        QRegExp rx(leUSBFilter_3->text(), Qt::CaseInsensitive);
        filterUSBModel_3->setFilterRegExp(rx);
    });
    connect(leUSBFilter_4, &QLineEdit::editingFinished, [this]() {
        filterUSBModel_4->setFilterKeyColumn(4);
        QRegExp rx(leUSBFilter_4->text(), Qt::CaseInsensitive);
        filterUSBModel_4->setFilterRegExp(rx);
    });
    connect(leUSBFilter_5, &QLineEdit::editingFinished, [this]() {
        filterUSBModel_5->setFilterKeyColumn(5);
        QRegExp rx(leUSBFilter_5->text(), Qt::CaseInsensitive);
        filterUSBModel_5->setFilterRegExp(rx);
    });

    ui->tableUSBView->viewport()->installEventFilter(new TableToolTip(ui->tableUSBView));
    ui->tableUSBView->update();
}

void MainForm::tableFilesColumnResize(int columnIndex, int oldSize, int newSize)
{
    ui->tableFilesView->setColumnWidth(columnIndex, newSize);
}

void MainForm::tableUSBColumnResize(int columnIndex, int oldSize, int newSize)
{
    ui->tableUSBView->setColumnWidth(columnIndex, newSize);
}

void MainForm::languageChanged(const QString &lang)
{
    QString twoCharLangCode;
    QStringList availableLangs = getAvailableLanguages(false);

    //e.g Russian -> ru, English - en
    for(int i =0; i< availableLangs.size();i++)
    {
        if(availableLangs[i] == lang)
            twoCharLangCode = getAvailableLanguages(true)[i];
    }

    qApp->removeTranslator(&translator);
    translator.load(":/languages/agentui_" + twoCharLangCode + ".qm");
    qApp->installTranslator(&translator);
    settings->changeCurrentLanguage(twoCharLangCode);

    QLocale::setDefault(QLocale(twoCharLangCode));
}


/*!
 * \brief MainForm::updateConnectionState - обновляем сетевой статус
 * \param newState - новое состояние (текущее хранится в connectionState)
 *
 * Тут:
 *      - обновляем текст в поле "Соединение с ATHENA"
 *      - окрашиваем поле под этот текст (задаём стиль)
 *      - управляем блоком сообщений об ошибках соединения
 *
 * Метод вызывается из:
 *      connectionStatus_web(std:string)
 *      connectionStatus_socket(bool)
 *
 */
void MainForm::updateConnectionState(ConnectionState newState, bool lngChUpdate)
{
    if(newState == connectionState && !lngChUpdate)
        return;

    connectionState = newState;
    QString st_status = "";

    if(newState == SOCKET_NOT_CONNETCTED)
    {
        st_status = "gray";
        ui->connection->setText(tr("НЕ ПОДКЛЮЧЁН"));

        clearPluginInfoLayout(ui->settingsPluginsInfo->layout());
        ui->settingsPluginsInfo->hide();

        msgTab->removeMessage(MSG_NOT_CONNECTED_TO_ATHENA);
        msgTab->removeMessage(MSG_NOT_CONFIRMED_AGENT);

        msgTab->addNewMessage(MSG_NOT_CONNECTED_TO_AGENT);
    }
    else // SOCKET_CONNECTED+
    {
        st_status = "yellow";
        ui->connection->setText(tr("ПОДКЛЮЧЁН"));

        ui->settingsPluginsInfo->show();

        msgTab->removeMessage(MSG_NOT_CONNECTED_TO_AGENT);

        // если есть состояние из Web'а
        if(newState > SOCKET_CONNECTED)
        {
            msgTab->removeMessage(MSG_NOT_CONNECTED_TO_ATHENA);
            msgTab->removeMessage(MSG_NOT_CONFIRMED_AGENT);

            switch(newState) {
            case WEB_NOT_CONNECTED:
                st_status = "gray";
                ui->connection->setText(tr("НЕТ СОЕДИНЕНИЯ"));

                msgTab->addNewMessage(MSG_NOT_CONNECTED_TO_ATHENA);
                break;
            case WEB_NOT_CONFIRMED:
                st_status = "yellow";
                ui->connection->setText(tr("НЕ ПОДТВЕРЖДЁН"));

                msgTab->addNewMessage(MSG_NOT_CONFIRMED_AGENT);
                break;
            case WEB_CONFIRMED:
                st_status = "green";
                ui->connection->setText(tr("АКТИВНО"));
                break;
            default:
                break;
            }
        }
    }

    // Разрешаем сканирование в единственном случае - Агент подтверждён
    if( connectionState == WEB_CONFIRMED && !(ui->pbScanSystem->isEnabled()) )
        ui->pbScanSystem->setEnabled(true);
    else
        ui->pbScanSystem->setEnabled(false);

    ui->connection->setProperty("status", st_status);
    ui->connection->style()->unpolish(ui->connection);
    ui->connection->style()->polish(ui->connection);
}

void MainForm::bellClicked(bool isOn)
{
    // открыть вкладку
    if(isOn) {
        msgTab->show();
        QPoint globalXY = mapToGlobal(wdgBell->pos());
        /// NOTE: +1 - потому что мы ещё рисуем outset границу шириной 1px
        ///         чтобы визуально всё совпадало c BellIndicator
        ///         (msgTab > ui > scrollArea > styleSheet)
        msgTab->move(globalXY.x() + wdgBell->width() + 1 - msgTab->width(),
                     globalXY.y() + wdgBell->height());
    }
    else {
        msgTab->hide();
    }
}

void MainForm::slotEnableScanBtn(bool en)
{
    ui->pbScanSystem->setEnabled(en);
}
