#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QMessageBox>
#include <QtSql/QSqlRelationalDelegate>
#include <QtSql/QSqlQueryModel>
//#include <QSqlRecord>
#include <QButtonGroup>

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

    void sellectOneUser();

private:
    QSqlQueryModel *userModel;
    Ui::Login *ui;

    QButtonGroup * group;

};

#endif // LOGIN_H
