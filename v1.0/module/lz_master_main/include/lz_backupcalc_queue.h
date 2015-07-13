#ifndef LZ_BACKUPCALC_QUEUE_H
#define LZ_BACKUPCALC_QUEUE_H

#include <QList>
#include <QDebug>

#include "lz_msg_queue.h"
#include "lz_working_enum.h"
#include "fileoperation.h"

/**
 * 从控机备份队列类设计
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
class LzBackupCalcQueue : public LzMsgQueue
{
    Q_OBJECT

public:
    /**
     * 构造函数
     */
    LzBackupCalcQueue(QObject * parent = 0);

    virtual ~LzBackupCalcQueue();

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
    /**
     * 开始备份
     */
    int backup_start(QString filename);
    /**
     * 备份完成
     */
    void backup_finishAll();

signals:

    /**
     * 告知上层当前任务名
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param task 任务名
     * @param remark 任务名对应任务备注（出错信息等）
     */
    void signalCalcuBakcupTask(WorkingStatus workingstatus, QString id, int threadid, QString task, QString status);

    // 计算备份进度条
    /**
     * 告知上层更新计算备份进度条（计算进度分不同的线程号）
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id
     * @param totalhasbackupnum 总任务数 
     * @param tmphasbackupnum 当前完成任务数 
     */
    void calcubackupProgressingBar(WorkingStatus status, QString slavid, int threadid, int totalhasbackupnum, int tmphasbackupnum, bool);

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
    void receiveThreadFinish(int isok, QString filename, qint64 suspendfilepos);
    
    /**
     * 接收线程运行过程中消息
     * @param filename 文件名
     * @param currentfilepos 当前文件正在备份中的文件位置
     */
    void receiveStatusShow(qint64, QString);

private:
    /**
     * 备份线程
     */
    LzBackupThread * thread;

    // 当前任务源文件路径
    QString current_sourcefile_dir;
    // 当前任务目标文件路径
    QString current_directionfile_dir;
};

#endif // LZ_BACKUPCALC_QUEUE_H
