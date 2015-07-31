//fanxiang
#include <QApplication>

#include <QtCore/QCoreApplication>
#include <QMessageBox>
#include <QTextCodec>
#include <QFile>
#include <QDateTime>
#include <QDebug>

#include "drawimage.h"
#include "clearance_item.h"
#include "xmlsynthesisheightsfileloader.h"

SectionData data1;
SectionData data2;

void LoadFile(QString filename, bool ifcompare)
{
    // Function that reads in ASCII file data

    QFile file;
    QString in;

    file.setFileName(filename);

    // Currently here for debugging purposes
    qDebug() << "Reading file: " << filename.toLocal8Bit().data() << endl;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream inStream(&file);
        
        // readfile
        
        in = inStream.readLine();

        int height;
        int left;
        int right;

        while(!(in.isNull()))
        {
            height = in.section("\t",0,0).toInt();
            left = in.section("\t",1,1).toInt();
            // Reads in fifth column: signal-to-noise ratio
            right = in.section("\t",2,2).toInt();
            
            if (ifcompare)
                data1.updateToMapVals(height, (float)left, (float)right);
            else
                data2.updateToMapVals(height, (float)left, (float)right);

            in = inStream.readLine();
        }

        file.close();

        qDebug() << "read_end" << endl;
    }
    // Do some proper debugging here at some point...
    else 
        qDebug() << "Problem reading file: " << filename.toLocal8Bit().data();

}

int main(int argc, char* argv[])
{
    //cv::Mat img;
    //img = cv::imread("C:\\Users\\Administrator\\Desktop\\0.bmp");
	//cv::cvtColor(img,img, CV_BGR2GRAY);
    //cv::Mat result;
	// 直方图均衡化
    //cv::equalizeHist(img, result);
    //cv::imwrite("C:\\Users\\Administrator\\Desktop\\2.png", result);
    //cv::imshow("win1",result);
    //cv::waitKey(0);

    /**
     * 测试用
     * @author fanxiang
     */
    QApplication a(argc, argv);

    //QCoreApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码

    // 加载本地文件存储目录
    QFile pathfile("slave_datastore_path.txt");
    QString parentpath;
    if (!pathfile.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                                QObject::tr("无文件slave_datastore_path.txt，无法加载高度配置文件"),
                                QMessageBox::Ok);
        return 1;
    }
    else
    {
        parentpath = pathfile.readLine().trimmed();
        pathfile.close();

        XMLSynthesisHeightsFileLoader * ff3 = new XMLSynthesisHeightsFileLoader(parentpath + "/system/output_heights.xml");
        bool ret = ff3->loadFile();
        delete ff3;

    }

    bool ret = data1.initMaps();
    if (!ret)
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                                QObject::tr("SectionData初始化失败"),
                                QMessageBox::Ok);
        return 1;
    }

    ret = data2.initMaps();

    // 加载本地文件存储目录
    QFile pathfile2("section_heights_file_config.txt");
    if (!pathfile2.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                                QObject::tr("无文件section_heights_file_config.txt，无法加载断面提取高度结果数据"),
                                QMessageBox::Ok);
        return 1;
    }
    else
    {
        QString filename1 = pathfile2.readLine().trimmed();
        filename1 = pathfile2.readLine().trimmed();
        qDebug() << filename1;
        LoadFile(filename1, true);
        filename1 = pathfile2.readLine().trimmed();
        data1.setCenterHeight(filename1.toFloat());

        filename1 = pathfile2.readLine().trimmed();
        filename1 = pathfile2.readLine().trimmed();
        qDebug() << filename1;
        LoadFile(filename1, false);
        filename1 = pathfile2.readLine().trimmed();
        data2.setCenterHeight(filename1.toFloat());
    }


    SectionImage imagesection;
    imagesection.show();
    
    imagesection.initPointsArray(data1.getMaps().size(), true);
    imagesection.SectionDataToPointsArray(data1, true);

    imagesection.initPointsArray(data2.getMaps().size(), false);
    imagesection.SectionDataToPointsArray(data2, false);

    imagesection.setFusePointArrayVisible(false);
   
    imagesection.saveImage(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".jpg");
    imagesection.repaint();

    return a.exec();
}
