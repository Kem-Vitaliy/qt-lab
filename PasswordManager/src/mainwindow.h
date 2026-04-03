#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

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
    void onActionNewTriggered();
    void onActionEditTriggered();
    void onActionDeleteTriggered();
    void onActionSaveTriggered();
    void onActionExitTriggered();
    void onSearchTextChanged(const QString &text);
    void onItemChanged(QStandardItem *item);

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QSqlDatabase db;

    void setupModel();
    bool initDatabase();
    void loadRecords();
    bool saveRecord(int row);
    bool deleteFromDatabase(int id);
};
#endif // MAINWINDOW_H
