#ifndef TICKET_H
#define TICKET_H

#include <QString>
#include <QDateTime>

struct Ticket {
    int id;
    QString title;
    QString priority;
    QString status;
    QDateTime createdAt;
    QString description;
};

#endif // TICKET_H
