#ifndef POPUPMESSAGETAB_H
#define POPUPMESSAGETAB_H

#include <QFrame>
#include <QVBoxLayout>

#include "tabmessage.h"

namespace Ui {
class PopupMessageTab;
}

/*!
 * \brief The PopupMessageTab class - впслывающее окно для вывод сообщений
 *
 * По нажатию на иконку уведомлений ("колокольчик") открывается вкладка со стэком
 * сообщений.
 *
 * Открыть (show)
 * Закрыть (hide)
 *
 * У сообщений есть индикатор - кружок.
 * Если соощение не прочитано - он рыжий.
 * Если прочитано - серый.
 *
 * Общий индикатор - на "колокольчике", всё прочитано - серый, не всё - рыжий.
 * При открытии этой вклалки - общий индикатор временно скрываем.
 *
 * "Прочитать" сообщение - значит кликнуть по нему.
 *      отдельный класс для "сообщений":
 *      - индикатор (прочитано/не прочитано)
 *      - заголовок
 *      - текст (тело)
 *
 * Пользователь никак не влияет появление(добавления в список), удаление сообщений.
 *
 * Скролл (делать всё изначально - ScrollArea)
 * + его стили
 *  на данный момент - не так много сообщений - скрола нет
 *
 */
class PopupMessageTab : public QFrame
{
    Q_OBJECT

public:
    explicit PopupMessageTab(QWidget *parent = nullptr);
    ~PopupMessageTab();

    void setTheme(const QString& lightTheme);

    void addNewMessage(MessageType type);
    void removeMessage(const MessageType type);
/// TODO: удалять сообщение по имени (сейчас так)
///     либо... сделать перечень сообщений с идентификатором
///         - готовый перечень
///         - идентификаторы
///         - описание сообщений - будет храниться в GUI

protected:
    void hideEvent(QHideEvent *event);

signals:
    void signalAllMsgsRead(short readAll);
    void signalHidden();
public slots:
    void retranslate();

private slots:
    void updateAllMsgReadStatus();

private:
    Ui::PopupMessageTab *ui;

    QVBoxLayout* layout;

    QList<TabMessage*> msgList;

    QString lightTheme;
    void updateTheme();

    void removeSepLine(int msgIndex);

    int msgHeight;
    void updateTabHeight();
};

#endif // POPUPMESSAGETAB_H
