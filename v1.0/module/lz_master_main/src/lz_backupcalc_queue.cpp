#include "lz_backupcalc_queue.h"

#include <QFile>

#include "setting_master.h"
#include "lz_project_access.h"

/**
 * 从控机备份队列类实现
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
LzBackupCalcQueue::LzBackupCalcQueue(QObject * parent) : LzMsgQueue(parent)
{
    thread = NULL;
}

LzBackupCalcQueue::~LzBackupCalcQueue()
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
void LzBackupCalcQueue::finish() 
{
    // 因为文件太小总是发送这个
    //backup_finishAll();
}

/**
 * 任务命令解析
 * 实现父类的纯虚函数
 */
bool LzBackupCalcQueue::parseMsg(QString msg)
{
    QStringList strList = msg.split(",", QString::SkipEmptyParts);
 
    int msgid = strList.value(0).toInt();
    qDebug() << "msgid= " << msgid;

    if (msgid < 0)
    {
        switch(msgid)
        {
            case -40: // -40,xxx,xx,xx
            {
                if (strList.length() < 4)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return false;
                }
                QString filename = strList.at(1);
                int hasbackup = strList.at(2).toInt();
                __int64 backupinterruptpos = strList.at(3).toLongLong();

                emit signalParsedMsgToSlave(tr("[主控] 命令:开始备份文件为%1").arg(filename));
                // 确认开始备份
                // 单线程备份，threadid 为 1
                emit signalCalcuBakcupTask(WorkingStatus::Backuping, "0", 1, filename, "");
                // 备份该隧道
                int ret = backup_start(filename);
                if (ret != 0)
                    endMsg();

                break;
            }
            case 3401://暂停备份
            {
                /************TODO*************/
                suspend();
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
int LzBackupCalcQueue::backup_start(QString filename)
{
    QString nasserverip = MasterSetting::getSettingInstance()->getNASAccessIP();
    if (nasserverip.compare("") == 0)
    {
        emit signalParsedMsgToSlave(QString("NAS设备IP地址%1未设置!").arg(nasserverip));
        return -2;
    }
    QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Backup);
    QString projectfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Backup);
    // 任务源文件路径
    current_sourcefile_dir = MasterSetting::getSettingInstance()->getParentPath() +  "\\" + projectfilename.left(projectfilename.size() - 5) + "\\";
    QString sendfile = current_sourcefile_dir + filename;
    // 加入目标NAS的存储路径
    current_directionfile_dir = QString("\\\\%1\\LanZhou\\").arg(nasserverip) +  "\\" + projectfilename.left(projectfilename.size() - 5) + "\\";
    QString todir = current_directionfile_dir + filename;

    // 开启备份线程
    if (thread != NULL)
    {
        delete thread;
        thread = NULL;
    }
    // 开启备份线程
    thread = new LzBackupThread();
    connect(thread, SIGNAL(finish(int, QString, qint64)), this, SLOT(receiveThreadFinish(int, QString, qint64)));
    connect(thread, SIGNAL(statusShow(qint64, QString)), this, SLOT(receiveStatusShow(qint64, QString)));

    // 主控文件拷贝不做中断恢复
    thread->initCopy(sendfile, todir, true, false, 0);
    
    thread->start();

    return 0;
}

void LzBackupCalcQueue::receiveThreadFinish(int isok, QString filename, qint64 suspendfilepos)
{
    qDebug() << "NAS backup_start file " << filename << isok << current_sourcefile_dir;

    if (isok <= 0)
    {
        QString errstr;
        switch (isok)
        {
            case 0: // 反馈、发送计算结果
                    emit calcubackupProgressingBar(WorkingStatus::Backuping, "0", 1, 0, 0, true);
                    // 单线程备份，threadid 为 1
                    emit signalCalcuBakcupTask(WorkingStatus::Backuping, "0", 1, filename, "ok");
                    emit signalParsedMsgToSlave(QString("[主控] 备份文件%1，完成").arg(filename));
                    break;
            case -1:// 暂停
                    // TODO TOADD
                    break;
            case -2:errstr = QObject::tr("原文件路径与目标路径相同"); break;
            case -3:errstr = QObject::tr("原路径中%1不存在任务文件").arg(current_sourcefile_dir); break;
            case -4:errstr = QObject::tr("不能在目标路径%1中创建文件").arg(current_directionfile_dir); break;
            case -5:errstr = QObject::tr("无法备份（连接不到NAS）"); break;
            case -6:errstr = QObject::tr("未初始化备份线程"); break;
            case -7:errstr = QObject::tr("原文件正被占用，无法打开"); break;
            case -8:errstr = QObject::tr("目标文件正被占用，无法打开"); break;
            case -9:errstr = QObject::tr("无法retrieve原文件的起始帧"); break;
            default: break;
        }
        if (isok < -1)
        {
            // 单线程备份，threadid 为 1
            emit signalCalcuBakcupTask(WorkingStatus::Backuping, "0", 1, filename, errstr);
            emit signalParsedMsgToSlave(QString("[主控] 备份文件%1，出错%2").arg(filename).arg(errstr));
        }
    }
    endMsg();
}

void LzBackupCalcQueue::receiveStatusShow(qint64, QString) {}