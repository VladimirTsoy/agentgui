#include "tableviewdelegate.h"
#include "settings.h"
#include <QDebug>


TableViewDelegate::TableViewDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void TableViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem op(option);
    bool isSelected = op.state & QStyle::State_Selected;

//    if(!Settings::getSettings()->getDisableColorVerdict())
    QString verd;
    QBrush brush;

    switch (mode) {
        case Mode::files: {
            verd = index.model()->data(index.model()->index(index.row(), 4), Qt::DisplayRole).toString();
            break;
        }
        case Mode::devices: {
            verd = index.model()->data(index.model()->index(index.row(), 5), Qt::DisplayRole).toString();
            break;
        }
    }

    QColor colorGray;
    QColor colorYellow;
    QColor colorGreen;
    QColor colorRed;
    QColor colorSelected;
    /// NOTE: в идеале  - нужно одно место где будут задаваться все цвета
    /// даже если не подвязывать на единый .qss - всё равно будет легче вносить изменения

//    if(Settings::getSettings()->getEnableLightTheme()) { // светлая тема
    {
        colorGray.setRgb(199, 199, 199);
        colorYellow.setRgb(239, 242, 157);
        colorGreen.setRgb(237, 255, 207);
        colorRed.setRgb(245, 223, 223);

        colorSelected.setRgb(255, 150, 110);
    }
/*
    else { // тёмная тема
        colorGray.setRgb(102, 102, 102);
        colorYellow.setRgb(180, 180, 10);
        colorGreen.setRgb(110, 140, 10);
        colorRed.setRgb(200, 80, 80);

        colorSelected.setRgb(230, 120, 75);
    }
*/
    if(isSelected) {
        brush.setColor(colorSelected);
        op.font.setBold(true);
    }
    else {
        if (verd == "Не определено") {
            brush.setColor(colorGray);
        }
        else if (verd == "Безопасный" || verd == "Разрешено") {
            brush.setColor(colorGreen);
        }
        else if ( verd == "Вредоносный"|| verd == "Заблокировано") {
            brush.setColor(colorRed);
        }
        else if (verd == "Подозрительный") {
            brush.setColor(colorYellow);
        }
        else {
            brush.setColor(colorGray);
        }
    }

    painter->fillRect(op.rect, brush.color());
    QStyledItemDelegate::paint(painter, op, index);
}

void TableViewDelegate::setMode(Mode mode)
{
    this->mode = mode;
}
