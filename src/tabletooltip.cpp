#include "tabletooltip.h"

#include <QDebug>

TableToolTip::TableToolTip(QObject* parent): QObject(parent)
{
}

/*! Обрабочик событий
 *
 * Выводим просто содержимое ячеек.
 * (сам toolTip никто не задавал)
 *
 * Сейчас текст выводится для всех полей таблицы
 *
 * если надо для конкретных столбцов, то надо подтянуть mode(files/USB)
 * и можно конкретные столбцы выделить
 * ЛИБО
 * задавать всё же ToolTipRole в модели(ячейках) и использовать itemTooltip
 *
 */
bool TableToolTip::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::ToolTip)
    {
        QAbstractItemView* view = qobject_cast<QAbstractItemView*>(obj->parent());
        if(!view)
        {
            return false;
        }

        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
        QPoint pos = helpEvent->pos();
        QModelIndex index = view->indexAt(pos);
        if (!index.isValid())
            return false;

        QString itemText = view->model()->data(index).toString();
//        QString itemTooltip = view->model()->data(index, Qt::ToolTipRole).toString();

        QFontMetrics fm(view->font());
//        int itemTextWidth = fm.width(itemText);
        QRect rect = view->visualRect(index);
//        int rectWidth = rect.width();

//        if ((itemTextWidth > rectWidth) && !itemTooltip.isEmpty())
        if(!itemText.isEmpty())
        {
            QToolTip::showText(helpEvent->globalPos(), itemText, view, rect);
        }
        else
        {
            QToolTip::hideText();
        }
        return true;
    }
    return false;
}
