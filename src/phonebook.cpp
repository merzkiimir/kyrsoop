#include "phonebook.h"

#include <QFile>
#include <QTextStream>

void Phonebook::update(int index, const Contact& c) {
    if (index < 0 || index >= (int)m_contacts.size()) return;
    m_contacts[(size_t)index] = c;
}

void Phonebook::remove(int index) {
    if (index < 0 || index >= (int)m_contacts.size()) return;
    m_contacts.erase(m_contacts.begin() + index);
}

bool Phonebook::loadFromFile(const QString& path, QString* error) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error) *error = "Cannot open file for reading: " + path;
        return false;
    }

    QTextStream in(&file);
    m_contacts.clear();

    while (!in.atEnd()) {
        const QString lineQt = in.readLine();
        const std::string line = lineQt.toStdString();
        if (line.empty()) continue;
        m_contacts.push_back(Contact::deserialize(line));
    }

    return true;
}

bool Phonebook::saveToFile(const QString& path, QString* error) const {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (error) *error = "Cannot open file for writing: " + path;
        return false;
    }

    QTextStream out(&file);
    for (const auto& c : m_contacts) {
        out << QString::fromStdString(c.serialize()) << "\n";
    }

    return true;
}
