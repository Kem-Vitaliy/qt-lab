#include "lessonloader.h"

#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>

// ═══════════════════════════════════════════════════════════════════
// Конструктор
// ═══════════════════════════════════════════════════════════════════

LessonLoader::LessonLoader()
{
    // Каталог за замовчуванням: поруч із виконуваним файлом
    m_dir = QCoreApplication::applicationDirPath() + "/lessons";
}

void LessonLoader::setLessonsDir(const QString &dirPath)
{
    m_dir = dirPath;
}

// ═══════════════════════════════════════════════════════════════════
// Сканування каталогу (п.3)
// ═══════════════════════════════════════════════════════════════════

int LessonLoader::scan()
{
    m_entries.clear();

    // ── п.8: каталог відсутній — повертаємо 0, не падаємо ──────────
    QDir dir(m_dir);
    if (!dir.exists()) {
        qWarning() << "[LessonLoader] Lessons directory not found:" << m_dir;
        return 0;
    }

    // Отримати *.txt, відсортовані за іменем
    const QStringList nameFilters = { "*.txt" };
    const QFileInfoList fileList  = dir.entryInfoList(
        nameFilters,
        QDir::Files | QDir::Readable,
        QDir::Name
    );

    // ── п.8: є файли не .txt → вони просто не попадають у фільтр ──

    for (const QFileInfo &fi : fileList) {
        LessonEntry entry;
        entry.filePath    = fi.absoluteFilePath();
        entry.fileSize    = fi.size();
        // Назва без розширення, підкреслення → пробіли для читабельності
        entry.displayName = fi.completeBaseName().replace('_', ' ');
        m_entries.append(entry);
    }

    qDebug() << "[LessonLoader] Found" << m_entries.size() << "lessons in" << m_dir;
    return m_entries.size();
}

// ═══════════════════════════════════════════════════════════════════
// Завантаження тексту (п.5)
// ═══════════════════════════════════════════════════════════════════

bool LessonLoader::loadText(int index, QString &outText, QString &outError) const
{
    // ── п.8: якщо уроків 0 або індекс невалідний ───────────────────
    if (index < 0 || index >= m_entries.size()) {
        outError = QString("Invalid lesson index: %1 (total: %2)")
                       .arg(index).arg(m_entries.size());
        qWarning() << "[LessonLoader]" << outError;
        return false;
    }
    return loadFile(m_entries.at(index).filePath, outText, outError);
}

bool LessonLoader::loadFile(const QString &filePath,
                             QString       &outText,
                             QString       &outError) const
{
    QFile file(filePath);

    // ── п.8: файл не відкривається — не падаємо, повертаємо false ──
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        outError = QString("Cannot open file '%1': %2")
                       .arg(filePath, file.errorString());
        qWarning() << "[LessonLoader]" << outError;
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);  // Qt 6
    outText = in.readAll();
    file.close();

    // Прибрати зайвий перевід рядка в кінці, якщо є
    while (outText.endsWith('\n') || outText.endsWith('\r'))
        outText.chop(1);

    if (outText.isEmpty()) {
        outError = QString("File '%1' is empty.").arg(filePath);
        qWarning() << "[LessonLoader]" << outError;
        return false;
    }

    outError.clear();
    return true;
}
