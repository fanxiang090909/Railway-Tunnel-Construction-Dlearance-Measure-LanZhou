#include <QtGui/QApplication>
#include <QTextCodec>
#include <QMessageBox>
#include <QTimer>
#include <QSplashScreen>
#include <QDebug>
#include <QTime>
#include <QDateTime>
#include <QFileInfo>
#include <QtGui>

#include "login.h"
#include "windowmanager.h"

#include "masterprogram.h"
#include "xmlnetworkfileloader.h"
#include "serverthread.h"

#include "connectdb.h"
#include "xmlcheckedtaskfileloader.h"
#include "xmltaskfileloader.h"

#include "check_task.h"

#include "setting_master.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::addLibraryPath("./plugins");

    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码

    Q_INIT_RESOURCE(icons);

    // 系统图标
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    QSplashScreen *splash = new QSplashScreen();
    splash->setPixmap(QPixmap(":/image/start.png"));
    QTimer::singleShot(3000, splash, SLOT(close()));
    splash->show();
    Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
    splash->showMessage(QObject::tr("正在初始化..."),topRight,Qt::yellow);

    // 加载本地文件存储目录
    QFile file("master_datastore_path.txt");
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                              QObject::tr("无文件master_datastore_path.txt，无法加载主控机数据存储目录位置"),
                              QMessageBox::Ok);
        return 1;
    }
    else
    {
        QString parentpath = file.readLine().trimmed();
        MasterSetting::getSettingInstance()->setParentPath(parentpath);
        
        double defaultDistanceMode = 0.5103;
        QString tmpbastr = file.readLine().trimmed();
        if (tmpbastr.split(':').length() >= 2)
            defaultDistanceMode = tmpbastr.split(':').at(1).toDouble();
        MasterSetting::getSettingInstance()->setDefaultDistanceMode(defaultDistanceMode);
        file.close();
    }

    // 【标记2】此处作废-因为MasterProgram类的run函数作废
    // 【注意】顺序：先有masterProgramInstance，才run，能连接信号槽
    //MasterProgram::getMasterProgramInstance()->run();
    /*******************************/
    // 【作废】@author 范翔 20141029改为计算备份分开界面
    //MainWidget *w = new MainWidget(true, Calculating_Backuping);
    //w->show();

    // 【注意】顺序：先有masterProgramInstance，才能连接信号槽
    // 【注意】顺序：先有masterProgramInstance，才能连接信号槽
    MasterProgram::getMasterProgramInstance();
    // connect函数必在MasterProgram::getMasterProgramInstance()之后
    MasterProgram::getMasterProgramInstance()->init();

    /**********读取主控network_config配置***********/
    QString newpath = MasterSetting::getSettingInstance()->getParentPath() + "/system/network_config.xml";
    QFile file1(newpath);
    if(!file1.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                              QObject::tr("主控机network_config配置文件不存在，") + newpath,
                              QMessageBox::Ok);
        file1.close();
        return 1;
    }
    file1.close();

    MasterProgram::getMasterProgramInstance()->init_NetworkConfigFile(newpath);

    QString nasaccessip = NetworkConfigList::getNetworkConfigListInstance()->getMasterBackupNasIP();
    QString dbserverip = NetworkConfigList::getNetworkConfigListInstance()->getDBServerIP();
    
    MasterSetting::getSettingInstance()->setNASAccessIP(nasaccessip);
    
    splash->showMessage(QObject::tr("正在启动程序..."),topRight,Qt::yellow);

    if (dbserverip.compare("") == 0)
    {
        QMessageBox::warning(0, QObject::tr("启动警告"),
                                QObject::tr("主控机数据库服务器IP未配置%2").arg(dbserverip),
                              QMessageBox::Ok);
    }

    CREATE_MYSQL_CONNECTION(dbserverip);
    WindowManager maw;

    splash->finish(&maw);

    delete splash;

    return a.exec();
}
