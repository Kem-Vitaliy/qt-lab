#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QComboBox>
#include <QLineEdit>
#include <QSignalBlocker>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , numberValidator(new QDoubleValidator(this))
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupValidators();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupValidators()
{
    numberValidator->setNotation(QDoubleValidator::StandardNotation);
    numberValidator->setDecimals(10);

    ui->editLengthTop->setValidator(numberValidator);
    ui->editLengthBottom->setValidator(numberValidator);
    ui->editMassTop->setValidator(numberValidator);
    ui->editMassBottom->setValidator(numberValidator);
    ui->editTemperatureTop->setValidator(numberValidator);
    ui->editTemperatureBottom->setValidator(numberValidator);
}

void MainWindow::setupConnections()
{
    // Text edits (user input)
    connect(ui->editLengthTop, &QLineEdit::textEdited, this, &MainWindow::onTopEdited);
    connect(ui->editLengthBottom, &QLineEdit::textEdited, this, &MainWindow::onBottomEdited);
    connect(ui->editMassTop, &QLineEdit::textEdited, this, &MainWindow::onTopEdited);
    connect(ui->editMassBottom, &QLineEdit::textEdited, this, &MainWindow::onBottomEdited);
    connect(ui->editTemperatureTop, &QLineEdit::textEdited, this, &MainWindow::onTopEdited);
    connect(ui->editTemperatureBottom, &QLineEdit::textEdited, this, &MainWindow::onBottomEdited);

    // Unit changes
    connect(ui->comboLengthTopUnit, &QComboBox::currentIndexChanged, this, &MainWindow::onUnitChanged);
    connect(ui->comboLengthBottomUnit, &QComboBox::currentIndexChanged, this, &MainWindow::onUnitChanged);
    connect(ui->comboMassTopUnit, &QComboBox::currentIndexChanged, this, &MainWindow::onUnitChanged);
    connect(ui->comboMassBottomUnit, &QComboBox::currentIndexChanged, this, &MainWindow::onUnitChanged);
    connect(ui->comboTemperatureTopUnit, &QComboBox::currentIndexChanged, this, &MainWindow::onUnitChanged);
    connect(ui->comboTemperatureBottomUnit, &QComboBox::currentIndexChanged, this, &MainWindow::onUnitChanged);

    // Tab changed
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

void MainWindow::onTopEdited(const QString &)
{
    lastEditedField = ActiveField::Top;
    handleEdit(ActiveField::Top);
}

void MainWindow::onBottomEdited(const QString &)
{
    lastEditedField = ActiveField::Bottom;
    handleEdit(ActiveField::Bottom);
}

void MainWindow::onUnitChanged(int)
{
    recalculateForCurrentTab(lastEditedField);
}

void MainWindow::onTabChanged(int)
{
    recalculateForCurrentTab(lastEditedField);
}

void MainWindow::handleEdit(ActiveField sourceField)
{
    recalculateForCurrentTab(sourceField);
}

Converter::Mode MainWindow::currentMode() const
{
    switch (ui->tabWidget->currentIndex()) {
    case 0: return Converter::Mode::Length;
    case 1: return Converter::Mode::Mass;
    case 2: return Converter::Mode::Temperature;
    default: return Converter::Mode::Length;
    }
}

void MainWindow::recalculateForCurrentTab(ActiveField sourceField)
{
    Converter::Mode mode = currentMode();

    QLineEdit *sourceEdit = nullptr;
    QLineEdit *targetEdit = nullptr;
    QComboBox *sourceCombo = nullptr;
    QComboBox *targetCombo = nullptr;

    if (mode == Converter::Mode::Length) {
        if (sourceField == ActiveField::Top) {
            sourceEdit = ui->editLengthTop;
            targetEdit = ui->editLengthBottom;
            sourceCombo = ui->comboLengthTopUnit;
            targetCombo = ui->comboLengthBottomUnit;
        } else {
            sourceEdit = ui->editLengthBottom;
            targetEdit = ui->editLengthTop;
            sourceCombo = ui->comboLengthBottomUnit;
            targetCombo = ui->comboLengthTopUnit;
        }
    } else if (mode == Converter::Mode::Mass) {
        if (sourceField == ActiveField::Top) {
            sourceEdit = ui->editMassTop;
            targetEdit = ui->editMassBottom;
            sourceCombo = ui->comboMassTopUnit;
            targetCombo = ui->comboMassBottomUnit;
        } else {
            sourceEdit = ui->editMassBottom;
            targetEdit = ui->editMassTop;
            sourceCombo = ui->comboMassBottomUnit;
            targetCombo = ui->comboMassTopUnit;
        }
    } else { // Temperature
        if (sourceField == ActiveField::Top) {
            sourceEdit = ui->editTemperatureTop;
            targetEdit = ui->editTemperatureBottom;
            sourceCombo = ui->comboTemperatureTopUnit;
            targetCombo = ui->comboTemperatureBottomUnit;
        } else {
            sourceEdit = ui->editTemperatureBottom;
            targetEdit = ui->editTemperatureTop;
            sourceCombo = ui->comboTemperatureBottomUnit;
            targetCombo = ui->comboTemperatureTopUnit;
        }
    }

    if (!sourceEdit || !targetEdit || !sourceCombo || !targetCombo) {
        return;
    }

    QString text = sourceEdit->text();

    int pos = 0;
    QString copy = text;
    if (numberValidator->validate(copy, pos) != QValidator::Acceptable || text.isEmpty()) {
        markError(sourceEdit, true);
        return;
    }

    markError(sourceEdit, false);

    bool ok = false;
    double value = text.toDouble(&ok);
    if (!ok) {
        markError(sourceEdit, true);
        return;
    }

    double result = converter.convert(mode,
                                      sourceCombo->currentIndex(),
                                      targetCombo->currentIndex(),
                                      value);

    QString formatted = formatValue(result);
    QSignalBlocker blocker(targetEdit);
    targetEdit->setText(formatted);
}

QString MainWindow::formatValue(double value) const
{
    return QString::number(value, 'f', 4);
}

void MainWindow::markError(QWidget *widget, bool error)
{
    if (error) {
        widget->setStyleSheet("QLineEdit { background-color: #ffcccc; }");
    } else {
        widget->setStyleSheet("");
    }
}

