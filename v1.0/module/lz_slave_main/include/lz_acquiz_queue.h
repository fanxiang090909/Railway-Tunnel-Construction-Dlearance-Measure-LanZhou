#ifndef LZ_ACQUIZ_QUEUE_H
#define LZ_ACQUIZ_QUEUE_H

#include <QList>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTimer>

#include "lz_slavemsg_queue.h"

#include "setting_slave.h"
#include "lz_project_access.h"

#include <acquisition.h>
//#include <calculate.h>
#include <compress.h>
#include <datastructure.h>

#include "network_config_list.h"
#include "plantask_list.h"
#include "lz_working_enum.h"

/**
 * 从控机采集队列类设计
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
class LzAcquizQueue : public LzSlaveMsgQueue
{
    Q_OBJECT

public:
    /**
     * 构造函数
     */
    LzAcquizQueue(QObject * parent = 0);
    LzAcquizQueue(LzCameraCollectingMode initmode, QObject * parent = 0);

    virtual ~LzAcquizQueue();
    
    bool getIsAcquizing();
//TODO TOCHANGE TO private
public:

    /**
     * 任务命令解析
     * 实现父类的纯虚函数
     */
    virtual bool parseMsg(QString newmsg);

    /**
     * 设置触发模式及采集曝光时间
     */
    void setTriggerModeAndExposureTime(LzCameraCollectingMode newtriggermode, int newexposuretime);

    /***************关于采集****************/
    /**
     * 相机硬件初始化
     * 步骤：
     * （1）配置相机SerialNumber和index(A1A2B1B2...R1R2的对应关系)
     * （2）设置采集模式（连续采集/硬件外同步触发）
     * （3）开启相机
     * @return 是否开启成功，上述步骤如果出错，则捕获异常，通过消息告知主控
     * @author 丁志宇 范翔
     */    
    bool cameraInit();
    /**
     * 开始采集某条线路
     * @author 范翔
     */
    bool collect_start_line();
    /**
     * 开始采集某条隧道
     * @author 范翔
     */
    bool collect_start_tunnel();
    /**
     * 相机采集异常处理，并告知主控机
     * @param working表示工作状态，在主控接收端是一个枚举类型0表示关闭，1表示开启但未在采集
     *					2表示正在工作，3表示异常
     * @author 范翔
     */
    void collect_CameraStatusFeedback(QString cameraindex, int working, QString errortype);

    /**
     * 采集某一隧道完成
     * @param ifdelete bool 是否删除这些文件
     */
    void collect_end_tunnel(bool ifdelete);
    /**
     * 发送采集完某一隧道消息
     * 1102,cameraindex=x,return=true,tunnelid=x,tunnelname=xxx,seqno=x(顺序号), start_framecounter=xx,end_framecounter=xx,filename_prefix=xx
     * @paran cameraindex 相机索引号A1...
     * @param tunnelid 隧道ID号（不是铁路规定编号）
     * @param tunnelname 隧道名称
     * @param seqno 当次线路采集序列号
     * @param startFrameCounter 起始帧号
     * @param endFrameCounter 终止帧号
     * @param filenamePrfix 存储路径前缀（注：后缀分别是"_A1.raw"...）
     */
    void collect_msgFinishOneTunnel(QString cameraindex, int tunnelid, QString tunnelname, int seqno, _int64 startFrameCounter, _int64 endFrameCounter, QString filenamePrfix);
    
    /**
     * 发送预进洞有效无效消息
     * 1104,cameraindex=x,isvalid=true（预进洞有效，else预进洞无效）,return=true,startframe=xx,endframe=xx,filename_prefix=xx(预进洞有效时有filename_prefix)
     */
    void collect_msgFinishIntoTunnel(bool isvalid, QString cameraindex, _int64 startFrameCounter, _int64 endFrameCounter, QString filename_prefix);
    /**
     * 采集当前计划任务线路,关闭相机
     */
    void collect_end_line();

    /**
     * 预进洞无效处理函数
     */
    void collect_beginIntoTunnelNotValid(_int64 endfc_box1cameraref, _int64 endfc_box1camera, _int64 endfc_box2cameraref, _int64 endfc_box2camera);

    /**
     * 出洞处理函数
     */
    void collect_beginOutfromTunnel(_int64 endfc_box1cameraref, _int64 endfc_box1camera, _int64 endfc_box2cameraref, _int64 endfc_box2camera,
                                        float startmile, float endmile);

    /**
     * 软同步采集一帧
     */
    void collect_one_frame_software_trigger();

    /**
     * 从机硬件采集状态复位
     * 如果正在执行任何任务，或出现任何问题，均停止恢复到初始未采集状态
     */
    bool collect_slave_reset();

    /**
     * 从控程序得到缓存图片
     */
    void retriveImg(QString tmpsn, unsigned __int64 * tmpfc, Mat & tmpimg, bool & isacquiz);

private:

    LzCameraAcquisition * lzacqui;		//采集的主类
    LzCameraCollectingMode collectionMode; // 采集模式（外同步触发采集/连续采集）
	int exposureTime;

    bool isAcquizing;
    bool hasCamerainit;

    // 从机的隧道记录
    QList<PlanTask>::iterator planTaskIterator;
    int seq;

    // 计算时间间隔时使用
    QDateTime lastTime;
    int m_time;

    /**
     * 从控界面定时监控显示计时器
     */
    QTimer* timer;

public slots:
    /**
     * 帧号反馈
     */
    void collect_feedbackFCToMaster(QString cameraid, long long currentfc);

private slots:
    
    /**
     * 执行完预进洞无效信号的结束信号槽函数
     */
    //void end_IntoTunnelNotValid( vector<AcquiRet> );
    void end_IntoTunnelNotValid( QString camera1, qint64 c1beg, qint64 c1end, QString camera2, qint64 c2beg, qint64 c2end,
                                 QString camera3, qint64 c3beg, qint64 c3end, QString camera4, qint64 c4beg, qint64 c4end);
    /**
     * 执行完出洞信号的结束信号槽函数
     */
    //void end_OutFromTunnel( vector<AcquiRet> );
    void end_OutFromTunnel( QString camera1, qint64 c1beg, qint64 c1end, QString camera2, qint64 c2beg, qint64 c2end,
                            QString camera3, qint64 c3beg, qint64 c3end, QString camera4, qint64 c4beg, qint64 c4end);

    /**
     * 从控界面定时监控显示计时器槽函数
     */
    void onTimeOut();

signals:

    /**
     * 告知界面信号
     */
    void startCollect(QString, QString, QString, QString, QString, QString, QString, QString);
    
    /**
     * 告知界面信号
     */
    void fcupdate(WorkingStatus, int, QString, long long);

    /**
     * 告知界面信号
     */
    void changeCollectCameraLight(QString ,HardwareStatus);
};

#endif // LZ_ACQUIZ_QUEUE_H
