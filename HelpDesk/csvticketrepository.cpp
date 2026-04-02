#include "csvticketrepository.h"
#include <QFile>
#include <QTextStream>
#include <QSaveFile>
#include <QStringList>
#include <QDateTime>

CsvTicketRepository::CsvTicketRepository(const QString &filePath)
    : m_filePath(filePath)
{
}

QVector<Ticket> CsvTicketRepository::load()
{
    QVector<Ticket> tickets;
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return tickets;

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    // Skip header if it exists
    QString header = in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) continue;

        // Simple CSV parser that handles quotes
        QStringList fields;
        QString field;
        bool inQuotes = false;
        for (int i = 0; i < line.length(); ++i) {
            QChar c = line[i];
            if (c == '"') {
                if (inQuotes && i + 1 < line.length() && line[i + 1] == '"') {
                    field += '"';
                    i++;
                } else {
                    inQuotes = !inQuotes;
                }
            } else if (c == ',' && !inQuotes) {
                fields << field;
                field.clear();
            } else {
                field += c;
            }
        }
        fields << field;

        if (fields.size() >= 5) {
            Ticket t;
            t.id = fields[0].toInt();
            t.title = fields[1];
            t.priority = fields[2];
            t.status = fields[3];
            t.createdAt = QDateTime::fromString(fields[4], Qt::ISODate);
            if (fields.size() >= 6) {
                t.description = fields[5];
            }
            tickets.append(t);
        }
    }

    file.close();
    return tickets;
}

bool CsvTicketRepository::save(const QVector<Ticket> &tickets)
{
    QSaveFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // Header
    out << "ID,Title,Priority,Status,CreatedAt,Description\n";

    for (const Ticket &t : tickets) {
        out << QString::number(t.id) << ","
            << "\"" << escapeCsvField(t.title) << "\","
            << "\"" << escapeCsvField(t.priority) << "\","
            << "\"" << escapeCsvField(t.status) << "\","
            << t.createdAt.toString(Qt::ISODate) << ","
            << "\"" << escapeCsvField(t.description) << "\"\n";
    }

    return file.commit();
}

QString CsvTicketRepository::escapeCsvField(const QString &field)
{
    QString escaped = field;
    return escaped.replace("\"", "\"\"");
}
