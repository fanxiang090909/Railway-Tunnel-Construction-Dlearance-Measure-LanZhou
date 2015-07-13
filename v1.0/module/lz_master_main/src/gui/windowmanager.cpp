#include "windowmanager.h"
#include "ui_login.h"
#include "tabwidgetmanager.h"
#include <QDebug>
#include <QCloseEvent>

/**
 * 主要窗口界面管理类实现
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-08-17
 */
WindowManager::WindowManager(QWidget *parent):QWidget(parent)
{
    lg = new Login;
    //w = new MainWidget;
    t = new TabWidgetManager;
    about = new About;

    // 设置无边框登录窗口
    lg->setWindowOpacity(1);
    lg->setWindowFlags(Qt::FramelessWindowHint);
    lg->setAttribute(Qt::WA_TranslucentBackground);

    lg->show();

    QObject::connect(lg,SIGNAL(enter()),this,SLOT(showWindow()));
    //QObject::connect(w,SIGNAL(logout()),this,SLOT(showLogin()));
    QObject::connect(lg,SIGNAL(help()),this,SLOT(showAbout()));
}

WindowManager::~WindowManager()
{
    //qDebug() << "release ";
    if (lg->isActiveWindow())
        delete lg;
    //if (w->isActiveWindow())
    //    delete w;
    if (t->isActiveWindow())
        delete t;
    if (about->isActiveWindow())
       delete about;
}

void WindowManager::showWindow()
{
    lg->hide();
    //w->show();
    t->show();
}

void WindowManager::showLogin()
{
    //w->close();
    t->close();
    delete lg;
    //delete w;
    delete t;
    delete about;
    lg = new Login;
    //w = new MainWidget;
    about = new About;
    t = new TabWidgetManager;

    //设置无边框登录窗口
    lg->setWindowOpacity(1);
    lg->setWindowFlags(Qt::FramelessWindowHint);
    lg->setAttribute(Qt::WA_TranslucentBackground);
    lg->show();

    QObject::connect(lg,SIGNAL(enter()),this,SLOT(showWindow()));
    //QObject::connect(w,SIGNAL(logout()),this,SLOT(showLogin()));
    QObject::connect(lg,SIGNAL(help()),this,SLOT(showAbout()));
}

void WindowManager::showAbout()
{
    //设置无边框登录窗口
    about->setWindowTitle(tr("隧道建筑限界测量系统"));
    about->setWindowOpacity(1);
    about->setWindowFlags(Qt::FramelessWindowHint);
    about->setAttribute(Qt::WA_TranslucentBackground);

    about->show();
}