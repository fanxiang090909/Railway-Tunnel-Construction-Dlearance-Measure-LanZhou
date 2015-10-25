#include "view_3dtvcalcumatfile.h"
#include "ui_view_3dtvcalcumatfile.h"

#include <QTextCodec>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QScrollArea>
#include <QObject>
#include "LzSerialStorageMat.h"
#include "lz_exception.h"

#include "LzCalculator.h"
#include "MatToQImage.h"

#include "show_3dpoints.h"
#include "network_config_list.h"
#include "xmlnetworkfileloader.h"
#include "lz_project_access.h"

#include "clientprogram.h"
#include "setting_client.h"

using namespace std;


View3DTVCalcuMatDataWidget::View3DTVCalcuMatDataWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::View3DTVCalcuMatDataWidget)
{
    ui->setupUi(this);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":player/player_play.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);

    // 【注意】，此处添加widget为了是窗口布局随大窗口缩放而改变
    // 【注意！】ui->gridLayout为自己在Ui设计器中命名的Layout，该Layout中包含QStackedWidget
    ui->gridLayout->addWidget(ui->page4, 0, 1, 1, 1);
    ui->gridLayout->addWidget(ui->page5, 0, 1, 1, 1);

    widget4 = new SelectAvaliableTunnelWidget(ui->page4);
    widget56 = new ImageViewer3DTwoViewWidget(ui->page5);

    QGridLayout *layout2 = new QGridLayout();
    layout2->addWidget(widget4);
    ui->page4->setLayout(layout2);

    QGridLayout *layout3 = new QGridLayout();
    layout3->addWidget(widget56);
    ui->page5->setLayout(layout3);

    /************************************/
    // 鼠标点击信号槽
    connect(ui->button4, SIGNAL(clicked()), this, SLOT(button4Clicked()));
    connect(ui->button5, SIGNAL(clicked()), this, SLOT(button5Clicked()));

    // 界面间通信信号槽
    connect(widget4, SIGNAL(signalSelectedTunnelToEdit(int, QString, bool, bool, bool, double, long long, long long)), this, SLOT(slotSelectedTunnelToSynthesis(int, QString, bool, bool, bool, double, long long, long long)));

    // 按钮切换界面信号槽
    connect(widget4, SIGNAL(signalSelectedTunnelToEdit(int, QString, bool, bool, bool, double, long long, long long)), this, SLOT(button5Clicked()));
    connect(widget56, SIGNAL(finish()), this, SLOT(button4Clicked()));

    /************关于流程控制*************/
    // 开始时禁止点击按钮功能
    ui->button5->setEnabled(false);

    // 显示第一个界面
    button4Clicked();
}

View3DTVCalcuMatDataWidget::~View3DTVCalcuMatDataWidget()
{
    delete ui;

    if (widget4 != NULL)
        delete widget4;
    if (widget56 != NULL)
        delete widget56;
}

// 界面切换槽函数
void View3DTVCalcuMatDataWidget::slotSelectedTunnelToSynthesis(int tunnelid, QString selectfile, bool isDouble, bool carriagedirect, bool isNormal, double distanceMode, long long startframeno, long long endframeno)
{
    if (widget56 == NULL)
        return;

    QString projectpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main);
    QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Main);
    QString tunnelname = "";
    QString tmpimg_projectpath = ClientSetting::getSettingInstance()->getClientTmpLocalParentPath();

    qDebug() << projectpath << tmpimg_projectpath << projectname.left(projectname.size() - 5) <<  "       " << tunnelid << selectfile << startframeno << endframeno;
    widget56->setInfo(false, projectpath, projectname.left(projectname.size() - 5), tmpimg_projectpath, tunnelid, tunnelname);
    widget56->setCurrentFCs(startframeno);

}

void View3DTVCalcuMatDataWidget::my_close_slot()
{
    emit my_close();
    button4Clicked();
}

// 转变TabButton格式
void View3DTVCalcuMatDataWidget::setTabButtonStyle(int i)
{
    ui->page4->setVisible(false);
    ui->page5->setVisible(false);
    ui->button4->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton1_up.png)4 4 4 4 stretch stretch;}"
        "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}"
        "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}");
    ui->button5->setStyleSheet("QPushButton{border-width:4px;border-image:url(:image/tabbutton1_up.png)4 4 4 4 stretch stretch;}"
        "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}"
        "QPushButton:hover{border: 0.1px solid;border-image:url(:image/tabbutton1.png)}");

    switch(i)
    {
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
    default:break;
    }
}

void View3DTVCalcuMatDataWidget::button4Clicked()
{
    ui->button4->setEnabled(true);
    ui->button5->setEnabled(false);
    setTabButtonStyle(4);
}

void View3DTVCalcuMatDataWidget::button5Clicked()
{
    ui->button4->setEnabled(false);
    ui->button5->setEnabled(true);
    setTabButtonStyle(5);
}
