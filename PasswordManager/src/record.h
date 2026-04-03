#ifndef RECORD_H
#define RECORD_H

#include <QString>
#include <QDateTime>

struct Record {
    int id = -1;
    QString title;
    QString username;
    QString password;
    QString website;
    QString category;
    QString updatedAt;
};

#endif // RECORD_H