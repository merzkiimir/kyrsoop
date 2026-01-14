#include "contact.h"

#include <sstream>
#include <algorithm>
#include <cctype>
#include <ctime>

string Contact::trim(const string& s) const {
    const string ws = " \t";
    size_t start = s.find_first_not_of(ws);
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

bool Contact::valname(const string& s) const {
    static const regex pattern(
        R"(^[A-Za-z][A-Za-z0-9]*(?:[ -][A-Za-z0-9]+)*$)"
    );
    return regex_match(s, pattern);
}

bool Contact::valemail(const string& s) const {
    static const regex pattern(
        R"(^[A-Za-z][A-Za-z\d]*[@][A-Za-z][A-Za-z\d]*[.][A-Za-z]*$)"
    );
    return regex_match(s, pattern);
}

bool Contact::valphone(const string& s) const {
    static const regex pattern(
        R"(^(\+7|8)\(?\d{3}\)?\d{3}(-?\d{2}){2}$)"
    );
    return regex_match(s, pattern);
}

int Contact::dateKey(const string& ddmmyyyy) {
    if (ddmmyyyy.size() != 10) return -1;
    if (ddmmyyyy[2] != '.' || ddmmyyyy[5] != '.') return -1;

    int d = 0, m = 0, y = 0;
    try {
        d = stoi(ddmmyyyy.substr(0, 2));
        m = stoi(ddmmyyyy.substr(3, 2));
        y = stoi(ddmmyyyy.substr(6, 4));
    } catch (...) {
        return -1;
    }
    return y * 10000 + m * 100 + d;
}

bool Contact::valdate(const string& s) const {
    static const regex pattern(R"(^\d{2}\.\d{2}\.\d{4}$)");
    if (!regex_match(s, pattern)) return false;

    int day = stoi(s.substr(0, 2));
    int month = stoi(s.substr(3, 2));
    int year = stoi(s.substr(6, 4));

    if (month < 1 || month > 12) return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    if (month == 2 && leap) daysInMonth[1] = 29;

    if (day < 1 || day > daysInMonth[month - 1]) return false;

    time_t t = time(nullptr);
    tm* now = localtime(&t);
    if (!now) return false;

    int cy = now->tm_year + 1900, cm = now->tm_mon + 1, cd = now->tm_mday;

    if (year < cy) return true;
    if (year == cy && month < cm) return true;
    if (year == cy && month == cm && day < cd) return true;
    return false;
}

void Contact::input() {
    string s;

    do {
        cout << "Enter first name: ";
        getline(cin, s);
        s = trim(s);
        if (!valname(s)) cout << "Invalid first name. Try again.\n";
    } while (!valname(s));
    name = s;

    do {
        cout << "Enter last name: ";
        getline(cin, s);
        s = trim(s);
        if (!valname(s)) cout << "Invalid last name. Try again.\n";
    } while (!valname(s));
    surname = s;

    cout << "Enter middle name (optional): ";
    getline(cin, s);
    s = trim(s);
    if (!s.empty() && !valname(s)) {
        cout << "Invalid middle name — will be saved as empty.\n";
        s.clear();
    }
    otchestvo = s;

    cout << "Enter address (optional): ";
    getline(cin, s);
    address = trim(s);

    do {
        cout << "Enter email: ";
        getline(cin, s);
        s = trim(s);
        s.erase(remove_if(s.begin(), s.end(), [](unsigned char ch) { return std::isspace(ch); }), s.end());
        if (!valemail(s)) cout << "Invalid email. Try again.\n";
    } while (!valemail(s));
    email = s;

    do {
        cout << "Enter birth date (DD.MM.YYYY): ";
        getline(cin, s);
        s = trim(s);
        if (!valdate(s)) cout << "Invalid birth date. Try again.\n";
    } while (!valdate(s));
    bd = s;

    phones.clear();
    cout << "Enter phone numbers (empty line to finish):\n";
    while (true) {
        cout << "Phone: ";
        getline(cin, s);
        s = trim(s);
        if (s.empty()) break;
        if (!valphone(s)) {
            cout << "Invalid phone number. Try again.\n";
            continue;
        }
        phones.push_back(s);
    }

    if (phones.empty()) {
        do {
            cout << "At least 1 phone number is required. Phone: ";
            getline(cin, s);
            s = trim(s);
        } while (!valphone(s));
        phones.push_back(s);
    }
}

void Contact::edit() {
    string s;

    cout << "\n--- Edit contact ---\n";
    cout << "Press Enter to keep the current value.\n\n";

    while (true) {
        cout << "First name (" << name << "): ";
        getline(cin, s);
        s = trim(s);
        if (s.empty()) break;
        if (valname(s)) { name = s; break; }
        cout << "Invalid first name.\n";
    }

    while (true) {
        cout << "Last name (" << surname << "): ";
        getline(cin, s);
        s = trim(s);
        if (s.empty()) break;
        if (valname(s)) { surname = s; break; }
        cout << "Invalid last name.\n";
    }

    while (true) {
        cout << "Middle name (" << otchestvo << "): ";
        getline(cin, s);
        s = trim(s);
        if (s.empty()) break;
        if (valname(s)) { otchestvo = s; break; }
        cout << "Invalid middle name.\n";
    }

    cout << "Address (" << address << "): ";
    getline(cin, s);
    s = trim(s);
    if (!s.empty()) address = s;

    while (true) {
        cout << "Email (" << email << "): ";
        getline(cin, s);
        s = trim(s);
        if (s.empty()) break;
        s.erase(remove_if(s.begin(), s.end(), [](unsigned char ch) { return std::isspace(ch); }), s.end());
        if (valemail(s)) { email = s; break; }
        cout << "Invalid email.\n";
    }

    while (true) {
        cout << "Birth date (" << bd << ") (DD.MM.YYYY): ";
        getline(cin, s);
        s = trim(s);
        if (s.empty()) break;
        if (valdate(s)) { bd = s; break; }
        cout << "Invalid birth date.\n";
    }

    cout << "\nCurrent phones:\n";
    for (size_t i = 0; i < phones.size(); ++i)
        cout << "  " << (i + 1) << ") " << phones[i] << "\n";

    cout << "Replace the phone list? (y/N): ";
    getline(cin, s);
    if (!s.empty() && (s[0] == 'y' || s[0] == 'Y')) {
        vector<string> newPhones;
        cout << "Enter new phone numbers (empty line to finish):\n";
        while (true) {
            cout << "Phone: ";
            getline(cin, s);
            s = trim(s);
            if (s.empty()) break;
            if (!valphone(s)) {
                cout << "Invalid phone number.\n";
                continue;
            }
            newPhones.push_back(s);
        }
        if (!newPhones.empty()) phones = std::move(newPhones);
        else cout << "Phone list not changed (must contain at least 1 phone).\n";
    }
}

void Contact::print() const {
    cout << surname << " " << name;
    if (!otchestvo.empty()) cout << " " << otchestvo;

    cout << "\n  Address: " << address
         << "\n  Email: " << email
         << "\n  Birth date: " << bd
         << "\n  Phones:\n";

    for (const auto& p : phones)
        cout << "    - " << p << "\n";
}

string Contact::serialize() const {
    string phonesstring;
    for (size_t i = 0; i < phones.size(); i++) {
        phonesstring += phones[i];
        if (i + 1 < phones.size()) phonesstring += "|";
    }
    return name + ";" + surname + ";" + otchestvo + ";" + email + ";" + address + ";" + bd + ";" + phonesstring;
}

Contact Contact::deserialize(const string& line) {
    Contact c;
    stringstream ss(line);
    string field;

    vector<string> parts;
    while (getline(ss, field, ';'))
        parts.push_back(field);

    if (parts.size() < 7) return c;

    c.name = parts[0];
    c.surname = parts[1];
    c.otchestvo = parts[2];
    c.email = parts[3];
    c.address = parts[4];
    c.bd = parts[5];

    string phonesstring = parts[6];
    size_t start = 0, pos;
    while ((pos = phonesstring.find('|', start)) != string::npos) {
        c.phones.push_back(phonesstring.substr(start, pos - start));
        start = pos + 1;
    }
    if (start < phonesstring.size())
        c.phones.push_back(phonesstring.substr(start));

    return c;
}

bool Contact::match(const string& q) const {
    string low = q;
    for (auto& ch : low)
        ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));

    auto contains = [&](const string& s) {
        string t = s;
        for (auto& ch : t)
            ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
        return t.find(low) != string::npos;
    };

    if (contains(name)) return true;
    if (contains(surname)) return true;
    if (contains(email)) return true;
    if (contains(address)) return true;

    for (const auto& p : phones)
        if (contains(p)) return true;

    return false;
}
