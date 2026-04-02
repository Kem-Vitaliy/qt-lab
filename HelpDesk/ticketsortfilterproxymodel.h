#ifndef TICKETSORTFILTERPROXYMODEL_H
#define TICKETSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class TicketSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit TicketSortFilterProxyModel(QObject *parent = nullptr);

    void setFilterStatus(const QString &status);
    void setFilterPriority(const QString &priority);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString m_filterStatus;
    QString m_filterPriority;
};

#endif // TICKETSORTFILTERPROXYMODEL_H
