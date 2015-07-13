#include "formmanager.h"

/**
 * 从控机界面显示管理类定义
 * @author fanxiang
 * @version 1.0.0
 * @date 20140112
 */
FormManager::FormManager(QObject *parent) :
    QObject(parent)
{

}

FormManager::~FormManager()
{
    delete f;
}

void FormManager::setSlave(SlaveProgram* slave)
{
	connect(slave, SIGNAL(showGUI(bool)), this, SLOT(showForm(bool)));
	f = new Form(slave);
	f->show();
	//slave->start();
}

// 已连接主控后对界面的影响
void FormManager::showForm(bool showornot)
{
    qDebug() << "showform" << showornot;
    // 不显示
    if (showornot)
	{
        ;//f->show();
	}
    else
	{
        ;//f->hide();
	}
}
