#pragma once

#include <QWidget>
#include <QMap>
#include <QString>

class QPushButton;

/**
 * KeyboardWidget — Visual QWERTY keyboard.
 *
 * Call highlightKey(ch) to illuminate a key (e.g., on current char).
 * Call clearHighlight() to reset all keys to default style.
 */
class KeyboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KeyboardWidget(QWidget *parent = nullptr);

    void highlightKey(QChar ch);
    void clearHighlight();

private:
    // Map from key label (uppercase) to its QPushButton
    QMap<QString, QPushButton*> m_keyMap;

    QPushButton* makeKey(const QString &label, int minW = 40, int h = 36);
    void buildLayout();
};
