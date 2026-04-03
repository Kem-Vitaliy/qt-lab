#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "databasemanager.h"
#include <QMessageBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize database
    if (!DatabaseManager::instance().initialize()) {
        QMessageBox::critical(this, "Error", "Failed to initialize database.");
    }

    repository = new Repository();
    model = new PasswordModel(repository, this);
    proxyModel = new PasswordProxyModel(this);

    setupModel();

    // Connect signals
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onActionNewTriggered);
    connect(ui->actionEdit, &QAction::triggered, this, &MainWindow::onActionEditTriggered);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::onActionDeleteTriggered);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onActionSaveTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onActionExitTriggered);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(ui->categoryComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onCategoryFilterChanged);
    
    // Connect model changes to check for empty results
    connect(proxyModel, &QAbstractItemModel::modelReset, this, &MainWindow::checkEmptyResults);
    connect(proxyModel, &QAbstractItemModel::rowsInserted, this, &MainWindow::checkEmptyResults);
    connect(proxyModel, &QAbstractItemModel::rowsRemoved, this, &MainWindow::checkEmptyResults);
    connect(proxyModel, &QAbstractItemModel::layoutChanged, this, &MainWindow::checkEmptyResults);
    
    // Initial check
    checkEmptyResults();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete repository;
    DatabaseManager::instance().close();
}

void MainWindow::setupModel()
{
    // Load initial data
    model->reloadData();

    // Set up proxy model
    proxyModel->setSourceModel(model);

    // Set model to view
    ui->tableView->setModel(proxyModel);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::onActionNewTriggered()
{
    Record record;
    record.title = "New Entry";
    record.username = "user";
    record.password = "pass";
    record.website = "http://";
    record.category = "General";
    record.updatedAt = QDateTime::currentDateTime().toString(Qt::ISODate);

    model->addRecord(record);
    
    // Select the new record (it's added at the end of the source model)
    int sourceRow = model->rowCount() - 1;
    QModelIndex sourceIndex = model->index(sourceRow, 1); // Focus on Title column
    QModelIndex proxyIndex = proxyModel->mapFromSource(sourceIndex);

    if (proxyIndex.isValid()) {
        ui->tableView->selectRow(proxyIndex.row());
        ui->tableView->setCurrentIndex(proxyIndex);
        ui->tableView->edit(proxyIndex);
    }
    
    ui->statusLabel->setText("New record added.");
}

void MainWindow::onActionEditTriggered()
{
    QModelIndex proxyIndex = ui->tableView->currentIndex();
    if (proxyIndex.isValid()) {
        ui->tableView->edit(proxyIndex);
    } else {
        QMessageBox::warning(this, "Edit", "Please select a cell to edit.");
    }
}

void MainWindow::onActionDeleteTriggered()
{
    QModelIndex proxyIndex = ui->tableView->currentIndex();
    if (proxyIndex.isValid()) {
        if (QMessageBox::question(this, "Delete", "Are you sure you want to delete this record?",
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            
            // Map proxy index to source index to get the actual row in the model
            QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
            if (sourceIndex.isValid()) {
                model->removeRecord(sourceIndex.row());
                ui->statusLabel->setText("Record deleted.");
            }
        }
    } else {
        QMessageBox::warning(this, "Delete", "Please select a record to delete.");
    }
}

void MainWindow::onActionSaveTriggered()
{
    QMessageBox::information(this, "Save", "Changes are automatically synchronized with the database when you finish editing a cell.");
}

void MainWindow::onActionExitTriggered()
{
    close();
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    proxyModel->setSearchString(text);
}

void MainWindow::onCategoryFilterChanged(const QString &category)
{
    proxyModel->setCategoryFilter(category);
}

void MainWindow::checkEmptyResults()
{
    if (proxyModel->rowCount() == 0) {
        ui->statusLabel->setText("No records found matching the criteria.");
    } else {
        ui->statusLabel->setText(QString("Displaying %1 records.").arg(proxyModel->rowCount()));
    }
}
