#include "form.h"
#include "ui_form.h"

#include <QTextCodec>
#include <QTime>
#include "masterprogram.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    /*******************************/
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK")); //当QString变量中含有中文时，需要增加这两行代码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

    // 【注意】顺序：先有masterProgramInstance，才能连接信号槽

    MasterProgram::getMasterProgramInstance();

    connect(MasterProgram::getMasterProgramInstance(),
            SIGNAL(signalMsgToGUI(QString)), this, SLOT(appendMsg(QString)));
    connect(MasterProgram::getMasterProgramInstance(),
            SIGNAL(signalErrorToGUI(QString)), this, SLOT(appendError(QString)));

    // connect函数必在MasterProgram::getMasterProgramInstance()之后

    MasterProgram::getMasterProgramInstance()->init();

    // 【已经作废】
    // 【注意】顺序：先有masterProgramInstance，才run，能连接信号槽
    //MasterProgram::getMasterProgramInstance()->run();
    /*******************************/
    connect(MasterProgram::getMasterProgramInstance(),
            SIGNAL(signalToLights(HardwareType, QString, HardwareStatus)), this, SLOT(changeHardwareStatus(HardwareType, QString, HardwareStatus)));

    ui->slave1Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));
    ui->slave2Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));
    ui->slave3Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));
    ui->slave4Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));
    ui->slave5Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));
    ui->slave6Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));
    ui->slave7Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));
    ui->slave8Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));
    ui->backupLabel_net->setPixmap(QString::fromUtf8(":image//lightred.png"));
}

void Form::changeHardwareStatus(HardwareType type, QString hardwareidstr, HardwareStatus status)
{
    if (type == Slave_Computer_Type)
    {
        int hardwareid = hardwareidstr.toInt();
        if (status == Hardware_Off)
            switch (hardwareid)
            {
                case 1:ui->slave1Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));break;
                case 2:ui->slave2Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));break;
                case 3:ui->slave3Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));break;
                case 4:ui->slave4Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));break;
                case 5:ui->slave5Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));break;
                case 6:ui->slave6Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));break;
                case 7:ui->slave7Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));break;
                case 8:ui->slave8Label_net->setPixmap(QString::fromUtf8(":image//lightred.png"));break;
                case 9:ui->backupLabel_net->setPixmap(QString::fromUtf8(":image//lightred.png"));break;
                default:;
            }
        else if (status == Hardware_OnButFree)
            switch (hardwareid)
            {
                case 1:ui->slave1Label_net->setPixmap(QString::fromUtf8(":image//lightyellow.png"));break;
                case 2:ui->slave2Label_net->setPixmap(QString::fromUtf8(":image//lightyellow.png"));break;
                case 3:ui->slave3Label_net->setPixmap(QString::fromUtf8(":image//lightyellow.png"));break;
                case 4:ui->slave4Label_net->setPixmap(QString::fromUtf8(":image//lightyellow.png"));break;
                case 5:ui->slave5Label_net->setPixmap(QString::fromUtf8(":image//lightyellow.png"));break;
                case 6:ui->slave6Label_net->setPixmap(QString::fromUtf8(":image//lightyellow.png"));break;
                case 7:ui->slave7Label_net->setPixmap(QString::fromUtf8(":image//lightyellow.png"));break;
                case 8:ui->slave8Label_net->setPixmap(QString::fromUtf8(":image//lightyellow.png"));break;
                case 9:ui->backupLabel_net->setPixmap(QString::fromUtf8(":image//lightyellow.png"));break;
                default:;
            }
    }
    else if (type == Camera_Type)
    {

    }
    else // type == LaserType
    {

    }
}


Form::~Form()
{
    delete ui;
}

void Form::on_pushButton_clicked()
{
    long start_mile = 1100;
    long end_mile = 20000;
    MasterProgram::getMasterProgramInstance()->collect_EndCurrentTunnelFiles(start_mile, end_mile);
}

/*******************************/
void Form::appendMsg(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
}

void Form::appendError(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
}

void Form::on_pushButton_2_clicked()
{
    //MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer()->sendFileToSlaves(tr("file.rar"));
    MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer()->sendFileToSlaves(tr("D:\\兰新下行-随意综合-2006-12-15.pdf"));
    //MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer()->sendFileToOneSlave(QHostAddress("10.13.29.95"),"123.xml");
}

void Form::on_pushButton_3_clicked()
{
    MasterProgram::getMasterProgramInstance()->calculate_Start();
}

void Form::on_pushButton_4_clicked()
{
    MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer()->sendFileToSlaves("666.xml");
}

void Form::on_pushButton_5_clicked()
{
    MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer()->sendMessageToOneSlave(2, "hahaha");
}

void Form::on_pushButton_6_clicked()
{
    MasterProgram::getMasterProgramInstance()->askForRawImages(QString("plan_兰青下行_20140221.xml").toLocal8Bit().constData(), 1, 207, 12345, 40);
}

void Form::on_pushButton_8_clicked()
{
    MasterProgram::getMasterProgramInstance()->backup_Start();
}

void Form::on_pushButton_9_clicked()
{
    MasterProgram::getMasterProgramInstance()->backup_Stop();
}

void Form::on_pushButton_7_clicked()
{
    MasterProgram::getMasterProgramInstance()->calculate_Stop();
}

void Form::on_pushButton_10_clicked()
{
    MasterProgram::getMasterProgramInstance()->workingMode_Setting(QString("兰青下行_20140221.proj").toLocal8Bit().constData(), 123, QString("兰青下行").toLocal8Bit().constData(), "20140115", Collecting, true);
}

void Form::on_pushButton_11_clicked()
{
    MasterProgram::getMasterProgramInstance()->workingMode_Setting(QString("兰青下行_20140221.proj").toLocal8Bit().constData(), 123, QString("兰青下行").toLocal8Bit().constData(), "20140115", Calculating_Backuping, true);
}

void Form::on_pushButton_12_clicked()
{
    MasterProgram::getMasterProgramInstance()->workingMode_Setting(QString("兰青下行_20140221.proj").toLocal8Bit().constData(), 123, QString("兰青下行").toLocal8Bit().constData(), "20140115", Preparing, true);
}
