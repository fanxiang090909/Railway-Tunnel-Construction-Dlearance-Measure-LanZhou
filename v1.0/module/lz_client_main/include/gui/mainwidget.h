#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>

namespace Ui {
    class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

protected:
    // 桌面系统图标相关，防止误操作关闭，可后台运行
    void closeEvent(QCloseEvent *event);

signals:
    void my_close();

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

private:
    Ui::MainWidget *ui;
};

#endif // MAINWIDGET_H
