//fanxiang
#include <QApplication>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QMessageBox>
#include "clientprogram.h"
#include "setting_client.h"

// 测试用
#include "form.h"

#pragma comment(lib,"QtCored4.lib")
#pragma comment(lib,"QtNetworkd4.lib")

int main(int argc, char *argv[])
{
    /**
     * 测试用
     * @author fanxiang
     */
    QApplication a(argc, argv);

    //QCoreApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码

    qDebug()<<"*************************************************";
    qDebug()<<"*                                               *";
    qDebug()<<"*            Office Client Program              *";
    qDebug()<<"*                                               *";
    qDebug()<<"*************************************************";

    
    // 加载本地文件存储目录
    QFile pathfile("client_datastore_path.txt");
    if (!pathfile.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                              QObject::tr("无文件client_datastore_path.txt，无法加载客户端数据存储目录位置"),
                              QMessageBox::Ok);
        return 1;
    }
    else
    {
        QString parentpath = pathfile.readLine().trimmed();
        ClientSetting::getSettingInstance()->setParentPath(parentpath);
        ClientSetting::getSettingInstance()->setClientTmpLocalParentPath(parentpath);
        pathfile.close();
    }

    /**********读取服务器IP***********/
    QString serverip = "";
    QString dbserverip = "";
    QString nasaccessip = "";

    QFile file(ClientSetting::getSettingInstance()->getParentPath() + "/system/network_serverip_client.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                              QObject::tr("办公室服务器IP配置文件不存在，无法连接办公室服务器，") + ClientSetting::getSettingInstance()->getParentPath() + "/system/network_serverip_client.txt",
                              QMessageBox::Ok);
        return 1;
    }
    else
    {
        nasaccessip = QString(file.readLine().trimmed()).split(":", QString::SkipEmptyParts).value(1);
        serverip = QString(file.readLine().trimmed()).split(":", QString::SkipEmptyParts).value(1);
        dbserverip = QString(file.readLine().trimmed()).split(":", QString::SkipEmptyParts).value(1);
        // 设置主控IP
        ClientSetting::getSettingInstance()->setNASAccessIP(nasaccessip);
        ClientSetting::getSettingInstance()->setServerIP(serverip);
        ClientSetting::getSettingInstance()->setDBServerIP(dbserverip);
        // 数据库配置的IP地址
        //DatabaseSetting::getDBSettingInstance()->setIPAddress(dbserverip);
        file.close();

        /*if (dbserverip.compare("") == 0)
        {
            QMessageBox::critical(0, QObject::tr("启动错误"),
                              QObject::tr("数据库服务器IP未配置,无法连接数据库服务器!"),
                              QMessageBox::Ok);
            return 1;
        }
        else
        {
            if (!CREATE_MYSQL_CONNECTION(dbserverip))
            {
                delete splash;
                return 1;
            }
        }*/
    }

    /*XMLSynthesisHeightsFileLoader * ff3 = new XMLSynthesisHeightsFileLoader(ClientSetting::getSettingInstance()->getParentPath() + "/system/output_heights.xml");
    bool ret = ff3->loadFile();
    delete ff3;
    if (!ret)
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                              QObject::tr("无法加载高度配置文件!"),
                              QMessageBox::Ok);
        qDebug() << QObject::tr("无法加载高度配置文件!");
        return 1;
    }*/

    if (serverip.compare("") == 0)
    {
        QMessageBox::warning(0, QObject::tr("启动警告"),
                              QObject::tr("办公室服务器IP未配置,无法连接办公室服务器!"),
                              QMessageBox::Ok);
        qDebug() << QObject::tr("办公室服务器IP未配置,无法连接办公室服务器!");
        return 1;
    }
    else
    {
        bool ret = ClientProgram::getClientProgramInstance()->initServerIP();
        if (ret == false)
        {
            qDebug() << QObject::tr("办公室服务器网络IP未配置!");
            // return 1; 去掉退出，可在界面选择后边连接
        }
        ret = ClientProgram::getClientProgramInstance()->initConnect();
        if (ret == false)
        {
            qDebug() << QObject::tr("网络无法连接到办公室服务器!");
            // return 1; 去掉退出，可在界面选择后边连接
        }
    }

    if (nasaccessip.compare("") == 0)
    {
        QMessageBox::warning(0, QObject::tr("启动警告"),
                          QObject::tr("网络存储NAS设备IP未配置,无法连接到NAS!"),
                          QMessageBox::Ok);
    }


    Form f;
    f.show();

    qDebug() << "endmain!";

    return a.exec();
}
