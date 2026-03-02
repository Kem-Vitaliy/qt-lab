#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString currentFile;

private:
    Ui::MainWindow *ui;

private slots:
    void showMessage();
    void openFile();
    void createNewFile();
    void saveFile();
    void selectAllText();
    void cutText();
    void copyText();
    void pastText();
    void saveAsDocument();
};

#endif // MAINWINDOW_H
