#include "lzgrasp.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    //QCoreApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));//����ʾ�����ֲ�����
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));//����ʾ�����ֲ�����
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));//����ʾ�����ֲ�����

    LzGrasp w;
	w.show();
	return a.exec();
}
