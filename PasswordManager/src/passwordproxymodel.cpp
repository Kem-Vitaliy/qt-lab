#include "passwordproxymodel.h"

PasswordProxyModel::PasswordProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void PasswordProxyModel::setSearchString(const QString &search)
{
    m_searchString = search;
    invalidateFilter();
}

void PasswordProxyModel::setCategoryFilter(const QString &category)
{
    m_categoryFilter = category;
    invalidateFilter();
}

bool PasswordProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!sourceModel())
        return false;

    // Get indices for relevant columns: Title(1), Username(2), Website(4), Category(5)
    QModelIndex titleIndex = sourceModel()->index(source_row, 1, source_parent);
    QModelIndex usernameIndex = sourceModel()->index(source_row, 2, source_parent);
    QModelIndex websiteIndex = sourceModel()->index(source_row, 4, source_parent);
    QModelIndex categoryIndex = sourceModel()->index(source_row, 5, source_parent);

    QString title = sourceModel()->data(titleIndex).toString();
    QString username = sourceModel()->data(usernameIndex).toString();
    QString website = sourceModel()->data(websiteIndex).toString();
    QString category = sourceModel()->data(categoryIndex).toString();

    // Check category filter
    bool matchesCategory = true;
    if (!m_categoryFilter.isEmpty() && m_categoryFilter != "All Categories") {
        matchesCategory = (category == m_categoryFilter);
    }

    // Check text search filter (matches Title, Username, or Website)
    bool matchesSearch = true;
    if (!m_searchString.isEmpty()) {
        matchesSearch = title.contains(m_searchString, Qt::CaseInsensitive) ||
                        username.contains(m_searchString, Qt::CaseInsensitive) ||
                        website.contains(m_searchString, Qt::CaseInsensitive);
    }

    return matchesCategory && matchesSearch;
}
