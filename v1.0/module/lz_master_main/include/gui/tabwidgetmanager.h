#ifndef WIDGETMANAGER_H
#define WIDGETMANAGER_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QObject>
#include <QPushButton>

#include "mainwidget.h"

namespace Ui {
    class TabWidgetManager;
}

/**
 * 左侧按钮切换页面tabwidget界面类声明
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-04-17
 */
class TabWidgetManager : public QWidget
{
    Q_OBJECT

public:
    explicit TabWidgetManager(QWidget *parent = 0);
    ~TabWidgetManager();

private slots:

    void button1Clicked();
    void button2Clicked();
    void button3Clicked();
    void button4Clicked();
    void button5Clicked();
    void button6Clicked();
    void button7Clicked();

    // 转变TabButton格式
    void setTabButtonStyle(int i);

private:
    Ui::TabWidgetManager *ui;

protected:
    // 桌面系统图标相关，防止误操作关闭，可后台运行
    void closeEvent(QCloseEvent *event);

private slots:

    // 桌面系统图标相关，防止误操作关闭，可后台运行
    void setIcon();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void messageClicked();

    // collectWidgetButton , calcuWidgetButton , backupWidgetButton 点击事件槽函数
    void showCollect();
    void showCalcu();
    void showBackup();

private:
    // 桌面系统图标相关，防止误操作关闭，可后台运行
    void createActions();
    void createTrayIcon();

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    // 监控界面是否初始化
    bool hasinitcollect;
    bool hasinitcalcu;
    bool hasinitbackup;

    MainWidget * collectwidget;
    MainWidget * calcuwidget;
    MainWidget * backupwidget;

};

#endif // WIDGETMANAGER_H
