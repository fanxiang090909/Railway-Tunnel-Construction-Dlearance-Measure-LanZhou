#ifndef SLAVEPROGRAM_H
#define SLAVEPROGRAM_H

#include <QtCore\QObject>
#include "client.h"
#include "xmltaskfileloader.h"
#include "lz_project_access.h"

// @author 范翔
#include "lz_working_enum.h"

#include "lz_msg_queue.h"
#include "lz_acquiz_queue.h"
#include "lz_calc_queue.h"
#include "lz_backup_queue.h"
#include "lz_correct_queue.h"
#include "fileoperation.h"

using namespace std;

/**
 * 从控机程序业务处理类声明
 * @author 范翔
 * @author 熊雪
 * @author 冯明晨
 * @version 1.0.0
 * @date 20141029
 */
class SlaveProgram : public QObject
{
	Q_OBJECT

public:
    /**
     * 从控程序构造函数：传入初始化参数
     * @param initmode 采集模式（外同步触发采集/连续采集）
     *                 当前经过实验,不支持连续采集（因为文件的关闭和创建时间间隔过短可能会出现空指针异常），
     *                              只支持硬触发采集
     */
    SlaveProgram(LzCameraCollectingMode initmode, QObject *parent = 0);
    ~SlaveProgram();
    
    /**
     * 网络初始化
     */
    bool networkConnectInit(); //从控程序初始化函数

    LzAcquizQueue * getLzAcquizQueue();
    LzCalcQueue * getLzCalcQueue();
    LzBackupQueue * getLzBackupQueue();
    LzCorrectQueue * getLzCorrectQueue();

public slots:
    
    /**
     * 给主控发消息（发布时改为private）
     */
    void sendMsgToMaster(QString msg);
    /**
     * 给主控发文件
     */
    void sendFileToMaster(QString filename);

private slots:
    void sendParsedMsgToGUI(QString newmsg);

private:
    //发送接收消息的类
    Client * client;
	bool firstInitNetwork;

    // 缓冲执行采集、计算、备份命令的队列
    LzAcquizQueue * lzAcquizQueue;
    LzCalcQueue * lzCalcQueue;
    LzBackupQueue * lzBackupQueue;
    LzCorrectQueue * lzCorrectQueue;

    // 计算时间间隔时使用
    QDateTime lastTime;
    int m_time;

public: //TODO发布时应改为private，public因为测试界面form中调用

    /**
     * 切换工作模式-采集、备份时新工程配置
     * 设置文件接收路径
     *     工程路径
     *     工程文件.proj解析加载
     * @author 范翔 @date 20140509
     */
    bool changeMode_newProjectConfig(LzProjectClass projectcalss, QString newfilename);

    /*******各种给主控发送的消息，私有函数********/
    /***********四类配置文件的加载********/
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
    void checkedok(WorkingStatus function, QString filename, QString linename, QString date);

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

    /*****************传送文件**************/
    /**
     * 发送普通文件
     * @author 熊雪
     */
    void send_file(QString filename);

private slots:

    /**
     * 网络消息字符串解析
     * @author fengmingchen
     * @author fanxiang
     * @date 2013-11-24
     */
    void ParseMsg(QString msg);//如果备份和计算的时候有断点消息会比较长，这个时候需要对消息解析

signals:
    /**
     * 给主控发消息
     * @author fanxiang
     */    
    void signalMsgToGUI(QString);
    void signalErrorToGUI(QString);

    /**
     * 界面归零
     */
    void initGroupBox(WorkingStatus);

    /**
     * 更新界面基础配置显示
     */
    void showSlaveID();
};

#endif // SLAVEPROGRAM_H
