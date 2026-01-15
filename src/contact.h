#pragma once

#include <string>
#include <vector>

class Contact {
public:
    Contact() = default;

    const std::string& getname() const { return name; }
    const std::string& getsurname() const { return surname; }
    const std::string& getmiddle() const { return otchestvo; }
    const std::string& getaddress() const { return address; }
    const std::string& getbd() const { return bd; }       
    const std::string& getemail() const { return email; }
    const std::vector<std::string>& getphones() const { return phones; }

    void setname(std::string v) { name = std::move(v); }
    void setsurname(std::string v) { surname = std::move(v); }
    void setmiddle(std::string v) { otchestvo = std::move(v); }
    void setaddress(std::string v) { address = std::move(v); }
    void setbd(std::string v) { bd = std::move(v); }
    void setemail(std::string v) { email = std::move(v); }
    void setphones(std::vector<std::string> v) { phones = std::move(v); }

    
    std::string serialize() const;
    static Contact deserialize(const std::string& line);

    static int dateKey(const std::string& ddmmyyyy);
    int birthKey() const { return dateKey(bd); }

    static bool valname(const std::string& s);
    static bool valemail(const std::string& s);
    static bool valphone(const std::string& s);
    static bool valdate(const std::string& s);   

    static std::string trim(const std::string& s);

    std::string phonesAsText() const;

private:
    std::string name;
    std::string surname;
    std::string otchestvo;
    std::string address;
    std::string bd;
    std::string email;
    std::vector<std::string> phones;
};
