#include "ReadHardwareThread.h"
#include <iostream>

ReadHardwareThread::ReadHardwareThread()
{
	stopped = false;
	bitsForReading.resize(READ_BIT_COUNT);
	//bitsForReading[1] = 1;   // default M1 = 1
	
	try {
		pReadHardware = new ReadHardware(READ_PORT_NUM,READ_BIT_COUNT);
	} catch(std::exception)
	{
		pReadHardware = NULL;
		qDebug() << "11";
	}
    // 默认出洞
    last_status = CONFIRM_OUT_;
    last_status_time = QTime::currentTime();
}

ReadHardwareThread::ReadHardwareThread(int portNum, int bitCount)
{
	stopped = false;
	bitsForReading.resize(bitCount);
	pReadHardware = new ReadHardware(portNum,bitCount);
}

ReadHardwareThread::~ReadHardwareThread()
{
	if( pReadHardware )
	delete pReadHardware;
}

bool ReadHardwareThread::isChanged(const vector<bool>& bits, int num)
{
	num = min(num, bits.size());
	for(int i = 0; i < num ; i++)
		if(bitsForReading[i] != bits[i])  // different
			return true;
	return false;
}


void ReadHardwareThread::init()
{
	try
	{
		pReadHardware->init();
	}
	catch(std::exception &e)
	{
		throw e;
	}
}

void ReadHardwareThread::run() 
{	
	vector<bool> bits;
	while( true )
	{
		mutex.lock();
		if(stopped)
		{
			stopped = false;
			mutex.unlock();
			break;
		}
		mutex.unlock();
		//从硬件读取
		try
		{
			bits = pReadHardware->read();
		}
		catch(std::exception &e)
		{
			//qDebug() << QString(e.what());
			emit sendReadException(e.what());
			return;
		}
		
		if( isChanged(bits) )
		{
			bitsForReading = bits;
			emit sendChange(bitsForReading[0], bitsForReading[1]);  
			checkInOrOutTunnle(bitsForReading[0], bitsForReading[1]);            
            //qDebug()<<"send, mode changed";
		}
				
		QThread::msleep(10);  
	}
}

void ReadHardwareThread::stop()
{
	mutex.lock();
	stopped = true;
	mutex.unlock();
}

/**
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
 *  输出三个信号：
 *     1)  预进洞有效（即真正进洞）emit InTunnel(true);
 *     2)  预进洞无效              emit InTunnel(false);
 *     3)  出洞                    emit OutTunnel();
 */
bool ReadHardwareThread::checkInOrOutTunnle(bool p0, bool p1)
{
    IN_OUT_Tunnel newstatus;

    // 如果时间太频繁，不改变状态
    // TODO
    // 判断新状态，p0和p1两位
    if (!p0 && !p1)
        newstatus = CONFIRM_OUT_;
    else if (!p0 && p1)
        newstatus = UNKNOWN_;
    else if (p0 && !p1)
        newstatus = PRE_IN_;
    else
        newstatus = CONFIRM_IN_;

    // 检查新状态是否有效
    // 出洞->进洞【预进洞有效】，出洞->预进洞
    if (last_status == CONFIRM_OUT_ && newstatus == CONFIRM_IN_)
    {
        emit InTunnel(true);
    }
    // 进洞->出洞【出洞】
    else if (last_status == CONFIRM_IN_ && newstatus == CONFIRM_OUT_)
    {
        emit OutTunnel();
    }
    // 预进洞->进洞【预进洞有效】，预进洞->出洞【预进洞无效】
    else if (last_status == PRE_IN_ && newstatus == CONFIRM_IN_)
    {
        emit InTunnel(true);
    }
    else if (last_status == PRE_IN_ && newstatus == CONFIRM_OUT_)
    {
        emit InTunnel(false);
    }
    else if (last_status == CONFIRM_OUT_ && newstatus == PRE_IN_)
    {
        last_status = newstatus;
        return true;
    }
    else
    {
        emit sendReadException(tr("error status %1").arg(newstatus).toLocal8Bit().constData());
        return false;
    }

    last_status = newstatus;
    return true;
}