#include <QtGui>
#include <QtGui/QApplication>
#include <QTextCodec>
#include <QMessageBox>
#include <QTimer>
#include <QSplashScreen>
#include <QDebug>
#include <QTime>
#include <QDateTime>
#include <QFileInfo>

#include "windowmanager.h"

#include "masterprogram.h"
#include "setting_master.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(icons);

    QApplication a(argc, argv);

    // 系统图标
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }

    QApplication::setQuitOnLastWindowClosed(false);

    QApplication::addLibraryPath("./plugins");

    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码

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
        qDebug() << QObject::tr("无文件master_datastore_path.txt，无法加载主控机数据存储目录位置");
        return 1;
    }
    else
    {
        QString parentpath = file.readLine().trimmed();
        MasterSetting::getSettingInstance()->setParentPath(parentpath);
        file.close();
    }

    // 【标记2】此处作废-因为MasterProgram类的run函数作废
    // 【注意】顺序：先有masterProgramInstance，才run，能连接信号槽
    //MasterProgram::getMasterProgramInstance()->run();
    /*******************************/
    MainWidget *w = new MainWidget(true, Collecting);
    w->show();

    // 【注意】顺序：先有masterProgramInstance，才能连接信号槽
    MasterProgram::getMasterProgramInstance();
    // connect函数必在MasterProgram::getMasterProgramInstance()之后
    MasterProgram::getMasterProgramInstance()->init();

    splash->showMessage(QObject::tr("正在启动程序..."),topRight,Qt::yellow);

    splash->finish(w);
    delete splash;

    return a.exec();
}
