#pragma once

#include "contact.h"
#include <vector>
#include <string>

using namespace std;

class Phonebook {
private:
    vector<Contact> contacts;

    int askIndex() const;
    static string proveToLower(string s);
    static string askLine(const string& prompt);

public:
    void addcontact();
    void showall() const;
    void removecontact();
    void editcontact();
    void sortcontacts();

    void save(const string& file) const;
    void load(const string& file);

    void saveToPrompt();     
    void loadFromPrompt();   

    void search() const;
    void menu();
};
