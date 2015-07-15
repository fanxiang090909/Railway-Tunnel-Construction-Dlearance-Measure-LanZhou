//fanxiang
#include <QApplication>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QMessageBox>
#include "slaveprogram.h"
#include "setting_slave.h"

// 测试用
#include "form.h"
#include "formmanager.h"


int main(int argc, char* argv[])
{
    /*string calibfile = "D:/datastore/定西隧道模型1200+8窄_20141024/calcu_calibration/Trans_Mat_E_20141107.xml";
    loadcalib(calibfile ,0);               //载入标定文件

    Mat P1,P2,R1,R2;
	Mat lcameramat,rcameramat,ldistcoeffs,rdistcoeffs;
	Mat R,T;
  	Mat Trans_R,Trans_T;      //分中转换的旋转，平移矩阵
    Size recsize;

    FileStorage fs("D:/datastore/定西隧道模型1200+8窄_20141024/calcu_calibration/Trans_Mat_E_20141107.xml", FileStorage::READ);

    //FileStorage fs("D:\\datastore\\定西隧道模型1200+8窄_20141024\\calcu_calibration\\Trans_Mat_B_20141107.xml", FileStorage::READ);

    if ( !fs.isOpened() )
    {
        // throw exception;
        std::cout<<"Can't open the Calib_file!";
        return 0;
    }

    fs["P1"] >> P1;
    fs["P2"] >> P2;
    fs["R1"] >> R1;
    fs["R2"] >> R2;
    fs["lcameramat"] >> lcameramat;
    fs["rcameramat"] >> rcameramat;
    fs["ldistcoeffs"] >> ldistcoeffs;
    fs["rdistcoeffs"] >> rdistcoeffs;
    fs["R"] >> Trans_R;
    fs["T"] >> Trans_T;
    fs["recsizewid"] >> recsize.width;
    fs["recsizehei"] >> recsize.height;
    recsize = Size(REC_IMG_SIZE_WID, REC_IMG_SIZE_HEI);

    fs.release();

    cv::Mat img;
    img = cv::imread("D:\\0.bmp");
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

    QApplication::addLibraryPath("./plugins");

    //QCoreApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码

    qDebug()<<"*************************************************";
    qDebug()<<"*                                               *";
    qDebug()<<"*               Slave Program                   *";
    qDebug()<<"*                                               *";
    qDebug()<<"*************************************************";

    // 加载本地文件存储目录
    QFile pathfile("slave_datastore_path.txt");
    if (!pathfile.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                                QObject::tr("无文件slave_datastore_path.txt，无法加载从控数据存储目录位置"),
                                QMessageBox::Ok);
        return 1;
    }
    else
    {
        QString parentpath = pathfile.readLine().trimmed();
        SlaveSetting::getSettingInstance()->setParentPath(parentpath);
		QString myslaveip = pathfile.readLine().trimmed();
		SlaveSetting::getSettingInstance()->setMySlaveIP(myslaveip);
        pathfile.close();
    }

    /**********读取主控IP***********/
    QString network_file = SlaveSetting::getSettingInstance()->getParentPath() + "/system/network_config.xml";
    QFile file1(network_file);
    if (!file1.exists())
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
            QObject::tr("从控机network_config配置文件不存在!") + network_file,
                                QMessageBox::Ok);
        return 1;
    }

    
    // 从控程序默认为连续采集LzCameraCollectingMode::Lz_Camera_Continus;
    // 当在确认有硬触发设备时才采用外触发采集模式采集
    SlaveProgram w = SlaveProgram(LzCameraCollectingMode::Lz_Camera_HardwareTrigger);
    // 配置master_ip和nas_ip
    w.init_NetworkCameraHardwareFile(LzProjectClass::Main, network_file);

    Form f(&w);
    f.show();
    f.showshow();
    //fm.setSlave(&w);
    w.networkConnectInit();

    qDebug() << "endmain!";

    return a.exec();
}
