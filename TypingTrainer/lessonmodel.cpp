#include "lessonmodel.h"

// ── Вбудовані уроки ────────────────────────────────────────────────────────

QList<LessonModel::LessonDef> LessonModel::allLessons()
{
    return {
        {
            "Starter Text",
            "jfj j jff ffjjjj fjjf fjjj jjjjjjj\n"
            "fff jjj fff jjj fj fj fj fj fj fj\n"
            "asd asd fgh fgh jkl jkl asd fgh jkl\n"
            "the cat sat on the mat and ate a rat",
            "Basic home row exercise. Practice the fundamental finger positions: a s d f j k l ;"
        },
        {
            "Numbers and Symbols",
            "1234567890 !@#$%^&*()\n"
            "0987654321 )(*&^%$#@!\n"
            "price: $12.50; count: 42; rate: 3.14%\n"
            "phone: (555) 123-4567; code: #A1B2C3",
            "Practice typing numbers 0-9 and common symbols: ! @ # $ % ^ & * ( )"
        },
        {
            "Common Words Part 1",
            "the quick brown fox jumps over the lazy dog\n"
            "a stitch in time saves nine or so they say\n"
            "pack my box with five dozen liquor jugs now\n"
            "how vexingly quick daft zebras jump today",
            "Frequently used English words — short and medium length."
        },
        {
            "Common Words Part 2",
            "practice makes perfect every single day of work\n"
            "hard work always pays off in the end of time\n"
            "success is not final failure is not fatal here\n"
            "it is the courage to continue that counts most",
            "More common words, longer sequences for fluency building."
        },
        {
            "Python Code Sample",
            "def greet(name):\n"
            "    return \"Hello, \" + name + \"!\"\n"
            "for i in range(10):\n"
            "    print(greet(\"World\"))",
            "Python code snippets to practice programming-style typing."
        },
        {
            "Lorem Ipsum",
            "Lorem ipsum dolor sit amet, consectetur\n"
            "adipiscing elit, sed do eiusmod tempor\n"
            "incididunt ut labore et dolore magna aliqua\n"
            "ut enim ad minim veniam, quis nostrud",
            "Classic Lorem Ipsum placeholder text for general typing practice."
        }
    };
}

// ── Конструктори ────────────────────────────────────────────────────────────

LessonModel::LessonModel()
{
    auto lessons = allLessons();
    if (!lessons.isEmpty())
        loadText(lessons.first().text);
}

LessonModel::LessonModel(const QString &text)
{
    loadText(text);
}

// ── Завантаження тексту ─────────────────────────────────────────────────────

void LessonModel::loadText(const QString &text)
{
    m_text = text;
    rebuildLines();
    reset();
}

void LessonModel::reset()
{
    m_lineIndex = 0;
    m_charIndex = 0;
}

void LessonModel::rebuildLines()
{
    // split('\n') завжди повертає >= 1 елемент, але може бути порожній рядок
    m_lines = m_text.split('\n');
}

// ── Доступ до рядків ────────────────────────────────────────────────────────

QString LessonModel::lineAt(int index) const
{
    if (index < 0 || index >= m_lines.size())
        return QString();          // безпечне повернення порожнього
    return m_lines.at(index);
}

// ── Стан завершення ─────────────────────────────────────────────────────────

bool LessonModel::isFinished() const
{
    return m_lineIndex >= m_lines.size();
}

// ── Фрагменти поточного рядка ───────────────────────────────────────────────

QString LessonModel::typedPart() const
{
    QString line = currentLine();
    if (line.isEmpty() || m_charIndex <= 0)
        return QString();
    // left() безпечний: якщо charIndex > length(), повертає весь рядок
    return line.left(qMin(m_charIndex, line.length()));
}

QString LessonModel::currentChar() const
{
    QString line = currentLine();
    if (line.isEmpty() || m_charIndex >= line.length())
        return QString();
    return QString(line.at(m_charIndex));
}

QString LessonModel::remainingPart() const
{
    QString line = currentLine();
    if (line.isEmpty())
        return QString();
    // символи після поточного (charIndex + 1)
    int nextPos = m_charIndex + 1;
    if (nextPos >= line.length())
        return QString();
    return line.mid(nextPos);
}

// ── Просування позиції ──────────────────────────────────────────────────────

bool LessonModel::advance()
{
    if (isFinished())
        return false;

    QString line = currentLine();

    // Якщо є ще символи в поточному рядку — просто збільшуємо charIndex
    if (m_charIndex < line.length() - 1) {
        ++m_charIndex;
        return true;
    }

    // Кінець рядка — переходимо на наступний
    ++m_lineIndex;
    m_charIndex = 0;

    // Пропускаємо порожні рядки автоматично
    while (!isFinished() && currentLine().isEmpty()) {
        ++m_lineIndex;
    }

    return !isFinished();
}
