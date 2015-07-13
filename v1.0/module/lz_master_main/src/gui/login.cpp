#include "login.h"
#include "ui_login.h"

#include <QMessageBox>
#include <QtSql/QSqlRelationalDelegate>
#include <QtSql/QSqlRecord>

#include "setting_master.h"
#include "daoadmin.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":image/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);
}

Login::~Login()
{
    delete ui;
}

void Login::on_loginButton_clicked()
{
    emit enter();

    // 2014-02-18暂时加上fan
    /*QString username = ui->nameedit->text();
    QString passwd = ui->passwordedit->text();

    int ret = AdminDAO::getAdminDAOInstance()->setCurrentUser(username, passwd);
    if (ret == 0)
        emit enter();
    else if (ret == 1)
        ui->label->setText(tr("用户名不存在"));
    else
        ui->label->setText(tr("用户名存在，但密码错误"));*/
}

void Login::on_nameedit_textChanged(QString )
{
    ui->loginButton->setEnabled(true);
}

void Login::on_helpButton_clicked()
{
    emit help();
}

void Login::on_passwordedit_returnPressed()
{
    ui->loginButton->click();
}

