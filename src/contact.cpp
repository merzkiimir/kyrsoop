#include "contact.h"

#include <sstream>
#include <regex>
#include <algorithm>
#include <cctype>
#include <ctime>

std::string Contact::trim(const std::string& s) {
    const std::string ws = " \t";
    size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

bool Contact::valname(const std::string& s) {
    static const std::regex pattern(
        R"(^[A-Za-z][A-Za-z0-9]*(?:[ -][A-Za-z0-9]+)*$)"
    );
    return std::regex_match(s, pattern);
}


bool Contact::valemail(const std::string& s) {
    static const std::regex pattern(
        R"(^[A-Za-z][A-Za-z\d]*[@][A-Za-z][A-Za-z\d]*[.][A-Za-z]*$)"
    );
    return std::regex_match(s, pattern);
}

bool Contact::valphone(const std::string& s) {
    static const std::regex pattern(
        R"(^(\+7|8)\(?\d{3}\)?\d{3}(-?\d{2}){2}$)"
    );
    return std::regex_match(s, pattern);
}

int Contact::dateKey(const std::string& ddmmyyyy) {
    if (ddmmyyyy.size() != 10) return -1;
    if (ddmmyyyy[2] != '.' || ddmmyyyy[5] != '.') return -1;

    int d = 0, m = 0, y = 0;
    try {
        d = std::stoi(ddmmyyyy.substr(0, 2));
        m = std::stoi(ddmmyyyy.substr(3, 2));
        y = std::stoi(ddmmyyyy.substr(6, 4));
    } catch (...) {
        return -1;
    }
    return y * 10000 + m * 100 + d;
}

bool Contact::valdate(const std::string& s) {
    static const std::regex pattern(R"(^\d{2}\.\d{2}\.\d{4}$)");
    if (!std::regex_match(s, pattern)) return false;

    int day = std::stoi(s.substr(0, 2));
    int month = std::stoi(s.substr(3, 2));
    int year = std::stoi(s.substr(6, 4));

    if (month < 1 || month > 12) return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    if (month == 2 && leap) daysInMonth[1] = 29;

    if (day < 1 || day > daysInMonth[month - 1]) return false;

    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    if (!now) return false;

    int cy = now->tm_year + 1900, cm = now->tm_mon + 1, cd = now->tm_mday;

    if (year < cy) return true;
    if (year == cy && month < cm) return true;
    if (year == cy && month == cm && day < cd) return true;
    return false;
}

std::string Contact::serialize() const {
    std::string phonesstring;
    for (size_t i = 0; i < phones.size(); i++) {
        phonesstring += phones[i];
        if (i + 1 < phones.size()) phonesstring += "|";
    }
    return name + ";" + surname + ";" + otchestvo + ";" + email + ";" + address + ";" + bd + ";" + phonesstring;
}

Contact Contact::deserialize(const std::string& line) {
    Contact c;
    std::stringstream ss(line);
    std::string field;

    std::vector<std::string> parts;
    while (std::getline(ss, field, ';'))
        parts.push_back(field);

    if (parts.size() < 7) return c;

    c.name = parts[0];
    c.surname = parts[1];
    c.otchestvo = parts[2];
    c.email = parts[3];
    c.address = parts[4];
    c.bd = parts[5];

    std::string phonesstring = parts[6];
    size_t start = 0, pos;
    while ((pos = phonesstring.find('|', start)) != std::string::npos) {
        c.phones.push_back(phonesstring.substr(start, pos - start));
        start = pos + 1;
    }
    if (start < phonesstring.size())
        c.phones.push_back(phonesstring.substr(start));

    return c;
}

std::string Contact::phonesAsText() const {
    std::string out;
    for (size_t i = 0; i < phones.size(); ++i) {
        out += phones[i];
        if (i + 1 < phones.size()) out += ", ";
    }
    return out;
}
