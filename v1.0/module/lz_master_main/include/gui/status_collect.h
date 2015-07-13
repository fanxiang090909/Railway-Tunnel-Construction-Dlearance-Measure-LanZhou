#ifndef STATUS_COLLECT_WIDGET_H
#define STATUS_COLLECT_WIDGET_H

#include <QWidget>
#include "status.h"
#include "triggersetting.h"

namespace Ui {
    class CollectWidget;
}

/**
 * 监控界面——采集界面类声明
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 2014-03-01
 */
class CollectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CollectWidget(QWidget *parent = 0);
    ~CollectWidget();

private:
    Ui::CollectWidget *ui;

public slots:
    /**
     * 改变相机提示灯
     */
    void changeCamera(QString, HardwareStatus);
    void changeCameraFC(QString cameraindex,qint64);
    /**
     * 告知界面更新采集界面的当前任务名
     * @param status 更新进度条所在界面（计算、备份在不同界面）
     * @param slavid 从机号 QString型
     * @param threadid 线程id （采集时此参数无用，可赋0）
     * @param task 任务名
     * @param currentfc 当前进行到的帧号
     */
    void changeCameraTask(WorkingStatus status, QString cameraindex, int threadid, QString file, QString remark);

    /** 
     * 改变激光器提示灯
     */
    void changeLayser(QString, HardwareStatus);
    
    /**
     * 关闭所有提示灯
     */
    void turnOffHardwares();

private slots:

    /**
     * 设置采集触发工作模式,显示界面
     */
    void showModeSettingWidget();

    /**
     * 初始采集控制（在郑老师测速脉冲转化控制盒子未安装时使用）
     */
    void initCollectCtrlButton();

    ////////////TODO////////
    ////////////TODELETE////
    // 预进洞有效
    void intoTunnelValid();
    // 预进洞无效
    void intoTunnelNotValid();
    // 出洞
    void outFromTunnel();    
    // 完成采集
    void endCollectLine();
    
    /**
     * 软触发采集一帧
     */
    void softwareTriggerOneFrame();
   
    ///////临时

    /**
     * 相机复位
     */
    void hardwareReset();

    // 对UI界面按钮的更新
    // 预进洞有效 预进洞无效
    void InTunnel_UpdateUI(bool isvalid);
    // 出洞
    void OutTunnel_UpdateUI();
};

#endif // STATUS_COLLECT_WIDGET_H
