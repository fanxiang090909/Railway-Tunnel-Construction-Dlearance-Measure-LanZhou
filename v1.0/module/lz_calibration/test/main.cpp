#include "lz_calibration.h"
#include <QtGui/QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    QApplication::addLibraryPath("./plugins");

    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));//����ʾ�����ֲ�����
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));//����ʾ�����ֲ�����
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));//����ʾ�����ֲ�����

	LZ_Calibration w;
	w.show();
	return a.exec();
}

