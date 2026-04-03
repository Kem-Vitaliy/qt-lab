#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupModel();
    
    if (initDatabase()) {
        loadRecords();
    }

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onActionNewTriggered);
    connect(ui->actionEdit, &QAction::triggered, this, &MainWindow::onActionEditTriggered);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::onActionDeleteTriggered);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onActionSaveTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onActionExitTriggered);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(model, &QStandardItemModel::itemChanged, this, &MainWindow::onItemChanged);
}

MainWindow::~MainWindow()
{
    if (db.isOpen()) {
        db.close();
    }
    delete ui;
}

void MainWindow::setupModel()
{
    model = new QStandardItemModel(0, 7, this);
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Title");
    model->setHeaderData(2, Qt::Horizontal, "Username");
    model->setHeaderData(3, Qt::Horizontal, "Password");
    model->setHeaderData(4, Qt::Horizontal, "Website");
    model->setHeaderData(5, Qt::Horizontal, "Category");
    model->setHeaderData(6, Qt::Horizontal, "UpdatedAt");

    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
}

bool MainWindow::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("passwords.db");

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", "Unable to open database:\n" + db.lastError().text());
        return false;
    }

    QSqlQuery query;
    QString createTable = "CREATE TABLE IF NOT EXISTS records ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "title TEXT, "
                          "username TEXT, "
                          "password TEXT, "
                          "website TEXT, "
                          "category TEXT, "
                          "updated_at DATETIME)";
    
    if (!query.exec(createTable)) {
        QMessageBox::critical(this, "Database Error", "Unable to create table:\n" + query.lastError().text());
        return false;
    }

    return true;
}

void MainWindow::loadRecords()
{
    disconnect(model, &QStandardItemModel::itemChanged, this, &MainWindow::onItemChanged);
    model->removeRows(0, model->rowCount());

    QSqlQuery query("SELECT id, title, username, password, website, category, updated_at FROM records", db);
    while (query.next()) {
        int row = model->rowCount();
        model->insertRow(row);
        for (int i = 0; i < 7; ++i) {
            model->setData(model->index(row, i), query.value(i));
        }
    }
    connect(model, &QStandardItemModel::itemChanged, this, &MainWindow::onItemChanged);
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::onActionNewTriggered()
{
    if (!db.isOpen()) {
        QMessageBox::critical(this, "Error", "Database is not open!");
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO records (title, username, password, website, category, updated_at) "
                  "VALUES (:title, :username, :password, :website, :category, :updated_at)");
    query.bindValue(":title", "New Entry");
    query.bindValue(":username", "user");
    query.bindValue(":password", "pass");
    query.bindValue(":website", "http://");
    query.bindValue(":category", "General");
    query.bindValue(":updated_at", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (query.exec()) {
        loadRecords(); 
        int lastRow = model->rowCount() - 1;
        if (lastRow >= 0) {
            ui->tableView->selectRow(lastRow);
            ui->tableView->setCurrentIndex(model->index(lastRow, 1));
            ui->tableView->edit(model->index(lastRow, 1));
        }
        ui->statusLabel->setText("Record created in database.");
    } else {
        QMessageBox::warning(this, "Error", "Failed to create record:\n" + query.lastError().text());
    }
}

void MainWindow::onActionEditTriggered()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid()) {
        ui->tableView->edit(index);
    } else {
        QMessageBox::warning(this, "Edit", "Please select a cell to edit.");
    }
}

void MainWindow::onActionDeleteTriggered()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid()) {
        int id = model->data(model->index(index.row(), 0)).toInt();
        if (QMessageBox::question(this, "Delete", "Are you sure you want to delete this record?",
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            if (deleteFromDatabase(id)) {
                loadRecords();
                ui->statusLabel->setText("Record deleted from database.");
            }
        }
    } else {
        QMessageBox::warning(this, "Delete", "Please select a record to delete.");
    }
}

bool MainWindow::deleteFromDatabase(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM records WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        QMessageBox::warning(this, "Error", "Failed to delete record:\n" + query.lastError().text());
        return false;
    }
    return true;
}

void MainWindow::onItemChanged(QStandardItem *item)
{
    int row = item->row();
    int id = model->data(model->index(row, 0)).toInt();
    
    // Auto-update timestamp on any change except if changing the ID itself or the timestamp
    if (item->column() != 0 && item->column() != 6) {
        QString now = QDateTime::currentDateTime().toString(Qt::ISODate);
        model->blockSignals(true);
        model->setData(model->index(row, 6), now);
        model->blockSignals(false);
    }

    QSqlQuery query;
    query.prepare("UPDATE records SET title = :title, username = :username, password = :password, "
                  "website = :website, category = :category, updated_at = :updated_at WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":title", model->data(model->index(row, 1)));
    query.bindValue(":username", model->data(model->index(row, 2)));
    query.bindValue(":password", model->data(model->index(row, 3)));
    query.bindValue(":website", model->data(model->index(row, 4)));
    query.bindValue(":category", model->data(model->index(row, 5)));
    query.bindValue(":updated_at", model->data(model->index(row, 6)));

    if (!query.exec()) {
        ui->statusLabel->setText("Update failed!");
        QMessageBox::warning(this, "Error", "Failed to update record:\n" + query.lastError().text());
    } else {
        ui->statusLabel->setText("Database updated.");
    }
}

void MainWindow::onActionSaveTriggered()
{
    // In this implementation, data is saved on each change. 
    // We can explicitly reload just to be sure.
    loadRecords();
    QMessageBox::information(this, "Save", "All changes are automatically synchronized with SQLite.");
}

void MainWindow::onActionExitTriggered()
{
    close();
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    if (text.isEmpty()) {
        loadRecords();
        return;
    }

    model->blockSignals(true);
    model->removeRows(0, model->rowCount());

    QSqlQuery query;
    query.prepare("SELECT id, title, username, password, website, category, updated_at FROM records "
                  "WHERE title LIKE :search OR username LIKE :search OR website LIKE :search");
    query.bindValue(":search", "%" + text + "%");
    
    if (query.exec()) {
        while (query.next()) {
            int row = model->rowCount();
            model->insertRow(row);
            for (int i = 0; i < 7; ++i) {
                model->setData(model->index(row, i), query.value(i));
            }
        }
    }
    model->blockSignals(false);
}
