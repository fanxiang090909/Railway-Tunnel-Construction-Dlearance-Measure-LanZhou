#include <QtGui/QApplication>
#include <QMessageBox>
#include <QFile>

#include "setting_server.h"
#include "form.h"
#include "serverprogram.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::addLibraryPath("./plugins");

    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));//让显示的文字不乱码

    // 加载本地文件存储目录
    QFile pathfile("server_datastore_path.txt");
    if (!pathfile.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0, QObject::tr("启动错误"),
                              QObject::tr("无文件client_datastore_path.txt，无法加载客户端数据存储目录位置"),
                              QMessageBox::Ok);
        return 1;
    }
    else
    {
        QString parentpath = pathfile.readLine().trimmed();
        ServerSetting::getSettingInstance()->setParentPath(parentpath);
        QString nasaddress = pathfile.readLine().trimmed();
        ServerSetting::getSettingInstance()->setNASIPAddress(nasaddress);
        pathfile.close();
    }

    Form f;
    f.show();

    return a.exec();
}
