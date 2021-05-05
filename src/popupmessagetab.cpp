#include "popupmessagetab.h"
#include "ui_popupmessagetab.h"

#include <QHideEvent>
#include <QCommonStyle>

#include <QDebug>

#define SEPARATOR_HEIGHT 1
PopupMessageTab::PopupMessageTab(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PopupMessageTab),
    msgHeight(0)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup | Qt::Window | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0); /// NOTE: ??
    ui->scrollAreaWidgetContents->setLayout(layout);
}

PopupMessageTab::~PopupMessageTab()
{
    delete ui;
}

// syle="light"
void PopupMessageTab::setTheme(const QString& lightTheme) {
    this->lightTheme = lightTheme;
    updateTheme();
}
void PopupMessageTab::updateTheme(){

    ui->scrollArea->setProperty("style", lightTheme);
    ui->scrollArea->style()->unpolish(ui->scrollArea);
    ui->scrollArea->style()->polish(ui->scrollArea);

    foreach(TabMessage* guiMsg, msgList) {
        guiMsg->setTheme(lightTheme);
    }

    // NOTE:  самый простой вариант установки стиля разделительной линии
    if(lightTheme == "light")
        setStyleSheet("QFrame {color:rgb(255, 255, 255);}");
    else
        setStyleSheet("QFrame{color: rgb(50, 50, 52);}");

}

void PopupMessageTab::hideEvent(QHideEvent *event)
{
    emit signalHidden();
}

void PopupMessageTab::addNewMessage(MessageType type)
{
    foreach(TabMessage* guiMsg, msgList) {
        if(guiMsg->type() == type) {
            return; // не дублируем сообщения
        }
    }

    // После первого элемента надо добавить разделитель перед каждым последующим
    if(!msgList.isEmpty()) {
        QFrame *separatorLine = new QFrame(this);
        separatorLine->setFrameShape(QFrame::HLine); // Horizontal line
        separatorLine->setFrameShadow(QFrame::Plain);
        separatorLine->setLineWidth(SEPARATOR_HEIGHT);
        separatorLine->setMaximumHeight(SEPARATOR_HEIGHT);
        layout->addWidget(separatorLine);
    }

    TabMessage* newMsg = new TabMessage(type, this);
    layout->addWidget(newMsg);
    msgList.append(newMsg);

    if(!msgHeight) // вот таким кривым способом ...
        msgHeight = newMsg->height();

    connect(newMsg, &TabMessage::signalClicked, this, &PopupMessageTab::updateAllMsgReadStatus);

    updateTabHeight();
    updateAllMsgReadStatus();

    updateTheme();
}

void PopupMessageTab::removeMessage(const MessageType type)
{
    // удалять с формы
    // удалять и разделитель
    // удалять из списка
    foreach(TabMessage* guiMsg, msgList)
    {
        if(guiMsg->type() == type)
        {
            int index = msgList.indexOf(guiMsg);
            if((index > 0)
                || (index == 0 && msgList.size() > 1))
                removeSepLine(index);

            msgList.removeOne(guiMsg);
            delete guiMsg;
        }
    }

    if(msgList.isEmpty())
    {
        emit signalAllMsgsRead(0);
        if(isVisible()) {
            hide();
        }
    }
    else
    {
        updateTabHeight();
    }

    updateAllMsgReadStatus();
}

void PopupMessageTab::removeSepLine(int msgIndex)
{
    // NOTE: если сообщений больше 1, и удаляем первое - то разделитель тоже удалять

    QLayoutItem *item;
    if(msgIndex == 0)
        item = layout->takeAt(1);
    else // msgIndex > 0
        item = layout->takeAt(msgIndex*2-1);

    delete item;
}

void PopupMessageTab::updateTabHeight()
{
    int newHeight = msgHeight*msgList.size()        // сообщения
            + SEPARATOR_HEIGHT*(msgList.size()-1)  // разделители
            + 2;              // граница самого виджета

    setMinimumHeight(newHeight);
    setMaximumHeight(newHeight);
    resize(width(), newHeight);
}

void PopupMessageTab::updateAllMsgReadStatus()
{
    bool unreadMsgs = false;
    foreach(TabMessage* guiMsg, msgList) {
        if(!guiMsg->isRead()) {
            unreadMsgs = true;
            break;
        }
    }
/*
    msg_no = 0,     // нет сообщений
    msg_to_read = 1,// есть непрочитанные сообщения
    msg_read = 2    // всё прочитали
*/
    short msgsSatus;
    if(msgList.isEmpty())
        msgsSatus = 0;
    else if(unreadMsgs)
        msgsSatus = 1;
    else
        msgsSatus = 2;

    emit signalAllMsgsRead(msgsSatus);
}

void PopupMessageTab::retranslate()
{
    for(auto msg : msgList)
        msg->retranslate();
}
