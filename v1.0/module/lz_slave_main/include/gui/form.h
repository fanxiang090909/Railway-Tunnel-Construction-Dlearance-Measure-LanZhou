#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "slaveprogram.h"

#include <QDir>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QSystemTrayIcon>
#include <QMenu>

namespace Ui {
    class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(SlaveProgram * cvice, QWidget *parent = 0);
    ~Form();

    void showshow();

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
    
    // 对相机采集提示灯的改变
    void turnoffalllight();
    void changeCollectCameraLight(QString, HardwareStatus);

private slots:

    // 采集计算状态
    void startCollect(QString, QString, QString, QString, QString, QString, QString, QString);
    void fcupdate(WorkingStatus, int, QString, long long);
    //void receiveStatusShow(qint64, int, QString);
    void initGroupBox(WorkingStatus);

    void on_pushButton_clicked();

    /**** TO paste to widget two of masterGUI program ****/
    void appendMsg(QString msg);
    void appendError(QString msg);

    /**** TO paste to widget two of masterGUI program ****/
    /**
     * 解析消息字符串
     */
    //void parseMsg(QString msg);
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_18_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_23_clicked();

    void on_pushButton_24_clicked();

    void on_pushButton_25_clicked();

    void on_pushButton_26_clicked();

    void on_pushButton_27_clicked();

    void on_pushButton_28_clicked();

    void on_pushButton_29_clicked();

    void on_pushButton_30_clicked();

    void on_pushButton_31_clicked();

    void on_pushButton_32_clicked();

    void on_pushButton_33_clicked();

    void on_pushButton_34_clicked();

    void on_pushButton_35_clicked();

private:
    Ui::Form *ui;
    SlaveProgram * cvice;

    // 计算时间间隔使用
    QDateTime lastTime;
    int m_time;
};

#endif // FORM_H
