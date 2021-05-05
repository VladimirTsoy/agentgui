#include "filterfilesmodel.h"

FilterFilesModel::FilterFilesModel(QObject* parent): QSortFilterProxyModel(parent)
{
}

bool FilterFilesModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    return 0;
}

bool FilterFilesModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QVariant leftData = sourceModel()->data(source_left);
    QVariant rightData = sourceModel()->data(source_right);

    return 0;
}
