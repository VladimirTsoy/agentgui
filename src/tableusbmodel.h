#ifndef TABLEUSBMODEL_H
#define TABLEUSBMODEL_H

#include <QAbstractTableModel>

#include "Deps/device.h"

class TableUSBModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TableUSBModel(QObject* parent = nullptr);
    ~TableUSBModel();

    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    int columnCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addDevice(Device usb);

private:
    QList<Device>* devices;
    QMap<int, int> dev_row;
    QMap<std::string, std::string> stateEnum
    {
        {"Allowed", "Разрешено"},
        {"Blocked" , "Заблокировано"}
    };
    int currentID;
};

#endif // TABLEUSBMODEL_H
