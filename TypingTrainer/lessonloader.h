#pragma once

#include <QString>
#include <QList>

/**
 * LessonLoader
 *
 * Відповідає за:
 *  - пошук файлів уроків у каталозі lessons/ через QDir
 *  - зберігання метаданих (назва, шлях, розмір)
 *  - завантаження тексту з файлу через QFile + QTextStream
 *
 * Не залежить від UI; легко тестується окремо.
 */
class LessonLoader
{
public:
    /// Метадані одного файлу уроку.
    struct LessonEntry {
        QString displayName;  ///< Ім'я без розширення (для ComboBox)
        QString filePath;     ///< Повний шлях до .txt файлу
        qint64  fileSize = 0; ///< Розмір у байтах (QFileInfo)
    };

    explicit LessonLoader();

    /**
     * Встановити каталог пошуку.
     * За замовчуванням — "lessons/" відносно QCoreApplication::applicationDirPath().
     */
    void setLessonsDir(const QString &dirPath);

    /// Поточний каталог пошуку.
    QString lessonsDir() const { return m_dir; }

    /**
     * Сканувати каталог і оновити внутрішній список уроків.
     * Повертає кількість знайдених уроків (0 якщо каталогу нема або він порожній).
     */
    int scan();

    /// Список знайдених уроків після scan().
    QList<LessonEntry> entries() const { return m_entries; }

    /// Кількість знайдених уроків.
    int count() const { return m_entries.size(); }

    /// Чи є хоч один урок.
    bool hasLessons() const { return !m_entries.isEmpty(); }

    /**
     * Завантажити текст уроку за індексом.
     * @param index  індекс у entries()
     * @param outText  [out] прочитаний текст
     * @param outError [out] текст помилки (порожній якщо успішно)
     * @return true якщо успішно
     */
    bool loadText(int index, QString &outText, QString &outError) const;

    /**
     * Завантажити текст уроку за шляхом до файлу.
     * @return true якщо успішно
     */
    bool loadFile(const QString &filePath, QString &outText, QString &outError) const;

private:
    QString            m_dir;
    QList<LessonEntry> m_entries;
};
