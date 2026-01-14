#include "phonebook.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>

using namespace std;

string Phonebook::proveToLower(string s) {
    for (auto& ch : s)
        ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
    return s;
}

string Phonebook::askLine(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);

    auto is_ws = [](unsigned char c){ return c==' ' || c=='\t' || c=='\r' || c=='\n'; };
    while (!s.empty() && is_ws((unsigned char)s.front())) s.erase(s.begin());
    while (!s.empty() && is_ws((unsigned char)s.back())) s.pop_back();

    return s;
}

void Phonebook::addcontact() {
    Contact c;
    c.input();
    contacts.push_back(std::move(c));
}

void Phonebook::showall() const {
    if (contacts.empty()) {
        cout << "The list is empty.\n";
        return;
    }

    cout << "\n=== Contacts (" << contacts.size() << ") ===\n";
    for (size_t i = 0; i < contacts.size(); ++i) {
        cout << "\n[" << (i + 1) << "]\n";
        contacts[i].print();
    }
}

int Phonebook::askIndex() const {
    if (contacts.empty()) {
        cout << "The list is empty.\n";
        return -1;
    }

    showall();
    cout << "\nEnter contact number (1.." << contacts.size() << "): ";

    int n = 0;
    if (!(cin >> n)) {
        cin.clear();
        cin.ignore(100000, '\n');
        cout << "Invalid input.\n";
        return -1;
    }
    cin.ignore(100000, '\n');

    if (n < 1 || static_cast<size_t>(n) > contacts.size()) {
        cout << "No contact with this number.\n";
        return -1;
    }
    return n - 1;
}

void Phonebook::removecontact() {
    int idx = askIndex();
    if (idx == -1) return;

    cout << "Delete this contact? (y/N): ";
    string s;
    getline(cin, s);

    if (!s.empty() && (s[0] == 'y' || s[0] == 'Y')) {
        contacts.erase(contacts.begin() + idx);
        cout << "Deleted.\n";
    } else {
        cout << "Cancelled.\n";
    }
}

void Phonebook::editcontact() {
    int idx = askIndex();
    if (idx == -1) return;
    contacts[idx].edit();
}

void Phonebook::sortcontacts() {
    if (contacts.size() < 2) {
        cout << "Nothing to sort.\n";
        return;
    }

    cout << "\nSort by:\n"
         << " 1 - Last name\n"
         << " 2 - First name\n"
         << " 3 - Email\n"
         << " 4 - Birth date\n"
         << "Choice: ";

    int cmd = 0;
    if (!(cin >> cmd)) {
        cin.clear();
        cin.ignore(100000, '\n');
        cout << "Invalid input.\n";
        return;
    }
    cin.ignore(100000, '\n');

    switch (cmd) {
        case 1:
            sort(contacts.begin(), contacts.end(), [](const Contact& a, const Contact& b) {
                return proveToLower(a.getsurname()) < proveToLower(b.getsurname());
            });
            break;
        case 2:
            sort(contacts.begin(), contacts.end(), [](const Contact& a, const Contact& b) {
                return proveToLower(a.getname()) < proveToLower(b.getname());
            });
            break;
        case 3:
            sort(contacts.begin(), contacts.end(), [](const Contact& a, const Contact& b) {
                return proveToLower(a.getemail()) < proveToLower(b.getemail());
            });
            break;
        case 4:
            sort(contacts.begin(), contacts.end(), [](const Contact& a, const Contact& b) {
                return a.birthKey() < b.birthKey();
            });
            break;
        default:
            cout << "Unknown field.\n";
            return;
    }

    cout << "Sorted.\n";
}

void Phonebook::save(const string& file) const {
    ofstream out(file);
    if (!out) {
        cout << "Error: cannot open file for writing: " << file << "\n";
        return;
    }

    for (const auto& c : contacts)
        out << c.serialize() << "\n";

    cout << "Saved " << contacts.size() << " contact(s) to " << file << "\n";
}

void Phonebook::load(const string& file) {
    ifstream in(file);
    if (!in) {
        cout << "Error: cannot open file for reading: " << file << "\n";
        return;
    }

    contacts.clear();

    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        contacts.push_back(Contact::deserialize(line));
    }

    cout << "Loaded " << contacts.size() << " contact(s) from " << file << "\n";
}

void Phonebook::saveToPrompt() {
    string file = askLine("Enter filename to save: ");
    if (file.empty()) {
        cout << "Filename is empty.\n";
        return;
    }
    save(file);
}

void Phonebook::loadFromPrompt() {
    string file = askLine("Enter filename to load: ");
    if (file.empty()) {
        cout << "Filename is empty.\n";
        return;
    }
    load(file);
}

void Phonebook::search() const {
    if (contacts.empty()) {
        cout << "The list is empty.\n";
        return;
    }

    cout << "Enter search query: ";
    string q;
    getline(cin, q);

    bool found = false;
    for (const auto& c : contacts) {
        if (c.match(q)) {
            cout << "\n";
            c.print();
            found = true;
        }
    }

    if (!found)
        cout << "No matches.\n";
}

void Phonebook::menu() {
    cout << "Load contacts from a file at start? (y/N): ";
    string s;
    getline(cin, s);
    if (!s.empty() && (s[0] == 'y' || s[0] == 'Y')) {
        loadFromPrompt();
    }

    int cmd = 0;
    while (true) {
        cout << "\n===== Phone Book =====\n"
             << " 1 - Add contact\n"
             << " 2 - Show all contacts\n"
             << " 3 - Search\n"
             << " 4 - Sort\n"
             << " 5 - Edit contact\n"
             << " 6 - Delete contact\n"
             << " 7 - Save to file...\n"
             << " 8 - Load from file...\n"
             << " 9 - Exit\n"
             << "Choice: ";

        if (!(cin >> cmd)) {
            cin.clear();
            cin.ignore(100000, '\n');
            cout << "Invalid input.\n";
            continue;
        }
        cin.ignore(100000, '\n');

        if (cmd == 1) addcontact();
        else if (cmd == 2) showall();
        else if (cmd == 3) search();
        else if (cmd == 4) sortcontacts();
        else if (cmd == 5) editcontact();
        else if (cmd == 6) removecontact();
        else if (cmd == 7) saveToPrompt();
        else if (cmd == 8) loadFromPrompt();
        else if (cmd == 9) break;
        else cout << "Unknown command.\n";
    }
}
