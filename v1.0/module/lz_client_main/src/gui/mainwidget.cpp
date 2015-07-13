#include "mainwidget.h"
#include "ui_mainwidget.h"

#include "tabwidgetmanager.h"
#include "widgetheader.h"
#include <QMessageBox>
#include <QCloseEvent>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("铁路隧道建筑限界测量系统-数据操作终端"));

    QIcon icon;
    icon.addFile(QString::fromUtf8(":image/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);

    TabWidgetManager * tabWidgetManager = new TabWidgetManager(ui->keyWeight);
    // 关闭窗口信号槽，告知数据库可能需要备份
    connect(this, SIGNAL(my_close()), tabWidgetManager, SLOT(my_close_slot()));

    QGridLayout *layout1 = new QGridLayout();
    layout1->addWidget(tabWidgetManager);
    ui->keyWeight->setLayout(layout1);

    // 桌面系统图标相关，防止误操作关闭，可后台运行
    createActions();
    createTrayIcon();

    setIcon();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    trayIcon->show();
}

MainWidget::~MainWidget()
{
    delete ui;
}

// 桌面系统图标相关，防止误操作关闭，可后台运行
//! [2]
void MainWidget::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("提示"),
                                 tr("程序将在后台运行，终止程序请点击桌面右下小图标，选择退出！"));
        emit my_close();
        //hide();
        event->ignore();
    }
}
//! [2]

//! [3]
void MainWidget::setIcon()
{
    QIcon icon = QIcon(":/image/icon.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
    trayIcon->setToolTip(tr("铁路隧道建筑限界测量系统--数据操作终端"));
    setWindowTitle(tr("铁路隧道建筑限界测量系统--数据操作终端"));
}
//! [3]

//! [4]
void MainWidget::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        //iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1)
        //                              % iconComboBox->count());
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        ;
    }
}
//! [4]

//! [5]
void MainWidget::showMessage()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(0);
    trayIcon->showMessage("aaa", "bbb", icon, 1000);
}
//! [5]

//! [6]
void MainWidget::messageClicked()
{
    QMessageBox::information(0, tr("提示"),
                                tr("Sorry, I already gave what help I could.\n"
                                   "Maybe you should try asking a human?"));
}
//! [6]

void MainWidget::createActions()
{
    minimizeAction = new QAction(tr("最小化"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("最大化"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("显示"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("退出"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWidget::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}
