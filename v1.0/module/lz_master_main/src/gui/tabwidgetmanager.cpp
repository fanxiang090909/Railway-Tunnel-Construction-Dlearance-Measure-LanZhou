#include "tabwidgetmanager.h"
#include "ui_tabwidgetmanager.h"

#include <QBoxLayout>
#include <QMessageBox>
#include <QCloseEvent>

#include "hardware_config.h"
//不同界面。。。的头文件
#include "create_plan_task.h"
#include "check_task.h"
#include "create_project.h"

/**
 * 左侧按钮切换页面tabwidget界面类实现
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-04-17
 */
TabWidgetManager::TabWidgetManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabWidgetManager)
{
    ui->setupUi(this);
    //setFixedSize(919,653);//固定界面的大小
    QIcon icon;
    icon.addFile(QString::fromUtf8(":image/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);
    // qDebug()<<this->size();

    // 监控界面Boolean型初始化设置
    hasinitcollect = false;
    hasinitcalcu = false;
    hasinitbackup = false;

    // 【注意】，此处添加widget为了是窗口布局随大窗口缩放而改变
    // 【注意！】ui->gridLayout为自己在Ui设计器中命名的Layout，该Layout中包含QStackedWidget
    ui->gridLayout_2->addWidget(ui->page1, 1, 0);
    ui->gridLayout_2->addWidget(ui->page2, 1, 0);
    ui->gridLayout_2->addWidget(ui->page3, 1, 0);
    ui->gridLayout_2->addWidget(ui->page4, 1, 0);
    ui->gridLayout_2->addWidget(ui->page5, 1, 0);
    ui->gridLayout_2->addWidget(ui->page6, 1, 0);
    ui->gridLayout_2->addWidget(ui->page7, 1, 0);

    PlanTaskWidget *widget1 = new PlanTaskWidget(ui->page1, MasterSetting::getSettingInstance()->getDefaultDistanceMode());
    HardwareConfigWidget *widget2 = new HardwareConfigWidget(ui->page2);
    CreateProjectWidget *widget3 = new CreateProjectWidget(WorkingStatus::Preparing, ui->page3);
    QWidget *widget4 = new QWidget(ui->page4); // 空白Widget
    CheckTaskWidget *widget5 = new CheckTaskWidget(ui->page5);
    QWidget *widget6 = new QWidget(ui->page6); // 空白Widget
    QWidget *widget7 = new QWidget(ui->page7); // 空白Widget

    // connect(widgettwo,SIGNAL(send_datatocheck_task(QString)),widgetthree,SLOT(receive_datafrom_widgettwo(QString)));

    QGridLayout *layout1 = new QGridLayout();
    layout1->addWidget(widget1);
    ui->page1->setLayout(layout1);

    QGridLayout *layout2 = new QGridLayout();
    layout2->addWidget(widget2);
    ui->page2->setLayout(layout2);

    QGridLayout *layout3 = new QGridLayout();
    layout3->addWidget(widget3);
    ui->page3->setLayout(layout3);

    QGridLayout *layout4 = new QGridLayout();
    layout4->addWidget(widget4);
    QGridLayout *layout4_1 = new QGridLayout();
    layout4_1->addWidget(new QLabel(tr("请打开采集程序。。。")), 0,0,0,0);
    widget4->setLayout(layout4_1);
    ui->page4->setLayout(layout4);

    QGridLayout *layout5 = new QGridLayout();
    layout5->addWidget(widget5);
    ui->page5->setLayout(layout5);

    QGridLayout *layout6 = new QGridLayout();
    layout6->addWidget(widget6);
    QGridLayout *layout6_1 = new QGridLayout();
    layout6_1->addWidget(new QLabel(tr("请查看计算监控界面。。。")), 0,0,0,0);
    widget6->setLayout(layout6_1);
    ui->page6->setLayout(layout6);

    QGridLayout *layout7 = new QGridLayout();
    layout7->addWidget(widget7);
    QGridLayout *layout7_1 = new QGridLayout(); 
    layout7_1->addWidget(new QLabel(tr("请查看备份监控界面。。。")), 0,0,0,0);
    widget7->setLayout(layout7_1);
    ui->page7->setLayout(layout7);

    // 开始显示第二界面
    //ui->buttonTwo->setDown(true);
    //ui->page_2->setVisible(true);
    //on_buttonTwo_clicked();
    //与其他界面设置状态提示
    //TabWidgetManager *tab =new TabWidgetManager();
    //connect(tab,SIGNAL(clicked01(QString)),this,SLOT(xianshiwenjian()));
    //connect(tab,SIGNAL(clicked111(QString)),this,SLOT(xianshiwenjian()));

    // 鼠标点击信号槽
    connect(ui->button1, SIGNAL(clicked()), this, SLOT(button1Clicked()));
    connect(ui->button2, SIGNAL(clicked()), this, SLOT(button2Clicked()));
    connect(ui->button3, SIGNAL(clicked()), this, SLOT(button3Clicked()));
    connect(ui->button4, SIGNAL(clicked()), this, SLOT(button4Clicked()));
    connect(ui->button5, SIGNAL(clicked()), this, SLOT(button5Clicked()));
    connect(ui->button6, SIGNAL(clicked()), this, SLOT(button6Clicked()));
    connect(ui->button7, SIGNAL(clicked()), this, SLOT(button7Clicked()));

    /************关于流程控制*************/


    // 初始界面第一个界面显示
    button1Clicked();

    // 桌面系统图标相关，防止误操作关闭，可后台运行
    createActions();
    createTrayIcon();

    setIcon();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    trayIcon->show();
}

TabWidgetManager::~TabWidgetManager()
{
    delete ui;
}

// 转变TabButton格式
void TabWidgetManager::setTabButtonStyle(int i)
{
    ui->page1->setVisible(false);
    ui->page2->setVisible(false);
    ui->page3->setVisible(false);
    ui->page4->setVisible(false);
    ui->page5->setVisible(false);
    ui->page6->setVisible(false);
    ui->page7->setVisible(false);

    ui->button1->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton1_up.png)4 4 4 4 stretch stretch;}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}");
    ui->button2->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton1_up.png)4 4 4 4 stretch stretch;}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}");
    ui->button3->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton1_up.png)4 4 4 4 stretch stretch;}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}");
    ui->button4->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton1_up.png)4 4 4 4 stretch stretch;}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}");
    ui->button5->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton1_up.png)4 4 4 4 stretch stretch;}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}");
    ui->button6->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton1_up.png)4 4 4 4 stretch stretch;}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}");
    ui->button7->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton1_up.png)4 4 4 4 stretch stretch;}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}");

    ui->button1->setEnabled(false);
    ui->button2->setEnabled(false);
    ui->button3->setEnabled(false);
    ui->button4->setEnabled(false);
    ui->button5->setEnabled(false);
    ui->button6->setEnabled(false);
    ui->button7->setEnabled(false);
    
    switch(i)
    {
        case 1: ui->page1->setVisible(true);
                ui->button1->setEnabled(true);
                ui->button2->setEnabled(true);
                ui->button1->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");
                break;
        case 2: ui->page2->setVisible(true);
                ui->button1->setEnabled(true);
                ui->button2->setEnabled(true);
                ui->button3->setEnabled(true);
                ui->button2->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");
                break;
        case 3: ui->page3->setVisible(true);
                ui->button2->setEnabled(true);
                ui->button3->setEnabled(true);
                ui->button4->setEnabled(true);
                ui->button5->setEnabled(true);
                ui->button3->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");
                break;
        case 4: ui->page4->setVisible(true);
                ui->button4->setEnabled(true);
                ui->button3->setEnabled(true);
                ui->button4->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");
                break;
        case 5: ui->page5->setVisible(true);
                ui->button3->setEnabled(true);
                ui->button5->setEnabled(true);
                ui->button6->setEnabled(true);
                ui->button7->setEnabled(true);
                ui->button5->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");
                break;
        case 6: ui->page6->setVisible(true);
                ui->button5->setEnabled(true);
                ui->button6->setEnabled(true);
                ui->button7->setEnabled(true);
                ui->button6->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");
                break;
        case 7: ui->page7->setVisible(true);
                ui->button5->setEnabled(true);
                ui->button6->setEnabled(true);
                ui->button7->setEnabled(true);
                ui->button7->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");
                break;
        default:break;
    }
}

void TabWidgetManager::button1Clicked()
{
    setTabButtonStyle(1);
}

void TabWidgetManager::button2Clicked()
{
    setTabButtonStyle(2);
}

void TabWidgetManager::button3Clicked()
{
    setTabButtonStyle(3);
}

void TabWidgetManager::button4Clicked()
{
    setTabButtonStyle(4);
    showCollect();
}

void TabWidgetManager::button5Clicked()
{
    setTabButtonStyle(5);
}

void TabWidgetManager::button6Clicked()
{
    setTabButtonStyle(6);
    showCalcu();
}

void TabWidgetManager::button7Clicked()
{
    setTabButtonStyle(7);
    showBackup();
}

// 桌面系统图标相关，防止误操作关闭，可后台运行
//! [2]
void TabWidgetManager::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("提示"),
                                 tr("程序将在后台运行，终止程序请点击桌面右下小图标，选择退出！"));
        hide();
        event->ignore();
    }
}
//! [2]

//! [3]
void TabWidgetManager::setIcon()
{
    QIcon icon = QIcon(":/image/icon.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
    trayIcon->setToolTip(tr("铁路隧道建筑限界测量系统--数据操作终端"));
    setWindowTitle(tr("铁路隧道建筑限界测量系统--数据操作终端"));
}
//! [3]

//! [4]
void TabWidgetManager::iconActivated(QSystemTrayIcon::ActivationReason reason)
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
void TabWidgetManager::showMessage()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(0);
    trayIcon->showMessage("aaa", "bbb", icon, 1000);
}
//! [5]

//! [6]
void TabWidgetManager::messageClicked()
{
    QMessageBox::information(0, tr("提示"),
                                tr("Sorry, I already gave what help I could.\n"
                                   "Maybe you should try asking a human?"));
}
//! [6]

void TabWidgetManager::createActions()
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

void TabWidgetManager::createTrayIcon()
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

// collectWidgetButton , calcuWidgetButton , backupWidgetButton 点击事件槽函数
void TabWidgetManager::showCollect()
{
    if (!hasinitcollect)
    {
        collectwidget = new MainWidget(false, Collecting);
        hasinitcollect = true;
    }
    collectwidget->show();
}

void TabWidgetManager::showCalcu()
{
    if (!hasinitcalcu)
    {
        calcuwidget = new MainWidget(false, Calculating);
        hasinitcalcu = true;
    }
    calcuwidget->show();
}

void TabWidgetManager::showBackup()
{
    if (!hasinitbackup)
    {
        backupwidget = new MainWidget(false, Backuping);
        hasinitbackup = true;
    }
    backupwidget->show();
}