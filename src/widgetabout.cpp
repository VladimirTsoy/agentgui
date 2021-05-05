#include "widgetabout.h"
#include "ui_widgetabout.h"

#include <QDesktopServices>
#include <QUrl>
#include <QDir>

WidgetAbout::WidgetAbout(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetAbout)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("Справка","code"));
    this->setWindowIcon(QIcon(":/resources/trayIcon.png"));

    connect(ui->agreeButton, &QPushButton::clicked, this, &WidgetAbout::openAgreements);
    connect(ui->aboutButton, &QPushButton::clicked, this, &WidgetAbout::openAbout);

    connect(ui->btnClose, &QPushButton::clicked, this, &WidgetAbout::close);
}

WidgetAbout::~WidgetAbout()
{
    delete ui;
}

void WidgetAbout::openAbout()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath()+"/ATHENA_AGENT_ABOUT.docx"));
}

void WidgetAbout::openAgreements()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath()+"/ATHENA_AGENT_EULA.docx"));
}
