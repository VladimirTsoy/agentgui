#include "tablefilesmodel.h"
#include <QDebug>
#include <QDate>
#include <QDateTime>
#include <iostream>
#include <QColor>
#include <QDir>


TableFilesModel::TableFilesModel(QObject *parent) : QAbstractTableModel(parent)
{
    files = new QList<File>();
}

TableFilesModel::~TableFilesModel()
{
    delete files;
}

bool TableFilesModel::insertRows(int position, int rows, const QModelIndex &/*index*/)
{
    beginInsertRows(QModelIndex(), position, position+rows-1);
    for (int row = 0; row < rows; row++)
    {
        files->push_front(File());
    }
    endInsertRows();

    return true;
}

int TableFilesModel::rowCount(const QModelIndex &) const
{
    return files->size();
}

int TableFilesModel::columnCount(const QModelIndex &) const
{
    return 6;
}

QVariant TableFilesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return  QVariant();

    if(orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
//            return QString(tr("Дата и время"));
            return QString(tr(""));
        case 1:
//            return QString(tr("Имя файла"));
            return QString(tr(""));
        case 2:
//            return QString(tr("Путь к файлу"));
            return QString(tr(""));
        case 3:
//            return QString(tr("Статус"));
            return QString(tr(" "));
        case 4:
//            return QString(tr("Вердикт"));
            return QString(tr(" "));
        case 5:
//            return QString(tr("Контрольная сумма"));
            return QString(tr(""));

        default: Q_ASSERT(false);
        }
    }
    return section + 1;
}

QVariant TableFilesModel::data(const QModelIndex &index, int role) const
{
    switch(role)
    {
    case Qt::DisplayRole:
    {
        if (!index.isValid() || index.row() < 0 || index.row() > files->count() || index.column() < 0 || index.column() > 5)
        {
            return QVariant();
        }

        switch (index.column())
        {
        case 0:
        {
            return QVariant(QString::fromStdString((*files)[index.row()].time));
        }
        case 1:
        {
            return QVariant(QString::fromStdString((*files)[index.row()].name));
        }
        case 2:
        {
//            return QVariant(QString::fromStdString((*files)[index.row()].path));
            return QVariant(QDir::toNativeSeparators(QString::fromStdString((*files)[index.row()].path)));
        }
        case 3:
        {
            int st = (*files)[index.row()].status;
            std::string a = stateEnum[st];
            return QVariant(QString::fromStdString(a));
        }
        case 4:
        {
            return QVariant(QString::fromStdString((*files)[index.row()].verdict));
        }
        case 5:
        {
            return QVariant(QString::fromStdString((*files)[index.row()].hash));
        }
        case 6:
        {
            return QVariant(QString("test"));
        }
        default: return  QVariant();
        }
    }
// чтобы это работало - надо определиться
// работать через data и Role либо через StyleSheet
//    case Qt::TextColorRole:
//        return QColor(Qt::red);
    default:
    {
        return  QVariant();
    }
    }
}


int TableFilesModel::getFileRow(int id)
{
    return file_row[id];
}

void TableFilesModel::setStatus(File file)
{
    bool srch = false;
    for(int i = 0; i < files->size(); i++)
    {
        if(files->at(i).hash == file.hash)
        {
            srch = true;
            (*files)[i].webid = file.webid;
            if(file.webid == -1)
            {
                (*files)[i].status = -1;
                (*files)[i].verdict = verdictEnum["Undefined"];
            }
            else
            {
                (*files)[i].status = file.status;
                (*files)[i].verdict = verdictEnum[file.verdict];
            }
//            return;
            // на данный момент одинаковые позиции при работе не схлопываются
            // поэтому изменить статус (и расскрасить надо все одинаковые строки)
        }
    }

    if(!srch)
        addFile(file);
}

QList<File>* TableFilesModel::getFiles()
{
    return files;
}

void TableFilesModel::addFile(File file)
{
    insertRow(0);

    (*files).first().id = file.id;
    (*files).first().webid = file.webid;
    (*files).first().name = file.name;
    (*files).first().path = file.path;
    (*files).first().status = file.status;
    (*files).first().verdict = verdictEnum[file.verdict];

    (*files).first().time = QDateTime::fromString(QString::fromStdString(file.time), Qt::DateFormat::ISODateWithMs).toString(Qt::DateFormat::SystemLocaleDate).toStdString();
    (*files).first().hash = file.hash;
}

void TableFilesModel::deleteFile(File file)
{
    for(int i = 0; i < files->size(); i++)
    {
        if(files->at(i).hash == file.hash)
        {
            removeRow(i);
        }
    }
}

bool TableFilesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), row, row+count-1);
    files->removeAt(row);
    endRemoveRows();
    return true;
}

