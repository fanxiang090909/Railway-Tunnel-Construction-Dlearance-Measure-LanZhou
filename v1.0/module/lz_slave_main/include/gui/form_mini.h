#ifndef FORM_MINI_H
#define FORM_MINI_H

#include <QWidget>
#include "slaveprogram_mini.h"

#include <QDir>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QSystemTrayIcon>
#include <QMenu>

namespace Ui {
    class FormMini;
}

class FormMini : public QWidget
{
    Q_OBJECT

public:
    explicit FormMini(SlaveMiniProgram * cvice = NULL, QWidget *parent = 0);
    ~FormMini();

protected:
    // 桌面系统图标相关，防止误操作关闭，可后台运行
    void closeEvent(QCloseEvent *event);

private slots:

    // 桌面系统图标相关，防止误操作关闭，可后台运行
    void setIcon();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void messageClicked();

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

public slots:
    // 更新从机号显示
    void showSlaveID();

private slots:

    /**** TO paste to widget two of masterGUI program ****/
    void appendMsg(QString msg);
    void appendError(QString msg);

    void sendFileButton_clicked();
    void terminateSlave();

private:
    Ui::FormMini *ui;
    SlaveMiniProgram * cvice;

    // 计算时间间隔使用
    QDateTime lastTime;
    int m_time;
};

#endif // FORM_MINI_H
