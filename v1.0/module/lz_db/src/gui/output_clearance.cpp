#include "output_clearance.h"
#include "ui_output_clearance.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QSqlTableModel>

#include "daoclearanceoutput.h"
#include "setting_client.h"

#include "daotask.h"
#include "daotasktunnel.h"
#include "lz_output.h"

#include <QVariant>
#include <QProcess>

#include <QPainter>
#include <QFile>
#include <QTextCodec>
#include <iostream>
#include <string>

using namespace std;

/**
 * 隧道限界图表输出界面类定义
 *
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 20140609
 */
OutputClearanceWidget::OutputClearanceWidget(QWidget *parent) :
    QWidget(parent),
    singleTunnelModel(ClientSetting::getSettingInstance()->getSingleTunnelModel()), 
    multiTunnelsModel(ClientSetting::getSettingInstance()->getMultiTunnelsModel()), 
    ui(new Ui::OutputClearanceWidget)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    ui->setupUi(this);

    templatepath = ClientSetting::getSettingInstance()->getOutExcelTemplatePath();

    // 界面上默认的底板是基础隧道限界--电力牵引
    outImageType = OutType_B_DianLi;
    datatype = AllType;

    // 单隧道基本信息模型设置为NULL，界面切换过来时再从ClientSetting中加载
    singleTunnelBasicMode = NULL;

    edit = NULL;
	webview = NULL;
    iscorrect = true;//true表示默认修正
    
    selectClearanceType = Curve_Straight;

    // PDF EXCEL生成接口，在未点击时默认为NULL
    outputAccess = new LzOutputAccess();
    outputAccess->setTemplatePath(templatepath);

    /**********************/
    // 【注意】，此处添加widget为了是窗口布局随大窗口缩放而改变
    // 【注意！】ui->gridLayout为自己在Ui设计器中命名的Layout，该Layout中包含QStackedWidget

    scrollArea = new QScrollArea(ui->graphicsView);
    syntunnelcorrect = new ClearanceImage(ui->graphicsView);
    
    scrollArea->setWidget(syntunnelcorrect);
    ui->tab_2->layout()->addWidget(scrollArea);

    // 单隧道、多隧道综合数据Tab布局[无效]

    /*ui->gridLayout_2->addWidget(ui->page_2, 0, 0, 1, 1);

    QGridLayout *layout1 = new QGridLayout();
    layout1->addWidget(ui->page_2);
    ui->page_2->setLayout(layout1);*/

    // 多隧道综合数据list显示
    ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 当多隧道综合时，批量输出各个隧道的限界图表到EXCEL文件
    connect(ui->exportAllToExcelsButton, SIGNAL(clicked()),this, SLOT(exportAllToExcels()));

    // 选择输出图标类型信号槽
    connect(ui->comboBox_OutImageType, SIGNAL(currentIndexChanged(int)), SLOT(selectOutputClearanceImageType(int)));
    // 告知画图界面输出图标类型信号槽
    connect(this, SIGNAL(sendOutputType(OutputClearanceImageType)), syntunnelcorrect, SLOT(setfloornumber(OutputClearanceImageType)));
    //选择显示的数据类型
    connect(ui->comboBox_outCurveType,SIGNAL(currentIndexChanged(int)),this,SLOT(selectOutputClearanceDataType(int)));
    //告知画图界面输出数据类型信号槽
    connect(this,SIGNAL(sendOutputDataType(ShowDataType)),syntunnelcorrect, SLOT(setImageType(ShowDataType)));
    // 限界输出预览信号槽，PDF和EXCEL两种方式
    connect(ui->pdfPreviewButton, SIGNAL(clicked()), this, SLOT(pdfPreview()));
    connect(ui->excelViewButton, SIGNAL(clicked()), this, SLOT(excelPreview()));

	// 返回上一个界面按钮
    connect(ui->backButton, SIGNAL(clicked()), this, SLOT(backToSelectionTunnel()));

    // 加载表头数据
    clearanceTableHeaderLoad();

    // 手动输入项初值 起讫站名
    qiqizhanming = "";
    // 最大外轨超高
    waiguichaogao = "";  
    // 最低接触网高度
    jiechuwanggaodu = "";

    // 关于编辑限界值、保存限界值的信号槽
    connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(saveItemChanged(QTableWidgetItem*)));
    connect(ui->lineEdit_minHeight,SIGNAL(textEdited(QString)),this,SLOT(minheightChanged(QString)));
    connect(ui->modifyButton, SIGNAL(clicked()), this, SLOT(canBeModified()));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));
}

OutputClearanceWidget::~OutputClearanceWidget()
{
    delete ui;

    if (syntunnelcorrect != NULL)
        delete syntunnelcorrect;
    if (scrollArea != NULL)
        delete scrollArea;

    // 删除PDF和EXCEL预览接口
    if (edit != NULL)
        delete edit;

    if (webview != NULL)
        delete webview;

    // 删除outputAccess
    if (outputAccess != NULL)
        delete outputAccess;
}


/**
 * 初始化PDF预览接口
 */
void OutputClearanceWidget::initTextEdit()
{
    if (edit != NULL)
    {
        int result = QMessageBox::warning(this,tr("警告"), tr("当前正在查看某界面，尚未关闭。确定浏览新输出限界？"),
                                          QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::No)
            return;
        else if (result == QMessageBox::Yes)
        {
            delete edit;
            edit = new TextEdit();
            return;
        }
    }
    else
        edit = new TextEdit();
}

/**
 * 初始化浏览器界面
 */
void OutputClearanceWidget::initFancyBrowser()
{
	if (webview != NULL)
    {
        int result = QMessageBox::warning(this,tr("警告"), tr("当前正在查看某界面，尚未关闭。确定浏览新输出限界？"),
                                          QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::No)
            return;
        else if (result == QMessageBox::Yes)
        {
            delete webview;
            webview = new FancyBrowser();
            return;
        }
    }
    else
        webview = new FancyBrowser();
}

/**
 * makedir，在用textEdit生成html时不选择生成目录，因此可能目录不存在
 */
void OutputClearanceWidget::makedir(QString filename)
{
    QDir mydir;
    if (!mydir.exists(QFileInfo(filename).path()))
        mydir.mkpath(QFileInfo(filename).path());
}

/**
 * 界面上的单隧道限界表预览（非图形的）
 */
void OutputClearanceWidget::clearanceTableHeaderLoad()
{
    singleTunnelModel.getClearanceStraightData().initMaps();
    int heightlength = singleTunnelModel.getClearanceStraightData().getMaps().size();
    ui->tableWidget->setColumnCount(16);
    ui->tableWidget->setRowCount(heightlength);
    //ui->tableWidget->setHorizontalHeaderItem(LzOutputTableColumn::Height, new QTableWidgetItem(QObject::tr("轨面高度")));
    ui->tableWidget->setHorizontalHeaderItem(Straight_Left_Val - 1, new QTableWidgetItem(QObject::tr("直线尺寸-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(Straight_Right_Val - 1, new QTableWidgetItem(QObject::tr("直线尺寸-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(Straight_Left_Pos - 1, new QTableWidgetItem(QObject::tr("直线控制点-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(Straight_Right_Pos - 1, new QTableWidgetItem(QObject::tr("直线控制点-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(LeftCurve_Left_Val - 1, new QTableWidgetItem(QObject::tr("左转尺寸-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(LeftCurve_Right_Val - 1, new QTableWidgetItem(QObject::tr("左转尺寸-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(LeftCurve_Left_Radius - 1, new QTableWidgetItem(QObject::tr("左转半径-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(LeftCurve_Right_Radius - 1, new QTableWidgetItem(QObject::tr("左转半径-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(LeftCurve_Left_Pos - 1, new QTableWidgetItem(QObject::tr("左转控制点-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(LeftCurve_Right_Pos - 1, new QTableWidgetItem(QObject::tr("左转控制点-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(RightCurve_Left_Val - 1, new QTableWidgetItem(QObject::tr("右转尺寸-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(RightCurve_Right_Val - 1, new QTableWidgetItem(QObject::tr("右转尺寸-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(RightCurve_Left_Radius - 1, new QTableWidgetItem(QObject::tr("右转半径-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(RightCurve_Right_Radius - 1, new QTableWidgetItem(QObject::tr("右转半径-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(RightCurve_Left_Pos - 1, new QTableWidgetItem(QObject::tr("右转控制点-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(RightCurve_Right_Pos - 1, new QTableWidgetItem(QObject::tr("右转控制点-右侧")));

    // 选择模式
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    // 编辑模式，不可编辑
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //for(int i = 0; i < heightlength; i++)
    //{
    //    ui->tableWidget->item(i, 0)->setFlags(Qt::NoItemFlags);
    //}

    std::map <int,ClearanceItem>::reverse_iterator it = singleTunnelModel.getClearanceStraightData().getMaps().rbegin();
    int k = 0;
    while (it != singleTunnelModel.getClearanceStraightData().getMaps().rend())
    {
        ui->tableWidget->setVerticalHeaderItem(k, new QTableWidgetItem(QString("%1").arg((*it).first)));
        k++;
        it++;
    }
}

void OutputClearanceWidget::clearanceTablePreview()
{
    maketable = true;

    // 临时变量
    int hass  = 0, hasl = 0, hasr = 0;
    ClearanceData & datas = singleTunnelModel.getClearanceStraightData();
    ClearanceData & datal = singleTunnelModel.getClearanceLeftData();
    ClearanceData & datar = singleTunnelModel.getClearanceRightData();

    ui->lineEdit_minHeight->setText(QString("%1").arg(datas.getMinCenterHeight()));

    // 若是单隧道综合结果输出
    if (singleMultiMode == SingleOrMultiSelectionMode::Single_Mode)
    {
        hass = singleTunnelModel.getHasStraight();
        hasl = singleTunnelModel.getHasLeft();
        hasr = singleTunnelModel.getHasRight();
    }
    else // 多隧道区段综合
    {
        hass = multiTunnelsModel.getNumOfStraight();
        hasl = multiTunnelsModel.getNumOfLeft();
        hasr = multiTunnelsModel.getNumOfRight();
        datas = multiTunnelsModel.getClearanceStraightData();
        datal = multiTunnelsModel.getClearanceLeftData();
        datar = multiTunnelsModel.getClearanceRightData();
    }
    
    if (!hass && !hasl && !hasr)
        return;

    // map反向遍历，正向为高度从小到大，插入图表是需要高度从大到小
    std::map<int, ClearanceItem>::reverse_iterator its = datas.getMaps().rbegin();
    std::map<int, ClearanceItem>::reverse_iterator itl = datal.getMaps().rbegin();
    std::map<int, ClearanceItem>::reverse_iterator itr = datar.getMaps().rbegin();
    int k = 0;
    while (its != datas.getMaps().rend())
    {
        std::pair<int, ClearanceItem> pairs = (*its);
        std::pair<int, ClearanceItem> pairl = (*itl);
        std::pair<int, ClearanceItem> pairr = (*itr);

        if (hass)
        {
            ui->tableWidget->setItem(k, Straight_Left_Val - 1, new QTableWidgetItem(QString("%1").arg(pairs.second.leftval)));
            ui->tableWidget->setItem(k, Straight_Right_Val - 1, new QTableWidgetItem(QString("%1").arg(pairs.second.rightval)));
            ui->tableWidget->setItem(k, Straight_Left_Pos - 1, new QTableWidgetItem(QString("%1").arg(pairs.second.leftpos)));
            ui->tableWidget->setItem(k, Straight_Right_Pos - 1, new QTableWidgetItem(QString("%1").arg(pairs.second.rightpos)));
        }

        if (hasl)
        {
            ui->tableWidget->setItem(k, LeftCurve_Left_Val - 1, new QTableWidgetItem(QString("%1").arg(pairl.second.leftval)));
            ui->tableWidget->setItem(k, LeftCurve_Right_Val - 1, new QTableWidgetItem(QString("%1").arg(pairl.second.rightval)));
            ui->tableWidget->setItem(k, LeftCurve_Left_Radius - 1, new QTableWidgetItem(QString("%1").arg(pairl.second.leftradius)));
            ui->tableWidget->setItem(k, LeftCurve_Right_Radius - 1, new QTableWidgetItem(QString("%1").arg(pairl.second.rightradius)));
            ui->tableWidget->setItem(k, LeftCurve_Left_Pos - 1, new QTableWidgetItem(QString("%1").arg(pairl.second.leftpos)));
            ui->tableWidget->setItem(k, LeftCurve_Right_Pos - 1, new QTableWidgetItem(QString("%1").arg(pairl.second.rightpos)));
        }

        if (hasr)
        {
            ui->tableWidget->setItem(k, RightCurve_Left_Val - 1, new QTableWidgetItem(QString("%1").arg(pairr.second.leftval)));
            ui->tableWidget->setItem(k, RightCurve_Right_Val - 1, new QTableWidgetItem(QString("%1").arg(pairr.second.rightval)));
            ui->tableWidget->setItem(k, RightCurve_Left_Radius - 1, new QTableWidgetItem(QString("%1").arg(pairr.second.leftradius)));
            ui->tableWidget->setItem(k, RightCurve_Right_Radius - 1, new QTableWidgetItem(QString("%1").arg(pairr.second.rightradius)));
            ui->tableWidget->setItem(k, RightCurve_Left_Pos - 1, new QTableWidgetItem(QString("%1").arg(pairr.second.leftpos)));
            ui->tableWidget->setItem(k, RightCurve_Right_Pos - 1, new QTableWidgetItem(QString("%1").arg(pairr.second.rightpos)));
        }

        its++;
        itl++;
        itr++;
        k++;
    }
    maketable = false;

}

/**
 * 界面上的单隧道限界图预览
 */
void OutputClearanceWidget::clearanceImagePreview()
{
    // 临时变量
    int hass  = 0, hasl = 0, hasr = 0;
    ClearanceData & datas = singleTunnelModel.getClearanceStraightData();
    ClearanceData & datal = singleTunnelModel.getClearanceLeftData();
    ClearanceData & datar = singleTunnelModel.getClearanceRightData();

    // 若是单隧道综合结果输出
    if (singleMultiMode == SingleOrMultiSelectionMode::Single_Mode)
    {
        hass = singleTunnelModel.getHasStraight();
        hasl = singleTunnelModel.getHasLeft();
        hasr = singleTunnelModel.getHasRight();
    }
    else // 多隧道区段综合
    {
        hass = multiTunnelsModel.getNumOfStraight();
        hasl = multiTunnelsModel.getNumOfLeft();
        hasr = multiTunnelsModel.getNumOfRight();
        datas = multiTunnelsModel.getClearanceStraightData();
        datal = multiTunnelsModel.getClearanceLeftData();
        datar = multiTunnelsModel.getClearanceRightData();
    }
    
    syntunnelcorrect->clearPointsArrayAll();

    if (!hass && !hasl && !hasr)
        return;

    qDebug() << "loadSynthesisData: hasstraight = " << hass << ", hasleft = " << hasl << ", hasright" << hasr;
   
    bool ret;
    if (hass)
    {
        syntunnelcorrect->initPointsArray(datas.getMaps().size(), Curve_Straight);
        ret = syntunnelcorrect->ClearanceDataToPointsArray(datas, Curve_Straight);
    }
    if (hasl)
    {
        syntunnelcorrect->initPointsArray(datal.getMaps().size(), Curve_Left);
        ret = syntunnelcorrect->ClearanceDataToPointsArray(datal, Curve_Left);
    }
    if (hasr)
    {
        syntunnelcorrect->initPointsArray(datar.getMaps().size(), Curve_Right);
        ret = syntunnelcorrect->ClearanceDataToPointsArray(datar, Curve_Right);
    }
    // 即更新绘图
    syntunnelcorrect->update();

    canSave(false);
}

void OutputClearanceWidget::selectOutputClearanceImageType(int index)
{
    if (singleMultiMode == SingleOrMultiSelectionMode::Single_Mode)
    {
        if (ui->lineEdit_lineType->text().compare(tr("内燃牵引")) == 0 && index == 0)       //即基本限界内燃牵引
            outImageType = OutType_B_NeiRan;
        else if(ui->lineEdit_lineType->text().compare(tr("电力牵引")) == 0 && index == 0)   //即基本限界电力牵引
            outImageType = OutType_B_DianLi;
        else if(ui->lineEdit_lineType->text().compare(tr("内燃牵引")) == 0 && index == 1)   //即双层集装箱内燃牵引
            outImageType = OutType_D_NeiRan;
        else if(ui->lineEdit_lineType->text().compare(tr("电力牵引")) == 0 && index == 1)   //即双层集装箱电力牵引
            outImageType = OutType_D_DianLi;
        qDebug() << "outImageType" << outImageType;
        emit sendOutputType(outImageType);
    }
    else // 若是多隧道综合结果输出
    {
        outImageType = OutType_B_DianLi;
        emit sendOutputType(outImageType);
    }
}

void OutputClearanceWidget::selectOutputClearanceDataType(int index)
{
    index = ui->comboBox_outCurveType->currentIndex();
    if(index == 0)
        datatype = AllType;
    else if(index == 1)
        datatype = StraightType;
    else if(index == 2)
        datatype = LeftType;
    else
        datatype = RightType;
    syntunnelcorrect->setImageType(datatype);
}

void OutputClearanceWidget::updateClearanceTableModel(SingleOrMultiSelectionMode outputMode, CurveType newClearanaceType)
{
    singleMultiMode = outputMode;
    selectClearanceType = newClearanaceType;

    // 手动输入项初值 起讫站名，最大外轨超高，最低接触网高度
    ui->tunnelstartendstationedit->setText("");
    ui->maxwaiguichaogaoedit->setText("");
    ui->jiechuwanggaoduedit->setText("");

    // 若是单隧道综合结果输出
    if (outputMode == SingleOrMultiSelectionMode::Single_Mode)
    {
        // 可见起讫站名输入框
        ui->tunnelstartendstationedit->setVisible(true);
        ui->tunnelstartendstationlabel->setVisible(true);

        // 显示单隧道信息
        ui->page->setVisible(true);
        ui->page_2->setVisible(false);
        
        // 加载单隧道信息到相关Label中
        QString tunnelinfo = singleTunnelModel.getTaskTunnelInfo();
        QStringList strlist = tunnelinfo.split("_");
        if (strlist.length() < 2)
        {
            qDebug() << QObject::tr("采集时间加载错误");
        }
        else
        {
            QString collectdate = strlist.at(1);
            ui->lineEdit_collectdate->setText(collectdate);
        }

        bool ret;
        TunnelDataModel *tunnelDataModel = ClientSetting::getSettingInstance()->getOutputTunnelDataModel(ret);
        if (ret)
        {
            ui->lineEdit_linename->setText(QString("%1").arg(tunnelDataModel->getLinename().c_str()));
            ui->lineEdit_startStation->setText(QString("%1").arg(tunnelDataModel->getLineStartStation().c_str()));
            ui->lineEdit_endStation->setText(QString("%1").arg(tunnelDataModel->getLineEndStation().c_str()));
            ui->lineEdit_name->setText(QString(tr("%1")).arg(QString::fromLocal8Bit(tunnelDataModel->getName().c_str())));
            ui->lineEdit_IDstd->setText(QString(tr("%1")).arg(QString::fromLocal8Bit(tunnelDataModel->getIdStd().c_str())));
            ui->lineEdit_startPoint->setText(QString("%1").arg(tunnelDataModel->getStartPoint()));
            ui->lineEdit_endPoint->setText(QString("%1").arg(tunnelDataModel->getEndPoint()));

            int isNewLine = tunnelDataModel->getIsNew();
            int lineType = tunnelDataModel->getLineType();
            bool isDoubleLine = tunnelDataModel->getIsDoubleLine();
            bool isDownLink = tunnelDataModel->getIsDownlink();

			bool isNormalTravel = tunnelDataModel->getIsNormalTravel();
            //qDebug() << "isNewLine:" << isNewLine << ", lineType:" << lineType << ", isDoubleLine:" << isDoubleLine << ", isDownLink:" << isDownLink;

            if (isNewLine == 1) // 新线
                ui->lineEdit_isNew->setText(tr("新线"));
            else if (isNewLine == 0) // 旧线
                ui->lineEdit_isNew->setText(tr("旧线"));
            else
                ui->lineEdit_isNew->setText(tr(""));

            if (lineType == 1) // 内燃牵引
            {
                ui->lineEdit_lineType->setText(tr("内燃牵引"));
                syntunnelcorrect->setfloornumber(OutType_B_NeiRan);
            }
            else if (lineType == 0) // 电力牵引
            {
                ui->lineEdit_lineType->setText(tr("电力牵引"));
                syntunnelcorrect->setfloornumber(OutType_B_DianLi);
            }
            else
                ui->lineEdit_lineType->setText(tr(""));

            if (isDoubleLine == true) // 双线
                ui->lineEdit_isDoubleLine->setText(tr("双线"));
            else if (isDoubleLine == false) //
                ui->lineEdit_isDoubleLine->setText(tr("单线"));
            else
                ui->lineEdit_isDoubleLine->setText(tr(""));

            if (isDownLink == true) // 下行
                ui->lineEdit_isDownLink->setText(tr("下行"));
            else if (isDownLink == false) // 上行
                ui->lineEdit_isDownLink->setText(tr("上行"));
            else
                ui->lineEdit_isDownLink->setText(tr(""));
			
			/*if(isNormalTravel == true) //正常行驶
				ui->lineEdit_isNormalTravel->setText(tr("正常行驶"));
			else if(isNormalTravel == false)
				ui->lineEdit_isNormalTravel->setText(tr("非正常行驶"));
			else
				ui->lineEdit_isNormalTravel->setText(tr(""));*/



        }
        else
        {
            QMessageBox::warning(this, tr("提示"), tr("当前不能找到隧道模型%1！").arg(tunnelinfo));
        }

        singleTunnelBasicMode = tunnelDataModel;

    }
    else     // 若是多隧道综合结果输出
    {
        // 不可见起讫站名输入框
        ui->tunnelstartendstationedit->setVisible(false);
        ui->tunnelstartendstationlabel->setVisible(false);

        // 显示区段综合隧道信息
        ui->page->setVisible(false);
        ui->page_2->setVisible(true);

        // 加载区段隧道信息到相关区段listView中
        ui->listView->setModel(multiTunnelsModel.getTunnelsNames());
    }

    // 更新限界表格数据
    clearanceTablePreview();

    // 更新限界图
    clearanceImagePreview();

}

/**
 * 返回到限界选择槽，触发转换界面信号
 */
void OutputClearanceWidget::backToSelectionTunnel()
{
    emit backToSelectionSignal();
}

/**
 * 限界输出预览槽函数，PDF方式
 */
void OutputClearanceWidget::pdfPreview()
{
    // 输出时到图形tab界面，否则无法打印图片
    ui->tabWidget->setCurrentIndex(1);

    // 起讫站名，最大外轨超高，最低接触网高度赋值
    qiqizhanming = ui->tunnelstartendstationedit->text();
    waiguichaogao = ui->maxwaiguichaogaoedit->text();
    jiechuwanggaodu = ui->jiechuwanggaoduedit->text();

    QString outputfilename;
    QString insertimgfile = ClientSetting::getSettingInstance()->getParentPath() + "/output/";
    int ret = -1;
    
    outputAccess->initOutput(LzOutputAccess::OutputInHTML);

    if (singleMultiMode == SingleOrMultiSelectionMode::Single_Mode) // 单隧道综合
    {
        // 起讫站名，最大外轨超高，最低接触网高度赋值
        singleTunnelModel.setQiQiZhanMing(qiqizhanming);
        singleTunnelModel.setWaiGuiChaoGao(waiguichaogao);
        singleTunnelModel.setJieChuWangGaoDu(jiechuwanggaodu);

        // 获得输出文件名
        outputfilename = ClientSetting::getSettingInstance()->getParentPath() + "/output/" + singleTunnelModel.getTaskTunnelInfoFormat() + ".html";
        makedir(outputfilename);

        // 保存图片
        insertimgfile = insertimgfile + singleTunnelModel.getTaskTunnelInfoFormat() + ".jpg";
        syntunnelcorrect->saveImage(insertimgfile);

        bool ret1;
        ret = outputAccess->outputSingleTunnel(ClientSetting::getSettingInstance()->getOutputTunnelDataModel(ret1), &singleTunnelModel, outputfilename, insertimgfile);

        statusShow(outputfilename, ret, true);
    }
    else // 多隧道综合
    {
        // 起讫站名，最大外轨超高，最低接触网高度赋值
        multiTunnelsModel.setWaiGuiChaoGao(waiguichaogao);
        multiTunnelsModel.setJieChuWangGaoDu(jiechuwanggaodu);

        // 获得输出文件名 TODO
        QString datetimestr = QDateTime::currentDateTime().toString("yyyyMMdd hhmmss");
        outputfilename = ClientSetting::getSettingInstance()->getParentPath() + "/output/" + QObject::tr("区段综合") + "_" + datetimestr + ".html";
        makedir(outputfilename);

        // 保存图片
        insertimgfile = insertimgfile + QObject::tr("区段综合") + "_" + datetimestr+ ".jpg";
        syntunnelcorrect->saveImage(insertimgfile);

        // 输出到excel
        ret = outputAccess->outputMultiTunnels(&multiTunnelsModel, outputfilename, insertimgfile);
        
        statusShow(outputfilename, ret, true);
    }

    if (ret != 0)
        return;

    //用QTextEdit显示限界图表
    //Q_INIT_RESOURCE(textedit);
    //initTextEdit();
    //edit->initOpenFileName(outputfilename);
    //edit->show();

    // 用浏览器显示限界图和表
	initFancyBrowser();
	// @author 范翔 url打开本地文件需加file:///

    // TODO 还有bug 乱码
    //QString urlfile = QString("file:///") + string_To_UTF8(outputfilename.toLocal8Bit().constData()).c_str();
    QString urlfile = "file:///" + outputfilename.replace("#", "%23");
    qDebug() << urlfile;
    webview->setUrl(QUrl(urlfile));
	webview->show();
}

/**
 * 限界输出预览槽函数，EXCEL方式
 */
void OutputClearanceWidget::excelPreview()
{
    // 输出时到图形tab界面，否则无法打印图片
    ui->tabWidget->setCurrentIndex(1);

    // 起讫站名，最大外轨超高，最低接触网高度赋值
    qiqizhanming = ui->tunnelstartendstationedit->text();
    waiguichaogao = ui->maxwaiguichaogaoedit->text();
    jiechuwanggaodu = ui->jiechuwanggaoduedit->text();

    QString outputfilename;
    QString insertimgfile = ClientSetting::getSettingInstance()->getParentPath() + "/output/";
    int ret = -1;
    
    outputAccess->initOutput(LzOutputAccess::OutputInExcel);

    if (singleMultiMode == SingleOrMultiSelectionMode::Single_Mode) // 单隧道综合
    {
        // 起讫站名，最大外轨超高，最低接触网高度赋值
        singleTunnelModel.setQiQiZhanMing(qiqizhanming);
        singleTunnelModel.setWaiGuiChaoGao(waiguichaogao);
        singleTunnelModel.setJieChuWangGaoDu(jiechuwanggaodu);

        // 获得输出文件名
        QString openFileDir = ClientSetting::getSettingInstance()->getParentPath() + "/output/" + singleTunnelModel.getTaskTunnelInfoFormat();
;
        outputfilename = QFileDialog::getSaveFileName(this, tr("导出到文件"), openFileDir, tr("EXCEL (*.xls)"));

        // 保存图片
        insertimgfile = QString(openFileDir.constData()) + ".jpg";
		qDebug() << insertimgfile;
		syntunnelcorrect->saveImage(insertimgfile);

		bool ret1;
        ret = outputAccess->outputSingleTunnel(ClientSetting::getSettingInstance()->getOutputTunnelDataModel(ret1), &singleTunnelModel, outputfilename, insertimgfile);
        
        statusShow(outputfilename, ret, true);
    }
    else // 多隧道综合
    {
        // 起讫站名，最大外轨超高，最低接触网高度赋值
        multiTunnelsModel.setWaiGuiChaoGao(waiguichaogao);
        multiTunnelsModel.setJieChuWangGaoDu(jiechuwanggaodu);

        // 获得输出文件名
        QString datetimestr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
        QString openFileDir = ClientSetting::getSettingInstance()->getParentPath() + "/output/" + multiTunnelsModel.getLineName() + "-" + QObject::tr("区段综合") + "_" + datetimestr;
        outputfilename = QFileDialog::getSaveFileName(this, tr("导出到文件"),openFileDir, tr("EXCEL (*.xls)"));
        
        // 保存图片
        insertimgfile = QString(openFileDir.constData()) + ".jpg";
        syntunnelcorrect->saveImage(insertimgfile);

        // 输出到excel
        ret = outputAccess->outputMultiTunnels(&multiTunnelsModel, outputfilename, insertimgfile);
        
        statusShow(outputfilename, ret, true);
    }
}

/**
 * 当多隧道综合时，批量输出各个隧道的限界图表到EXCEL文件
 */
void OutputClearanceWidget::exportAllToExcels()
{
    int ret = QMessageBox::information(this, tr("输出到Excel"), tr("确认批量输出以上各个隧道的限界图表到excel？"), QMessageBox::Ok | QMessageBox::No);
    switch (ret)
    {
        case QMessageBox::No:
            return;
        case QMessageBox::Ok:
        {
            // 选择创建目录，默认为刚刚创建的目录
            QString openFileDir = ClientSetting::getSettingInstance()->getParentPath() + "/output/";
            QFileDialog *fd = new QFileDialog(this, tr("全部导出到Excel，请选择导出目录"));
            fd->setFileMode(QFileDialog::DirectoryOnly);
            fd->setDirectory(openFileDir);
            if(fd->exec() == QFileDialog::Accepted) // ok
            {
                QStringList folders = fd->selectedFiles();
                if(folders.size() <= 0)
                {
                    ui->status->setText(tr("全部导出到Excel终止，目录未选择。"));
                    delete fd;
                    return;
                }
                openFileDir = folders[0];
                qDebug() << "directory" << openFileDir;
                QString outputfilename = openFileDir;

                // 设置输出方式为Excel
                outputAccess->initOutput(LzOutputAccess::OutputInExcel);

                // 遍历隧道准备单隧道数据输出
                std::list<_int64>::iterator it = multiTunnelsModel.tasktunnelids.begin();
                _int64 tmptasktunnelid = -1;
                int tmptunnelid = -1;
                QString tmptunnelname;
                QString tmpdate;
                QString tmplinename;
                QString insertimgfilepath = ClientSetting::getSettingInstance()->getParentPath() + "/output/";
                QString tmpinsertimgfile;
                while (it != multiTunnelsModel.tasktunnelids.end())
                {
                    tmptasktunnelid = (*it);
                    // 根据采集隧道ID得到隧道基本ID
                    TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelInfo(tmptasktunnelid, tmptunnelid, tmptunnelname, tmpdate, tmplinename);
                    // 设置隧道基本信息
                    ClientSetting::getSettingInstance()->setOutputTunnelDataModel(tmptunnelid);

                    // 从数据库中加载当个隧道限界数据
                    singleTunnelModel.initClearanceDatas(tmptasktunnelid);
                    ret = singleTunnelModel.loadsynthesisdata();
                    
                    // 更新限界图
                    singleMultiMode = SingleOrMultiSelectionMode::Single_Mode;
                    clearanceImagePreview();
                    singleMultiMode = SingleOrMultiSelectionMode::Multi_Mode;

                    outputfilename = openFileDir + "/" + singleTunnelModel.getTaskTunnelInfoFormat() + ".xls";

                    // 保存图片
                    tmpinsertimgfile = insertimgfilepath + singleTunnelModel.getTaskTunnelInfoFormat() + ".jpg";
                    syntunnelcorrect->saveImage(tmpinsertimgfile);

                    bool ret1;
                    ret = outputAccess->outputSingleTunnel(ClientSetting::getSettingInstance()->getOutputTunnelDataModel(ret1), &singleTunnelModel, outputfilename, tmpinsertimgfile);
        
                    statusShow(outputfilename, ret, true);
                    it++;
                }
                clearanceImagePreview();
            }
            else
            {
                ui->status->setText(tr("全部导出到Excel终止，目录未选择。"));
                delete fd;
                return;
            }
            delete fd;
        }
    }
}

/**
 * statuslabel显示状态
 */
void OutputClearanceWidget::statusShow(QString outfilename, int retval, bool isappend)
{
    QString statusstr;

    switch (retval)
    {
        case 0: statusstr = QObject::tr("限界图表输出到文件%1成功").arg(outfilename);break;
        case 1: statusstr = QObject::tr("限界图表输出到文件%1失败，模板文件无法打开").arg(outfilename);break;
        case 2: statusstr = QObject::tr("限界图表输出到文件%1失败，目标文件正在占用").arg(outfilename);break;
        case 3: statusstr = QObject::tr("限界图表输出到文件%1失败，待插入限界图不存在").arg(outfilename);break;
        default:statusstr = QObject::tr("限界图表输出到文件%1失败").arg(outfilename);break;
    }

    // 显示状态
    if (isappend)
        ui->status->append(statusstr);
    else
        ui->status->setText(statusstr);
}


void OutputClearanceWidget::minheightChanged(QString a)
{
    if (maketable)
    {
        return;
    }
    hasedit = true;
}

void OutputClearanceWidget::saveItemChanged(QTableWidgetItem* item)
{
    /* 如果正在建表, 不执行槽函数 */
    if (maketable)
    {
        return;
    }

    int row = item->row();
    int column = item->column();
    
    int key = ui->tableWidget->verticalHeaderItem(row)->data(Qt::DisplayRole).toInt();
    int value = item->text().toInt();
    if (value == 0)
    {
        QMessageBox::StandardButton rb = QMessageBox::question(NULL,tr("警告"),tr("您输入的是无效数字，应输入>0的整数"),QMessageBox::Yes | QMessageBox::No);

        if (rb == QMessageBox::Yes || rb == QMessageBox::No)        
            return;
    }
    //
    //qDebug() << "row:" << row << ", verticalHeaderItem:" << ui->tableWidget->verticalHeaderItem(row)->data(Qt::DisplayRole).toString();
    //qDebug() << "column:" << column << ", horizontalHeaderItem:" << ui->tableWidget->horizontalHeaderItem(column)->data(Qt::DisplayRole).toString();
    //qDebug() << item->text().toInt() << " " << item->text();

    switch (column)
    {
        case Straight_Left_Val - 1:        singleTunnelModel.getClearanceStraightData().getMaps().at(key).leftval = value;    break;
        case Straight_Right_Val - 1:       singleTunnelModel.getClearanceStraightData().getMaps().at(key).rightval = value;   break;  
        case Straight_Left_Pos - 1:        singleTunnelModel.getClearanceStraightData().getMaps().at(key).leftpos = value;    break; 
        case Straight_Right_Pos - 1:       singleTunnelModel.getClearanceStraightData().getMaps().at(key).rightpos = value;   break; 
        case LeftCurve_Left_Val - 1:       singleTunnelModel.getClearanceLeftData().getMaps().at(key).leftval = value;    break;
        case LeftCurve_Right_Val - 1:      singleTunnelModel.getClearanceLeftData().getMaps().at(key).rightval = value;   break;  
        case LeftCurve_Left_Radius - 1:    singleTunnelModel.getClearanceLeftData().getMaps().at(key).leftradius = value; break;
        case LeftCurve_Right_Radius - 1:   singleTunnelModel.getClearanceLeftData().getMaps().at(key).rightradius = value;break;
        case LeftCurve_Left_Pos - 1:       singleTunnelModel.getClearanceLeftData().getMaps().at(key).leftpos = value;    break; 
        case LeftCurve_Right_Pos - 1:      singleTunnelModel.getClearanceLeftData().getMaps().at(key).rightpos = value;   break; 
        case RightCurve_Left_Val - 1:      singleTunnelModel.getClearanceRightData().getMaps().at(key).leftval = value;    break;
        case RightCurve_Right_Val - 1:     singleTunnelModel.getClearanceRightData().getMaps().at(key).rightval = value;   break;  
        case RightCurve_Left_Radius - 1:   singleTunnelModel.getClearanceRightData().getMaps().at(key).leftradius = value; break;
        case RightCurve_Right_Radius - 1:  singleTunnelModel.getClearanceRightData().getMaps().at(key).rightradius = value;break;
        case RightCurve_Left_Pos - 1:      singleTunnelModel.getClearanceRightData().getMaps().at(key).leftpos = value;    break; 
        case RightCurve_Right_Pos - 1:     singleTunnelModel.getClearanceRightData().getMaps().at(key).rightpos = value;   break; 
        default: break;
    }
    hasedit = true;
}

void OutputClearanceWidget::canBeModified()
{
    hasedit = false;
    canSave(true);
}

void OutputClearanceWidget::canSave(bool can)
{
    if (can)
    {
        ui->tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->lineEdit_minHeight->setEnabled(true);
    }
    else
    {
        ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->lineEdit_minHeight->setEnabled(false);
    }
    ui->modifyButton->setEnabled(!can);
    ui->saveButton->setEnabled(can);
}

void OutputClearanceWidget::saveChanges()
{
    QString currentcollectdate = ui->lineEdit_collectdate->text();
    long long tasktunnelid = singleTunnelModel.getTaskTunnelid();
    QString tunnelname = ui->lineEdit_name->text();
    int tunnelid = singleTunnelModel.getTunnelid();
    __int64 taskid = TaskDAO::getTaskDAOInstance()->getTaskID(tunnelid, currentcollectdate);

    qDebug() << "collectdate:" << currentcollectdate << ", tunnelid:" << tunnelid << ", tunnelname:" << tunnelname << ", taskid:" << taskid << ", tasktunnelid:" << tasktunnelid;

    ClearanceData & datas = singleTunnelModel.getClearanceStraightData();
    ClearanceData & datal = singleTunnelModel.getClearanceLeftData();
    ClearanceData & datar = singleTunnelModel.getClearanceRightData();


    if (tasktunnelid <= 0)
    {
        return;
    }
    
    if (tasktunnelid > 0)
    {
        int height = ui->lineEdit_minHeight->text().toInt();
        datas.setMinCenterHeight(height);
        datal.setMinCenterHeight(height);
        datar.setMinCenterHeight(height);
        int ret1 = ClearanceOutputDAO::getClearanceOutputDAOInstance()->clearanceDataToDBData(datas, tasktunnelid, ClearanceType::Straight_Smallest);
        int ret2 = ClearanceOutputDAO::getClearanceOutputDAOInstance()->clearanceDataToDBData(datal, tasktunnelid, ClearanceType::LeftCurve_Smallest);
        int ret3 = ClearanceOutputDAO::getClearanceOutputDAOInstance()->clearanceDataToDBData(datar, tasktunnelid, ClearanceType::RightCurve_Smallest);

        qDebug() << "clearanceDataToDBData" << ret1 << ret2 << ret3;
        //straightdata.showMaps();

        ui->statusArea->append(QObject::tr("重新保存隧道采集任务%1并添加到数据库完成").arg(tunnelname));
    }
    else 
    {
        ui->statusArea->append(QObject::tr("重新保存隧道采集任务%1失败，隧道采集任务（工程中的隧道任务）找不到ID为%2的tasktunnel").arg(tunnelname).arg(tasktunnelid));
    }

    ui->saveButton->setEnabled(false);
    ui->modifyButton->setEnabled(true);
    hasedit = false;
    canSave(false);

    // 更新限界图
    clearanceImagePreview();
}