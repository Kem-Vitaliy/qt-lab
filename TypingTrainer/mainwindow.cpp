#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QAction>
#include <QRandomGenerator>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("TypingTrainer");
    resize(860, 620);

    populateComboBox();

    connect(ui->actionExit,  &QAction::triggered, this, &MainWindow::onExitClicked);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAboutClicked);

    connect(ui->btnStartTraining,
            &QPushButton::clicked, this, &MainWindow::onStartTrainingClicked);
    connect(ui->btnRestartTraining,
            &QPushButton::clicked, this, &MainWindow::onRestartTrainingClicked);
    connect(ui->btnReturnToMain,
            &QPushButton::clicked, this, &MainWindow::onReturnToMainClicked);

    connect(ui->comboLesson,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onLessonChanged);

    // Меню Lessons: Random + Reload (п.6, п.7)
    QMenu   *menuLessons = menuBar()->addMenu("Lessons");
    QAction *actRandom   = menuLessons->addAction("Random lesson");
    QAction *actReload   = menuLessons->addAction("Reload lessons");
    actRandom->setShortcut(QKeySequence("Ctrl+R"));
    actReload->setShortcut(QKeySequence("Ctrl+Shift+R"));
    connect(actRandom, &QAction::triggered, this, &MainWindow::onRandomLesson);
    connect(actReload, &QAction::triggered, this, &MainWindow::onReloadLessons);

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
    delete ui;
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
        // п.8: 0 уроків — нічого не робимо
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

    // Встановлення в ComboBox викличе onLessonChanged → loadLessonByIndex
    ui->comboLesson->setCurrentIndex(random);
}

// ── п.7: Перезавантаження списку уроків ────────────────────────────

void MainWindow::onReloadLessons()
{
    populateComboBox(true);  // keepSelection = true
    QMessageBox::information(this, "Lessons reloaded",
        QString("Found %1 lesson(s) in:\n%2")
            .arg(m_loader.count())
            .arg(m_loader.lessonsDir()));
}

// ── Зміна вибору в ComboBox ────────────────────────────────────────

void MainWindow::onLessonChanged(int index)
{
    loadLessonByIndex(index);
}

// ── Навігація між екранами ─────────────────────────────────────────

void MainWindow::onStartTrainingClicked()
{
    if (!m_loader.hasLessons()) return;

    m_model.reset();
    ui->lblTime->setText("00:00");
    ui->lblSpeed->setText("0 CPM");
    ui->lblAccuracy->setText("100%");
    ui->progressAccuracy->setValue(100);
    ui->stackedWidget->setCurrentIndex(1);
    refreshTrainingDisplay();
}

void MainWindow::onRestartTrainingClicked() { onStartTrainingClicked(); }
void MainWindow::onReturnToMainClicked()    { ui->stackedWidget->setCurrentIndex(0); }

// ── Training UI ────────────────────────────────────────────────────

void MainWindow::refreshTrainingDisplay()
{
    QString prevLine  = m_model.previousLine();
    QString typed     = m_model.typedPart();
    QString curChar   = m_model.currentChar();
    QString remaining = m_model.remainingPart();

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
        for (QChar c : typed) {
            QString ch = (c == ' ') ? "&nbsp;" : QString(c).toHtmlEscaped();
            html += "<span style=\"background:#1e4429;color:#a6e3a1;\">" + ch + "</span>";
        }
        if (!curChar.isEmpty()) {
            QString ch = (curChar[0] == ' ') ? "&nbsp;" : curChar.toHtmlEscaped();
            html += "<span style=\"background:#f38ba8;color:#1e1e2e;padding:0 2px;\">"
                    + ch + "</span>";
        }
        for (QChar c : remaining)
            html += (c == ' ') ? "&nbsp;" : QString(c).toHtmlEscaped();
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

// ── Debug ──────────────────────────────────────────────────────────

void MainWindow::onAdvanceChar()
{
    if (ui->stackedWidget->currentIndex() != 1) return;

    if (m_model.isFinished()) {
        ui->lblResultTime->setText(ui->lblTime->text());
        ui->lblResultSpeed->setText(ui->lblSpeed->text());
        ui->lblResultAccuracy->setText(ui->lblAccuracy->text());
        ui->stackedWidget->setCurrentIndex(2);
        return;
    }

    m_model.advance();
    refreshTrainingDisplay();

    if (m_model.isFinished()) {
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
        "<li><b>Lessons -> Reload lessons</b> (Ctrl+Shift+R)</li>"
        "<li><b>Debug -> Next char</b> (->)</li>"
        "</ul>");
}

void MainWindow::onExitClicked() { close(); }
