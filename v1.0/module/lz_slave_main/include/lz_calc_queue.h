#ifndef LZCALC_QUEUE_H
#define LZCALC_QUEUE_H

#include <QList>
#include <QDebug>

#include "lz_slavemsg_queue.h"
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
class LzCalcQueue : public LzSlaveMsgQueue
{
    Q_OBJECT

public:
    /**
     * 构造函数
     */
    LzCalcQueue(QObject * parent = 0);

    virtual ~LzCalcQueue();
    
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
     * 开始计算
     */
    void calculate_start(QString filename, int tunnelid, QString cameraGroupIndex, bool isinterrupt, qint64 interruptfc);
    /*****************关于计算反馈**************/
    /**
     * 开始计算反馈
     */
    void calculate_feedbackStartToMaster(int threadid, QString filename, int tunnelid, QString cameraGroupIndex, qint64 beginfc, qint64 endfc);
    /**
     * 计算完成进度反馈
     */
    void calculate_feedbackToMaster(int threadid, QString filename, int tunnelid, QString cameraGroupIndex);
    /**
     * 计算异常反馈
     */
    void calculate_handleError(int threadid, QString filename, int tunnelid, QString cameraGroupIndex, QString errortype);
    /**
     * 确认暂停计算
     * @paran ret true 确认暂停，false，尚不能暂停，已加入队列稍后暂停
     */
    void calculate_feedbackToMaster_suspend(bool ret, int threadid, QString filename, int tunnelid, QString cameraGroupIndex, qint64 endfc);
    /**
     * 计算完成
     */
    void calculate_finishAll();

private slots:
    /**
     * 从控任务开始的处理槽函数
     * @param threadid 线程编号（单线程时为1）
     */
    void receiveThreadStart(int threadid, bool isok, int tunnelid, QString filename, qint64 beginfc, qint64 endfc);
    
    /**
     * 从控任务执行完的处理槽函数
     * @param threadid 线程编号（单线程时为1）
     * @param isok 0 正常计算结束
     *             -1 暂停计算成功。需要参数endfc
     *             1 找不到SlaveModel中对应的cameragroupindex
     *             2 得不到有效的CheckedTunnelTaskModel
     *             5 计算结果文件未打开（主要针对从原来暂停文件中恢复计算的情况）
     *             6 其他异常，（融合高度配置文件未加载）
     */
    void receiveThreadFinish(int threadid, int isok, int tunnelid, QString filename, qint64 endfc);
    /**
     * 槽函数
     * @param threadid 线程编号（单线程时为1）
     * @param tmpfc 当前计算帧号
     */
    void receiveStatusShow(int threadid, qint64 tmpfc, int tunnelid, QString filename);

signals:

    /**
     * 告知界面信号
     * @param status WorkingStatus 
     * @param threadid 线程号
     * @param currentfc 当前帧号
     */
    void fcupdate(WorkingStatus, int threadid, QString filename, long long currentfc);

};

#endif // LZCALC_QUEUE_H
