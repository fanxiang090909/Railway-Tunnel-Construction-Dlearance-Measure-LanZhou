#include "setvalidframes.h"
#include "ui_setvalidframes.h"
#include <QMessageBox>

/**
 * 采集文件信息校对界面——设置有效帧数界面管理类实现
 * @author 熊雪
 * @version 1.0.0
 * @date 2014-01-20
 */
SetValidFrames::SetValidFrames(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetValidFrames)
{
    ui->setupUi(this);

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(setframes()));

    ui->widget->setVisible(false);
    ui->widget->hide();
    connect(ui->showButton, SIGNAL(toggled(bool)), this, SLOT(showWidget(bool)));

    // 生成临时工程信号槽
    connect(ui->createTempProjectButton, SIGNAL(clicked()), this, SLOT(createTempProject()));

    // 数字输入正则表达式过滤器
    QRegExp regExp("[0-9]{0,10}");
    // 绑入lineEdit :
    ui->lineEdit->setValidator(new QRegExpValidator(regExp, this));
    ui->lineEdit_2->setValidator(new QRegExpValidator(regExp, this));

    ui->lineEdit_3->setValidator(new QRegExpValidator(regExp, this));
    ui->lineEdit_4->setValidator(new QRegExpValidator(regExp, this));
}

SetValidFrames::~SetValidFrames()
{
    delete ui;
}

void SetValidFrames::setframes()
{
    if((ui->lineEdit->text()=="")||(ui->lineEdit_2->text()==""))
    {
        QMessageBox::warning(this,tr("提示"),tr("请先输入需要修改的帧数！"),QMessageBox::Yes|QMessageBox::No);
        return;
    }
    long long startfames = ui->lineEdit->text().toLongLong();
    long long endframes=ui->lineEdit_2->text().toLongLong();
    emit sendframestocheck_task(startfames,endframes);
    this->close();
}

void SetValidFrames::on_pushButton_3_clicked()//重置按钮
{
    ui->lineEdit->setText("");
    ui->lineEdit_2->setText("");
}

void SetValidFrames::setShowWidgetDisVisible(bool ifshow)
{
    ui->showButton->setVisible(false);
}

/** 
 * 是否显示下方的Widget
 */
void SetValidFrames::showWidget(bool ifshow)
{
    if (ifshow)
    {
        ui->showButton->setText(QObject::tr("收回"));
        ui->lineEdit->setEnabled(false);
        ui->lineEdit_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
        ui->pushButton->setEnabled(false);
        ui->widget->setVisible(true);
        ui->widget->show();
    }
    else
    {
        ui->showButton->setText(QObject::tr("展开"));
        ui->lineEdit->setEnabled(true);
        ui->lineEdit_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        ui->pushButton->setEnabled(true);
        ui->widget->setVisible(false);
        ui->widget->hide();
    }
}

/**
 * 生成临时工程槽函数
 */
void SetValidFrames::createTempProject()
{
    if((ui->lineEdit_3->text()=="")||(ui->lineEdit_4->text()==""))
    {
        QMessageBox::warning(this,tr("提示"),tr("请先输入需要修改的帧数！"),QMessageBox::Yes|QMessageBox::No);
        return;
    }
    long long startfames = ui->lineEdit_3->text().toLongLong();
    long long endframes=ui->lineEdit_4->text().toLongLong();
    emit sendframestocheck_task_createtmppro(startfames,endframes);
}