#include "tableusbmodel.h"
#include <QDateTime>

TableUSBModel::TableUSBModel(QObject *parent):QAbstractTableModel(parent)
{
    devices = new QList<Device>();
}

TableUSBModel::~TableUSBModel()
{
    delete devices;
}

bool TableUSBModel::insertRows(int position, int rows, const QModelIndex &/*index*/)
{
    beginInsertRows(QModelIndex(), position, position+rows-1);
    for (int row = 0; row < rows; row++)
    {
        devices->push_back(Device());
    }
    endInsertRows();
    return true;
}

int TableUSBModel::rowCount(const QModelIndex &) const
{
    return devices->size();
}

int TableUSBModel::columnCount(const QModelIndex &) const
{
    return 6;
}

QVariant TableUSBModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return  QVariant();

    if(orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 1:
            return QString(tr("Имя устройства"));
        case 2:
            return QString(tr("VID"));
        case 3:
            return QString(tr("PID"));
        case 4:
            return QString(tr("Серийный номер"));
        case 5:
            return QString(tr("Вердикт"));
        case 0:
            return QString(tr("Дата и время"));
        default: Q_ASSERT(false);
        }
    }
    return section + 1;
}

QVariant TableUSBModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole
            || index.row() < 0
            || index.row() > devices->count()
            || index.column() < 0 || index.column() > 5)
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 1:
        {
            return QVariant(QString::fromStdString((*devices)[index.row()].name));
        }
        case 2:
        {
            return QVariant(QString::fromStdString((*devices)[index.row()].vid));
        }
        case 3:
        {
            return QVariant(QString::fromStdString((*devices)[index.row()].pid));
        }
        case 4:
        {
            return QVariant(QString::fromStdString((*devices)[index.row()].serial));
        }
        case 5:
        {
            return QVariant(QString::fromStdString((*devices)[index.row()].block_state));
        }
        case 0:
        {
            return QDateTime::fromString(QString::fromStdString((*devices)[index.row()].date), Qt::DateFormat::ISODateWithMs).toString(Qt::DateFormat::SystemLocaleDate);
        }
        default: return  QVariant();
        }
    }
    return  QVariant();
}

void TableUSBModel::addDevice(Device device)
{
    for(int i = 0; i < devices->size(); ++i)
    {
        if (
            (*devices)[i].serial == device.serial
            && (*devices)[i].vid == device.vid
            && (*devices)[i].pid == device.pid
            )
        {
            (*devices)[i].block_state = stateEnum[device.block_state];
            (*devices)[i].date = device.date;
            return;
        }
    }

    insertRow(rowCount());

    (*devices).last().name = device.name;
    (*devices).last().pid = device.pid;
    (*devices).last().vid = device.vid;
    (*devices).last().serial = device.serial;
    (*devices).last().block_state = stateEnum[device.block_state];
    (*devices).last().date = device.date;
}
