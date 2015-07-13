#include "ReadWriteHardwareInterface.h"

/**
 * 读写硬件接口，IO模式读写接口卡数据类接口
 * @author 范翔
 * @date 20150525
 * @version 1.0.0
 */
ReadWriteHardwareInterface::ReadWriteHardwareInterface()
{
    r = NULL;
    initread = false;
    w = NULL;
    initwrite = false;
    r = new ReadHardwareThread();

    // 写4号端口，写四位
    initWrite(4, 8);
}

ReadWriteHardwareInterface::~ReadWriteHardwareInterface()
{
    if (r != NULL)
        delete r;
    initread = false;
    if (w != NULL)
        delete w;
    initwrite = false;
}
    
bool ReadWriteHardwareInterface::writeCollectModeAndDistanceOrNoConfig(LzCollectingMode collectMode, int distanceOrNoDistanceMode)
{
    if (!initwrite)
        return false;

    vector<bool> bits;
    bits.clear();

    switch (collectMode)
    {
        case Lz_Collecting_Manual_DistanceMode:     // 0 0
            bits.push_back(false); bits.push_back(false);
            break;
        case Lz_Collecting_Automatic_DistanceMode:  // 0 1
            bits.push_back(false); bits.push_back(true);
            break;
        case Lz_Collecting_FreeMode_NoDistanceMode: // 1 0
            bits.push_back(true); bits.push_back(false);
            break;
        default:                                    // 1 1
            bits.push_back(true); bits.push_back(true);
    }

    switch (distanceOrNoDistanceMode)
    {
        case 0: // 0 0
            bits.push_back(false); bits.push_back(false);
            break;
        case 1: // 0 1
            bits.push_back(false); bits.push_back(true);
            break;
        case 2: // 1 0
            bits.push_back(true); bits.push_back(false);
            break;
        case 3: // 1 1 
            bits.push_back(true); bits.push_back(true);
            break;
        default: // 1 0
            bits.push_back(true); bits.push_back(false);
    }
    return writeBits(bits);
}
    
void ReadWriteHardwareInterface::startReadThread()
{
    try 
    {
        //connect(r, SIGNAL(sendChange(bool, bool)), this, SLOT (getReadHardwareChange(bool, bool)));
        connect(r, SIGNAL(sendReadException(const char *)), this, SLOT(getReadHardwareException(const char *)));
        connect(r, SIGNAL(InTunnel(bool)), this, SLOT(slotInTunnel(bool)));
        connect(r, SIGNAL(OutTunnel()), this, SLOT(slotOutTunnel()));

        r->init();
        r->start();
    }
    catch (std::exception & e)
    {
        qDebug() << "exception:" << e.what();
        emit ReadWriteHardwareException(QString::fromLocal8Bit(e.what()));
    }
}

void ReadWriteHardwareInterface::initWrite(int portNum, int bitCount)
{
   	try 
    {
        if (w != NULL)
            delete w;
        initwrite = false;
        // 构造函数参数表示，写哪几位
        w = new WriteHardware(portNum, bitCount);
        w->init();
    }
    catch (std::exception & e)
    {
        initwrite = false;
        qDebug() << QString("init Write Exception: %1").arg(e.what());
        return;
    }
    initwrite = true;
}

bool ReadWriteHardwareInterface::writeBits(vector<bool> & bits)
{
    try 
    {
        if (w != NULL);
            w->write(bits);
    }
    catch (std::exception & e)
    {
        qDebug() << QString("Write Exception: %1").arg(e.what());
        return false;
    }
    return true;
}

/*void ReadWriteHardwareInterface::getReadHardwareChange(bool bit1, bool bit2)
{
}*/

void ReadWriteHardwareInterface::getReadHardwareException(char const * error)
{
    qDebug() << "error:" << error;
    emit ReadWriteHardwareException(QString::fromLocal8Bit(error));
}

// 出洞
void ReadWriteHardwareInterface::slotOutTunnel()
{
    emit OutTunnel();
}

// 进洞有效，预进洞无效
void ReadWriteHardwareInterface::slotInTunnel(bool isvalid)
{
    emit InTunnel(isvalid);
}