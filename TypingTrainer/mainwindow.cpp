#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QAction>
#include <QRandomGenerator>
#include <QDebug>
#include <QKeyEvent>
#include <QPushButton>

#include <QTimer>
#include <QSettings>
#include <QActionGroup>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("TypingTrainer");
    resize(860, 620);

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTimerTick);

    populateComboBox();
    loadSettings();

    connect(ui->actionExit,  &QAction::triggered, this, &MainWindow::onExitClicked);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAboutClicked);

    connect(ui->btnStartTraining,
            &QPushButton::clicked, this, &MainWindow::onStartTrainingClicked);
    connect(ui->btnRestartTraining,
            &QPushButton::clicked, this, &MainWindow::onRestartTrainingClicked);
    connect(ui->btnReturnToMain,
            &QPushButton::clicked, this, &MainWindow::onReturnToMainClicked);
    connect(ui->btnBackToMain,
            &QPushButton::clicked, this, &MainWindow::onReturnToMainClicked);

    connect(ui->comboLesson,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onLessonChanged);

    // Меню Lessons: Random + Reload (п.6, п.7)
    QMenu   *menuLessons = menuBar()->addMenu("Lessons");
    QAction *actRandom   = menuLessons->addAction("Random lesson");
    QAction *actReload   = menuLessons->addAction("Reload lessons");
    actRandom->setShortcut(QKeySequence("Ctrl+R"));
    actReload->setShortcut(QKeySequence(Qt::Key_F5));
    connect(actRandom, &QAction::triggered, this, &MainWindow::onRandomLesson);
    connect(actReload, &QAction::triggered, this, &MainWindow::onReloadLessons);

    // Метрика швидкості (п.8.2)
    QMenu *menuSettings = menuBar()->addMenu("Settings");
    QActionGroup *groupSpeed = new QActionGroup(this);
    QAction *actCPM = menuSettings->addAction("CPM (Characters per minute)");
    QAction *actWPM = menuSettings->addAction("WPM (Words per minute)");
    actCPM->setCheckable(true);
    actWPM->setCheckable(true);
    groupSpeed->addAction(actCPM);
    groupSpeed->addAction(actWPM);

    if (m_speedMetric == CPM) actCPM->setChecked(true);
    else actWPM->setChecked(true);

    connect(actCPM, &QAction::triggered, this, [this](){ m_speedMetric = CPM; updateMetricsUI(); saveSettings(); });
    connect(actWPM, &QAction::triggered, this, [this](){ m_speedMetric = WPM; updateMetricsUI(); saveSettings(); });

    // Ctrl+Shift+R → повернення на Start-екран (вибір уроку)
    auto *actReturnToMain = new QAction("Return to lesson selection", this);
    actReturnToMain->setShortcut(QKeySequence("Ctrl+Shift+R"));
    actReturnToMain->setShortcutContext(Qt::ApplicationShortcut);
    addAction(actReturnToMain);
    connect(actReturnToMain, &QAction::triggered, this, &MainWindow::onReturnToMainClicked);

    // Кнопки Start-екрану (якщо присутні в .ui)
    if (auto *b = findChild<QPushButton*>("btnRandomLesson"))
        connect(b, &QPushButton::clicked, this, &MainWindow::onRandomLesson);
    if (auto *b = findChild<QPushButton*>("btnReloadLessons"))
        connect(b, &QPushButton::clicked, this, &MainWindow::onReloadLessons);

    // Debug меню (з ПР7)
    QMenu   *menuDebug  = menuBar()->addMenu("Debug");
    QAction *actAdvance = menuDebug->addAction("Next char  [->]");
    actAdvance->setShortcut(Qt::Key_Right);
    connect(actAdvance, &QAction::triggered, this, &MainWindow::onAdvanceChar);

    // Перехоплення клавіш (активується лише під час Training)
    qApp->installEventFilter(this);

    ui->stackedWidget->setCurrentIndex(0);
    ui->lblTime->setText("00:00");
    ui->lblSpeed->setText("0 CPM");
    ui->lblAccuracy->setText("100%");
    ui->progressAccuracy->setValue(100);
    ui->lblResultTime->setText("00:00");
    ui->lblResultSpeed->setText("0 CPM");
    ui->lblResultAccuracy->setText("0%");
}

MainWindow::~MainWindow()
{
    saveSettings();
    if (qApp) qApp->removeEventFilter(this);
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    if (!m_trainingInputEnabled) return QMainWindow::eventFilter(obj, event);
    if (ui->stackedWidget->currentIndex() != 1) return QMainWindow::eventFilter(obj, event);

    if (event->type() != QEvent::KeyPress) return QMainWindow::eventFilter(obj, event);
    auto *ke = static_cast<QKeyEvent*>(event);

    // Не ламати меню-скорочення типу Ctrl+Q / Ctrl+R тощо
    const auto mods = ke->modifiers();
    const bool hasCtrlAlt = (mods.testFlag(Qt::ControlModifier) || mods.testFlag(Qt::AltModifier));
    if (hasCtrlAlt) return QMainWindow::eventFilter(obj, event);

    highlightVirtualKeyForEvent(ke);

    // Службові клавіші
    if (ke->key() == Qt::Key_Backspace) {
        m_model.backspace();
        refreshTrainingDisplay();
        return true;
    }
    if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
        m_model.enter();
        refreshTrainingDisplay();
        return true;
    }

    // Символьний ввід
    const QString t = ke->text();
    if (t.isEmpty()) {
        // Shift/Ctrl/Alt/стрілки тощо — просто ігноруємо, щоб не заважали сесії
        return true;
    }

    QChar ch = t.at(0);

    auto mapCyrillicToQwertyLatin = [](QChar c) -> QChar {
        const QChar lower = c.toLower();
        // Russian layout → physical QWERTY Latin key
        static const QHash<QChar, QChar> m = {
            {u'й', u'q'}, {u'ц', u'w'}, {u'у', u'e'}, {u'к', u'r'}, {u'е', u't'},
            {u'н', u'y'}, {u'г', u'u'}, {u'ш', u'i'}, {u'щ', u'o'}, {u'з', u'p'},
            {u'х', u'['}, {u'ъ', u']'},

            {u'ф', u'a'}, {u'ы', u's'}, {u'в', u'd'}, {u'а', u'f'}, {u'п', u'g'},
            {u'р', u'h'}, {u'о', u'j'}, {u'л', u'k'}, {u'д', u'l'},
            {u'ж', u';'}, {u'э', u'\''},

            {u'я', u'z'}, {u'ч', u'x'}, {u'с', u'c'}, {u'м', u'v'}, {u'и', u'b'},
            {u'т', u'n'}, {u'ь', u'm'}, {u'б', u','}, {u'ю', u'.'},

            {u'ё', u'~'} // approximate to tilde key
        };

        // Ukrainian extras (most common physical positions)
        static const QHash<QChar, QChar> ua = {
            {u'і', u's'},  // usually on S
            {u'ї', u']'},  // often on ]
            {u'є', u'\''}, // often on '
            {u'ґ', u'~'}   // often on `
        };

        if (m.contains(lower)) return m.value(lower);
        if (ua.contains(lower)) return ua.value(lower);
        return QChar();
    };

    // Fallback для розкладок: якщо урок очікує латиницю, а text() дав інший символ,
    // пробуємо відновити латинський символ з key() (A..Z, 0..9).
    const QString expectedStr = m_model.currentChar();
    if (!expectedStr.isEmpty()) {
        const QChar expected = expectedStr.at(0);
        if (ch != expected) {
            // 1) Якщо прийшла кирилиця, але очікується ASCII — мапимо по фізичній клавіші
            if (expected.unicode() <= 0x007F) {
                const QChar mapped = mapCyrillicToQwertyLatin(ch);
                if (!mapped.isNull()) {
                    ch = expected.isUpper() ? mapped.toUpper() : mapped.toLower();
                }
            }

            // 2) Якщо все ще не співпало — пробуємо через key() для A..Z/0..9
            const int k = ke->key();
            if (k >= Qt::Key_A && k <= Qt::Key_Z) {
                QChar latin(static_cast<char16_t>(u'A' + (k - Qt::Key_A)));
                if (!expected.isUpper()) latin = latin.toLower();
                ch = latin;
            } else if (k >= Qt::Key_0 && k <= Qt::Key_9) {
                ch = QChar(static_cast<char16_t>(u'0' + (k - Qt::Key_0)));
            }
        }
    }

    bool correct = false;
    if (m_model.inputChar(ch, &correct)) {
        m_totalTyped++;
        if (correct) m_correctTyped++;
    }
    refreshTrainingDisplay();
    updateMetricsUI();

    if (m_model.isFinished()) {
        m_timer->stop();
        ui->lblResultTime->setText(ui->lblTime->text());
        ui->lblResultSpeed->setText(ui->lblSpeed->text());
        ui->lblResultAccuracy->setText(ui->lblAccuracy->text());
        ui->stackedWidget->setCurrentIndex(2);
        setTrainingInputEnabled(false);
    }

    return true;
}

// ── п.3–4: Сканування + заповнення ComboBox ────────────────────────

void MainWindow::populateComboBox(bool keepSelection)
{
    // Запам'ятати поточний шлях для відновлення (п.7)
    QString prevPath;
    if (keepSelection) {
        int pi = ui->comboLesson->currentIndex();
        if (pi >= 0) prevPath = ui->comboLesson->itemData(pi).toString();
    }

    m_loader.scan();

    ui->comboLesson->blockSignals(true);
    ui->comboLesson->clear();

    const auto &entries = m_loader.entries();

    if (entries.isEmpty()) {
        // п.8: порожній/відсутній каталог
        ui->comboLesson->setEnabled(false);
        ui->btnStartTraining->setEnabled(false);
        ui->lblLessonDescription->setText(
            "No lessons found. Add .txt files to the 'lessons/' folder.");
        ui->comboLesson->blockSignals(false);
        return;
    }

    ui->comboLesson->setEnabled(true);
    ui->btnStartTraining->setEnabled(true);

    int restoreIndex = 0;
    for (int i = 0; i < entries.size(); ++i) {
        // п.4: назва + шлях як userData
        ui->comboLesson->addItem(entries[i].displayName, entries[i].filePath);
        if (keepSelection && entries[i].filePath == prevPath)
            restoreIndex = i;
    }

    ui->comboLesson->blockSignals(false);
    ui->comboLesson->setCurrentIndex(restoreIndex);
    loadLessonByIndex(restoreIndex);
}

// ── п.5: Завантаження уроку з файлу ────────────────────────────────

void MainWindow::loadLessonByIndex(int index)
{
    if (!m_loader.hasLessons() || index < 0 || index >= m_loader.count()) {
        m_model.loadText(QString());
        ui->lblLessonDescription->setText("No lesson loaded.");
        return;
    }

    QString text, error;
    if (!m_loader.loadText(index, text, error)) {
        // п.8: файл не відкрився — не падаємо
        QMessageBox::warning(this, "Lesson load error",
            QString("Could not load lesson:\n%1").arg(error));
        m_model.loadText(QString());
        ui->lblLessonDescription->setText("Failed to load lesson.");
        return;
    }

    m_model.loadText(text);  // скидає lineIndex/charIndex (п.5)

    const auto &e = m_loader.entries().at(index);
    ui->lblLessonDescription->setText(
        QString("%1\nFile: %2  |  Size: %3 bytes")
            .arg(e.displayName)
            .arg(e.filePath.section('/', -1))
            .arg(e.fileSize));
}

// ── п.6: Випадковий урок ────────────────────────────────────────────

void MainWindow::onRandomLesson()
{
    if (!m_loader.hasLessons()) {
        QMessageBox::information(this, "Random lesson",
            "No lessons available. Add .txt files to 'lessons/' first.");
        return;
    }

    int count   = m_loader.count();
    int current = ui->comboLesson->currentIndex();
    int random  = current;

    if (count > 1) {
        while (random == current)
            random = static_cast<int>(QRandomGenerator::global()->bounded(count));
    }

    // Встановлення в ComboBox викличе onLessonChanged -> loadLessonByIndex
    ui->comboLesson->setCurrentIndex(random);

    // Якщо ми вже на екрані тренування — одразу запускаємо новий урок
    if (ui->stackedWidget->currentIndex() == 1) {
        onStartTrainingClicked();
    }
}

// ── п.7: Перезавантаження списку уроків ────────────────────────────

void MainWindow::onReloadLessons()
{
    populateComboBox(true);  // Оновлює список і завантажує текст у модель

    // Якщо ми зараз на екрані тренування — оновлюємо UI, щоб побачити зміни в тексті
    if (ui->stackedWidget->currentIndex() == 1) {
        m_model.reset(); // Скидаємо прогрес на початок оновленого тексту
        m_elapsedSeconds = 0;
        m_totalTyped = 0;
        m_correctTyped = 0;
        ui->lblTime->setText("00:00");
        updateMetricsUI();
        refreshTrainingDisplay();
    }

    if (statusBar()) {
        statusBar()->showMessage(QString("Lessons reloaded. Found %1 lesson(s).").arg(m_loader.count()), 3000);
    }
}

// ── Зміна вибору в ComboBox ────────────────────────────────────────

void MainWindow::onLessonChanged(int index)
{
    loadLessonByIndex(index);
    saveSettings();
}

void MainWindow::onTimerTick()
{
    m_elapsedSeconds++;
    int minutes = m_elapsedSeconds / 60;
    int seconds = m_elapsedSeconds % 60;
    ui->lblTime->setText(QString("%1:%2")
                         .arg(minutes, 2, 10, QChar('0'))
                         .arg(seconds, 2, 10, QChar('0')));

    updateMetricsUI();
}

void MainWindow::updateMetricsUI()
{
    // Speed
    double speed = 0;
    if (m_elapsedSeconds > 0) {
        if (m_speedMetric == CPM) {
            speed = (m_totalTyped * 60.0) / m_elapsedSeconds;
        } else {
            // WPM: 5 chars = 1 word
            speed = (m_totalTyped / 5.0 * 60.0) / m_elapsedSeconds;
        }
    }

    QString unit = (m_speedMetric == CPM) ? "CPM" : "WPM";
    ui->lblSpeed->setText(QString("%1 %2").arg(qRound(speed)).arg(unit));

    // Accuracy
    int accuracy = 100;
    if (m_totalTyped > 0) {
        accuracy = qRound((m_correctTyped * 100.0) / m_totalTyped);
    }
    ui->lblAccuracy->setText(QString("%1%").arg(accuracy));
    ui->progressAccuracy->setValue(accuracy);
}

void MainWindow::loadSettings()
{
    QSettings settings("TypingTrainer", "TypingTrainer");
    QString lastPath = settings.value("lastLessonPath").toString();
    if (!lastPath.isEmpty()) {
        for (int i = 0; i < ui->comboLesson->count(); ++i) {
            if (ui->comboLesson->itemData(i).toString() == lastPath) {
                ui->comboLesson->blockSignals(true);
                ui->comboLesson->setCurrentIndex(i);
                loadLessonByIndex(i);
                ui->comboLesson->blockSignals(false);
                break;
            }
        }
    }

    int metric = settings.value("speedMetric", CPM).toInt();
    m_speedMetric = static_cast<SpeedMetric>(metric);
}

void MainWindow::saveSettings()
{
    QSettings settings("TypingTrainer", "TypingTrainer");
    int idx = ui->comboLesson->currentIndex();
    if (idx >= 0) {
        settings.setValue("lastLessonPath", ui->comboLesson->itemData(idx).toString());
    }
    settings.setValue("speedMetric", static_cast<int>(m_speedMetric));
}

// ── Навігація між екранами ─────────────────────────────────────────

void MainWindow::onStartTrainingClicked()
{
    if (!m_loader.hasLessons()) return;

    m_model.reset();
    m_elapsedSeconds = 0;
    m_totalTyped = 0;
    m_correctTyped = 0;
    m_timer->stop(); // Ensure it's stopped before starting
    m_timer->start();

    ui->lblTime->setText("00:00");
    updateMetricsUI();
    ui->stackedWidget->setCurrentIndex(1);
    setTrainingInputEnabled(true);
    ui->pageTraining->setFocusPolicy(Qt::StrongFocus);
    ui->pageTraining->setFocus(Qt::OtherFocusReason);
    refreshTrainingDisplay();
}

void MainWindow::onRestartTrainingClicked() { onStartTrainingClicked(); }
void MainWindow::onReturnToMainClicked()
{
    m_timer->stop();
    ui->stackedWidget->setCurrentIndex(0);
    setTrainingInputEnabled(false);
}

// ── Training UI ────────────────────────────────────────────────────

void MainWindow::refreshTrainingDisplay()
{
    QString prevLine  = m_model.previousLine();
    QString curLine   = m_model.currentLine();

    QString html =
        "<html><body style=\"font-family:'Courier New',monospace;"
        "font-size:17px;background:#181825;color:#cdd6f4;\">";

    if (!prevLine.isEmpty()) {
        html += "<p style=\"background:#2a2a3e;color:#a6adc8;padding:4px 8px;margin:2px 0;\">";
        for (QChar c : prevLine)
            html += (c == ' ') ? "&nbsp;" : QString(c).toHtmlEscaped();
        html += "</p>";
    }

    if (!m_model.isFinished()) {
        html += "<p style=\"background:#1e1e2e;padding:4px 8px;margin:2px 0;\">";
        const int li = m_model.lineIndex();
        const int ci = m_model.charIndex();
        for (int j = 0; j < curLine.length(); ++j) {
            const QChar c = curLine.at(j);
            const QString ch = (c == ' ') ? "&nbsp;" : QString(c).toHtmlEscaped();

            if (j < ci) {
                const auto st = m_model.stateAt(li, j);
                if (st == LessonModel::CharState::Correct) {
                    html += "<span style=\"background:#1e4429;color:#a6e3a1;\">" + ch + "</span>";
                } else if (st == LessonModel::CharState::Wrong) {
                    html += "<span style=\"background:#f38ba8;color:#1e1e2e;\">" + ch + "</span>";
                } else {
                    html += "<span style=\"background:#313244;color:#a6adc8;\">" + ch + "</span>";
                }
                continue;
            }

            if (j == ci) {
                html += "<span style=\"background:#cba6f7;color:#1e1e2e;padding:0 2px;\">"
                        + ch + "</span>";
                continue;
            }

            html += ch;
        }
        // Якщо кінець рядка — показати підказку, що чекаємо Enter
        if (!curLine.isEmpty() && m_model.atEndOfLine()) {
            html += "<span style=\"color:#a6adc8;padding-left:6px;\">⏎</span>";
        }
        html += "</p>";

        for (int i = m_model.lineIndex() + 1; i < m_model.lineCount(); ++i) {
            html += "<p style=\"color:#585b70;padding:4px 8px;margin:2px 0;\">";
            for (QChar c : m_model.lineAt(i))
                html += (c == ' ') ? "&nbsp;" : QString(c).toHtmlEscaped();
            html += "</p>";
        }
    } else {
        html += "<p style=\"color:#a6e3a1;padding:8px;font-size:16px;"
                "text-align:center;\">Lesson complete!</p>";
    }

    html += "</body></html>";
    ui->textDisplay->setHtml(html);
}

void MainWindow::setTrainingInputEnabled(bool enabled)
{
    m_trainingInputEnabled = enabled;
    if (!enabled) clearVirtualKeyHighlight();
}

static const QString KEY_HIGHLIGHT =
    "QPushButton {"
    "  background-color: #cba6f7;"
    "  color: #1e1e2e;"
    "  border: 2px solid #d0b4fe;"
    "  border-radius: 4px;"
    "  font-size: 12px;"
    "  font-weight: bold;"
    "}";

void MainWindow::clearVirtualKeyHighlight()
{
    if (!m_lastHighlightedKey) return;
    m_lastHighlightedKey->setStyleSheet(m_lastHighlightedKeyStyle);
    m_lastHighlightedKey = nullptr;
    m_lastHighlightedKeyStyle.clear();
}

void MainWindow::highlightVirtualKeyByObjectName(const QString &objectName)
{
    auto *btn = findChild<QPushButton*>(objectName);
    if (!btn) return;
    if (m_lastHighlightedKey == btn) return;
    clearVirtualKeyHighlight();
    m_lastHighlightedKey = btn;
    m_lastHighlightedKeyStyle = btn->styleSheet();
    btn->setStyleSheet(KEY_HIGHLIGHT);
}

void MainWindow::highlightVirtualKeyForChar(QChar ch)
{
    if (ch == ' ') { highlightVirtualKeyByObjectName("key_space"); return; }

    const QString s = QString(ch);
    const QChar u = s.toUpper().at(0);

    if (u.isLetterOrNumber()) {
        highlightVirtualKeyByObjectName(QString("key_%1").arg(u));
        return;
    }

    // Пунктуація / символи, які мають власні objectName
    switch (ch.unicode()) {
        case '~': highlightVirtualKeyByObjectName("key_tilde"); return;
        case '-': highlightVirtualKeyByObjectName("key_minus"); return;
        case '=': highlightVirtualKeyByObjectName("key_equals"); return;
        case '[': highlightVirtualKeyByObjectName("key_lbracket"); return;
        case ']': highlightVirtualKeyByObjectName("key_rbracket"); return;
        case '\\': highlightVirtualKeyByObjectName("key_bslash"); return;
        case ';': highlightVirtualKeyByObjectName("key_semi"); return;
        case '\'': highlightVirtualKeyByObjectName("key_quote"); return;
        case ',': highlightVirtualKeyByObjectName("key_comma"); return;
        case '.': highlightVirtualKeyByObjectName("key_dot"); return;
        case '/': highlightVirtualKeyByObjectName("key_slash"); return;
        default: break;
    }
}

void MainWindow::highlightVirtualKeyForEvent(QKeyEvent *ke)
{
    if (!ke) return;
    if (ke->key() == Qt::Key_Backspace) { highlightVirtualKeyByObjectName("key_backspace"); return; }
    if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) { highlightVirtualKeyByObjectName("key_enter"); return; }

    const int k = ke->key();
    if (k >= Qt::Key_A && k <= Qt::Key_Z) {
        highlightVirtualKeyByObjectName(QString("key_%1").arg(QChar(static_cast<char16_t>(u'A' + (k - Qt::Key_A)))));
        return;
    }
    if (k >= Qt::Key_0 && k <= Qt::Key_9) {
        highlightVirtualKeyByObjectName(QString("key_%1").arg(QChar(static_cast<char16_t>(u'0' + (k - Qt::Key_0)))));
        return;
    }
    const QString t = ke->text();
    if (!t.isEmpty()) highlightVirtualKeyForChar(t.at(0));
}

// ── Debug ──────────────────────────────────────────────────────────

void MainWindow::onAdvanceChar()
{
    if (ui->stackedWidget->currentIndex() != 1) return;

    if (m_model.isFinished()) {
        m_timer->stop();
        ui->lblResultTime->setText(ui->lblTime->text());
        ui->lblResultSpeed->setText(ui->lblSpeed->text());
        ui->lblResultAccuracy->setText(ui->lblAccuracy->text());
        ui->stackedWidget->setCurrentIndex(2);
        return;
    }

    m_model.advance();
    refreshTrainingDisplay();

    if (m_model.isFinished()) {
        m_timer->stop();
        ui->lblResultTime->setText(ui->lblTime->text());
        ui->lblResultSpeed->setText(ui->lblSpeed->text());
        ui->lblResultAccuracy->setText(ui->lblAccuracy->text());
        ui->stackedWidget->setCurrentIndex(2);
    }
}

// ── Меню ───────────────────────────────────────────────────────────

void MainWindow::onAboutClicked()
{
    QMessageBox::about(this, "About TypingTrainer",
        "<h3>TypingTrainer v1.0 PR8</h3>"
        "<p>Lessons are loaded from the <b>lessons/</b> folder "
        "next to the executable.</p>"
        "<ul>"
        "<li><b>Lessons -> Random lesson</b> (Ctrl+R)</li>"
        "<li><b>Lessons -> Reload lessons</b> (F5)</li>"
        "<li><b>Return to lesson selection</b> (Ctrl+Shift+R)</li>"
        "<li><b>Debug -> Next char</b> (->)</li>"
        "</ul>");
}

void MainWindow::onExitClicked() { close(); }
