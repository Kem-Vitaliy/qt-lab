#ifndef TICKETTABLEMODEL_H
#define TICKETTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "ticket.h"

class TicketTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TicketTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void addTicket(const Ticket &ticket);
    void updateTicket(int row, const Ticket &ticket);
    void removeTicket(int row);
    Ticket getTicket(int row) const;
    QVector<Ticket> getTickets() const;
    void setTickets(const QVector<Ticket> &tickets);

private:
    QVector<Ticket> m_tickets;
};

#endif // TICKETTABLEMODEL_H
