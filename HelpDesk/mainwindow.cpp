#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ticketdialog.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_model(new TicketTableModel(this))
{
    ui->setupUi(this);
    ui->tableViewTickets->setModel(m_model);
    ui->tableViewTickets->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewTickets->setSelectionMode(QAbstractItemView::SingleSelection);

    // Seed test data
    m_model->addTicket({1, "Cannot log in", "High", "Open", QDateTime::currentDateTime().addDays(-2), "User reports password reset doesn't work."});
    m_model->addTicket({2, "Printer out of toner", "Low", "In Progress", QDateTime::currentDateTime().addDays(-1), "Floor 3 printer needs toner replacement."});
    m_model->addTicket({3, "New software request", "Normal", "Closed", QDateTime::currentDateTime().addSecs(-5 * 3600), "Request for Visual Studio installation."});
    m_model->addTicket({4, "Network is slow", "Critical", "Open", QDateTime::currentDateTime().addSecs(-2 * 3600), "Multiple reports of intermittent connection."});
    m_model->addTicket({5, "Monitor flickering", "Normal", "Resolved", QDateTime::currentDateTime().addSecs(-30 * 60), "Cable replaced, flickering stopped."});

    connect(ui->tableViewTickets->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::updateActionsState);
    
    updateActionsState();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    
    // Auto-increment ID for simplicity
    int newId = 1;
    if (m_model->rowCount() > 0) {
        newId = m_model->getTicket(m_model->rowCount() - 1).id + 1;
    }
    
    Ticket newTicket;
    newTicket.id = newId;
    newTicket.createdAt = QDateTime::currentDateTime();
    dialog.setTicket(newTicket);

    if (dialog.exec() == QDialog::Accepted) {
        m_model->addTicket(dialog.getTicket());
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
