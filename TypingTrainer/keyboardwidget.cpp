#include "keyboardwidget.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

// Dark theme (Catppuccin Mocha palette)
static const QString KEY_NORMAL =
    "QPushButton {"
    "  background-color: #313244;"
    "  color: #cdd6f4;"
    "  border: 1px solid #45475a;"
    "  border-radius: 4px;"
    "  font-size: 12px;"
    "  font-weight: bold;"
    "}"
    "QPushButton:hover { background-color: #45475a; }"
    "QPushButton:pressed { background-color: #585b70; }";

static const QString KEY_MODIFIER =
    "QPushButton {"
    "  background-color: #45475a;"
    "  color: #cdd6f4;"
    "  border: 1px solid #585b70;"
    "  border-radius: 4px;"
    "  font-size: 11px;"
    "}"
    "QPushButton:hover { background-color: #585b70; }";

static const QString KEY_HIGHLIGHT =
    "QPushButton {"
    "  background-color: #cba6f7;"
    "  color: #1e1e2e;"
    "  border: 2px solid #d0b4fe;"
    "  border-radius: 4px;"
    "  font-size: 12px;"
    "  font-weight: bold;"
    "}";

KeyboardWidget::KeyboardWidget(QWidget *parent)
    : QWidget(parent)
{
    buildLayout();
}

QPushButton *KeyboardWidget::makeKey(const QString &label, bool modifier, int minW)
{
    auto *btn = new QPushButton(label, this);
    btn->setMinimumSize(minW, 30);
    btn->setMaximumHeight(30);
    if (!modifier) btn->setMaximumWidth(42);
    btn->setFocusPolicy(Qt::NoFocus);
    btn->setStyleSheet(modifier ? KEY_MODIFIER : KEY_NORMAL);
    m_keyMap[label.toUpper()] = btn;
    return btn;
}

void KeyboardWidget::buildLayout()
{
    auto *vl = new QVBoxLayout(this);
    vl->setSpacing(4);
    vl->setContentsMargins(8, 6, 8, 6);

    auto addRow = [&](QList<QPair<QString,bool>> keys, int extraStretch = 1) {
        auto *row = new QHBoxLayout();
        row->setSpacing(3);
        for (auto &[label, mod] : keys) {
            int minW = mod ? (label.length() > 3 ? 88 : 60) : 36;
            row->addWidget(makeKey(label, mod, minW));
        }
        if (extraStretch) row->addStretch();
        vl->addLayout(row);
    };

    using P = QPair<QString,bool>;
    addRow({ P{"~",false},P{"1",false},P{"2",false},P{"3",false},P{"4",false},
             P{"5",false},P{"6",false},P{"7",false},P{"8",false},P{"9",false},
             P{"0",false},P{"-",false},P{"=",false},P{"Backspace",true} });

    addRow({ P{"Tab",true},P{"Q",false},P{"W",false},P{"E",false},P{"R",false},
             P{"T",false},P{"Y",false},P{"U",false},P{"I",false},P{"O",false},
             P{"P",false},P{"[",false},P{"]",false},P{"\\",false} });

    addRow({ P{"Caps",true},P{"A",false},P{"S",false},P{"D",false},P{"F",false},
             P{"G",false},P{"H",false},P{"J",false},P{"K",false},P{"L",false},
             P{";",false},P{"'",false},P{"Enter",true} });

    addRow({ P{"Shift",true},P{"Z",false},P{"X",false},P{"C",false},P{"V",false},
             P{"B",false},P{"N",false},P{"M",false},P{",",false},P{".",false},
             P{"/",false},P{"Shift",true} });

    // Space bar row manually (wide space)
    auto *row5 = new QHBoxLayout();
    row5->setSpacing(3);
    row5->addWidget(makeKey("Ctrl", true, 54));
    auto *sp = new QPushButton("Space", this);
    sp->setMinimumSize(260, 30);
    sp->setMaximumHeight(30);
    sp->setFocusPolicy(Qt::NoFocus);
    sp->setStyleSheet(KEY_NORMAL);
    m_keyMap["SPACE"] = sp;
    row5->addWidget(sp);
    row5->addWidget(makeKey("Ctrl", true, 54));
    row5->addStretch();
    vl->addLayout(row5);
}

void KeyboardWidget::highlightKey(QChar ch)
{
    clearHighlight();
    QString key = (ch == ' ') ? "SPACE" : QString(ch).toUpper();
    if (m_keyMap.contains(key))
        m_keyMap[key]->setStyleSheet(KEY_HIGHLIGHT);
}

void KeyboardWidget::clearHighlight()
{
    for (auto it = m_keyMap.begin(); it != m_keyMap.end(); ++it) {
        const QString &k = it.key();
        bool mod = (k == "TAB" || k == "CAPS" || k == "SHIFT" ||
                    k == "ENTER" || k == "CTRL" || k == "BACKSPACE");
        it.value()->setStyleSheet(mod ? KEY_MODIFIER : KEY_NORMAL);
    }
}
