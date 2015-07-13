#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QMessageBox>
#include <QtSql/QSqlRelationalDelegate>
//#include <QSqlRecord>

namespace Ui {
    class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();
signals:
    void enter();
    void help();

private slots:
    void on_helpButton_clicked();
    void on_loginButton_clicked();
    void on_nameedit_textChanged(QString );
    void on_passwordedit_returnPressed();



private:
    Ui::Login *ui;
};

#endif // LOGIN_H
