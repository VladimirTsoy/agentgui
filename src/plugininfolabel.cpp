#include "plugininfolabel.h"
#include "ui_plugininfolabel.h"

PluginInfoLabel::PluginInfoLabel(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PluginInfoLabel)
{
    ui->setupUi(this);
}

PluginInfoLabel::~PluginInfoLabel()
{
    delete ui;
}

void PluginInfoLabel::setText(QString text)
{
    ui->lb_textInfo->setText(text);
}

/* status:
    0  - is in list,
    1  - init,
    2  - is running (started successfully)
    -1 - deleted (init() failed)
    NOTE: сейчас есть только 1 и 2, т.к.списка нет (соотв.и -1 отпадает)
*/
void PluginInfoLabel::setState(short state)
{
    switch(state) // short
    {
    case 2: // green
        ui->lb_led->setStyleSheet( "\
        #lb_led {\
            background-color: qradialgradient(\
                spread:pad, cx:0.5, cy:0.5,\
                radius:0.5, fx:0.5, fy:0.5,\
                stop:0.489796 rgba(138, 226, 52, 255),\
                stop:1 rgba(78, 154, 6, 255));\
            border-radius: 8px;\
        }");
        break;
    default: // red
        ui->lb_led->setStyleSheet( "\
        #lb_led {\
            background-color: qradialgradient(\
                spread:pad, cx:0.5, cy:0.5,\
                radius:0.5, fx:0.5, fy:0.5,\
                stop:0.489796 rgba(255, 0, 0, 255),\
                stop:1 rgba(164, 0, 0, 255));\
            border-radius: 8px;\
        }");
        break;
    }
}
