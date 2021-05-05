#include "tabmessage.h"
#include "ui_tabmessage.h"

#include <QCommonStyle>
#include <QMouseEvent>

#include <QDebug>

TabMessage::TabMessage(MessageType type, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TabMessage),
    type_(type),
    wasRead(false)
{
    ui->setupUi(this);

    ui->lb_head->setText(headText(type));
    ui->te_message->setText(messageText(type));

    installEventFilter(this);
}

TabMessage::~TabMessage()
{
    delete ui;
}

bool TabMessage::isRead()
{
    return wasRead;
}

QString TabMessage::headText() const
{
    return ui->lb_head->text();
}

MessageType TabMessage::type() const
{
    return type_;
}

QString TabMessage::headText(MessageType type)
{
    switch (type) {
    case MessageType::MSG_NOT_CONNECTED_TO_AGENT:
        return tr("Нет соединения с Агентом!");
    case MessageType::MSG_NOT_CONFIRMED_AGENT:
        return tr("Агент не подтверждён!");
    case MessageType::MSG_NOT_CONNECTED_TO_ATHENA:
        return tr("Нет соединения с ATHENA!");
    default:
        return "Undefined";
    }
}

QString TabMessage::messageText(MessageType type)
{
    switch (type) {
    case MessageType::MSG_NOT_CONNECTED_TO_AGENT:
        return tr("Возможно Агент не запущен на вашем компьютере. Обратитесь к администратору.");
    case MessageType::MSG_NOT_CONFIRMED_AGENT:
        return tr("Ваша версия агента не подтверждена, обратитесь к администратору");
    case MessageType::MSG_NOT_CONNECTED_TO_ATHENA:
        return tr("Нет соединения с системой ATHENA, обратитесь к администратору");
    default:
        return "Undefined";
    }
}

// Чтобы клик отрабатывал по всему виджету - забили фокус и взаимодействие с текстом
//    для всех внутренних виджетов
bool TabMessage::eventFilter(QObject *o, QEvent*e) {
    if(e->type() == QEvent::MouseButtonPress) {
        wasRead = true;
        updateIndicator();
        emit signalClicked(); // высылать для Tab
    }
}

void TabMessage::retranslate()
{
    ui->lb_head->setText(headText(type_));
    ui->te_message->setText(messageText(type_));
}

// style="light"
void TabMessage::setTheme(const QString& ligthTheme) {
    ui->fr_led->setProperty("style", ligthTheme);
    ui->fr_led->style()->unpolish(ui->fr_led);
    ui->fr_led->style()->polish(ui->fr_led);

    ui->lb_head->setProperty("style", ligthTheme);
    ui->lb_head->style()->unpolish(ui->lb_head);
    ui->lb_head->style()->polish(ui->lb_head);

    ui->te_message->setProperty("style", ligthTheme);
    ui->te_message->style()->unpolish(ui->te_message);
    ui->te_message->style()->polish(ui->te_message);
}

// msgToRead="done"
void TabMessage::updateIndicator() {
    QString strStyleRead;
    if(wasRead)
        strStyleRead = "done";
    else
        strStyleRead = "";

    ui->fr_led->setProperty("msgToRead", strStyleRead);
    ui->fr_led->style()->unpolish(ui->fr_led);
    ui->fr_led->style()->polish(ui->fr_led);
}
