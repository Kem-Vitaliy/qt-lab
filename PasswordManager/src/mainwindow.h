#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

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

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;

    void setupModel();
};
#endif // MAINWINDOW_H
