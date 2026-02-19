#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showMessage);
    connect(ui->actionDocumentOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionNewDocument, &QAction::triggered, this, &MainWindow::createNewFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showMessage()
{
    QMessageBox::information(this, "About", "Are you really need help?");
}

void MainWindow::openFile()
{
    QFileDialog::getOpenFileName(this);
}

void MainWindow::createNewFile()
{
    QFileDialog::getSaveFileName(this);
}
