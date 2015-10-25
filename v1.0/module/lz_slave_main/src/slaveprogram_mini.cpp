#include "slaveprogram_mini.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QDateTime>

#include "LzException.h"
#include "fileoperation.h"
#include "network_config_list.h"
#include "xmlnetworkfileloader.h"
#include "setting_slave_mini.h"

// 终止进程
//************************************
#include <stdio.h>  
#include <string.h> 
#include <windows.h>  
#include <Tlhelp32.h>  
#include <vector>  

void TerminateProcessByName(const wchar_t * name);

void TerminateProcessByName(const wchar_t * name)  
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
  
    CloseHandle(snapshot);  
      
    size_t size = pids.size();  
    for(size_t i = 0; i < size; ++i)  
    {  
        HANDLE one_process = OpenProcess(PROCESS_TERMINATE, FALSE, pids[i]);  
        BOOL res = TerminateProcess(one_process,0);  
        if(res)  
        {  
            WaitForSingleObject(one_process,1000);  
        }  
    }  
}  

//************************************

using namespace std;


/**
 * 从控机程序业务处理类定义
 * @author 范翔
 * @version 1.0.0
 * @date 20151003
 */
//SlaveMiniProgram::SlaveMiniProgram(QObject *parent) : QObject(parent)
SlaveMiniProgram::SlaveMiniProgram(QString initslaveexepath, QString initslavestartupbatpath, QObject *parent) : QObject(parent) 
{
    //issuspend_cal.store(false);
    //qDebug() << tr("构造");
	firstInitNetwork = true;

    slaveexepath = initslaveexepath;  
    slavestartupbatpath = initslavestartupbatpath;
}

SlaveMiniProgram::~SlaveMiniProgram()
{
    //qDebug() << tr("析构");
    if (client != NULL)
        delete client;
}

/**
 * 网络初始化
 */
bool SlaveMiniProgram::networkConnectInit()
{
    // @author fanxiang
    // 如果没有主控，不能连接
    if (SlaveMiniSetting::getSettingInstance()->hasMasterIP() == false)
    {
        //this->WriteLog("connect to master failed!");
        qDebug() << tr("主控机IP未配置,无法连接主控!");
        return false;
    }

    QString tmpstr = SlaveMiniSetting::getSettingInstance()->getMasterIP();
    if (tmpstr.compare("") == 0)
    {
        qDebug() << tr("主控机IP未配置,无法连接主控!");
        return false;
    }

    // @author 范翔 @date 20151003
    client = new Client(tmpstr, true, 9425, 7778, 8889); //发送消息类的初始化
    QFileInfo fileinfo(slaveexepath);
    qDebug() << "path:\t" << fileinfo.path();
    client->setCurrentSavingPath(fileinfo.path()); // EXE，更新程序保存路径

    // 底层连接错误信号槽
    connect(client, SIGNAL(transmitmsg(QString)), this, SLOT(ParseMsg(QString)),Qt::DirectConnection);//这个函数主要用于传递从tcpsend得到的信号，在这里进行处理
    // 首次运行TCPConnection未创建时，不能连接TCPConnecttion与CviceProgram的的信号槽
    // 需手动触发。只能调用一次
    client->start();

    return true;
}

/**
 * 重启从控程序
 */
bool SlaveMiniProgram::restartSlaveProgram()
{
    terminateSlave();
    
    startSlave();   

    return true;
}

void SlaveMiniProgram::terminateSlave()
{
    QFileInfo fileinfo(slaveexepath);
    qDebug() << "path:\t" << fileinfo.path();
    qDebug() << "filename:\t" << fileinfo.fileName();
    
    QByteArray ba = fileinfo.fileName().toLocal8Bit();
    //QByteArray ba = fileinfo.fileName().toLocal8Bit();

    wstringstream wss;
    wss << ba.constData();
    qDebug() << wss.str().c_str();

    // 关闭进程   
    TerminateProcessByName(wss.str().c_str());

}

void SlaveMiniProgram::startSlave()
{
    QFileInfo fileinfo(slavestartupbatpath);
    qDebug() << "path:\t" << fileinfo.path();
    qDebug() << "filename:\t" << fileinfo.fileName();

    // 重新打开程序
    //QProcess pro1(0);
    //pro1.setWorkingDirectory(fileinfo.path());
    //int bret1 = pro1.execute(slavestartupbatpath);
    //qDebug() << "restartSlaveProgram" << bret1;

    QByteArray ba = slavestartupbatpath.toLocal8Bit();
    
    int bret2 = WinExec(ba.constData(), 5);
    qDebug() << "restartSlaveProgram" << bret2;
}

void SlaveMiniProgram::shutdown()
{
    QProcess pro1(0);
    //pro1.setWorkingDirectory(fileinfo.path());
    int bret1 = pro1.execute("shutdown /s /t 1");
}

/**
 * 网络消息字符串解析
 * @author fengmingchen
 * @author fanxiang
 * @date 2013-11-24
 */
void SlaveMiniProgram::ParseMsg(QString msg){

    QStringList strList = msg.split(",", QString::SkipEmptyParts);
 
    int msgid = strList.value(0).toInt();
    qDebug() << "msgid= " << msgid;
    if (msgid <= 0) // 从控机自己通知
    {
        QString filename;
        int type = 0; // 文件接收还是发送，0是接收，1是发送
        if (msgid >= -11 && msgid <= -9)
        {
            if (strList.length() < 3)
            {
                qDebug() << tr("解析字符出错") << msg;
                return;
            }
            else
            {
                type = strList.at(1).toInt();
                filename = strList.at(2);
            }
        }
        switch(msgid)
        {
            case 0:
            {
                // @author 范翔 @date 20140520
                // TODO应该怎么处理？
                // 异常断开后的处理
                /*if (lzAcquizQueue->getIsAcquizing())
                {
                    ToDoMsg toDoMsg;
                    toDoMsg.msg = msg;
                    lzAcquizQueue->pushBack(toDoMsg);
                }*/
                emit signalMsgToGUI(QObject::tr("[从控] 与主控断开连接"));
                break;
            }
            case -1:
                emit signalMsgToGUI(QObject::tr("[从控] 连接到主控"));
                break;
            case -9:
            {
                if (strList.length() < 5)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                int filesize = strList.at(3).toInt();
                int timespend = strList.at(4).toInt();
                if (type == 0)
                    emit signalMsgToGUI(QObject::tr("[客户端] 接收服务器的文件%1成功，文件大小：%2KB，用时%3秒").arg(filename).arg(filesize).arg(timespend));
                else
                    emit signalMsgToGUI(QObject::tr("[客户端] 发送至服务器的文件%1成功，文件大小：%2KB，用时%3秒").arg(filename).arg(filesize).arg(timespend));
                break;
            }
            case -10:
            {
                if (type == 0)
                    emit signalErrorToGUI(QObject::tr("[从控] 接收主控的文件%1时打开文件失败").arg(filename));
                else
                    emit signalErrorToGUI(QObject::tr("[从控] 发送至主控的文件%1时打开文件失败").arg(filename));
                break;
            }
            case -11:
            {
                if (strList.length() < 4)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                QString error = strList.at(3);
                if (type == 0)
                    emit signalErrorToGUI(QObject::tr("[从控] 接收主控的文件%1出错%2").arg(filename).arg(error));
                else
                    emit signalErrorToGUI(QObject::tr("[从控] 发送至主控的文件%1出错%2").arg(filename).arg(error));
                break;
            }
            case -20: // 关闭+重启
            {
                emit signalMsgToGUI(QObject::tr("[从控] 接收到主控的重启命令"));
                restartSlaveProgram();
                emit signalMsgToGUI(QObject::tr("[从控] 重启成功"));
                sendMsgToMaster("-20");
                break;    
            }
            case -21: // 只关闭
            {
                emit signalErrorToGUI(QObject::tr("[从控] 接收到主控的关闭程序进程命令"));
                sendMsgToMaster("-21");
                terminateSlave();
                emit signalErrorToGUI(QObject::tr("[从控] 关闭成功"));
                break;
            }
            case -22: // 只启动运行
            {
                emit signalErrorToGUI(QObject::tr("[从控] 接收到主控的运行程序进程命令"));
                sendMsgToMaster("-22");
                startSlave();
                emit signalErrorToGUI(QObject::tr("[从控] 运行程序成功"));
                break;
            }
            case -30: // 关机
            {
                emit signalErrorToGUI(QObject::tr("[从控] 接收到主控的关机指令"));
                // 
                sendMsgToMaster("-30");
                shutdown();
                break;
            }
            default:;
        }
    }
}

void SlaveMiniProgram::sendParsedMsgToGUI(QString newmsg)
{
    qDebug() << newmsg;
    emit signalMsgToGUI(newmsg);
}

/**
 * 测试用，给主控发消息
 * @author fanxiang
 */
void SlaveMiniProgram::sendMsgToMaster(QString msg)
{
    client->sendMsgToMaster(msg);
}

void SlaveMiniProgram::sendFileToMaster(QString filename)
{
    client->sendFileToServer(SlaveMiniSetting::getSettingInstance()->getMasterIP(), filename);
}

/**
 * 加载网络从控及相机SNID硬件，双目视觉标定文件名称配置文件
 */
bool SlaveMiniProgram::init_NetworkCameraHardwareFile(QString filename)
{
    bool networkok = false;
    // @author fanxiang注文件解析
    qDebug() << "init_NetworkCameraHardwareFile" << filename;
    QFile file0(filename);//file0代表index和camid的文件
    if(file0.exists())
    {
        file0.close();
        qDebug() << QObject::tr("index文件存在,正在解析...");//在控制台上显示
        // @author fanxiang注文件解析

        XMLNetworkFileLoader* ff2 = new XMLNetworkFileLoader(filename);
        bool ret = ff2->loadFile();
        delete ff2;
        if (ret)
        {
            //NetworkConfigList::getNetworkConfigListInstance()->showList();

            // master_ip
            QString masterip = NetworkConfigList::getNetworkConfigListInstance()->getMasterIP();
            SlaveMiniSetting::getSettingInstance()->setMasterIP(masterip);

            ret = NetworkConfigList::getNetworkConfigListInstance()->findSlave(SlaveMiniSetting::getSettingInstance()->getMySlaveIP(),
                                                                      SlaveMiniSetting::getSettingInstance()->getCurrentSlaveModel());
            SlaveMiniSetting::getSettingInstance()->setHasModel(ret);

            SlaveModel & slaveModel = SlaveMiniSetting::getSettingInstance()->getCurrentSlaveModel();

            if (ret)
            {
				emit signalMsgToGUI(QString("[从控] NetworkCameraHardwareConfig文件解析成功%1！").arg(filename));
				emit showSlaveID();
                
                if (!firstInitNetwork)
				{
					//qDebug() << QObject::tr("NetworkCameraHardwareConfig文件解析成功%1！").arg(filename);//在控制台上显示
					networkok = true;
					client->sendMsgToMaster("112");
                    return true;
				}
            }
			else
                emit signalMsgToGUI(QString("[从控] 网络硬件配置文件不存在或解析失败%1,可能文件中不存在本机IP地址%2").arg(filename).arg(SlaveMiniSetting::getSettingInstance()->getMySlaveIP()));
        }
    }
    file0.close();
	if (!firstInitNetwork)
	{
		//qDebug() << QObject::tr("网络硬件配置文件不存在或解析失败,向主控发送请求"));
		client->sendMsgToMaster("110");//index和cameraid文件没有向主控发请求
	}
	firstInitNetwork = false;
    return false;
}

/**
 * 发送普通文件
 * @author 熊雪
 */
void SlaveMiniProgram::send_file(QString filename)
{
    QString ip = SlaveMiniSetting::getSettingInstance()->getMasterIP();
    qDebug() << ip;
    client->sendFileToServer(ip, filename);
}
