#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "lessonmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // ── Навігація між екранами ──────────────────────────
    void onStartTrainingClicked();
    void onRestartTrainingClicked();
    void onReturnToMainClicked();

    // ── Меню ────────────────────────────────────────────
    void onAboutClicked();
    void onExitClicked();

    // ── Start-екран ─────────────────────────────────────
    void onLessonChanged(int index);

    // ── Тестова дія (ПР7 п.8) ──────────────────────────
    /// Просуває позицію на один символ і оновлює Training UI.
    void onAdvanceChar();

private:
    Ui::MainWindow *ui;
    LessonModel    m_model;   ///< Модель поточного уроку

    // ── Ініціалізація ────────────────────────────────────
    void setupLessons();

    // ── Оновлення Training-екрана ────────────────────────
    /// Оновити обидва рядки тексту у Training UI з моделі.
    void refreshTrainingDisplay();

    /// Побудувати HTML-рядок з підсвіченням набраного/поточного/залишку.
    QString buildLineHtml(const QString &typed,
                          const QString &current,
                          const QString &remaining,
                          bool isActive) const;
};

#endif // MAINWINDOW_H
