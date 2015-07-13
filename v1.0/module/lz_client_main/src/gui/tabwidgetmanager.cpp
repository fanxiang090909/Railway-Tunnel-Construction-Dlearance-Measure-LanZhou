#include "tabwidgetmanager.h"
#include "ui_tabwidgetmanager.h"

#include <QBoxLayout>

// 不同界面。。。的头文件
#include "widgetone.h"
#include "output_clearance.h"
#include "input_bridge_clearance.h"
#include "select_tunnel.h"
#include "select_historical_tunnel.h"
#include "synthesis_correct.h"
#include "backup_db.h"

/**
 * 左侧按钮切换页面tabwidget界面类实现
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-08-18
 */
TabWidgetManager::TabWidgetManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabWidgetManager)
{
    ui->setupUi(this);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":image/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);

    // 【注意】，此处添加widget为了是窗口布局随大窗口缩放而改变
    // 【注意！】ui->gridLayout为自己在Ui设计器中命名的Layout，该Layout中包含QStackedWidget
    ui->gridLayout->addWidget(ui->page1, 0, 1, 1, 1);
    ui->gridLayout->addWidget(ui->page2, 0, 1, 1, 1);
    ui->gridLayout->addWidget(ui->page3, 0, 1, 1, 1);
    ui->gridLayout->addWidget(ui->page4, 0, 1, 1, 1);
    ui->gridLayout->addWidget(ui->page5, 0, 1, 1, 1);
    ui->gridLayout->addWidget(ui->page6, 0, 1, 1, 1);
    ui->gridLayout->addWidget(ui->page7, 0, 1, 1, 1);
    ui->gridLayout->addWidget(ui->page8, 0, 1, 1, 1);

    BackupDatabaseWidget *widgetbackupdb = new BackupDatabaseWidget(ui->page3);

    WidgetOne *widget2 = new WidgetOne(ui->page1);
    InputBridgeWidget * widget3 = new InputBridgeWidget(ui->page2);
    SelectAvaliableTunnelWidget *widget4 = new SelectAvaliableTunnelWidget(ui->page4);
    SynthesisCorrectWidget * widget56 = new SynthesisCorrectWidget(ui->page5);
    SelectHistoricalTunnelDataWidget *widget7 = new SelectHistoricalTunnelDataWidget(ui->page7);
    OutputClearanceWidget *widget8 = new OutputClearanceWidget(ui->page8);

    QGridLayout *layout1 = new QGridLayout();
    layout1->addWidget(widget2);
    ui->page1->setLayout(layout1);

    QGridLayout *layout8 = new QGridLayout();
    layout8->addWidget(widget3);
    ui->page2->setLayout(layout8);

    QGridLayout *layout7 = new QGridLayout();
    layout7->addWidget(widgetbackupdb);
    ui->page3->setLayout(layout7);

    QGridLayout *layout2 = new QGridLayout();
    layout2->addWidget(widget4);
    ui->page4->setLayout(layout2);

    QGridLayout *layout3 = new QGridLayout();
    layout3->addWidget(widget56);
    ui->page5->setLayout(layout3);

    QGridLayout *layout6 = new QGridLayout();
    layout6->addWidget(widget7);
    ui->page7->setLayout(layout6);

    QGridLayout *layout4 = new QGridLayout();
    layout4->addWidget(widget8);
    ui->page8->setLayout(layout4);

    /************************************/
    // 鼠标点击信号槽
    connect(ui->button1, SIGNAL(clicked()), this, SLOT(button1Clicked()));
    connect(ui->button2, SIGNAL(clicked()), this, SLOT(button2Clicked()));
    connect(ui->button3, SIGNAL(clicked()), this, SLOT(button3Clicked()));
    connect(ui->button4, SIGNAL(clicked()), this, SLOT(button4Clicked()));
    connect(ui->button5, SIGNAL(clicked()), this, SLOT(button5Clicked()));
    connect(ui->button6, SIGNAL(clicked()), this, SLOT(button6Clicked()));
    connect(ui->button7, SIGNAL(clicked()), this, SLOT(button7Clicked()));
    connect(ui->button8, SIGNAL(clicked()), this, SLOT(button8Clicked()));

    // 界面间通信信号槽
    connect(widget7, SIGNAL(updateOutput(SingleOrMultiSelectionMode, CurveType)), widget8, SLOT(updateClearanceTableModel(SingleOrMultiSelectionMode, CurveType)));
    connect(widget2, SIGNAL(signalBridgeToEdit(int, QString)), widget3, SLOT(slotBridgeToEdit(int, QString)));
    connect(widget4, SIGNAL(signalSelectedTunnelToEdit(int, QString, bool)), widget56, SLOT(slotSelectedTunnelToSynthesis(int, QString, bool)));
	// 关闭窗口信号槽，告知数据库可能需要备份
    connect(this, SIGNAL(my_close()), widgetbackupdb, SLOT(checkIfExport()));

    // 按钮切换界面信号槽
    connect(widget7, SIGNAL(updateOutput(SingleOrMultiSelectionMode, CurveType)), this, SLOT(button8Clicked()));
    connect(widget2, SIGNAL(signalBridgeToEdit(int, QString)), this, SLOT(button2Clicked()));
    connect(widget8, SIGNAL(backToSelectionSignal()), this, SLOT(button7Clicked()));
    connect(widget3, SIGNAL(signalEndEdit()), this, SLOT(button1Clicked()));
    connect(widget4, SIGNAL(signalSelectedTunnelToEdit(int, QString, bool)), this, SLOT(button5Clicked()));
    connect(widget56, SIGNAL(toTabCorrect()), this, SLOT(button6Clicked()));
    connect(widget56, SIGNAL(toTabSynthesis()), this, SLOT(button5Clicked()));
    connect(widget56, SIGNAL(finish()), this, SLOT(button4Clicked()));

    /************关于流程控制*************/
    // 开始时禁止点击按钮功能
    ui->button2->setEnabled(false);
    ui->button5->setEnabled(false);
    ui->button6->setEnabled(false);
    ui->button8->setEnabled(false);

    // 显示第一个界面
	button3Clicked();
}

TabWidgetManager::~TabWidgetManager()
{
    delete ui;
}

void TabWidgetManager::my_close_slot()
{
    emit my_close();
    button3Clicked();
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
    ui->page8->setVisible(false);
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
    ui->button8->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton1_up.png)4 4 4 4 stretch stretch;}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}"
                                 "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}");

    switch(i)
    {
        case 1: ui->page1->setVisible(true);
                ui->button1->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");
                break;
        case 2: ui->page2->setVisible(true);
                ui->button2->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                         "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                         "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");
                break;
        case 3: ui->page3->setVisible(true);
                ui->button3->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");
                break;
        case 4: ui->page4->setVisible(true);
                ui->button4->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");

                break;
        case 5: ui->page5->setVisible(true);
                ui->button5->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");

                break;
        case 6: ui->page5->setVisible(true);//ui->page6->setVisible(true); // @author范翔更改界面方式，button5button6指向同一个widget56
                ui->button6->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");
                break;
        case 7: ui->page7->setVisible(true);
                ui->button7->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");

                break;
        case 8: ui->page8->setVisible(true);
                ui->button8->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton2.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton2.png)}");
                break;
        default:break;
    }
}

void TabWidgetManager::button1Clicked()
{
    ui->button2->setEnabled(false);
    ui->button1->setEnabled(true);
    setTabButtonStyle(1);
}

void TabWidgetManager::button2Clicked()
{
    ui->button2->setEnabled(true);
    ui->button1->setEnabled(false);
    setTabButtonStyle(2);
}

void TabWidgetManager::button3Clicked()
{
    emit my_close();
    setTabButtonStyle(3);
}

void TabWidgetManager::button4Clicked()
{
    ui->button4->setEnabled(true);
    ui->button5->setEnabled(false);
    ui->button6->setEnabled(false);
    setTabButtonStyle(4);
}

void TabWidgetManager::button5Clicked()
{
    ui->button4->setEnabled(false);
    ui->button5->setEnabled(true);
    ui->button6->setEnabled(false);
    setTabButtonStyle(5);
}

void TabWidgetManager::button6Clicked()
{
    ui->button4->setEnabled(false);
    ui->button5->setEnabled(false);
    ui->button6->setEnabled(true);
    setTabButtonStyle(6);
}

void TabWidgetManager::button7Clicked()
{
    ui->button7->setEnabled(true);
    ui->button8->setEnabled(false);
    setTabButtonStyle(7);
}

void TabWidgetManager::button8Clicked()
{
    ui->button7->setEnabled(false);
    ui->button8->setEnabled(true);
    setTabButtonStyle(8);
}
