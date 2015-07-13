//fanxiang
#include <QApplication>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QMessageBox>
#include <QTextCodec>
#include <QFile>

// 测试用
#include "view_aquizfile.h"

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

    std::string output_file = "D:\\datastore\\定西隧道模型1200+8窄_20141024\\collect\\1_隧道模型1200_20141024_B2.tmpdat";
    QString tmpimgdir = QObject::tr("D:\\datastore\\定西隧道模型1200+8窄_20141024\\tmp_img\\1_隧道模型1200_20141024_B2\\");
    DataHead datahead;
    LzSerialStorageAcqui *lzserialimg = new LzSerialStorageAcqui();
    lzserialimg->createFile(output_file.c_str(), sizeof(DataHead));
    bool output_isopen = lzserialimg->openFile(output_file.c_str());
    lzserialimg->writeHead(&datahead);

    QString tmpimgfiledir; 
    for (int frameno = 0; frameno <= 72; frameno++)
    {
        
        tmpimgfiledir = tmpimgdir + QString("%1.png").arg(frameno);
        qDebug() << tmpimgfiledir;
        QByteArray tmpba = tmpimgfiledir.toLocal8Bit();
        cv::Mat tmpimg = cv::imread(tmpba.constData(), cv::IMREAD_GRAYSCALE);
        lzserialimg->writeMat(tmpimg);
    }
    lzserialimg->closeFile();
    delete lzserialimg;

    return a.exec();
}
