#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include "Core/file.h"

class TableFilesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TableFilesModel(QObject * parent = nullptr);
    ~TableFilesModel();

    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;

    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    int columnCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int getFileRow(int id);
    void setStatus(File file);

    void clear();

    QList<File>* getFiles();

    void addFile(File file);
    void deleteFile(File file);

    QMap<int, std::string> stateEnum
    {
        {-1, "Ошибка"},
        {0, "Файл в очереди"},
        {1, "Поиск файла в локальной БД"},
        {2, "Файл найден в локальной БД"},
        {3, "Проверка файла по хеш-сумме"},
        {4, "Файл проверен по хеш-сумме"},
        {5, "Файл загружается в систему"},
        {6, "Файл загружен в систему"},
        {7, "Файл проверяется песочницей"},
        {8, "Файл проверен песочницей"},
        {9, "Файл проверяется по ID проверки"},
        {10, "Получен результат проверки по ID"},
        {11, "Файл проверен"}
    };


    QMap<std::string, std::string> verdictEnum
    {
        {"Undefined", "Не определено"},
        {"Benign", "Безопасный"},
        {"Grayware", "Подозрительный"},
        {"Malware", "Вредоносный"},
        {"Test verdict", "Не определено"},
        {"Error" , "Ошибка"}
    };

private:
    QList<File>* files;
    QMap<int, int> file_row;
};

#endif // TABLEMODEL_H
