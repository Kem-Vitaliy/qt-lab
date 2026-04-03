#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "passwordmodel.h"
#include "passwordproxymodel.h"
#include "repository.h"

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
    void onCategoryFilterChanged(const QString &category);
    void checkEmptyResults();

private:
    Ui::MainWindow *ui;
    PasswordModel *model;
    PasswordProxyModel *proxyModel;
    Repository *repository;

    void setupModel();
    void updateStatusLabel();
};
#endif // MAINWINDOW_H
