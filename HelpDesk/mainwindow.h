#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tickettablemodel.h"
#include "csvticketrepository.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();
    void on_actionView_triggered();
    void on_actionEdit_triggered();
    void on_actionDelete_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void updateActionsState();

private:
    void saveData();

    Ui::MainWindow *ui;
    TicketTableModel *m_model;
    CsvTicketRepository *m_repository;
};
#endif // MAINWINDOW_H
