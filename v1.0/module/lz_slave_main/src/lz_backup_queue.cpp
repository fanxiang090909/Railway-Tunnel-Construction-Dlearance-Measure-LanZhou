#include "lz_backup_queue.h"

#include "setting_slave.h"

#include <QFile>

/**
 * 从控机备份队列类实现
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
LzBackupQueue::LzBackupQueue(QObject * parent) : LzSlaveMsgQueue(parent)
{
    thread = NULL;
    current_tunnelid = -1;
}

LzBackupQueue::~LzBackupQueue()
{
    if (thread != NULL)
    {
        delete thread;
        thread = NULL;
    }
}

/**
 * 队列完成后执行
 */
void LzBackupQueue::finish() 
{
    // 因为文件太小总是发送这个
    //backup_finishAll();
}

/**
 * 任务命令解析
 * 实现父类的纯虚函数
 */
bool LzBackupQueue::parseMsg(QString msg)
{
    QStringList strList = msg.split(",", QString::SkipEmptyParts);
 
    int msgid = strList.value(0).toInt();
    qDebug() << "msgid= " << msgid;

    if (msgid > 3000)
    {
        switch(msgid)
        {
            case 3001: // 3001,filename=xxx,tunnelid=xx,isinterrupted=false(true),interruptedfilepos=xx,beginfc=xx,endfc=xx
            {
                if (strList.length() < 7 || !strList.at(1).startsWith("filename") || !strList.at(2).startsWith("tunnelid") || !strList.at(3).startsWith("isinterrupted")
                                         || !strList.at(4).startsWith("interruptedfilepos") || !strList.at(5).startsWith("beginfc") || !strList.at(6).startsWith("endfc"))
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return false;
                }
                QString filename = strList.at(1).mid(9);
                int tunnelid = strList.at(2).mid(9).toInt();
                current_tunnelid = tunnelid;
                QString isinterruptstr = strList.at(3).mid(14);
                bool isinterrupt = false;
                if (isinterruptstr.compare("true") == 0)
                    isinterrupt = true;
                qint64 interruptfilepos = strList.at(4).mid(19).toLongLong();
                qint64 beginfc = strList.at(5).mid(8).toLongLong();
                qint64 endfc = strList.at(6).mid(6).toLongLong();
                if (isinterrupt)
                    emit signalParsedMsgToSlave(tr("[主控] 命令:开始备份文件为%1，文件起始帧号%2，终止帧号%3，从中断位置%4帧开始").arg(filename).arg(beginfc).arg(endfc).arg(interruptfilepos));
                else
                    emit signalParsedMsgToSlave(tr("[主控] 命令:开始备份文件为%1，文件起始帧号%2，终止帧号%3，从头开始").arg(filename).arg(beginfc).arg(endfc));

                // 确认开始备份
                backup_feedbackStartToMaster(filename, tunnelid, beginfc, endfc);
                // 备份该隧道
                int ret = backup_start(filename, tunnelid, isinterrupt, interruptfilepos);
                if (ret != 0)
                    endMsg();

                break;
            }
            case 3401://暂停备份
            {
                qDebug() << QObject::tr("[主控] 命令:暂停备份");
                /************TODO*************/
                backup_suspend();

                emit signalParsedMsgToSlave(tr("[主控] 命令:暂停备份"));
                break;
            }
            default: return false;
        }
    }

    return true;
}

/**
 * 开始备份
 */
int LzBackupQueue::backup_start(QString filename, int tunnelid, bool isinterrupted, qint64 interruptedfilepos)
{
    if (!SlaveSetting::getSettingInstance()->getHasNasAccessIP())
        return -1;

    QString nasserverip = SlaveSetting::getSettingInstance()->getNASAccessIP();
    if (nasserverip.compare("") == 0)
    {
        backup_handleError(filename, tunnelid, QObject::tr("NAS设备IP地址%1未配置!").arg(nasserverip));
        return -2;
    }
    QString projectfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Backup);
    // 任务源文件路径
    current_sourcefile_dir = SlaveSetting::getSettingInstance()->getParentPath() + "\\" + projectfilename.left(projectfilename.size() - 5) + "\\collect\\";
    QString sendfile = current_sourcefile_dir + filename;
    // 加入目标NAS的存储路径
    current_directionfile_dir = QString("\\\\%1\\LanZhou\\").arg(nasserverip) +  "\\" + projectfilename.left(projectfilename.size() - 5) + "\\collect\\";
    QString todir = current_directionfile_dir + filename;

    // 开启备份线程
    if (thread != NULL)
    {
        delete thread;
        thread = NULL;
    }
    thread = new LzSerialStorageBackupThread();
    connect(thread, SIGNAL(finish(int, QString, qint64)), this, SLOT(receiveThreadFinish(int, QString, qint64)));
    connect(thread, SIGNAL(statusShow(qint64, QString)), this, SLOT(receiveStatusShow(qint64, QString)));

    if (isinterrupted)
        thread->initCopy(sendfile, todir, false, isinterrupted, interruptedfilepos);
    else
        thread->initCopy(sendfile, todir, true, isinterrupted, interruptedfilepos);

    thread->start();
    
    return 0;
}

/**
 * 暂停备份
 */
bool LzBackupQueue::backup_suspend()
{
    return thread->suspendThread();
}

/**
 * 确认开始备份
 */
void LzBackupQueue::backup_feedbackStartToMaster(QString filename, int tunnelid, long long beginfc, long long endfc)
{
    emit signalMsgToMaster(QString("3002,filename=%1,tunnelid=%2,return=true,beginfc=%3,endfc=%4").arg(filename).arg(tunnelid).arg(beginfc).arg(endfc));
}

/**
 * 备份某一文件中途进度反馈
 */
void LzBackupQueue::backup_feedbackFCToMaster(QString filename, int tunnelid, long long currentfc)
{
    emit signalMsgToMaster(QString("3020,filename=%1,tunnelid=%2,currentfc=%3").arg(filename).arg(tunnelid).arg(currentfc));
}

/**
 * 备份进度反馈
 */
void LzBackupQueue::backup_feedbackEndToMaster(QString filename, int tunnelid)
{
    emit signalMsgToMaster(QString("3010,filename=%1,tunnelid=%2").arg(filename).arg(tunnelid));
    emit signalParsedMsgToSlave(QString("[从控] 备份文件%1完成").arg(filename));
}

/**
 * 备份异常反馈
 */
void LzBackupQueue::backup_handleError(QString filename, int tunnelid, QString errortype)
{
    emit signalMsgToMaster(QString("3200,filename=%1,tunnelid=%2,errortype=%3").arg(filename).arg(tunnelid).arg(errortype));
    emit signalParsedMsgToSlave(QString("[从控] 备份文件%1，出错%2").arg(filename).arg(errortype));
}

/**
 * 暂停备份
 * @paran ret true 确认暂停，false，尚不能暂停，已加入队列稍后暂停
 */
void LzBackupQueue::backup_feedbackSuspend(QString filename, int tunnelid, long long interruptfc, bool ret)
{
    QString retstr = "false";
    if (ret)
        retstr = "true";
    emit signalMsgToMaster(QString("3402,filename=%1,tunnelid=%2,interruptfc=%3,return=%4").arg(filename).arg(tunnelid).arg(interruptfc).arg(retstr));
    if (ret)
        emit signalParsedMsgToSlave(QString("[从控] 备份文件%1暂停成功，暂停帧号%2").arg(filename).arg(interruptfc));
}

/**
 * 备份完成
 * @author 范翔
 */
void LzBackupQueue::backup_finishAll()
{
    emit signalMsgToMaster(QString("3100"));
}

void LzBackupQueue::receiveThreadFinish(int isok, QString filename, qint64 endfc)
{
    qDebug() << "NAS backup_start file " << filename << isok;

    if (isok <= 0)
    {
        switch (isok)
        {
            case 0:  // 反馈、备份结果
                    backup_feedbackEndToMaster(filename, current_tunnelid); 
                    break;
            case -1: // 暂停
                    backup_feedbackSuspend(filename, current_tunnelid, endfc, true);
                    break;
            case -2:backup_handleError(filename, current_tunnelid, QObject::tr("原文件路径与目标路径相同")); break;
            case -3:backup_handleError(filename, current_tunnelid, QObject::tr("原路径中%1不存在任务文件").arg(current_sourcefile_dir)); break;
            case -4:backup_handleError(filename, current_tunnelid, QObject::tr("不能在目标路径%1中创建文件").arg(current_directionfile_dir)); break;
            case -5:backup_handleError(filename, current_tunnelid, QObject::tr("无法备份（连接不到NAS）")); break;
            case -6:backup_handleError(filename, current_tunnelid, QObject::tr("未初始化备份线程")); break;
            case -7:backup_handleError(filename, current_tunnelid, QObject::tr("原文件正被占用，无法打开")); break;
            case -8:backup_handleError(filename, current_tunnelid, QObject::tr("目标文件正被占用，无法打开")); break;
            case -9:backup_handleError(filename, current_tunnelid, QObject::tr("无法retrieve原文件的起始帧")); break;
            default: break;
        }
    }
    endMsg();
}

void LzBackupQueue::receiveStatusShow(qint64 tmpfc, QString filename)
{
    //emit statusShow(tmpfc, tunnelid, filename);
    // 告知界面 
    emit fcupdate(WorkingStatus::Backuping, 1, filename, tmpfc);
    // 隔5帧反馈一次告知主控（防止消息过多）
    if (tmpfc != 0 && tmpfc % 100 == 0 ) 
    {
        backup_feedbackFCToMaster(filename, current_tunnelid, tmpfc);
    }
}