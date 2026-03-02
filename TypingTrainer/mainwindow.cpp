#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QAction>

// ═══════════════════════════════════════════════════════════════════
// Конструктор / деструктор
// ═══════════════════════════════════════════════════════════════════

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("TypingTrainer");
    resize(860, 620);

    // ── Заповнити комбо-бокс уроками ──────────────────────────────
    setupLessons();

    // ── Меню ──────────────────────────────────────────────────────
    connect(ui->actionExit,  &QAction::triggered, this, &MainWindow::onExitClicked);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAboutClicked);

    // ── Кнопки навігації ──────────────────────────────────────────
    connect(ui->btnStartTraining,
            &QPushButton::clicked, this, &MainWindow::onStartTrainingClicked);
    connect(ui->btnRestartTraining,
            &QPushButton::clicked, this, &MainWindow::onRestartTrainingClicked);
    connect(ui->btnReturnToMain,
            &QPushButton::clicked, this, &MainWindow::onReturnToMainClicked);

    // ── Зміна вибраного уроку ─────────────────────────────────────
    connect(ui->comboLesson,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onLessonChanged);

    // ── Тестова дія "Advance" (п.8) ───────────────────────────────
    // Додаємо пункт меню «Debug → Next char» і кнопку в Training
    QMenu   *menuDebug = menuBar()->addMenu("Debug");
    QAction *actAdvance = menuDebug->addAction("Next char  [→]");
    actAdvance->setShortcut(Qt::Key_Right);
    connect(actAdvance, &QAction::triggered, this, &MainWindow::onAdvanceChar);

    // Також підключаємо кнопку btnAdvance якщо є в UI
    // (вона є в .ui — key_enter тут НЕ підключаємо, щоб не заважати)

    // ── Початковий екран ──────────────────────────────────────────
    ui->stackedWidget->setCurrentIndex(0);

    // Ініціалізуємо модель першим уроком
    auto lessons = LessonModel::allLessons();
    if (!lessons.isEmpty())
        m_model.loadText(lessons.first().text);

    // Ініціалізуємо статусні мітки
    ui->lblTime->setText("00:00");
    ui->lblSpeed->setText("0 CPM");
    ui->lblAccuracy->setText("100%");
    ui->progressAccuracy->setValue(100);

    // Ініціалізуємо мітки результатів
    ui->lblResultTime->setText("00:00");
    ui->lblResultSpeed->setText("0 CPM");
    ui->lblResultAccuracy->setText("0%");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ═══════════════════════════════════════════════════════════════════
// Ініціалізація уроків
// ═══════════════════════════════════════════════════════════════════

void MainWindow::setupLessons()
{
    ui->comboLesson->blockSignals(true);
    ui->comboLesson->clear();

    const auto lessons = LessonModel::allLessons();
    for (const auto &lesson : lessons)
        ui->comboLesson->addItem(lesson.title);

    ui->comboLesson->blockSignals(false);

    // Показати опис першого уроку
    if (!lessons.isEmpty())
        ui->lblLessonDescription->setText(lessons.first().description);
}

// ═══════════════════════════════════════════════════════════════════
// Оновлення Training-екрана з моделі
// ═══════════════════════════════════════════════════════════════════

void MainWindow::refreshTrainingDisplay()
{
    // ── Попередній рядок ────────────────────────────────────────────
    QString prevLine = m_model.previousLine();  // "" якщо ми на рядку 0

    // ── Поточний рядок: три фрагменти ──────────────────────────────
    QString typed     = m_model.typedPart();    // вже пройдено
    QString curChar   = m_model.currentChar();  // поточний символ
    QString remaining = m_model.remainingPart();// залишок

    // ── Будуємо HTML для textDisplay ───────────────────────────────
    QString html =
        "<html><body style=\""
        "font-family:'Courier New',monospace;"
        "font-size:17px;"
        "background:#181825;"
        "color:#cdd6f4;\">";

    // Попередній рядок (завершений, приглушений фон)
    if (!prevLine.isEmpty()) {
        html += "<p style=\"background:#2a2a3e;color:#a6adc8;"
                "padding:4px 8px;margin:2px 0;\">";
        for (QChar c : prevLine)
            html += (c == ' ') ? "&nbsp;" : QString(c).toHtmlEscaped();
        html += "</p>";
    }

    // Поточний рядок з підсвічуванням
    if (!m_model.isFinished()) {
        html += "<p style=\"background:#1e1e2e;padding:4px 8px;margin:2px 0;\">";

        // Вже набрана частина — зеленим
        for (QChar c : typed) {
            QString ch = (c == ' ') ? "&nbsp;" : QString(c).toHtmlEscaped();
            html += "<span style=\"background:#1e4429;color:#a6e3a1;\">" + ch + "</span>";
        }

        // Поточний символ — червоним
        if (!curChar.isEmpty()) {
            QString ch = (curChar[0] == ' ') ? "&nbsp;" : curChar.toHtmlEscaped();
            html += "<span style=\"background:#f38ba8;color:#1e1e2e;"
                    "padding:0 2px;\">" + ch + "</span>";
        }

        // Залишок — звичайним кольором
        for (QChar c : remaining) {
            QString ch = (c == ' ') ? "&nbsp;" : QString(c).toHtmlEscaped();
            html += ch;
        }

        html += "</p>";

        // Наступні рядки (майбутні) — приглушені
        for (int i = m_model.lineIndex() + 1; i < m_model.lineCount(); ++i) {
            QString line = m_model.lineAt(i);
            html += "<p style=\"color:#585b70;padding:4px 8px;margin:2px 0;\">";
            for (QChar c : line)
                html += (c == ' ') ? "&nbsp;" : QString(c).toHtmlEscaped();
            html += "</p>";
        }
    } else {
        // Текст пройдено повністю
        html += "<p style=\"color:#a6e3a1;padding:8px;"
                "font-size:16px;text-align:center;\">✔ Lesson complete!</p>";
    }

    html += "</body></html>";
    ui->textDisplay->setHtml(html);
}

// ═══════════════════════════════════════════════════════════════════
// Навігація між екранами
// ═══════════════════════════════════════════════════════════════════

void MainWindow::onStartTrainingClicked()
{
    // Скинути позицію і перейти на Training
    m_model.reset();
    ui->lblTime->setText("00:00");
    ui->lblSpeed->setText("0 CPM");
    ui->lblAccuracy->setText("100%");
    ui->progressAccuracy->setValue(100);

    ui->stackedWidget->setCurrentIndex(1);
    refreshTrainingDisplay();
}

void MainWindow::onRestartTrainingClicked()
{
    onStartTrainingClicked();
}

void MainWindow::onReturnToMainClicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

// ═══════════════════════════════════════════════════════════════════
// Start-екран: зміна уроку
// ═══════════════════════════════════════════════════════════════════

void MainWindow::onLessonChanged(int index)
{
    auto lessons = LessonModel::allLessons();
    if (index < 0 || index >= lessons.size())
        return;

    // Оновити опис
    ui->lblLessonDescription->setText(lessons[index].description);

    // Завантажити нові текст і скинути позицію на початок (п.7)
    m_model.loadText(lessons[index].text);
}

// ═══════════════════════════════════════════════════════════════════
// Тестова дія: просування на один символ (п.8)
// ═══════════════════════════════════════════════════════════════════

void MainWindow::onAdvanceChar()
{
    // Дія активна лише якщо ми на Training-екрані
    if (ui->stackedWidget->currentIndex() != 1)
        return;

    if (m_model.isFinished()) {
        // Показати екран результатів
        ui->lblResultTime->setText(ui->lblTime->text());
        ui->lblResultSpeed->setText(ui->lblSpeed->text());
        ui->lblResultAccuracy->setText(ui->lblAccuracy->text());
        ui->stackedWidget->setCurrentIndex(2);
        return;
    }

    // Просуваємо позицію на 1 символ (або на наступний рядок)
    m_model.advance();

    // Оновлюємо відображення
    refreshTrainingDisplay();

    // Якщо щойно закінчили — перейти на Results
    if (m_model.isFinished()) {
        ui->lblResultTime->setText(ui->lblTime->text());
        ui->lblResultSpeed->setText(ui->lblSpeed->text());
        ui->lblResultAccuracy->setText(ui->lblAccuracy->text());
        ui->stackedWidget->setCurrentIndex(2);
    }
}

// ═══════════════════════════════════════════════════════════════════
// Меню
// ═══════════════════════════════════════════════════════════════════

void MainWindow::onAboutClicked()
{
    QMessageBox::about(this, "About TypingTrainer",
        "<h3>TypingTrainer v1.0</h3>"
        "<p>A typing practice application built with Qt Widgets.</p>"
        "<p><b>Debug → Next char</b> or <b>→</b> key advances the cursor.</p>");
}

void MainWindow::onExitClicked()
{
    close();
}
