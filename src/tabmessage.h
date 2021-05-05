#ifndef TABMESSAGE_H
#define TABMESSAGE_H

#include <QFrame>

namespace Ui {
class TabMessage;
}

/*!
 * \brief The TabMessage class
 *
 * TODO:
 *  - индикатор (прочитано/не прочитано)
 *  - заголовок
 *  - текст (тело)
 * + должна устанавливаться тема light/dark
 *
 * Установка заголовка
 * Установка текста
 *
 * Обработка клика - "прочитано"
 * Возврат состояния - "прочитано" (для общего индикатора)
 *
 */
enum MessageType{
    MSG_NOT_CONNECTED_TO_AGENT,
    MSG_NOT_CONNECTED_TO_ATHENA,
    MSG_NOT_CONFIRMED_AGENT
};

class TabMessage : public QFrame
{
    Q_OBJECT

public:
    explicit TabMessage(MessageType type, QWidget *parent = nullptr);
    ~TabMessage();

    void setTheme(const QString& ligthTheme);

    bool isRead();

    QString headText() const;
    MessageType type() const;

    static QString headText(MessageType type);
    static QString messageText(MessageType type);

protected:
    bool eventFilter(QObject *o, QEvent*e) override;

signals:
    void signalClicked();
public slots:
    void retranslate();

private:
    Ui::TabMessage *ui;

    const MessageType type_;

    void updateIndicator();

    bool wasRead; // прочитано ли сообщение
};

#endif // TABMESSAGE_H
