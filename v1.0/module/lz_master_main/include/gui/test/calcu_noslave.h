#ifndef CALCUNOSLAVEWIDGET_H
#define CALCUNOSLAVEWIDGET_H
#include <tinyxml.h>
#include <tinystr.h>

#include <QObject>
#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>

#include "lz_working_enum.h"
#include "lz_project_access.h"

#include "lz_fusecalc_queue.h"
#include "lz_calc_queue.h"
#include "setting_slave.h"

namespace Ui {
    class CalcuNoSlaveWidget;
}

/**
 * 主控监控页面tabwidget界面类声明
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-04-17
 */
class CalcuNoSlaveWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * 构造函数传入WorkingStatus status初始化界面
     * @param newsinglewidgetapp 是否为单一界面程序（采集程序有可能为单一界面程序，其结束方式与复界面程序不用） @see closeEvent方法
     * @param status 工作状态，@see status.h
     * Collecting = 1 采集时
     * Calculating_Backuping = 5 计算备份同时进行
     */
    explicit CalcuNoSlaveWidget(bool newsinglewidgetapp, WorkingStatus initstatus = Calculating, QWidget *parent = 0);
    ~CalcuNoSlaveWidget();

signals:
    // 对采集和计算备份界面
    void sendproject_file_name(QString);
    // 对采集界面
    // 更改相机提示灯
    void changeCameraFC(QString cameraindex, qint64 fc);
    /**
     * [采集计算备份转发消息]
     * 告知界面更新采集、计算、备份界面的当前任务名
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param task 任务名
     * @param remark 任务名对应任务备注（出错信息等）
     */
    void changeSlaveCameraTask(WorkingStatus status, QString index, int threadid, QString filename, QString remark);
    /**
     * [计算转发消息]
     * 计算界面用（当前线程当前文件的当前计算帧号）告知界面更新采集、计算、备份界面的当前任务名
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param task 任务名
     * @param currentfc 当前进行到的帧号
     */
    void changeSlaveCameraTaskFC(WorkingStatus status, QString index, int threadid, QString filename, long long currentfc);
    /**
     * [计算转发消息]
     * 计算界面用（当前线程当前文件的当前计算帧号）告知界面更新采集、计算、备份界面的当前任务名
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param task 任务名
     * @param startfc 任务起始帧号
     * @param endfc 任务终止帧号
     */
    void changeSlaveCameraTaskFC_init(WorkingStatus status, QString index, int threadid, QString filename, long long startfc, long long endfc);
    // 更改激光器状态提示灯
    void changeLayserStatus(QString layser, HardwareStatus status);
    // 关闭所有提示灯信号
    void turnOffAllHardware();

    // 对计算备份界面
    void makeGUIActive(bool);
    void changeCameraStatus(QString cameraindex, HardwareStatus status);
    /**
     * 告知界面更新计算备份进度条（计算进度分不同的线程号）
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id
     * @param totalhasbackupnum 总任务数 
     * @param tmphasbackupnum 当前完成任务数 
     */
    void calcubackupProgressingBar(WorkingStatus status, QString slaveid, int threadid, int totalhasbackupnum, int tmphasbackupnum, bool);

    //////////////////////从控
    void initGroupBox(WorkingStatus);
    void showSlaveID();

protected:
    // 桌面系统图标相关，防止误操作关闭，可后台运行
    void closeEvent(QCloseEvent *event);

public slots:
    void appendMsg(QString msg);
    void appendError(QString msg);

    // 每个从控的每个线程每个任务
    /**
     * [采集计算备份转发消息槽函数]
     * 每个从控的每个线程每个任务-任务名称
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param task 任务名
     * @param remark 任务名对应任务备注（出错信息等）
     */
    void changeSlaveCameraTask_slot(WorkingStatus status, QString index, int threadid, QString file, QString remark);
    /**
     * [计算转发消息槽函数]
     * 每个从控的每个线程每个任务-进度初始化
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param task 任务名
     * @param currentfc 当前进行到的帧号
     */
    void changeSlaveCameraTaskFC_init_slot(WorkingStatus status, QString index, int threadid, QString file, long long startfc, long long endfc);
    /**
     * [计算转发消息槽函数]
     * 每个从控的每个线程每个任务-帧号反馈
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param task 任务名
     * @param currentfc 当前进行到的帧号
     */
    void changeSlaveCameraTaskFC_slot(WorkingStatus status, QString index, int threadid, QString file, long long currentfc);
    // 每个从控的总任务
    /**
     * [计算备份转发消息槽函数]
     * 每个从控的总任务 告知界面更新计算备份进度条（计算进度分不同的线程号）
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id
     * @param totalhasbackupnum 总任务数 
     * @param tmphasbackupnum 当前完成任务数 
     */
    void changecalcubackupProgressingBar_slot(WorkingStatus status, QString slaveid, int threadid, int totalhasbackupnum, int tmphasbackupnum, bool);

    void getCalcuBackupGUIActive(bool);

    ///////主控界面////////////;
    ///////////////////CREATEPROJEC///////
    /**
     * 导入工程文件
     */
    void importProj();

    ////////////////////////////////////////////////
    /////////////////主控函数复制//////////////////
    /**
     * 开始从机双目计算
     * 全部计算模式
     */
    void calculate_beginStartAll();
    void calculate_beginStartOneTunnel(int tunnelid);
    void calculate_Fuse_beginOneTunnel(int tunnelid);
    void calculate_ExtractHeight_beginOneTunnel(int tunnelid);
    void calculate_StartOneFile(int slaveid, QString cameragroupindex, QString filename, int tunnelid, bool isinterrupted, qint64 interruptfc);

    /**
     * 暂停计算
     */
    void calculate_Stop();

    void calculate_Fuse_checkisready();

    /**
     * 开始融合计算和提高度计算
     * 前提是在双目及车底RT计算的中间结果文件全部返回之后
     */
    void calculate_Fuse_beginAll();
    /**
     * 暂停融合
     */
    void calculate_Fuse_stop();
	
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    /////////////////从控函数复制//////////////////
    bool changeMode_newProjectConfig(LzProjectClass projectclass, QString newfilename);
    /**
     * 加载网络从控及相机SNID硬件，双目视觉标定文件名称配置文件
     */
    bool init_NetworkCameraHardwareFile(LzProjectClass projectclass, QString filename);
    /**
     * 每次启动都要向主控申请task文件（如果是上次备份又开始则不需要申请）
     */
    bool init_TaskFile(QString filename);
    /**
     * 加载轨平面分中坐标系标定配置文件
     */
    bool init_FenZhongFile(QString projectpath, SlaveModel & sm);
    /**
     * 加载双目视觉标定配置文件
     */
    bool init_CameraCalibrationFile(QString projectpath, SlaveModel & sm);

    /**
     * 初始化完成
     * @param WorkingStatus function 相当于一个枚举，1表示是否可以开始采集，2表示是否可以开始计算
     * @param QString filename 要解析的任务文件，当采集时需要计划文件"plan_xxxx_20140211.xml"
     *                                        当计算备份时需要校正文件"checked_xxxx_20140211.xml"
     * @param QString linename, QString date 暂时没用，与filename冲突
     */
    bool checkedok(WorkingStatus function, QString filename, QString linename, QString date);

    /*****************解析计划隧道任务校正文件**************/
    /**
     * 接收实际采集记录文件
     * @param projectclass 对备份？计算？校正任务加载realtask
     * @param filename 配置文件绝对路径
     */
    bool loadRealTaskFile(LzProjectClass projectclass, QString filename);
    /**
     * 接收校正文件
     * @param projectclass 对备份？计算？加载realtask
     * @param filename 配置文件绝对路径
     */
    bool loadCorrectedTaskFile(LzProjectClass projectclass, QString filename);

    ///////////////////////////////////////////////////////////
private slots:
    ///////////////////NEW
    void sendParsedMsgToGUI(QString);
    void sendMsgToMaster(QString);
    void receiveThreadStart(int threadid, bool isok, int tunnelid, QString filename, qint64 beginfc, qint64 endfc);
    void receiveThreadFinish(int threadid, int isok, int tunnelid, QString filename);
    //void receiveStatusShow(int threadid, qint64 tmpfc, int tunnelid, QString filename);
    void receivefcupdate(WorkingStatus, int, QString, qint64);


    void setSelectProject(bool);
    /**
     * @param int 类型，0双目全部算，1双目按隧道算 2 fuse全部算，4 fuse按隧道算 
     * @param int tunnelid
     */
    void startcalcu2(int, int);
    /////////////////////

    // 关闭所有提示灯
    void turnOffLights();

    // 配置操作及操作线路槽函数
    void configCurrentOperation();
    void configCurrentOperation2();

    // 界面传递参数-工程文件
    void slotprojectfilename(QString);

    // 桌面系统图标相关，防止误操作关闭，可后台运行
    void setIcon();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void messageClicked();

private:
    // 桌面系统图标相关，防止误操作关闭，可后台运行
    void createActions();
    void createTrayIcon();

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

private:
    Ui::CalcuNoSlaveWidget *ui;

    // 是否单一界面程序，结束方式不同
    bool singlewidgetapp;

    // 当前工作模式，根据这个不同，初始化界面不同
    WorkingStatus workingstatus;

    QString projectfilename;

    // 当前用户名（创建工程时记录）
    QString currentusername;

    LzProjectClass projectclass;
    QString projectclassstr;

    /**
     * 主控融合计算队列
     */
    LzFuseCalcQueue * lzFuseCalcQueue;

    LzCalcQueue * lzCalcQueue;

    bool firstInitNetwork;
};

#endif // CALCUNOSLAVEWIDGET_H
