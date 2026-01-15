#include "contactdialog.h"

#include <QLineEdit>
#include <QDateEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDate>

ContactDialog::ContactDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Contact");

    m_firstEdit  = new QLineEdit(this);
    m_lastEdit   = new QLineEdit(this);
    m_middleEdit = new QLineEdit(this);
    m_addressEdit= new QLineEdit(this);
    m_emailEdit  = new QLineEdit(this);

    m_birthEdit = new QDateEdit(this);
    m_birthEdit->setCalendarPopup(true);
    m_birthEdit->setDisplayFormat("dd.MM.yyyy");
    m_birthEdit->setDate(QDate::currentDate().addYears(-18));
    m_birthEdit->setMaximumDate(QDate::currentDate().addDays(-1));

    m_phoneEdit = new QLineEdit(this);
    m_phoneEdit->setPlaceholderText("+7(812)123-45-67");

    m_phoneTable = new QTableWidget(this);
    m_phoneTable->setColumnCount(1);
    m_phoneTable->setHorizontalHeaderLabels({ "Number" });
    m_phoneTable->horizontalHeader()->setStretchLastSection(true);
    m_phoneTable->verticalHeader()->setVisible(false);
    m_phoneTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_phoneTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_phoneTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto *btnAddPhone    = new QPushButton("Add phone", this);
    auto *btnRemovePhone = new QPushButton("Remove phone", this);

    auto *phoneEditLayout = new QHBoxLayout;
    phoneEditLayout->addWidget(m_phoneEdit);
    phoneEditLayout->addWidget(btnAddPhone);
    phoneEditLayout->addWidget(btnRemovePhone);

    auto *formLayout = new QFormLayout;
    formLayout->addRow("First name *:",   m_firstEdit);
    formLayout->addRow("Last name *:",    m_lastEdit);
    formLayout->addRow("Middle name:",    m_middleEdit);
    formLayout->addRow("Address:",        m_addressEdit);
    formLayout->addRow("Birth date *:",   m_birthEdit);
    formLayout->addRow("E-mail *:",       m_emailEdit);
    formLayout->addRow("Phone:",          phoneEditLayout);
    formLayout->addRow("Phones:",         m_phoneTable);

    auto *btnOk     = new QPushButton("OK", this);
    auto *btnCancel = new QPushButton("Cancel", this);

    auto *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(btnOk);
    buttonsLayout->addWidget(btnCancel);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    connect(btnAddPhone,    &QPushButton::clicked, this, &ContactDialog::onAddPhone);
    connect(btnRemovePhone, &QPushButton::clicked, this, &ContactDialog::onRemovePhone);
    connect(btnOk,          &QPushButton::clicked, this, &ContactDialog::onAccept);
    connect(btnCancel,      &QPushButton::clicked, this, &QDialog::reject);
}

void ContactDialog::onAddPhone()
{
    const QString phone = m_phoneEdit->text().trimmed();
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter phone number.");
        return;
    }

    if (!Contact::valphone(phone.toStdString())) {
        QMessageBox::warning(this, "Error", "Phone format is invalid.");
        return;
    }

    const int row = m_phoneTable->rowCount();
    m_phoneTable->insertRow(row);
    m_phoneTable->setItem(row, 0, new QTableWidgetItem(phone));
    m_phoneEdit->clear();
}

void ContactDialog::onRemovePhone()
{
    int row = m_phoneTable->currentRow();
    if (row < 0)
        row = m_phoneTable->rowCount() - 1;
    if (row >= 0)
        m_phoneTable->removeRow(row);
}

bool ContactDialog::validate(QString &errorMessage) const
{
    const std::string first = Contact::trim(m_firstEdit->text().toStdString());
    const std::string last  = Contact::trim(m_lastEdit->text().toStdString());
    const std::string mid   = Contact::trim(m_middleEdit->text().toStdString());

    if (first.empty() || last.empty()) {
        errorMessage = "First name and last name are required.";
        return false;
    }

    if (!Contact::valname(first) || !Contact::valname(last) || (!mid.empty() && !Contact::valname(mid))) {
        errorMessage = "Name fields must match the required pattern (latin letters/digits, spaces/hyphen).";
        return false;
    }

    QString email = m_emailEdit->text().trimmed();
    email.remove(' ');
    email.remove('\t');

    if (email.isEmpty()) {
        errorMessage = "E-mail is required.";
        return false;
    }
    if (!Contact::valemail(email.toStdString())) {
        errorMessage = "E-mail format is invalid.";
        return false;
    }

    if (m_phoneTable->rowCount() == 0) {
        errorMessage = "At least 1 phone number is required.";
        return false;
    }

    if (m_birthEdit->date() >= QDate::currentDate()) {
        errorMessage = "Birth date must be earlier than today.";
        return false;
    }

    return true;
}

void ContactDialog::onAccept()
{
    QString error;
    if (!validate(error)) {
        QMessageBox::warning(this, "Error", error);
        return;
    }

    Contact c;
    c.setname(Contact::trim(m_firstEdit->text().toStdString()));
    c.setsurname(Contact::trim(m_lastEdit->text().toStdString()));
    c.setmiddle(Contact::trim(m_middleEdit->text().toStdString()));
    c.setaddress(Contact::trim(m_addressEdit->text().toStdString()));

    QString email = m_emailEdit->text().trimmed();
    email.remove(' ');
    email.remove('\t');
    c.setemail(email.toStdString());

    c.setbd(m_birthEdit->date().toString("dd.MM.yyyy").toStdString());

    std::vector<std::string> phones;
    phones.reserve((size_t)m_phoneTable->rowCount());
    for (int row = 0; row < m_phoneTable->rowCount(); ++row) {
        auto *numItem = m_phoneTable->item(row, 0);
        if (!numItem) continue;
        phones.push_back(numItem->text().trimmed().toStdString());
    }
    c.setphones(std::move(phones));

    m_contact = c;
    accept();
}

void ContactDialog::setContact(const Contact &c)
{
    m_contact = c;

    m_firstEdit->setText(QString::fromStdString(c.getname()));
    m_lastEdit->setText(QString::fromStdString(c.getsurname()));
    m_middleEdit->setText(QString::fromStdString(c.getmiddle()));
    m_addressEdit->setText(QString::fromStdString(c.getaddress()));
    m_emailEdit->setText(QString::fromStdString(c.getemail()));

    const QDate birth = QDate::fromString(QString::fromStdString(c.getbd()), "dd.MM.yyyy");
    if (birth.isValid()) m_birthEdit->setDate(birth);

    m_phoneTable->setRowCount(0);
    for (const auto& num : c.getphones()) {
        const int row = m_phoneTable->rowCount();
        m_phoneTable->insertRow(row);
        m_phoneTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(num)));
    }
}

Contact ContactDialog::contact() const
{
    return m_contact;
}
