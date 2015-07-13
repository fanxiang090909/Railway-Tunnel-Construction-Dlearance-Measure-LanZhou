#include <iostream>

#include "headdefine.h"
#include "LzException.h"
#include <QDebug>

#include "LzSerialStorageMat.h"
#include "LzSerialStorageAcqui.h"
#include <opencv2\core\core.hpp>
#include <sstream>

#include <QTextCodec>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{

    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));//����ʾ�����ֲ�����
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));//����ʾ�����ֲ�����
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));//����ʾ�����ֲ�����

    std::cout << "�����˫Ŀ����";

    // ��ͼ��
   	LzSerialStorageAcqui * lzacqui = new LzSerialStorageAcqui();
    QString openfilename = QObject::tr("D://4_��ͷ������_20140509_A1.dat");
    //QString openfilename = QObject::tr("3_��ͷ������_20140509_B2.dat");

    try {
            
        lzacqui->openFile(openfilename.toLocal8Bit().constData());
        
        DataHead dataread;
        
        // ���ļ�ͷ
        //lzacqui->readHead(&dataread);
        //qDebug() << dataread.carriage_direction  << dataread.is_normal << dataread.tunnel_id << QString::fromLocal8Bit(dataread.tunnel_name) << dataread.date;
        
     	std::vector<BLOCK_KEY> keys = lzacqui->readKeys();
	    qDebug() << "size:" << keys.size();
	    for (int i = 0; i < keys.size(); i++)
		    qDebug() << keys.at(i);

        cv::Mat m;

        if ( !lzacqui->retrieveFrame(123) )
			return false;

        while (lzacqui->readFrame(m));
        {
            std::stringstream sstr;
            //cv::imshow("win1", m);
			cv::imwrite("D://1.jpg", m);
        }
        lzacqui->closeFile();
            
        qDebug() << QObject::tr("[�ӿ�] ����ʽ�ļ�%1������ʱСͼƬ�ɹ�").arg(openfilename.toLocal8Bit().constData());
    }
    catch (LzException & ex)
    {
        qDebug() << QObject::tr("[�ӿ�] ����ʽ�ļ�%1������ʱСͼƬʧ��").arg(openfilename.toLocal8Bit().constData());
    }
    delete lzacqui;

	/*LzSerialStorageMat * midcalcresult = new LzSerialStorageMat();
	// ��ע�⣡�����û����С1M��Ĭ��1G̫��
	midcalcresult->setting(100, 1024*1024, true);

    long mile;
	long framecounter;
	if (!midcalcresult->openFile("����ƻ�_20140121_h.dat"))
	{
		qDebug() << "can not open file: 123_h.dat";
		delete midcalcresult;
		return 0;
	}
	qDebug() << "open file: 123_h.dat";

	DataHead dataread;
	// ���ļ�ͷ
	midcalcresult->readHead(&dataread);
	qDebug() << dataread.carriage_direction  << dataread.is_normal << dataread.tunnel_id << QString::fromLocal8Bit(dataread.tunnel_name) << dataread.date;

	std::vector<BLOCK_KEY> keys = midcalcresult->readKeys();
	qDebug() << "size:" << keys.size();
	for (int i = 0; i < keys.size(); i++)
		qDebug() << keys.at(i);
	//������һ��δָ��key��key��0��ʼ++
	//ret = syn->retrieveMap(0);
	//����������ָ��key
	midcalcresult = syn->retrieveMap(355);
	qDebug() << "retrive result:" << ret;
	for (int i = 0; i < keys.size(); i++)
	{
		// ��readMap�е��õ�blocktomap���ѵ���data.resetMaps();
		ret = syn->readMap(mile, framecounter, data);
		if (ret == false)
			continue;
		qDebug() << "read " << i << "times. return:" << ret;
		qDebug() << "mile:" << mile << ", framecounter:" << framecounter;
		data.showMaps();
	}
	midcalcresult->closeFile();

	delete midcalcresult;
	
	midcalcresult = NULL;
    */

    return 0;
}
