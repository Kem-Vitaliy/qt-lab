#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ticketdialog.h"
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_model(new TicketTableModel(this))
    , m_repository(new CsvTicketRepository(QCoreApplication::applicationDirPath() + "/tickets.csv"))
{
    ui->setupUi(this);
    ui->tableViewTickets->setModel(m_model);
    ui->tableViewTickets->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewTickets->setSelectionMode(QAbstractItemView::SingleSelection);

    // Load data from repository
    m_model->setTickets(m_repository->load());

    connect(ui->tableViewTickets->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::updateActionsState);
    
    updateActionsState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::saveData()
{
    m_repository->save(m_model->getTickets());
}

void MainWindow::updateActionsState()
{
    bool hasSelection = ui->tableViewTickets->selectionModel()->hasSelection();
    ui->actionView->setEnabled(hasSelection);
    ui->actionEdit->setEnabled(hasSelection);
    ui->actionDelete->setEnabled(hasSelection);
}

void MainWindow::on_actionNew_triggered()
{
    TicketDialog dialog(TicketDialog::Mode::New, this);
    
    // Auto-increment ID based on existing max ID
    int newId = 1;
    QVector<Ticket> tickets = m_model->getTickets();
    for (const auto& t : tickets) {
        if (t.id >= newId) newId = t.id + 1;
    }
    
    Ticket newTicket;
    newTicket.id = newId;
    newTicket.createdAt = QDateTime::currentDateTime();
    dialog.setTicket(newTicket);

    if (dialog.exec() == QDialog::Accepted) {
        m_model->addTicket(dialog.getTicket());
        saveData();
    }
}

void MainWindow::on_actionView_triggered()
{
    QModelIndex index = ui->tableViewTickets->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select a ticket to view.");
        return;
    }

    TicketDialog dialog(TicketDialog::Mode::View, this);
    dialog.setTicket(m_model->getTicket(index.row()));
    if (dialog.exec() == QDialog::Accepted) {
        m_model->updateTicket(index.row(), dialog.getTicket());
        saveData();
    }
}

void MainWindow::on_actionEdit_triggered()
{
    QModelIndex index = ui->tableViewTickets->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select a ticket to edit.");
        return;
    }

    TicketDialog dialog(TicketDialog::Mode::Edit, this);
    dialog.setTicket(m_model->getTicket(index.row()));
    if (dialog.exec() == QDialog::Accepted) {
        m_model->updateTicket(index.row(), dialog.getTicket());
        saveData();
    }
}

void MainWindow::on_actionDelete_triggered()
{
    QModelIndex index = ui->tableViewTickets->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select a ticket to delete.");
        return;
    }

    if (QMessageBox::question(this, "Delete", "Are you sure you want to delete the selected ticket?") == QMessageBox::Yes) {
        m_model->removeTicket(index.row());
        saveData();
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About Helpdesk", "Helpdesk System v0.1\nCreated for Task 1.");
}
