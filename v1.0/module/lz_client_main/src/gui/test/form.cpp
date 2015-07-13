#include "form.h"
#include "ui_form.h"

#include <QTextCodec>
#include <QFileDialog>
#include "clientprogram.h"
#include "setting_client.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    /*******************************/
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK")); //当QString变量中含有中文时，需要增加这两行代码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

    // 【注意】顺序：先有masterProgramInstance，才能连接信号槽

    ClientProgram::getClientProgramInstance();

    connect(ClientProgram::getClientProgramInstance(),
            SIGNAL(signalMsgToGUI(QString)), this, SLOT(appendMsg(QString)));
    connect(ClientProgram::getClientProgramInstance(),
            SIGNAL(signalErrorToGUI(QString)), this, SLOT(appendError(QString)));

    /*******************************/

}

Form::~Form()
{
    delete ui;
}

/*******************************/
void Form::appendMsg(QString msg)
{
    ui->statusArea->append(msg);
}

void Form::appendError(QString msg)
{
    ui->statusArea->append(msg);
}


void Form::on_pushButton_clicked()
{
    ClientProgram::getClientProgramInstance()->applyForModifiy(QString(tr("兰青_20140122")), -1, QString(tr("实验室隧道")), "20140217", "fanxiang");
}

void Form::on_pushButton_2_clicked()
{
    ClientProgram::getClientProgramInstance()->askForAvalibleDirectory();
}

void Form::on_pushButton_3_clicked()
{
    ClientProgram::getClientProgramInstance()->askForRawImages(QString(tr("兰青_20140122")), "A1",1,12345,50);
}

void Form::on_pushButton_4_clicked()
{
    ClientProgram::getClientProgramInstance()->finishModify(QString(tr("兰青_20140122")), 216, QString(tr("实验室隧道")), "20140217", "fanxiang");
}

void Form::on_pushButton_5_clicked()
{
    ClientProgram::getClientProgramInstance()->send_file(tr("123456.ppt"));
}

void Form::on_pushButton_6_clicked()
{
    QString openFileDir = ClientSetting::getSettingInstance()->getParentPath();
    QString filename = QFileDialog::getOpenFileName(this, tr("发送文件"), openFileDir, "(*.*)");

    ClientProgram::getClientProgramInstance()->send_file(filename);
}