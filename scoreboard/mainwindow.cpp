#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->BtnPlusA, &QPushButton::clicked,
            this, &MainWindow::PlusA);

    connect(ui->BtnMinusA, &QPushButton::clicked,
            this, &MainWindow::MinusA);

    connect(ui->BtnPlusB, &QPushButton::clicked,
            this, &MainWindow::PlusB);

    connect(ui->BtnMinusB, &QPushButton::clicked,
            this, &MainWindow::MinusB);

    connect(ui->Reset, &QPushButton::clicked,
            this, &MainWindow::Reset);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::PlusA(){
    playerA.plus();
    ui->PointsA->display(playerA.getpoints());
}

void MainWindow::MinusA()
{
    playerA.minus();
    ui->PointsA->display(playerA.getpoints());
}

void MainWindow::PlusB()
{
    playerB.plus();
    ui->PointsB->display(playerB.getpoints());
}

void MainWindow::MinusB()
{
    playerB.minus();
    ui->PointsB->display(playerB.getpoints());
}

void MainWindow::Reset()
{
    playerA.reset();
    playerB.reset();
    ui->PointsA->display(playerA.getpoints());
    ui->PointsB->display(playerB.getpoints());
}
