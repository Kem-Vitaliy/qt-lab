#pragma once
#include <QTextEdit>
#include <QString>

class TextDisplayWidget : public QTextEdit {
    Q_OBJECT
public:
    explicit TextDisplayWidget(QWidget *parent = nullptr);
    void setExerciseText(const QString &text);
    void updateProgress(int cursorPos);
private:
    QString m_text;
    int m_cursorPos = 0;
    void refreshDisplay();
};
