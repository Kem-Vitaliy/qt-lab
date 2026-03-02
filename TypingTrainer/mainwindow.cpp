#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("TypingTrainer");
    resize(700, 520);

    setupLessons();

    // Connect menu actions
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onExitClicked);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAboutClicked);

    // Connect screen buttons
    connect(ui->btnStartTraining, &QPushButton::clicked, this, &MainWindow::onStartTrainingClicked);
    connect(ui->btnRestartTraining, &QPushButton::clicked, this, &MainWindow::onRestartTrainingClicked);
    connect(ui->btnReturnToMain, &QPushButton::clicked, this, &MainWindow::onReturnToMainClicked);

    // Connect combo box
    connect(ui->comboLesson, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onLessonChanged);

    // Start on the Start screen (index 0)
    ui->stackedWidget->setCurrentIndex(0);

    // Initialize session info labels
    ui->lblTime->setText("00:00");
    ui->lblSpeed->setText("0 CPM");
    ui->lblAccuracy->setText("100%");

    // Initialize result labels
    ui->lblResultTime->setText("00:00");
    ui->lblResultSpeed->setText("0 CPM");
    ui->lblResultAccuracy->setText("0%");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupLessons()
{
    ui->comboLesson->clear();
    ui->comboLesson->addItem("Starter Text");
    ui->comboLesson->addItem("Numbers and Symbols");
    ui->comboLesson->addItem("Common Words Part 1");
    ui->comboLesson->addItem("Common Words Part 2");
    ui->comboLesson->addItem("Python Code Sample");
    ui->comboLesson->addItem("Lorem Ipsum");

    updateLessonDescription(0);
}

void MainWindow::updateLessonDescription(int index)
{
    QStringList descriptions = {
        "Basic home row exercise. Practice the fundamental finger positions: a s d f j k l ;",
        "Practice typing numbers 0-9 and common symbols: ! @ # $ % ^ & * ( )",
        "Frequently used English words — short and medium length.",
        "More common words, longer sequences for fluency building.",
        "Python code snippets to practice programming-style typing.",
        "Classic Lorem Ipsum placeholder text for general typing practice."
    };

    if (index >= 0 && index < descriptions.size())
        ui->lblLessonDescription->setText(descriptions[index]);
}

void MainWindow::onStartTrainingClicked()
{
    // Switch to Training screen
    ui->stackedWidget->setCurrentIndex(1);
    ui->lblTime->setText("00:00");
    ui->lblSpeed->setText("0 CPM");
    ui->lblAccuracy->setText("100%");

    // Load sample text into the text display area based on selected lesson
    QStringList sampleTexts = {
        "jfj j jff ffjjjj fjjf fjjj jjjjjj\njfj j jff ffjjjj Fjn ffj",
        "1234567890 !@#$%^&*()\n0987654321 )(*&^%$#@!",
        "the quick brown fox jumps over the lazy dog\na stitch in time saves nine",
        "practice makes perfect every single day\nhard work always pays off in the end",
        "def hello():\n    print(\"Hello, World!\")\n    return True",
        "Lorem ipsum dolor sit amet, consectetur\nadipiscing elit, sed do eiusmod tempor"
    };

    int idx = ui->comboLesson->currentIndex();
    if (idx >= 0 && idx < sampleTexts.size())
        ui->textDisplay->setPlainText(sampleTexts[idx]);
}

void MainWindow::onRestartTrainingClicked()
{
    onStartTrainingClicked();
}

void MainWindow::onReturnToMainClicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::onAboutClicked()
{
    QMessageBox::about(this, "About TypingTrainer",
        "<h3>TypingTrainer v1.0</h3>"
        "<p>A typing practice application built with Qt Widgets.</p>"
        "<p>Improve your typing speed and accuracy!</p>");
}

void MainWindow::onExitClicked()
{
    close();
}

void MainWindow::onLessonChanged(int index)
{
    updateLessonDescription(index);
}
