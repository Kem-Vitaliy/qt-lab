#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include "lessonmodel.h"
#include "lessonloader.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QPushButton;
class QKeyEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

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

    bool m_trainingInputEnabled = false;
    QPointer<QPushButton> m_lastHighlightedKey;
    QString m_lastHighlightedKeyStyle;

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
    void setTrainingInputEnabled(bool enabled);
    void highlightVirtualKeyForEvent(QKeyEvent *ke);
    void highlightVirtualKeyForChar(QChar ch);
    void highlightVirtualKeyByObjectName(const QString &objectName);
    void clearVirtualKeyHighlight();
};

#endif // MAINWINDOW_H
