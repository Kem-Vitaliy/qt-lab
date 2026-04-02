#ifndef TICKETDIALOG_H
#define TICKETDIALOG_H

#include <QDialog>
#include "ticket.h"

namespace Ui {
class TicketDialog;
}

class TicketDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Mode {
        View,
        Edit,
        New
    };

    explicit TicketDialog(Mode mode, QWidget *parent = nullptr);
    ~TicketDialog();

    void setTicket(const Ticket &ticket);
    Ticket getTicket() const;

private slots:
    void on_btnEdit_clicked();
    void on_btnSave_clicked();
    void on_btnCancel_clicked();
    void on_btnClose_clicked();

private:
    void setMode(Mode mode);
    void updateUIForMode();

    Ui::TicketDialog *ui;
    Mode m_mode;
    int m_id;
    QDateTime m_createdAt;
};

#endif // TICKETDIALOG_H
