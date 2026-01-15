#pragma once

#include "contact.h"
#include <vector>
#include <QString>

class Phonebook {
public:
    const std::vector<Contact>& contacts() const { return m_contacts; }

    void add(const Contact& c) { m_contacts.push_back(c); }
    void update(int index, const Contact& c);
    void remove(int index);
    void clear() { m_contacts.clear(); }

    // Task2: QFile
    bool loadFromFile(const QString& path, QString* error = nullptr);
    bool saveToFile(const QString& path, QString* error = nullptr) const;

private:
    std::vector<Contact> m_contacts;
};
