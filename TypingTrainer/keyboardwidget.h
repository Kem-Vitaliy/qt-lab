#pragma once

#include <QWidget>
#include <QMap>
#include <QString>

class QPushButton;

/**
 * KeyboardWidget — Visual QWERTY keyboard.
 *
 * highlightKey(ch)  — підсвітити клавішу (напр., поточний символ).
 * clearHighlight()  — скинути всі клавіші до звичайного стилю.
 */
class KeyboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KeyboardWidget(QWidget *parent = nullptr);

    void highlightKey(QChar ch);
    void clearHighlight();

private:
    QMap<QString, QPushButton*> m_keyMap;

    /// modifier=true → використовує стиль модифікатора (Shift, Ctrl…)
    QPushButton *makeKey(const QString &label, bool modifier = false, int minW = 36);
    void buildLayout();
};
