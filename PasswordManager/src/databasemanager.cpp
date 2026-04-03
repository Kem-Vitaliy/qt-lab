#include "databasemanager.h"
#include <QDebug>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
}

DatabaseManager::~DatabaseManager() {
    close();
}

bool DatabaseManager::initialize(const QString& dbName) {
    if (m_db.isOpen()) {
        return true;
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbName);

    if (!m_db.open()) {
        qWarning() << "Failed to connect to database:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);
    QString createTable = "CREATE TABLE IF NOT EXISTS records ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "title TEXT, "
                          "username TEXT, "
                          "password TEXT, "
                          "website TEXT, "
                          "category TEXT, "
                          "updated_at DATETIME)";
    
    if (!query.exec(createTable)) {
        qWarning() << "Failed to create table:" << query.lastError().text();
        return false;
    }

    return true;
}

QSqlDatabase DatabaseManager::database() const {
    return m_db;
}

void DatabaseManager::close() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}
