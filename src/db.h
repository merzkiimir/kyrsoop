#pragma once

#include <QSqlDatabase>
#include <QString>
#include <vector>

class QWidget;
class Contact;

class DbStorage
{
public:
    DbStorage();
    ~DbStorage();

    DbStorage(const DbStorage&) = delete;
    DbStorage& operator=(const DbStorage&) = delete;

    bool ensureOpen(QWidget* parent, QString* error = nullptr);
    bool isOpen() const;

    bool saveAll(const std::vector<Contact>& contacts, QString* error = nullptr);
    bool loadAll(std::vector<Contact>& out, QString* error = nullptr);

    void close();

private:
    QString m_connName;
    QSqlDatabase m_db;

    bool openWithConfig(const QString& host,
                        int port,
                        const QString& dbName,
                        const QString& user,
                        const QString& pass,
                        QString* error);

    bool ensureSchema(QString* error);
};
