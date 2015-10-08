#include "clientprogram.h"
#include "setting_client.h"

#include <QFile>

/**
 * 办公室底层网络消息处理类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-11-23
 */

// singleton设计模式，静态私有实例变量
ClientProgram * ClientProgram::clientProgramInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
ClientProgram* ClientProgram::getClientProgramInstance()
{
    if (clientProgramInstance == NULL)
    {
        clientProgramInstance = new ClientProgram();
    }
    return clientProgramInstance;
}

ClientProgram::ClientProgram(QObject *parent) : QObject(parent)
{
    // 是否连接网络的服务器标志
    isinitconnect = false;
}

/**
 * 析构函数，删除线程数组中的实例线程
 */
ClientProgram::~ClientProgram()
{
    if (isinitserverip == true)
        delete client;
}

/**********功能函数********************/
/**
 * 申请图形修正
 * @param projectname 
 * @param tunnelid
 * @param tunnelname
 * @param date
 * @param username
 */
void ClientProgram::applyForModifiy(QString projectname, int tunnelid, QString tunnelname, QString date, QString username)
{
    qDebug() << "applyForModifiy" << projectname << tunnelname;
    if (isinitserverip && isinitconnect)
        client->sendMsgToMaster(QString("5100,projectname=%1,tunnelid=%2,tunnelname=%3,date=%4,username=%5,lock=true").arg(projectname).arg(tunnelid).arg(tunnelname).arg(date).arg(username));
    else if (!isinitserverip)
        qDebug() << tr("服务器IP未配置,无法连接服务器!");
    else
    {
        if (initConnect())
            client->sendMsgToMaster(QString("5100,projectname=%1,tunnelid=%2,tunnelname=%3,date=%4,username=%5,lock=true").arg(projectname).arg(tunnelid).arg(tunnelname).arg(date).arg(username));
		else
            return;
    }
}

/**
 * 完成图形修正
 * @param projectname 
 * @param tunnelid
 * @param tunnelname
 * @param date
 * @param username
 */
void ClientProgram::finishModify(QString projectname, int tunnelid, QString tunnelname, QString date, QString username)
{
    if (isinitserverip && isinitconnect)
        client->sendMsgToMaster(QString("5102,projectname=%1,tunnelid=%2,tunnelname=%3,date=%4,username=%5,lock=false").arg(projectname).arg(tunnelname).arg(tunnelname).arg(date).arg(username));
    else if (!isinitserverip)
        qDebug() << tr("服务器IP未配置,无法连接服务器!");
    else
    {
        if (initConnect())
            client->sendMsgToMaster(QString("5102,projectname=%1,tunnelid=%2,tunnelname=%3,date=%4,username=%5,lock=false").arg(projectname).arg(tunnelname).arg(tunnelname).arg(date).arg(username));
        else
            return;
    }
}

/**
 * 查看服务器可修正隧道目录
 */
void ClientProgram::askForAvalibleDirectory()
{
    if (isinitserverip && isinitconnect)
        client->sendMsgToMaster("5200");
    else if (!isinitserverip)
        qDebug() << tr("服务器IP未配置,无法连接服务器!");
    else
    {
        if (initConnect())
            client->sendMsgToMaster("5200");
        else
            return;
    }
}

/**
 * 已经获得服务器可修正隧道目录
 */
void ClientProgram::hasAvalibleDirectory()
{
    if (isinitserverip && isinitconnect)
        client->sendMsgToMaster("5202");
    else if (!isinitserverip)
        qDebug() << tr("服务器IP未配置,无法连接服务器!");
    else
    {
        if (initConnect())
            client->sendMsgToMaster("5202");
        else
            return;
    }
}

/**
 * 希望查看原始图像 
 * 工程projectname下文件seqno，cameraIndex从startFramecounter到(startFramecounter+frameNum)的帧图片
 * @param projectname
 * @param cameraIndex
 * @param seqno
 * @param startFramecounter 
 * @param frameNum 帧数
 */
void ClientProgram::askForRawImages(QString projectname, QString cameraIndex, int seqno, _int64 startFramecounter, int frameNum)
{
    if (isinitserverip && isinitconnect)
        client->sendMsgToMaster(QString("5000,projectname=%1,camera_index=%2,seqno=%3,start_framecounter=%4,frame_num=%5")
                        .arg(projectname).arg(cameraIndex).arg(seqno).arg(startFramecounter).arg(frameNum));
    else if (!isinitserverip)
        qDebug() << tr("服务器IP未配置,无法连接服务器!");
    else
    {
        if (initConnect())
            client->sendMsgToMaster(QString("5000,projectname=%1,camera_index=%2,seqno=%3,start_framecounter=%4,frame_num=%5")
                        .arg(projectname).arg(cameraIndex).arg(seqno).arg(startFramecounter).arg(frameNum));
        else
            return;
    }
}

/**
 * 解析消息字符串
 * @param msg 自定义的消息，这里解析从服务器传到客户端的消息
 * @see 《兰州项目网络消息设计v2.0.ppt》版本v2.0
 */
void ClientProgram::parseMsg(QString msg)
{
    QStringList strList = msg.split(",", QString::SkipEmptyParts);
    if (strList.length() < 1)
    {
        qDebug() << tr("解析字符出错") << msg;
        return;
    }
    int msgid = strList.value(0).toInt();

    if (msgid <= 0)
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
                // disconnect
                emit signalMsgToGUI(QObject::tr("[客户端] 与服务器断开连接成功"));
                isinitconnect = false;
                break;
            }
            case 1: 
            {
                // connect
                emit signalMsgToGUI(QObject::tr("[客户端] 与服务器建立连接成功"));
                isinitconnect = true;
                break;
            }
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
                    emit signalErrorToGUI(QObject::tr("[客户端] 接收服务器的文件%1时打开文件失败").arg(filename));
                else
                    emit signalErrorToGUI(QObject::tr("[客户端] 发送至服务器的文件%1时打开文件失败").arg(filename));
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
                    emit signalErrorToGUI(QObject::tr("[客户端] 接收服务器的文件%1出错%2").arg(filename).arg(error));
                else
                    emit signalErrorToGUI(QObject::tr("[客户端] 发送至服务器的文件%1出错%2").arg(filename).arg(error));
                break;
            }
            default:;
        }
        //qDebug() << msgid;
	}
    else if (msgid > 5100)
    {
        switch(msgid)
        {
            case 5101:
            {
                //5101,projectname=xxx,tunnelid=xx,tunnelname=xxx,date=xxx,canmodify=true（可以修正）
                //5101,projectname=xxx,tunnelid=xx,tunnelname=xxx,date=xxx,canmodify=false,currentuser=xx（不能修正，有人正在修正）
                if (strList.length() < 6)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                QString projectname = strList.at(1).mid(12);
                int tunnelid = strList.at(2).mid(9).toInt();
                QString tunnelname = strList.at(3).mid(11);
                QString date = strList.at(4).mid(5);
                QString appOp = strList.at(5).mid(10);
                bool op;
                if (appOp.compare("true") == 0)
                {
                    op = true;
                    bool ret = ClientSetting::getSettingInstance()->setCanEditing(projectname, tunnelname, true);
                    if (ret)
                        emit signalMsgToGUI(QObject::tr("[服务器] 申请修正工程%1下隧道%2成功").arg(projectname).arg(tunnelname));
                }
                else
                {
                    op = false;
                    QString username = strList.at(6).mid(12);
                    if (username.compare(ClientSetting::getSettingInstance()->getCurrentUser()) != 0)
                    {
                        bool ret = ClientSetting::getSettingInstance()->setCanEditing(projectname, tunnelname, false);
                        emit signalMsgToGUI(QObject::tr("[服务器] 申请修正工程%1隧道%2失败，用户%3从%4开始正在修正").arg(projectname).arg(tunnelname).arg(username).arg(date));
                    }
                    else  // 正在修正用户就是自己
                        emit signalMsgToGUI(QObject::tr("[服务器] 申请修正工程%1下隧道%2成功").arg(projectname).arg(tunnelname));

                }
                break;
            }
            case 5103: // 5103,projectname=xx,tunnelid=xxx,tunnelname=xxx,date=xxx.receive=true
            {
                if (strList.length() < 6)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                QString projectname = strList.at(1).mid(12);
                int tunnelid = strList.at(2).mid(9).toInt();
                QString tunnelname = strList.at(3).mid(11);
                QString date = strList.at(4).mid(6);
                QString appOp = strList.at(5).mid(8);
                bool op = false;
                if (appOp.compare("true") == 0)
                    emit signalMsgToGUI(QObject::tr("[服务器] 服务器接收工程%1隧道%2修正结果文件成功").arg(projectname).arg(tunnelname));
                else
                    emit signalMsgToGUI(QObject::tr("[服务器] 服务器接收工程%1隧道%2修正结果文件失败").arg(projectname).arg(tunnelname));
                break;
            }
            default:;
        }
    }
    else  // 其他（数据操作服务器，暂不考虑）
        return;
}

/******************************/
bool ClientProgram::initServerIP()
{
    QString tmpstr = ClientSetting::getSettingInstance()->getServerIP();
    if (tmpstr.compare("") == 0)
    {
        qDebug() << tr("服务器IP未配置,无法连接服务器!");
        isinitserverip = false;
        return false;
    }

    // @author 范翔 @date 20151003修改端口号配置
    client = new Client(tmpstr, false, 9425, 7778, 8889); //发送消息类的初始化
    // @范翔
    connect(client, SIGNAL(transmitmsg(QString)), this, SLOT(parseMsg(QString)),Qt::DirectConnection);//这个函数主要用于传递从tcpsend得到的信号，在这里进行处理
    isinitserverip = true;
    return true;
}

bool ClientProgram::initConnect()
{
    if (isinitconnect == true)
    {
		qDebug() << tr("已经连接服务器，无需再连");
        return true;
    }

    // 首次运行TCPConnection未创建时，不能连接TCPConnecttion与CviceProgram的的信号槽
    // 需手动触发。只能调用一次
    int ret = client->start();
    if (ret == 1)
    {
        qDebug() << "init wrong, cannot connect to server!";
        emit signalMsgToGUI(QObject::tr("[客户端] 无法连接到服务器"));
        isinitconnect = false;
        return false;
    }
    qDebug() << "init ok";
    isinitconnect = true;
    return true;
}

bool ClientProgram::hasInitConnect()
{
    return isinitconnect;
}

bool ClientProgram::hasInitServerIP()
{
    return isinitserverip;
}

/**
 * 发送普通文件
 * @author 熊雪
 */
void ClientProgram::send_file(QString filename)
{
    QString ip = ClientSetting::getSettingInstance()->getServerIP();
    qDebug() << ip;
    client->sendFileToServer(ip, filename);
}
