#include "lessonmodel.h"
#include <utility>

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
    rebuildStates();
    reset();
}

void LessonModel::reset()
{
    m_lineIndex = 0;
    m_charIndex = 0;
    rebuildStates();
    skipEmptyLinesForward();
}

void LessonModel::rebuildLines()
{
    // split('\n') завжди повертає >= 1 елемент, але може бути порожній рядок
    m_lines = m_text.split('\n');
}

void LessonModel::rebuildStates()
{
    m_states.clear();
    m_states.reserve(m_lines.size());
    for (const QString &line : m_lines) {
        QVector<CharState> st;
        st.resize(line.length());
        for (int i = 0; i < st.size(); ++i) st[i] = CharState::Unknown;
        m_states.push_back(std::move(st));
    }
}

void LessonModel::skipEmptyLinesForward()
{
    while (!isFinished() && currentLine().isEmpty()) {
        ++m_lineIndex;
        m_charIndex = 0;
    }
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
    if (m_lines.isEmpty()) return true;
    if (m_lineIndex >= m_lines.size()) return true;
    const bool isLastLine = (m_lineIndex == m_lines.size() - 1);
    if (!isLastLine) return false;
    return m_charIndex >= m_lines.at(m_lineIndex).length();
}

bool LessonModel::atEndOfLine() const
{
    if (isFinished()) return true;
    const QString line = currentLine();
    return m_charIndex >= line.length();
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
    if (m_charIndex >= line.length())
        return QString();
    // символи після поточного (charIndex + 1)
    const int nextPos = m_charIndex + 1;
    return (nextPos >= line.length()) ? QString() : line.mid(nextPos);
}

// ── Просування позиції ──────────────────────────────────────────────────────

bool LessonModel::advance()
{
    if (isFinished())
        return false;

    QString line = currentLine();

    // Якщо рядок порожній — одразу на наступний
    if (line.isEmpty()) {
        ++m_lineIndex;
        m_charIndex = 0;
        skipEmptyLinesForward();
        return !isFinished();
    }

    // Просуваємось у межах рядка. На останньому символі стаємо в позицію "кінець рядка".
    if (m_charIndex < line.length())
        ++m_charIndex;
    return !isFinished();
}

// ── Ввід під час сесії ──────────────────────────────────────────────────────

LessonModel::CharState LessonModel::stateAt(int line, int pos) const
{
    if (line < 0 || line >= m_states.size()) return CharState::Unknown;
    if (pos < 0 || pos >= m_states.at(line).size()) return CharState::Unknown;
    return m_states.at(line).at(pos);
}

bool LessonModel::inputChar(QChar ch, bool *outCorrect)
{
    if (outCorrect) *outCorrect = false;
    if (isFinished()) return false;

    const QString line = currentLine();
    if (line.isEmpty()) return false;
    if (m_charIndex >= line.length()) return false; // кінець рядка — чекаємо Enter

    const QChar expected = line.at(m_charIndex);
    const bool correct = (ch == expected);
    if (m_lineIndex >= 0 && m_lineIndex < m_states.size() &&
        m_charIndex >= 0 && m_charIndex < m_states[m_lineIndex].size()) {
        m_states[m_lineIndex][m_charIndex] = correct ? CharState::Correct : CharState::Wrong;
    }

    if (outCorrect) *outCorrect = correct;
    advance();
    return true;
}

bool LessonModel::backspace()
{
    if (isFinished()) {
        // якщо вже за останнім рядком — дозволимо повернутись на кінець останнього
        if (m_lines.isEmpty()) return false;
        m_lineIndex = m_lines.size() - 1;
        m_charIndex = m_lines.at(m_lineIndex).length();
    }

    // Повернення в межах тексту (може перейти на попередній рядок)
    if (m_charIndex > 0) {
        --m_charIndex;
    } else if (m_lineIndex > 0) {
        // перейти на попередній непорожній рядок
        int li = m_lineIndex - 1;
        while (li >= 0 && m_lines.at(li).isEmpty()) --li;
        if (li < 0) return false;
        m_lineIndex = li;
        m_charIndex = m_lines.at(m_lineIndex).length();
        if (m_charIndex > 0) --m_charIndex;
        else return false;
    } else {
        return false; // на самому початку
    }

    if (m_lineIndex >= 0 && m_lineIndex < m_states.size() &&
        m_charIndex >= 0 && m_charIndex < m_states[m_lineIndex].size()) {
        m_states[m_lineIndex][m_charIndex] = CharState::Unknown;
    }
    return true;
}

bool LessonModel::enter()
{
    if (isFinished()) return false;

    const QString line = currentLine();
    if (m_charIndex < line.length()) return false; // рядок ще не завершено

    ++m_lineIndex;
    m_charIndex = 0;
    skipEmptyLinesForward();
    return !isFinished();
}
