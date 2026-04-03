#ifndef PASSWORDMODEL_H
#define PASSWORDMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "record.h"
#include "repository.h"

class PasswordModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit PasswordModel(Repository* repository, QObject *parent = nullptr);

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // Custom methods
    void setItems(const QList<Record>& records);
    Record getRecord(int row) const;
    void addRecord(const Record& record);
    void removeRecord(int row);
    void reloadData();

private:
    QList<Record> m_records;
    Repository* m_repository;
};

#endif // PASSWORDMODEL_H