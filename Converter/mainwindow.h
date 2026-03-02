#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDoubleValidator>

#include "converter.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    enum class ActiveField {
        Top,
        Bottom
    };

    Converter converter;
    QDoubleValidator *numberValidator;
    ActiveField lastEditedField = ActiveField::Top;

    Ui::MainWindow *ui;

    void setupValidators();
    void setupConnections();

    void handleEdit(ActiveField sourceField);
    void recalculateForCurrentTab(ActiveField sourceField);

    QString formatValue(double value) const;

    Converter::Mode currentMode() const;

    void markError(QWidget *widget, bool error);

private slots:
    void onTopEdited(const QString &);
    void onBottomEdited(const QString &);

    void onUnitChanged(int);
    void onTabChanged(int);
};

#endif // MAINWINDOW_H

