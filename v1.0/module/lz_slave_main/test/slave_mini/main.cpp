//fanxiang
#include <QApplication>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QMessageBox>
#include "slaveprogram_mini.h"
#include "setting_slave_mini.h"

// 测试用
#include "form_mini.h"

int main(int argc, char* argv[])
{
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
    qDebug()<<"*             Slave Mini Program                *";
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
        SlaveMiniSetting::getSettingInstance()->setParentPath(parentpath);
		QString myslaveip = pathfile.readLine().trimmed();
		SlaveMiniSetting::getSettingInstance()->setMySlaveIP(myslaveip);
        QString slaveminiexepath = pathfile.readLine().trimmed();
        SlaveMiniSetting::getSettingInstance()->setSlaveEXEPath(slaveminiexepath);
        QString slavestartbatpath = pathfile.readLine().trimmed();
        SlaveMiniSetting::getSettingInstance()->setSlaveStartupBATPath(slavestartbatpath);
        pathfile.close();
    }

    /**********读取主控IP***********/
    QString network_file = SlaveMiniSetting::getSettingInstance()->getParentPath() + "/system/network_config.xml";
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
    SlaveMiniProgram * w = new SlaveMiniProgram(SlaveMiniSetting::getSettingInstance()->getSlaveEXEPath(), SlaveMiniSetting::getSettingInstance()->getSlaveStartupBATPath());
    //SlaveMiniProgram w = SlaveMiniProgram();
    
    // 配置master_ip和nas_ip
    w->init_NetworkCameraHardwareFile(network_file);

    FormMini f(w);
    f.show();
    //fm.setSlave(&w);
    w->networkConnectInit();

    qDebug() << "endmain!";

    return a.exec();
}
