#include "login.h"
#include "ui_login.h"

#include <QMessageBox>
#include <QtSql/QSqlRelationalDelegate>
#include <QtSql/QSqlRecord>
#include <QRadioButton>

#include "setting_client.h"
#include "daoadmin.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":image/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);

    group = new QButtonGroup(this);

    QStringListModel * model = AdminDAO::getAdminDAOInstance()->getUsernames();
    
    QString img = ":/users/0.jpg";
    QRadioButton * tmpbutton = ui->userButton_0;

    for (int i = 0; i < model->rowCount(); i++)
    {
        img = QString(":/users/%1.jpg").arg(i); 
        switch (i)
        {
        case 0: tmpbutton = ui->userButton_0; break;
        case 1: tmpbutton = ui->userButton_1; break;
        case 2: tmpbutton = ui->userButton_2; break;
        case 3: tmpbutton = ui->userButton_3; break;
        case 4: tmpbutton = ui->userButton_4; break;
        case 5: tmpbutton = ui->userButton_5; break;
        default: tmpbutton = ui->userButton_0; break;
        }
        tmpbutton->setIcon(QPixmap(img));
        tmpbutton->setIconSize(QSize(40,40));
    }

    group->addButton(ui->userButton_0);
    group->addButton(ui->userButton_1);
    group->addButton(ui->userButton_2);
    group->addButton(ui->userButton_3);
    group->addButton(ui->userButton_4);
    group->addButton(ui->userButton_5);

    group->setId(ui->userButton_0, 0);
    group->setId(ui->userButton_1, 1);
    group->setId(ui->userButton_2, 2);
    group->setId(ui->userButton_3, 3);
    group->setId(ui->userButton_4, 4);
    group->setId(ui->userButton_5, 5);

    connect(ui->userButton_0, SIGNAL(clicked()), this, SLOT(sellectOneUser()));
    connect(ui->userButton_1, SIGNAL(clicked()), this, SLOT(sellectOneUser()));
    connect(ui->userButton_2, SIGNAL(clicked()), this, SLOT(sellectOneUser()));
    connect(ui->userButton_3, SIGNAL(clicked()), this, SLOT(sellectOneUser()));
    connect(ui->userButton_4, SIGNAL(clicked()), this, SLOT(sellectOneUser()));
    connect(ui->userButton_5, SIGNAL(clicked()), this, SLOT(sellectOneUser()));
}

Login::~Login()
{
    delete ui;
}

void Login::sellectOneUser()
{
    // 外部做判断，并选择文件夹
    /*int result = QMessageBox::warning(this,tr("警告"), tr("确认导出全部？"),
                                        QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::No)
        return;
    else if (result == QMessageBox::Yes)*/
    {
        QString tmp = QObject::tr("西工大调试人员");
        int id = group->checkedId();
        tmp = AdminDAO::getAdminDAOInstance()->getUserNameByID(id);
        ui->nameedit->setText(tmp);
    }
}

void Login::on_loginButton_clicked()
{
    // 2014-02-18暂时加上fan
    QString username = ui->nameedit->text();
    QString passwd = ui->passwordedit->text();

    int ret = AdminDAO::getAdminDAOInstance()->setCurrentUser(username, passwd);
    if (ret == 0)
    {
        emit enter();
        ClientSetting::getSettingInstance()->setCurrentUser(username);
    }
    else if (ret == 1)
        ui->label->setText(tr("用户名不存在"));
    else
        ui->label->setText(tr("用户名存在，但密码错误"));
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

