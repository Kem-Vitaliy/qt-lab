#include "ticketdialog.h"
#include "ui_ticketdialog.h"

TicketDialog::TicketDialog(Mode mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TicketDialog),
    m_mode(mode)
{
    ui->setupUi(this);
    setMode(m_mode);
}

TicketDialog::~TicketDialog()
{
    delete ui;
}

void TicketDialog::setMode(Mode mode)
{
    m_mode = mode;
    updateUIForMode();
}

void TicketDialog::updateUIForMode()
{
    bool isReadOnly = (m_mode == Mode::View);
    
    // Set field states
    ui->lineEditTitle->setReadOnly(isReadOnly);
    ui->plainTextEditDescription->setReadOnly(isReadOnly);
    ui->comboBoxStatus->setEnabled(!isReadOnly);
    ui->comboBoxPriority->setEnabled(!isReadOnly);

    // Set visibility/text for buttons
    if (m_mode == Mode::View) {
        ui->btnEdit->show();
        ui->btnSave->hide();
        ui->btnCancel->hide();
        ui->btnClose->show();
        setWindowTitle("View Ticket");
    } else {
        ui->btnEdit->hide();
        ui->btnSave->show();
        ui->btnCancel->show();
        ui->btnClose->hide();
        setWindowTitle(m_mode == Mode::New ? "New Ticket" : "Edit Ticket");
    }
}

void TicketDialog::on_btnEdit_clicked()
{
    setMode(Mode::Edit);
}

void TicketDialog::on_btnSave_clicked()
{
    // Save logic here
    if (m_mode == Mode::New) {
        accept();
    } else {
        setMode(Mode::View);
    }
}

void TicketDialog::on_btnCancel_clicked()
{
    if (m_mode == Mode::New) {
        reject();
    } else {
        setMode(Mode::View);
    }
}

void TicketDialog::on_btnClose_clicked()
{
    close();
}
