#include "ticketdialog.h"
#include "ui_ticketdialog.h"

TicketDialog::TicketDialog(Mode mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TicketDialog),
    m_mode(mode),
    m_id(0),
    m_createdAt(QDateTime::currentDateTime())
{
    ui->setupUi(this);
    setMode(m_mode);
}

TicketDialog::~TicketDialog()
{
    delete ui;
}

void TicketDialog::setTicket(const Ticket &ticket)
{
    m_id = ticket.id;
    m_createdAt = ticket.createdAt;
    
    ui->lineEditTitle->setText(ticket.title);
    ui->comboBoxStatus->setCurrentText(ticket.status);
    ui->comboBoxPriority->setCurrentText(ticket.priority);
    ui->plainTextEditDescription->setPlainText(ticket.description);
    
    ui->labelID->setText(QString::number(m_id));
    ui->labelCreated->setText(m_createdAt.toString("yyyy-MM-dd HH:mm:ss"));
}

Ticket TicketDialog::getTicket() const
{
    Ticket ticket;
    ticket.id = m_id;
    ticket.title = ui->lineEditTitle->text();
    ticket.status = ui->comboBoxStatus->currentText();
    ticket.priority = ui->comboBoxPriority->currentText();
    ticket.description = ui->plainTextEditDescription->toPlainText();
    ticket.createdAt = m_createdAt;
    return ticket;
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
    // In both New and Edit modes, "Save" should accept the dialog
    accept();
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
