#ifndef READ_WRITE_HARDWARE_INTERFACE_H_
#define READ_WRITE_HARDWARE_INTERFACE_H_

#include <QObject>
#include <QDebug>
#include <QMutex>
#include <QTime>
#include "ReadHardwareThread.h"
#include "WriteHardware.h"

#include "lz_working_enum.h"

using namespace std;

#ifndef  deviceDescription
#define  deviceDescription  L"USB-4751,BID#0"
#endif

/**
 * 读写硬件接口，IO模式读写接口卡数据类接口
 * @author 范翔
 * @date 20150525
 * @version 1.0.0
 */
class ReadWriteHardwareInterface : public QObject
{
    Q_OBJECT
private:
    vector<bool> bitread;
    ReadHardwareThread * r;

    WriteHardware * w;

    bool initwrite;
    bool initread;

public:

    ReadWriteHardwareInterface();

    ~ReadWriteHardwareInterface();

    /**
     * 设置里程间隔脉冲计数
     * @param distanceOrNoDistanceMode 脉冲间隔距离，单位cm 
     *                 50 间隔0.5m采集一帧 
     *                 75 间隔0.75m采集一帧 
     *                 100 间隔1m采集一帧 
     *                 1250 间隔1.25m采集一帧
     */
    bool writeCollectModeAndDistanceOrNoConfig(LzCollectingMode collectMode, int distanceOrNoDistanceMode);

    /**
     * 开启读进出洞信号（从接口读）线程
     */
    void startReadThread();

signals:
    // 三个对外信号转发
    // 出洞
    void OutTunnel();
    // 进洞有效，预进洞无效
    void InTunnel(bool isvalid);

    // 状态显示
    void sendMsg(QString);

    void ReadWriteHardwareException(QString str);

private:

   
    void initWrite(int portNum = 4, int bitCount = 3);

    bool writeBits(vector<bool> & bits);
        
private slots:

    //void getReadHardwareChange(bool bit1, bool bit2);
    //void getReadHardwareChange(int s0, bool p0, bool p1, bool p2, bool p3);

    //void getReadHardwareException(QString error);
    void getReadHardwareException(const char *);

    // 三个对外信号转发
    // 出洞
    void slotOutTunnel();
    // 进洞有效，预进洞无效
    void slotInTunnel(bool isvalid);
};

#endif // READ_WRITE_HARDWARE_INTERFACE_H_