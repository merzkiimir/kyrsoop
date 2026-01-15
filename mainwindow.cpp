#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QStatusBar>
#include <QHeaderView>

#include <QtSql/QSqlDatabase>

#include "src/contactdialog.h"
#include "src/searchdialog.h"

#include <cctype>
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto header = ui->tableContacts->horizontalHeader();

    header->setStretchLastSection(false);
    header->setSectionResizeMode(QHeaderView::Fixed);
    header->setMinimumSectionSize(80);
    header->setSectionsMovable(false);

    setStyleSheet(R"(

        QMainWindow, QDialog {
            background-color: #fff6fb;
            color: #3a2a38;
            font-family: "Segoe UI";
            font-size: 10pt;
        }

        QWidget#centralwidget {
            background-color: #fff6fb;
        }

        QLabel {
            color: #3a2a38;
        }

        /* Inputs */
        QLineEdit, QDateEdit {
            background-color: #ffffff;
            color: #3a2a38;
            border: 1px solid #f0c7df;
            border-radius: 10px;
            padding: 6px 10px;
            selection-background-color: #ffb7d5;
            selection-color: #2a1d27;
        }

        QLineEdit:focus, QDateEdit:focus {
            border: 2px solid #d7a7ff;
            background-color: #fff0f8;
        }

        /* Table */
        QTableWidget {
            background-color: #ffffff;
            color: #3a2a38;
            gridline-color: #f3d6e6;
            selection-background-color: #ffd1e6;
            selection-color: #2a1d27;
            alternate-background-color: #fff0f8;
            border: 1px solid #f0c7df;
            border-radius: 12px;
        }

        QTableWidget::item {
            padding: 6px;
            border: none;
        }

        QTableWidget::item:selected {
            border: 1px solid #d7a7ff;
            border-radius: 6px;
        }

        /* Header */
        QHeaderView::section {
            background-color: #ffe3f1;
            color: #3a2a38;
            padding: 8px;
            border: none;
            border-right: 1px solid #f3d6e6;
            border-bottom: 1px solid #f3d6e6;
            font-weight: 600;
        }

        QHeaderView {
            background-color: transparent;
        }

        /* Buttons */
        QPushButton {
            color: #ffffff;
            border: none;
            border-radius: 12px;
            padding: 6px 14px;
            font-weight: 600;

            /* Soft pink gradient */
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #ff77b7,
                                        stop:1 #b48cff);
        }

        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #ff8cc4,
                                        stop:1 #c19aff);
        }

        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #e85aa1,
                                        stop:1 #9d73ff);
        }

        QPushButton:disabled {
            background-color: #f3d6e6;
            color: #9a7f92;
        }

        /* Scrollbars */
        QScrollBar:vertical {
            background: #fff6fb;
            width: 12px;
            margin: 0px;
            border: none;
        }
        QScrollBar::handle:vertical {
            background: #f1b8d6;
            min-height: 24px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical:hover {
            background: #d7a7ff;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
            background: transparent;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
        }

        QScrollBar:horizontal {
            background: #fff6fb;
            height: 12px;
            margin: 0px;
            border: none;
        }
        QScrollBar::handle:horizontal {
            background: #f1b8d6;
            min-width: 24px;
            border-radius: 6px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #d7a7ff;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
            background: transparent;
        }
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
            background: transparent;
        }

    )");

    ui->tableContacts->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->tableContacts->setColumnWidth(0, 110);
    ui->tableContacts->setColumnWidth(1, 110);
    ui->tableContacts->setColumnWidth(2, 120);
    ui->tableContacts->setColumnWidth(3, 170);
    ui->tableContacts->setColumnWidth(4, 130);
    ui->tableContacts->setColumnWidth(5, 110);
    ui->tableContacts->setColumnWidth(6, 170);
    // --------------------------------------------------------

    setWindowTitle(QStringLiteral("Телефонный справочник"));
    setFixedSize(size());

    const bool hasPgDriver = QSqlDatabase::isDriverAvailable("QPSQL");
    ui->btnLoadDb->setEnabled(hasPgDriver);
    ui->btnSaveDb->setEnabled(hasPgDriver);

    ui->tableContacts->setSortingEnabled(true);

    statusBar()->showMessage(hasPgDriver ? "Ready"
                                         : "QPSQL driver not available (PostgreSQL driver for Qt is missing)");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshTable()
{
    const bool wasSorting = ui->tableContacts->isSortingEnabled();
    ui->tableContacts->setSortingEnabled(false);

    ui->tableContacts->setRowCount(0);

    const auto& list = m_book.contacts();
    ui->tableContacts->setRowCount((int)list.size());

    for (int i = 0; i < (int)list.size(); ++i) {
        const Contact& c = list[(size_t)i];

        auto *item0 = new QTableWidgetItem(QString::fromStdString(c.getname()));
        item0->setData(Qt::UserRole, i);

        ui->tableContacts->setItem(i, 0, item0);
        ui->tableContacts->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(c.getsurname())));
        ui->tableContacts->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(c.getmiddle())));
        ui->tableContacts->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(c.getemail())));
        ui->tableContacts->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(c.getaddress())));
        ui->tableContacts->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(c.getbd())));
        ui->tableContacts->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(c.phonesAsText())));

        ui->tableContacts->setRowHidden(i, false);
    }

    ui->tableContacts->setSortingEnabled(wasSorting);
    statusBar()->showMessage(QString("Contacts: %1").arg(list.size()));
}

int MainWindow::selectedContactIndex() const
{
    const int row = ui->tableContacts->currentRow();
    if (row < 0) return -1;

    QTableWidgetItem* item0 = ui->tableContacts->item(row, 0);
    if (!item0) return -1;

    return item0->data(Qt::UserRole).toInt();
}

bool MainWindow::containsCaseInsensitive(const std::string& haystack, const std::string& needle)
{
    if (needle.empty()) return true;

    auto lower = [](std::string s) {
        for (char& ch : s) ch = (char)std::tolower((unsigned char)ch);
        return s;
    };

    const std::string h = lower(haystack);
    const std::string n = lower(needle);
    return h.find(n) != std::string::npos;
}

bool MainWindow::matchesQuery(const Contact& c, const std::unordered_map<std::string, std::string>& q)
{
    auto get = [&](const char* key) -> std::string {
        auto it = q.find(key);
        return it == q.end() ? std::string() : it->second;
    };

    const std::string first   = get("first");
    const std::string last    = get("last");
    const std::string middle  = get("middle");
    const std::string email   = get("email");
    const std::string address = get("address");
    const std::string date    = get("date");
    const std::string phone   = get("phone");

    if (!first.empty()   && !containsCaseInsensitive(c.getname(), first)) return false;
    if (!last.empty()    && !containsCaseInsensitive(c.getsurname(), last)) return false;
    if (!middle.empty()  && !containsCaseInsensitive(c.getmiddle(), middle)) return false;
    if (!email.empty()   && !containsCaseInsensitive(c.getemail(), email)) return false;
    if (!address.empty() && !containsCaseInsensitive(c.getaddress(), address)) return false;

    if (!date.empty()) {
        if (c.getbd() != date) return false;
    }

    if (!phone.empty()) {
        bool ok = false;
        for (const auto& num : c.getphones()) {
            if (containsCaseInsensitive(num, phone)) {
                ok = true; break;
            }
        }
        if (!ok) return false;
    }

    return true;
}

void MainWindow::on_btnadd_clicked()
{
    ContactDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_book.add(dlg.contact());
        refreshTable();
    }
}

void MainWindow::on_btnedit_clicked()
{
    const int idx = selectedContactIndex();
    if (idx < 0) {
        QMessageBox::information(this, "Info", "Select a contact first.");
        return;
    }
    if (idx >= (int)m_book.contacts().size()) return;

    ContactDialog dlg(this);
    dlg.setContact(m_book.contacts()[(size_t)idx]);

    if (dlg.exec() == QDialog::Accepted) {
        m_book.update(idx, dlg.contact());
        refreshTable();
    }
}


void MainWindow::on_btndel_clicked()
{
    const int idx = selectedContactIndex();
    if (idx < 0) {
        QMessageBox::information(this, "Info", "Select a contact first.");
        return;
    }

    if (QMessageBox::question(this, "Confirm", "Delete selected contact?") != QMessageBox::Yes)
        return;

    m_book.remove(idx);
    refreshTable();
}

void MainWindow::on_btnload_clicked()
{
    const QString path = QFileDialog::getOpenFileName(this, "Load contacts", QString(), "Text files (*.txt);;All files (*.*)");
    if (path.isEmpty()) return;

    QString err;
    if (!m_book.loadFromFile(path, &err)) {
        QMessageBox::critical(this, "Error", err);
        return;
    }

    refreshTable();
    statusBar()->showMessage("Loaded: " + path);
}

void MainWindow::on_btnsave_clicked()
{
    const QString path = QFileDialog::getSaveFileName(this, "Save contacts", QString(), "Text files (*.txt);;All files (*.*)");
    if (path.isEmpty()) return;

    QString err;
    if (!m_book.saveToFile(path, &err)) {
        QMessageBox::critical(this, "Error", err);
        return;
    }

    statusBar()->showMessage("Saved: " + path);
}

void MainWindow::on_btnsearch_clicked()
{
    if (m_book.contacts().empty()) {
        QMessageBox::information(this, "Info", "The list is empty.");
        return;
    }

    SearchDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    const auto q = dlg.query();

    int found = 0;
    for (int row = 0; row < ui->tableContacts->rowCount(); ++row) {
        QTableWidgetItem* item0 = ui->tableContacts->item(row, 0);
        if (!item0) continue;

        const int idx = item0->data(Qt::UserRole).toInt();
        if (idx < 0 || idx >= (int)m_book.contacts().size()) continue;

        const bool ok = matchesQuery(m_book.contacts()[(size_t)idx], q);
        ui->tableContacts->setRowHidden(row, !ok);
        if (ok) found++;
    }

    statusBar()->showMessage(QString("Found: %1").arg(found));
}

void MainWindow::on_btnreset_clicked()
{
    for (int row = 0; row < ui->tableContacts->rowCount(); ++row)
        ui->tableContacts->setRowHidden(row, false);

    statusBar()->showMessage("Filter reset");
}


void MainWindow::on_btnSaveDb_clicked()
{
    if (m_book.contacts().empty()) {
        QMessageBox::information(this, "DB", "Nothing to save: contact list is empty.");
        return;
    }

    QString err;
    if (!m_db.ensureOpen(this, &err)) {
        QMessageBox::warning(this, "DB", err);
        return;
    }

    if (!m_db.saveAll(m_book.contacts(), &err)) {
        QMessageBox::critical(this, "DB", err);
        return;
    }

    statusBar()->showMessage("Saved to PostgreSQL");
    QMessageBox::information(this, "DB", "Saved to PostgreSQL.");
}

void MainWindow::on_btnLoadDb_clicked()
{
    QString err;
    if (!m_db.ensureOpen(this, &err)) {
        QMessageBox::warning(this, "DB", err);
        return;
    }

    std::vector<Contact> loaded;
    if (!m_db.loadAll(loaded, &err)) {
        QMessageBox::critical(this, "DB", err);
        return;
    }

    m_book.clear();
    for (const auto& c : loaded) {
        m_book.add(c);
    }

    refreshTable();
    statusBar()->showMessage("Loaded from PostgreSQL");
    QMessageBox::information(this, "DB", QString("Loaded from PostgreSQL: %1").arg((int)loaded.size()));
}
