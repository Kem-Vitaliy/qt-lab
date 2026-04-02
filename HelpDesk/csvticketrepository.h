#ifndef CSVTICKETREPOSITORY_H
#define CSVTICKETREPOSITORY_H

#include <QString>
#include <QVector>
#include "ticket.h"

class CsvTicketRepository
{
public:
    explicit CsvTicketRepository(const QString &filePath);

    QVector<Ticket> load();
    bool save(const QVector<Ticket> &tickets);

private:
    QString m_filePath;
    QString escapeCsvField(const QString &field);
    QString unescapeCsvField(const QString &field);
};

#endif // CSVTICKETREPOSITORY_H
