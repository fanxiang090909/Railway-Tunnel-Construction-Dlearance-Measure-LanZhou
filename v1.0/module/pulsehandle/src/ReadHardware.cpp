
#include "ReadHardware.h"

ReadHardware::ReadHardware(int portNum, int bitCount)
	: m_nPortNum(portNum), m_nBitCount(bitCount)
{
	bits.resize(bitCount);
	instantDiCtrl = AdxInstantDiCtrlCreate();
}

ReadHardware::~ReadHardware()
{
	instantDiCtrl->Dispose();
}

void ReadHardware::init() 
{
	// select a device by device number or device description and specify the access mode.
	DeviceInformation devInfo(deviceDescription);
	ret = instantDiCtrl->setSelectedDevice(devInfo);
	if( ret != Success)
	{
		throw LzException(1, "initialize hardware failed");
	}
	// 设置该端口为输入模式
	ICollection<PortDirection>* portDirection = instantDiCtrl->getPortDirection();
	portDirection->getItem(m_nPortNum).setDirection(Input);
}

vector<bool> ReadHardware::read() 
{
	ret = instantDiCtrl->Read(m_nPortNum, PORTCOUNT, &bufferForReading);
	if( ret != Success)
	{
		throw LzException(1 ,"read hardware failed");
	}
	//qDebug() << bufferForReading;
	for(int i = 0; i < m_nBitCount; i++)
	{
		bits[i] = bufferForReading & 0x1;
		bufferForReading >>= 1;
	}
	return bits;
}