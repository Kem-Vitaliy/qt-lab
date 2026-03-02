#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showMessage);
    connect(ui->actionDocumentOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionNewDocument, &QAction::triggered, this, &MainWindow::createNewFile);
    connect(ui->actionSaveDocument, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionSelectAll, &QAction::triggered, this, &MainWindow::selectAllText);
    connect(ui->actionCut, &QAction::triggered, this, &MainWindow::cutText);
    connect(ui->actionCopy, &QAction::triggered, this, &MainWindow::copyText);
    connect(ui->actionPast, &QAction::triggered, this, &MainWindow::pastText);
    connect(ui->actionSaveAsDocument, &QAction::triggered, this, &MainWindow::saveAsDocument);
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
    currentFile = QFileDialog::getOpenFileName(
        this,
        "Open file",
        "",
        "(*.txt *.docx)"
    );

    if(currentFile.isEmpty())
        return;

    QFile file(currentFile);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(this, "Error", "Can not open the file");
        return;
    }

    QTextStream in(&file);
    QString text = in.readAll();

    file.close();

    ui->textEdit->setPlainText(text);
}

void MainWindow::createNewFile()
{
    QFileDialog::getSaveFileName(this);
}

void MainWindow::saveFile(){

    if (currentFile.isEmpty()){
        QMessageBox::warning(this, "Error", "File not open");
        return;
    }

    QFile file(currentFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(this, "Error", "Can not save file");
        return;
    }

    QTextStream out(&file);
    out << ui->textEdit->toPlainText();

    file.close();
}

void MainWindow::selectAllText(){
    ui->textEdit->selectAll();
}

void MainWindow::cutText(){
    ui->textEdit->cut();
}

void MainWindow::copyText(){
    ui->textEdit->copy();
}

void MainWindow::pastText(){
    ui->textEdit->paste();
}

void MainWindow::saveAsDocument(){
    QString fileName = QFileDialog::getSaveFileName(this, "Save file");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(this, "Error", "Can not save file");
        return;
    }

    QTextStream out(&file);
    out << ui->textEdit->toPlainText();

    file.close();
}
