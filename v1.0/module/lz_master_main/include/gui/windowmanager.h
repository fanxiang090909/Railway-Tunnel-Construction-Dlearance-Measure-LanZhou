#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include "login.h"
#include "about.h"
#include "mainwidget.h"
#include "tabwidgetmanager.h"

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>

/**
 * 主要窗口界面管理类声明
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-08-17
 */
class WindowManager: public QWidget
{
    Q_OBJECT
public:
    explicit WindowManager(QWidget *parent= 0);
    ~WindowManager();

private slots:
    // 切换界面，界面控制
    void showLogin();
    void showWindow();
    void showAbout();

private:
    // 切换界面，界面控制
    Login *lg;
    //MainWidget *w;
    About *about;
    TabWidgetManager *t;



};
#endif // WINDOWMANAGER_H
