/************************************************************************
*ReadHardwareThread.h
* @brief 
*   thread for read bits from USB-4751
* @params
*    the port's number and bits to read
* @author   Yike    
*          2015.06.10
*****************************************************************/
#ifndef READHARDWARETHREAD_H
#define READHARDWARETHREAD_H

#include <QtCore\QThread>
#include <QtCore\QDebug>
#include <QtCore\QMutex>
#include <QTime>

#include "ReadHardware.h"

#define  READ_PORT_NUM    5
#define  READ_BIT_COUNT   8

    
/**
 * 预进洞状态  00 
 * 进洞状态    01
 * 出洞状态    10
 * 预留状态    11
 */
enum IN_OUT_Tunnle
{
    PRE_IN_ = 0,
    CONFIRM_IN_ = 1,
    CONFIRM_OUT_ = 2,
    UNKNOWN_ = 3
};

class ReadHardwareThread : public QThread
{
	Q_OBJECT
public:
	ReadHardwareThread();
	ReadHardwareThread(int portNum, int bitCount = READ_BIT_COUNT);
	~ReadHardwareThread();
	void init();
	bool isChanged(const vector<bool>&, int num = READ_BIT_COUNT);
	void stop();
protected:

	void run();

     /**
     * 进出洞信号检验（从接口读）
     *   
     * 状态转换详见《郑老师软硬件接口定义v2.0.ppt》
     *  自动模式下：                  手动/自由触发模式下：
     *
     *            _ 进洞 _                    进洞 _
     *           /        \                     \   \
     *          / ________ \                     \   \
     *         / /        \ \                     \   \
     *       预进洞        出洞                    \_出洞
     *           \_________/
     *
     */
    bool checkInOrOutTunnle(bool p0, bool p1);

private:
	vector<bool>     bitsForReading;
	ReadHardware*    pReadHardware;     // pointer of readHardware
	QMutex           mutex;
	volatile bool    stopped;

    IN_OUT_Tunnle last_status;

    QTime last_status_time;

    QMutex mutex2;
signals:
	void sendChange(bool, bool);
	void sendReadException(const char *);

    // 三个对外信号转发
    // 出洞
    void OutTunnel();
    // 进洞有效，预进洞无效
    void InTunnel(bool isvalid);
};

#endif