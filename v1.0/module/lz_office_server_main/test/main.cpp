#include <QtGui/QApplication>
#include <QMessageBox>
#include <QFile>

#include "setting_server.h"
#include "form.h"
#include "serverprogram.h"

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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::addLibraryPath("./plugins");

    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码

    bool isrunning = CheckIfRunning("lzservertest.exe");
    if (isrunning)
    {
        return 1;
    }

    // 加载本地文件存储目录
    QFile pathfile("server_datastore_path.txt");
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
        ServerSetting::getSettingInstance()->setParentPath(parentpath);
        QString nasaddress = pathfile.readLine().trimmed();
        ServerSetting::getSettingInstance()->setNASIPAddress(nasaddress);
        pathfile.close();
    }

    Form f;
    f.show();

    return a.exec();
}
