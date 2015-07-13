/****************************************************************
* @brief 
*    write bits to USB-4751, then M_PC or machine A read it.
* @params
*    the port's number and bits to write
* @author   Yike    
*          2015.06.10
*****************************************************************/

#ifndef WRITEHARDWARE_H
#define WRITEHARDWARE_H

#include <QtCore\QDebug>
#include <vector>

#include "bdaqctrl.h"
#include "compatibility.h"
#include "LzException.h"

#define  deviceDescription  L"USB-4751,BID#0"
#define PORTCOUNT  1    // 每次写一个端口
#define BITCOUNT   8

using namespace Automation::BDaq;
using std::vector;
typedef unsigned char byte;

class WriteHardware
{
public:
	/* initialize with port-number, and it's bits to write(1~8), default 8  */
	WriteHardware(int portNum, int bitCount = BITCOUNT);
	~WriteHardware();

	void write(const vector<bool>&);
	void write(byte byteForWriting);
	vector<bool> read();
	void init() throw(std::exception);

private:
	ErrorCode         ret;
	InstantDoCtrl *   instantDoCtrl;
	int               m_nPortNum;
	int               m_nBitCount;
};

#endif
