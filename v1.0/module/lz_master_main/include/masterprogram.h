#ifndef MASTERPROGRAM_H
#define MASTERPROGRAM_H

#include "slavemodel.h"
#include "serverthread_master.h"
#include "multithreadtcpserver_master.h"
#include "status.h"

#include <QHostInfo>
#include <QStringList>

#include "status.h"
#include "xmlnetworkfileloader.h"
#include "xmlprojectfileloader.h"
#include "xmlcheckedtaskfileloader.h"
#include "xmltaskfileloader.h"
#include "xmlrealtaskfileloader.h"

#include "lz_project_access.h"

#include "setting_master.h"
#include "network_config_list.h"
#include "plantask_list.h"
#include "realtask_list.h"
#include "checkedtask_list.h"
#include "lz_working_enum.h"
#include "lz_acquizctrl_queue.h"
#include "lz_fusecalc_queue.h"
#include "lz_backupcalc_queue.h"

#include "ReadWriteHardwareInterface.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

/**
 * 主控服务程序驱动类设计
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-9-8
 */
class MasterProgram : public QObject
{
    Q_OBJECT
private:

    // singleton设计模式，静态私有实例变量
    static MasterProgram * masterProgramInstance;

    // 本身作为主控机的tcpserver服务，有listen方法
    MasterMultiThreadTcpServer * multiThreadTcpServer;

    // 接受到从控机tcp连接的socket列表
    QList<QTcpSocket *> tcpReceivedSocketList;

    // 凡是操作Status的列表，都要加锁解锁
    QMutex mutex;

    // 当前是否有异常，有异常之后不能再做操作
    //bool hasException;///TODO?

    /**
     * 是否需要保存计算备份状态
     */
    bool hasChangeBackupCalcuStatus;

    /**
     * 集成里程、进出洞检测装置后和郑老师定义的采集模式 手动触发模式 自动检测模式 自由触发模式
     * @author 范翔
     * @date 20150625
     */
    LzCollectingMode collectMode;
    
    /**
     * 配置从控相机的曝光时间
     */
    int exposureTime;
    /**
     * 触发采集模式
     */
    LzCameraCollectingMode triggerMode;
    /**
     * 硬触发采集模式下的帧间隔模式，隔0.5m，0.75m，1m采集一帧
     */
    LzCollectHardwareTriggerDistanceMode distanceMode;
    /**
     * 自由触发模式下采集帧率 30Hz、40Hz、50Hz、60Hz
     */
    LzCollectHardwareTriggerNoDistanceMode  noDistanceMode;

 public:

    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static MasterProgram* getMasterProgramInstance();

    ~MasterProgram();

    LzCameraCollectingMode getTriggerMode();


    const QList<SlaveModel>* list();

    /**
     * 返回多线程服务器类指针
     */
    MasterMultiThreadTcpServer * getMultiThreadTcpServer();

    /**
     * 采集从控同步进出洞操作控制队列
     */
    LzAcquizCtrlQueue * lzAcquizCtrlQueue;

    /**
     * 采集接口卡读写接口类
     * @date 20150526
     * @author 范翔
     */
    ReadWriteHardwareInterface * lzReadWriteHardwareInterface;

    /**
     * 主控融合计算队列
     */
    LzFuseCalcQueue * lzFuseCalcQueue;
    /**
     * 主控备份队列
     */
    LzBackupCalcQueue * lzBackupCalcQueue;

    /**
     * 设置采集模式和曝光时间
     */
    void setTriggerModeAndExposureTime(LzCameraCollectingMode newtriggerMode, int newexposureTime, LzCollectingMode collectmode, int distanceOrNoDistanceMode);

    /**
     * 设置脉冲触发间隔模式，隔1m1帧，0.5m一帧，0.75m一帧
     */
    bool setDistanceMode(LzCollectingMode newcollectmode, int distanceOrNoDistanceMode);

    /**
     * 初始化配置，启动服务器
     * @see MasterProgram的private方法 init_X..
     */
    void init();

    /**
     * 重启服务器
     */
    void restartServer();
    /**
     * 重启服务器
     */
    bool closeServer();
    /**
     * 保存采集时记录信息至文件 collect_EndCurrentLine时调用
     * @see 函数collect_EndCurrentLine
     */
    bool saveCollectedInfo();
    /**
     * 保存计算备份状态
     */
    bool saveCalcuBackupInfo(LzProjectClass projectclass);
    /**
     * 保存当前状态至文件
     * @see 函数closeServer时调用
     */
    //bool saveCurrentStatus();

    /*****************关于初始化*********************/
    /**
     * 初始化index和cameraidex
     * 从控机接收消息后会调用从控程序的相机init函数
     * @param filename index和cameraidex的配置文件路径
     */
    bool init_NetworkConfigFile(QString filename);

    /**
     * 初始化分中配置
     * @param filename 分中参数配置文件路径
     */
    bool init_FenZhongCalibParamFile();

    /**
     * 初始化双目相机标定参数配置文件
     * 发给指定的从控机上的相机组
     */
    bool init_CameraCalibrationParamFile();

    /**
     * 初始化任务配置
     * @param filename 任务配置文件路径
     */
    void init_TaskFile(QString filename);

    /*****************设置采集计算备份线路及模式*******/
    /**
     * 设置（采集、计算、备份、计算&&备份、文件信息校正）线路及模式，告知所有从控机
     * @param filename 工程入口文件名
     * @param lineid 线路ID号
     * @param linename 线路名称
     * @param date 日期
     * @param status 要切换的工作模式（采集、计算、备份、计算&&备份、文件信息校正）
     * @param isprep 是否为准备状态 如果为true，检查是否可以切换，只做检查配置文件是否存在并解析，若为false，直接切换工作模式开始工作（采集、计算、...）
     */
    void workingMode_Setting(QString filename, int lineid, QString linename, QString date, WorkingStatus status, bool isprep);

    /**
     * 创建临时工程（计算）
     * @param filename 工程入口文件名
     * @param lineid 线路ID号
     * @param linename 线路名称
     * @param date 日期
     * @param tunnelid 隧道编号
     * @param seqno 采集序号
     * @param newstartframes 新的起始帧号
     * @param newendframes 新的终止帧号
     */
    void createTempProject(QString filename, int lineid, QString linename, QString date, int tunnelid, int seqno, long long newstartframes, long long newendframes);
                    
    /*********************关于采集*******************/
public slots:

    /**
     * 预进洞
     * @param isvalid true 预进洞有效，false 预进洞无效
     */
    void collect_IntoTunnel(bool isvalid);
    /**
     * 出洞
     */
    void collect_OutFromTunnel();

public:

    /**
     * 结束采集线路，告知所有从控机关闭相机
     */
    void collect_EndCurrentLine();

    /**
     * 软同步触发采集一帧
     */
    void collect_SoftwareTriggerOneFrame();

    /**
     * 采集各个从机复位
     */
    void collect_ResetSlaves();

    /*****************关于计算*********************/
    /**
     * 开始从机双目计算
     * 全部计算模式
     */
    void calculate_beginStartAll();
    void calculate_beginStartOneTunnel(int tunnelid);
    /**
     * 计算单个文件
     * @param slaveid 告知那台从控机
     * @param cameragroupindex 相机组号
     * @param filename 计算结果文件名，也就是这个计算任务的内容
     * @param tunnelid 隧道ID
     * @param isinterrupted 是否是从中断计算开始计算（if true 上次曾被中断）
     * @param interruptfc if （isinterrupted == true） 中断帧号
     *                    else 此处无效，可置为0
     */
    void calculate_StartOneFile(int slaveid, QString cameragroupindex, QString filename, int tunnelid, bool isinterrupted, qint64 interruptfc);

    /**
     * 暂停计算
     */
    void calculate_Stop();
    /**
     * 融合计算之前的数据检查
     */
    void calculate_Fuse_checkisready();

    /**
     * 开始融合计算和提高度计算
     * 前提是在双目及车底RT计算的中间结果文件全部返回之后
     */
    void calculate_Fuse_beginStartAll(bool useerrorrectifyfactor, bool usesafetyfactor);
    void calculate_Fuse_beginStartOneTunnel(int tunnelid, bool useerrorrectifyfactor, bool usesafetyfactor);
    /**
     * 暂停融合
     */
    void calculate_Fuse_stop();

    /**
     * 开始做提高度计算
     * 前提是已经融合好
     */
   	void calculate_ExtractHeight_beginStartAll(bool useerrorrectifyfactor, bool usesafetyfactor);
    void calculate_ExtractHeight_beginStartOneTunnel(int tunnelid, bool useerrorrectifyfactor, bool usesafetyfactor);

    /*****************关于备份 *********************/
    /**
     * 开始备份
     */
    void backup_beginStartAll();
    void backup_beginStartOneTunnel(int tobackuptunnelid/*QString totalseqnostr*/);
    /**
     * 备份单个文件
     * @param slaveid 告知那台从控机
     * @param filename 备份文件名，也就是这个备份任务的内容
     * @param tunnelid 隧道编号
     * @param isinterrupted 是否是从中断备份开始备份（if true 上次曾被中断）
     * @param interruptfc if （isinterrupted == true） 中断位置
     *                    else 此处无效，可置为0
     * @param beginfc 文件起始帧号
     * @param endfc 文件终止帧号
     */
    void backup_StartOneFile(int slaveid, QString filename, int tunnelid, bool isinterrupted, qint64 interruptfc, qint64 beginfc, qint64 endfc);

    /**
     * 暂停备份
     */
    void backup_Stop();

    /**
     * 主控上的备份
     */
    void backup_Master_beginStartAll();
    void backup_Master_beginStartOneTunnel(int tunnelid);
    void backup_Master_StartOneTunnel(int tunnelid, QString tunnelfilenameprefix, int tmphasbackup, __int64 tmpbackupfc);
    /*****************关于文件校正*********************/
    /**
     * 希望查看原始图像
     */
    bool askForRawImages(QString realfilename, QString cameraindex, int seqno,int tunnelid, __int64 startFramecounter, int frameNum);

private:
    explicit MasterProgram(QObject *parent = 0);

    /***********处理接收到的从控消息***************/
    /*****************关于采集*********************/
    /**
     * 收到从控机返回的采集完数据
     * @param slaveid
     * @param tunnelid
     * @param tunnelname
     * @param newseqno
     * @param newcameraindex
     * @param start_framecounter
     * @param end_framecounter
     */
    void collect_SlaveCollected_R(int slaveid, int tunnelid, QString tunnelname, int newseqno, QString newcameraindex,
                                _int64 start_framecounter, _int64 end_framecounter);

    /**
     * 收到从机返回的关闭相机状态消息
     * @param slaveid
     * @param isclosecamera true 表示正常关闭，false表示非正常
     */
    void collect_SlaveEndLineCloseCamera_R(int slaveid, bool isclosecamera);

    /**
     * 警告采集异常
     */
    void warning_collect_R();

    /*****************关于计算*********************/
    /**
     * 收到从控机返回的计算进度
     * @param slaveid 从机号
     * @param tunnelid 隧道ID
     * @param camera_groupindex 或相机组号
     * @param isinterrupt 是否为暂停
     * @param interruptfc 暂停帧号（当 isinterrupt == true 时有作用，否则置为0）
     */
    void calculate_FeedbackPerTunnel_CameraGroup_R(int threadid, QString filename, int slaveid, int tunnelid, QString cameragroup_index, bool isinterrupt, __int64 interruptfc);

    /**
     * 警告计算异常
     */
    void warning_calculate_R();

    /*****************关于备份 *********************/
    /**
     * 收到从控机返回的备份进度
     * @param slaveid 从机号
     * @param seqno 采集序列号，在采集时由主机记录下到配置文件中，同时各个从机按seqno依次创建文件，文件名为“[seqno]_[tunnelname]_[date].dat”
     * @param fileType 表示文件类型，若为raw，则index表示相机编号；若为data，则index表示相机组号
     * @param index 相机，或相机组号
     */
    void backup_FeedbackPerTunnel_CameraGroup_R(QString filename, int slaveid, int type, int seqno, QString index, bool isinterrupted, qint64 interruptfc);

    /**
     * 警告备份异常
     */
    void warning_backup_R();

    // 相机index的string（A1, A2...）转换为int型
    // @author范翔 已经作废
    int indexStringToUiInteger(QString index);

signals:

    // 采集
    void msgToGUIShowCProjectCTRL(WorkingStatus, bool);
    // 三个对外信号
    // 出洞
    void OutTunnel();
    // 进洞有效，预进洞无效
    void InTunnel(bool isvalid);

    // 所有指示灯关闭
    void turnOffAllHardware();
    // 给上层界面的状态灯变变换提供消息（不同的硬件消息参数HardwareType不一样）
    void signalToLights(HardwareType type, QString hardwareid, HardwareStatus status);
    void signalToCameraFC(QString, qint64);
    /**
     * 告知界面更新采集、计算、备份界面的当前任务名
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param task 任务名
     * @param remark 任务名对应任务备注（出错信息等）
     */
    void signalToCameraTask(WorkingStatus status, QString slaveid, int threadid, QString task, QString remark);
    // [计算转发消息]
    /**
     * [计算转发消息槽函数]
     * 每个从控的每个线程每个任务-进度初始化
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param task 任务名
     * @param currentfc 当前进行到的帧号
     */
    void signalToSlaveCameraTaskFC_init(WorkingStatus status, QString slaveid, int threadid, QString task, long long startfc, long long endfc);
    /**
     * [计算转发消息槽函数]
     * 每个从控的每个线程每个任务-任务当前帧反馈
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param task 任务名
     * @param currentfc 当前进行到的帧号
     */
    void signalToCameraTaskFC(WorkingStatus status, QString slaveid, int threadid, QString task, long long currentfc);

    // [计算备份消息]
    /**
     * 告知界面更新计算备份进度条（计算进度分不同的线程号）
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id
     * @param totalhasbackupnum 总任务数 
     * @param tmphasbackupnum 当前完成任务数 
     */
    void calcubackupProgressingBar(WorkingStatus status, QString slaveid, int threadid, int totalhasbackupnum, int tmphasbackupnum, bool);

    // 给上层界面的控制台输出提供消息
    void signalMsgToGUI(WorkingStatus, const QString &msg);
    void signalErrorToGUI(WorkingStatus, const QString &msg);
    
    // 计算备份按钮
    void makeGUIActive(WorkingStatus, bool);

private slots:

    /**
     * !!现在这个函数多余了2013-09-24fan
     * ||如果需要应该在该类的init函数中去掉注释的connect两句话
     * 检测到server的msg，向上层抛出，这里不做处理
     */
    void displayMsg(QString msg);

    /**
     * 解析消息字符串
     * @param msg 自定义的消息，这里解析从从控机传到主控机的消息
     * @see 《主控从控程序流程及消息设计.ppt》版本v1.0
     */
    void parseMsg(QString msg);

    // [采集转发消息]
    void slotParsedMsg(QString);
    void slotToLights(HardwareType, QString, HardwareStatus);
    void slotToCameraFC(QString, qint64);
    /**
     * @param-type WorkingStatus 
     * @param-type QString 从机或主机id
     * @param-type QString 显示备注信息（错误等）
     */
    void slotToCameraTask(WorkingStatus, QString, QString);

    // [采集计算备份转发消息]
    /**
     * 告知界面更新采集、计算、备份界面的当前任务名
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param task 任务名
     * @param remark 任务名对应任务备注（出错信息等）
     */
    void slotToCalcuBackupTask(WorkingStatus workingstatus, QString id, int threadid, QString task, QString status);

    // [计算备份转发消息]
    /**
     * 告知界面更新计算、备份任务总进度条
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id
     * @param totalhasbackupnum 总任务数 
     * @param tmphasbackupnum 当前完成任务数 
     */
    void slotCalcubackupProgressingBar(WorkingStatus workingstatus, QString id, int threadid, int totalnum, int currentnum, bool isupdate);

    // [计算-融合计算转发消息]
    /**
     * [计算单一线程单一任务中的初始化状态（起始帧、终止帧）]
     * @param status 当前工作状态、计算
     * @param index 主机号
     * @param threadid 线程号
     * @param taskname 任务名
     * @param startfc 起始帧
     * @param endfc 终止帧
     */
    void slotToCalcuBackupTaskFC_init(WorkingStatus status, QString index, int threadid, QString taskname, long long startfc, long long endfc);
    /**
     * [计算单一线程单一任务中的进度反馈]
     * @param status 当前工作状态、计算
     * @param index 主机号
     * @param threadid 线程号
     * @param taskname 任务名
     * @param currentfc 当前计算帧
     */
    void slotToCalcuBackupTaskFC(WorkingStatus status, QString index, int threadid, QString taskname, long long currentfc);

    // [向从机转发消息文件]
    /**
     * 向某一从机转发消息
     */
    void slotMsgToSlave(QString ip, QString msg);
    /**
     * 向所有从机转发消息
     */
    void slotMsgToSlaves(QString msg);
    /**
     * 向某一从机转发文件
     */
    void slotMsgToFile(QString ip, QString fullfilepath);
    
    /**
     * 采集里程计数相关硬件异常
     */
    void slotReadWriteHardwareException(QString excep);
};

#endif // MASTERPROGRAM_H
