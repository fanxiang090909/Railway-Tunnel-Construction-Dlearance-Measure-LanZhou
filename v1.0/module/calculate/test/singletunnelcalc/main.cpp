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

    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码

    std::cout << "单隧道双目计算";

    // 读图像
   	LzSerialStorageAcqui * lzacqui = new LzSerialStorageAcqui();
    QString openfilename = QObject::tr("D://4_虎头崖明洞_20140509_A1.dat");
    //QString openfilename = QObject::tr("3_虎头崖明洞_20140509_B2.dat");

    try {
            
        lzacqui->openFile(openfilename.toLocal8Bit().constData());
        
        DataHead dataread;
        
        // 读文件头
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
            
        qDebug() << QObject::tr("[从控] 从流式文件%1生成临时小图片成功").arg(openfilename.toLocal8Bit().constData());
    }
    catch (LzException & ex)
    {
        qDebug() << QObject::tr("[从控] 从流式文件%1生成临时小图片失败").arg(openfilename.toLocal8Bit().constData());
    }
    delete lzacqui;

	/*LzSerialStorageMat * midcalcresult = new LzSerialStorageMat();
	// 【注意！】设置缓存大小1M，默认1G太大
	midcalcresult->setting(100, 1024*1024, true);

    long mile;
	long framecounter;
	if (!midcalcresult->openFile("范翔计划_20140121_h.dat"))
	{
		qDebug() << "can not open file: 123_h.dat";
		delete midcalcresult;
		return 0;
	}
	qDebug() << "open file: 123_h.dat";

	DataHead dataread;
	// 读文件头
	midcalcresult->readHead(&dataread);
	qDebug() << dataread.carriage_direction  << dataread.is_normal << dataread.tunnel_id << QString::fromLocal8Bit(dataread.tunnel_name) << dataread.date;

	std::vector<BLOCK_KEY> keys = midcalcresult->readKeys();
	qDebug() << "size:" << keys.size();
	for (int i = 0; i < keys.size(); i++)
		qDebug() << keys.at(i);
	//【方案一】未指定key，key从0开始++
	//ret = syn->retrieveMap(0);
	//【方案二】指定key
	midcalcresult = syn->retrieveMap(355);
	qDebug() << "retrive result:" << ret;
	for (int i = 0; i < keys.size(); i++)
	{
		// 在readMap中调用的blocktomap中已调用data.resetMaps();
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
