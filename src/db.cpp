#include "db.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDate>

#include "contact.h"

static bool askText(QWidget* parent,
                    const QString& title,
                    const QString& label,
                    QString& value,
                    bool password = false)
{
    bool ok = false;
    QString res = QInputDialog::getText(
        parent,
        title,
        label,
        password ? QLineEdit::Password : QLineEdit::Normal,
        value,
        &ok
        );
    if (!ok) return false;
    value = res.trimmed();
    return true;
}

static bool askInt(QWidget* parent,
                   const QString& title,
                   const QString& label,
                   int& value,
                   int minV,
                   int maxV)
{
    bool ok = false;
    int res = QInputDialog::getInt(parent, title, label, value, minV, maxV, 1, &ok);
    if (!ok) return false;
    value = res;
    return true;
}

DbStorage::DbStorage()
    : m_connName(QStringLiteral("phonebook_conn"))
{
}

DbStorage::~DbStorage()
{
    close();
    if (QSqlDatabase::contains(m_connName)) {
        QSqlDatabase::removeDatabase(m_connName);
    }
}

bool DbStorage::isOpen() const
{
    return m_db.isValid() && m_db.isOpen();
}

void DbStorage::close()
{
    if (m_db.isValid()) {
        m_db.close();
    }
}

bool DbStorage::openWithConfig(const QString& host,
                               int port,
                               const QString& dbName,
                               const QString& user,
                               const QString& pass,
                               QString* error)
{

    if (QSqlDatabase::contains(m_connName)) {
        m_db = QSqlDatabase::database(m_connName);
    } else {
        m_db = QSqlDatabase::addDatabase("QPSQL", m_connName);
    }

    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(dbName);
    m_db.setUserName(user);
    m_db.setPassword(pass);

    if (!m_db.open()) {
        if (error) *error = "DB open error: " + m_db.lastError().text();
        return false;
    }

    return true;
}

bool DbStorage::ensureSchema(QString* error)
{
    QSqlQuery q(m_db);

    if (!q.exec(
            "CREATE TABLE IF NOT EXISTS contacts ("
            "  id SERIAL PRIMARY KEY,"
            "  first_name TEXT NOT NULL,"
            "  last_name  TEXT NOT NULL,"
            "  patronymic TEXT,"
            "  address    TEXT,"
            "  birth_date DATE NOT NULL,"
            "  email      TEXT NOT NULL"
            ");"
            )) {
        if (error) *error = "Create contacts failed: " + q.lastError().text();
        return false;
    }

    if (!q.exec(
            "CREATE TABLE IF NOT EXISTS phones ("
            "  id SERIAL PRIMARY KEY,"
            "  contact_id INTEGER NOT NULL REFERENCES contacts(id) ON DELETE CASCADE,"
            "  phone_number TEXT NOT NULL"
            ");"
            )) {
        if (error) *error = "Create phones failed: " + q.lastError().text();
        return false;
    }

    if (!q.exec("CREATE INDEX IF NOT EXISTS idx_phones_contact_id ON phones(contact_id);")) {
        if (error) *error = "Create index failed: " + q.lastError().text();
        return false;
    }

    return true;
}

bool DbStorage::ensureOpen(QWidget* parent, QString* error)
{
    if (isOpen()) return true;

    QSettings s("qttrip", "kyrsbd");
    QString host   = s.value("db/host", "127.0.0.1").toString();
    int     port   = s.value("db/port", 5432).toInt();
    QString dbName = s.value("db/name", "kyrsbd").toString();
    QString user   = s.value("db/user", "postgres").toString();
    QString pass   = s.value("db/pass", "superpass").toString();

    QString openErr;
    if (!openWithConfig(host, port, dbName, user, pass, &openErr)) {
        QString title = "PostgreSQL connection";

        if (!askText(parent, title, "Host:", host)) return false;
        if (!askInt(parent, title, "Port:", port, 1, 65535)) return false;
        if (!askText(parent, title, "Database name:", dbName)) return false;
        if (!askText(parent, title, "User:", user)) return false;
        if (!askText(parent, title, "Password:", pass, true)) return false;

        s.setValue("db/host", host);
        s.setValue("db/port", port);
        s.setValue("db/name", dbName);
        s.setValue("db/user", user);
        s.setValue("db/pass", pass);

        if (!openWithConfig(host, port, dbName, user, pass, &openErr)) {
            if (error) *error = openErr;
            return false;
        }
    }

    QString schemaErr;
    if (!ensureSchema(&schemaErr)) {
        if (error) *error = schemaErr;
        return false;
    }

    return true;
}

bool DbStorage::saveAll(const std::vector<Contact>& contacts, QString* error)
{
    if (!isOpen()) {
        if (error) *error = "DB is not open.";
        return false;
    }

    if (!m_db.transaction()) {
        if (error) *error = "Transaction start failed: " + m_db.lastError().text();
        return false;
    }

    QSqlQuery q(m_db);


    if (!q.exec("TRUNCATE TABLE contacts RESTART IDENTITY CASCADE;")) {
        m_db.rollback();
        if (error) *error = "TRUNCATE failed: " + q.lastError().text();
        return false;
    }

    for (const Contact& c : contacts) {
        const QString bdStr = QString::fromStdString(c.getbd());
        const QDate bd = QDate::fromString(bdStr, "dd.MM.yyyy");
        if (!bd.isValid()) {
            m_db.rollback();
            if (error) *error = "Invalid birth date in contact: " + bdStr;
            return false;
        }

        QSqlQuery qc(m_db);
        qc.prepare(
            "INSERT INTO contacts(first_name, last_name, patronymic, address, birth_date, email) "
            "VALUES (:fn, :ln, :pt, :ad, :bd, :em) "
            "RETURNING id;"
            );

        qc.bindValue(":fn", QString::fromStdString(c.getname()));
        qc.bindValue(":ln", QString::fromStdString(c.getsurname()));
        qc.bindValue(":pt", QString::fromStdString(c.getmiddle()));
        qc.bindValue(":ad", QString::fromStdString(c.getaddress()));
        qc.bindValue(":bd", bd);
        qc.bindValue(":em", QString::fromStdString(c.getemail()));

        if (!qc.exec() || !qc.next()) {
            m_db.rollback();
            if (error) *error = "Insert contact failed: " + qc.lastError().text();
            return false;
        }

        const int contactId = qc.value(0).toInt();

        QSqlQuery qp(m_db);
        qp.prepare("INSERT INTO phones(contact_id, phone_number) VALUES (:cid, :num);");

        const auto& phones = c.getphones();
        for (const std::string& numStd : phones) {
            qp.bindValue(":cid", contactId);
            qp.bindValue(":num", QString::fromStdString(numStd));
            if (!qp.exec()) {
                m_db.rollback();
                if (error) *error = "Insert phone failed: " + qp.lastError().text();
                return false;
            }
        }
    }

    if (!m_db.commit()) {
        if (error) *error = "Commit failed: " + m_db.lastError().text();
        return false;
    }

    return true;
}

bool DbStorage::loadAll(std::vector<Contact>& out, QString* error)
{
    out.clear();

    if (!isOpen()) {
        if (error) *error = "DB is not open.";
        return false;
    }

    QSqlQuery qc(m_db);
    if (!qc.exec(
            "SELECT id, first_name, last_name, patronymic, address, "
            "       to_char(birth_date, 'DD.MM.YYYY') AS birth_date, "
            "       email "
            "FROM contacts ORDER BY id;"
            )) {
        if (error) *error = "Select contacts failed: " + qc.lastError().text();
        return false;
    }

    while (qc.next()) {
        const int id = qc.value("id").toInt();

        Contact c;
        c.setname(qc.value("first_name").toString().toStdString());
        c.setsurname(qc.value("last_name").toString().toStdString());
        c.setmiddle(qc.value("patronymic").toString().toStdString());
        c.setaddress(qc.value("address").toString().toStdString());
        c.setbd(qc.value("birth_date").toString().toStdString());
        c.setemail(qc.value("email").toString().toStdString());

        std::vector<std::string> phones;

        QSqlQuery qp(m_db);
        qp.prepare("SELECT phone_number FROM phones WHERE contact_id = :cid ORDER BY id;");
        qp.bindValue(":cid", id);

        if (!qp.exec()) {
            if (error) *error = "Select phones failed: " + qp.lastError().text();
            return false;
        }

        while (qp.next()) {
            phones.push_back(qp.value(0).toString().toStdString());
        }

        c.setphones(std::move(phones));
        out.push_back(std::move(c));
    }

    return true;
}
