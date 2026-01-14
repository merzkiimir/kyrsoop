#pragma once

#include <string>
#include <vector>
#include <regex>
#include <iostream>

using namespace std;

class Contact {
private:
    string name;
    string surname;
    string otchestvo;
    string address;
    string bd;
    string email;
    vector<string> phones;

    string trim(const string& s) const;

    bool valname(const string& s) const;
    bool valemail(const string& s) const;
    bool valphone(const string& s) const;
    bool valdate(const string& s) const;

    static int dateKey(const string& ddmmyyyy);

public:
    void input();
    void edit();
    void print() const;

    string serialize() const;
    static Contact deserialize(const string& line);

    bool match(const string& q) const;

    string getname() const { return name; }
    string getsurname() const { return surname; }
    string getemail() const { return email; }
    string getbd() const { return bd; }
    int birthKey() const { return dateKey(bd); }
};
