#include <QtGui/QApplication>
#include <QTextCodec>
#include <QMessageBox>
#include <QTimer>
#include <QSplashScreen>
#include <QDebug>
#include <QTime>
#include <QDateTime>
#include <QFileInfo>
#include <QtGui>

#include "xmlnetworkfileloader.h"
#include "serverthread.h"

#include "xmlcheckedtaskfileloader.h"
#include "xmlsynthesisheightsfileloader.h"
#include "xmltaskfileloader.h"

#include "calcu_noslave.h"
#include "setting_slave.h"
#include "LzCalc_thread.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::addLibraryPath("./plugins");

    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码

    Q_INIT_RESOURCE(icons);

    // 系统图标
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    QSplashScreen *splash = new QSplashScreen();
    splash->setPixmap(QPixmap(":/image/start.png"));
    QTimer::singleShot(3000, splash, SLOT(close()));
    splash->show();
    Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
    splash->showMessage(QObject::tr("正在初始化..."),topRight,Qt::yellow);

    // 加载本地文件存储目录
    QFile file("master_datastore_path.txt");
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                              QObject::tr("无文件master_datastore_path.txt，无法加载主控机数据存储目录位置"),
                              QMessageBox::Ok);
        return 1;
    }
    else
    {
        QString parentpath = file.readLine().trimmed();
        SlaveSetting::getSettingInstance()->setParentPath(parentpath);
        file.close();
        XMLSynthesisHeightsFileLoader * ff3 = new XMLSynthesisHeightsFileLoader(parentpath + "/system/output_heights.xml");
        bool ret3 = ff3->loadFile();
        delete ff3;
        if (!ret3)
        {
            QMessageBox::critical(0, QObject::tr("启动错误"), QObject::tr("无法加载高度配置文件%1").arg(parentpath + "/system/output_heights.xml"), QMessageBox::Ok);
            return 1;
        }
        list<int> Item = OutputHeightsList::getOutputHeightsListInstance()->list();
    }



    // 【标记2】此处作废-因为MasterProgram类的run函数作废
    // 【注意】顺序：先有masterProgramInstance，才run，能连接信号槽
    //MasterProgram::getMasterProgramInstance()->run();
    /*******************************/
    // 【作废】@author 范翔 20141029改为计算备份分开界面
    //MainWidget *w = new MainWidget(true, Calculating_Backuping);
    //w->show();

    /**********读取主控network_config配置***********/

    QString newpath = SlaveSetting::getSettingInstance()->getParentPath() + "/system/network_config.xml";
    QFile file1(newpath);
    if(!file1.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                              QObject::tr("主控机network_config配置文件不存在，") + newpath,
                              QMessageBox::Ok);
        file1.close();
        return 1;
    }
    file1.close();

    CalcuNoSlaveWidget maw(true);
    splash->finish(&maw);
    delete splash;
    maw.show();

    return a.exec();

	// @date 20150626 唐书航调试
    /*    Vector<Point3f> calibed_pnts3d;
        
        LzCalculator Cam_Img;                                                            //生成两组相机计算类 得到两组三维点
        string calibfile = "C:\\Users\\FAN\\Desktop\\Lz_Cali_20141203_E.xml";
        Cam_Img.init(calibfile.c_str());
        Mat img_L,img_R;
        string file_L = "C:\\Users\\FAN\\Desktop\\ef_TEST\\e2\\70150.jpg";
        string file_R = "C:\\Users\\FAN\\Desktop\\ef_TEST\\e1\\70150.jpg";
        img_L = imread(file_L,CV_8UC1);
        img_R = imread(file_R,CV_8UC1);
        Mat _Ledge,_Redge ;
        Cam_Img.calc(img_L,img_R,_Ledge,_Redge);
        if(Cam_Img.is_valid)
        {
            imwrite("C:\\Users\\FAN\\Desktop\\img_L.jpg",_Ledge);
            imwrite("C:\\Users\\FAN\\Desktop\\img_R.jpg",_Redge);
        }
        else
            printf("No imgage");
	*/
}
