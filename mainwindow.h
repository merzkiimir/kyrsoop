#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "src/phonebook.h"
#include "src/db.h"

#include <unordered_map>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnadd_clicked();
    void on_btnedit_clicked();

    void on_btndel_clicked();

    void on_btnload_clicked();
    void on_btnsave_clicked();
    void on_btnsearch_clicked();
    void on_btnreset_clicked();

    void on_btnSaveDb_clicked();
    void on_btnLoadDb_clicked();

private:
    Ui::MainWindow *ui;
    Phonebook m_book;
    DbStorage m_db;

    void refreshTable();
    int selectedContactIndex() const;

    static bool containsCaseInsensitive(const std::string& haystack, const std::string& needle);
    static bool matchesQuery(const Contact& c, const std::unordered_map<std::string, std::string>& q);
};

#endif // MAINWINDOW_H
