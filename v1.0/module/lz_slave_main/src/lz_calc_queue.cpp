#include "lz_calc_queue.h"

#include "setting_slave.h"

/**
 * 从控机计算队列类实现
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
LzCalcQueue::LzCalcQueue(QObject * parent) : LzSlaveMsgQueue(parent)
{
    lzcalc = NULL;
    hasinit = false;
}

LzCalcQueue::~LzCalcQueue()
{
    if (lzcalc != NULL)
        delete lzcalc;
}

/**
 * 暂停
 */
void LzCalcQueue::suspend()
{
    LzSlaveMsgQueue::suspend();
    if (lzcalc != NULL)
        lzcalc->suspendAllThreads();
}

bool LzCalcQueue::initCalc()
{
    if (lzcalc != NULL)
        delete lzcalc;
    
    bool isrt = slaveModel.getIsRT();
    LzCalcThreadType threadtype = LzCalcThreadType::LzCalcThread_GeneralSlave;
    if (isrt)
        threadtype = LzCalcThreadType::LzCalcThread_RTSlave_V2;

    // 几个线程同时开始计算
    lzcalc = new LzSlaveCalculate(threadtype, 2);
    
    QObject::connect(lzcalc, SIGNAL(myStart(int, bool, int, QString, qint64, qint64)), this, SLOT(receiveThreadStart(int, bool, int, QString, qint64, qint64)));
    QObject::connect(lzcalc, SIGNAL(finish(int, int, int, QString, qint64)), this, SLOT(receiveThreadFinish(int, int, int, QString, qint64)));
    QObject::connect(lzcalc, SIGNAL(statusShow(int, qint64, int, QString)), this, SLOT(receiveStatusShow(int, qint64, int, QString)));

    hasinit = true;
    
    return hasinit;
}

/**
 * 队列完成后执行
 */
void LzCalcQueue::finish() 
{
    calculate_finishAll();
}

/**
 * 检查是否有空闲线程可以执行下一任务
 */
bool LzCalcQueue::checkHasFreeThread()
{
    if (hasinit)
    {
        if (lzcalc->getNumOfCurrentThreads() < lzcalc->getNumOfMaxThreads())
            return true;
    }
    return false;
}

/**
 * 任务命令解析
 * 实现父类的纯虚函数
 */
bool LzCalcQueue::parseMsg(QString msg)
{
    QStringList strList = msg.split(",", QString::SkipEmptyParts);
 
    int msgid = strList.value(0).toInt();
    qDebug() << "msgid= " << msgid;

    if (msgid > 2000)
    {
        switch (msgid)
        {
            case 2001: // 开始计算2001,filename=xxx,tunnelid=xx,cameragroup_index=xx,isinterrupted=false(true),interruptfc=xx
            {   
                if (strList.length() < 6)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return false;
                }
                QString filename = strList.at(1).mid(9);
                int tunnelid = strList.at(2).mid(9).toInt();
                QString cameraGroupIndex = strList.at(3).mid(18);
                QString isinterruptstr = strList.at(4).mid(14);
                bool isinterrupt = false;
                if (isinterruptstr.compare("true") == 0)
                    isinterrupt = true;
                qint64 interruptfc = strList.at(5).mid(12).toLongLong();
                if (!isinterrupt)
                    emit signalParsedMsgToSlave(tr("[主控] 命令:开始计算文件为%1的隧道%2的相机组号为%3的文件").arg(filename).arg(tunnelid).arg(cameraGroupIndex));
                else
                    emit signalParsedMsgToSlave(tr("[主控] 命令:开始计算文件为%1的隧道%2的相机组号为%3的文件，从上次暂停的%4帧开始").arg(filename).arg(tunnelid).arg(cameraGroupIndex).arg(interruptfc));
                {
                    // @author 范翔 20141202 改到LzCalc中，通过消息槽告知
                    // 确认开始计算
                    //calculate_feedbackStartToMaster(0, filename, tunnelid, cameraGroupIndex);
                    // 计算该隧道
                    calculate_start(filename, tunnelid, cameraGroupIndex, isinterrupt, interruptfc);
                    
                    // 如果不够2个线程
                    if (checkHasFreeThread())
                        endMsg();
                }
                break;
            }
            case 2401://暂停计算
            {
                /************TODO*************/

                emit signalParsedMsgToSlave(tr("[主控] 命令:暂停计算"));
                break;
            }
            default: return false;
        }
    }

    return true;
}

/**
 * 开始计算
 * @author 范翔
 */
void LzCalcQueue::calculate_start(QString filename, int tunnelid, QString cameraGroupIndex, bool isinterrupt, qint64 interruptfc)
{
    bool ret = false;
    if (hasinit)
    {
        // 如果未初始化从机模型
        ret = lzcalc->hasInitSlaveModel();
        if (!ret)
        {
            ret = lzcalc->initSlaveModel(slaveModel);
        }
        if (!ret)
        {
            calculate_handleError(0, filename, tunnelid, cameraGroupIndex, QObject::tr("can not init slave model"));
            return;
        }

        // 如果未设置计算任务--checkedtaskfile文件，设置并解析
        ret = lzcalc->hasInitCheckedTask();
        if (!ret)
        {
            QString projectfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Calculate);
            QString projectdirname = SlaveSetting::getSettingInstance()->getParentPath() + "/" + projectfilename.left(projectfilename.length() - 5);
            ProjectModel & pm = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Calculate);
            ret = lzcalc->initChekedTaskFile(projectdirname, pm.getCheckedFilename());
        }
        if (!ret)
        {
            calculate_handleError(0, filename, tunnelid, cameraGroupIndex, QObject::tr("can not init checked task file"));
            return;
        }

        // 开始计算
        ret = lzcalc->startNewTwoViewCalc(cameraGroupIndex, tunnelid, filename, isinterrupt, interruptfc);
        if (!ret)
        {
            calculate_handleError(0, filename, tunnelid, cameraGroupIndex, QObject::tr("can not start two view calcu thread"));
            return;
        }
    }
}

/**
 * 开始计算反馈
 */
void LzCalcQueue::calculate_feedbackStartToMaster(int threadid, QString filename, int tunnelid, QString cameraGroupIndex, qint64 beginfc, qint64 endfc)
{
    emit signalMsgToMaster(QString("2002,threadid=%1,filename=%2,tunnelid=%3,camera_groupindex=%4,return=true,beginfc=%5,endfc=%6").arg(threadid).arg(filename).arg(tunnelid).arg(cameraGroupIndex).arg(beginfc).arg(endfc));
}

/**
 * 计算进度反馈
 */
void LzCalcQueue::calculate_feedbackToMaster(int threadid, QString filename, int tunnelid, QString cameraGroupIndex)
{
    emit signalMsgToMaster(QString("2010,threadid=%1,filename=%2,tunnelid=%3,camera_groupindex=%4").arg(threadid).arg(filename).arg(tunnelid).arg(cameraGroupIndex));
    emit signalParsedMsgToSlave(QString("[从控] 双目计算相机组号%1，文件%2完成").arg(cameraGroupIndex).arg(filename));

    // 发送结果文件
    QString projectfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Calculate);
    QString tmpdir = SlaveSetting::getSettingInstance()->getParentPath() + "/" + projectfilename.left(projectfilename.length() - 5) + "/mid_calcu/";
    QString fullfilename = tmpdir + filename;
    qDebug() << "calculate_feedbackToMaster tosend filename" << fullfilename;

    if (filename.endsWith(".mdat"))
        emit signalFileToMaster(fullfilename);
    else  // @author 范翔 如果是RT计算，还要多发送两个文件
    {
        //filename = filename.left(filename.length() - 8);
        QString R_Pnts_out_file = tmpdir + filename + "_R.mdat";   //R相机地面三维点的输出文件
        emit signalFileToMaster(R_Pnts_out_file);
	    QString Q_Pnts_out_file = tmpdir + filename + "_Q.mdat";   //Q相机地面三维点的输出文件
        emit signalFileToMaster(Q_Pnts_out_file);
        QString QR_Pnts_out_file = tmpdir + filename + "_QR.mdat";   //Q相机地面三维点的输出文件
        emit signalFileToMaster(QR_Pnts_out_file);
        QString RT_Pnts_out_file = tmpdir + filename + "_RT.mdat";   //Q相机地面三维点的输出文件
        emit signalFileToMaster(RT_Pnts_out_file);
    }
}

/**
 * 计算异常反馈
 */
void LzCalcQueue::calculate_handleError(int threadid, QString filename, int tunnelid, QString cameraGroupIndex, QString errortype)
{
    // threadid 为0，不作处理
    emit signalMsgToMaster(QString("2200,threadid=%1,filename=%2,tunnelid=%3,camera_groupindex=%4,errortype=%5").arg(threadid).arg(filename).arg(tunnelid).arg(cameraGroupIndex).arg(errortype));
    emit signalParsedMsgToSlave(QString("[从控] 双目计算%1，出错%2").arg(filename).arg(errortype));
}

/**
 * 暂停计算
 */
void LzCalcQueue::calculate_feedbackToMaster_suspend(bool ret, int threadid, QString filename, int tunnelid, QString cameraGroupIndex, qint64 endfc)
{
    QString retstr = "false";
    if (ret)
        retstr = "true";
    emit signalMsgToMaster(QString("2402,threadid=%1,filename=%2,tunnelid=%3,camera_groupindex=%4,interruptfc=%5,return=%6").arg(threadid).arg(filename).arg(tunnelid).arg(cameraGroupIndex).arg(endfc).arg(retstr));
    if (ret)
        emit signalParsedMsgToSlave(QString("[从控] 双目计算相机组号%1，文件%2暂停成功，暂停帧号%3").arg(cameraGroupIndex).arg(filename).arg(endfc));
}

/**
 * 计算完成
 */
void LzCalcQueue::calculate_finishAll()
{
    emit signalMsgToMaster(QString("2100"));
}

/**
 * 从控任务开始的处理槽函数
 * @param threadid 线程编号（单线程时为1）
 */
void LzCalcQueue::receiveThreadStart(int threadid, bool isok, int tunnelid, QString filename, qint64 beginfc, qint64 endfc)
{
    QString cameragroupindex = filename.right(6).left(1);
    if (cameragroupindex.compare("T") == 0)
        cameragroupindex = "RT";
    calculate_feedbackStartToMaster(threadid, filename, tunnelid, cameragroupindex, beginfc, endfc);
}

/**
 * 从控任务执行完的处理槽函数
 */
void LzCalcQueue::receiveThreadFinish(int threadid, int isok, int tunnelid, QString filename, qint64 endfc)
{
    QString cameragroupindex = filename.right(6).left(1);
    if (!filename.endsWith(".mdat"))
        cameragroupindex = "RT";
    if (isok == 0)
    {
        calculate_feedbackToMaster(threadid, filename, tunnelid, cameragroupindex);
    }
    else if (isok == -1)
    {
        calculate_feedbackToMaster_suspend(true, threadid, filename, tunnelid, cameragroupindex, endfc);
    }
    else
    {
         /**
          * @param isok 1 找不到SlaveModel中对应的cameragroupindex
          *             2 得不到有效的CheckedTunnelTaskModel
          *             5 计算结果文件未打开（主要针对从原来暂停文件中恢复计算的情况）
          *             6 其他异常，（融合高度配置文件未加载）
          *             7 原始图像流式文件不存在
          *             8 原采集数据dat文件不能retrieve到起始帧
          */ 
        QString errorstr;
        switch (isok)
        {
            case 1: errorstr = QObject::tr("找不到SlaveModel中对应的cameragroupindex"); break;
            case 2: errorstr = QObject::tr("得不到有效的CheckedTunnelTaskModel"); break;
            case 5: errorstr = QObject::tr("计算结果文件未打开"); break;
            case 6: errorstr = QObject::tr("其他异常（融合高度配置文件未加载等）"); break;
            case 7: errorstr = QObject::tr("原采集数据dat文件不存在"); break;
            case 8: errorstr = QObject::tr("原采集数据dat文件不能retrieve到起始帧"); break;
            default:errorstr = QObject::tr("未知错误"); break;
        }
        calculate_handleError(threadid, filename, tunnelid, cameragroupindex, QString("%1").arg(errorstr));
    }
    endMsg();
}

/**
 * 向LzCalcuQueue转发信号
 * @param tmpfc 当前计算帧号
 */
void LzCalcQueue::receiveStatusShow(int threadid, qint64 tmpfc, int tunnelid, QString filename)
{
    //emit statusShow(tmpfc, tunnelid, filename);
    // 告知界面 
    emit fcupdate(WorkingStatus::Calculating, threadid, filename, tmpfc);
    // 隔20帧反馈一次告知主控（防止消息过多）
    if (tmpfc != 0 && tmpfc % 20 == 0 ) 
    {
        QString cameragroupindex = filename.right(6).left(1);
        if (!filename.endsWith(".mdat"))
            cameragroupindex = "RT";
        emit signalMsgToMaster(QString("2020,threadid=%1,filename=%2,tunnelid=%3,camera_groupindex=%4,currentfc=%5").arg(threadid).arg(filename).arg(tunnelid).arg(cameragroupindex).arg(tmpfc));
    }
}