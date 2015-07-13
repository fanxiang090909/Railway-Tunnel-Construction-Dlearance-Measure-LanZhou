#ifndef LZ_BACKUP_QUEUE_H
#define LZ_BACKUP_QUEUE_H

#include <QList>
#include <QDebug>

#include "lz_working_enum.h"
#include "lz_slavemsg_queue.h"

#include "fileoperation.h"
#include "fileoperation_lzserial.h"

/**
 * 从控机备份队列类设计
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
class LzBackupQueue : public LzSlaveMsgQueue
{
    Q_OBJECT

public:
    /**
     * 构造函数
     */
    LzBackupQueue(QObject * parent = 0);

    virtual ~LzBackupQueue();

protected:
    /**
     * 队列完成后执行
     */
    virtual void finish();

private:

    /**
     * 任务命令解析
     * 实现父类的纯虚函数
     */
    virtual bool parseMsg(QString newmsg);
// TODO TOCHANTE to private
public:

    /*****************关于备份**************/
    // 备份操作
    /**
     * 开始备份
     */
    int backup_start(QString filename, int tunnelid, bool isinterrupted, qint64 interruptedfilepos);
    /**
     * 暂停备份
     */
    bool backup_suspend();

    // 备份反馈
    /**
     * 确认开始备份
     */
    void backup_feedbackStartToMaster(QString filename, int tunnelid, long long beginfc, long long endfc);
    /**
     * 备份某一文件中途进度反馈
     */
    void backup_feedbackFCToMaster(QString filename, int tunnelid, long long currentfc);
    /**
     * 备份完某一文件进度反馈
     */
    void backup_feedbackEndToMaster(QString filename, int tunnelid);
    /**
     * 备份异常反馈
     * @author 范翔
     */
    void backup_handleError(QString filename, int tunnelid, QString errortype);
    /**
     * 暂停备份反馈
     * @paran ret true 确认暂停，false，尚不能暂停，已加入队列稍后暂停
     */
    void backup_feedbackSuspend(QString filename, int tunnelid, long long interruptfc, bool ret);
    /**
     * 备份完成
     */
    void backup_finishAll();

private slots:

    /**
     * 接收线程运行结束消息
     * @param isok 0 正常备份结束，反馈结果
                  -1 暂停结束
                  -2 "source file same with to file";
                  -3 "no source file"
                  -4 "cannot create to destination dir"
                  -5 "cannot copy: may not connect to nas!"
                  -6 "has not init backup thread!"
     * @param filename 文件名
     * @param suspendfilepos 文件暂停中断位置
     */
    void receiveThreadFinish(int isok, QString filename, qint64 endfc);

    /**
     * 接收线程运行过程中消息
     * @param filename 文件名
     * @param currentfilepos 当前文件正在备份中的文件位置
     */
    void receiveStatusShow(qint64 tmpfc, QString filename);

signals:

    /**
     * 告知界面信号
     * @param status WorkingStatus 
     * @param threadid 线程号
     * @param currentfc 当前帧号
     */
    void fcupdate(WorkingStatus, int threadid, QString filename, long long currentfc);

private:
    /**
     * 备份线程
     */
    LzSerialStorageBackupThread * thread;

    // 错误类型使用
    // 当前任务隧道ID号
    int current_tunnelid;
    // 当前任务源文件路径
    QString current_sourcefile_dir;
    // 当前任务目标文件路径
    QString current_directionfile_dir;
};

#endif // LZ_BACKUP_QUEUE_H
