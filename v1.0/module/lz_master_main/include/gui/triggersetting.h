#ifndef TRIGGERSETTING_H
#define TRIGGERSETTING_H

#include <QDialog>
#include "lz_working_enum.h"

namespace Ui {
    class TriggerSettingWidget;
}

/**
 * 采集状态监控界面,设置采集触发状态
 * @author 范翔
 * @version 1.0.0
 * @date 2014-10-23
 */
class TriggerSettingWidget : public QDialog
{
    Q_OBJECT

public:
    explicit TriggerSettingWidget(QWidget *parent = 0, double defaultDistanceMode = 0.5103, int currentDistanceModeType = 0);
    
    ~TriggerSettingWidget();

private slots:
 
    void setframes();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void comboBox1currentIndexChanged(int);
    void comboBox2currentIndexChanged(int);

signals:
    void sendframestocheck_task(int,int);

    void signalSetDistanceMode(int);

private:
    Ui::TriggerSettingWidget *ui;

    // 
    LzCameraCollectingMode cameraCollectMode;

    /**
     * 采集模式 手动触发模式 自动检测模式 自由触发模式
     */
    LzCollectingMode defaultcomboindex;

    /**
     * 手动触发模式 自动检测模式 下采集间隔 默认1米1帧
     */
    LzCollectHardwareTriggerDistanceMode distanceMode;

    /**
     * 外面传进来的 默认的0.5m一帧模式下的 相邻两帧间隔里程，比如0.5103，并非绝对的0.5
     */
    double defaultDistanceMode;

    /**
     * 自由触发模式下采集帧率 30Hz、40Hz、50Hz、60Hz
     */
    LzCollectHardwareTriggerNoDistanceMode  noDistanceMode;

    int defaultcomboindex_2;

    bool changingComboBox2;
};

#endif // TRIGGERSETTING_H
