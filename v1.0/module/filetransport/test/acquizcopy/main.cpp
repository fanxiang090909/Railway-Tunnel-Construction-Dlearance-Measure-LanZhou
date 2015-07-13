//fanxiang
#include <QApplication>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QMessageBox>
#include <QTextCodec>
#include <QFile>

#include "fileoperation_lzserial.h"

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
                                QObject::tr("无文件slave_datastore_path.txt，无法加载从控数据存储目录位置"),
                                QMessageBox::Ok);
        return 1;
    }
    else
    {
        parentpath = pathfile.readLine().trimmed();
        pathfile.close();
    }

    LzSerialStorageBackupThread * thread = new LzSerialStorageBackupThread();
    thread->initCopy(QObject::tr("D:\\datastore\\测试定西兰州_20141121\\collect\\3_定西兰州测试隧道3_20141121_A1.dat"), 
                    QObject::tr("D:\\collect\\3_定西兰州测试隧道3_20141121_A1.dat"), false, false, 0);
    thread->start();

    return a.exec();
}
