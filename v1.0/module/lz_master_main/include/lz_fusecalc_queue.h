#ifndef LZ_FUSECALC_QUEUE_H
#define LZ_FUSECALC_QUEUE_H

#include <QList>
#include <QDebug>

#include "lz_msg_queue.h"
#include "lz_working_enum.h"
#include "LzCalc.h"

/**
 * 从控机计算队列类设计
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
class LzFuseCalcQueue : public LzMsgQueue
{
    Q_OBJECT

public:
    /**
     * 构造函数
     */
    LzFuseCalcQueue(QObject * parent = 0);

    virtual ~LzFuseCalcQueue();

    /**
     * 暂停
     */
    virtual void suspend();

protected:
    /**
     * 队列完成后执行
     */
    virtual void finish();

private:

    // 多线程计算类
    LzSlaveCalculate * lzcalc;
    bool hasinit;

    /**
     * 任务命令解析
     * 实现父类的纯虚函数
     */
    virtual bool parseMsg(QString newmsg);

    /**
     * 检查是否有空闲线程可以执行下一任务
     */
    bool checkHasFreeThread();

//TODO TOCHANGE TO private
public:
    
    bool initCalc();
    
    /*****************关于计算**************/
    /**
     * 计算完成
     */
    void calculate_finishAll();

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
     * [计算总进度]告知上层更新计算备份进度条（计算进度分不同的线程号）
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id
     * @param totalhasbackupnum 总任务数 
     * @param tmphasbackupnum 当前完成任务数 
     */
    void calcubackupProgressingBar(WorkingStatus status, QString slavid, int threadid, int totalhasbackupnum, int tmphasbackupnum, bool);
    
    // 单一线程单一任务消息
    /**
     * [计算单一线程单一任务中的初始化状态（起始帧、终止帧）]
     * @param status 当前工作状态、计算
     * @param index 主机号
     * @param threadid 线程号
     * @param taskname 任务名
     * @param startfc 起始帧
     * @param endfc 终止帧
     */
    void signalCalcuBackupTaskFC_init(WorkingStatus status, QString index, int threadid, QString taskname, long long startfc, long long endfc);
    /**
     * [计算单一线程单一任务中的进度反馈]
     * @param status 当前工作状态、计算
     * @param index 主机号
     * @param threadid 线程号
     * @param taskname 任务名
     * @param currentfc 当前计算帧
     */
    void signalCalcuBackupTaskFC(WorkingStatus status, QString index, int threadid, QString taskname, long long currentfc);

private slots:
    /**
     * 主控融合计算开始的处理槽函数
     * @param threadid 线程编号（单线程时为1）
     */
    void receiveThreadStart(int threadid, bool isok, int tunnelid, QString filename, qint64 beginfc, qint64 endfc);
    
    /**
     * 主控融合计算执行完的处理槽函数
     * @param threadid 线程编号（单线程时为1）
     */
    void receiveThreadFinish(int threadid, int isok, int tunnelid, QString filename, qint64 endfc);
    /**
     * 主控融合计算过程帧信息反馈槽函数
     * @param threadid 线程编号（单线程时为1）
     * @param tmpfc 当前计算帧号
     */
    void receiveStatusShow(int threadid, qint64 tmpfc, int tunnelid, QString filename);

};

#endif // LZ_FUSECALC_QUEUE_H
