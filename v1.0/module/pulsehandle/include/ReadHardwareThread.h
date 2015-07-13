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
 * Ԥ����״̬  00 
 * ����״̬    01
 * ����״̬    10
 * Ԥ��״̬    11
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
     * �������źż��飨�ӽӿڶ���
     *   
     * ״̬ת�������֣��ʦ��Ӳ���ӿڶ���v2.0.ppt��
     *  �Զ�ģʽ�£�                  �ֶ�/���ɴ���ģʽ�£�
     *
     *            _ ���� _                    ���� _
     *           /        \                     \   \
     *          / ________ \                     \   \
     *         / /        \ \                     \   \
     *       Ԥ����        ����                    \_����
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

    // ���������ź�ת��
    // ����
    void OutTunnel();
    // ������Ч��Ԥ������Ч
    void InTunnel(bool isvalid);
};

#endif