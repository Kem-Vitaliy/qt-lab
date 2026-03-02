#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "lessonmodel.h"
#include "lessonloader.h"

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
    // ── Навігація між екранами ──────────────────────────────
    void onStartTrainingClicked();
    void onRestartTrainingClicked();
    void onReturnToMainClicked();

    // ── Меню ────────────────────────────────────────────────
    void onAboutClicked();
    void onExitClicked();

    // ── Start-екран ─────────────────────────────────────────
    /// Вибір уроку в ComboBox → читати файл, оновити модель.
    void onLessonChanged(int index);

    /// Кнопка «Random lesson» → вибір випадкового уроку (п.6).
    void onRandomLesson();

    /// Кнопка / дія «Reload lessons» → повторно сканувати папку (п.7).
    void onReloadLessons();

    // ── Training-екран ──────────────────────────────────────
    /// Debug: просунути позицію на 1 символ (з ПР7 п.8).
    void onAdvanceChar();

private:
    Ui::MainWindow *ui;
    LessonModel    m_model;   ///< Модель поточного уроку
    LessonLoader   m_loader;  ///< Файловий сканер / завантажувач

    // ── Ініціалізація / сканування ───────────────────────────
    /**
     * Сканує lessons/, заповнює ComboBox.
     * keepSelection=true → намагатися зберегти поточний вибір після reload.
     */
    void populateComboBox(bool keepSelection = false);

    /// Завантажити урок з файлу за індексом ComboBox.
    void loadLessonByIndex(int index);

    // ── Оновлення Training UI ────────────────────────────────
    void refreshTrainingDisplay();
};

#endif // MAINWINDOW_H
