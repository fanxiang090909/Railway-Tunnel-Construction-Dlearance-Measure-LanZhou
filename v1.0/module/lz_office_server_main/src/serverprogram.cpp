#include "serverprogram.h"
#include <QHostInfo>
#include <QStringList>

#include "editing_list.h"

/**
 * 办公室服务器程序驱动类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-2-11
 */
// singleton设计模式，静态私有实例变量
ServerProgram * ServerProgram::serverProgramInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
ServerProgram* ServerProgram::getServerProgramInstance()
{
    if (serverProgramInstance == NULL)
    {
        serverProgramInstance = new ServerProgram();
    }
    return serverProgramInstance;
}

// singleton单一设计模式，构造函数私有
ServerProgram::ServerProgram(QObject *parent) :
    QObject(parent)
{
    qDebug() << "main thread:" << QThread::currentThreadId();
    multiThreadTcpServer = NULL;
}

ServerProgram::~ServerProgram()
{
    delete multiThreadTcpServer;
}

void ServerProgram::init()
{
    // 获取主机名。
    QString localHostName = QHostInfo::localHostName();
    // 获取本机的IP地址。
    QHostInfo info = QHostInfo::fromName(localHostName);
    QString myip;
    //qDebug() << localHostName << " IP Address:"<< info.addresses().value(0).toIPv4Address();
    for (int i = 0; i < info.addresses().size(); i++)
    {
        if ((info.addresses().value(i) != QHostAddress::LocalHost) && (info.addresses().value(i).protocol() == QAbstractSocket::IPv4Protocol))
        {
            myip = info.addresses().value(i).toString();
            qDebug() << localHostName << " IP Address:" << myip;
        }
    }
    qDebug() << localHostName << "local network IP Address:" << myip;

    // 程序初始化
    qDebug() << "Begin init...";
    emit signalMsgToGUI(QObject::tr("[服务器] 程序初始化"));

    // @author 范翔 @date 20151003修改端口号配置
    multiThreadTcpServer = new OfficeMultiThreadTcpServer(this, 9425, 7778, 8889);

    connect(multiThreadTcpServer, SIGNAL(signalMsgToMaster(QString)), this, SLOT(parseMsg(QString)));
    connect(multiThreadTcpServer, SIGNAL(signalErrorToMaster(QString)), this, SLOT(parseMsg(QString)));
    //QObject::connect(multiThreadTcpServer,SIGNAL(newConnection()),this,SLOT(newReceiverConnectToSlave())); //新连接信号触发，调用newConnect()槽函数，这个跟信号函数一样，其实你可以随便取。

    // 初始化完成
    qDebug() << "End init...";

    emit signalMsgToGUI(QObject::tr("[服务器] 程序初始化结束"));

    // 等待客户端初始化完成
    // 可以开始采集
}

/**
 * 返回多线程服务器类指针
 */
OfficeMultiThreadTcpServer * ServerProgram::getMultiThreadTcpServer()
{
    return multiThreadTcpServer;
}

/**
 * !!现在这个函数多余了2013-09-24fan，
 * ||如果需要应该在该类的init函数中去掉注释的connect两句话
 * 检测到server的msg，向上层抛出，这里不做处理
 */
void ServerProgram::displayMsg(QString msg)
{
    // 消息向上抛出，不做处理
    //qDebug() << "in master: " << msg;
    //emit (msg);
}

/**
 * 解析消息字符串
 * @param msg 自定义的消息，这里解析从客户端机传到服务器机的消息
 * @see 《兰州项目网络消息设计v2.0.ppt》版本v2.0
 */
void ServerProgram::parseMsg(QString msg)
{
    QStringList strList = msg.split(",", QString::SkipEmptyParts);
    if (strList.length() < 2)
    {
        qDebug() << tr("解析字符出错") << msg;
        return;
    }
    QString clientip = strList.value(0);
    int msgid = strList.value(1).toInt();

    //qDebug() << clientip << msgid;
    if (msgid <= 0)
    {
        QString filename;
        int type = 0; // 文件接收还是发送，0是接收，1是发送
        if (msgid >= -11 && msgid <= -9)
        {
            if (strList.length() < 4)
            {
                qDebug() << tr("解析字符出错") << msg;
                return;
            }
            else
            {
                type = strList.at(2).toInt();
                filename = strList.at(3);
            }
        }
        switch (msgid)
        {
            case 0:emit signalErrorToGUI(QObject::tr("[服务器] 没有连接客户端，无法发送消息"));break;
            case -7:
            {
                emit signalMsgToGUI(QObject::tr("[客户端%1] 连接开启").arg(clientip));
                emit updateIPListSignal(clientip, true);
                break;
            }
            case -8:
            {
                emit signalMsgToGUI(QObject::tr("[客户端%1] 连接断开").arg(clientip));
                emit updateIPListSignal(clientip, false);
                break;
            }
            case -9:
            {
                if (strList.length() < 6)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                int filesize = strList.at(4).toInt();
                int timespend = strList.at(5).toInt();
                if (type == 0)
                    emit signalMsgToGUI(QObject::tr("[服务器] 接收客户端%1的文件%2成功，文件大小：%3KB，用时%4秒").arg(clientip).arg(filename).arg(filesize).arg(timespend));
                else
                    emit signalMsgToGUI(QObject::tr("[服务器] 发送至客户端%1的文件%2成功，文件大小：%3KB，用时%4秒").arg(clientip).arg(filename).arg(filesize).arg(timespend));

                break;
            }
            case -10:
            {
                if (type == 0)
                    emit signalErrorToGUI(QObject::tr("[服务器] 接收客户端%1的文件%2时打开文件失败").arg(clientip).arg(filename));
                else
                    emit signalErrorToGUI(QObject::tr("[服务器] 发送至客户端%1的文件%2时打开文件失败").arg(clientip).arg(filename));
                break;
            }
            case -11:
            {
                if (strList.length() < 5)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                QString error = strList.at(4);
                if (type == 0)
                    emit signalErrorToGUI(QObject::tr("[服务器] 接收客户端%1的文件%2出错%3").arg(clientip).arg(filename).arg(error));
                else
                    emit signalErrorToGUI(QObject::tr("[服务器] 发送至客户端%1的文件%2出错%3").arg(clientip).arg(filename).arg(error));
                break;
            }
            case -20: // 关闭+重启
            {
                emit signalMsgToGUI(QObject::tr("[客户端%1] 重启从控程序成功").arg(clientip));
                break;    
            }
            case -21: // 只关闭
            {
                emit signalMsgToGUI(QObject::tr("[客户端%1] 关闭从控程序成功").arg(clientip));
                break;
            }
            case -22: // 只启动运行
            {
                emit signalMsgToGUI(QObject::tr("[客户端%1] 启动从控程序成功").arg(clientip));
                break;
            }
            case -30:
            {
                emit signalMsgToGUI(QObject::tr("[客户端%1] 从控确认关机").arg(clientip));
                break;
            }
            default:;
        }
    }
    else if (msgid >= 5000)
    {
        switch(msgid)
        {
            case 5000:
            {
                // 5000,projectname=xxx,camera_index=x,seqno=x,start_framecounter=x,frame_num=100(frame_num表示start与end相差距离)
                if (strList.length() < 7)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                QString projectname = strList.at(2).mid(12);
                QString cameraIndex = strList.at(3).mid(13);
                int seqno = strList.at(4).mid(6).toInt();
                long startFramecounter = strList.at(5).mid(19).toLong();
                int frameNum = strList.at(6).mid(10).toInt();
                ///////////TODO

                emit signalErrorToGUI(QObject::tr("[客户端%1] 读取日期%1%1号文件相机%2原始数据的%3号文件结果，帧号从%4开始%5长度")
                                      .arg(clientip).arg(cameraIndex).arg(seqno).arg(startFramecounter).arg(frameNum));
                break;
            }
            case 5100: case 5102:
            {
                //5100, projectname=xxx,tunnelid=xx,tunnelname=xx,date=xxx, username=xx, lock =true（希望获得锁）
                //5102, projectname=xxx,tunnelid=xx,tunnelname=xx,date=xxx, username=xx, lock =false（希望释放锁）
                if (strList.length() < 8)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                QString projectname1 = strList.at(2).mid(12);
                int tunnelid1 = strList.at(3).mid(9).toInt();
                QString tunnelname1 = strList.at(4).mid(11);
                QString date = strList.at(5).mid(5);
                QString username = strList.at(6).mid(9);
                QString appOp = strList.at(7).mid(5);
                //qDebug() << tunnelid << " " << appOp << " " << username;
                bool op = false;

                EditingItem item;
                item.tunnelid = tunnelid1;
                item.begintime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                item.endtime = "";
                item.projectname = projectname1;
                item.tunnelname = tunnelname1;
                item.collectdate = date;
                item.userid = username;

                if (appOp.compare("true") == 0)
                {
                    op = true;
                    emit signalMsgToGUI(QObject::tr("[客户端%1] 用户【%2】申请修正采集工程%3的隧道%4").arg(clientip).arg(username).arg(projectname1).arg(tunnelname1));
                    // 检查是否正在被别人修改
                    checkIfModiiedByOther(clientip, item);
                }
                else
                {
                    op = false;
                    emit signalMsgToGUI(QObject::tr("[客户端%1] 用户【%2】完成修正采集工程%3的隧道%4").arg(clientip).arg(username).arg(projectname1).arg(tunnelname1));
                    //if (EditingList::getEditingListInstance()->exist(item, currentUser))
                    {
                        EditingList::getEditingListInstance()->remove(item);
                        // 通知上层界面，更新列表
                        emit updateEditingProjectListSignal();
                    }
                }
                break;
            }
            case 5200:
            {
                /////////////TODO
                emit signalMsgToGUI(QObject::tr("[客户端%1] 读取服务器目录失败，请求读取").arg(clientip));break;
            }
            case 5202:
            {
                /////////////TODO
                emit signalMsgToGUI(QObject::tr("[客户端%1] 读取服务器目录成功").arg(clientip));break;
            }
            default:;
        }
    }
    else  // 其他（数据操作服务器，暂不考虑）
        return;
}

/*****************图形修正*********************/
/**
 * 收到客户端发来的申请
 * @param toip 客户端IP地址
 * @param item 一个正在修正项的信息，@see EditingItem 包括工程名“线路_日期”、 隧道ID、 隧道名、 开始编辑日期
 */
void ServerProgram::checkIfModiiedByOther(QString toip, EditingItem item)
{
    QString currentUser = "";
    QString startTime = "";
    // 查找是否正在修正item，如果正在修正，给currentUser,startTime赋值
    if (!EditingList::getEditingListInstance()->exist(item, currentUser, startTime))
    {
        EditingList::getEditingListInstance()->pushBack(item);

        // 通知上层界面，更新列表
        emit updateEditingProjectListSignal();

        getMultiThreadTcpServer()->sendMessageToOneSlave(toip, QObject::tr("5101,projectname=%1,tunnelid=%2,tunnelname=%3,date=%4,canmodify=true").arg(item.projectname).arg(item.tunnelid).arg(item.tunnelname).arg(item.begintime));
    }
    else
    {
        getMultiThreadTcpServer()->sendMessageToOneSlave(toip, QObject::tr("5101,projectname=%1,tunnelid=%2,tunnelname=%3,date=%4,canmodify=false,currentuser=%5").arg(item.projectname).arg(item.tunnelid).arg(item.tunnelname).arg(startTime).arg(currentUser));
    }
}

/**
 * 服务器保存隧道图形修正完成反馈给客户端
 * @param toip 客户端IP地址
 * @param projectname 工程名“线路_日期”
 * @param tunnelid 隧道ID
 * @param tunnelname 隧道名
 * @param date 隧道采集日期
 */
void ServerProgram::receiveFileFeedback(QString toip, QString projectname, int tunnelid, QString tunnelname, QString date)
{
    getMultiThreadTcpServer()->sendMessageToOneSlave(toip, QObject::tr("5103,projectname=%1,tunnelid=%2,tunnelname=%3,date=%4,receive=true").arg(projectname).arg(tunnelid).arg(tunnelname).arg(date));
}

void ServerProgram::returnDirectoryFiles(QString toip, QString filename)
{
    //QHostAddress address = QHostAddress(toip);
    getMultiThreadTcpServer()->sendFileToOneSlave(toip, filename);
    emit signalMsgToGUI(QObject::tr("[服务器] 发送目录文件到%1").arg(toip));
}

void ServerProgram::sendCalcuResultData(QString toip, QString filename)
{
    //QHostAddress address = QHostAddress(toip);
    getMultiThreadTcpServer()->sendFileToOneSlave(toip, filename);
    emit signalMsgToGUI(QObject::tr("[服务器] 发送计算结果文件到%1").arg(toip));
}

void ServerProgram::sendRawImagesPackage(QString toip, QString filename)
{
    //QHostAddress address = QHostAddress(toip);
    getMultiThreadTcpServer()->sendFileToOneSlave(toip, filename);
    emit signalMsgToGUI(QObject::tr("[服务器] 发送原始图像压缩包到%1").arg(toip));
}
/*******************************/

/**
 * 重启slave程序
 */
void ServerProgram::resetSlaveProgram()
{
    getMultiThreadTcpServer()->sendMessageToSlaves(QObject::tr("-20"));
    emit signalMsgToGUI(QObject::tr("[服务器] 发送从控程序重启命令"));
}

void ServerProgram::terminateSlaveProgram()
{
    getMultiThreadTcpServer()->sendMessageToSlaves(QObject::tr("-21"));
    emit signalMsgToGUI(QObject::tr("[服务器] 发送从控程序关闭命令"));
}

/**
 * 从控关机
 */
void ServerProgram::shutdownAllSlaves()
{
    getMultiThreadTcpServer()->sendMessageToSlaves(QObject::tr("-30"));
    emit signalMsgToGUI(QObject::tr("[服务器] 发送从控机关机命令"));
}