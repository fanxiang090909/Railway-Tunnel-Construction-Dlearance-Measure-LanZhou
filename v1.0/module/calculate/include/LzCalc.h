#ifndef LZCALCULATE_H
#define LZCALCULATE_H

#include <QObject>
#include <QMutex>
#include <QList>
#include <QDebug>
#include <string>

#include "LzCalc_thread.h"

#include "slavemodel.h"
#include "checkedtask_list.h"
#include "lz_project_access.h"
#include "xmlcheckedtaskfileloader.h"

/**
 * 兰州从控计算调度类
 * @author 范翔
 * @version 1.0.0
 * @date 2014-08-05
 */
class LzSlaveCalculate : public QObject
{
    Q_OBJECT

public:
    LzSlaveCalculate(LzCalcThreadType initcalcthreadtype, int initnumofthreads = 1, QObject * parent = 0);

    ~LzSlaveCalculate();
    
    /**
     * 得到允许最大线程数
     */
    int getNumOfMaxThreads();

    /**
     * 得到当前线程数
     */
    int getNumOfCurrentThreads();

    bool hasInitSlaveModel();
    bool hasInitCheckedTask();

    bool initProjectPath(QString path);

    bool initChekedTaskFile(QString path, QString filename);

    bool initSlaveModel(SlaveModel & initmodel);

    /**
     * 开始计算
     */
    bool startNewTwoViewCalc(QString cameragroupindex, int tunnelid, QString filename, bool isinterrupt, qint64 interruptedfc);

    /**
     * 开始融合计算
     */
    bool startFuseCalc(int tunnelid, QString filename, bool isinterrupt, qint64 interruptedfc);

    /**
     * 当前所有线程暂停计算
     */
    bool suspendAllThreads();

private:
    /**
     * 工程目录
     */
    std::string projectpath;
    /**
     * 从机模型
     */
    SlaveModel slavemodel;
    bool hasinitslavemodel;

    bool hasinitcheckedtask;

    /**
     * 计算包括线程类别，是否为负责计算RT的从机
     */
    LzCalcThreadType calcuthreadtype;

    /**
     * 最大线程数
     */
    int maxnumofthreads;

    /**
     * 当前正在运行线程
     */
    QList<LzCalculateThread *> currentThreads;
    int currentnumofthreads;
    QMutex nummutex;

signals:
     /**
     * 向LzCalcuQueue转发信号
     * @param threadid 线程编号（单线程时为1）
     *             0 线程无关（错误信息），可能开始计算创建线程之前就出错
     * @param isok true 开始正常，false 开始不正常
     */
    void myStart(int threadid, bool isok, int tunnelid, QString filename, qint64 beginfc, qint64 endfc);

    /**
     * 向LzCalcuQueue转发信号
     * @param threadid 线程编号（单线程时为1）
     *             0 线程无关（错误信息），可能开始计算创建线程之前就出错
     * @param isok 0 正常计算结束
     *             -1 暂停计算成功。需要参数endfc
     *             1 找不到SlaveModel中对应的cameragroupindex
     *             2 得不到有效的CheckedTunnelTaskModel
     *             5 计算结果文件未打开（主要针对从原来暂停文件中恢复计算的情况）
     *             6 其他异常，（融合高度配置文件未加载）
     * @param tunnelid 线程所计算的隧道ID
     * @param filename 线程所计算的任务文件名
     * @param endfc 计算的终止帧号
     */
    void finish(int threadid, int isok, int tunnelid, QString filename, qint64 endfc);

    /**
     * 向LzCalcuQueue转发信号
     * @param threadid 线程编号（单线程时为1）
     * @param tmpfc 当前计算帧号
     */
    void statusShow(int threadid, qint64 tmpfc, int tunnelid, QString filename);

private slots:
    /**
     * 接收执行线程结束的消息
     * @param threadid 线程编号（单线程时为1）
     */
    void receiveThreadFinish(int threadid, int isok, int tunnelid, QString filename, qint64 endfc);

    /**
     * 向LzCalcuQueue转发信号
     * @param threadid 线程编号（单线程时为1）
     * @param tmpfc 当前计算帧号
     */
    void receiveStatusShow(int threadid, qint64 tmpfc, int tunnelid, QString filename);
};

#endif // LZCALCULATE_H