#include "bellindicator.h"
#include "ui_bellindicator.h"

#include <QCommonStyle>
#include <QMouseEvent>

#include <QDebug>

/**
 * Тааааааааак...
 * есть три вида иконки
 *  - чёрный
 *  - серый
 *  - белый
 * может быть кружок
 * -- серый
 *      - серый для light
 *      - серый для dark
 * -- рыжий
 *      - рыжий для light
 *      - рыжий для dark
 * + выделение (когда открыли вкладку)
 *  - без всего (прозрачный фон)
 *  - скруглённый квадрат (шапка вкладки)
 *      - серый для light
 *      - серый для dark
 *
 * ИТОГ - делать отдельный виджет
 *  - в нём по заданию последовательно параметров, либо
 *    один метод с параметрами - задание внешнего вида виджета
 *  + обработка нажатия клавиши (перегрузить)
 *
**/

BellIndicator::BellIndicator(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::BellIndicator),
    isClicked(false),
    msgStatus(0) // msg_no
{
    ui->setupUi(this);

    updateMsgStatus();
}

BellIndicator::~BellIndicator()
{
    delete ui;
}

/**
 Обрабатываем клик по этому объекту
    и высылаем сигнал - по нему открываем вкладку.

 Закрываем вкладку:
 - по повторному клику на этот объект
 - по клику вне вкдадки (...)
    установить для виджета с сообщениями > setWindowFlags(Qt::Popup);

* есть ещё > QEvent::NonClientAreaMouseButtonPress
    A mouse button press occurred outside the client area (QMouseEvent).
 **/
void BellIndicator::mousePressEvent(QMouseEvent *event) {
    if(!msgStatus)
        return;

    if (event->button() == Qt::LeftButton) {
        isClicked = !isClicked;
        emit signalClicked(isClicked);
    }
    setOpenTab(isClicked);
}

void BellIndicator::click() {
    if(!msgStatus)
        return;

    isClicked = !isClicked;
    setOpenTab(isClicked);
}

// syle="light"
void BellIndicator::setTheme(const QString& ligthTheme) {
    setProperty("style", ligthTheme);
    style()->unpolish(this);
    style()->polish(this);

    ui->fr_bell->setProperty("style", ligthTheme);
    ui->fr_bell->style()->unpolish(ui->fr_bell);
    ui->fr_bell->style()->polish(ui->fr_bell);

    ui->fr_circle->setProperty("style", ligthTheme);
    ui->fr_circle->style()->unpolish(ui->fr_circle);
    ui->fr_circle->style()->polish(ui->fr_circle);
}

// tab="showtab"
void BellIndicator::setOpenTab(bool openTab) {
    QString strTab;
    if(openTab)
        strTab = "showtab";
    else
        strTab = "";

    setProperty("tab", strTab);
    style()->unpolish(this);
    style()->polish(this);

    updateMsgStatus();
}

void BellIndicator::setMsgStatus(short status)
{
    if(msgStatus != status) {
        msgStatus = status;
        if(msgStatus == 0)  {
            isClicked = false;
            setOpenTab(false); // там же updateMsgStatus()
        }
        else if(!isClicked)
            updateMsgStatus();
    }
}

// read="done"
void BellIndicator::updateMsgStatus() {
    QString strMsgStatus;

    // Когда мы нажимаем на кнопку - то индикатор убираем
    // либо просто нет сообщений (mgsStatus=msg_no==0)
    if(isClicked || !msgStatus) {
        ui->fr_circle->setProperty("msgToRead", "no");
        ui->fr_circle->style()->unpolish(ui->fr_circle);
        ui->fr_circle->style()->polish(ui->fr_circle);
        return;
    }
    /// NOTE: в styleSheet в итоге отрабатывается последннее совпавшее условие
    ///         поэтому [msgToRead="no"] там указан в конце

    if(msgStatus == 2)
        strMsgStatus = "done";
    else // == msg_to_read(1)
        strMsgStatus = "";

    ui->fr_circle->setProperty("msgToRead", strMsgStatus);
    ui->fr_circle->style()->unpolish(ui->fr_circle);
    ui->fr_circle->style()->polish(ui->fr_circle);
}
