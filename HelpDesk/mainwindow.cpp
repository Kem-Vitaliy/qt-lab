#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ticketdialog.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    TicketDialog *dialog = new TicketDialog(TicketDialog::Mode::New, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::on_actionView_triggered()
{
    TicketDialog *dialog = new TicketDialog(TicketDialog::Mode::View, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::on_actionEdit_triggered()
{
    TicketDialog *dialog = new TicketDialog(TicketDialog::Mode::Edit, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::on_actionDelete_triggered()
{
    QMessageBox::question(this, "Delete", "Are you sure you want to delete the selected ticket?");
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About Helpdesk", "Helpdesk System v0.1\nCreated for Task 1.");
}
