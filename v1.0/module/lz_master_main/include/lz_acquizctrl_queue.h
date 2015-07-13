#ifndef LZ_ACQUIZCTRL_QUEUE_H
#define LZ_ACQUIZCTRL_QUEUE_H

#include <QList>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QFile>

#include "lz_msg_queue.h"

#include "network_config_list.h"
#include "plantask_list.h"
#include "lz_working_enum.h"
#include "status.h"

/**
 * 从控机采集队列类设计
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
class LzAcquizCtrlQueue : public LzMsgQueue
{
    Q_OBJECT

public:
    /**
     * 构造函数
     */
    LzAcquizCtrlQueue(QObject * parent = 0);

    virtual ~LzAcquizCtrlQueue();
    
    bool getIsAcquizing();

    void endCurrentMsg();

    /**
     * 是否正在采集状态
     */
    void setIsCollecting(bool newcollecting);
    /**
     * 是否正在采集状态
     */
    bool getIsCollecting();
//TODO TOCHANGE TO private
public:

    /**
     * 任务命令解析
     * 实现父类的纯虚函数
     */
    virtual bool parseMsg(QString newmsg);

    /*****************关于采集*********************/
    /**
     * 预进洞
     * @param isvalid true 预进洞有效，false 预进洞无效
     */
    void collect_IntoTunnel(bool isvalid);
    /**
     * 出洞
     */
    void collect_OutFromTunnel();

    /**
     * 发送预进洞信号
     * 给不同从机发送不同数据
     * 预进洞有效、无效信号 1103,isvalid=true(预进洞有效，false预进洞无效),slaveid=x,endframe_box1camref=x, endframe_box1cam=x, endframe_box2camref=x, endframe_box2cam=x,numofframes=x   
     */
    void collect_StartCurrentTunnelFiles(int slaveid, bool isvalid, _int64 endframe_box1camref, _int64 endframe_box1cam, _int64 endframe_box2camref, _int64 endframe_box2cam, int numofframes);

    /**
     * 结束采集文件
     * 告知所有从控机，存储完上一文件并等待接收下一帧外触发数据
     * @param slaveid 从机号
     * @param endframe_box1camref,endframe_box1cam,endframe_box2camref,endframe_box2cam 从机所连四个相机的终止帧号
     */
    void collect_EndCurrentTunnelFiles(int slaveid, _int64 endframe_box1camref, _int64 endframe_box1cam, _int64 endframe_box2camref, _int64 endframe_box2cam, int numofframes, float start_mile, float end_mile);

    /**
     * 结束采集线路，告知所有从控机关闭相机
     */
    void collect_EndCurrentLine();

    /**
     * 软同步触发采集一帧
     */
    void collect_SoftwareTriggerOneFrame();
   
    /**
     * 采集复位
     */
    void collect_ResetSlaves();

    /**
     * 私有函数
     * 记录里程和帧号
     */
    void collect_RecordFrameCounterAndMile();

private:

    /**
     * 是否正在采集状态
     */
    bool isCollecting;

    /**
     * 是否进入隧道中（正在写文件）
     */
    bool isAcquizing;
    bool hasCamerainit;

signals:

    /**
     * 向上层转发的消息
     */
    void signalToLights(HardwareType type, QString hardwareid, HardwareStatus status);
    void signalToCameraFC(QString, qint64);
    void signalToCameraTask(WorkingStatus, QString, QString);

    /**
     * 要转发给Slave的消息，经上层转发
     */
    void signalMsgToSlave(QString slaveip, QString msgToSlave);

    /**
     * 要转发给Slave的消息，经上层转发
     */
    void signalMsgToSlaves(QString msgToSlave);

    /**
     * 要转发给Master的文件，经上层转发
     */
    void signalFileToSlave(QString slaveid, QString fileToMaster);
};

#endif // LZ_ACQUIZCTRL_QUEUE_H
