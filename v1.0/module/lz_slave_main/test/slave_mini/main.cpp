//fanxiang
#include <QApplication>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QMessageBox>
#include "slaveprogram_mini.h"
#include "setting_slave_mini.h"

// 测试用
#include "form_mini.h"

#include "Windows.h" 
#include <sstream> 
#include <Tlhelp32.h>  

using namespace std;

bool CheckIfRunningProcessByName(const wchar_t * name)  
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
        return true;
    else
        return false;
}  

bool CheckIfRunning(QString programename)
{
    QByteArray ba = programename.toLocal8Bit();
    wstringstream wss;
    wss << ba.constData();
    qDebug() << wss.str().c_str();
    bool isrunning = CheckIfRunningProcessByName(wss.str().c_str());
    if (isrunning)
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                                QObject::tr("%1程序正在运行中，请先关闭正在运行的程序！").arg(programename),
                                QMessageBox::Ok);
        return true;
    }
    return false;
}

int main(int argc, char* argv[])
{
    /**
     * 测试用
     * @author fanxiang
     */
    QApplication a(argc, argv);

    QApplication::addLibraryPath("./plugins");

    //QCoreApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码

    qDebug()<<"*************************************************";
    qDebug()<<"*                                               *";
    qDebug()<<"*             Slave Mini Program                *";
    qDebug()<<"*                                               *";
    qDebug()<<"*************************************************";

    bool isrunning = CheckIfRunning("slave_mini.exe");
    if (isrunning)
    {
        return 1;
    }

    // 加载本地文件存储目录
    QFile pathfile("slave_datastore_path.txt");
    if (!pathfile.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                                QObject::tr("无文件slave_datastore_path.txt，无法加载从控数据存储目录位置"),
                                QMessageBox::Ok);
        return 1;
    }
    else
    {
        QString parentpath = pathfile.readLine().trimmed();
        SlaveMiniSetting::getSettingInstance()->setParentPath(parentpath);
		QString myslaveip = pathfile.readLine().trimmed();
		SlaveMiniSetting::getSettingInstance()->setMySlaveIP(myslaveip);
        QString slaveminiexepath = pathfile.readLine().trimmed();
        SlaveMiniSetting::getSettingInstance()->setSlaveEXEPath(slaveminiexepath);
        QString slavestartbatpath = pathfile.readLine().trimmed();
        SlaveMiniSetting::getSettingInstance()->setSlaveStartupBATPath(slavestartbatpath);
        pathfile.close();
    }

    /**********读取主控IP***********/
    QString network_file = SlaveMiniSetting::getSettingInstance()->getParentPath() + "/system/network_config.xml";
    QFile file1(network_file);
    if (!file1.exists())
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
            QObject::tr("从控机network_config配置文件不存在!") + network_file,
                                QMessageBox::Ok);
        return 1;
    }

    
    // 从控程序默认为连续采集LzCameraCollectingMode::Lz_Camera_Continus;
    // 当在确认有硬触发设备时才采用外触发采集模式采集
    SlaveMiniProgram * w = new SlaveMiniProgram(SlaveMiniSetting::getSettingInstance()->getSlaveEXEPath(), SlaveMiniSetting::getSettingInstance()->getSlaveStartupBATPath());
    //SlaveMiniProgram w = SlaveMiniProgram();
    
    // 配置master_ip和nas_ip
    w->init_NetworkCameraHardwareFile(network_file);

    FormMini f(w);
    f.show();
    //fm.setSlave(&w);
    w->networkConnectInit();

    qDebug() << "endmain!";

    return a.exec();
}
