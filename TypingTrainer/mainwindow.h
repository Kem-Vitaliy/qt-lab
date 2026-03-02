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

private slots:
    void onStartTrainingClicked();
    void onRestartTrainingClicked();
    void onReturnToMainClicked();
    void onAboutClicked();
    void onExitClicked();
    void onLessonChanged(int index);

private:
    Ui::MainWindow *ui;

    void setupLessons();
    void updateLessonDescription(int index);
};

#endif // MAINWINDOW_H
