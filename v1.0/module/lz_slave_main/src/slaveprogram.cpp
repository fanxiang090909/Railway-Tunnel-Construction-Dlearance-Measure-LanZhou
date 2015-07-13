#include "slaveprogram.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QDateTime>

#include "LzException.h"
#include "LzSerialStorageAcqui.h"
#include "fileoperation.h"
#include "network_config_list.h"
#include "xmlnetworkfileloader.h"
#include "xmltaskfileloader.h"
#include "xmlrealtaskfileloader.h"
#include "xmlcheckedtaskfileloader.h"
#include "setting_slave.h"
#include "plantask_list.h"
#include "realtask_list.h"
#include "checkedtask_list.h"

using namespace std;

/**
 * 从控机程序业务处理类定义
 * @author 范翔
 * @author 熊雪
 * @author 冯明晨
 * @version 1.0.0
 * @date 20141029
 */

/**
 * 从控程序构造函数：传入初始化参数
 * @param initmode 采集模式（外同步触发采集/连续采集）
 *                 当前经过实验,不支持连续采集（因为文件的关闭和创建时间间隔过短可能会出现空指针异常），
 *                              只支持硬触发采集
 */
SlaveProgram::SlaveProgram(LzCameraCollectingMode initmode, QObject *parent)
{
    //issuspend_cal.store(false);
    //qDebug() << tr("构造");
	firstInitNetwork = true;

    lzAcquizQueue = new LzAcquizQueue(this);
    //lzAcquizQueue = new LzAcquizQueue(initmode);
    lzBackupQueue = new LzBackupQueue(this);
    lzCalcQueue = new LzCalcQueue(this);
    lzCorrectQueue = new LzCorrectQueue(this);

    connect(lzAcquizQueue, SIGNAL(signalParsedMsgToSlave(QString)), this, SLOT(sendParsedMsgToGUI(QString)));
    connect(lzAcquizQueue, SIGNAL(signalMsgToMaster(QString)), this, SLOT(sendMsgToMaster(QString)));
    connect(lzBackupQueue, SIGNAL(signalParsedMsgToSlave(QString)), this, SLOT(sendParsedMsgToGUI(QString)));
    connect(lzBackupQueue, SIGNAL(signalMsgToMaster(QString)), this, SLOT(sendMsgToMaster(QString)));
    connect(lzCalcQueue, SIGNAL(signalParsedMsgToSlave(QString)), this, SLOT(sendParsedMsgToGUI(QString)));
    connect(lzCalcQueue, SIGNAL(signalMsgToMaster(QString)), this, SLOT(sendMsgToMaster(QString)));
    connect(lzCalcQueue, SIGNAL(signalFileToMaster(QString)), this, SLOT(sendFileToMaster(QString)));
    connect(lzCorrectQueue, SIGNAL(signalParsedMsgToSlave(QString)), this, SLOT(sendParsedMsgToGUI(QString)));
    connect(lzCorrectQueue, SIGNAL(signalMsgToMaster(QString)), this, SLOT(sendMsgToMaster(QString)));
    connect(lzCorrectQueue, SIGNAL(signalFileToMaster(QString)), this, SLOT(sendFileToMaster(QString)));
}

SlaveProgram::~SlaveProgram()
{
    delete lzAcquizQueue;
    delete lzBackupQueue;
    delete lzCalcQueue;
    delete lzCorrectQueue;
    //qDebug() << tr("析构");
    if (client != NULL)
        delete client;
}

/**
 * 测试界面用
 */
LzAcquizQueue * SlaveProgram::getLzAcquizQueue() { return lzAcquizQueue; }
LzCalcQueue * SlaveProgram::getLzCalcQueue() { return lzCalcQueue; }
LzBackupQueue * SlaveProgram::getLzBackupQueue() { return lzBackupQueue; }
LzCorrectQueue * SlaveProgram::getLzCorrectQueue() { return lzCorrectQueue; }

/**
 * 网络初始化
 */
bool SlaveProgram::networkConnectInit()
{
    // @author fanxiang
    // 如果没有主控，不能连接
    if (SlaveSetting::getSettingInstance()->hasMasterIP() == false)
    {
        //this->WriteLog("connect to master failed!");
        qDebug() << tr("主控机IP未配置,无法连接主控!");
        return false;
    }

    QString tmpstr = SlaveSetting::getSettingInstance()->getMasterIP();
    if (tmpstr.compare("") == 0)
    {
        qDebug() << tr("主控机IP未配置,无法连接主控!");
        return false;
    }

    client = new Client(tmpstr); //发送消息类的初始化
    client->setCurrentSavingPath(SlaveSetting::getSettingInstance()->getParentPath());

    // 底层连接错误信号槽
    connect(client, SIGNAL(transmitmsg(QString)), this, SLOT(ParseMsg(QString)),Qt::DirectConnection);//这个函数主要用于传递从tcpsend得到的信号，在这里进行处理
    // 首次运行TCPConnection未创建时，不能连接TCPConnecttion与CviceProgram的的信号槽
    // 需手动触发。只能调用一次
    client->start();

    return true;
}

/**
 * 网络消息字符串解析
 * @author fengmingchen
 * @author fanxiang
 * @date 2013-11-24
 */
void SlaveProgram::ParseMsg(QString msg){

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
            default:;
        }
    }
    else if (msgid <= 2000)
    {
        switch (msgid)
        {
            case 1003: // 收到软触发采集信号
            {
                ToDoMsg toDoMsg;
                toDoMsg.msg = msg;
                lzAcquizQueue->pushBack(toDoMsg);

                break;
            }
            case 1101: // 收到出洞信号，相机停止拍摄 1101,slaveid=x.endframe_box1camref=x, endframe_box1cam=x, endframe_box2camref=x, endframe_box2cam=x,numofframes=x, start_mile=x,end_mile=x
            {
                // 1101 关闭文件调用collect_end_tunnel(false)
                ToDoMsg toDoMsg;
                toDoMsg.msg = msg;
                lzAcquizQueue->pushBack(toDoMsg);
                // 补充消息-1101：打开下一文件调用collect_start_tunnel()
                ToDoMsg toDoMsg2;
                toDoMsg2.msg = "-1101";
                lzAcquizQueue->pushBack(toDoMsg2);

                break;
            }
            case 1103: // 收到预进洞有效、无效信号 1103,isvalid=true(预进洞有效，false预进洞无效),slaveid=x,endframe_box1camref=x, endframe_box1cam=x, endframe_box2camref=x, endframe_box2cam=x. numofframes=x
            {
                ToDoMsg toDoMsg;
                toDoMsg.msg = msg;
                lzAcquizQueue->pushBack(toDoMsg);
                
                break;
            }
            case 1301: // 停止采集当前线路 1301
            {
                ToDoMsg toDoMsg;
                toDoMsg.msg = msg;
                lzAcquizQueue->pushBack(toDoMsg);
                
                ToDoMsg toDoMsg2;
                toDoMsg2.msg = "0";
                lzAcquizQueue->pushBack(toDoMsg2);

                break;
            }
            case 1401: // 复位从机 1401
            {
                ToDoMsg toDoMsg;
                toDoMsg.msg = msg;
                lzAcquizQueue->pushBack(toDoMsg);
                break;
            }
            default:break;
        }
    }	
    else if (msgid > 2000) 
    {
        switch(msgid)
        {
            case 4101: // 浏览原图计算4101,realfile=“xxxx”,camera_index=x,seqno=x,tunnelid=x,start_framecounter=xx,frame_num=xx(frame_num表示start与end相差距离)
            {
                ToDoMsg toDoMsg;
                toDoMsg.msg = msg;
                lzCorrectQueue->pushBack(toDoMsg);
                //emit signalMsgToGUI(QObject::tr("[主控] 命令:申请隧道Id%1相机号%2的原始图像数据%3文件，帧号从%4开始%5个").arg(tunnelid).arg(cameraindex).arg(seqno).arg(startFrameCounter).arg(frameNum));
                break;
            }
            case 4201:                        //4201,filename=xxx,lineid=xx,linename=xxx,date=xx,operation_type=“collect”(“calculate_backup”采集存储备份)
            case 4202://4202不做检查,直接开始 //4202,filename=xxx,lineid=xx,linename=xxx,date=xx,operation_type=“collect”(“calculate_backup”采集存储备份)
            {
                if (strList.length() < 6)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                QString filename = strList.at(1).mid(9);
                int lineid = strList.at(2).mid(7).toInt();
                QString linename = strList.at(3).mid(9);
                QString date = strList.at(4).mid(5);
                QString operation_type = strList.at(5).mid(15);
                if (msgid == 4201) // 4201
                {
                    qDebug() << QObject::tr("[主控] 命令:告知即将%1，隧道号为%2的隧道%3，任务时间是%4请准备就绪！任务文件为%5").arg(operation_type).arg(lineid).arg(linename).arg(date).arg(filename);
                    emit signalMsgToGUI(tr("[主控] 命令:告知即将%1，隧道号为%2的隧道%3，任务时间是%4请准备就绪！任务文件为%5").arg(operation_type).arg(lineid).arg(linename).arg(date).arg(filename));
                
                    bool rrval = false;
                    if (operation_type.compare("preparing") == 0) // 要做采集
                    {
                        rrval = changeMode_newProjectConfig(LzProjectClass::Main, filename);
                    }
                    else if (operation_type.compare("collect") == 0) // 要做采集
                    {
                        rrval = changeMode_newProjectConfig(LzProjectClass::Collect, filename);
                        if (rrval)
                            this->checkedok(WorkingStatus::Collecting, filename, linename, date);
                    }
                    else if (operation_type.compare("correct") == 0) // 校正
                    {
                        rrval = changeMode_newProjectConfig(LzProjectClass::Main, filename);
                        if (rrval)
                            this->checkedok(WorkingStatus::Correcting, filename, linename, date);
                    }
                    else if (operation_type.compare("calculate_backup") == 0) // 要同时做计算+备份
                    {
                        rrval = changeMode_newProjectConfig(LzProjectClass::Calculate, filename);
                        if (rrval)
                            this->checkedok(WorkingStatus::Calculating_Backuping, filename, linename, date);
                    }
                    else if (operation_type.compare("calculate") == 0) // 计算
                    {
                        rrval = changeMode_newProjectConfig(LzProjectClass::Calculate, filename);
                        if (rrval)
                            this->checkedok(WorkingStatus::Calculating, filename, linename, date);
                    }
                    else if (operation_type.compare("backup") == 0) // 备份
                    {
                        rrval = changeMode_newProjectConfig(LzProjectClass::Backup, filename);
                        if (rrval)
                            this->checkedok(WorkingStatus::Backuping, filename, linename, date);
                    }

                    // 删除没有告知即将，只有确认删除

                }
                else // 4202
                {
                    qDebug() << QObject::tr("[主控] 命令:控制开始%1，隧道号为%2的隧道%3，任务时间是%4请准备就绪！任务文件为%5").arg(operation_type).arg(lineid).arg(linename).arg(date).arg(filename);
                    emit signalMsgToGUI(tr("[主控] 命令:控制开始%1，隧道号为%2的隧道%3，任务时间是%4请准备就绪！任务文件为%5").arg(operation_type).arg(lineid).arg(linename).arg(date).arg(filename));

                    if (operation_type.compare("collect") == 0) // 要做采集
                    {

                        //4202,filename=xxx,lineid=xx,linename=xxx,date=xx,operation_type=“collect”,triggermode="continue"/"hardware"/"software",exposuretime=5000
                        // 设置采集触发模式
                        QString triggermodestr = strList.at(6).mid(12);
                        LzCameraCollectingMode triggermode = Lz_Camera_SoftwareTrigger;
                        int exposuretime = strList.at(7).mid(13).toInt();
                        qDebug() << "triggermode:" << triggermodestr << ", exposuretime:" << exposuretime;
                        if (triggermodestr.compare("continue") == 0)
                            triggermode = Lz_Camera_Continus;
                        else if (triggermodestr.compare("hardware") == 0)
                            triggermode = Lz_Camera_HardwareTrigger;
                        else if (triggermodestr.compare("software") == 0)
                            triggermode = Lz_Camera_SoftwareTrigger;
							
						lzAcquizQueue->setTriggerModeAndExposureTime(triggermode, exposuretime);

                        ToDoMsg toDoMsg;
                        toDoMsg.msg = "-4202";
                        lzAcquizQueue->pushBack(toDoMsg);

                    }
                    else if (operation_type.compare("correct") == 0) // 校正
                    {
                        //TODO;
                    }
                    else if (operation_type.compare("calculate_backup") == 0) // 要同时做计算+备份
                    {
                        // 初始化计算
                        lzCalcQueue->initCalc();
                    }
                    else if (operation_type.compare("calculate") == 0) // 要同时做计算+备份
                    {
                        // 初始化计算
                        lzCalcQueue->initCalc();
                    }
                    else if (operation_type.compare("delete") == 0)
                    {
                        bool rrval = changeMode_newProjectConfig(LzProjectClass::Main, filename);
                        if (rrval)
                            this->checkedok(WorkingStatus::Deleting, filename, linename, date);
                    }
                }
                break;
            }
            case 4301: // 生成临时工程 4301,filename="xxx",lineid=xx,linename="xxx",date=xxx,tunnelid=xx,seqno=x,start_framecounter=xx,frame_num=xx
            {
                ToDoMsg toDoMsg;
                toDoMsg.msg = msg;
                lzCorrectQueue->pushBack(toDoMsg);
                break;
            }
            case 2001:
            {
                ToDoMsg toDoMsg;
                toDoMsg.msg = msg;
                lzCalcQueue->pushBack(toDoMsg);
                break;
            }
            case 2401://暂停计算
            {
                lzCalcQueue->suspend();
                emit signalMsgToGUI(tr("[主控] 命令:暂停计算"));
                break;
            }
            case 3001:
            {
                ToDoMsg toDoMsg;
                toDoMsg.msg = msg;
                lzBackupQueue->pushBack(toDoMsg);
                break;
            }
            case 3401://暂停备份
            {
                lzBackupQueue->suspend();
                emit signalMsgToGUI(tr("[主控] 命令:暂停备份"));
                break;
            }
            
            default:;
        }
    }
}

void SlaveProgram::sendParsedMsgToGUI(QString newmsg)
{
    qDebug() << newmsg;
    emit signalMsgToGUI(newmsg);
}

/**
 * 测试用，给主控发消息
 * @author fanxiang
 */
void SlaveProgram::sendMsgToMaster(QString msg)
{
    client->sendMsgToMaster(msg);
}

void SlaveProgram::sendFileToMaster(QString filename)
{
    client->sendFileToServer(SlaveSetting::getSettingInstance()->getMasterIP(), filename);
}

/**
 * 切换工作模式-采集、备份时新工程配置
 * 设置文件接收路径
 *     工程路径
 *     工程文件.proj解析加载
 * @author 范翔 @date 20140509
 */
bool SlaveProgram::changeMode_newProjectConfig(LzProjectClass projectclass, QString newfilename)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*
    if (lzAcquizQueue->getIsAcquizing() == true)
        return false;
    */

    // 按当前时间创建空目录，等待存储
    QString linename = newfilename.left(newfilename.length() - 14);
    QString datetime11 = newfilename.right(13).left(8);
    qDebug() << "linename=" << linename << ",datatime=" << datetime11;
    QString newname = linename + "_" + datetime11;

    // 创建的新的工程目录 【参考】设计-每个工程目录结构.txt
    QString newprojectfilepath = SlaveSetting::getSettingInstance()->getParentPath() + "/" + newname;

    // 如果没有该目录，创建新目录
    bool ret = SlaveSetting::getSettingInstance()->createSlaveProjectDir(newprojectfilepath);

    // 设置工作工程文件名
    ret = LzProjectAccess::getLzProjectAccessInstance()->setProjectName(projectclass, newprojectfilepath + "/" + newfilename);
    
    QString newprojectdir = SlaveSetting::getSettingInstance()->getParentPath() + "/" + newfilename.left(newfilename.length() - 5);

    if (ret)
        emit signalMsgToGUI(QString("[从控] 配置本次线路工程成功%1").arg(newprojectdir));
    else
    {
        client->sendMsgToMaster("170");
        emit signalMsgToGUI(QString("[从控] 配置本次线路工程失败%1").arg(newprojectdir));
    }

    // 设置接收文件工程存放目录，备份不需要
    // TODO 备份与计算同时进行时备份的接收文件，与计算的接收文件路径不同，怎么办？？
    if (projectclass == LzProjectClass::Main || projectclass == LzProjectClass::Calculate || projectclass == LzProjectClass::Collect || projectclass == LzProjectClass::Backup)
        client->setCurrentSavingPath(newprojectdir);

    LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Network) = false;
    LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Calibration) = false;
    LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_FenZhong) = true;  // @author 范翔 每组相机倒到分中坐标系的配置文件与双目标定配置文件二合一
    LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Plan) = false;
    LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Real) = false;
    LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Checked) = false;

    return ret;
}

/**
 * 每次启动都要向主控申请task文件（如果是上次备份又开始则不需要申请）
 * @author 冯明晨
 * @author 熊雪改
 */
bool SlaveProgram::init_TaskFile(QString filename)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        qDebug() << QString("[从控] 正在采集,不能重新加载计划任务配置文件");
        emit signalMsgToGUI(QString("[从控] 正在采集,不能重新加载计划任务配置文件"));
        return false;
    }*/
    bool & plantaskok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(LzProjectClass::Collect, ConfigFileType::Config_Plan);

    plantaskok = false;
    QFile file3(filename);//file3代表任务文件
    if(file3.exists())
    {
        // @fanxiang 改
        // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
        QByteArray tempstr = filename.toLocal8Bit();
        XMLTaskFileLoader * taskFileLoader = new XMLTaskFileLoader(tempstr);
        bool ret = taskFileLoader->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Collect));
        delete taskFileLoader;
        if (ret)
        {
            qDebug() << QObject::tr("[从控] 任务文件解析完毕！");
            emit signalMsgToGUI(QString("[从控] 任务文件解析完毕！"));
            client->sendMsgToMaster("142");
            plantaskok = true;
            file3.close();
            return true;
        }
        qDebug() << "load task file:" << ret;
    }

    file3.close();
    qDebug() << QObject::tr("[从控] 计划任务文件不存在或解析失败，向主控申请任务文件。。。");
    emit signalMsgToGUI(QString("[从控] 计划任务文件不存在或解析失败，向主控申请任务文件。。。"));
    client->sendMsgToMaster("140");
    return false;
}

/**
 * 加载网络从控及相机SNID硬件，双目视觉标定文件名称配置文件
 */
bool SlaveProgram::init_NetworkCameraHardwareFile(LzProjectClass projectclass, QString filename)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        qDebug() << QString("[从控] 正在采集,不能重新加载网络硬件配置文件");
        emit signalMsgToGUI(QString("[从控] 正在采集,不能重新加载网络硬件配置文件"));
        return false;
    }*/
    bool & networkok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Network);
    networkok = false;
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
            SlaveSetting::getSettingInstance()->setMasterIP(masterip);

            ret = NetworkConfigList::getNetworkConfigListInstance()->findSlave(SlaveSetting::getSettingInstance()->getMySlaveIP(),
                                                                      SlaveSetting::getSettingInstance()->getCurrentSlaveModel());
            SlaveSetting::getSettingInstance()->setHasModel(ret);

            SlaveModel & slaveModel = SlaveSetting::getSettingInstance()->getCurrentSlaveModel();
            // nasip
            SlaveSetting::getSettingInstance()->setNASAccessIP(slaveModel.getBackupNasIP().c_str());

            lzAcquizQueue->initSlaveModel(slaveModel);
            lzBackupQueue->initSlaveModel(slaveModel);
            lzCalcQueue->initSlaveModel(slaveModel);

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
                emit signalMsgToGUI(QString("[从控] 网络硬件配置文件不存在或解析失败%1,可能文件中不存在本机IP地址%2").arg(filename).arg(SlaveSetting::getSettingInstance()->getMySlaveIP()));
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
 * 接收校正文件
 */
bool SlaveProgram::loadRealTaskFile(LzProjectClass projectclass, QString filename)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        //qDebug() << QString("[从控] 正在采集,不能重新加载实际采集记录配置文件");
        emit signalMsgToGUI(QString("[从控] 正在采集,不能重新加载实际采集记录配置文件"));
        return false;
    }*/

    bool & realtaskok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Real);
    realtaskok = false;
    // @author fanxiang注文件解析
    QFile file0(filename);//file0代表index和camid的文件
    if(file0.exists())
    {
        file0.close();
        XMLRealTaskFileLoader * ff3 = new XMLRealTaskFileLoader(filename);
        bool ret = ff3->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(projectclass));
        delete ff3;
        if (ret)
        {
            //qDebug() << QObject::tr("[从控] 校正任务checked_task文件解析成功！");//在控制台上显示
            emit signalMsgToGUI(QString("[从控] 校正任务checked_task文件解析成功%1！").arg(filename));
            client->sendMsgToMaster("162");
            realtaskok = true;
            return true;
        }
    }
    file0.close();
    //qDebug() << QObject::tr("[从控] 实际采集记录real_task文件不存在或解析失败,向主控发送请求");
    emit signalMsgToGUI(QString("[从控] 实际采集记录real_task文件不存在或解析失败%1！").arg(filename));
    client->sendMsgToMaster("160");
    return false;
}

/**
 * 接收校正文件
 */
bool SlaveProgram::loadCorrectedTaskFile(LzProjectClass projectclass, QString filename)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        qDebug() << QString("[从控] 正在采集,不能重新加载采集任务校正配置文件");
        emit signalMsgToGUI(QString("[从控] 正在采集,不能重新加载采集任务校正配置文件"));
        return false;
    }*/
    bool & checkedtaskok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Checked);
    checkedtaskok = false;
    // @author fanxiang注文件解析
    QFile file0(filename);//file0代表index和camid的文件
    if(file0.exists())
    {
        file0.close();
        XMLCheckedTaskFileLoader * ff3 = new XMLCheckedTaskFileLoader(filename);
        bool ret = ff3->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(projectclass));
        delete ff3;
        if (ret)
        {
            //qDebug() << QObject::tr("校正任务checked_task文件解析成功！");//在控制台上显示
            emit signalMsgToGUI(QString("[从控] 校正任务checked_task文件解析成功%1！").arg(filename));
            client->sendMsgToMaster("152");
            checkedtaskok = true;
            return true;
        }
    }
    file0.close();
    //qDebug() << QObject::tr("校正任务checked_task文件不存在或解析失败,向主控发送请求");
    emit signalMsgToGUI(QString("[从控] 校正任务checked_task文件不存在或解析失败%1！").arg(filename));
    client->sendMsgToMaster("150");
    return false;
}

/**
 * 加载分中硬件配置文件
 */
bool SlaveProgram::init_FenZhongFile(QString projectpath, SlaveModel & sm)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        qDebug() << QString("[从控] 正在采集,不能重新加载轨平面分中坐标系标定参数文件");
        emit signalMsgToGUI(QString("[从控] 正在采集,不能重新加载轨平面分中坐标系标定参数文件"));
        return false;
    }*/
    bool & fenzhongok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(LzProjectClass::Calculate, ConfigFileType::Config_FenZhong);
    fenzhongok = true;
    QFile file1(projectpath + "/calcu_calibration/" + sm.box1.box_fenzhong_calib_file.c_str());//file1代表分中参数的文件
    if (!file1.exists())
    {
        fenzhongok = false;
        file1.close();
    }
    QFile file2(projectpath + "/calcu_calibration/" + sm.box2.box_fenzhong_calib_file.c_str());
    if (!file2.exists())
    {
        fenzhongok = false;
        file2.close();
    }
    file1.close();
    file2.close();

    if (fenzhongok)
    {
        qDebug() << QObject::tr("轨平面分中坐标系标定参数文件存在！%1").arg(projectpath + "/calcu_calibration/" + sm.box1.box_fenzhong_calib_file.c_str() + ", " + projectpath + "/calcu_calibration/" + sm.box2.box_fenzhong_calib_file.c_str());
        emit signalMsgToGUI(QString("[从控] 轨平面分中坐标系标定参数文件存在！%1").arg(projectpath + "/calcu_calibration/" + sm.box1.box_fenzhong_calib_file.c_str() + ", " + projectpath + "/calcu_calibration/" + sm.box2.box_fenzhong_calib_file.c_str()));
        fenzhongok = true;
        client->sendMsgToMaster("122");//分中参数文件没有向主控发请求
        return true;
    }
    else
    {
        qDebug() << QObject::tr("轨平面分中坐标系标定参数文件不存在！%1").arg(projectpath + "/calcu_calibration/" + sm.box1.box_fenzhong_calib_file.c_str() + ", " + projectpath + "/calcu_calibration/" + sm.box2.box_fenzhong_calib_file.c_str());
        emit signalMsgToGUI(QString("[从控] 轨平面分中坐标系标定参数文件不存在！%1").arg(projectpath + "/calcu_calibration/" + sm.box1.box_fenzhong_calib_file.c_str() + ", " + projectpath + "/calcu_calibration/" + sm.box2.box_fenzhong_calib_file.c_str()));
        client->sendMsgToMaster("120");//分中参数文件没有向主控发请求
        return false;
    }
    return false;
}

/**
 * 加载双目视觉标定配置文件
 */
bool SlaveProgram::init_CameraCalibrationFile(QString projectpath, SlaveModel & sm)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        qDebug() << QString("[从控] 正在采集,不能重新加载双目相机标定配置文件");
        emit signalMsgToGUI(QString("[从控] 正在采集,不能重新加载双目相机标定配置文件"));
        return false;
    }*/
    bool & calibrationok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(LzProjectClass::Calculate, ConfigFileType::Config_Calibration);
    calibrationok = true;
    QFile file1(projectpath + "/calcu_calibration/" + sm.box1.box_camera_calib_file.c_str());//file1代表分中参数的文件
    if (!file1.exists())
    {
        calibrationok = false;
        file1.close();
    }
    QFile file2(projectpath + "/calcu_calibration/" + sm.box2.box_camera_calib_file.c_str());
    if (!file2.exists())
    {
        calibrationok = false;
        file2.close();
    }
    file1.close();
    file2.close();

    if (calibrationok)
    {
        qDebug() << QObject::tr("相机标定配置文件存在！%1").arg(projectpath + "/calcu_calibration/" + sm.box1.box_camera_calib_file.c_str() + ", " + projectpath + "/calcu_calibration/" + sm.box2.box_camera_calib_file.c_str());
        emit signalMsgToGUI(QString("[从控] 相机标定配置文件存在！%1").arg(projectpath + "/calcu_calibration/" + sm.box1.box_camera_calib_file.c_str() + ", " + projectpath + "/calcu_calibration/" + sm.box2.box_camera_calib_file.c_str()));
        calibrationok = true;
        client->sendMsgToMaster("132");
        return true;
    }
    else
    {
        qDebug() << QObject::tr("相机标定配置文件不存在！%1").arg(projectpath + "/calcu_calibration/" + sm.box1.box_camera_calib_file.c_str() + ", " + projectpath + "/calcu_calibration/" + sm.box2.box_camera_calib_file.c_str());
        emit signalMsgToGUI(QString("[从控] 相机标定配置文件不存在！%1").arg(projectpath + "/calcu_calibration/" + sm.box1.box_camera_calib_file.c_str() + ", " + projectpath + "/calcu_calibration/" + sm.box2.box_camera_calib_file.c_str()));
        client->sendMsgToMaster("130");//分中参数文件没有向主控发请求
        return false;
    }
    return false;
}

/**
 * 初始化完成
 * @param WorkingStatus function 相当于一个枚举，1表示是否可以开始采集，(2计算, 3备份--作废) 4表示是否可以开始计算备份
 * @param QString filename 要解析的任务文件，当采集时需要计划文件"xxxx_20140211.plan"
 *                                        当计算备份时需要校正文件"xxxx_20140211.check"
 * @param QString linename, QString date 暂时没用，与filename冲突
 */
void SlaveProgram::checkedok(WorkingStatus function, QString filename, QString linename, QString date)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        qDebug() << QString("[从控] 正在采集,不能重新配置采集任务");
        emit signalMsgToGUI(QString("[从控] 正在采集,不能重新配置采集任务"));
        return;
    }*/
    
    LzProjectClass projectclass;
    switch (function)
    {
        case Collecting : projectclass = LzProjectClass::Collect; break;
        case Calculating : projectclass = LzProjectClass::Calculate; break;
        case Backuping: projectclass = LzProjectClass::Backup; break;
        case Calculating_Backuping : projectclass = LzProjectClass::Calculate; break;
        default: projectclass = LzProjectClass::Main; break;
    }

    bool ret = false;
    QString projectfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(projectclass);
    QString projectdirname = SlaveSetting::getSettingInstance()->getParentPath() + "/" + projectfilename.left(projectfilename.length() - 5);
    filename = projectdirname + "/" + filename;
    //qDebug() << filename;

    bool & networkok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Network);
    bool & plantaskok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Plan);
    bool & realtaskok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Real);
    bool & checkedtaskok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Checked);

    if (!networkok)
        networkok = init_NetworkCameraHardwareFile(projectclass, projectdirname + "/network_config.xml");

    if (!networkok)
        return;

    ProjectModel currentProjectModel = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(projectclass);

    if (function == WorkingStatus::Collecting) // 采集
    {
        plantaskok = false;
        if (!plantaskok)
        {
            // 检查计划任务文件
            QString taskfilename = projectdirname + "/" + currentProjectModel.getPlanFilename();
            //qDebug() << "taskfilename" << taskfilename;
            ret = init_TaskFile(taskfilename);
            if (ret == false)
                return;
        }

        emit initGroupBox(Collecting);
        client->sendMsgToMaster("200");
    }
    else if (function == WorkingStatus::Correcting) // 校正
    {
        realtaskok = false;
        if (!checkedtaskok)
        {
            QString realfilename = projectdirname + "/" + currentProjectModel.getRealFilename();
            //qDebug() << "realfilename" << realfilename;
            ret = loadRealTaskFile(LzProjectClass::Main, realfilename);
            if (ret == false)
                return;
        }
        client->sendMsgToMaster("204");
    }
    else if (function == WorkingStatus::Calculating) // 计算
    {
        checkedtaskok = false;
        if (!checkedtaskok)
        {
            // 检查实际采集和校正任务文件
            QString realfilename = projectdirname + "/" + currentProjectModel.getRealFilename();
            ret = loadRealTaskFile(LzProjectClass::Calculate, realfilename);
            if (ret == false)
                return;

            QString chekedfilename = projectdirname + "/" + currentProjectModel.getCheckedFilename();
            ret = loadCorrectedTaskFile(LzProjectClass::Calculate, chekedfilename);
            if (ret == false)
                return;

            SlaveModel & slaveModel = SlaveSetting::getSettingInstance()->getCurrentSlaveModel();
            ret = init_CameraCalibrationFile(projectdirname, slaveModel);
            if (ret == false)
                return;
            
            // @author 范翔 每组相机倒到分中坐标系的配置文件与双目标定配置文件二合一
            //ret = init_FenZhongFile(projectdirname, slaveModel);
            //if (ret == false)
            //    return;

        }

        emit initGroupBox(Calculating);
        client->sendMsgToMaster("202");
    }
    else if (function == WorkingStatus::Backuping) // 备份
    {
        checkedtaskok = false;
        if (!checkedtaskok)
        {
            // 检查实际采集和校正任务文件
            QString realfilename = projectdirname + "/" + currentProjectModel.getRealFilename();
            ret = loadRealTaskFile(LzProjectClass::Backup, realfilename);
            if (ret == false)
                return;

            QString chekedfilename = projectdirname + "/" + currentProjectModel.getCheckedFilename();
            ret = loadCorrectedTaskFile(LzProjectClass::Backup, chekedfilename);
            if (ret == false)
                return;

        }

        emit initGroupBox(Backuping);
        client->sendMsgToMaster("210");
    }
    else if (function == Deleting)
    {
        // 删除
        ret = LocalFileOperation::removeDirwithContent(projectdirname);
        if (ret == false)
            return;
        client->sendMsgToMaster("208");
    }
}

/**
 * 发送普通文件
 * @author 熊雪
 */
void SlaveProgram::send_file(QString filename)
{
    QString ip = SlaveSetting::getSettingInstance()->getMasterIP();
    qDebug() << ip;
    client->sendFileToServer(ip, filename);
}
