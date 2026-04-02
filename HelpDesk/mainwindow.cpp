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
    , m_proxyModel(new TicketSortFilterProxyModel(this))
    , m_repository(new CsvTicketRepository(QCoreApplication::applicationDirPath() + "/tickets.csv"))
{
    ui->setupUi(this);
    
    m_proxyModel->setSourceModel(m_model);
    ui->tableViewTickets->setModel(m_proxyModel);
    ui->tableViewTickets->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewTickets->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewTickets->setSortingEnabled(true);

    // Load data from repository
    m_model->setTickets(m_repository->load());

    connect(ui->tableViewTickets->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::updateActionsState);
    
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &MainWindow::onFilterChanged);
    connect(ui->comboBoxFilterStatus, &QComboBox::currentTextChanged, this, &MainWindow::onFilterChanged);
    connect(ui->comboBoxFilterPriority, &QComboBox::currentTextChanged, this, &MainWindow::onFilterChanged);
    connect(ui->tableViewTickets, &QTableView::doubleClicked, this, &MainWindow::onTableViewDoubleClicked);
    
    updateActionsState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onFilterChanged()
{
    m_proxyModel->setFilterFixedString(ui->lineEditSearch->text());
    m_proxyModel->setFilterStatus(ui->comboBoxFilterStatus->currentText());
    m_proxyModel->setFilterPriority(ui->comboBoxFilterPriority->currentText());
    
    // Update empty state visibility
    bool noResults = (m_proxyModel->rowCount() == 0 && m_model->rowCount() > 0);
    ui->labelNoTickets->setVisible(noResults);
}

void MainWindow::onTableViewDoubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        on_actionView_triggered();
    }
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
        onFilterChanged(); // Refresh filter state
    }
}

void MainWindow::on_actionView_triggered()
{
    QModelIndex proxyIndex = ui->tableViewTickets->currentIndex();
    if (!proxyIndex.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select a ticket to view.");
        return;
    }

    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
    TicketDialog dialog(TicketDialog::Mode::View, this);
    dialog.setTicket(m_model->getTicket(sourceIndex.row()));
    if (dialog.exec() == QDialog::Accepted) {
        m_model->updateTicket(sourceIndex.row(), dialog.getTicket());
        saveData();
    }
}

void MainWindow::on_actionEdit_triggered()
{
    QModelIndex proxyIndex = ui->tableViewTickets->currentIndex();
    if (!proxyIndex.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select a ticket to edit.");
        return;
    }

    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
    TicketDialog dialog(TicketDialog::Mode::Edit, this);
    dialog.setTicket(m_model->getTicket(sourceIndex.row()));
    if (dialog.exec() == QDialog::Accepted) {
        m_model->updateTicket(sourceIndex.row(), dialog.getTicket());
        saveData();
    }
}

void MainWindow::on_actionDelete_triggered()
{
    QModelIndex proxyIndex = ui->tableViewTickets->currentIndex();
    if (!proxyIndex.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select a ticket to delete.");
        return;
    }

    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
    if (QMessageBox::question(this, "Delete", "Are you sure you want to delete the selected ticket?") == QMessageBox::Yes) {
        m_model->removeTicket(sourceIndex.row());
        saveData();
        onFilterChanged(); // Refresh filter state
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
