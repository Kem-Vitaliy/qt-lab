#include "tickettablemodel.h"

TicketTableModel::TicketTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int TicketTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_tickets.count();
}

int TicketTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 5; // ID, Title, Priority, Status, CreatedAt
}

QVariant TicketTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_tickets.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        const Ticket &ticket = m_tickets.at(index.row());
        switch (index.column()) {
            case 0: return ticket.id;
            case 1: return ticket.title;
            case 2: return ticket.priority;
            case 3: return ticket.status;
            case 4: return ticket.createdAt.toString("yyyy-MM-dd HH:mm:ss");
        }
    }
    return QVariant();
}

QVariant TicketTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return tr("ID");
            case 1: return tr("Title");
            case 2: return tr("Priority");
            case 3: return tr("Status");
            case 4: return tr("Created At");
        }
    }
    return QVariant();
}

void TicketTableModel::addTicket(const Ticket &ticket)
{
    beginInsertRows(QModelIndex(), m_tickets.count(), m_tickets.count());
    m_tickets.append(ticket);
    endInsertRows();
}

void TicketTableModel::updateTicket(int row, const Ticket &ticket)
{
    if (row < 0 || row >= m_tickets.size())
        return;

    m_tickets[row] = ticket;
    emit dataChanged(index(row, 0), index(row, 4));
}

void TicketTableModel::removeTicket(int row)
{
    if (row < 0 || row >= m_tickets.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    m_tickets.removeAt(row);
    endRemoveRows();
}

Ticket TicketTableModel::getTicket(int row) const
{
    if (row < 0 || row >= m_tickets.size())
        return Ticket();
    return m_tickets.at(row);
}

QVector<Ticket> TicketTableModel::getTickets() const
{
    return m_tickets;
}

void TicketTableModel::setTickets(const QVector<Ticket> &tickets)
{
    beginResetModel();
    m_tickets = tickets;
    endResetModel();
}
