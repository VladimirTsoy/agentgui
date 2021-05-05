#ifndef BELLINDICATOR_H
#define BELLINDICATOR_H

#include <QFrame>

namespace Ui {
    class BellIndicator;
}

class BellIndicator : public QFrame
{
    Q_OBJECT

public:
    explicit BellIndicator(QWidget *parent = nullptr);
    ~BellIndicator();

    void setTheme(const QString& ligthTheme);

public slots:
    void click();
    void setMsgStatus(short status);

signals:
    void signalClicked(bool isOn);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::BellIndicator *ui;

    void setOpenTab(bool openTab);
    void updateMsgStatus();

    bool isClicked;
    short msgStatus; // если сообщений нет, то все клики игнорируем
// msg_no      = 0,// нет сообщений
// msg_to_read = 1,// есть непрочитанные сообщения
// msg_read    = 2 // всё прочитали

};

#endif // BELLINDICATOR_H
