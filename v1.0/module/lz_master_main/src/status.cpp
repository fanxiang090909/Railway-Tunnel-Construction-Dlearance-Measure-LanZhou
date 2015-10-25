#include "status.h"
#include "setting_master.h"
#include "network_config_list.h"
#include "realtask_list.h"
#include "xmlrealtaskfileloader.h"

#include "lz_project_access.h"

#include <QDebug>
#include <QFile>
#include <QTime>

/**
 * 行车状态类实现
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-10-13
 */

// singleton设计模式，静态私有实例变量
Status * Status::statusInstance = NULL;

// 相机数量36路
//int Status::COLLECT_CAMERA_NUM = 36;

// 计算时候相机组数。16组
//int Status::CALCULATE_CAMERA_GROUP_NUM = 16;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
Status* Status::getStatusInstance()
{
    if (statusInstance == NULL)
    {
        statusInstance = new Status();
    }
    return statusInstance;
}

// singleton单一设计模式，构造函数私有
Status::Status(QObject *parent) :
    QObject(parent)
{
    workingStatus = Preparing;

    initWorkingStatus();

    slaveFrameCounterRecord = vector<SlaveCollectFrameCounterRecord>();
    for (int i = 0; i < 9; i++)
    {
        SlaveCollectFrameCounterRecord newslavefcrecord;
        newslavefcrecord.startfc_box1cameraref = 0;
        newslavefcrecord.startfc_box1camera = 0;
        newslavefcrecord.startfc_box2cameraref = 0;
        newslavefcrecord.startfc_box2camera = 0;
        newslavefcrecord.endfc_box1cameraref = 0;
        newslavefcrecord.endfc_box1camera = 0;
        newslavefcrecord.endfc_box2cameraref = 0;
        newslavefcrecord.endfc_box2camera = 0;
        slaveFrameCounterRecord.push_back(newslavefcrecord);
    }
    startfc_master = -1;
    endfc_master = -1;
    startmile_master = 0;
    endmile_master = 0;
    seq = -1;
    collecting_filenameprefix_master = "";

    master_collect_status = MasterCollectingStatus::Collecting_Wrong;

    collectList = list<CollectRecord>();
    slave1RecordList = list<SlaveCollectRecord>();
    slave2RecordList = list<SlaveCollectRecord>();
    slave3RecordList = list<SlaveCollectRecord>();
    slave4RecordList = list<SlaveCollectRecord>();
    slave5RecordList = list<SlaveCollectRecord>();
    slave6RecordList = list<SlaveCollectRecord>();
    slave7RecordList = list<SlaveCollectRecord>();
    slave8RecordList = list<SlaveCollectRecord>();
    slave9RecordList = list<SlaveCollectRecord>();

    // 初始化各种状态
    workingStatus = Preparing;
    // 当前连接的从机数量
    slavenum_connected = 0;
    slave1_connected = false;
    slave2_connected = false;
    slave3_connected = false;
    slave4_connected = false;
    slave5_connected = false;
    slave6_connected = false;
    slave7_connected = false;
    slave8_connected = false;
    slave9_connected = false;

    // 正常当前工作状态的从机数量
    slavenum_in_collect = 0;
    master_in_collect = false;
    // 记录每个从机的工作状态
    slave1_in_collect = false;
    slave2_in_collect = false;
    slave3_in_collect = false;
    slave4_in_collect = false;
    slave5_in_collect = false;
    slave6_in_collect = false;
    slave7_in_collect = false;
    slave8_in_collect = false;
    slave9_in_collect = false;

    // 正常当前工作状态的从机数量
    slavenum_in_calcu = 0;
    master_in_calcu = false;
    // 记录每个从机的工作状态
    slave1_in_calcu = false;
    slave2_in_calcu = false;
    slave3_in_calcu = false;
    slave4_in_calcu = false;
    slave5_in_calcu = false;
    slave6_in_calcu = false;
    slave7_in_calcu = false;
    slave8_in_calcu = false;
    slave9_in_calcu = false;

    // 正常当前工作状态的从机数量
    slavenum_in_backup = 0;
    master_in_backup = false;
    // 记录每个从机的工作状态
    slave1_in_backup = false;
    slave2_in_backup = false;
    slave3_in_backup = false;
    slave4_in_backup = false;
    slave5_in_backup = false;
    slave6_in_backup = false;
    slave7_in_backup = false;
    slave8_in_backup = false;
    slave9_in_backup = false;

    /*slave1CalcuList = list<SlaveCalculateRecord>();
    slave2CalcuList = list<SlaveCalculateRecord>();
    slave3CalcuList = list<SlaveCalculateRecord>();
    slave4CalcuList = list<SlaveCalculateRecord>();
    slave5CalcuList = list<SlaveCalculateRecord>();
    slave6CalcuList = list<SlaveCalculateRecord>();
    slave7CalcuList = list<SlaveCalculateRecord>();
    slave8CalcuList = list<SlaveCalculateRecord>();
    slave9CalcuList = list<SlaveCalculateRecord>();

    slave1UncompletedList = list<Uncompleted>();
    slave2UncompletedList = list<Uncompleted>();
    slave3UncompletedList = list<Uncompleted>();
    slave4UncompletedList = list<Uncompleted>();
    slave5UncompletedList = list<Uncompleted>();
    slave6UncompletedList = list<Uncompleted>();
    slave7UncompletedList = list<Uncompleted>();
    slave8UncompletedList = list<Uncompleted>();
    slave9UncompletedList = list<Uncompleted>();*/
}

Status::~Status()
{
    slaveFrameCounterRecord.clear();

    collectList.clear();

    slave1RecordList.clear();
    slave2RecordList.clear();
    slave3RecordList.clear();
    slave4RecordList.clear();
    slave5RecordList.clear();
    slave6RecordList.clear();
    slave7RecordList.clear();
    slave8RecordList.clear();
    slave9RecordList.clear();

    /*slave1CalcuList.clear();
    slave2CalcuList.clear();
    slave3CalcuList.clear();
    slave4CalcuList.clear();
    slave5CalcuList.clear();
    slave6CalcuList.clear();
    slave7CalcuList.clear();
    slave8CalcuList.clear();
    slave9CalcuList.clear();

    slave1UncompletedList.clear();
    slave2UncompletedList.clear();
    slave3UncompletedList.clear();
    slave4UncompletedList.clear();
    slave5UncompletedList.clear();
    slave6UncompletedList.clear();
    slave7UncompletedList.clear();
    slave8UncompletedList.clear();
    slave9UncompletedList.clear();*/
}

/**
 * 得到从机当前记录帧号结构体
 * @param ret 返回值是否成功，引用
 */
SlaveCollectFrameCounterRecord & Status::getSlaveCollectFrameCounterRecord(int slaveid, bool & ret)
{
    ret = false;
    if (slaveid > 0 && slaveid <= 9)
    {
        ret = true;
        return slaveFrameCounterRecord[slaveid - 1];
    }
    return slaveFrameCounterRecord[0];
}

/**
 * 返回list中tunnelid中已知
 */
list<CollectRecord>::iterator Status::findCollectListIterator(int tunnelid, int seqno)
{
    collectListmutex.lock();
    list<CollectRecord>::reverse_iterator it = collectList.rbegin();
    for (; it != collectList.rend(); it++)
    {
        if ((*it).record_tunnelid == tunnelid && (*it).seqno == seqno)
        {
            collectListmutex.unlock();
            return it.base();
        }
    }
    collectListmutex.unlock();
    return it.base();
}

/**
 * 返回list中seqno已知
 */
list<CollectRecord>::reverse_iterator Status::findCollectListIterator(int seqno)
{
    list<CollectRecord>::reverse_iterator it = collectList.rbegin();
    for (; it != collectList.rend(); it++)
    {
        if ((*it).seqno == seqno)
        {
            return it;
        }
    }
    return it;
}

/**
 * 删除list中已知tunnelid和remark的元素
 * 调用STL list的erace(list<xxx>::iterator方法)it1 = mylist.erase (it1);
 */
int Status::deleteFromCollectList(int tunnelid, int seqno)
{
    collectListmutex.lock();
    list<CollectRecord>::iterator it = findCollectListIterator(tunnelid, seqno);
    collectListmutex.unlock();

    if (it != collectList.end())
        return -1;

    if ((*it).record_tunnelid == tunnelid && (*it).seqno == seqno)
        it = collectList.erase(it);

    return 1;
}

/**
 * 删除采集隧道链表中全部元素
 */
bool Status::deleteAllCollectList()
{
    collectListmutex.lock();
    collectList.clear();
    collectListmutex.unlock();
    return true;
}

/**
 * 删除slaveRecordList全部元素
 */
bool Status::deleteAllSlaveRecordList()
{
    bool ret = false;
    for (int i = 1; i <= 9; i++)
        getSlaveRecordList(i, ret).clear();
    return true;
}

/**
 * 保存采集时所记录的信息至RealTaskList，并写入文件
 * @see RealTaskList
 * @see XMLRealTaskFileLoader
 */
bool Status::saveCollectedInfoToRealFile()
{
    LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Collect).clear();

    collectListmutex.lock();
    list<CollectRecord>::iterator it = collectList.begin();

    bool ret;
    list<SlaveCollectRecord>::iterator slaveit1 = getSlaveRecordList(1, ret).begin();
    list<SlaveCollectRecord>::iterator slaveit2 = getSlaveRecordList(2, ret).begin();
    list<SlaveCollectRecord>::iterator slaveit3 = getSlaveRecordList(3, ret).begin();
    list<SlaveCollectRecord>::iterator slaveit4 = getSlaveRecordList(4, ret).begin();
    list<SlaveCollectRecord>::iterator slaveit5 = getSlaveRecordList(5, ret).begin();
    list<SlaveCollectRecord>::iterator slaveit6 = getSlaveRecordList(6, ret).begin();
    list<SlaveCollectRecord>::iterator slaveit7 = getSlaveRecordList(7, ret).begin();
    list<SlaveCollectRecord>::iterator slaveit8 = getSlaveRecordList(8, ret).begin();
    list<SlaveCollectRecord>::iterator slaveit9 = getSlaveRecordList(9, ret).begin();
    bool hasslaveit1 = true;
    bool hasslaveit2 = true;
    bool hasslaveit3 = true;
    bool hasslaveit4 = true;
    bool hasslaveit5 = true;
    bool hasslaveit6 = true;
    bool hasslaveit7 = true;
    bool hasslaveit8 = true;
    bool hasslaveit9 = true;

    // 临时，遍历用
    list<SlaveCollectRecord>::iterator tmpslaveit = slaveit1;
    list<SlaveCollectRecord>::iterator tmpslaveit_end = getSlaveRecordList(1, ret).end();

    std::string tmpcameraindex;
    _int64 tmpstartfc, tmpendfc, tmpfclength = -1;

    while (it != collectList.end()) // 更新，returnnum++
    {
        if (((*it).returnnum) > MasterSetting::getSettingInstance()->getMaxSalveNum())
        {
            qDebug() << tr("已达到最大数量SlALE_MAX:%1,请检查").arg(MasterSetting::getSettingInstance()->getMaxSalveNum());
            collectListmutex.unlock();
            //return false;
        }

        RealTask tmp;
        tmp.seqno = (*it).seqno;

        tmp.tunnelid = (*it).record_tunnelid;
        tmp.tunnelname = (*it).record_tunnelname;
        tmp.datetime = (*it).record_time;
        
        // 记录有效帧数
        // 改为遍历所有的后再记录
        //tmp.valid_frames = (*it).end_framecounter - (*it).start_framecounter;
        tmp.isvalid = true;
        tmpfclength = -1;

        tmp.start_frame_master = (*it).start_framecounter;
        tmp.end_frame_master = (*it).end_framecounter;
        tmp.start_mile = (*it).start_mile;
        tmp.end_mile = (*it).end_mile;

        bool * tmphasit = &hasslaveit1;
        for (int i = 1; i <= 9; i++)
        {
            switch(i)
            {
                case 1: tmpslaveit = slaveit1; tmpslaveit_end = getSlaveRecordList(1, ret).end(); tmphasit = &hasslaveit1; break;
                case 2: tmpslaveit = slaveit2; tmpslaveit_end = getSlaveRecordList(2, ret).end(); tmphasit = &hasslaveit2; break;
                case 3: tmpslaveit = slaveit3; tmpslaveit_end = getSlaveRecordList(3, ret).end(); tmphasit = &hasslaveit3; break;
                case 4: tmpslaveit = slaveit4; tmpslaveit_end = getSlaveRecordList(4, ret).end(); tmphasit = &hasslaveit4; break;
                case 5: tmpslaveit = slaveit5; tmpslaveit_end = getSlaveRecordList(5, ret).end(); tmphasit = &hasslaveit5; break;
                case 6: tmpslaveit = slaveit6; tmpslaveit_end = getSlaveRecordList(6, ret).end(); tmphasit = &hasslaveit6; break;
                case 7: tmpslaveit = slaveit7; tmpslaveit_end = getSlaveRecordList(7, ret).end(); tmphasit = &hasslaveit7; break;
                case 8: tmpslaveit = slaveit8; tmpslaveit_end = getSlaveRecordList(8, ret).end(); tmphasit = &hasslaveit8; break;
                case 9: tmpslaveit = slaveit9; tmpslaveit_end = getSlaveRecordList(9, ret).end(); tmphasit = &hasslaveit9; break;
            }
            if (tmphasit && tmpslaveit != tmpslaveit_end)
            {
                for (int j = 0; j < 4; j++)
                {
                    switch(j)
                    {
                        case 0: tmpcameraindex = (*tmpslaveit).index_camera1; tmpstartfc = (*tmpslaveit).start_framecounter_camera1; tmpendfc = (*tmpslaveit).end_framecounter_camera1; break;
                        case 1: tmpcameraindex = (*tmpslaveit).index_camera2; tmpstartfc = (*tmpslaveit).start_framecounter_camera2; tmpendfc = (*tmpslaveit).end_framecounter_camera2; break;
                        case 2: tmpcameraindex = (*tmpslaveit).index_camera3; tmpstartfc = (*tmpslaveit).start_framecounter_camera3; tmpendfc = (*tmpslaveit).end_framecounter_camera3; break;
                        case 3: tmpcameraindex = (*tmpslaveit).index_camera4; tmpstartfc = (*tmpslaveit).start_framecounter_camera4; tmpendfc = (*tmpslaveit).end_framecounter_camera4; break;
                    }
                    
                    // 更新起始终止里程帧号
                    tmp.saveToStartEndVal(tmpcameraindex, tmpstartfc, tmpendfc);
                    
                    // 主控帧号与A1一致
                    // @author 范翔
                    // @date 20150526
                    if (tmpcameraindex.compare("A1") == 0)
                        tmp.saveToStartEndVal("master", tmpstartfc, tmpendfc);

                    // 计算有效帧数
                    if (tmpfclength > 0) // 如果已有，比较是否有效帧数相同
                    {
                        if (tmpfclength != tmpendfc - tmpstartfc)
                             tmp.isvalid = false;
                    }
                    else
                        tmpfclength = tmpendfc - tmpstartfc;
                }
            }
            else
            {
                (*tmphasit) = false; 
                continue;
            }
        }

        tmp.valid_frames = tmpfclength + 1;

        LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Collect).pushBack(tmp);
        //(*it).returnnum;
        it++;
        if (hasslaveit1) slaveit1++;
        if (hasslaveit2) slaveit2++;
        if (hasslaveit3) slaveit3++;
        if (hasslaveit4) slaveit4++;
        if (hasslaveit5) slaveit5++;
        if (hasslaveit6) slaveit6++;
        if (hasslaveit7) slaveit7++;
        if (hasslaveit8) slaveit8++;
        if (hasslaveit9) slaveit9++;
    }
    collectListmutex.unlock();

    ProjectModel & projm = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Collect);
    QString savedir = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Collect) + "/" + projm.getRealFilename();
    XMLRealTaskFileLoader* rtl = new XMLRealTaskFileLoader(savedir);
    ret = rtl->saveFile(LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Collect));
    qDebug() << "saveCollectedInfoToRealTaskFile = " << ret << ", filename: " << savedir;
    delete rtl;

    return ret; 
}

/**
 * 返回实际采集的list，方便遍历显示等界面或其他操作
 */
list<CollectRecord> & Status::getCollectList() { return collectList; }
list<CollectRecord>::iterator Status::collectListBegin() { return collectList.begin(); }
list<CollectRecord>::iterator Status::collectListEnd() { return collectList.end(); }

/**
 * 返回从控机的备份
 * @param slaveid 从机索引号
 * @param ret 输出返回值 true为存在slaveid
 */
list<SlaveCollectRecord> & Status::getSlaveRecordList(int slaveid, bool & ret)
{
    ret = true;
    switch (slaveid)
    {
        case 1:return slave1RecordList;
        case 2:return slave2RecordList;
        case 3:return slave3RecordList;
        case 4:return slave4RecordList;
        case 5:return slave5RecordList;
        case 6:return slave6RecordList;
        case 7:return slave7RecordList;
        case 8:return slave8RecordList;
        case 9:return slave9RecordList;
        default: ret = false; return slave1RecordList;
    }
}

/**
 * 返回从控机的计算
 * @param int slaveid
 */
/*list<SlaveCalculateRecord> & Status::getSlaveCalcuList(int slaveid, bool & ret)
{
    ret = true;
    switch (slaveid)
    {
        case 1:return slave1CalcuList;
        case 2:return slave2CalcuList;
        case 3:return slave3CalcuList;
        case 4:return slave4CalcuList;
        case 5:return slave5CalcuList;
        case 6:return slave6CalcuList;
        case 7:return slave7CalcuList;
        case 8:return slave8CalcuList;
        case 9:return slave9CalcuList;
        default: ret = false; return slave1CalcuList;
    }
}*/

/**
 * 返回从控机的计算和备份的未完成结点
 * @param int slaveid
 */
/*list<Uncompleted> & Status::getUncompletedList(int slaveid, bool & ret)
{
    ret = true;
    switch (slaveid)
    {
        case 0:return masterUncompletedList;
        case 1:return slave1UncompletedList;
        case 2:return slave2UncompletedList;
        case 3:return slave3UncompletedList;
        case 4:return slave4UncompletedList;
        case 5:return slave5UncompletedList;
        case 6:return slave6UncompletedList;
        case 7:return slave7UncompletedList;
        case 8:return slave8UncompletedList;
        case 9:return slave9UncompletedList;
        default: ret = false; return masterUncompletedList;
    }
}*/

/**
 * 返回主控的总工作状态
 * 计算&备份   或   采集
 */
WorkingStatus Status::getWorkingStatus()
{
    return workingStatus;
}

/**
 * 返回从控机的是否处在某状态中的引用, 若slaveid=0返回的是主机是否在该状态中的引用
 * @param tofind 查找的状态
 * @param slaveid 从机索引号
 * @param ret 返回值true为存在slaveid
 * @return 返回从机slaveid是否在该状态中 
 */
bool & Status::getSlaveWorkingStatus(WorkingStatus tofind, int slaveid, bool & ret)
{
    ret = true;
    switch (tofind)
    {
        case Preparing:
        {
            switch (slaveid)
            {
                case 1: return slave1_connected;
                case 2: return slave2_connected;
                case 3: return slave3_connected;
                case 4: return slave4_connected;
                case 5: return slave5_connected;
                case 6: return slave6_connected;
                case 7: return slave7_connected;
                case 8: return slave8_connected;
                case 9: return slave9_connected;
                default: ret = false; return slave1_connected;
            }
            break;
        }
        case Collecting:
        {
            switch (slaveid)
            {
                case 0: return master_in_collect;
                case 1: return slave1_in_collect;
                case 2: return slave2_in_collect;
                case 3: return slave3_in_collect;
                case 4: return slave4_in_collect;
                case 5: return slave5_in_collect;
                case 6: return slave6_in_collect;
                case 7: return slave7_in_collect;
                case 8: return slave8_in_collect;
                case 9: return slave9_in_collect;
                default: ret = false; return master_in_collect;
            }
            break;
        }
        case Calculating:
        {
            switch (slaveid)
            {
                case 0: return master_in_calcu;
                case 1: return slave1_in_calcu;
                case 2: return slave2_in_calcu;
                case 3: return slave3_in_calcu;
                case 4: return slave4_in_calcu;
                case 5: return slave5_in_calcu;
                case 6: return slave6_in_calcu;
                case 7: return slave7_in_calcu;
                case 8: return slave8_in_calcu;
                case 9: return slave9_in_calcu;
                default: ret = false; return master_in_calcu;
            }
            break;
        }
        case Backuping:
        {
            switch (slaveid)
            {
                case 0: return master_in_backup;
                case 1: return slave1_in_backup;
                case 2: return slave2_in_backup;
                case 3: return slave3_in_backup;
                case 4: return slave4_in_backup;
                case 5: return slave5_in_backup;
                case 6: return slave6_in_backup;
                case 7: return slave7_in_backup;
                case 8: return slave8_in_backup;
                case 9: return slave9_in_backup;
                default: ret = false; return master_in_backup;
;
            }
            break;
        }
        default: ret = false;
        return master_in_backup;
    }
}

/**
 * 得到从机状态数值引用，只有Collecting，Calculating，和Backuping状态可以有值，其他均不做记录
 * @param stat 状态
 * @param ret 是否成功找到返回值的引用
 * @return 返回从机slaveid是否在该状态中
 */
int & Status::getSlaveNumWorkingStatus(WorkingStatus stat, bool & ret)
{
    ret = true;
    switch(stat)
    {
        case Collecting: return slavenum_in_collect;
        case Calculating: return slavenum_in_calcu;
        case Backuping: return slavenum_in_backup;
        default: ret = false; return slavenum_in_collect;
    }
}

// ！不安全的做法，为了从机断开连接后，即出现故障，主控需要关闭
void Status::initWorkingStatus()
{
    workingStatus = Preparing;
    // 采集
    slavenum_in_collect = 0;
    master_in_collect = false;
    slave1_in_collect = false;
    slave2_in_collect = false;
    slave3_in_collect = false;
    slave4_in_collect = false;
    slave5_in_collect = false;
    slave6_in_collect = false;
    slave7_in_collect = false;
    slave8_in_collect = false;
    slave9_in_collect = false;
    // 计算
    slavenum_in_calcu = 0;
    master_in_calcu = false;
    slave1_in_calcu = false;
    slave2_in_calcu = false;
    slave3_in_calcu = false;
    slave4_in_calcu = false;
    slave5_in_calcu = false;
    slave6_in_calcu = false;
    slave7_in_calcu = false;
    slave8_in_calcu = false;
    slave9_in_calcu = false;
    // 备份
    slavenum_in_backup = 0;
    master_in_backup = false;
    // 记录每个从机的工作状态
    slave1_in_backup = false;
    slave2_in_backup = false;
    slave3_in_backup = false;
    slave4_in_backup = false;
    slave5_in_backup = false;
    slave6_in_backup = false;
    slave7_in_backup = false;
    slave8_in_backup = false;
    slave9_in_backup = false;
}

/**
 * 更新工作状态
 * @return true 可以更新到新状态（currentStatusNum==0）
 *         false 不能更新到新工作状态（currentStatusNum!=0）
 */
bool Status::setWorkingStatus(WorkingStatus newStatus)
{
    if (newStatus != Collecting && newStatus != Calculating_Backuping)
        return false;

    workingStatus = newStatus;
    return true;
}

// 从机进入工作状态
// 有如下工作状态：Preparing（网络连接），Collecting（采集），Backuping（备份），Calculating（计算）、、
void Status::slaveIntoStatus(WorkingStatus newStatus, int slaveid)
{
    bool ret = false;
    getSlaveWorkingStatus(newStatus, slaveid, ret);
    if (ret)
    {
        getSlaveWorkingStatus(newStatus, slaveid, ret) = true;
        getSlaveNumWorkingStatus(newStatus, ret)++;
    }
}

// 从机离开当前工作状态
// 有如下工作状态：Preparing（网络连接），Collecting（采集），Backuping（备份），Calculating（计算）、、
void Status::slaveOutfromStatus(WorkingStatus newStatus, int slaveid)
{
    bool ret = false;
    getSlaveWorkingStatus(newStatus, slaveid, ret);
    if (ret)
    {
        getSlaveWorkingStatus(newStatus, slaveid, ret) = false;
        getSlaveNumWorkingStatus(newStatus, ret)--;
    }
}

/**
 * 得到当前状态下的从机数量
 */
int Status::getStatusSlaveNum(WorkingStatus newStatus)
{
    bool ret = false;
    int retint = getSlaveNumWorkingStatus(newStatus, ret);
    if (!ret)
        return 0;
    else
        return retint;
}

/**
 * 初始化采集，主机的PlanTaskList信息
 */
bool Status::initCollectStatus()
{
    if (workingStatus != Collecting)
        return false;

    ///TODO 。。。TODELETE
    /*if (getSlaveNumInCurrentWorkingStatus() < 9)
    {
        qDebug() << tr("从机数量小于9个进入采集模式，不能配置初始化采集！");
        return false;
    }*/

    slaveFrameCounterRecord.clear();
        
    for (int i = 0; i < 9; i++)
    {
        SlaveCollectFrameCounterRecord newslavefcrecord;
        newslavefcrecord.startfc_box1cameraref = 0;
        newslavefcrecord.startfc_box1camera = 0;
        newslavefcrecord.startfc_box2cameraref = 0;
        newslavefcrecord.startfc_box2camera = 0;
        newslavefcrecord.endfc_box1cameraref = 0;
        newslavefcrecord.endfc_box1camera = 0;
        newslavefcrecord.endfc_box2cameraref = 0;
        newslavefcrecord.endfc_box2camera = 0;
        slaveFrameCounterRecord.push_back(newslavefcrecord);
    }
    startfc_master = -1;
    endfc_master = -1;
    startmile_master = 0;
    endmile_master = 0;
    seq = -1;
    collecting_filenameprefix_master = "";

    collectList.clear();

    slave1RecordList.clear();
    slave2RecordList.clear();
    slave3RecordList.clear();
    slave4RecordList.clear();
    slave5RecordList.clear();
    slave6RecordList.clear();
    slave7RecordList.clear();
    slave8RecordList.clear();
    slave9RecordList.clear();

    // 一次性设置迭代器指针
    planTaskIterator =  LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Collect).begin();
    // 配置后seqno清零
    seqmutex.lock();
    this->seq = 1;
    seqmutex.unlock();
    QByteArray tmpba = planTaskIterator->tunnelname.c_str();
    collecting_filenameprefix_master = QObject::tr("%1_%2_%3").arg(1).arg(tmpba.constData()).arg(QDateTime::currentDateTime().toString("yyyyMMdd"));
    return true;
}

/**
 * 初始化计算备份状态
 * 加载已有文件的数据到各个结构中
 */
bool Status::initCalculateBackupStatus() 
{
    //TODO   TOADD
    return true;
}

/**
 * 指定九个文件位置的读取未完成工作（计算备份断点）
 * uncompleted_slave_1.txt   ...   uncompleted_slave_9.txt
 */
/*bool Status::saveUncompletedToFiles()
{
    const QList<SlaveModel> * slavelist = NetworkConfigList::getNetworkConfigListInstance()->listsnid();
    if (slavelist->length() != MasterSetting::getMaxSalveNum())
    {
        qDebug() << tr("从机数量不够，无法添加任务");
        return false;
    }
    int slaveid;
    QString projectname = MasterSetting::getSettingInstance()->getProjectFilename();
    QString fileprefix = MasterSetting::getSettingInstance()->getProjectPath() + "/slave_undo/" + projectname.left(projectname.length() - 5);
    for (int i = 0; i < slavelist->length(); i++)
    {
        slaveid = slavelist->at(i).getIndex();
        QString filename = QString("%1_slave%2_undo.txt").arg(fileprefix).arg(slaveid);
        QFile file(filename);
        if(!file.open(QIODevice::WriteOnly))
        {
            qDebug() << tr("打不开文件%1，无法创建").arg(filename);
            emit errortogui(tr("打不开文件%1，无法创建").arg(filename));
            continue;
        }
        QTextStream out(&file);
        bool ret = false;
        list<Uncompleted> & tmplist = getUncompletedList(slaveid, ret);
        list<Uncompleted>::iterator tmpit = tmplist.begin();
        while (ret && tmpit != tmplist.end())
        {
            out << tmpit->status << "," << tmpit->filetype << "," << tmpit->seqno << "," << tmpit->index.c_str() << "," << tmpit->frameStart << "\n";
            tmpit++;
        }
        file.close();
    }
    return true;
}*/

/*bool Status::loadUncompletedFromFiles()
{
    // 清空原有list 【慎重！！】
    if (slave1UncompletedList.size() > 0 || slave2UncompletedList.size() > 0 || slave3UncompletedList.size() > 0
            || slave4UncompletedList.size() > 0 || slave5UncompletedList.size() > 0 || slave6UncompletedList.size() > 0
            || slave7UncompletedList.size() > 0 || slave8UncompletedList.size() > 0 || slave9UncompletedList.size() > 0)
    {
        qDebug() << tr("不能重新加载，原来的uncompletedlist中有数据，请先保存");
        return false;
    }

    const QList<SlaveModel> * slavelist = NetworkConfigList::getNetworkConfigListInstance()->listsnid();
    if (slavelist->length() != MasterSetting::getMaxSalveNum())
    {
        qDebug() << tr("从机模型list数量不够，无法添加任务");
        return false;
    }
    int slaveid;
    QString projectname = MasterSetting::getSettingInstance()->getProjectFilename();
    QString fileprefix = MasterSetting::getSettingInstance()->getProjectPath() + "/slave_undo/" + projectname.left(projectname.length() - 5);
    for (int i = 0; i < slavelist->length(); i++)
    {
        slaveid = slavelist->at(i).getIndex();
        QString filename = QString("%1_slave%2_undo.txt").arg(fileprefix).arg(slaveid);
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly))
        {
            qDebug() << tr("打不开文件%1").arg(filename);
            emit errortogui(tr("打不开文件%1").arg(filename));
            continue;
        }
        QTextStream in(&file);
        QString line = in.readLine();
        bool ret = false;
        list<Uncompleted> & tmplist = getUncompletedList(slaveid, ret);
        while (ret && line.compare("") != 0)
        {
            QStringList strList = line.split(",", QString::SkipEmptyParts);
            if (strList.length() < 5)
            {
                qDebug() << tr("解析字符出错") << line;
                line = in.readLine();
                continue;
            }
            Uncompleted tmp;
            tmp.status = (WorkingStatus) strList.value(0).toInt();
            tmp.filetype = strList.value(1).toInt();
            tmp.seqno = strList.value(2).toInt();
            tmp.index = strList.value(3).toStdString(); // 是否出错？
            tmp.frameStart = strList.value(4).toLongLong();
            tmplist.push_back(tmp);
            line = in.readLine();
        }
        file.close();
    }
    return true;
}*/

/**
 * 增加采集记录（主控端）
 * 此函数由主控主动调用
 */
bool Status::addToCollectList_master(_int64 start_framecounter, _int64 end_framecounter, float start_mile, float end_mile)
{
    int seqno;
    seqmutex.lock();
    seqno = this->seq;
    seqmutex.unlock();

    // 要把主控信号发送端的起始帧号终止帧号，起始里程，终止里程写入，在给各从机发消息前外部调用插入
    QByteArray tmpba = planTaskIterator->tunnelname.c_str();
    QString currenttime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    CollectRecord a;
    
    a.seqno = seqno;
    //a.seqno = this->tempseqno; // 初始为1，后边新的隧道++
    a.corrected_tunnelid = planTaskIterator->tunnelnum;
    a.record_tunnelid = planTaskIterator->tunnelnum;
    a.record_tunnelname = tmpba.constData();
    tmpba = currenttime.toLocal8Bit();
    a.record_time = tmpba.constData();
    //a.record_remark = remark;
    a.returnnum = 0; // 第一次插入说明已经接受了一次
    a.isvalid = false;
    //a.start_framecounter = 0;
    //a.end_framecounter = 0;
    a.start_framecounter = start_framecounter;
    a.end_framecounter = end_framecounter;
    a.start_mile = start_mile;
    a.end_mile = end_mile;

    collectListmutex.lock();
    collectList.push_back(a);
    collectListmutex.unlock();

    //qDebug() << "pushback to collectList" << slaveid;

    // 更新seq和planTaskIterator
    seqmutex.lock();
    seqno = ++this->seq;
    planTaskIterator++;
    seqmutex.unlock();
    if (planTaskIterator == LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Collect).end() && planTaskIterator !=  LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Collect).begin())
        planTaskIterator--;
    
    tmpba = planTaskIterator->tunnelname.c_str();
    collecting_filenameprefix_master = QObject::tr("%1_%2_%3").arg(seqno).arg(tmpba.constData()).arg(QDateTime::currentDateTime().toString("yyyyMMdd"));

    return true;
}

/**
 * 增加采集记录(或更新9台采集机的returnnum值，最大为Setting::SLAVE_MAX)
 * 内部对list增加或更新操作，
 * 若tunnelid最新，则增加list元素；
 * 若已有tunnnelid，则更新数据，一般在多从控返回时，vector中的对应元素的returnnum++
 * @param slaveid 从机号
 * @param tunnelid 隧道号
 * @param tunnelname 隧道名称
 * @param seqno 采集序列号
 * @param cameraindex 相机索引号A1。。。
 * //@param remark 隧道号备注号（根据 @parma tunnelid 和 remark 作为标识确定从控中对应的文件）
 * @return true 操作成功，false失败
 */
bool Status::addToCollectList(int slaveid, int tunnelid, QString tunnelname, int seqno, QString cameraindex,
                                        _int64 nstart_framecounter, _int64 nend_framecounter)
{
    const QList<SlaveModel> * slavelist = NetworkConfigList::getNetworkConfigListInstance()->listsnid();
    if (slavelist->length() != MasterSetting::getMaxSalveNum())
    {
        qDebug() << tr("从机数量不够，无法添加任务");
        return false;
    }
    for (int i = 0; i < slavelist->length(); i++)
    {
        if (slaveid == slavelist->at(i).getIndex())
        {
            SlaveModel slaveModel = slavelist->at(i);

            bool ret;
            list<SlaveCollectRecord> & tmplist = this->getSlaveRecordList(slaveid, ret);
            if (ret)
            {
                list<SlaveCollectRecord>::reverse_iterator rit = tmplist.rbegin();
                // 新的seqno
                if (rit == tmplist.rend() || (*rit).seqno < seqno)
                {
                    SlaveCollectRecord s;
                    // 默认一台slave机要返回4次，插入一回，其他三回只更新start_framecounter，end_framecounter值
                    s.index_camera1 = slaveModel.box1.camera_ref;
                    s.index_camera2 = slaveModel.box1.camera;
                    s.index_camera3 = slaveModel.box2.camera_ref;
                    s.index_camera4 = slaveModel.box2.camera;
                    s.backup_camera1 = false;
                    s.backup_camera2 = false;
                    s.backup_camera3 = false;
                    s.backup_camera4 = false;
                    s.seqno = seqno;
                    s.start_framecounter_camera1 = -1;
                    s.end_framecounter_camera1 = -1;
                    s.start_framecounter_camera2 = -1;
                    s.end_framecounter_camera2 = -1;
                    s.start_framecounter_camera3 = -1;
                    s.end_framecounter_camera3 = -1;
                    s.start_framecounter_camera4 = -1;
                    s.end_framecounter_camera4 = -1;
                    tmplist.push_back(s);
                }

                // 记录seqno值，此时rbegin可能值比上一个if 的rbegin多1
                SlaveCollectRecord & tmp = (*tmplist.rbegin());
                if (tmp.seqno == seqno)
                {
                    if (tmp.index_camera1.compare(cameraindex.toAscii().constData()) == 0)
                    {
                        tmp.start_framecounter_camera1 = nstart_framecounter;
                        tmp.end_framecounter_camera1 = nend_framecounter;
                    }
                    else if (tmp.index_camera2.compare(cameraindex.toAscii().constData()) == 0)
                    {
                        tmp.start_framecounter_camera2 = nstart_framecounter;
                        tmp.end_framecounter_camera2 = nend_framecounter;
                    }
                    else if (tmp.index_camera3.compare(cameraindex.toAscii().constData()) == 0)
                    {
                        tmp.start_framecounter_camera3 = nstart_framecounter;
                        tmp.end_framecounter_camera3 = nend_framecounter;
                    }
                    else if (tmp.index_camera4.compare(cameraindex.toAscii().constData()) == 0)
                    {
                        tmp.start_framecounter_camera4 = nstart_framecounter;
                        tmp.end_framecounter_camera4 = nend_framecounter;
                    }
                }

                // qDebug() << "pushback to slaveRecordList" << slaveid;
            }
            break;
        }
    }

    list<CollectRecord>::iterator it = findCollectListIterator(tunnelid, seqno);
    if (it != collectList.end()) // 更新，returnnum++
    {
        if (((*it).returnnum) > MasterSetting::getSettingInstance()->getMaxSalveNum())
        {
            //qDebug() << tr("已达到最大数量SlALE_MAX:%1,请检查").arg(MasterSetting::getSettingInstance()->getMaxSalveNum());
            return false;
        }
        (*it).returnnum++;
        //qDebug() << "tunnelid " << (*it).record_tunnelid << ", remark " << (*it).record_remark << ", returnnum " << (*it).returnnum;
    }
 
    return false;
}

/**
 * 【注意！】 记录计算状态时必须
 * 增加到各自slaveid的断点记录list中
 */
/*bool Status::addTaskTunnelToAllCalculatedList()
{
    if (workingStatus == WorkingStatus::Collecting)
    {
        qDebug() << tr("正在采集无法，无法添加计算任务");
        return false;
    }

    const QList<SlaveModel> * slavelist = NetworkConfigList::getNetworkConfigListInstance()->listsnid();
    if (slavelist->length() != MasterSetting::getMaxSalveNum())
    {
        qDebug() << tr("从机数量不够，无法添加任务");
        return false;
    }

    for (int i = 0; i < slavelist->length(); i++)
    {
        const QList<PlanTask>* tasklist = PlanTaskList::getPlanTaskListInstance()->list();
        for (int j = 0; j < tasklist->length(); j++)
        {
            SlaveModel slaveModel = slavelist->at(i);
            SlaveCalculateRecord s;

            s.corrected_tunnelid = tasklist->at(j).tunnelnum;
            s.group1index = slaveModel.box1.boxindex;
            s.group2index = slaveModel.box2.boxindex;
            s.group1status = 0;
            s.group2status = 0;
            bool ret = false;
            list<SlaveCalculateRecord> tmplist = this->getSlaveCalcuList(i, ret);
            if (ret)
                tmplist.push_back(s);
        }
    }
    return true;
}*/

/**
 * 增加到各自slaveid的断点记录list中
 */
/*bool Status::addToUncompletedList(QString filename, int newslaveid, int type, int newseqno, WorkingStatus newstatus, string newindex, _int64 newframestart)
{
    Uncompleted u;
    // 未完成状态记录
    u.seqno = newseqno;
    u.status = newstatus;
    u.index = newindex;// A1、A2等为备份\A、B等为计算
    u.filetype = type;
    u.frameStart = newframestart;
    bool ret = false;
    list<Uncompleted> & tmplist = this->getUncompletedList(newslaveid, ret);
    if (ret)
        tmplist.push_back(u);
    return true;
}*/

/**
 * 更新计算状态
 * 【前提】计算状态记录链表已经初始化了
 */
/*bool Status::recordCalculate(QString filename, int slaveid, int tunnelid, string groupindex)
{
    bool ret = false;
    list<SlaveCalculateRecord> list1 = this->getSlaveCalcuList(slaveid, ret);

    if (list1.size() == 0 || !ret)
    {
        qDebug() << tr("计划任务还未生成计算状态队列") << slaveid;
        return false;
    }

    list<SlaveCalculateRecord>::iterator it = list1.begin();
    while (it != list1.end())
    {
        if (tunnelid == (*it).corrected_tunnelid)
        {
            if (groupindex.compare((*it).group1index) == 0)
                (*it).group1status++;
            else if (groupindex.compare((*it).group2index) == 0)
                (*it).group2status++;
            return true;
        }
        it++;
    }
    return false;
}*/

/**
 * 更新备份状态
 * 【注意！】在函数内部根据index的长度判断是备份的A、B、A1、A2...哪个文件（计算中间or实际采集）
 * @param filetype 1是原始图像数据，2是中间计算结果数据
 * @param seqno  当计算中间文件根据tunnelid标识
 *                 实际采集文件根据seqno标识
 */
/*bool Status::recordBackup(QString filename, int slaveid, int filetype, int seqno, string index)
{
    if (filetype == 1) // 原始数据进行备份
    {
        // 需要从list<SlaveCollectRecord>里边找
        bool ret;
        list<SlaveCollectRecord> list1 = this->getSlaveRecordList(slaveid, ret);
        if (list1.size() == 0 || !ret)
        {
            qDebug() << tr("原始数据文件的备份状态队列不存在") << slaveid;
            return false;
        }

        list<SlaveCollectRecord>::iterator it = list1.begin();
        while (it != list1.end())
        {
            if (seqno == (*it).seqno)
            {
                if (index.compare((*it).index_camera1) == 0)
                    (*it).backup_camera1 = true;
                else if (index.compare((*it).index_camera2) == 0)
                    (*it).backup_camera2 = true;
                else if (index.compare((*it).index_camera3) == 0)
                    (*it).backup_camera3 = true;
                else if (index.compare((*it).index_camera4) == 0)
                    (*it).backup_camera4 = true;
                return true;
            }
            it++;
        }
    }
    else if (filetype == 2) // 说明是相机组号，计算中间文件及结果文件的备份
    {
        int tunnelid = seqno;
        // 需要从list<SlaveCollectRecord>里边找
        bool ret = false;
        list<SlaveCalculateRecord> list2 = this->getSlaveCalcuList(slaveid, ret);
        if (list2.size() == 0 || !ret)
        {
            qDebug() << tr("计算文件的备份状态队列不存在") << slaveid;
            return false;
        }
        list<SlaveCalculateRecord>::iterator it2 = list2.begin();
        while (it2 != list2.end())
        {
            if (tunnelid == (*it2).corrected_tunnelid)
            {
                if (index.compare((*it2).group1index))
                    (*it2).group1status++;
                else if (index.compare((*it2).group2index))
                    (*it2).group2status++;
                return true;
            }
            it2++;
        }
    }
    else
        qDebug() << tr("index不能识别是相机或者是相机组");
    return false;
}*/

/**
 * 删除uncompletedList中的头元素，并返回，pop
 * @param slaveid 输入从机索引号
 * @param outputUncompleted 输出从机slaveid的未完成项
 * @return true 找到并成功pop出
 */
/*bool Status::popFromUncompletedList(int slaveid, Uncompleted & outputUncompleted)
{
    bool ret = false;
    list<Uncompleted> & tmplist = this->getUncompletedList(slaveid, ret);
    if (!ret)
        return false;
    list<Uncompleted>::iterator it = tmplist.begin();

    if (it != tmplist.end())
    {
        outputUncompleted.seqno = (*it).seqno;
        outputUncompleted.frameStart = (*it).frameStart;
        outputUncompleted.status = (*it).status;
        outputUncompleted.index = (*it).index;
        return true;
    }
    return false;
}*/
