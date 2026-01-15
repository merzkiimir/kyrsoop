#pragma once

#include <QDialog>
#include "contact.h"

class QLineEdit;
class QDateEdit;
class QTableWidget;

class ContactDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ContactDialog(QWidget *parent = nullptr);

    void setContact(const Contact &c);
    Contact contact() const;

private slots:
    void onAddPhone();
    void onRemovePhone();
    void onAccept();

private:
    QLineEdit *m_firstEdit;
    QLineEdit *m_lastEdit;
    QLineEdit *m_middleEdit;
    QLineEdit *m_addressEdit;
    QDateEdit *m_birthEdit;
    QLineEdit *m_emailEdit;

    QLineEdit *m_phoneEdit;
    QTableWidget *m_phoneTable;

    Contact m_contact;

    bool validate(QString &errorMessage) const;
};
