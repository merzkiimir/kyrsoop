#pragma once

#include <QDialog>
#include <unordered_map>
#include <string>

class QLineEdit;

class SearchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SearchDialog(QWidget *parent = nullptr);

    std::unordered_map<std::string, std::string> query() const;

private:
    QLineEdit *m_firstEdit;
    QLineEdit *m_lastEdit;
    QLineEdit *m_middleEdit;
    QLineEdit *m_emailEdit;
    QLineEdit *m_addressEdit;
    QLineEdit *m_dateEdit;
    QLineEdit *m_phoneEdit;
};
