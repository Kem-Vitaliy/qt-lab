#ifndef PASSWORDPROXYMODEL_H
#define PASSWORDPROXYMODEL_H

#include <QSortFilterProxyModel>

class PasswordProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit PasswordProxyModel(QObject *parent = nullptr);

    void setSearchString(const QString &search);
    void setCategoryFilter(const QString &category);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString m_searchString;
    QString m_categoryFilter;
};

#endif // PASSWORDPROXYMODEL_H