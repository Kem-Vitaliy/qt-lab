#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "record.h"
#include <QList>
#include <QString>

class Repository {
public:
    Repository();
    
    QList<Record> getAllRecords();
    bool addRecord(Record& record);
    bool updateRecord(const Record& record);
    bool deleteRecord(int id);
};

#endif // REPOSITORY_H