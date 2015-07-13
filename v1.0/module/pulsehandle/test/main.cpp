#include <iostream>

#include <QApplication>
#include "ReadWriteHardwareInterface.h"

void test()
{
    WriteHardware writeHard(4,2);
    ReadHardware  readHard(5,2);
    try{
        writeHard.init();
        readHard.init();
    }
    catch(std::exception &e)
    {
        qDebug() << e.what();
        return;
    }
    vector<bool> bits = writeHard.read();
    for(int i = 0; i < 2; i++)
        qDebug() << bits[i];
    bits[0] = 1;
    bits[1] = 0;
    writeHard.write(bits);
    bits = readHard.read();
    for(int i = 0; i < 2; i++)
        qDebug() << bits[i];
    bits = writeHard.read();
    for(int i = 0; i < 2; i++)
        qDebug() << bits[i];
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //test();

    ReadWriteHardwareInterface * interface0 = new ReadWriteHardwareInterface();
    //connect(interface0, SIGNAL(OutTunnel), this, SLOT(/*TODO*/));
    //connect(interface0, SIGNAL(InTunnel_Valid), this, SLOT(/*TODO*/));
    //connect(interface0, SIGNAL(InTunnel_NotValid), this, SLOT(/*TODO*/));
    
    interface0->startReadThread();

    return a.exec();
}
