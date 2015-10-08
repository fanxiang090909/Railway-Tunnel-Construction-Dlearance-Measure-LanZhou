#ifndef SLAVEPROGRAM_MINI_H
#define SLAVEPROGRAM_MINI_H

#include <QtCore\QObject>
#include "client.h"

// @author 范翔
#include "lz_working_enum.h"

#include "lz_msg_queue.h"
#include "fileoperation.h"

using namespace std;

/**
 * 从控机程序业务处理类声明
 * @author 范翔
 * @version 1.0.0
 * @date 20151003
 */
class SlaveMiniProgram : public QObject
{
	Q_OBJECT

public:
    /**
     * 从控程序构造函数：传入初始化参数
     * @param initmode 采集模式（外同步触发采集/连续采集）
     *                 当前经过实验,不支持连续采集（因为文件的关闭和创建时间间隔过短可能会出现空指针异常），
     *                              只支持硬触发采集
     */
    SlaveMiniProgram(QObject *parent = 0);
    ~SlaveMiniProgram();
    
    /**
     * 网络初始化
     */
    bool networkConnectInit(); //从控程序初始化函数

    /**
     * 重启从控程序
     */
    bool restartSlaveProgram();

private:
    void terminateSlave();
    void startSlave();

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

    // 计算时间间隔时使用
    QDateTime lastTime;
    int m_time;

    QString slaveexepath;

public: //TODO发布时应改为private，public因为测试界面form中调用

    /*******各种给主控发送的消息，私有函数********/
    /***********四类配置文件的加载********/
    /**
     * 加载网络从控及相机SNID硬件，双目视觉标定文件名称配置文件
     */
    bool init_NetworkCameraHardwareFile(QString filename);

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

#endif // SLAVEPROGRAM_MINI_H
