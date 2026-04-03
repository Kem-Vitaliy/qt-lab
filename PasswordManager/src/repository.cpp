#include "repository.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

Repository::Repository() {
}

QList<Record> Repository::getAllRecords() {
    QList<Record> records;
    QSqlQuery query("SELECT id, title, username, password, website, category, updated_at FROM records", 
                    DatabaseManager::instance().database());

    while (query.next()) {
        Record record;
        record.id = query.value(0).toInt();
        record.title = query.value(1).toString();
        record.username = query.value(2).toString();
        record.password = query.value(3).toString();
        record.website = query.value(4).toString();
        record.category = query.value(5).toString();
        record.updatedAt = query.value(6).toString();
        records.append(record);
    }
    return records;
}

bool Repository::addRecord(Record& record) {
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("INSERT INTO records (title, username, password, website, category, updated_at) "
                  "VALUES (:title, :username, :password, :website, :category, :updated_at)");
    
    query.bindValue(":title", record.title);
    query.bindValue(":username", record.username);
    query.bindValue(":password", record.password);
    query.bindValue(":website", record.website);
    query.bindValue(":category", record.category);
    query.bindValue(":updated_at", record.updatedAt);

    if (query.exec()) {
        record.id = query.lastInsertId().toInt();
        return true;
    } else {
        qWarning() << "Failed to add record:" << query.lastError().text();
        return false;
    }
}

bool Repository::updateRecord(const Record& record) {
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE records SET title = :title, username = :username, password = :password, "
                  "website = :website, category = :category, updated_at = :updated_at WHERE id = :id");
    
    query.bindValue(":id", record.id);
    query.bindValue(":title", record.title);
    query.bindValue(":username", record.username);
    query.bindValue(":password", record.password);
    query.bindValue(":website", record.website);
    query.bindValue(":category", record.category);
    query.bindValue(":updated_at", record.updatedAt);

    if (query.exec()) {
        return true;
    } else {
        qWarning() << "Failed to update record:" << query.lastError().text();
        return false;
    }
}

bool Repository::deleteRecord(int id) {
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM records WHERE id = :id");
    query.bindValue(":id", id);
    
    if (query.exec()) {
        return true;
    } else {
        qWarning() << "Failed to delete record:" << query.lastError().text();
        return false;
    }
}
