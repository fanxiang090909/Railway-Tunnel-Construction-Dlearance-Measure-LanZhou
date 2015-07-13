#ifndef MAINWIDGET_H
#define MAINWIDGET_H
#include <tinyxml.h>
#include <tinystr.h>

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>

#include "status.h"
#include "lz_working_enum.h"
#include "lz_project_access.h"

#include "lz_logger.h"

namespace Ui {
    class MainWidget;
}

/**
 * 主控监控页面tabwidget界面类声明
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-04-17
 */
class MainWidget : public QWidget
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
    explicit MainWidget(bool newsinglewidgetapp, WorkingStatus initstatus = Collecting, QWidget *parent = 0);
    ~MainWidget();

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

    // 对工程创建导入界面
    void setCreateProjectWidgetEnabled(bool);

protected:
    // 桌面系统图标相关，防止误操作关闭，可后台运行
    void closeEvent(QCloseEvent *event);

private:
    /** 
     * 日志类初始化
     */
    bool initLogger(string filename, string username);

public slots:
    void appendMsg(WorkingStatus, QString msg);
    void appendError(WorkingStatus, QString msg);
    
    // 是否可以显示界面
    void msgToGUICreateProjectWidget(WorkingStatus, bool);

    /**
     * [采集转发消息槽函数]
     */
    void changeHardwareStatus(HardwareType, QString, HardwareStatus);
    /**
     * [采集转发消息槽函数]
     */
    void changeCameraFC_slot(QString cameraindex, qint64);
    
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

    /**
     * [计算备份转发消息槽函数]
     * 激活是否可以点击开始计算按钮
     */
    //void getCalcuBackupGUIActive(bool);

private slots:

    // 关闭所有提示灯
    void turnOffLights();

    // 开启网络连接从控计算机提示提示灯
    void initConnectedSlaves();

    // 重启服务器槽函数
    void restartServer();
    // 关闭服务器
    void closeServer();

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
    Ui::MainWidget *ui;

    // 是否单一界面程序，结束方式不同
    bool singlewidgetapp;

    // 当前工作模式，根据这个不同，初始化界面不同
    WorkingStatus workingstatus;

    QString projectfilename;

    // 当前用户名（创建工程时记录）
    QString currentusername;

    // 日志类
    LzLogger * logger;

    bool hasinitlog;

    LzProjectClass projectclass;
    QString projectclassstr;
};

#endif // MAINWIDGET_H
