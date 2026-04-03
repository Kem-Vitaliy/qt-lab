#include "passwordmodel.h"
#include <QDateTime>
#include <QDebug>

PasswordModel::PasswordModel(Repository* repository, QObject *parent)
    : QAbstractTableModel(parent), m_repository(repository)
{
}

int PasswordModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_records.count();
}

int PasswordModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    // Columns: ID(0), Title(1), Username(2), Password(3), Website(4), Category(5), UpdatedAt(6)
    return 7;
}

QVariant PasswordModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_records.count() || index.row() < 0)
        return QVariant();

    const Record& record = m_records.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
            case 0: return record.id;
            case 1: return record.title;
            case 2: return record.username;
            case 3: return record.password;
            case 4: return record.website;
            case 5: return record.category;
            case 6: return record.updatedAt;
            default: return QVariant();
        }
    }
    return QVariant();
}

QVariant PasswordModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return "ID";
            case 1: return "Title";
            case 2: return "Username";
            case 3: return "Password";
            case 4: return "Website";
            case 5: return "Category";
            case 6: return "UpdatedAt";
            default: return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags PasswordModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    // Allow editing for all columns except ID (0) and UpdatedAt (6)
    if (index.column() == 0 || index.column() == 6) {
        return QAbstractTableModel::flags(index);
    }
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool PasswordModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        Record record = m_records[index.row()];
        bool changed = false;

        switch (index.column()) {
            case 1:
                if (record.title != value.toString()) { record.title = value.toString(); changed = true; }
                break;
            case 2:
                if (record.username != value.toString()) { record.username = value.toString(); changed = true; }
                break;
            case 3:
                if (record.password != value.toString()) { record.password = value.toString(); changed = true; }
                break;
            case 4:
                if (record.website != value.toString()) { record.website = value.toString(); changed = true; }
                break;
            case 5:
                if (record.category != value.toString()) { record.category = value.toString(); changed = true; }
                break;
            default:
                return false;
        }

        if (changed) {
            record.updatedAt = QDateTime::currentDateTime().toString(Qt::ISODate);
            
            // Save to database
            if (m_repository && m_repository->updateRecord(record)) {
                // Update internal list
                m_records[index.row()] = record;
                
                // Notify views
                emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
                
                // Also update the timestamp column visually
                QModelIndex timeIndex = this->index(index.row(), 6);
                emit dataChanged(timeIndex, timeIndex, {Qt::DisplayRole});
                
                return true;
            } else {
                qWarning() << "Failed to save edits to database for ID" << record.id;
            }
        }
    }
    return false;
}

void PasswordModel::setItems(const QList<Record>& records)
{
    beginResetModel();
    m_records = records;
    endResetModel();
}

void PasswordModel::reloadData()
{
    if (m_repository) {
        setItems(m_repository->getAllRecords());
    }
}

Record PasswordModel::getRecord(int row) const
{
    if (row >= 0 && row < m_records.size()) {
        return m_records.at(row);
    }
    return Record();
}

void PasswordModel::addRecord(const Record& record)
{
    Record newRec = record;
    if (m_repository && m_repository->addRecord(newRec)) {
        beginInsertRows(QModelIndex(), m_records.count(), m_records.count());
        m_records.append(newRec);
        endInsertRows();
    }
}

void PasswordModel::removeRecord(int row)
{
    if (row >= 0 && row < m_records.size()) {
        if (m_repository && m_repository->deleteRecord(m_records[row].id)) {
            beginRemoveRows(QModelIndex(), row, row);
            m_records.removeAt(row);
            endRemoveRows();
        }
    }
}
