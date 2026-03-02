#pragma once

#include <QString>
#include <QStringList>

/**
 * LessonModel
 *
 * Зберігає повний текст уроку, розбиття на рядки та позицію
 * проходження у вигляді двох індексів: lineIndex / charIndex.
 *
 * Всі методи безпечні до виходу за межі (не падають на порожніх
 * рядках або невалідних індексах).
 */
class LessonModel
{
public:
    // ── Визначення уроків ─────────────────────────────────────────
    struct LessonDef {
        QString title;
        QString text;
        QString description;
    };

    /// Повертає список усіх вбудованих уроків.
    static QList<LessonDef> allLessons();

    // ── Конструктор / завантаження ────────────────────────────────
    LessonModel();
    explicit LessonModel(const QString &text);

    /// Завантажити новий текст і скинути позицію на початок.
    void loadText(const QString &text);

    /// Скинути позицію на початок поточного тексту.
    void reset();

    // ── Доступ до тексту ──────────────────────────────────────────
    /// Повний текст уроку.
    QString fullText() const { return m_text; }

    /// Список рядків (результат split('\n')).
    QStringList lines() const { return m_lines; }

    /// Рядок за індексом; повертає "" якщо індекс поза межами.
    QString lineAt(int index) const;

    /// Кількість рядків.
    int lineCount() const { return m_lines.size(); }

    // ── Поточна позиція ───────────────────────────────────────────
    int lineIndex() const { return m_lineIndex; }
    int charIndex() const { return m_charIndex; }

    /// Чи пройдено весь текст.
    bool isFinished() const;

    // ── Поточний / попередній рядок ───────────────────────────────
    /// Поточний рядок (або "", якщо текст закінчився).
    QString currentLine() const  { return lineAt(m_lineIndex); }

    /// Попередній рядок (або "", якщо ми на першому рядку).
    QString previousLine() const { return lineAt(m_lineIndex - 1); }

    // ── Фрагменти поточного рядка ─────────────────────────────────
    /// Вже пройдена частина поточного рядка (left(charIndex)).
    QString typedPart() const;

    /// Поточний символ для набору (один QChar або "").
    QString currentChar() const;

    /// Залишок рядка починаючи з наступного символу після поточного.
    QString remainingPart() const;

    // ── Просування позиції ────────────────────────────────────────
    /**
     * Збільшує charIndex на 1.
     * Якщо рядок завершився — переходить на наступний
     * (lineIndex++, charIndex = 0).
     * Повертає false якщо текст вже закінчився.
     */
    bool advance();

private:
    QString     m_text;
    QStringList m_lines;
    int         m_lineIndex = 0;
    int         m_charIndex = 0;

    void rebuildLines();
};
