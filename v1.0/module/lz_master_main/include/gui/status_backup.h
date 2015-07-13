#ifndef STATUS_BACKUP_WIDGET_H
#define STATUS_BACKUP_WIDGET_H

#include "masterprogram.h"
#include <QWidget>

#include "projectmodel.h"

namespace Ui {
    class BackupWidget;
}

/**
 * 监控界面——计算备份界面类声明
 * @author 熊雪
 * @version 1.0.0
 * @date 2014-03-01
 */
class BackupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BackupWidget(QWidget *parent = 0);
    ~BackupWidget();

    /** 
     * 添加经过校正之后的实际隧道队列数据到TableView中
     */
    void loadCheckTaskTunnelData();
    /** 
     * 更新经过校正之后的实际隧道队列数据到TableView中
     */
    void updateCheckedTaskWidget();

    /**
     * 设置计算模式选择
     */
    void SelectBackupMode();

private slots:

    void on_startBackupButton_clicked();

    void on_stopBackupButton_clicked();

public slots:

    /**
     * 修改计算备份进度状态槽函数
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param task 任务名
     * @param remark 任务名对应任务备注（出错信息等）
     */
    void changeSlaveTask(WorkingStatus status, QString index, int threadid, QString task, QString remark);
    /**
     * 修改进度条槽函数
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id
     * @param totalhasbackupnum 总任务数 
     * @param tmphasbackupnum 当前完成任务数 
     */
    void changecalcubackupProgressingBar(WorkingStatus status, QString index, int threadid, int totalhasbackupnum, int tmphasbackupnum, bool);
    
    /**
     * 初始化备份线程，初始化其状态状态槽函数
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param filename 任务名
     * @param startfc 任务起始帧
     * @param endfc 任务终止帧
     */
    void changeSlaveCameraTaskFC_init(WorkingStatus status, QString index, int threadid, QString filename, long long startfc, long long endfc);
    /**
     * 修改备份进度状态槽函数
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param filename 任务名
     * @param currentfc 当前计算帧号
     */
    void changeSlaveCameraTaskFC(WorkingStatus status, QString index, int threadid, QString filename, long long currentfc);
    
    /**
     * 设置按钮是否可用
     */
    void setOptButtonEnable(bool isinitstatus);

    /**
     * 加载计划任务中的隧道列表
     */
    void updateCheckedTunnelView();

private:
    Ui::BackupWidget *ui;

    enum {
        SLAVE_NUMBER = 0,
        CALCU_PROGRESS = 1,
        BACKUP_PROGRESS = 2
    };

    enum {
        CHECKEDTASK_TUNNELID = 0,
        CHECKEDTASK_TUNNELNAME = 1,
        CHECKEDTASK_TIME = 2,
        CHECKEDTASK_SEQNO_TOTAL = 3,
        CHECKEDTASK_START_MILE = 4,
        CHECKEDTASK_END_MILE = 5,
        CHECKEDTASK_STARTFRAME = 6,
        CHECKEDTASK_ENDFRAME = 7,
        CHECKEDTASK_TOTALFRAME = 8
    };

    // 当前工程名及工程入口模型
    // 【注意！】不采用MasterSetting中的ProjectModel的原因是怕在计算备份工程中MasterSetting中的值会改变
    QString project_filename;
    ProjectModel currentProjectModel;
    bool hasinintproject;

    bool isincalculatebackupstatus;
};

#endif // STATUS_BACKUP_WIDGET_H
