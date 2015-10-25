#include "synthesis_correct.h"
#include "ui_synthesis_correct.h"

#include <QGridLayout>
#include <QDir>
#include <QDebug>
#include <QMessageBox>

/**
 * 隧道综合和图像修正共有界面类定义
 *
 * @author 范翔
 * @author 熊雪
 * @version 1.0.0
 * @date 20140416
 */
SynthesisCorrectWidget::SynthesisCorrectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SynthesisCorrectWidget)
{
    ui->setupUi(this);
    filename="";
    ui->gridLayout->addWidget(ui->page_1, 0, 0, 1, 1);
    ui->gridLayout->addWidget(ui->page_2, 0, 0, 1, 1);

    widget_syn = new SynthesisTunnelWidget(ui->page_1);
    widget_cor = new CorrectClearanceWidget(ui->page_2);
	
    QGridLayout *layout3 = new QGridLayout();
    layout3->addWidget(widget_syn);
    ui->page_1->setLayout(layout3);

    QGridLayout *layout5 = new QGridLayout();
    layout5->addWidget(widget_cor);
    ui->page_2->setLayout(layout5);

    // 隧道综合界面的基础信息（显示缩放因子scale、pixel2mm、分中坐标原点x、y）
    connect(widget_syn,SIGNAL(Sendparametertocorrect_clearance(int,double,int,int)),widget_cor,SLOT(getparameterfromtest(int,double,int,int)));
    
    // 传文件名
    connect(this, SIGNAL(signalSelectedTunnelToEdit(int, QString, bool, bool, bool, double, long long, long long)), widget_cor, SLOT(slotSelectedTunnelToEdit(int, QString, bool, bool, bool, double, long long, long long)));
    connect(this, SIGNAL(signalSelectedTunnelToEdit(int, QString, bool, bool, bool, double, long long, long long)), widget_syn, SLOT(slotSelectedTunnelToSynthesis(int, QString, bool, bool, bool, double, long long, long long)));

    // scale同步
    connect(widget_syn, SIGNAL(sendspinBoxvalue(int)),widget_cor,SLOT(correctscale(int)));
    connect(widget_cor, SIGNAL(sendtosyn(int)),widget_syn,SLOT(correctscale(int)));

    // 转到图形修正界面【起始帧开始】，并切换界面
    connect(widget_syn, SIGNAL(startFromFirst(bool)), widget_cor, SLOT(startViewFromFirst(bool)));
    connect(widget_syn, SIGNAL(startFromFirst(bool)), this, SLOT(tocorrectTab()));
    connect(this, SIGNAL(sendCurrentFrame(_int64)), widget_cor, SLOT(startViewFromSelectedFrame(_int64)));

    connect(widget_syn,SIGNAL(updateFramesInSearchArea(__int64, float, int)),this,SLOT(updateFramesViewInSearchArea(__int64, float, int)), Qt::DirectConnection);
	
    // 收到SynthesisTunnelWidget信号，情况筛选帧号listWidget的内容
    connect(widget_syn, SIGNAL(clearlistwidget()), this, SLOT(clearListWidget()));

    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(sendFrameToChildWidget()));
  
    // 把单隧道综合界面的矩形传递到修正界面

    // 参数均为矩形区域左上、右下角的图像坐标（像素）
    connect(widget_syn, SIGNAL(sendRectPoint(float, float, float, float, bool)), widget_cor, SLOT(getRectPoint(float, float, float, float, bool)));

    // 按钮切换界面信号槽
    connect(widget_cor, SIGNAL(finish()), this, SLOT(finishcorrect()));
    connect(widget_syn, SIGNAL(finish()), this, SLOT(finishsynthesis()));
	
    // 显示第一个界面
    ui->page_1->setVisible(true);
}

SynthesisCorrectWidget::~SynthesisCorrectWidget()
{
    delete ui;
}

void SynthesisCorrectWidget::clearListWidget()
{
	ui->listWidget->clear();
}

void SynthesisCorrectWidget::updateFramesViewInSearchArea(__int64 newtestframecount, float newmile, int neworient)
{
	if (neworient == 1)
	{
	    ui->listWidget->addItem(QString("%1_%2").arg(newmile).arg(newtestframecount));
	}
	else
	{
	    ui->listWidget->addItem(QString("%1_%2").arg(newmile).arg(newtestframecount));
	}
}

void SynthesisCorrectWidget::sendFrameToChildWidget()
{
    tocorrectTab();
    
	int currentrow = ui->listWidget->currentRow();
    __int64 correct_frame;
	if (currentrow < 0)
	{
        // QMessageBox::warning(this,tr("提示"), tr("请在修正帧号中先选择一行！"));
        // return;
        //不选中某一帧,仍能打开图形修正界面
        correct_frame = QStringList(ui->listWidget->item(0)->text().split("_")).at(1).toLongLong();
    }
    correct_frame = QStringList(ui->listWidget->currentItem()->text().split("_")).at(1).toLongLong();

    // 发送当前选择帧号
    emit sendCurrentFrame(correct_frame);
   
}

// 界面切换槽函数
void SynthesisCorrectWidget::slotSelectedTunnelToSynthesis(int tunnelid, QString selectfile, bool isDouble, bool carriagedirect, bool isNormal, double distanceMode, long long startframeno, long long endframeno)
{
    QByteArray ba = selectfile.toLocal8Bit();
    filename = string(ba.constData());
    // 发送给隧道综合、图形修正两个界面
    emit signalSelectedTunnelToEdit(tunnelid, selectfile, isDouble, carriagedirect, isNormal, distanceMode, startframeno, endframeno);
}

// 界面切换槽函数
void SynthesisCorrectWidget::finishcorrect()
{
    ui->page_1->setVisible(true);
    ui->page_2->setVisible(false);
	emit toTabSynthesis();
}

void SynthesisCorrectWidget::finishsynthesis()
{
	emit finish();
}

// 界面切换槽函数
void SynthesisCorrectWidget::tocorrectTab()
{
    ui->page_1->setVisible(false);
    ui->page_2->setVisible(true);
    emit toTabCorrect();
}