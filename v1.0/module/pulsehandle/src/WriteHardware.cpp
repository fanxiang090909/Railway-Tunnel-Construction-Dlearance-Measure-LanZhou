
#include "WriteHardware.h"

WriteHardware::WriteHardware(int portNum, int bitCount)
	: m_nPortNum(portNum), m_nBitCount(bitCount)
{
	instantDoCtrl = AdxInstantDoCtrlCreate();
}

WriteHardware::~WriteHardware()
{
	instantDoCtrl->Dispose();
}

void WriteHardware::init()
{
	DeviceInformation devInfo(deviceDescription);
	ret = instantDoCtrl->setSelectedDevice(devInfo);
	if( ret != Success)
	{
		throw LzException(2, "initialize hardware failed");
	}
	// 设置该端口为输出模式
	ICollection<PortDirection>* portDirection = instantDoCtrl->getPortDirection();
	portDirection->getItem(m_nPortNum).setDirection(Output);
}

void WriteHardware::write(byte bfw)
{
	ret = instantDoCtrl->Write(m_nPortNum, PORTCOUNT, &bfw);
	if(ret != Success)
	{
		throw std::exception("write hardware failed");
	}
}

void WriteHardware::write(const vector<bool> & bits) 
{
	byte byteForWriting = 0;
	for(int i = 0; i < m_nBitCount; i++)
	{	
		if(  i == bits.size() )
			break;
		if( bits[i] )
			byteForWriting |= (1 << i);		
	}
	write(byteForWriting);
}

// read back
vector<bool> WriteHardware::read() 
{
	byte bufferForReading;
	ret = instantDoCtrl->Read(m_nPortNum,PORTCOUNT,&bufferForReading);
	if( ret != Success)
	{
		throw LzException(2, "read hardware failed");
	}
	qDebug() << bufferForReading;
	vector<bool> ret;
	for(int i = 0; i < m_nBitCount; i++)
	{
		ret.push_back(bufferForReading&0x1);
		bufferForReading >>= 1;
	}
	/*for(int i =0; i < m_nBitCount; i++)
		qDebug() << ret[i];*/
	return ret;
}