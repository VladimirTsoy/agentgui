#ifndef FILTERFILESMODEL_H
#define FILTERFILESMODEL_H

#include <QSortFilterProxyModel>

class FilterFilesModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    FilterFilesModel(QObject* parent = 0);


protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};

#endif // FILTERFILESMODEL_H
