#include <QtGui/QApplication>
#include <QTextCodec>
#include <QMessageBox>
#include <QTimer>
#include <QSplashScreen>
#include <QFile>
#include <QtGui>

#include "login.h"
#include "windowmanager.h"

#include "setting_client.h"
#include "clientprogram.h"

#include "connectdb.h"
#include "settingdb.h"
#include "manage_line.h"
#include "manage_tunnel.h"
#include "tunneldatamodel.h"
#include "xmlsynthesisheightsfileloader.h"

#include "Windows.h" 
#include <sstream> 
#include <Tlhelp32.h>  

using namespace std;

int CheckIfRunningProcessByName(const wchar_t * name)  
{  
    DWORD pid = 0;  
      
    // Create toolhelp snapshot.  
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
    PROCESSENTRY32W process;  
    ZeroMemory(&process, sizeof(process));  
    process.dwSize = sizeof(process);  
  
    std::vector<DWORD> pids;  
    // Walkthrough all processes.  
    if (Process32FirstW(snapshot, &process))  
    {  
        do  
        {  
            // Compare process.szExeFile based on format of name, i.e., trim file path  
            // trim .exe if necessary, etc.  
            if (!wcscmp(process.szExeFile, name))  
            {  
               pid = process.th32ProcessID;   
               pids.push_back(pid);  
            }  
        } while (Process32NextW(snapshot, &process));  
    }  
  
    size_t size = pids.size();  
    if (size > 1)
        return 2;
    else if (size == 1)
        return 1;
    else
        return 0;
}  

bool CheckIfRunning(QString programename, bool iscurrent)
{
    QByteArray ba = programename.toLocal8Bit();
    wstringstream wss;
    wss << ba.constData();
    qDebug() << wss.str().c_str();
    int isrunning = CheckIfRunningProcessByName(wss.str().c_str());
    if ((iscurrent && isrunning == 2) || (!iscurrent && isrunning > 0))
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                                QObject::tr("%1程序正在运行中，请先关闭正在运行的程序！").arg(programename),
                                QMessageBox::Ok);
        return true;
    }
    return false;
}

/**
 * @author fanxiang注 需要连接器
 * QtCore4.lib QtCored4.lib QtGui4.lib QtGuid4.lib QtNetwork4.lib QtNetworkd4.lib
 * QtSql4.lib QtSqld4.lib 
 * 在VS2012（VS2010）中调试时需要添加链接库，debug中配置，或者是用 #pragma comment声明
 * 在QTCreator中需要在pro中加CONFIG+=qaxcontainer
 * QAxContainerd.lib EXCEL的读取 Using ActiveX controls and COM in Qt 详见 http://qt-project.org/doc/qt-4.8/activeqt-container.html
 */
//#pragma comment(lib,"QAxContainerd.lib")

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

    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

    bool isrunning = CheckIfRunning("lanzhouclient2013.exe", true);
    if (isrunning)
    {
        return 1;
    }

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

    QSplashScreen *splash = new QSplashScreen();
    splash->setPixmap(QPixmap("image/start.png"));
    QTimer::singleShot(3000, splash, SLOT(close()));
    splash->show();
    Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
    splash->showMessage(QObject::tr("正在加载网络配置数据..."),topRight,Qt::yellow);

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
        delete splash;
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
        DatabaseSetting::getDBSettingInstance()->setIPAddress(dbserverip);
        file.close();

        if (dbserverip.compare("") == 0)
        {
            QMessageBox::critical(0, QObject::tr("启动错误"),
                              QObject::tr("数据库服务器IP未配置,无法连接数据库服务器!"),
                              QMessageBox::Ok);
            delete splash;
            return 1;
        }
        else
        {
            if (!CREATE_MYSQL_CONNECTION(dbserverip))
            {
                delete splash;
                return 1;
            }
        }
    }

    XMLSynthesisHeightsFileLoader * ff3 = new XMLSynthesisHeightsFileLoader(ClientSetting::getSettingInstance()->getParentPath() + "/system/output_heights.xml");
    bool ret = ff3->loadFile();
    delete ff3;
    if (!ret)
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                              QObject::tr("无法加载高度配置文件!"),
                              QMessageBox::Ok);
        qDebug() << QObject::tr("无法加载高度配置文件!");
        return 1;
    }

    WindowManager maw;
    splash->showMessage(QObject::tr("正在连接办公室服务器..."),topRight,Qt::yellow);

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

    splash->showMessage(QObject::tr("正在启动程序..."),topRight,Qt::yellow);
    splash->finish(&maw);
    delete splash;

    return a.exec();
}
