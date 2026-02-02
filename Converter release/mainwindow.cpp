#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->lineEditLength, &QLineEdit::textChanged,
            this, &MainWindow::conv_from_to1);

    connect(ui->comboBox, &QComboBox::currentIndexChanged,
            this, &MainWindow::conv_from_to1);

    connect(ui->comboBox_2, &QComboBox::currentIndexChanged,
            this, &MainWindow::conv_from_to1);

    connect(ui->lineEditMass, &QLineEdit::textChanged,
            this, &MainWindow::conv_from_to2);

    connect(ui->comboBox_3, &QComboBox::currentIndexChanged,
            this, &MainWindow::conv_from_to2);

    connect(ui->comboBox_4, &QComboBox::currentIndexChanged,
            this, &MainWindow::conv_from_to2);

    connect(ui->lineEditTemperature, &QLineEdit::textChanged,
            this, &MainWindow::conv_from_to3);

    connect(ui->comboBox_5, &QComboBox::currentIndexChanged,
            this, &MainWindow::conv_from_to3);

    connect(ui->comboBox_6, &QComboBox::currentIndexChanged,
            this, &MainWindow::conv_from_to3);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::conv_from_to1()
{
    QString text = ui->lineEditLength->text();

    if(text.isEmpty())
    {
        ui->lineEditLength->clear();
        ui->lineEditLength_2->clear();
        return;
    }

    bool ok;
    double amount = text.toDouble(&ok);

    if(!ok)
    {
        ui->lineEditLength->clear();
        QMessageBox::warning(this, "Error", "Не корректний тип данних");
        return;
    }

    QString from = ui->comboBox->currentText();
    QString to = ui->comboBox_2->currentText();

    double result = converter.conv(amount, from, to);

    ui->lineEditLength_2->setText(QString::number(result, 'f', 5));
}

void MainWindow::conv_from_to2()
{
    QString text = ui->lineEditMass->text();

    if(text.isEmpty())
    {
        ui->lineEditMass->clear();
        ui->lineEditMass_2->clear();
        return;
    }

    bool ok;
    double amount = text.toDouble(&ok);

    if(!ok)
    {
        ui->lineEditMass->clear();
        QMessageBox::warning(this, "Error", "Не корректний тип данних");
        return;
    }

    QString from = ui->comboBox_3->currentText();
    QString to = ui->comboBox_4->currentText();

    double result = converter.conv(amount, from, to);

    ui->lineEditMass_2->setText(QString::number(result));
}

void MainWindow::conv_from_to3()
{
    QString text = ui->lineEditTemperature->text();

    if(text.isEmpty())
    {
        ui->lineEditTemperature->clear();
        ui->lineEditTemperature_2->clear();
        return;
    }

    bool ok;
    double amount = text.toDouble(&ok);

    if(!ok)
    {
        ui->lineEditTemperature->clear();
        QMessageBox::warning(this, "Error", "Не корректний тип данних");
        return;
    }

    QString from = ui->comboBox_5->currentText();
    QString to = ui->comboBox_6->currentText();

    double result = converter.conv(amount, from, to);
    ui->lineEditTemperature_2->setText(QString::number(result, 'f', 5));
}
