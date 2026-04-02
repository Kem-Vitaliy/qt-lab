#include "ticketsortfilterproxymodel.h"

TicketSortFilterProxyModel::TicketSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterKeyColumn(1); // Default filter (Title) is in column 1
}

void TicketSortFilterProxyModel::setFilterStatus(const QString &status)
{
    if (m_filterStatus != status) {
        m_filterStatus = status;
        invalidateFilter();
    }
}

void TicketSortFilterProxyModel::setFilterPriority(const QString &priority)
{
    if (m_filterPriority != priority) {
        m_filterPriority = priority;
        invalidateFilter();
    }
}

bool TicketSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    // Check Status (Column 3)
    if (!m_filterStatus.isEmpty() && m_filterStatus != "All") {
        QModelIndex index = sourceModel()->index(source_row, 3, source_parent);
        if (sourceModel()->data(index).toString() != m_filterStatus)
            return false;
    }

    // Check Priority (Column 2)
    if (!m_filterPriority.isEmpty() && m_filterPriority != "All") {
        QModelIndex index = sourceModel()->index(source_row, 2, source_parent);
        if (sourceModel()->data(index).toString() != m_filterPriority)
            return false;
    }

    // Default QSortFilterProxyModel filter (Title search in Column 1)
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}
