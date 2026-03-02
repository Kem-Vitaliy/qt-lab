#include "textdisplaywidget.h"
#include <QStringList>

TextDisplayWidget::TextDisplayWidget(QWidget *parent)
    : QTextEdit(parent)
{
    setReadOnly(true);
    setFont(QFont("Courier New", 16));
    setStyleSheet(
        "QTextEdit {"
        "  background-color: #181825;"
        "  color: #cdd6f4;"
        "  border: 1px solid #45475a;"
        "  border-radius: 6px;"
        "  padding: 10px;"
        "}"
    );
}

void TextDisplayWidget::setExerciseText(const QString &text)
{
    m_text = text;
    m_cursorPos = 0;
    refreshDisplay();
}

void TextDisplayWidget::updateProgress(int cursorPos)
{
    m_cursorPos = cursorPos;
    refreshDisplay();
}

void TextDisplayWidget::refreshDisplay()
{
    if (m_text.isEmpty()) return;

    QStringList lines = m_text.split('\n');
    int charCount = 0;
    int cursorLine = 0, cursorInLine = 0;

    for (int i = 0; i < lines.size(); ++i) {
        int len = lines[i].length() + 1;
        if (charCount + len > m_cursorPos) {
            cursorLine    = i;
            cursorInLine  = m_cursorPos - charCount;
            break;
        }
        charCount += len;
    }

    QString html =
        "<html><body style=\""
        "font-family:'Courier New',monospace;"
        "font-size:17px;"
        "background:#181825;"
        "color:#cdd6f4;\">";

    for (int i = 0; i < lines.size(); ++i) {
        const QString &line = lines[i];
        QString lineHtml;

        if (i < cursorLine) {
            // completed line – muted purple bg
            lineHtml = "<p style=\"background:#2a2a3e;color:#a6adc8;"
                       "padding:4px 8px;margin:2px 0;\">";
            for (QChar c : line)
                lineHtml += (c == ' ') ? "&nbsp;" : QString(c).toHtmlEscaped();
            lineHtml += "</p>";
        } else if (i == cursorLine) {
            // active line – slight bg highlight
            lineHtml = "<p style=\"background:#1e1e2e;padding:4px 8px;margin:2px 0;\">";
            for (int j = 0; j < line.length(); ++j) {
                QString ch = (line[j] == ' ') ? "&nbsp;" : QString(line[j]).toHtmlEscaped();
                if (j < cursorInLine)
                    // typed – green
                    lineHtml += QString("<span style=\"background:#1e4429;color:#a6e3a1;\">%1</span>").arg(ch);
                else if (j == cursorInLine)
                    // current char – red highlight
                    lineHtml += QString("<span style=\"background:#f38ba8;color:#1e1e2e;"
                                        "padding:0 2px;\">%1</span>").arg(ch);
                else
                    lineHtml += ch;
            }
            lineHtml += "</p>";
        } else {
            // future lines – dimmed
            lineHtml = "<p style=\"color:#585b70;padding:4px 8px;margin:2px 0;\">";
            for (QChar c : line)
                lineHtml += (c == ' ') ? "&nbsp;" : QString(c).toHtmlEscaped();
            lineHtml += "</p>";
        }
        html += lineHtml;
    }
    html += "</body></html>";
    setHtml(html);
}
