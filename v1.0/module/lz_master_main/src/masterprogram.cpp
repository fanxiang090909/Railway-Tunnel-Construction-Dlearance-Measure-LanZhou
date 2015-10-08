#include "masterprogram.h"
#include "fileoperation.h"

/**
 * 主控服务程序驱动类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-9-8
 */
// singleton设计模式，静态私有实例变量
MasterProgram * MasterProgram::masterProgramInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
MasterProgram* MasterProgram::getMasterProgramInstance()
{
    if (masterProgramInstance == NULL)
    {
        masterProgramInstance = new MasterProgram();
    }
    return masterProgramInstance;
}

// singleton单一设计模式，构造函数私有
MasterProgram::MasterProgram(QObject *parent) :
    QObject(parent)
{
    qDebug() << "main thread:" << QThread::currentThreadId();
    
    multiThreadTcpServer = NULL;
    
    hasChangeBackupCalcuStatus = false;
    
    // 默认手动采集模式
    collectMode = Lz_Collecting_Manual_DistanceMode;
    // 默认曝光时间
    exposureTime = 500;
    // 默认触发采集模式
    triggerMode = Lz_Camera_HardwareTrigger;

    lzReadWriteHardwareInterface = NULL;
    distanceMode = LzCollectHardwareTriggerDistanceMode::Lz_HardwareTrigger_500mm;

    lzAcquizCtrlQueue = new LzAcquizCtrlQueue(this);
    lzFuseCalcQueue = new LzFuseCalcQueue(this);
    lzBackupCalcQueue = new LzBackupCalcQueue(this);

    // 状态提示，消息转发
    connect(lzAcquizCtrlQueue, SIGNAL(signalParsedMsgToSlave(QString)), this, SLOT(slotParsedMsg(QString)));
    connect(lzAcquizCtrlQueue, SIGNAL(signalToLights(HardwareType, QString, HardwareStatus)), this, SLOT(slotToLights(HardwareType, QString, HardwareStatus)));
    connect(lzAcquizCtrlQueue, SIGNAL(signalToCameraFC(QString, qint64)), this, SLOT(slotToCameraFC(QString, qint64))); 
    connect(lzAcquizCtrlQueue, SIGNAL(signalToCameraTask(WorkingStatus, QString, QString)), this, SLOT(slotToCameraTask(WorkingStatus, QString, QString)));

    // 消息文件传输，消息转发
    connect(lzAcquizCtrlQueue, SIGNAL(signalMsgToSlave(QString, QString)), this, SLOT(slotMsgToSlave(QString, QString)));
    connect(lzAcquizCtrlQueue, SIGNAL(signalMsgToSlaves(QString)), this, SLOT(slotMsgToSlaves(QString)));
    connect(lzAcquizCtrlQueue, SIGNAL(signalFileToSlave(QString, QString)), this, SLOT(slotMsgToFile(QString, QString)));

    // 融合计算状态提示，消息转发
    connect(lzFuseCalcQueue, SIGNAL(signalParsedMsgToSlave(QString)), this, SLOT(slotParsedMsg(QString)));
    connect(lzFuseCalcQueue, SIGNAL(signalCalcuBakcupTask(WorkingStatus, QString, int, QString, QString)), this, SLOT(slotToCalcuBackupTask(WorkingStatus, QString, int, QString, QString)));
    connect(lzFuseCalcQueue, SIGNAL(calcubackupProgressingBar(WorkingStatus, QString, int, int, int, bool)), this, SLOT(slotCalcubackupProgressingBar(WorkingStatus, QString, int, int, int, bool)));
    // 融合计算单一任务消息转发-当前计算帧号
    connect(lzFuseCalcQueue, SIGNAL(signalCalcuBackupTaskFC(WorkingStatus, QString, int, QString, long long)), this, SLOT(slotToCalcuBackupTaskFC(WorkingStatus, QString, int, QString, long long)));
    // 融合计算单一任务消息转发-任务初始化（起始帧、终止帧）
    connect(lzFuseCalcQueue, SIGNAL(signalCalcuBackupTaskFC_init(WorkingStatus, QString, int, QString, long long, long long)), this, SLOT(slotToCalcuBackupTaskFC_init(WorkingStatus, QString, int, QString, long long, long long)));

    // 主控文件备份状态提示，消息转发
    connect(lzBackupCalcQueue, SIGNAL(signalParsedMsgToSlave(QString)), this, SLOT(slotParsedMsg(QString)));
    connect(lzBackupCalcQueue, SIGNAL(signalCalcuBakcupTask(WorkingStatus, QString, int, QString, QString)), this, SLOT(slotToCalcuBackupTask(WorkingStatus, QString, int, QString, QString)));
    connect(lzBackupCalcQueue, SIGNAL(calcubackupProgressingBar(WorkingStatus, QString, int, int, int, bool)), this, SLOT(slotCalcubackupProgressingBar(WorkingStatus, QString, int, int, int, bool)));

}

MasterProgram::~MasterProgram()
{
    if (lzReadWriteHardwareInterface != NULL)
        delete lzReadWriteHardwareInterface;

    delete lzAcquizCtrlQueue;
    delete lzFuseCalcQueue;
    delete lzBackupCalcQueue;

    if (multiThreadTcpServer != NULL)
    {
        delete multiThreadTcpServer;
    }
    // 将当前状态存入文件
    //saveCurrentStatus()
    
    if (hasChangeBackupCalcuStatus)
    {
        saveCalcuBackupInfo(LzProjectClass::Calculate);
        saveCalcuBackupInfo(LzProjectClass::Backup);
    }
}

LzCameraCollectingMode MasterProgram::getTriggerMode() { return triggerMode; }

// LzAcquizQueue转发消息
void MasterProgram::slotParsedMsg(QString msg) { qDebug() << msg; emit signalMsgToGUI(WorkingStatus::Preparing, msg); }
void MasterProgram::slotToLights(HardwareType type, QString id, HardwareStatus status) { emit signalToLights(type, id, status); }
void MasterProgram::slotToCameraFC(QString id, qint64 fc) { emit signalToCameraFC(id, fc); }
void MasterProgram::slotToCameraTask(WorkingStatus workingstatus, QString id, QString task) 
{ 
    // 采集时不看threadid，可赋值为0
    emit signalToCameraTask(workingstatus, id, 0, task, ""); 
}

void MasterProgram::slotMsgToSlave(QString ip, QString msg) { getMultiThreadTcpServer()->sendMessageToOneSlave(ip, msg); }
void MasterProgram::slotMsgToSlaves(QString msg) { getMultiThreadTcpServer()->sendMessageToSlaves(msg); }
void MasterProgram::slotMsgToFile(QString ip, QString filename_path) { getMultiThreadTcpServer()->sendFileToOneSlave(ip, filename_path); }
// LzFuseCalc，LzMasterBackup转发消息
void MasterProgram::slotCalcubackupProgressingBar(WorkingStatus workingstatus, QString id, int threadid, int totalnum, int currentnum, bool isupdate) { emit calcubackupProgressingBar(workingstatus, id, threadid, totalnum, currentnum, isupdate); }
void MasterProgram::slotToCalcuBackupTask(WorkingStatus workingstatus, QString id, int threadid, QString task, QString status) 
{
    emit signalToCameraTask(workingstatus, id, threadid, task, status); 
    // 如果备份计算成功，记录状态
    if (status.compare("ok") == 0)
    {
        if (workingstatus == Calculating)
        {
            int tunnelid = -1;
            QString cameragroupindex = "";
        
            calculate_FeedbackPerTunnel_CameraGroup_R(threadid, task, 0, tunnelid, "fuse", false, 0);

            if (hasChangeBackupCalcuStatus)
                saveCalcuBackupInfo(LzProjectClass::Backup);
        }
        else if (workingstatus == Backuping) // 备份双目结果和融合结果
        {
            int seqno = -1;
            QString index = "";
            backup_FeedbackPerTunnel_CameraGroup_R(task, 0, 2, seqno, index, false, 0);
        }
    }
    else if (status.compare("pause") == 0)
    {
        if (workingstatus == Calculating)
        {
            if (hasChangeBackupCalcuStatus)
                saveCalcuBackupInfo(LzProjectClass::Backup);
        }
        else if (workingstatus == Backuping) // 备份双目结果和融合结果
        {
            ;//TODO
        }
    }
}

// [计算-融合计算转发消息]
void MasterProgram::slotToCalcuBackupTaskFC_init(WorkingStatus status, QString index, int threadid, QString taskname, long long startfc, long long endfc) 
{
    emit signalToSlaveCameraTaskFC_init(status, index, threadid, taskname, startfc, endfc);
}
void MasterProgram::slotToCalcuBackupTaskFC(WorkingStatus status, QString index, int threadid, QString taskname, long long currentfc) 
{
    emit signalToCameraTaskFC(status, index, threadid, taskname, currentfc);
    calculate_FeedbackPerTunnel_CameraGroup_R(threadid, taskname, 0, -1, "fuse", true, currentfc);
}

/**
 * 设置采集模式和曝光时间
 */
void MasterProgram::setTriggerModeAndExposureTime(LzCameraCollectingMode newtriggerMode, int newexposureTime, LzCollectingMode newcollectmode, int distanceOrNoDistanceMode)
{
    triggerMode = newtriggerMode;
    exposureTime = newexposureTime;
    collectMode = newcollectmode;
    setDistanceMode(newcollectmode, distanceOrNoDistanceMode);
}

/**
 * 设置脉冲触发间隔模式，隔1m1帧，0.5m一帧，0.75m一帧
 */
bool MasterProgram::setDistanceMode(LzCollectingMode newcollectmode, int distanceOrNoDistanceMode)
{
    if (lzReadWriteHardwareInterface != NULL)
    {
        QString collectmodestr;
        switch (collectMode)
        {
        case Lz_Collecting_Manual_DistanceMode: collectmodestr = QObject::tr("手动进出洞模式"); break;
        case Lz_Collecting_Automatic_DistanceMode: collectmodestr = QObject::tr("自动进出洞检测模式"); break;
        case Lz_Collecting_FreeMode_NoDistanceMode: collectmodestr = QObject::tr("自由触发模式"); break;
        }
        emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  设置采集模式为%1").arg(collectmodestr));

        bool ret;
        switch (collectMode)
        {
        case Lz_Collecting_Manual_DistanceMode:
        case Lz_Collecting_Automatic_DistanceMode:
            {
            distanceMode = (LzCollectHardwareTriggerDistanceMode) distanceOrNoDistanceMode;
            int distance = 1000;
            switch (distanceMode)
            {
                case LzCollectHardwareTriggerDistanceMode::Lz_HardwareTrigger_500mm : distance = 500; break;
                case LzCollectHardwareTriggerDistanceMode::Lz_HardwareTrigger_750mm : distance = 750; break;
                case LzCollectHardwareTriggerDistanceMode::Lz_HardwareTrigger_1000mm : distance = 1000; break;
                case LzCollectHardwareTriggerDistanceMode::Lz_HardwareTrigger_1250mm : distance = 1250; break;
                default: distance = 1000;
            }
            ret = lzReadWriteHardwareInterface->writeCollectModeAndDistanceOrNoConfig(collectMode, distanceOrNoDistanceMode);
            if (ret)
                emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  设置采集模式和采集帧间隔为%1米采集1帧到接口卡成功").arg((float)distance/1000));
            else
                emit signalErrorToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  设置采集模式和采集帧间隔为%1米采集1帧失败，检查接口卡").arg((float)distance/1000));
            break;
            }
        case Lz_Collecting_FreeMode_NoDistanceMode:
            {
            noDistanceMode = (LzCollectHardwareTriggerNoDistanceMode) distanceOrNoDistanceMode;
            int hz;
            switch (noDistanceMode)
            {
                case LzCollectHardwareTriggerNoDistanceMode::Lz_Collecting_FreeMode_30Hz : hz = 30; break;
                case LzCollectHardwareTriggerNoDistanceMode::Lz_Collecting_FreeMode_40Hz : hz = 40; break;
                case LzCollectHardwareTriggerNoDistanceMode::Lz_Collecting_FreeMode_50Hz : hz = 50; break;
                case LzCollectHardwareTriggerNoDistanceMode::Lz_Collecting_FreeMode_60Hz : hz = 60; break;
                default: hz = 30;
            }
            ret = lzReadWriteHardwareInterface->writeCollectModeAndDistanceOrNoConfig(collectMode, distanceOrNoDistanceMode);
            if (ret)
                emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  设置采集模式和采集帧率为每秒%1帧到接口卡成功").arg(hz));
            else
                emit signalErrorToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  设置采集模式和采集帧率为每秒%1帧失败，检查接口卡").arg(hz));
            return ret;

            break;
            }
        default:
            break;
        }
    }
    return false;
}

void MasterProgram::init()
{
    // 加载网络及从机上相机SN号配置文件
    QString networkconfig = MasterSetting::getSettingInstance()->getParentPath() + "/system/network_config.xml";
    MasterSetting::getSettingInstance()->setNetworkConfigFilename(networkconfig);
    XMLNetworkFileLoader *net = new XMLNetworkFileLoader(networkconfig);
    //net->saveFile();//生成Networkconfig.xml文件
    bool ret = net->loadFile();//解析Networkconfig.xml文件并存入list
    delete net;
    if (ret == false)
    {
        qDebug() << QObject::tr("无法加载网络及各个从机相机SerialNumber配置文件%1").arg(networkconfig);
        emit signalErrorToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  无法加载网络及各个从机相机SerialNumber配置文件%1，不能开启服务器！").arg(networkconfig));
        return;
    }

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
    emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  程序初始化"));

    if (multiThreadTcpServer != NULL)
    {
        delete multiThreadTcpServer;
        multiThreadTcpServer = NULL;
    }
    // 消息监听端口号为9424，7777为文件接收端口号
    multiThreadTcpServer = new MasterMultiThreadTcpServer(this, 9424, 7777);

    connect(multiThreadTcpServer, SIGNAL(signalMsgToMaster(QString)), this, SLOT(parseMsg(QString)));
    connect(multiThreadTcpServer, SIGNAL(signalErrorToMaster(QString)), this, SLOT(parseMsg(QString)));

    // 初始化完成
    qDebug() << "End init...";

    Status::getStatusInstance()->initWorkingStatus();

    emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  程序初始化结束"));

    // 等待从控初始化完成
    // 可以开始采集

    // 硬件接口卡，控制进出洞
    // @date 20150526
    // @author 范翔
    if (lzReadWriteHardwareInterface != NULL)
        delete lzReadWriteHardwareInterface;
    lzReadWriteHardwareInterface = new ReadWriteHardwareInterface();
    // 设置默认脉冲间隔 按里程采集、按距离1m1帧采集
    setDistanceMode(collectMode, (int) distanceMode);
    
    connect(lzReadWriteHardwareInterface, SIGNAL(OutTunnel()), this, SLOT(collect_OutFromTunnel()));
    connect(lzReadWriteHardwareInterface, SIGNAL(InTunnel(bool)), this, SLOT(collect_IntoTunnel(bool)));
    connect(lzReadWriteHardwareInterface, SIGNAL(ReadWriteHardwareException(QString)), this, SLOT(slotReadWriteHardwareException(QString)));
    connect(lzReadWriteHardwareInterface, SIGNAL(sendMsg(QString)), this, SLOT(slotParsedMsg(QString)));

    // 线程开始
    lzReadWriteHardwareInterface->startReadThread();
}

void MasterProgram::slotReadWriteHardwareException(QString excep)
{
    emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  采集里程计数相关硬件异常%1").arg(excep));
}

/**
 * 重启服务器
 */
void MasterProgram::restartServer()
{
    bool ret = closeServer();

    if (ret)
    {
        emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  正在重启服务器..."));

        // 重新初始化
        init();
    }
    else
        emit signalErrorToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  正在工作，不能重启服务器，请先停止当前任务！"));
}

/**
 * 重启服务器
 */
bool MasterProgram::closeServer()
{
    if (Status::getStatusInstance()->getStatusSlaveNum(Collecting) > 0 || Status::getStatusInstance()->getStatusSlaveNum(Calculating) > 0
                    || Status::getStatusInstance()->getStatusSlaveNum(Backuping) > 0)
    {
        qDebug() << "cannot close server";
        emit signalErrorToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  正在工作，不能关闭服务器，请先停止当前任务！"));
        return false;
    }

    if (multiThreadTcpServer != NULL)
    {
        delete multiThreadTcpServer;
        multiThreadTcpServer = NULL;
    }

    // 将当前状态存入文件
    //saveCurrentStatus();

    // 更改网络连接提示，所有指示灯关闭
    emit turnOffAllHardware();
    return true;
}

/**
 * 保存采集时记录信息至文件 collect_EndCurrentLine时调用
 * @see 函数collect_EndCurrentLine
 */
bool MasterProgram::saveCollectedInfo()
{
    return Status::getStatusInstance()->saveCollectedInfoToRealFile();
}

/**
 * 保存计算备份状态
 */
bool MasterProgram::saveCalcuBackupInfo(LzProjectClass projectclass)
{
    QString statusstr = "";
    WorkingStatus statusss = Preparing;
    if (projectclass == LzProjectClass::Calculate)
    {
        statusss = Calculating;
        statusstr = tr("计算");
    }
    else if (projectclass == LzProjectClass::Backup)
    {
        statusss = Backuping;
        statusstr = tr("备份");
    }

    if (hasChangeBackupCalcuStatus == false)
    {
        emit signalMsgToGUI(statusss, QObject::tr("[主控]  无需保存%1状态，状态未更改").arg(statusstr));
        return false;
    }
    // 为了解析planfile和checkedfile
    ProjectModel & projm = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(projectclass);

    // 解析realtaskfile
    QString realfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(projectclass) + "/" + projm.getRealFilename();
    XMLRealTaskFileLoader * taskfileloader1 = new XMLRealTaskFileLoader(realfilename);
    bool ret2 = taskfileloader1->saveFile(LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(projectclass));
    delete taskfileloader1;
    if (!ret2)
    {
        emit signalMsgToGUI(statusss, QObject::tr("[主控]  保存%1状态出错，无法保存实际采集任务文件%2").arg(statusstr).arg(realfilename));
        return false;
    }

    // 解析checkedtaskfile
    QString checkedfilename =  LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(projectclass) + "/" + projm.getCheckedFilename();
    XMLCheckedTaskFileLoader * taskfileloader = new XMLCheckedTaskFileLoader(checkedfilename);
    // TODO 往不同的工程保存
    ret2 = taskfileloader->saveFile(LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(projectclass));
    delete taskfileloader;
    if (!ret2)
    {
        emit signalMsgToGUI(statusss, QObject::tr("[主控]  保存%1状态出错，无法保存校正任务文件%2").arg(statusstr).arg(checkedfilename));
        return false;
    }
    emit signalMsgToGUI(statusss, QObject::tr("[主控]  保存%1状态成功%2，%3").arg(statusstr).arg(realfilename).arg(checkedfilename));

    hasChangeBackupCalcuStatus = false;
    return true;
}

/**
 * 保存当前状态至文件
 * @see 函数closeServer时调用
 */
/*bool MasterProgram::saveCurrentStatus()
{
    //return Status::getStatusInstance()->saveUncompletedToFiles();
}*/

/**
 * 返回多线程服务器类指针
 */
MasterMultiThreadTcpServer * MasterProgram::getMultiThreadTcpServer()
{
    return multiThreadTcpServer;
}

/**
 * !!现在这个函数多余了2013-09-24fan，
 * ||如果需要应该在该类的init函数中去掉注释的connect两句话
 * 检测到server的msg，向上层抛出，这里不做处理
 */
void MasterProgram::displayMsg(QString msg)
{
    // 消息向上抛出，不做处理
    //qDebug() << "in master: " << msg;
    //emit (msg);
}

/**
 * 解析消息字符串
 * @param msg 自定义的消息，这里解析从从控机传到主控机的消息
 * @see 《主控从控程序流程及消息设计.ppt》版本v1.0
 */
void MasterProgram::parseMsg(QString msg)
{
    QStringList strList = msg.split(",", QString::SkipEmptyParts);
    if (strList.length() < 2)
    {
        qDebug() << tr("解析字符出错") << msg;
        return;
    }
    int slaveid = strList.value(0).toInt();
    int msgid = strList.value(1).toInt();

    //qDebug() << msgid;
    if (msgid <= 0) // 主控自己通知
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
        switch(msgid)
        {
            case 0: break;//emit signalErrorToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  没有连接从控，无法发送消息"));break;
            case -7:
            {
                emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[从控%1] 连接开启").arg(slaveid));
                emit signalToLights(Slave_Computer_Type, QString("%1").arg(slaveid), Hardware_OnButFree);
                Status::getStatusInstance()->slaveIntoStatus(Preparing, slaveid);                
                break;
            }
            case -8:
            {
                emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[从控%1] 连接断开").arg(slaveid));
                Status::getStatusInstance()->slaveOutfromStatus(Preparing, slaveid);                
                WorkingStatus tmpstatus = Status::getStatusInstance()->getWorkingStatus();
                if (tmpstatus == Collecting)
                    Status::getStatusInstance()->slaveOutfromStatus(Collecting, slaveid);
                else if (tmpstatus == Calculating_Backuping)
                {
                    Status::getStatusInstance()->slaveOutfromStatus(Calculating, slaveid);
                    Status::getStatusInstance()->slaveOutfromStatus(Backuping, slaveid);
                }
                emit signalToLights(Slave_Computer_Type, QString("%1").arg(slaveid), Hardware_Off);
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
                    emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  接收从控%1的文件%2成功，文件大小：%3KB，用时%4秒").arg(slaveid).arg(filename).arg(filesize).arg(timespend));
                else
                    emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  发送至从控%1的文件%2成功，文件大小：%3KB，用时%4秒").arg(slaveid).arg(filename).arg(filesize).arg(timespend));
                break;
            }
            case -10:
            {
                if (type == 0)
                    emit signalErrorToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  接收从控%1的文件%2时打开文件失败").arg(slaveid).arg(filename));
                else
                    emit signalErrorToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  发送至从控%1的文件%2时打开文件失败").arg(slaveid).arg(filename));
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
                    emit signalErrorToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  接收从控%1的文件%2出错%3").arg(slaveid).arg(filename).arg(error));
                else
                    emit signalErrorToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  发送至从控%1的文件%2出错%3").arg(slaveid).arg(filename).arg(error));
                break;
            }
            default:;
        }
    }
    else if (msgid < 999)   // 初始化
    {
        switch(msgid)
        {
            case 110:emit signalErrorToGUI(WorkingStatus::Preparing, QObject::tr("[从控%1] 缺少网络及相机硬件参数文件（或解析失败,可能文件中找不到对应IP从机）").arg(slaveid));break;
            case 112:emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[从控%1] 确认接收网络及相机硬件参数文件，并解析成功").arg(slaveid));break;
            case 120:emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[从控%1] 缺少分中参数文件（或解析失败）").arg(slaveid));break;
            case 122:emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[从控%1] 确认接收分中参数文件，并解析成功").arg(slaveid));break;
            case 130:emit signalErrorToGUI(WorkingStatus::Calculating, QObject::tr("[从控%1] 缺少双目视觉配置参数文件（或解析失败）").arg(slaveid));break;
            case 132:emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[从控%1] 确认接收双目视觉配置参数文件，并解析成功").arg(slaveid));break;
            case 140:emit signalErrorToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 缺少计划任务文件（或解析失败）").arg(slaveid));break;
            case 142:emit signalMsgToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 确认接收计划任务文件，并解析成功").arg(slaveid));break;
            case 150:emit signalErrorToGUI(WorkingStatus::Calculating_Backuping, QObject::tr("[从控%1] 缺少校对任务文件（或解析失败）").arg(slaveid));break;
            case 152:emit signalMsgToGUI(WorkingStatus::Calculating_Backuping, QObject::tr("[从控%1] 确认接收校对任务文件，并解析成功").arg(slaveid));break;
            case 160:emit signalErrorToGUI(WorkingStatus::Calculating_Backuping, QObject::tr("[从控%1] 缺少实际采集记录任务文件（或解析失败）").arg(slaveid));break;
            case 162:emit signalMsgToGUI(WorkingStatus::Calculating_Backuping, QObject::tr("[从控%1] 确认接收实际采集记录任务文件，并解析成功").arg(slaveid));break;
            case 170:
            {
                emit signalToLights(Slave_Computer_Type, QString("%1").arg(slaveid), Hardware_OnButFree);
                emit signalErrorToGUI(WorkingStatus::Preparing, QObject::tr("[从控%1] 该工程目录不存在").arg(slaveid));
                break;
            }
            case 200:
            {
                Status::getStatusInstance()->slaveIntoStatus(WorkingStatus::Collecting, slaveid);
                emit signalMsgToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 采集功能初始化完成").arg(slaveid));
                emit signalToLights(Slave_Computer_Type, QString("%1").arg(slaveid), Hardware_Working);

                // 如果当前所有相机全部返回
                int numofreceived = 0;
                Status::getStatusInstance()->master_status_mutex.lock();
                Status::getStatusInstance()->num_master_collect_status = 0;
                numofreceived = ++Status::getStatusInstance()->num_master_collect_status;
                Status::getStatusInstance()->master_status_mutex.unlock();
                //@author 范翔 定西 -> 怕采集之中一个计算机断开连接。。。 无法保存文件了就
                if (numofreceived >= getMultiThreadTcpServer()->getNumOfThreads())
                {
                    emit msgToGUIShowCProjectCTRL(Collecting, true);
                }

                break;
            }
            case 202:
            {   
                Status::getStatusInstance()->slaveIntoStatus(WorkingStatus::Calculating, slaveid);
                emit signalToLights(Slave_Computer_Type, QString("%1").arg(slaveid), Hardware_Working);
                emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[从控%1] 计算功能初始化完成").arg(slaveid));
                break;
            }
            case 210:
            {
                Status::getStatusInstance()->slaveIntoStatus(WorkingStatus::Backuping, slaveid);
                emit signalToLights(Slave_Computer_Type, QString("%1").arg(slaveid), Hardware_Working);
                emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[从控%1] 计算备份初始化完成").arg(slaveid));
                break;
            }
            case 204:
            {
                Status::getStatusInstance()->slaveIntoStatus(WorkingStatus::Correcting, slaveid);
                emit signalMsgToGUI(WorkingStatus::Correcting, QObject::tr("[从控%1] 校正功能初始化完成").arg(slaveid));
                break;
            }
            case 208:
            {   
                Status::getStatusInstance()->slaveIntoStatus(WorkingStatus::Deleting, slaveid);
                emit signalMsgToGUI(WorkingStatus::Deleting, QObject::tr("[从控%1] 删除工程完成").arg(slaveid));
                break;
            }
            default:;
        }
    }
    else if (msgid < 1999)  // 采集
    {
        switch(msgid)
        {
            case 1200:
            {
                // 1200,cameraindex=x,working=x,errortype=x
                if (strList.length() != 5 || !strList.value(2).startsWith("cameraindex")
                        || !strList.value(3).startsWith("working"))
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                QString cameraindex = strList.value(2).mid(12);
                int working = strList.value(3).mid(8).toInt();
                QString error = strList.value(4).mid(10);
                switch(working)
                {
                    case 0:emit signalToLights(Camera_Type, cameraindex, Hardware_Off);
                            emit signalErrorToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 相机%2正常关闭").arg(slaveid).arg(cameraindex));
                            break;
                    case 1:emit signalToLights(Camera_Type, cameraindex, Hardware_OnButFree);
                            emit signalErrorToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 相机%2就绪").arg(slaveid).arg(cameraindex));
                            break;
                    case 2:emit signalToLights(Camera_Type, cameraindex, Hardware_Working);
                            emit signalErrorToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 相机%2正在采集").arg(slaveid).arg(cameraindex));
                            break;
                    case 3:emit signalToLights(Camera_Type, cameraindex, Hardware_ExceptionOff);
                            emit signalErrorToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 相机%2异常关闭").arg(slaveid).arg(cameraindex));
                            break;
                    case 4:emit signalToLights(Camera_Type, cameraindex, Hardware_NotDetect);
                            emit signalErrorToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 相机%2没有被检测到，请查看硬件连接").arg(slaveid).arg(cameraindex));
                            break;
                    default:emit signalToLights(Camera_Type, cameraindex, Hardware_Broken);
                            emit signalErrorToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 相机%2损坏").arg(slaveid).arg(cameraindex));
                            break;
                }
                // 异常处理
                warning_collect_R();
                break;
            }
            case 1102: 
            {
                // 1102,cameraindex=x,return=true,tunnelid=x,seqno=x(顺序号), start_framecounter=xx,end_framecounter=xx,filename_prefix=xx
                QString cameraindex = strList.value(2).mid(12);
                QString returnVal = strList.value(3).mid(7);
                if (returnVal.compare("false") == 0)
                {
                    emit signalErrorToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 相机%2，确认出洞出错").arg(slaveid).arg(cameraindex));
                    break;
                }
                if (strList.length() < 10 && !strList.value(4).startsWith("tunnelid")
                        || !strList.value(5).startsWith("tunnelname")
                        || !strList.value(6).startsWith("seqno")
                        || !strList.value(7).startsWith("start_framecounter")
                        || !strList.value(8).startsWith("end_framecounter")
                        || !strList.value(9).startsWith("filename"))
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                int tunnelid = strList.value(4).mid(9).toInt();
                QString tunnelname = QString::fromLocal8Bit(strList.at(5).mid(11).toStdString().c_str());
                int seqno = strList.value(6).mid(6).toInt();
                _int64 start_fa = strList.value(7).mid(19).toLongLong();
                _int64 end_fa = strList.value(8).mid(17).toLongLong();
                QString filename = QString::fromLocal8Bit(strList.value(9).mid(9).toStdString().c_str());
                /*if (remark == 0)
                    filename.chop(4); // 去掉".raw"四个字符
                else if (remark > 0 && remark < 10)
                    filename.chop(6); // 去掉"_2.raw"六个字节
                else if (remark >= 10)
                    filename.chop(7); // 去掉"_10.raw"七个字节*/

                // 记录采集状态
                collect_SlaveCollected_R(slaveid, tunnelid, tunnelname, seqno, cameraindex, start_fa, end_fa);

                SlaveModel tmpModel(1.0);
                NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(slaveid, tmpModel);
                bool ret;
 
                // 对slavefcrec操作枷锁
                Status::getStatusInstance()->fcrecord_mutex.lock();
                SlaveCollectFrameCounterRecord & slavefcrec = Status::getStatusInstance()->getSlaveCollectFrameCounterRecord(slaveid, ret);
                if (!ret)
                {
                    // 对slavefcrec操作解锁
                    Status::getStatusInstance()->fcrecord_mutex.unlock();
                    return;
                }
                // 如果当前所有相机全部返回
                static int numofreceived = 0;
                Status::getStatusInstance()->master_status_mutex.lock();
                numofreceived = ++Status::getStatusInstance()->num_master_collect_status;
                Status::getStatusInstance()->master_status_mutex.unlock();
                if (numofreceived >= getMultiThreadTcpServer()->getNumOfThreads() * 4)
                {
                    lzAcquizCtrlQueue->endCurrentMsg();
                    // saveCollectInfo
                    this->saveCollectedInfo();
                }
                if (tmpModel.box1.camera_ref.compare(cameraindex.toStdString()) == 0) // 参考相机
                {
                    slavefcrec.startfc_box1cameraref = end_fa;
                }
                else if (tmpModel.box1.camera.compare(cameraindex.toStdString()) == 0)
                {
                    slavefcrec.startfc_box1camera = end_fa;
                }
                else if (tmpModel.box2.camera_ref.compare(cameraindex.toStdString()) == 0) // 参考相机
                {
                    slavefcrec.startfc_box2cameraref = end_fa;
                }
                else if (tmpModel.box2.camera.compare(cameraindex.toStdString()) == 0)
                {
                    slavefcrec.startfc_box2camera = end_fa;           
                }
                // 对slavefcrec操作解锁
                Status::getStatusInstance()->fcrecord_mutex.unlock();

                emit signalToCameraFC(cameraindex, end_fa);
                emit signalMsgToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 相机%2，确认出洞，当前实际采集隧道%3_%4，起始帧%5，起始%6").arg(slaveid).arg(cameraindex).arg(seqno).arg(tunnelname).arg(start_fa).arg(end_fa));
                break;
            }
            case 1104: //1104,cameraindex=x,isvalid=true（预进洞有效，else预进洞无效）,return=true,startframe=xx,endframe=xx,filename_prefix=xx(当isvalid=true预进洞有效时有该属性)
            {
                QString cameraindex = strList.value(2).mid(12);
                QString isvalidstr = strList.value(3).mid(8);
                QString returnVal = strList.value(4).mid(7);
                if (returnVal.compare("false") == 0)
                {
                    emit signalErrorToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 相机%2，确认预进洞有效？%3出错").arg(slaveid).arg(cameraindex).arg(isvalidstr));
                    break;
                }
                if (strList.length() < 7 && !strList.value(5).startsWith("startframe")
                        || !strList.value(6).startsWith("endframe"))
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                _int64 start_fa = strList.value(5).mid(11).toLongLong();
                _int64 end_fa = strList.value(6).mid(9).toLongLong();
                bool isvalid = true;
                if (isvalidstr.compare("false") == 0) // 预进洞无效
                {
                    isvalid = false;
                    emit signalMsgToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 相机%2，确认预进洞无效，起始帧%3，终止%4").arg(slaveid).arg(cameraindex).arg(start_fa).arg(end_fa));
                }
                else
                {
                    QString filenameprefix = strList.value(7).mid(16);
                    // threadid无效，赋值0
                    emit signalToCameraTask(Collecting, cameraindex, 0, filenameprefix, "");
                    emit signalMsgToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 相机%2，确认进洞到%3").arg(slaveid).arg(cameraindex).arg(filenameprefix));
                    //emit signalMsgToGUI(QObject::tr("[从控%1] 相机%2，确认预进洞有效，起始帧%3，终止%4").arg(slaveid).arg(cameraindex).arg(start_fa).arg(end_fa));
                }
                SlaveModel tmpModel(1.0);
                NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(slaveid, tmpModel);
                bool ret;
 
                // 对slavefcrec操作枷锁
                Status::getStatusInstance()->fcrecord_mutex.lock();
                SlaveCollectFrameCounterRecord & slavefcrec = Status::getStatusInstance()->getSlaveCollectFrameCounterRecord(slaveid, ret);
                if (!ret)
                {
                    // 对slavefcrec操作解锁
                    Status::getStatusInstance()->fcrecord_mutex.unlock();
                    return;
                }

                if (isvalid) // 预进洞有效
                {

                    // 如果当前所有相机全部返回
                    static int numofreceived = 0;
                    Status::getStatusInstance()->master_status_mutex.lock();
                    numofreceived = ++Status::getStatusInstance()->num_master_collect_status;
                    Status::getStatusInstance()->master_status_mutex.unlock();
                    if (numofreceived >= getMultiThreadTcpServer()->getNumOfThreads() * 4)
                        lzAcquizCtrlQueue->endCurrentMsg();
                    /*if (tmpModel.box1.camera_ref.compare(cameraindex.toStdString()) == 0) // 参考相机
                    {
                        slavefcrec.startfc_box1cameraref = end_fa;
                    }
                    else if (tmpModel.box1.camera.compare(cameraindex.toStdString()) == 0)
                    {
                        slavefcrec.startfc_box1camera = end_fa;
                    }
                    else if (tmpModel.box2.camera_ref.compare(cameraindex.toStdString()) == 0) // 参考相机
                    {
                        slavefcrec.startfc_box2cameraref = end_fa;
                    }
                    else if (tmpModel.box2.camera.compare(cameraindex.toStdString()) == 0)
                    {
                        slavefcrec.startfc_box2camera = end_fa;                      
                    }*/
                }
                else // 预进洞无效
                {                    
                    // 如果当前所有相机全部返回
                    static int numofreceived = 0;
                    Status::getStatusInstance()->master_status_mutex.lock();
                    numofreceived = ++Status::getStatusInstance()->num_master_collect_status;
                    Status::getStatusInstance()->master_status_mutex.unlock();
                    if (numofreceived >= getMultiThreadTcpServer()->getNumOfThreads() * 4)
                        lzAcquizCtrlQueue->endCurrentMsg();

                    if (tmpModel.box1.camera_ref.compare(cameraindex.toStdString()) == 0) // 参考相机
                    {
                        slavefcrec.startfc_box1cameraref = end_fa;
                    }
                    else if (tmpModel.box1.camera.compare(cameraindex.toStdString()) == 0)
                    {
                        slavefcrec.startfc_box1camera = end_fa;
                    }
                    else if (tmpModel.box2.camera_ref.compare(cameraindex.toStdString()) == 0) // 参考相机
                    {
                        slavefcrec.startfc_box2cameraref = end_fa;
                    }
                    else if (tmpModel.box2.camera.compare(cameraindex.toStdString()) == 0)
                    {
                        slavefcrec.startfc_box2camera = end_fa;           
                    }
                    emit signalToCameraFC(cameraindex, end_fa);
                }
                // 对slavefcrec操作解锁
                Status::getStatusInstance()->fcrecord_mutex.unlock();

                break;
            }
            case 1302:
            {
                QString returnVal = strList.value(2).mid(7);
                bool retvalbool = false;
                if (returnVal.compare("true") == 0)
                {
                    retvalbool = true;
                    emit signalMsgToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 确认停止采集").arg(slaveid));
                }
                else
                    emit signalMsgToGUI(WorkingStatus::Collecting, QObject::tr("[从控%1] 停止采集失败").arg(slaveid));
                Status::getStatusInstance()->slaveOutfromStatus(Collecting, slaveid);
                    
                // 当所有从机结束采集后记录采集信息到real文件
                //if (Status::getStatusInstance()->getSlaveNumInCurrentWorkingStatus() == 0)
                // 如果当前所有相机全部返回
                int numofreceived = 0;
                Status::getStatusInstance()->master_status_mutex.lock();
                numofreceived = ++Status::getStatusInstance()->num_master_collect_status;
                Status::getStatusInstance()->master_status_mutex.unlock();
                //@author 范翔 定西 -> 怕采集之中一个计算机断开连接。。。 无法保存文件了就
                if (numofreceived >= getMultiThreadTcpServer()->getNumOfThreads())
                {
                    lzAcquizCtrlQueue->endCurrentMsg();
                    // saveCollectInfo
                    this->saveCollectedInfo();
                    emit msgToGUIShowCProjectCTRL(Collecting, false);
                }
                collect_SlaveEndLineCloseCamera_R(slaveid, retvalbool);
                break;

            }
            case 1402: // 复位确认
            {
                collect_SlaveEndLineCloseCamera_R(slaveid, true);
                break;
            }
            default:break;
        }
    }
    else if (msgid < 2999)  // 计算
    {
        QString filename = "";
        QString camera_groupindex = "";
        int tunnelid = -1;
        int threadid = -1;
        if (msgid != 2100)
        {
            if (strList.length() < 6 || !strList.value(2).startsWith("threadid") || !strList.value(3).startsWith("filename"))
            {
                qDebug() << tr("解析字符出错") << msg;
                return;
            }
            threadid = strList.value(2).mid(9).toInt();
            filename = strList.value(3).mid(9);
        }
        switch(msgid)
        {
            case 2002:
            {
                // 2002,threadid=x,filename=xxx,tunnelid=x,camera_groupindex=xx,return=true,beginfc=x,endfc=x
                if (strList.length() < 9 || !strList.value(5).startsWith("camera_groupindex") || !strList.value(6).startsWith("return") || !strList.value(7).startsWith("beginfc") || !strList.value(8).startsWith("endfc"))
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                tunnelid = strList.value(4).mid(9).toInt();
                camera_groupindex = strList.value(5).mid(18);
                QString returnVal = strList.value(6).mid(7);
                qint64 beginfc = strList.value(7).mid(8).toLongLong();
                qint64 endfc = strList.value(8).mid(6).toLongLong();

                if (returnVal.compare("true") == 0)
                {
                    emit signalToCameraTask(WorkingStatus::Calculating, QString("%1").arg(slaveid), threadid, filename, "");
                    // 从控每个线程任务状态初始消息
                    emit signalToSlaveCameraTaskFC_init(WorkingStatus::Calculating, QString("%1").arg(slaveid), threadid, filename, beginfc, endfc);
                    
                    emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[从控%1] 线程%2正在计算隧道文件%3，相机组号为%4，起始帧号%5，终止帧号%6").arg(slaveid).arg(threadid).arg(filename).arg(camera_groupindex).arg(beginfc).arg(endfc));
                }
                else
                    emit signalErrorToGUI(WorkingStatus::Calculating, QObject::tr("[从控%1] 线程%2无法计算隧道文件%3，相机组号为%4，起始帧号%5，终止帧号%6").arg(slaveid).arg(threadid).arg(filename).arg(camera_groupindex).arg(beginfc).arg(endfc));
                break;
            }
            case 2010: // 2010,threadid=x,filename=xxx,tunnelid=x,camera_groupindex=xx
            {
                if (strList.length() < 6 || !strList.value(5).startsWith("camera_groupindex")
                            || !strList.value(4).startsWith("tunnelid"))
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                tunnelid = strList.value(4).mid(9).toInt();
                camera_groupindex = strList.value(5).mid(18);

                calculate_FeedbackPerTunnel_CameraGroup_R(threadid, filename, slaveid, tunnelid, camera_groupindex, false, 0);

                saveCalcuBackupInfo(LzProjectClass::Calculate);
                

                emit signalToCameraTask(WorkingStatus::Calculating, QString("%1").arg(slaveid), threadid, filename, "ok");
                emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[从控%1] 计算完隧道%2，第%3组相机数据").arg(slaveid).arg(filename).arg(camera_groupindex));
                break;
            }
            case 2020:
            {
                // 2020,threadid=x,filename=xxx,tunnelid=x,camera_groupindex=x,currentfc=x
                if (strList.length() < 7 || !strList.value(5).startsWith("camera_groupindex")
                        || !strList.value(4).startsWith("tunnelid") || !strList.value(6).startsWith("currentfc"))
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                tunnelid = strList.value(4).mid(9).toInt();
                camera_groupindex = strList.value(5).mid(18);
                long long currentfc = strList.value(6).mid(10).toLongLong();
                emit signalToCameraTaskFC(WorkingStatus::Calculating, QString("%1").arg(slaveid), threadid, filename, currentfc);
                //@TODO TODELETE
                calculate_FeedbackPerTunnel_CameraGroup_R(threadid, filename, slaveid, tunnelid, camera_groupindex, false, currentfc);

                break;
            }
            case 2200:
            {
                // 2200,threadid=x,filename=xxx,tunnelid=x,camera_groupindex=x,errortype=x
                if (strList.length() < 7 || !strList.value(5).startsWith("camera_groupindex")
                        || !strList.value(4).startsWith("tunnelid") || !strList.value(6).startsWith("errortype"))
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                tunnelid = strList.value(4).mid(9).toInt();
                camera_groupindex = strList.value(5).mid(18);
                QString error = strList.value(6).mid(10);
                emit signalErrorToGUI(WorkingStatus::Calculating, QObject::tr("[从控%1] 计算隧道%2出错!!错误类型%3。第%4组相机数据")
                                    .arg(slaveid).arg(filename).arg(error).arg(camera_groupindex));
                emit signalToCameraTask(WorkingStatus::Calculating, QString("%1").arg(slaveid), threadid, filename, QObject::tr("出错%1").arg(error));

                // 异常处理
                warning_calculate_R();
                break;
            }
            case 2100:
            {
                emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[从控%1] 全部任务计算完成").arg(slaveid));
                break;
            }
            case 2402:
            {
                // 确认暂停计算2402,return=true(false)
                if (strList.length() < 8 || !strList.value(5).startsWith("camera_groupindex")
                            || !strList.value(4).startsWith("tunnelid") || !strList.value(6).startsWith("interruptfc") || !strList.value(7).startsWith("return"))
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                tunnelid = strList.value(4).mid(9).toInt();
                camera_groupindex = strList.value(5).mid(18);
                long long interruptfc = strList.value(6).mid(12).toLongLong();
                QString retstr = strList.value(7).mid(7);
                hasChangeBackupCalcuStatus = true;

                // 记录帧号
                calculate_FeedbackPerTunnel_CameraGroup_R(threadid, filename, slaveid, tunnelid, camera_groupindex, true, interruptfc);

                if (retstr.compare("false") == 0)
                {
                    emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[从控%1] 暂停计算隧道%2第%3组相机数据失败!!错误类型，正在计算中，请稍后").arg(slaveid).arg(tunnelid).arg(camera_groupindex));
                }
                else if (retstr.compare("true") == 0)
                {
                    // 记录中断帧号处理函数
                    if (hasChangeBackupCalcuStatus)
                        saveCalcuBackupInfo(LzProjectClass::Calculate);

                    emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[从控%1] 暂停计算隧道%2第%3组相机数据成功，暂停帧号为%4").arg(slaveid).arg(tunnelid).arg(camera_groupindex).arg(interruptfc));
                }

                break;
            }
            default:break;
        }
    }
    else if (msgid < 3999)  // 备份
    {
        QString filename = "";
        QString index = "";
        int tunnelid = -1;
        int seqno = -1;
        if (msgid != 3100)
        {
            if (strList.length() < 4 || !strList.value(2).startsWith("filename") || !strList.value(3).startsWith("tunnelid"))
            {
                qDebug() << tr("解析字符出错") << msg;
                return;
            }
            filename = strList.value(2).mid(9);
            tunnelid = strList.value(3).mid(9).toInt();
        }
        switch(msgid)
        {
            case 3002:
            {
                // 3002,filename=xx,tunnelid=xx,return=true,beginfc=xx,endfc=xx
                if (strList.length() < 7 || !strList.value(4).startsWith("return") || !strList.value(5).startsWith("beginfc") || !strList.value(6).startsWith("endfc"))
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                QString returnVal = strList.value(4).mid(7);
                __int64 beginfc = strList.value(5).mid(8).toLongLong();
                __int64 endfc = strList.value(6).mid(6).toLongLong();

                if (returnVal.compare("true") == 0)
                {
                    emit signalToCameraTask(WorkingStatus::Backuping, QString("%1").arg(slaveid), 1, filename, "");
                    // 从控每个线程任务状态初始消息
                    emit signalToSlaveCameraTaskFC_init(WorkingStatus::Backuping, QString("%1").arg(slaveid), 1, filename, beginfc, endfc);

                    emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[从控%1] 文件%2确认开始备份").arg(slaveid).arg(filename));
                }
                else
                    emit signalErrorToGUI(WorkingStatus::Backuping, QObject::tr("[从控%1] 文件%2开始备份失败").arg(slaveid).arg(filename));
                break;
            }
            case 3010:
            {
                //3010,filename=xx
                QStringList strlistfilename = filename.split("_", QString::SkipEmptyParts);
                seqno = strlistfilename.at(0).toInt();
                index = strlistfilename.at(3).left(2);
                qDebug() << "seqno:" << seqno << ", cameraindex:" << index;

                backup_FeedbackPerTunnel_CameraGroup_R(filename, slaveid, 0, seqno, index, false, 0);
                
                saveCalcuBackupInfo(LzProjectClass::Backup);
                
                // 每个从机单线程备份，所以threadid号为1
                emit signalToCameraTask(WorkingStatus::Backuping, QString("%1").arg(slaveid), 1, filename, "ok");
                emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[从控%1] 文件%2备份完成").arg(slaveid).arg(filename));
                break;
            }
            case 3020:
            {
                // 3020,filename=xx,tunnelid=xx,currentfc=xx
                if (strList.length() < 5 || !strList.value(4).startsWith("currentfc"))
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                __int64 currentfc = strList.value(4).mid(10).toLongLong();
                emit signalToCameraTaskFC(WorkingStatus::Backuping, QString("%1").arg(slaveid), 1, filename, currentfc);

                break;
            }
            case 3100:
            {
                emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[从控%1] 完成全部备份任务").arg(slaveid));
                break;
            }
            case 3200:
            {
                //3200,filename=xxx,tunnelid=x,errortype=x
                if (strList.length() < 5 || !strList.value(4).startsWith("errortype"))
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                QString error = strList.value(4).mid(10);
                // 异常处理
                warning_calculate_R();
                // 备份采用单线程，threadid为1
                emit signalToCameraTask(WorkingStatus::Backuping, QString("%1").arg(slaveid), 1, filename, error);
                emit signalErrorToGUI(WorkingStatus::Backuping, QObject::tr("[从控%1] 备份文件%2出错!!错误类型%3").arg(slaveid).arg(filename).arg(error));
                break;
            }
            case 3402:
            {
                // 确认暂停备份3402,filename=xxx,tunnelid=xx,interruptfc=xx,return=true(false)

                if (strList.length() < 6 || !strList.value(4).startsWith("interruptfc") || !strList.value(5).startsWith("return"))
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return;
                }
                __int64 interruptfc = strList.value(4).mid(12).toLongLong();
                QString retstr = strList.value(5).mid(7);

                backup_FeedbackPerTunnel_CameraGroup_R(filename, slaveid, 0, seqno, index, true, interruptfc);

                hasChangeBackupCalcuStatus = true;

                if (retstr.compare("false") == 0)
                {
                    emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[从控%1] 暂停备份失败，文件正在备份，请稍后"));
                }
                else if (retstr.compare("true") == 0)
                {
                    // 记录中断帧号处理函数
                    if (hasChangeBackupCalcuStatus)
                        saveCalcuBackupInfo(LzProjectClass::Backup);

                    emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[从控%1] 暂停备份文件%2成功，暂停帧号为%3").arg(slaveid).arg(filename).arg(interruptfc));
                }
                break;
            }
            default:;
        }
    }
    else if (msgid < 4999)  // 主控工程切换
    {
        switch (msgid)
        {
            case 4200:
                break;
            case 4302:
                emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[从控%1] 建立临时工程成功").arg(slaveid));
                break;
            default:;
        }
    }
    else  // 其他（数据操作服务器，暂不考虑）
        return;
}

/*****************关于初始化*********************/
/**
 * 初始化index和cameraidex
 * 从控机接收消息后会调用从控程序的相机init函数
 * @param index和cameraidex的配置文件路径
 */
bool MasterProgram::init_NetworkConfigFile(QString filename)
{
    getMultiThreadTcpServer()->sendFileToSlaves(filename);
    emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  发送相机index与cameraid对应文件%1").arg(filename));
    return true;
}

/**
 * 初始化分中配置
 * @param 分中参数配置文件路径
 */
bool MasterProgram::init_FenZhongCalibParamFile()
{
        // 临时变量
    QString projectpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Calculate);
    QString filename;
    QString toip;
    bool ret = true;
    // 通过groupindex查找address，只将其发送到某一台从控机
    QList<SlaveModel>::const_iterator it = NetworkConfigList::getNetworkConfigListInstance()->listsnid()->begin();
    while (it != NetworkConfigList::getNetworkConfigListInstance()->listsnid()->end())
    {
        toip = QObject::tr(it->getHostAddress().c_str());
        filename = projectpath + "/calcu_calibration/" + QObject::tr(it->box1.box_fenzhong_calib_file.c_str());
        if (QFile(filename).exists())
            getMultiThreadTcpServer()->sendFileToOneSlave(toip, filename);
        else
        {
            ret = false;
            emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  发送分中参数文件失败,没有找到文件%1").arg(filename));
        }
        filename = projectpath + "/calcu_calibration/" + QObject::tr(it->box2.box_fenzhong_calib_file.c_str());
        if (QFile(filename).exists())
            getMultiThreadTcpServer()->sendFileToOneSlave(toip, filename);
        else
        {
            ret = false;
            emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  发送分中参数文件失败,没有找到文件%1").arg(filename));
        }
        it++;
    }
    emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  发送分中参数文件"));
    return ret;
}

/**
 * 初始化双目相机标定参数配置文件
 * 发给指定的从控机上的相机组
 */
bool MasterProgram::init_CameraCalibrationParamFile()
{
    // 临时变量
    QString projectpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Calculate);
    QString filename;
    QString toip;
    bool ret = true;
    // 通过groupindex查找address，只将其发送到某一台从控机
    QList<SlaveModel>::const_iterator it = NetworkConfigList::getNetworkConfigListInstance()->listsnid()->begin();
    while (it != NetworkConfigList::getNetworkConfigListInstance()->listsnid()->end())
    {
        toip = QObject::tr(it->getHostAddress().c_str());
        filename = projectpath + "/calcu_calibration/" + QObject::tr(it->box1.box_camera_calib_file.c_str());
        if (QFile(filename).exists())
            getMultiThreadTcpServer()->sendFileToOneSlave(toip, filename);
        else
        {
            ret = false;
            emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  发送双目视觉配置参数文件失败,没有找到文件%1").arg(filename));
        }
        filename = projectpath + "/calcu_calibration/" + QObject::tr(it->box2.box_camera_calib_file.c_str());
        if (QFile(filename).exists())
            getMultiThreadTcpServer()->sendFileToOneSlave(toip, filename);
        else
        {
            ret = false;
            emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  发送双目视觉配置参数文件失败,没有找到文件%1").arg(filename));
        }
        it++;
    }
    emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  发送双目视觉配置参数文件"));
    return ret;
}

/**
 * 初始化任务配置
 * @param 任务配置文件路径
 */
void MasterProgram::init_TaskFile(QString filename)
{
    getMultiThreadTcpServer()->sendFileToSlaves(filename);
    emit signalMsgToGUI(WorkingStatus::Collecting, QObject::tr("[主控]  发送任务文件%1").arg(filename));
}

/*****************设置采集计算备份线路及模式*******/
/**
 * 设置（采集、计算、备份、计算&&备份、文件信息校正，删除）线路及模式，告知所有从控机
 * 消息4201（预备消息，采集多两个参数）
 * 消息4202（正式开始消息）
 * 采集4201,filename=%1,lineid=%2,linename=%3,date=%4,operation_type=%5",triggermode="%6",exposuretime="%7").arg(filename).arg(lineid).arg(linename).arg(date).arg(statusStr)
 * 除采集外其他4201,filename=%1,lineid=%2,linename=%3,date=%4,operation_type=%5").arg(filename).arg(lineid).arg(linename).arg(date).arg(statusStr)
 * 4202,filename=%1,lineid=%2,linename=%3,date=%4,operation_type=%5").arg(filename).arg(lineid).arg(linename).arg(date).arg(statusStr)
 * @param filename 工程入口文件名
 * @param lineid 线路ID号
 * @param linename 线路名称
 * @param date 日期
 * @param status 要切换的工作模式（采集、计算、备份、计算&&备份、文件信息校正）
 * @param isprep 是否为准备状态 如果为true，检查是否可以切换，只做检查配置文件是否存在并解析，若为false，直接切换工作模式开始工作（采集、计算、...）
 */
void MasterProgram::workingMode_Setting(QString filename, int lineid, QString linename, QString date, WorkingStatus status, bool isprep)
{
    QString statusStr = "preparing";
    LzProjectClass projectclass;

    bool ret = true;
    if (status == Collecting)
    {
        statusStr = "collect";
        projectclass = LzProjectClass::Collect;
        ret = Status::getStatusInstance()->setWorkingStatus(Collecting);
    }
    else if (status == Correcting)
    {
        statusStr = "correct";
        projectclass = LzProjectClass::Main;
    }
    else if (status == Calculating)
    {
        statusStr = "calculate";
        projectclass = LzProjectClass::Calculate;
        ret = Status::getStatusInstance()->setWorkingStatus(Calculating_Backuping);
    }
    else if (status == Backuping)
    {
        statusStr = "backup";
        projectclass = LzProjectClass::Backup;
        ret = Status::getStatusInstance()->setWorkingStatus(Calculating_Backuping);
    }
    else if (status == Calculating_Backuping)
    {
        statusStr = "calculate_backup";
        projectclass = LzProjectClass::Calculate;
        ret = Status::getStatusInstance()->setWorkingStatus(Calculating_Backuping);
    }
    else if (status == Deleting)
    {
        statusStr = "delete";
        projectclass = LzProjectClass::Main;
    }
    QString currentprojectpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(projectclass);
    //qDebug() << currentprojectpath;

    if (!ret) // 不能重新设置新工作模式
    {
        emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  不能那个设置工作线路%1到新工作模式%2").arg(linename).arg(statusStr));
        return;
    }
    // 设置文件接收目录
    QString newprojectdir = MasterSetting::getSettingInstance()->getParentPath() + "/" + filename.left(filename.length() - 5);
    qDebug() << "new file receiver path" << newprojectdir;
    ret = multiThreadTcpServer->setReceiveFileSavingPath(newprojectdir);
    if (!ret) // 不能设置接收文件存储路径，当前有文件正在被接收
    {
        emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  不能那个设置工作线路%1到新工作模式%2，不能设置接收文件存储路径，当前有文件正在被接收").arg(linename).arg(statusStr));
        return;
    }

    if (isprep)
    {
        if (status == Deleting)
            return;
        multiThreadTcpServer->sendMessageToSlaves(QString("4201,filename=%1,lineid=%2,linename=%3,date=%4,operation_type=%5").arg(filename).arg(lineid).arg(linename).arg(date).arg(statusStr));
        emit signalMsgToGUI(WorkingStatus::Preparing, QObject::tr("[主控]  发送:设置工作线路%1及模式%2").arg(linename).arg(statusStr));
    }
    else
    {
        // 为了解析planfile和checkedfile
        ProjectModel & projm = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(projectclass);

        if (status == Collecting)
        {
            // 解析plantaskfile
            QString planfilename = currentprojectpath + "/" + projm.getPlanFilename();
            XMLTaskFileLoader * taskfileloader = new XMLTaskFileLoader(planfilename);
            bool ret2 = taskfileloader->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Collect));
            delete taskfileloader;
            if (!ret2)
            {
                emit signalMsgToGUI(WorkingStatus::Collecting, QObject::tr("[主控]  确认开始采集工作线路出错，无法解析任务文件%1").arg(planfilename));
                return;
            }
            // 初始化采集状态
            ret2 = Status::getStatusInstance()->initCollectStatus();
            if (!ret2)
            {
                emit signalMsgToGUI(WorkingStatus::Collecting, QObject::tr("[主控]  确认开始采集工作线路出错，从机未全部进入采集状态"));
                return;
            }
            else
            {
                Status::getStatusInstance()->master_status_mutex.lock();
                Status::getStatusInstance()->master_collect_status = MasterCollectingStatus::Collecting_Ready;
                lzAcquizCtrlQueue->setIsCollecting(true);
                Status::getStatusInstance()->num_master_collect_status = 0;
                Status::getStatusInstance()->master_status_mutex.unlock();
                /*ToDoMsg todomsg;///TODO目前打开相机未有反馈消息，
                todomsg.msg = "-24";
                lzAcquizCtrlQueue->pushBack(todomsg);*/
            }
        }
        else if (status == Correcting)
        {
            // 解析realtaskfile
            QString realfilename = currentprojectpath + "/" + projm.getRealFilename();
            XMLRealTaskFileLoader * taskfileloader1 = new XMLRealTaskFileLoader(realfilename);
            bool ret2 = taskfileloader1->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Main));
            delete taskfileloader1;
            if (!ret2)
            {
                emit signalMsgToGUI(WorkingStatus::Correcting, QObject::tr("[主控]  确认开始校正工作线路出错，无法解析实际采集任务文件%1").arg(realfilename));
                return;
            }
        }
        else if (status == Calculating)
        {
            // 解析realtaskfile
            QString realfilename = currentprojectpath + "/" + projm.getRealFilename();
            XMLRealTaskFileLoader * taskfileloader1 = new XMLRealTaskFileLoader(realfilename);
            bool ret2 = taskfileloader1->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Calculate));
            delete taskfileloader1;
            if (!ret2)
            {
                emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  确认开始计算工作线路出错，无法解析实际采集任务文件%1").arg(realfilename));
                return;
            }

            // 解析checkedtaskfile
            QString checkedfilename = currentprojectpath + "/" + projm.getCheckedFilename();
            XMLCheckedTaskFileLoader * taskfileloader = new XMLCheckedTaskFileLoader(checkedfilename);
            ret2 = taskfileloader->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate));
            delete taskfileloader;
            if (!ret2)
            {
                emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  确认开始计算工作线路出错，无法解析校正任务文件%1").arg(checkedfilename));
                return;
            }
            // 初始化计算备份状态
            ret2 = Status::getStatusInstance()->initCalculateBackupStatus();
            if (!ret2)
            {
                emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  确认开始计算线路出错，从机未全部进入计算状态"));
                return;
            }
            
            // @author 范翔,初始化融合计算队列
            lzFuseCalcQueue->initCalc();

            hasChangeBackupCalcuStatus = false;

            // 计算备份按钮界面激活
            emit makeGUIActive(Calculating, true);
        }
        else if (status == Backuping)
        {
            // 解析realtaskfile
            QString realfilename = currentprojectpath + "/" + projm.getRealFilename();
            XMLRealTaskFileLoader * taskfileloader1 = new XMLRealTaskFileLoader(realfilename);
            bool ret2 = taskfileloader1->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Backup));
            delete taskfileloader1;
            if (!ret2)
            {
                emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[主控]  确认开始备份工作线路出错，无法解析实际采集任务文件%1").arg(realfilename));
                return;
            }

            // 解析checkedtaskfile
            QString checkedfilename = currentprojectpath + "/" + projm.getCheckedFilename();
            XMLCheckedTaskFileLoader * taskfileloader = new XMLCheckedTaskFileLoader(checkedfilename);
            ret2 = taskfileloader->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Backup));
            delete taskfileloader;
            if (!ret2)
            {
                emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[主控]  确认开始备份工作线路出错，无法解析校正任务文件%1").arg(checkedfilename));
                return;
            }
            // 初始化计算备份状态
            ret2 = Status::getStatusInstance()->initCalculateBackupStatus();
            if (!ret2)
            {
                emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[主控]  确认开始备份线路出错，从机未全部进入备份状态"));
                return;
            }
            
            hasChangeBackupCalcuStatus = false;

            // 计算备份按钮界面激活
            emit makeGUIActive(Backuping, true);
        }
        else if (status == Deleting) // 删除操作需检查是否都已计算备份完成
        {
            // 解析realtaskfile
            QString realfilename = currentprojectpath + "/" + projm.getRealFilename();
            XMLRealTaskFileLoader * taskfileloader1 = new XMLRealTaskFileLoader(realfilename);
            bool ret2 = taskfileloader1->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Main));
            delete taskfileloader1;
            if (!ret2)
            {
                emit signalMsgToGUI(WorkingStatus::Deleting, QObject::tr("[主控]  确认开始删除工作线路出错，无法解析实际采集任务文件%1").arg(realfilename));
                return;
            }

            // 解析checkedtaskfile
            QString checkedfilename = currentprojectpath + "/" + projm.getCheckedFilename();
            XMLCheckedTaskFileLoader * taskfileloader = new XMLCheckedTaskFileLoader(checkedfilename);
            ret2 = taskfileloader->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main));
            delete taskfileloader;
            if (!ret2)
            {
                emit signalMsgToGUI(WorkingStatus::Deleting, QObject::tr("[主控]  确认开始删除工作线路出错，无法解析校正任务文件%1").arg(checkedfilename));
                return;
            }
            if (LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Backup).checkAllBackup() == false)
            {
                emit signalMsgToGUI(WorkingStatus::Deleting, QObject::tr("[主控]  确认开始删除工作线路出错，从文件%1知，未全部备份").arg(realfilename));
                return;
            }
            if (LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Backup).checkAllCalcuBackup() == false)
            {
                emit signalMsgToGUI(WorkingStatus::Deleting, QObject::tr("[主控]  确认开始删除工作线路出错，从文件%1知，未全部计算备份").arg(checkedfilename));
                return;
            }
        }

        // 发送消息
        if (status == Collecting)
        {
            QString triggermodeStr = "";
            switch (triggerMode)
            {
                case Lz_Camera_Continus: triggermodeStr = "continue"; break;
                case Lz_Camera_HardwareTrigger: triggermodeStr = "hardware"; break;
                case Lz_Camera_SoftwareTrigger: triggermodeStr = "software"; break;
                default: triggermodeStr = "continue"; break;
            }
            multiThreadTcpServer->sendMessageToSlaves(QString("4202,filename=%1,lineid=%2,linename=%3,date=%4,operation_type=%5,triggermode=%6,exposuretime=%7").arg(filename).arg(lineid).arg(linename).arg(date).arg(statusStr).arg(triggermodeStr).arg(exposureTime));
        }
        else
        {
            multiThreadTcpServer->sendMessageToSlaves(QString("4202,filename=%1,lineid=%2,linename=%3,date=%4,operation_type=%5").arg(filename).arg(lineid).arg(linename).arg(date).arg(statusStr));
        }
        emit signalMsgToGUI(status, QObject::tr("[主控]  发送:确认开始工作线路%1及模式%2").arg(linename).arg(statusStr));
        
        // 主控的采集控制状态
        emit signalToLights(HardwareType::Laser_Type, "0", HardwareStatus::Hardware_OnButFree);
    }
}

/**
 * 创建临时工程（计算）
 * @param filename 工程入口文件名
 * @param lineid 线路ID号
 * @param linename 线路名称
 * @param date 日期
 * @param tunnelid 隧道编号
 * @param seqno 采集序号
 * @param newstartframes 新的起始帧号
 * @param newendframes 新的终止帧号
 */
void MasterProgram::createTempProject(QString filename, int lineid, QString linename, QString date, int tunnelid, int seqno, long long newstartframes, long long newendframes)
{

    multiThreadTcpServer->sendMessageToSlaves(QString("4301,filename=%1,lineid=%2,linename=%3,date=%4,tunnelid=%5,seqno=%6,start_framecounter=%7,frame_num=%8")
                                                                                .arg(filename).arg(lineid).arg(linename).arg(date)
                                                                                .arg(tunnelid).arg(seqno).arg(newstartframes).arg(newendframes-newstartframes+1));
}

/*********************关于采集*******************/
/**
 * 预进洞
 * @param isvalid true 预进洞有效，false 预进洞无效
 */
void MasterProgram::collect_IntoTunnel(bool isvalid)
{
    // 如果没有在采集状态，不能处理
    if (!lzAcquizCtrlQueue->getIsCollecting())
        return;

    if (isvalid)
    {
        ToDoMsg todomsg;
        todomsg.msg = "-20";
        lzAcquizCtrlQueue->pushBack(todomsg);
        emit InTunnel(true);
    }
    else
    {
        ToDoMsg todomsg;
        todomsg.msg = "-21";
        lzAcquizCtrlQueue->pushBack(todomsg);
        emit InTunnel(false);
    }
}

/**
 * 出洞
 */
void MasterProgram::collect_OutFromTunnel()
{
    // 如果没有在采集状态，不能处理
    if (!lzAcquizCtrlQueue->getIsCollecting())
        return;

    ToDoMsg todomsg;
    todomsg.msg = "-22";
    lzAcquizCtrlQueue->pushBack(todomsg);
    emit OutTunnel();
}

/**
 * 结束采集线路，告知所有从控机关闭相机
 */
void MasterProgram::collect_EndCurrentLine()
{
    ToDoMsg todomsg;
    todomsg.msg = "-23";
    lzAcquizCtrlQueue->pushBack(todomsg);
}

/**
 * 软同步触发采集一帧
 */
void MasterProgram::collect_SoftwareTriggerOneFrame()
{
    ToDoMsg todomsg;
    todomsg.msg = "-25";
    lzAcquizCtrlQueue->pushBack(todomsg);
}

/**
 * 采集各个从机复位
 */
void MasterProgram::collect_ResetSlaves()
{
    ToDoMsg todomsg;
    todomsg.msg = "-29";
    lzAcquizCtrlQueue->pushBack(todomsg);
}

/**
 * 收到从机返回的关闭相机状态消息
 * @param slaveid
 * @param isclosecamera true 表示正常关闭，false表示非正常
 */
void MasterProgram::collect_SlaveEndLineCloseCamera_R(int slaveid, bool isclosecamera)
{
    SlaveModel tmpSlaveModel(1.0);
    bool ret = NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(slaveid, tmpSlaveModel);
    if (ret && isclosecamera)
    {
        emit signalToLights(HardwareType::Camera_Type, tmpSlaveModel.box1.camera_ref.c_str(), Hardware_Off);
        emit signalToLights(HardwareType::Camera_Type, tmpSlaveModel.box1.camera.c_str(), Hardware_Off);
        emit signalToLights(HardwareType::Camera_Type, tmpSlaveModel.box2.camera_ref.c_str(), Hardware_Off);
        emit signalToLights(HardwareType::Camera_Type, tmpSlaveModel.box2.camera.c_str(), Hardware_Off);
        emit signalToLights(Slave_Computer_Type, QString("%1").arg(slaveid), Hardware_OnButFree);
    }
}

/**
 * 收到从控机返回的采集完数据
 * @param slaveid
 * @param tunnelid
 * @param tunnelname
 * @param newseqno
 * @param newcameraindex
 * @param start_framecounter
 * @param end_framecounter
 */
void MasterProgram::collect_SlaveCollected_R(int slaveid, int tunnelid, QString tunnelname, int seqno, QString newcameraindex,
                            _int64 start_framecounter, _int64 end_framecounter)
{
    // 调用Status的addToCollectList记录状态
    Status::getStatusInstance()->addToCollectList(slaveid, tunnelid, tunnelname, seqno, newcameraindex, start_framecounter, end_framecounter);
    // 发送采集状态到主控
}

/**
 * 警告采集异常
 */
void MasterProgram::warning_collect_R()
{
}

/**
 * 希望查看原始图像
 */
bool MasterProgram::askForRawImages(QString realfilename, QString cameraindex, int seqno,int tunnelid, __int64 startFramecounter, int frameNum)
{
    // 查看的原图位置，写死在代码中
    QString toip = NetworkConfigList::getNetworkConfigListInstance()->findIPAddressHasCamera(cameraindex);
    if (toip.compare("") != 0)
    {
        qDebug() << toip << cameraindex;
        int slaveid = NetworkConfigList::getNetworkConfigListInstance()->findSlaveId(toip);
        if (slaveid < 0)
        {
            qDebug() << slaveid;
            return false;
        }
        //下面这一行代码，把函数中的几个参数传到了从控
        multiThreadTcpServer->sendMessageToOneSlave(toip, QString("4101,realfile=%1,camera_index=%2,seqno=%3,tunnelid=%4,start_framecounter=%5,frame_num=%6")
                                        .arg(realfilename).arg(cameraindex).arg(seqno).arg(tunnelid).arg(startFramecounter).arg(frameNum));
        return true;
    }
    else
    {
        qDebug() << "askForRawImages: can not find cameraid" << cameraindex << " in which ip address";
        return false;
    }
}

/*****************关于计算*********************/
/**
 * 开始计算
 */
void MasterProgram::calculate_beginStartAll()
{
    // 给每个从机的开始地方都不一样
    // 2001,filename,seqno=x,tunnelid=x,cameratgroup_index=x,restart=true,restart_frame=x
    bool ret = true;
    bool ret1 = Status::getStatusInstance()->setWorkingStatus(Calculating_Backuping);
    if (!ret1)
        ret = false;

    if (ret)
    {
        SlaveModel tmpsm(1.0);
        QString tmpfilename = "";
        QString date = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Calculate).right(13).left(8);
        QString tmpnameprefix = "";
        qDebug() << "date:" << date;
        int tmptunnelid = -1;
        
        bool ret2 = false;
        QString tmpboxindex;
        int tmphasbackup;
        __int64 tmpinterruptfc;
        int tmphasbackupnum = 0;
        int totalhasbackupnum = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).list()->size() * 2;

        for (int i = 1; i < 10; i++)
        {
            tmphasbackupnum = 0;

            QList<CheckedTunnelTaskModel>::iterator it = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).begin();
            while (it != LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).end())
            {
                tmpnameprefix = (*it).planTask.tunnelname.c_str() + QString("_") + date;
                tmptunnelid = (*it).planTask.tunnelnum;

                ret2 = NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(i, tmpsm);
                if (!ret2)
                {
                    ret = false;
                    break;
                }
                else
                {
                    // @author 范翔 @date 20150527 注释掉，因为RT计算变成两线程，与其他普通从控计算任务一致
                    /*if (tmpsm.getIsRT())
                    {
                        tmpboxindex = "RT";
                        ret2 = (*it).calcuItem.getHasBackupCalc(tmpboxindex.toStdString(), tmphasbackup, tmpinterruptfc);
                        if (ret2)
                        {
                            tmpfilename = tmpnameprefix +  QString("_RT") + ".mdat";
                            if (tmphasbackup == 0)
                                calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, false, 0);
                            else if (tmphasbackup == 1)
                                calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, true, tmpinterruptfc);
                            else
                                ;//tmphasbackupnum++;
                        }
                    }
                    else*/
                    {
                        tmpboxindex = QString("%1").arg(tmpsm.box1.boxindex);
                        ret2 = (*it).calcuItem.getHasBackupCalc(tmpboxindex.toStdString(), tmphasbackup, tmpinterruptfc);
                        if (ret2)
                        {
                            tmpfilename = tmpnameprefix +  QString("_%1").arg(tmpboxindex) + ".mdat";
                            if (tmphasbackup == 0)
                                calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, false, 0);
                            else if (tmphasbackup == 1)
                                calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, true, tmpinterruptfc);
                            else
                                emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  已经计算完%1").arg(tmpfilename));;//tmphasbackupnum++;
                        }
                        tmpboxindex = QString("%1").arg(tmpsm.box2.boxindex);
                        ret2 = (*it).calcuItem.getHasBackupCalc(tmpboxindex.toStdString(), tmphasbackup, tmpinterruptfc);
                        if (ret2)
                        {
                            tmpfilename = tmpnameprefix +  QString("_%1").arg(tmpboxindex) + ".mdat";
                            if (tmphasbackup == 0)
                                calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, false, 0);
                            else if (tmphasbackup == 1)
                                calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, true, tmpinterruptfc);
                            else
                                emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  已经计算完%1").arg(tmpfilename));;//tmphasbackupnum++;
                        }
                    }
                }
                it++;
            }
            emit calcubackupProgressingBar(WorkingStatus::Calculating, QString("%1").arg(i), 1, totalhasbackupnum, tmphasbackupnum, false);
        }
    }
    if (ret)
        emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  发送:开始计算"));
    else
        emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  无法开始计算"));
}

void MasterProgram::calculate_beginStartOneTunnel(int tocalcutunnelid)
{
    bool ret = true;
    bool ret1 = Status::getStatusInstance()->setWorkingStatus(Calculating_Backuping);
    if (!ret1)
        ret = false;

    if (ret)
    {
        SlaveModel tmpsm(1.0);
        QString tmpfilename = "";
        QString date = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Calculate).right(13).left(8);
        QString tmpnameprefix = "";
        qDebug() << "date:" << date;
        int tmptunnelid = -1;
        
        bool ret2 = false;
        QString tmpboxindex;
        int tmphasbackup;
        __int64 tmpinterruptfc;
        int tmphasbackupnum = 0;
        int totalhasbackupnum = 2; // 计算一条隧道，每个从机两个任务

        for (int i = 1; i < 10; i++)
        {
            tmphasbackupnum = 0;

            CheckedTunnelTaskModel tmpmodel;
            ret = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).getCheckedTunnelModel(tocalcutunnelid, tmpmodel);
            if (ret)
            {
                tmpnameprefix = tmpmodel.planTask.tunnelname.c_str() + QString("_") + date;
                tmptunnelid = tmpmodel.planTask.tunnelnum;

                ret2 = NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(i, tmpsm);
                if (!ret2)
                {
                    ret = false;
                    break;
                }
                else
                {
                    // @author 范翔 @date 20150625 注释掉，因为计算RT变成两线程计算，与其他普通从控机计算任务一致
                    /*if (tmpsm.getIsRT())
                    {
                        tmpboxindex = "RT";
                        ret2 = tmpmodel.calcuItem.getHasBackupCalc(tmpboxindex.toStdString(), tmphasbackup, tmpinterruptfc);
                        if (ret2)
                        {
                            tmpfilename = tmpnameprefix + QString("_RT") + ".mdat";
                            if (tmphasbackup == 0)
                                calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, false, 0);
                            else if (tmphasbackup == 1)
                                calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, true, tmpinterruptfc);
                            else
                                ;//tmphasbackupnum++;
                        }
                    }
                    else*/
                    {
                        tmpboxindex = QString("%1").arg(tmpsm.box1.boxindex);
                        ret2 = tmpmodel.calcuItem.getHasBackupCalc(tmpboxindex.toStdString(), tmphasbackup, tmpinterruptfc);
                        if (ret2)
                        {
                            tmpfilename = tmpnameprefix + QString("_%1").arg(tmpboxindex) + ".mdat";
                            if (tmphasbackup == 0)
                                calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, false, 0);
                            else if (tmphasbackup == 1)
                                calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, true, tmpinterruptfc);
                            else
                                emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  已经计算完%1").arg(tmpfilename));;//tmphasbackupnum++;
                        }
                        tmpboxindex = QString("%1").arg(tmpsm.box2.boxindex);
                        ret2 = tmpmodel.calcuItem.getHasBackupCalc(tmpboxindex.toStdString(), tmphasbackup, tmpinterruptfc);
                        if (ret2)
                        {
                            tmpfilename = tmpnameprefix + QString("_%1").arg(tmpboxindex) + ".mdat";
                            if (tmphasbackup == 0)
                                calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, false, 0);
                            else if (tmphasbackup == 1)
                                calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, true, tmpinterruptfc);
                            else
                                emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  已经计算完%1").arg(tmpfilename));;//tmphasbackupnum++;
                        }
                    }
                }
            }
            emit calcubackupProgressingBar(WorkingStatus::Calculating, QString("%1").arg(i), 1, totalhasbackupnum, tmphasbackupnum, false);
        }
    }
    if (ret)
        emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  发送:开始计算"));
    else
        emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  无法开始计算"));

}

void MasterProgram::calculate_StartOneFile(int slaveid, QString cameragroupindex, QString filename, int tunnelid, bool isinterrupted, qint64 interruptfc)
{
    SlaveModel tmpSlaveModel(1.0);
    bool ret = NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(slaveid, tmpSlaveModel);
    if (ret)
    {
        QString isinterruptedstr = "false";
        if (isinterrupted)
            isinterruptedstr = "true";
        multiThreadTcpServer->sendMessageToOneSlave(tmpSlaveModel.getHostAddress().c_str(), QString("2001,filename=%1,tunnelid=%2,cameragroup_index=%3,isinterrupted=%4,interruptfc=%5").arg(filename).arg(tunnelid).arg(cameragroupindex).arg(isinterruptedstr).arg(interruptfc));
    }
}

/**
 * 暂停计算
 */
void MasterProgram::calculate_Stop()
{
    WorkingStatus curstatus = Status::getStatusInstance()->getWorkingStatus();
    if (WorkingStatus::Calculating_Backuping == curstatus || WorkingStatus::Calculating == curstatus)
    {
        multiThreadTcpServer->sendMessageToSlaves("2401");
        emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  发送:暂停计算"));

        if (hasChangeBackupCalcuStatus)
            saveCalcuBackupInfo(LzProjectClass::Calculate);
    }
    else
        emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  无法暂停计算"));
}

/**
 * 融合计算之前的数据检查
 */
void MasterProgram::calculate_Fuse_checkisready()
{
    ////TODO
}

/**
 * 开始融合计算和提高度计算
 * 前提是在双目及车底RT计算的中间结果文件全部返回之后
 */
void MasterProgram::calculate_Fuse_beginStartAll()
{
    bool ret = true;
    bool ret1 = Status::getStatusInstance()->setWorkingStatus(Calculating_Backuping);
    if (!ret1)
        ret = false;

    QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Calculate);
    QString date = projectname.right(13).left(8);
    ToDoMsg todomsg;

    int tmphasbackup;
    __int64 tmpinterruptfc;
    int tmphasbackupnum = 0;
    int totalhasbackupnum = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).list()->size();

    bool ret2 = false;
    QList<CheckedTunnelTaskModel>::iterator it = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).begin();
    while (it != LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).end())
    {

        ret2 = (*it).calcuItem.getHasBackupCalc("fuse", tmphasbackup, tmpinterruptfc);
        if (ret2)
        {
            if (tmphasbackup < 1)
            {
                int tunnelid = (*it).planTask.tunnelnum;
                QString filename = QObject::tr((*it).planTask.tunnelname.c_str()) + "_" + date;
                todomsg.msg = QString("-30,%1,%2,%3,%4").arg(tunnelid).arg(filename).arg(tmphasbackup).arg(tmpinterruptfc);
                lzFuseCalcQueue->pushBack(todomsg);
            }
            else
                ;//tmphasbackupnum++;
        }
        it++;
    }
    emit calcubackupProgressingBar(WorkingStatus::Calculating, QString("%1").arg(0), 1, totalhasbackupnum, tmphasbackupnum, false);
    if (ret)
        emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  开始融合计算"));
    else
        emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  无法开始融合计算"));

}

void MasterProgram::calculate_Fuse_beginStartOneTunnel(int tunnelid)
{
    bool ret = true;
    bool ret1 = Status::getStatusInstance()->setWorkingStatus(Calculating_Backuping);
    if (!ret1)
        ret = false;

    QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Calculate);
    QString date = projectname.right(13).left(8);
    ToDoMsg todomsg;

    int tmphasbackup;
    __int64 tmpinterruptfc;
    int tmphasbackupnum = 0;
    int totalhasbackupnum = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).list()->size();

    bool ret2 = false;
    QList<CheckedTunnelTaskModel>::iterator it = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).begin();
    while (it != LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).end())
    {

        ret2 = (*it).calcuItem.getHasBackupCalc("fuse", tmphasbackup, tmpinterruptfc);
        if (ret2)
        {
            if (tmphasbackup < 1)
            {
                if (tunnelid == (*it).planTask.tunnelnum)
                {
                    QString filename = QObject::tr((*it).planTask.tunnelname.c_str()) + "_" + date;
                    todomsg.msg = QString("-30,%1,%2,%3,%4").arg(tunnelid).arg(filename).arg(tmphasbackup).arg(tmpinterruptfc);
                    lzFuseCalcQueue->pushBack(todomsg);
                    break;
                }
            }
            else
                tmphasbackupnum++;
        }
        it++;
    }
    emit calcubackupProgressingBar(WorkingStatus::Calculating, QString("%1").arg(0), 1, totalhasbackupnum, tmphasbackupnum, false);
    if (ret)
        emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  开始融合计算"));
    else
        emit signalMsgToGUI(WorkingStatus::Calculating, QObject::tr("[主控]  无法开始融合计算"));
}

void MasterProgram::calculate_Fuse_stop()
{
    lzFuseCalcQueue->suspend();
}

void MasterProgram::calculate_ExtractHeight_beginStartAll()
{
    bool ret = true;
    bool ret1 = Status::getStatusInstance()->setWorkingStatus(Calculating_Backuping);
    if (!ret1)
        ret = false;

    // TODO
}

void MasterProgram::calculate_ExtractHeight_beginStartOneTunnel(int tunnelid)
{
    bool ret = true;
    bool ret1 = Status::getStatusInstance()->setWorkingStatus(Calculating_Backuping);
    if (!ret1)
        ret = false;

    QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Calculate);
    QString date = projectname.right(13).left(8);
    ToDoMsg todomsg;

    int tmphasbackup;
    __int64 tmpinterruptfc;
    int tmphasbackupnum = 0;
    int totalhasbackupnum = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).list()->size();

    bool ret2 = false;
    QList<CheckedTunnelTaskModel>::iterator it = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).begin();
    while (it != LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).end())
    {

        ret2 = (*it).calcuItem.getHasBackupCalc("fuse", tmphasbackup, tmpinterruptfc);
        if (ret2)
        {

            if (tunnelid == (*it).planTask.tunnelnum)
            {
                QString filename = QObject::tr((*it).planTask.tunnelname.c_str()) + "_" + date;
                QString parentpath = MasterSetting::getSettingInstance()->getParentPath();
                if (tmphasbackup == 1)
                    todomsg.msg = QString("-32,%1,%2,%3,%4,%5").arg(tunnelid).arg(filename).arg(tmphasbackup).arg(tmpinterruptfc).arg(parentpath);
                else
                {
                    todomsg.msg = QString("-32,%1,%2,%3,%4,%5").arg(tunnelid).arg(filename).arg(tmphasbackup).arg(0).arg(parentpath);
                    QString projectpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(Calculate);

                    qDebug() << projectpath + "/fuse_calcu/" + filename + ".fdat";

                    qDebug() << projectpath + "/syn_calcu/" + filename + ".syn";

                }
                lzFuseCalcQueue->pushBack(todomsg);
                break;
            }
        }
        it++;
    }
}

void MasterProgram::calculate_FeedbackPerTunnel_CameraGroup_R(int threadid, QString filename, int slaveid, int tunnelid, QString cameragroup_index, bool isinterrupt, __int64 interruptfc)
{
    // 记录计算进度
    //mutex.lock();
    //Status::getStatusInstance()->recordCalculate(filename, slaveid, tunnelid, cameragroup_index.toStdString());
    //mutex.unlock();
    bool ret;
    CheckedTunnelTaskModel & tmp = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).getCheckedTunnelModel(tunnelid, ret);
    if (ret)
    {
        if (!isinterrupt)
            ret = tmp.calcuItem.saveHasBackupCalc(cameragroup_index.toStdString(), 2, 0);
        else
            ret = tmp.calcuItem.saveHasBackupCalc(cameragroup_index.toStdString(), 1, interruptfc);
        if (!ret)
            qDebug() << QObject::tr("记录计算状态出错！没有%1这个相机组号！").arg(cameragroup_index);
        else
        {
            hasChangeBackupCalcuStatus = true;
            emit calcubackupProgressingBar(WorkingStatus::Calculating, QString("%1").arg(slaveid), threadid, 0, 0, true);
        }
    }
    else
        qDebug() << QObject::tr("记录计算状态出错！在CheckedTaskList中找不到含隧道ID为%1的元素").arg(tunnelid);
}

/**
 * 警告计算异常
 */
void MasterProgram::warning_calculate_R()
{
}

/*****************关于备份 *********************/
/**
 * 开始备份
 */
void MasterProgram::backup_beginStartAll()
{
    // 给每个从机的开始地方都不一样
    // 3001,filename==x
    bool ret = true;
    bool ret1 = Status::getStatusInstance()->setWorkingStatus(Calculating_Backuping);
    if (!ret1)
        ret = false;
    else
    {
        SlaveModel tmpsm(1.0);
        QString tmpfilename = "";
        QString date = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Backup).right(13).left(8);
        QString tmpnameprefix = "";
        int tmptunnelid = -1;
        qDebug() << "date:" << date;
        bool ret2 = false;

        QString tmpcamindex;
        int tmphasbackup;
        __int64 backupinterruptpos;
        __int64 beginfc;
        __int64 endfc;
        int tmphasbackupnum = 0;
        int totalhasbackupnum = LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Backup).list()->size() * 4;


        for (int i = 1; i < 10; i++)
        {
            tmphasbackupnum = 0;

            QList<CheckedTunnelTaskModel>::iterator it = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Backup).begin();
            while (it != LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Backup).end())
            {

                std::list<CheckedItem>::iterator it2 = (*it).begin();
                while (it2 != (*it).end())
                {
                    tmpnameprefix = QObject::tr("%1_%2_%3").arg(it2->seqno).arg(it2->tunnel_name.c_str()).arg(date);
                    tmptunnelid = (*it).planTask.tunnelnum;

                    for (int j = 0; j < 4; j++)
                    {
                        switch (j)
                        {
                            case 0: tmpcamindex = tmpsm.box1.camera_ref.c_str(); break;
                            case 1: tmpcamindex = tmpsm.box1.camera.c_str(); break;
                            case 2: tmpcamindex = tmpsm.box2.camera_ref.c_str(); break;
                            case 3: tmpcamindex = tmpsm.box2.camera.c_str(); break;
                            default:break;
                        }
                        it2->getStartEndVal(tmpcamindex.toLocal8Bit().constData(), beginfc, endfc);
                        ret2 = it2->getHasBackup(tmpcamindex.toStdString(), tmphasbackup, backupinterruptpos);
                        if (ret2)
                        {
                            tmpfilename = tmpnameprefix +  QString("_%1").arg(tmpcamindex) + ".dat";

                            if (tmphasbackup < 5) // 还未做备份
                                backup_StartOneFile(i, tmpfilename, tmptunnelid, false, 0, beginfc, endfc);
                            else if (tmphasbackup == 5)
                                backup_StartOneFile(i, tmpfilename, tmptunnelid, true, backupinterruptpos, beginfc, endfc);
                            else
                                emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[主控]  已经备份完%1").arg(tmpfilename));;//tmphasbackupnum++;
                                ;//tmphasbackupnum++;
                        }
                    }

                    it2++;
                }

                it++;
            }

            emit calcubackupProgressingBar(WorkingStatus::Backuping, QString("%1").arg(i), 1, totalhasbackupnum, tmphasbackupnum, false);
        }
    }
    if (ret)
        emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[主控]  发送:开始备份"));
    else
        emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[主控]  无法开始备份"));
}

void MasterProgram::backup_beginStartOneTunnel(int tobackuptunnelid /*,QString totalseqnostr*/)
{
    bool ret = true;
    bool ret1 = Status::getStatusInstance()->setWorkingStatus(Calculating_Backuping);
    if (!ret1)
        ret = false;
    else
    {
        SlaveModel tmpsm(1.0);
        QString tmpfilename = "";
        QString date = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Backup).right(13).left(8);
        QString tmpnameprefix = "";
        int tmptunnelid = -1;
        qDebug() << "date:" << date;
        bool ret2 = false;
        
        //QStringList seqlist = totalseqnostr.split("-");
        
        QString tmpcamindex;
        int tmphasbackup;
        __int64 backupinterruptpos;
        __int64 beginfc;
        __int64 endfc;
        int tmphasbackupnum = 0;
        int totalhasbackupnum = 0;

        for (int i = 1; i < 10; i++)
        {
            tmphasbackupnum = 0;

            CheckedTunnelTaskModel tmpmodel;
            ret = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).getCheckedTunnelModel(tobackuptunnelid, tmpmodel);

            if (ret)
            {
                totalhasbackupnum = tmpmodel.getRealList()->size();

                tmpnameprefix = tmpmodel.planTask.tunnelname.c_str() + QString("_") + date;
                tmptunnelid = tmpmodel.planTask.tunnelnum;

                ret2 = NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(i, tmpsm);

                std::list<CheckedItem>::iterator it2 = tmpmodel.begin();
                while (it2 != tmpmodel.end())
                {
                    tmpnameprefix = QObject::tr("%1_%2_%3").arg(it2->seqno).arg(it2->tunnel_name.c_str()).arg(date);
                    for (int j = 0; j < 4; j++)
                    {
                        switch (j)
                        {
                            case 0: tmpcamindex = tmpsm.box1.camera_ref.c_str(); break;
                            case 1: tmpcamindex = tmpsm.box1.camera.c_str(); break;
                            case 2: tmpcamindex = tmpsm.box2.camera_ref.c_str(); break;
                            case 3: tmpcamindex = tmpsm.box2.camera.c_str(); break;
                            default:break;
                        }
                        it2->getStartEndVal(tmpcamindex.toLocal8Bit().constData(), beginfc, endfc);
                        ret2 = it2->getHasBackup(tmpcamindex.toStdString(), tmphasbackup, backupinterruptpos);
                        if (ret2)
                        {
                            tmpfilename = tmpnameprefix +  QString("_%1").arg(tmpcamindex) + ".dat";
                            if (tmphasbackup < 5) // 还未做备份
                                backup_StartOneFile(i, tmpfilename, tmptunnelid, false, 0, beginfc, endfc);
                            else if (tmphasbackup == 5)
                                backup_StartOneFile(i, tmpfilename, tmptunnelid, true, backupinterruptpos, beginfc, endfc);
                            else
                                emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[主控]  已经备份完%1").arg(tmpfilename));;//tmphasbackupnum++;
                                ;//tmphasbackupnum++;
                        }
                    }

                    it2++;
                }
            }
            emit calcubackupProgressingBar(WorkingStatus::Backuping, QString("%1").arg(i), 1, totalhasbackupnum, tmphasbackupnum, false);
        }
    }
    if (ret)
        emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[主控]  发送:开始备份"));
    else
        emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[主控]  无法开始备份"));

}

void MasterProgram::backup_StartOneFile(int slaveid, QString filename, int tunnelid, bool isinterrupted, qint64 interruptfc, qint64 beginfc, qint64 endfc)
{
    SlaveModel tmpSlaveModel(1.0);
    bool ret = NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(slaveid, tmpSlaveModel);
    if (ret)
    {
        QString isinterruptedstr = "false";
        if (isinterrupted)
            isinterruptedstr = "true";
        multiThreadTcpServer->sendMessageToOneSlave(tmpSlaveModel.getHostAddress().c_str(), QString("3001,filename=%1,tunnelid=%2,isinterrupted=%3,interruptedfilepos=%4,beginfc=%5,endfc=%6").arg(filename).arg(tunnelid).arg(isinterruptedstr).arg(interruptfc).arg(beginfc).arg(endfc));
    }
}

/**
 * 暂停备份
 */
void MasterProgram::backup_Stop()
{
    WorkingStatus curstatus = Status::getStatusInstance()->getWorkingStatus();
    if (WorkingStatus::Calculating_Backuping == curstatus || WorkingStatus::Backuping == curstatus)
    {
        multiThreadTcpServer->sendMessageToSlaves("3401");
        emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[主控]  发送:暂停备份"));

        if (hasChangeBackupCalcuStatus)
            saveCalcuBackupInfo(LzProjectClass::Backup);
    }
    else
        emit signalMsgToGUI(WorkingStatus::Backuping, QObject::tr("[主控]  无法暂停备份"));
}

/**
 * 开始融合计算和提高度计算
 * 前提是在双目及车底RT计算的中间结果文件全部返回之后
 */
void MasterProgram::backup_Master_begin()
{
    QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Backup);
    QString date = projectname.right(13).left(8);
    ToDoMsg todomsg;

    int tmphasbackup;
    __int64 tmpbackupfc;
    int tmphasbackupnum = 0;
    int totalhasbackupnum = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Backup).list()->size();

    bool ret2 = false;
    QList<CheckedTunnelTaskModel>::iterator it = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Backup).begin();
    while (it != LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Backup).end())
    {

        ret2 = (*it).calcuItem.getHasBackupCalc("fuse", tmphasbackup, tmpbackupfc);
        if (ret2)
        {
            if (tmphasbackup < 2)
            {
                int tunnelid = (*it).planTask.tunnelnum;
                QString filename = QObject::tr((*it).planTask.tunnelname.c_str()) + "_" + date;
                todomsg.msg = QString("-40,%1,%2,%3").arg(filename).arg(tmphasbackup).arg(tmpbackupfc);
                lzBackupCalcQueue->pushBack(todomsg);
            }
            else
                tmphasbackupnum++;
        }
        it++;
    }
    emit calcubackupProgressingBar(WorkingStatus::Calculating, QString("%1").arg(0), 1, totalhasbackupnum, tmphasbackupnum, false);
}

void MasterProgram::backup_FeedbackPerTunnel_CameraGroup_R(QString filename, int slaveid, int type, int seqno, QString index, bool isinterrupt, __int64 interruptfc)
{
    // 记录备份进度
    //mutex.lock();
    //Status::getStatusInstance()->recordBackup(filename, slaveid, type, seqno, index.toStdString());
    //mutex.unlock();
    if (slaveid == 0) // 主控备份融合计算结果和双目计算结果
    {
        bool ret;
        CheckedTunnelTaskModel & tmp = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Backup).getCheckedTunnelModel(seqno, ret);;
        if (ret)
        {
            if (!isinterrupt)
                ret = tmp.calcuItem.saveHasBackupCalc(index.toStdString(), 6, 0);
            else
                ret = tmp.calcuItem.saveHasBackupCalc(index.toStdString(), 5, interruptfc);
            if (!ret)
                qDebug() << QObject::tr("记录主控备份状态出错！没有%1这个相机组号！").arg(index);
            else
            {
                hasChangeBackupCalcuStatus = true;
                emit calcubackupProgressingBar(WorkingStatus::Backuping, QString("%1").arg(0), 1, 0, 0, true);
            }
        }
        else
            qDebug() << QObject::tr("记录主控备份状态出错！在CheckedTaskList中找不到含隧道ID为%1的元素").arg(seqno);

        return;
    }

    bool ret;
    RealTask & tmp = LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Backup).getRealTaskBySeqno(seqno, ret);
    if (ret)
    {
        ret = tmp.saveToHasBackup(index.toStdString(), true);
        hasChangeBackupCalcuStatus = true;
        emit calcubackupProgressingBar(WorkingStatus::Backuping, QString("%1").arg(slaveid), 1, 0, 0, true);

        if (!ret)
            qDebug() << QObject::tr("记录备份状态出错！没有%1这个相机！").arg(index);
    }
    else
        qDebug() << QObject::tr("记录备份状态出错！在RealTaskList中找不到含序列号为%1的元素").arg(seqno);
}

/**
 * 警告备份异常
 */
void MasterProgram::warning_backup_R()
{
}

int MasterProgram::indexStringToUiInteger(QString index)
{
    int returnindex = -1;
    const char* a = index.toStdString().c_str();
    if (index.length() == 2)
    {
        returnindex = 2 * ((int)a - int('A'));
        returnindex += index.mid(1).toInt();
        return returnindex;
    } 
    else if (index.length() == 1)
    {
        returnindex = ((int)a - int('A'));
        return returnindex;
    }
    else
        return -1;
}