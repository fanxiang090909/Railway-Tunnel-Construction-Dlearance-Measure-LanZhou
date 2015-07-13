#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include "serverprogram.h"
#include <QStringListModel>


namespace Ui {
    class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();

protected:
    // 桌面系统图标相关，防止误操作关闭，可后台运行
    void closeEvent(QCloseEvent *event);

private slots:

    /**** TO paste to widget two of masterGUI program ****/
    void appendMsg(QString msg);
    void appendError(QString msg);

    /**** TO paste to widget two of masterGUI program ****/
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    /**
     * 更新当前连接IP列表
     * @param ip ip地址
     * @param addorremove 增加到iplist还是从iplist中删除，true为增加，false为删除
     */
    void updateIPListView(QString ip, bool addorremove);

    /**
     * 更新正在编辑工程列表
     */
    void updateEditingProjectListView();

    // 桌面系统图标相关，防止误操作关闭，可后台运行
    void setIcon();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void messageClicked();

    void on_iplistView_clicked(const QModelIndex &index);

private:

    /**
     * 将Ip地址队列转换为界面显示的listModel
     */
    QStringListModel * iplistmodel;
    /**
     * 将Ip地址队列
     */
    QStringList iplist;

    /**
     * 正在编辑隧道显示的listmodel
     */
    QStringListModel * editinglistmodel;

    // 桌面系统图标相关，防止误操作关闭，可后台运行
    void createActions();
    void createTrayIcon();

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    /**
     * 要发送至的clientip地址
     */
    QString toclientip;

    Ui::Form *ui;

};

#endif // FORM_H
