#include "lz_fusecalc_queue.h"

/**
 * 从控机计算队列类实现
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
LzFuseCalcQueue::LzFuseCalcQueue(QObject * parent) : LzMsgQueue(parent)
{
    lzcalc = NULL;
    hasinit = false;
}

LzFuseCalcQueue::~LzFuseCalcQueue()
{
    if (lzcalc != NULL)
        delete lzcalc;
}

/**
 * 暂停
 */
void LzFuseCalcQueue::suspend()
{
    LzMsgQueue::suspend();
    if (lzcalc != NULL)
        lzcalc->suspendAllThreads();
}

bool LzFuseCalcQueue::initCalc()
{
    if (lzcalc != NULL)
        delete lzcalc;
    
    // 单线程做融合计算
    lzcalc = new LzSlaveCalculate(LzCalcThreadType::LzCalcThread_Fuse, 1);

    QObject::connect(lzcalc, SIGNAL(myStart(int, bool, int, QString, qint64, qint64)), this, SLOT(receiveThreadStart(int, bool, int, QString, qint64, qint64)));
    QObject::connect(lzcalc, SIGNAL(finish(int, int, int, QString, qint64)), this, SLOT(receiveThreadFinish(int, int, int, QString, qint64)));
    QObject::connect(lzcalc, SIGNAL(statusShow(int, qint64, int, QString)), this, SLOT(receiveStatusShow(int, qint64, int, QString)));

    hasinit = true;
    
    return hasinit;
}

/**
 * 队列完成后执行
 */
void LzFuseCalcQueue::finish() 
{
    calculate_finishAll();
}

/**
 * 检查是否有空闲线程可以执行下一任务
 */
bool LzFuseCalcQueue::checkHasFreeThread()
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
bool LzFuseCalcQueue::parseMsg(QString msg)
{
    QStringList strList = msg.split(",", QString::SkipEmptyParts);
 
    int msgid = strList.value(0).toInt();
    qDebug() << "msgid= " << msgid;

    if (msgid < 0)
    {
        switch (msgid)
        {
            case -30: // 开始融合计算-30,"tunnelid","filename","hascalcu","calcuinterruptfc","QRcalibfile","rectifyheightfile"
            {
                if (strList.length() < 8)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return false;
                }
                int tunnelid = strList.at(1).toInt();
                QString filename = strList.at(2);
                int hascalcubackup = strList.at(3).toInt();
                qint64 calinterruptfc = strList.at(4).toLongLong();
                QString parentpath = strList.at(5);
                int userectifyfactorint = strList.at(6).toInt();
                int usesafetyfactorint = strList.at(7).toInt();
                QString QRcalibfile = strList.at(8);
                QString rectifyheightfile = strList.at(9);            
    
                qDebug() << QRcalibfile << rectifyheightfile;

                bool hasinterrupted = false;
                if (hascalcubackup == 1)
                    hasinterrupted = true;
                bool userectifyfactor = false;
                if (userectifyfactorint == 1)
                    userectifyfactor = true;
                bool usesafetyfactor = false;
                if (usesafetyfactorint == 1)
                    usesafetyfactor = true;
                if (checkHasFreeThread())
                    lzcalc->startFuseCalc(tunnelid, filename, hasinterrupted, calinterruptfc, parentpath, QRcalibfile, rectifyheightfile, userectifyfactor, usesafetyfactor);
                else
                    return false;

                break;
            }
            case -31: // 暂停计算
            {
                /************TODO*************/
                suspend();
                emit signalParsedMsgToSlave(tr("[主控] 命令:暂停融合计算"));
                break;
            }
            case -32: // 开始提高度计算-32,"tunnelid","filename","hascalcu","calcuinterruptfc","parentpath"
            {
                if (strList.length() < 8)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return false;
                }
                int tunnelid = strList.at(1).toInt();
                QString filename = strList.at(2);
                int hascalcubackup = strList.at(3).toInt();
                qint64 calinterruptfc = strList.at(4).toLongLong();
                QString parentpath = strList.at(5);
                int userectifyfactorint = strList.at(6).toInt();
                int usesafetyfactorint = strList.at(7).toInt();
                QString QRcalibfile = strList.at(8);
                QString rectifyheightfile = strList.at(9);    

                qDebug() << QRcalibfile << rectifyheightfile;

                bool hasinterrupted = false;
                if (hascalcubackup == 1)
                    hasinterrupted = true;
                bool userectifyfactor = false;
                if (userectifyfactorint == 1)
                    userectifyfactor = true;
                bool usesafetyfactor = false;
                if (usesafetyfactorint == 1)
                    usesafetyfactor = true;

                if (checkHasFreeThread())
                    lzcalc->startExtractHeightCalc(tunnelid, filename, hasinterrupted, calinterruptfc, parentpath, QRcalibfile, rectifyheightfile, userectifyfactor, usesafetyfactor);
                else
                    return false;

                break;
            }
            case -33: // 暂停计算
            {
                /************TODO*************/
                suspend();
                emit signalParsedMsgToSlave(tr("[主控] 命令:暂停提高度计算"));
                break;
            }

            default: return false;
        }
    }

    return true;
}

/**
 * 计算完成
 */
void LzFuseCalcQueue::calculate_finishAll()
{
    emit signalParsedMsgToSlave(QString("[主控] 全部计算任务完成"));
}

/**
 * 主控融合计算任务开始的处理槽函数
 * @param threadid 线程编号（单线程时为1）
 */
void LzFuseCalcQueue::receiveThreadStart(int threadid, bool isok, int tunnelid, QString filename, qint64 beginfc, qint64 endfc)
{
    emit signalCalcuBackupTaskFC_init(WorkingStatus::Calculating, "0", threadid, filename, beginfc, endfc);
}

/**
 * 主控融合计算任务执行完的处理槽函数
 */
void LzFuseCalcQueue::receiveThreadFinish(int threadid, int isok, int tunnelid, QString filename, qint64 endfc)
{
    if (isok == 0)
    {
        emit calcubackupProgressingBar(WorkingStatus::Calculating, "0", threadid, 0, 0, true);
        // 单线程融合计算，threadid 为 1
        emit signalCalcuBakcupTask(WorkingStatus::Calculating, "0", threadid, filename, "ok");
        emit signalParsedMsgToSlave(QString("[主控] 融合计算%1，完成").arg(filename));
    }
    else if (isok == -1)
    {
        // 暂停计算
        emit signalCalcuBackupTaskFC(WorkingStatus::Calculating, "0", threadid, filename, endfc);
        emit signalCalcuBakcupTask(WorkingStatus::Calculating, "0", threadid, filename, QObject::tr("pause"));
        emit signalParsedMsgToSlave(QString("[主控] 融合计算%1到第%2帧，暂停").arg(filename).arg(endfc));
    }
    else
    {
        // 单线程融合计算，threadid 为 1
        emit signalCalcuBakcupTask(WorkingStatus::Calculating, "0", threadid, filename, QObject::tr("出错！"));
        emit signalParsedMsgToSlave(QString("[主控] 融合计算%1，出错%2").arg(filename).arg(isok));
    }
    endMsg();
}

void LzFuseCalcQueue::receiveStatusShow(int threadid, qint64 tmpfc, int tunnelid, QString filename)
{
    // 告知界面 
    // 隔20帧反馈一次告知主控（防止消息过多）
    if (tmpfc != 0 && tmpfc % 20 == 0) 
    {
        emit signalCalcuBackupTaskFC(WorkingStatus::Calculating, "0", threadid, filename, tmpfc);
    }
}