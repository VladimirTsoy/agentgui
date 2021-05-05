#ifndef TABLEVIEWDELEGATE_H
#define TABLEVIEWDELEGATE_H

#include <QWidget>
#include <QStyledItemDelegate>
#include <QPainter>

enum Mode
{
    files,
    devices
};

class TableViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TableViewDelegate(QObject* parent = nullptr);
    void  paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setMode(Mode mode);
private:
    Mode mode;
  };

#endif // TABLEVIEWDELEGATE_H
