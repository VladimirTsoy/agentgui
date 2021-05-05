#ifndef TABLETOOLTIP_H
#define TABLETOOLTIP_H


#include <QObject>
#include <QAbstractItemView>
#include <QHelpEvent>
#include <QToolTip>

/*! Класс обработчик событий для вывода ToolTip кастомной модели таблицы.
 *
 */
class TableToolTip : public QObject
{
    Q_OBJECT

public:
    explicit TableToolTip(QObject* parent = nullptr);

protected:
    bool eventFilter(QObject* obj, QEvent* event);
};


#endif // TABLETOOLTIP_H
