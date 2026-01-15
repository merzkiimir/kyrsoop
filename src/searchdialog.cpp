#include "searchdialog.h"

#include <QLineEdit>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

SearchDialog::SearchDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Search contacts");

    m_firstEdit   = new QLineEdit(this);
    m_lastEdit    = new QLineEdit(this);
    m_middleEdit  = new QLineEdit(this);
    m_emailEdit   = new QLineEdit(this);
    m_addressEdit = new QLineEdit(this);
    m_dateEdit    = new QLineEdit(this);
    m_phoneEdit   = new QLineEdit(this);

    m_dateEdit->setPlaceholderText("dd.MM.yyyy");

    auto *form = new QFormLayout;
    form->addRow("First name:",  m_firstEdit);
    form->addRow("Last name:",   m_lastEdit);
    form->addRow("Middle name:", m_middleEdit);
    form->addRow("E-mail:",      m_emailEdit);
    form->addRow("Address:",     m_addressEdit);
    form->addRow("Birth date:",  m_dateEdit);
    form->addRow("Phone:",       m_phoneEdit);

    auto *btnOk     = new QPushButton("Search", this);
    auto *btnCancel = new QPushButton("Cancel", this);

    auto *buttons = new QHBoxLayout;
    buttons->addStretch();
    buttons->addWidget(btnOk);
    buttons->addWidget(btnCancel);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(form);
    mainLayout->addLayout(buttons);
    setLayout(mainLayout);

    connect(btnOk,     &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

std::unordered_map<std::string, std::string> SearchDialog::query() const
{
    std::unordered_map<std::string, std::string> q;

    auto put = [&q](const char *key, const QString &value) {
        const QString v = value.trimmed();
        if (!v.isEmpty())
            q[key] = v.toStdString();
    };

    put("first",   m_firstEdit->text());
    put("last",    m_lastEdit->text());
    put("middle",  m_middleEdit->text());
    put("email",   m_emailEdit->text());
    put("address", m_addressEdit->text());
    put("date",    m_dateEdit->text());
    put("phone",   m_phoneEdit->text());

    return q;
}
