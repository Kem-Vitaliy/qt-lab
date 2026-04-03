#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupModel();

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onActionNewTriggered);
    connect(ui->actionEdit, &QAction::triggered, this, &MainWindow::onActionEditTriggered);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::onActionDeleteTriggered);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onActionSaveTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onActionExitTriggered);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
}

MainWindow::~MainWindow()
{
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

void MainWindow::onActionNewTriggered()
{
    int row = model->rowCount();
    model->insertRow(row);
    
    // Set default values
    model->setData(model->index(row, 0), row + 1); // Mock ID
    model->setData(model->index(row, 6), QDateTime::currentDateTime().toString(Qt::ISODate));

    ui->tableView->setCurrentIndex(model->index(row, 1));
    ui->tableView->edit(model->index(row, 1));
    ui->statusLabel->setText("New record added.");
}

void MainWindow::onActionEditTriggered()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid()) {
        ui->tableView->edit(index);
        ui->statusLabel->setText("Editing record...");
    } else {
        QMessageBox::warning(this, "Edit", "Please select a cell to edit.");
    }
}

void MainWindow::onActionDeleteTriggered()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid()) {
        int row = index.row();
        if (QMessageBox::question(this, "Delete", "Are you sure you want to delete this record?",
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            model->removeRow(row);
            ui->statusLabel->setText("Record deleted.");
        }
    } else {
        QMessageBox::warning(this, "Delete", "Please select a record to delete.");
    }
}

void MainWindow::onActionSaveTriggered()
{
    QMessageBox::information(this, "Save", "Changes saved successfully! (Simulation)");
    ui->statusLabel->setText("Changes saved.");
}

void MainWindow::onActionExitTriggered()
{
    close();
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    // Placeholder for filtering logic
    ui->statusLabel->setText("Filtering for: " + text);
}
