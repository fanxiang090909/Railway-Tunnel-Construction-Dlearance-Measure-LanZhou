#include "lz_acquizctrl_queue.h"

/**
 * 从控机采集队列类实现
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
LzAcquizCtrlQueue::LzAcquizCtrlQueue(QObject * parent) : LzMsgQueue(parent)
{
    isCollecting = false;

    isAcquizing = false;
    hasCamerainit = false;
    
}

LzAcquizCtrlQueue::~LzAcquizCtrlQueue()
{
    // 如果正在采集
}

/**
 * 是否正在采集状态
 */
void LzAcquizCtrlQueue::setIsCollecting(bool newcollecting) { isCollecting = newcollecting; }
/**
 * 是否正在采集状态
 */
bool LzAcquizCtrlQueue::getIsCollecting() { return isCollecting; }

bool LzAcquizCtrlQueue::getIsAcquizing()
{
    return isAcquizing;
}

void LzAcquizCtrlQueue::endCurrentMsg()
{
    Status::getStatusInstance()->master_status_mutex.lock();
    Status::getStatusInstance()->num_master_collect_status = 0;
    Status::getStatusInstance()->master_status_mutex.unlock();
    endMsg();
}

/**
 * 任务命令解析
 * 实现父类的纯虚函数
 */
bool LzAcquizCtrlQueue::parseMsg(QString msg)
{
    QStringList strList = msg.split(",", QString::SkipEmptyParts);
 
    int msgid = strList.value(0).toInt();
    //qDebug() << "msgid= " << msgid;
    if (msgid <= 0)
    {
        switch (msgid)
        {
            case -20: // 预进洞有效
            {
                emit signalParsedMsgToSlave(QObject::tr("[主控]  ***************************************************************预进洞有效**********************************************************"));
                isAcquizing = true;
                collect_IntoTunnel(true);
                break;
            }
            case -21: // 预进洞无效
            {
                emit signalParsedMsgToSlave(QObject::tr("[主控]  ***************************************************************预进洞无效**********************************************************"));
                collect_IntoTunnel(false);
                break;
            }
            case -22: // 出洞
            {
                emit signalParsedMsgToSlave(QObject::tr("[主控]  *****************************************************************出洞**************************************************************"));
                collect_OutFromTunnel();
                break;
            }
            case -23: // 结束采集线路
            {
                collect_EndCurrentLine();
                break;
            }
            case -24: // 开始采集线路
            {
                //collect_StartCurrentLine();
                //TODOendMsg();
                break;
            }
            case -25: // 软触发一次
            {
                collect_SoftwareTriggerOneFrame();
                // 没有返回消息,不检查各从机是否执行完
                endMsg();
                break;
            }
            case -29: // 采集复位
            {
                collect_ResetSlaves();
                endMsg();
                break;
            }
            default: return false; 
        }
    }
    else
        return false; 
    return true;
}

/*****************关于采集*********************/
/**
 * 预进洞
 * @param isvalid true 预进洞有效，false 预进洞无效
 */
void LzAcquizCtrlQueue::collect_IntoTunnel(bool isvalid)
{
    /*if (!Status::getStatusInstance()->getWorkingStatus() == Collecting)
    {
        emit signalParsedMsgToSlave(QObject::tr("[主控]  未在采集，不能发送:预进洞信号"));
        return;
    }*/
    // 对slavefcrec操作加锁
    Status::getStatusInstance()->fcrecord_mutex.lock();
    _int64 currentfc_master = Status::getStatusInstance()->endfc_master;
    float currentmile_master = Status::getStatusInstance()->endmile_master;
    int numofframes = currentfc_master - Status::getStatusInstance()->startfc_master + 1;
    QString currentfileprefix = Status::getStatusInstance()->collecting_filenameprefix_master;
    if (!isvalid) // 预进洞无效，修改startfc
    {
        Status::getStatusInstance()->startfc_master = currentfc_master + 1;
        Status::getStatusInstance()->startmile_master = currentmile_master + MILEINTERVAL;
    }
    bool ret;
    for (int i = 1; i < 10; i++)
    {
        SlaveCollectFrameCounterRecord & rec = Status::getStatusInstance()->getSlaveCollectFrameCounterRecord(i, ret);

        rec.startfc_box1cameraref += numofframes;
        rec.startfc_box1camera += numofframes;
        rec.startfc_box2cameraref += numofframes;
        rec.startfc_box2camera += numofframes;

        if (ret)
        {
            collect_StartCurrentTunnelFiles(i, isvalid, rec.startfc_box1cameraref, rec.startfc_box1camera, rec.startfc_box2cameraref, rec.startfc_box2camera, numofframes);
            qDebug() << QObject::tr("[主控]  发送:预进洞有效给从控%1，有效帧数为%2，四个终止帧号%3，%4，%5，%6").arg(i).arg(numofframes).arg(rec.startfc_box1cameraref).arg(rec.startfc_box1camera).arg(rec.startfc_box2cameraref).arg(rec.startfc_box2camera);
        }
    }
    // 对slavefcrec操作解锁
    Status::getStatusInstance()->fcrecord_mutex.unlock();

    if (ret)
    {
        if (isvalid)
        {
            Status::getStatusInstance()->master_status_mutex.lock();
            Status::getStatusInstance()->master_collect_status = MasterCollectingStatus::Collecting_InTunnelValid;
            Status::getStatusInstance()->num_master_collect_status = 0;
            Status::getStatusInstance()->master_status_mutex.unlock();

            emit signalParsedMsgToSlave(QObject::tr("[主控]  发送:预进洞有效，有效帧数为%1").arg(numofframes));
            // 主控的采集控制状态
            emit signalToCameraFC("master", currentfc_master);
            emit signalToCameraTask(Collecting, "master", currentfileprefix);
            emit signalToLights(HardwareType::Laser_Type, "0", HardwareStatus::Hardware_Working);
        }
        else
        {
            Status::getStatusInstance()->master_status_mutex.lock();
            Status::getStatusInstance()->master_collect_status = MasterCollectingStatus::Collecting_InTunnelNotValid;
            Status::getStatusInstance()->num_master_collect_status = 0;
            Status::getStatusInstance()->master_status_mutex.unlock();

            emit signalParsedMsgToSlave(QObject::tr("[主控]  发送:预进洞无效，无效帧数为%1").arg(numofframes));
            // 主控的采集控制状态
            emit signalToCameraFC("master", currentfc_master);
        }
    }
}

/**
 * 出洞
 */
void LzAcquizCtrlQueue::collect_OutFromTunnel()
{
    /*if (!Status::getStatusInstance()->getWorkingStatus() == Collecting)
    {
        emit signalParsedMsgToSlave(QObject::tr("[主控]  未在采集，不能发送:出洞信号"));
        return;
    }*/
    // 对slavefcrec操作加锁
    Status::getStatusInstance()->fcrecord_mutex.lock();
    _int64 startfc_master = Status::getStatusInstance()->startfc_master;
    float startmile_master = Status::getStatusInstance()->startmile_master;
    _int64 currentfc_master = Status::getStatusInstance()->endfc_master;
    float currentmile_master = Status::getStatusInstance()->endmile_master;
    int numofframes = currentfc_master - startfc_master + 1;

    // 记录startfc和endfc
    bool ret = Status::getStatusInstance()->addToCollectList_master(startfc_master, currentfc_master, startmile_master, currentmile_master);
    if (!ret)
    {
        qDebug() << tr("主控添加记录addToCollectList_master出错，planTaskList到end()出界！");
        emit signalParsedMsgToSlave(QObject::tr("[主控]  添加记录addToCollectList_master出错，planTaskList到end()出界！"));
    }
    // 修改startfc
    Status::getStatusInstance()->startfc_master = currentfc_master + 1;
    Status::getStatusInstance()->startmile_master = currentmile_master + MILEINTERVAL;

    for (int i = 1; i < 10; i++)
    {
        SlaveCollectFrameCounterRecord & rec = Status::getStatusInstance()->getSlaveCollectFrameCounterRecord(i, ret);

        rec.startfc_box1cameraref += numofframes;
        rec.startfc_box1camera += numofframes;
        rec.startfc_box2cameraref += numofframes;
        rec.startfc_box2camera += numofframes;

        if (ret)
        {
            collect_EndCurrentTunnelFiles(i, rec.startfc_box1cameraref, rec.startfc_box1camera, rec.startfc_box2cameraref, rec.startfc_box2camera, numofframes, startmile_master, currentmile_master); 
            qDebug() << QObject::tr("[主控]  发送:出洞信号给从控%1，有效帧数为%2，四个终止帧号%3，%4，%5，%6").arg(i).arg(numofframes).arg(rec.startfc_box1cameraref).arg(rec.startfc_box1camera).arg(rec.startfc_box2cameraref).arg(rec.startfc_box2camera);
        }
    }
    // 对slavefcrec操作解锁
    Status::getStatusInstance()->fcrecord_mutex.unlock();
    if (ret)
    {
        Status::getStatusInstance()->master_status_mutex.lock();
        Status::getStatusInstance()->master_collect_status = MasterCollectingStatus::Collecting_OutfromTunnel;
        Status::getStatusInstance()->num_master_collect_status = 0;
        Status::getStatusInstance()->master_status_mutex.unlock();

        emit signalToCameraFC("master", currentfc_master);
        emit signalParsedMsgToSlave(QObject::tr("[主控]  发送:出洞信号，隧道内有效帧数为%1").arg(numofframes));
        // 主控的采集控制状态
        emit signalToLights(HardwareType::Laser_Type, "0", HardwareStatus::Hardware_OnButFree);
    }
}

/**
 * 发送预进洞信号
 * 给不同从机发送不同数据
 * 预进洞有效、无效信号 1103,isvalid=true(预进洞有效，false预进洞无效),slaveid=x,endframe_box1camref=x, endframe_box1cam=x, endframe_box2camref=x, endframe_box2cam=x,numofframes=x   
 */
void LzAcquizCtrlQueue::collect_StartCurrentTunnelFiles(int slaveid, bool isvalid, _int64 endframe_box1camref, _int64 endframe_box1cam, _int64 endframe_box2camref, _int64 endframe_box2cam, int numofframes)
{
    // @author范翔，因为sendMessageToOneSlave的一个参数slaveid函数不能使用
    // 遂修改为slaveip函数
    SlaveModel tmpSlaveModel(1.0);
    bool ret = NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(slaveid, tmpSlaveModel);
    QString isvalidstr = "false";
    if (isvalid)
        isvalidstr = "true";
    if (ret)
        emit signalMsgToSlave(tmpSlaveModel.getHostAddress().c_str(), QString("1103,isvalid=%1,slaveid=%2,endframe_box1camref=%3,endframe_box1cam=%4,endframe_box2camref=%5,endframe_box2cam=%6,numofframes=%7")
                                            .arg(isvalidstr).arg(slaveid).arg(endframe_box1camref).arg(endframe_box1cam).arg(endframe_box2camref).arg(endframe_box2cam).arg(numofframes));
    //emit signalMsgToGUI(QObject::tr("[主控]  发送:预进洞有效？%1对从机%2,有无效帧数%3,四个endframe：%4, %5, %6, %7").arg(isvalid).arg(slaveid).arg(numofframes).arg(endframe_box1camref).arg(endframe_box1cam).arg(endframe_box2camref).arg(endframe_box2cam).arg(numofframes));
}

/**
 * 发送出洞信号
 * 给不同从机发送不同数据
 * 告知所有从控机，结束采集上一文件并打开下一文件等待接收下一帧外触发数据
 * 相机停止拍摄 1101,slaveid=x.endframe_box1camref=x, endframe_box1cam=x, endframe_box2camref=x, endframe_box2cam=x,numofframes=x, start_mile=x,end_mile=x
 */
void LzAcquizCtrlQueue::collect_EndCurrentTunnelFiles(int slaveid, _int64 endframe_box1camref, _int64 endframe_box1cam, _int64 endframe_box2camref, _int64 endframe_box2cam, int numofframes, float start_mile, float end_mile)
{
    // @author范翔，因为sendMessageToOneSlave的一个参数slaveid函数不能使用
    // 遂修改为slaveip函数
    SlaveModel tmpSlaveModel(1.0);
    bool ret = NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(slaveid, tmpSlaveModel);
    if (ret)
    {
        emit signalMsgToSlave(tmpSlaveModel.getHostAddress().c_str(), QString("1101,slaveid=%1,endframe_box1camref=%2,endframe_box1cam=%3,endframe_box2camref=%4,endframe_box2cam=%5,numofframes=%6,start_mile=%7,end_mile=%8")
                                            .arg(slaveid).arg(endframe_box1camref).arg(endframe_box1cam).arg(endframe_box2camref).arg(endframe_box2cam).arg(numofframes).arg(start_mile).arg(end_mile));
    }
    //emit signalMsgToGUI(QObject::tr("[主控]  发送:出洞信号，对从机%1,有无效帧数%2,四个endframe：%3, %4, %5, %6，起始里程%7，终止里程%8").arg(slaveid).arg(endframe_box1camref).arg(endframe_box1cam).arg(endframe_box2camref).arg(endframe_box2cam).arg(numofframes).arg(start_mile).arg(end_mile));
}

/**
 * 结束采集线路，告知所有从控机关闭相机
 * 【注意】不在这里记录real文件信息，因为最后一条隧道的文件信息尚未返回，从机还没有完全停止
 */
void LzAcquizCtrlQueue::collect_EndCurrentLine()
{
    //WorkingStatus curstatus = Status::getStatusInstance()->getWorkingStatus();
    //if (WorkingStatus::Collecting == curstatus)
    {
        Status::getStatusInstance()->master_status_mutex.lock();
        Status::getStatusInstance()->master_collect_status = MasterCollectingStatus::Collecting_Ready;
        setIsCollecting(false);
        Status::getStatusInstance()->num_master_collect_status = 0;
        Status::getStatusInstance()->master_status_mutex.unlock();

        emit signalMsgToSlaves("1301");
        emit signalParsedMsgToSlave(QObject::tr("[主控]  发送:停止采集"));
        // 主控的采集控制状态
        emit signalToLights(HardwareType::Laser_Type, "0", HardwareStatus::Hardware_Off);
    }
    //else
    //    emit signalParsedMsgToSlave(QObject::tr("[主控]  未在采集，不能发送停止采集信号"));
}

/**
 * 软同步触发采集一帧
 */
void LzAcquizCtrlQueue::collect_SoftwareTriggerOneFrame()
{
    collect_RecordFrameCounterAndMile();
    emit signalMsgToSlaves("1003");
}

/**
 * 采集复位
 */
void LzAcquizCtrlQueue::collect_ResetSlaves()
{
    emit signalMsgToSlaves("1401");
}

/**
 * 私有函数
 * 记录里程和帧号
 */
void LzAcquizCtrlQueue::collect_RecordFrameCounterAndMile()
{
    // 对slavefcrec操作加锁
    Status::getStatusInstance()->fcrecord_mutex.lock();
    Status::getStatusInstance()->endfc_master++;
    Status::getStatusInstance()->endmile_master += MILEINTERVAL;
    // 对slavefcrec操作解锁
    Status::getStatusInstance()->fcrecord_mutex.unlock();
}