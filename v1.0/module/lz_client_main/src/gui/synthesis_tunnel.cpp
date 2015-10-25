#include "synthesis_tunnel.h"
#include "ui_synthesis_tunnel.h"

#include "clientprogram.h"
#include "setting_client.h"
#include "tunneldatamodel.h"

#include "lz_project_access.h"
#include "xmlcheckedtaskfileloader.h"
#include "checkedtask.h"
#include "checkedtask_list.h"

#include "LzSynth.h"
#include "daoclearanceoutput.h"
#include "daotasktunnel.h"
#include "daotask.h"
#include <QDebug>
#include <QDir>
#include <QMessageBox>

/**
 * 隧道综合选择界面类声明
 *
 * @author 范翔
 * @version 1.0.0
 * @date 20140403
 */
SynthesisTunnelWidget::SynthesisTunnelWidget(QWidget *parent) :
    QWidget(parent),
    singleTunnelModel(ClientSetting::getSettingInstance()->getSingleTunnelModel()),
    ui(new Ui::SynthesisTunnelWidget) 
{
    ui->setupUi(this);

    // 帧间隔里程系数
    interframe_mile = 0.5103;

    // 文件名赋初值
    filename = "";
    // 隧道编号初始为-1
    tunnelid = -1;
    currentcollectdate = "";

	current_startframeno = -1;
	current_endframeno = -1;

    connect(ui->editTunnelButton, SIGNAL(clicked()), this, SLOT(turnToEditWidget()));
	connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(frameSearchInInterestArea()));
   
    scrollArea = NULL;
	syntunnelcorrect = NULL;
    datatype= AllType;

    // 画图，进行修正
    scrollArea = new QScrollArea(ui->graphicsView);
    syntunnelcorrect = new ClearanceImage(ui->graphicsView);//生成对象第一次绘图
    // 调整scroll的滚动条大小范围
	scrollArea->setWidget(syntunnelcorrect);
	ui->verticalLayout->addWidget(scrollArea);

	//@zengwang 2015年9月2日
	connect(ui->submitWidthButton,SIGNAL(clicked()),this,SLOT(setNewWidth()));
	connect(ui->submitHeightButton,SIGNAL(clicked()),this,SLOT(setNewCenterHeight()));
	connect(ui->setDirectionValComboBox, SIGNAL(currentIndexChanged(int)),this, SLOT(setDirectionVal()));

	//@zengwang 2015年9月2日

	//@zengwang 2015年10月12日
	//connect(ui->confirmButton, SIGNAL(clicked()), this, SLOT(manualSelectData()));

	// 实时获得画图界面的鼠标位置
    connect(syntunnelcorrect, SIGNAL(sendMousePos(int, int)), this, SLOT(getMousePos(int, int)));

    connect(syntunnelcorrect, SIGNAL(sendInterestRectangle(float, float, float, float)), this, SLOT(getInterestRectangle(float, float, float, float)));

    connect(syntunnelcorrect, SIGNAL(LineTypeCount(int,int,int)), this,SLOT(setLineType(int,int,int)));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(activatecombox()));

    ui->horizontalSlider->setRange(10, 150);
    ui->spinBox->setRange(10, 150);
    ui->spinBox->setValue(40);
    connect(ui->spinBox,SIGNAL(valueChanged(int)),ui->horizontalSlider,SLOT(setValue(int)));
    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),ui->spinBox,SLOT(setValue(int)));
    //当spinBox的值改变时,发送信号到DrawImage界面
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(getspinBoxvalue(int)));
    // 根据scale改变图形界面的大小
    connect(this,SIGNAL(sendspinBoxvalue(int)),syntunnelcorrect,SLOT(UpdateScaleRelativeParameter(int)));
    // 当widget大小改变时，告知外层界面容器，更新scrollbar信号槽
    //connect(testsix, SIGNAL(resizeWidget()), this, SLOT(resizeGraphWidget()));

    connect(ui->carriageDirectionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(carriageDirectionChanged(int)));
    carriagedirectionlock = false;

    // 添加键盘快捷键，CTRL+上键放大，CTRL+向下暂停
    QAction* actionZoomIn = new QAction(this);
    actionZoomIn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up));
    QAction* actionZoomOut = new QAction(this);
    actionZoomOut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down));
	this->addAction(actionZoomIn);
    this->addAction(actionZoomOut);
    // 添加键盘快捷键信号槽
    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(KeyZoomIn()));
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(KeyZoomOut()));

    // 重新综合计算
    connect(ui->recalculateButton, SIGNAL(clicked()), this, SLOT(calculateSynthesisData()));

    // 显示高度辅助线
    connect(ui->heighthelplineButton,SIGNAL(toggled(bool)),this,SLOT(showhelpline(bool)));
    list<int> Item = OutputHeightsList::getOutputHeightsListInstance()->list();
    syntunnelcorrect->setInterestHeights(Item);

    TopLeftPoint = QPointF(0,0);
    BottomRightPoint = QPointF(0,0);


	isNormalTravel = true;

}

SynthesisTunnelWidget::~SynthesisTunnelWidget()
{
    delete ui;
	if(syntunnelcorrect != NULL)
		delete syntunnelcorrect;
    if(scrollArea != NULL)
        delete scrollArea;
}

void SynthesisTunnelWidget::activatecombox()
{
    int index = ui->comboBox->currentIndex();
    if (index == 0)
        datatype = AllType;
    else if (index == 1)
        datatype = StraightType;
    else if (index == 2)
        datatype = LeftType;
    else
        datatype = RightType;
    syntunnelcorrect->setImageType(datatype);
}

void SynthesisTunnelWidget::setLineType(int x1,int x2,int x3)
{
    this->rowcount = x1;
    this->row1count = x2;
    this->row2count = x3;
}


void SynthesisTunnelWidget::showhelpline(bool ishow)
{
    syntunnelcorrect->setShowheightline(ishow);
    syntunnelcorrect->update();
}

/**
 * 转发消息
 */
void SynthesisTunnelWidget::getInterestRectangle(float topleftx, float toplefty, float bottomrightx, float bottomrighty)
{
    TopLeftPoint.setX(topleftx);
    TopLeftPoint.setY(toplefty);
    BottomRightPoint.setX(bottomrightx);
    BottomRightPoint.setY(bottomrighty);

    emit sendRectPoint(topleftx, toplefty, bottomrightx, bottomrighty, currentcarriagedirection);
}


// 更新计算进度条
void SynthesisTunnelWidget::initProgressBar(long long start, long long end)
{
    ui->calcuProgress->setVisible(true);
    ui->progressBar_cal01->setMinimum(start);
    ui->progressBar_cal01->setMaximum(end);

    // 禁止点击
    ui->recalculateButton->setEnabled(false);
    ui->comboBox->setEnabled(false);
    ui->carriageDirectionComboBox->setEnabled(false);
    ui->searchButton->setEnabled(false);
    ui->finishButton->setEnabled(false);
    ui->editTunnelButton->setEnabled(false);


	//@zengwang 2015年10月7日
	ui->leftRightValidComboBox->setEnabled(false);

	//@zengwang 2015年10月11日
	ui->setDirectionValComboBox->setEnabled(false);
//	ui->confirmButton->setEnabled(false);
	ui->submitWidthButton->setEnabled(false);
	ui->submitHeightButton->setEnabled(false);


    update();
}

// 更新计算进度条
void SynthesisTunnelWidget::updateProgressBar(long long current)
{
    ui->progressBar_cal01->setValue(current);
}

void SynthesisTunnelWidget::endProgressBar()
{
    ui->calcuProgress->setVisible(false);

    // 恢复点击
    ui->recalculateButton->setEnabled(true);
    ui->comboBox->setEnabled(true);
    ui->carriageDirectionComboBox->setEnabled(true);
    ui->searchButton->setEnabled(true);
    ui->finishButton->setEnabled(true);
    ui->editTunnelButton->setEnabled(true);

	//@zengwang 2015年10月7日
	ui->leftRightValidComboBox->setEnabled(true);

	//@zengwang 2015年10月11日
	ui->setDirectionValComboBox->setEnabled(true);
	//ui->confirmButton->setEnabled(true);
	ui->submitWidthButton->setEnabled(true);
	ui->submitHeightButton->setEnabled(true);

    update();
}

/**
 * 修改车厢方向
 */
void SynthesisTunnelWidget::carriageDirectionChanged(int index)
{
    if (carriagedirectionlock)
        return;

    QString abc = QObject::tr("一致（同向）");
    currentcarriagedirection = true;
    if (index == 1)
    {
        abc = QObject::tr("不一致（反向）");
        currentcarriagedirection = false;
    }

	//@zengwang 2015年10月7日添加
	/*

	bool ret;
    TunnelDataModel *tunnelDataModel = ClientSetting::getSettingInstance()->getCorrectTunnelDataModel(ret);
	ClearanceData & straightdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceStraightData();
    ClearanceData & leftdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceLeftData();
    ClearanceData & rightdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceRightData();
	bool isDoubleLine = tunnelDataModel->getIsDoubleLine();
	bool isAccord = true;
	if(isDoubleLine)
	{
		if(index == 0)
		{
			//index为0，表示采集方向与出表方向一致，只显示出左边数据即可
			straightdata.resetLeftOrRight(isAccord);
			leftdata.resetLeftOrRight(isAccord);
			rightdata.resetLeftOrRight(isAccord);
			ui->statusArea->append(QObject::tr("采集方向与出表方向一致，只显示左边数据"));

		}
		else if(index == 1)
		{
			//index为1，表示采集方向与出表方向不一致，只显示右边数据
			isAccord = false;
			straightdata.resetLeftOrRight(isAccord);
			leftdata.resetLeftOrRight(isAccord);
			rightdata.resetLeftOrRight(isAccord);
			ui->statusArea->append(QObject::tr("采集方向与出表方向不一致，只显示右边数据"));

		}
		else
		{
			ui->statusArea->append(QObject::tr("采集方向与出表方向出错"));
			return;
		}
	}
	else
	{
			straightdata.swapLeftAndRight();
			leftdata.swapLeftAndRight();
			rightdata.swapLeftAndRight();
	}
	*/




    QMessageBox::StandardButton rb = QMessageBox::warning(NULL, tr("警告"),tr("要将本条采集隧道配置为【车厢方向与出表方向%1】（一般由于计划配置时出错），确认？").arg(abc), QMessageBox::Yes | QMessageBox::No);
    if (rb == QMessageBox::Yes)
    {
        bool ret = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main).setTunnelCarriageDirection(tunnelid, currentcarriagedirection);
        if (!ret)
        {
            ui->statusArea->append(QObject::tr("将隧道%1配置为【车厢%2向采集】失败，找不到隧道ID为%3").arg(tunnelname).arg(abc).arg(tunnelid));
            return;
        }
        QString path = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main);
        ProjectModel currentProjectModel = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Main);
        //从实际隧道文件的的xml中解析实际隧道数据和校正之后的数据到listcollect中
        QString checkedFile = path + "/" + currentProjectModel.getCheckedFilename();
        QString realFile = path + "/" + currentProjectModel.getRealFilename();
        qDebug() << "path:" << path << "checked filename: " << checkedFile << "real filename: " << realFile;

        XMLCheckedTaskFileLoader * checktask = new XMLCheckedTaskFileLoader(tr(checkedFile.toLocal8Bit().data()));
        ret = checktask->saveFile(LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main));//生成listtask
        delete checktask;

        ui->statusArea->append(QObject::tr("将隧道%1配置为【车厢方向与出表方向%2】成功").arg(tunnelname).arg(abc));
    }
}

/**
 * 添加实际采集“隧道”记录文件，并显示
 */
bool SynthesisTunnelWidget::loadCheckedTaskTunnelData() 
{
    QString path = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main);
    ProjectModel currentProjectModel = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Main);
    //从实际隧道文件的的xml中解析实际隧道数据和校正之后的数据到listcollect中
    QString checkedFile = path + "/" + currentProjectModel.getCheckedFilename();
    QString realFile = path + "/" + currentProjectModel.getRealFilename();
    qDebug() << "path:" << path << "checked filename: " << checkedFile << "real filename: " << realFile;
    XMLCheckedTaskFileLoader * checktask = new XMLCheckedTaskFileLoader(tr(checkedFile.toLocal8Bit().data()));
    bool ret = checktask->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main));//生成listtask
    delete checktask;
    return ret;
}

/**
 * 加载隧道基本数据
 */
void SynthesisTunnelWidget::loadBasicTunnelData()
{
    ui->lineEdit_collectdate->setText(currentcollectdate);

    carriagedirectionlock = true;
    if (currentcarriagedirection)
        ui->carriageDirectionComboBox->setCurrentIndex(0);
    else
        ui->carriageDirectionComboBox->setCurrentIndex(1);
    carriagedirectionlock = false;



	if(isNormalTravel == true)
		ui->lineEdit_isNormalTravel->setText(tr("正常行驶"));
	else if(isNormalTravel == false)
		ui->lineEdit_isNormalTravel->setText(tr("非正常行驶"));
	else
		ui->lineEdit_isNormalTravel->setText(tr(""));

	
    bool ret;
    TunnelDataModel *tunnelDataModel = ClientSetting::getSettingInstance()->getCorrectTunnelDataModel(ret);
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


		//bool isNormalTravel = tunnelDataModel->getIsNormalTravel();
        //qDebug() << "isNewLine:" << isNewLine << ", lineType:" << lineType << ", isDoubleLine:" << isDoubleLine << ", isDownLink:" << isDownLink;

        qDebug() << "isNewLine:" << isNewLine << ", lineType:" << lineType << ", isDoubleLine:" << isDoubleLine << ", isDownLink:" << isDownLink;

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
        {
            ui->lineEdit_lineType->setText(tr(""));
            syntunnelcorrect->setfloornumber(OutType_B_DianLi);
        }

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


    }
    ui->calcuProgress->setVisible(false);
}

/**
 * 加载综合好数据
 */
void SynthesisTunnelWidget::loadSynthesisData()
{
    __int64 tasktunnelid = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelID(tunnelid, currentcollectdate);

    syntunnelcorrect->clearPointsArrayAll();

    if (tasktunnelid < 0)
    {

        return;
    }

    // 选择并加载数据后可以输出预览
    ClientSetting::getSettingInstance()->getSingleTunnelModel().initClearanceDatas(tasktunnelid);
    bool ret = ClientSetting::getSettingInstance()->getSingleTunnelModel().loadsynthesisdata();
   
    ClearanceData & straightdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceStraightData();
    ClearanceData & leftdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceLeftData();
    ClearanceData & rightdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceRightData();

    bool hasstraight = true; //ClientSetting::getSettingInstance()->getSingleTunnelModel().getHasStraight();
    bool hasleft = ClientSetting::getSettingInstance()->getSingleTunnelModel().getHasLeft();
    bool hasright = ClientSetting::getSettingInstance()->getSingleTunnelModel().getHasRight();

    qDebug() << "loadSynthesisData: tasktunnelid = " << tasktunnelid << ", hasstraight = " << hasstraight << ", hasleft = " << hasleft << ", hasright" << hasright;
   
    if (hasstraight)
    {
        syntunnelcorrect->initPointsArray(straightdata.getMaps().size(), Curve_Straight);
        loadGaugeImage(straightdata, Curve_Straight);
    }
    if (hasleft)
    {
        syntunnelcorrect->initPointsArray(leftdata.getMaps().size(), Curve_Left);
        loadGaugeImage(leftdata, Curve_Left);
    }
    if (hasright)
    {
        syntunnelcorrect->initPointsArray(rightdata.getMaps().size(), Curve_Right);
        loadGaugeImage(rightdata, Curve_Right);
    }
}

/**
 * 重新综合
 */
void SynthesisTunnelWidget::calculateSynthesisData()
{
    // 隧道综合
    QString projectpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main);
    QByteArray tmpba = projectpath.toLocal8Bit();
    string projectpathstd(tmpba.constData());
    ProjectModel currentProjectModel = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Main);
    bool ret;
    TunnelDataModel * m = ClientSetting::getSettingInstance()->getCorrectTunnelDataModel(ret); 
    if (ret)
    {
        m->loadTunnelData();
        qDebug() << "id" << m->getId() << "name" << QString::fromLocal8Bit(m->getName().c_str()) << "idstd" << m->getIdStd().c_str()
			<< "isdouble" << m->getIsDoubleLine() << "isdownlink" << m->getIsDownlink() << "isnormal" << m->getIsNormalTravel() << "isnew" << m->getIsNew()
                 << "linetype" << m->getLineType() << "endpoint" << m->getEndPoint() << "startpoint" << m->getStartPoint()
                 << "lineid" << m->getLineid() << "linename" << QString::fromLocal8Bit(m->getLinename().c_str());
        qDebug() << "list size:" << m->getCurveList().size() << m->getNumberOfLeftCurves() << m->getNumberOfRightCurves() << m->getNumberOfStrights();
    }
    ret = loadCheckedTaskTunnelData();
    if (ret)
    {
        CheckedTunnelTaskModel tmp = CheckedTunnelTaskModel();
        LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main).getCheckedTunnelModel(tunnelid, tmp);
        
        // 打开文件名
        string tunnelheightssynname = filename;
        qDebug() << "currentcollectdate:" << currentcollectdate;

        __int64 tasktunnelid = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelID(tunnelid, currentcollectdate);
        __int64 taskid = TaskDAO::getTaskDAOInstance()->getTaskID(tunnelid, currentcollectdate);

        if (tasktunnelid < 0)
        {
            ui->statusArea->append(QObject::tr("首次综合计算该任务组(任务工程)，创建新计算任务组。。。"));
            // 没有这个任务组
            if (taskid < 0)
                taskid = TaskDAO::getTaskDAOInstance()->addTask(tmp.planTask.pulsepermeter, currentcollectdate, currentcarriagedirection, QObject::tr(tmp.planTask.tunnelname.c_str()));

            if (taskid < 0)
            {
                ui->statusArea->append(QObject::tr("创建新计算任务组失败！重新综合计算文件%1失败，数据库中无法添加采集日期为%2的任务隧道组").arg(QObject::tr(tunnelheightssynname.c_str())).arg(currentcollectdate));
                return;
            }
            else
            {
                ui->statusArea->append(QObject::tr("创建新计算任务组成功！"));
            }

            ui->statusArea->append(QObject::tr("创建新计算隧道任务（工程中的隧道任务）。。。"));
            // 有当天采集的任务组，但没有这个隧道任务
            if (TaskTunnelDAO::getTaskTunnelDAOInstance()->addTaskTunnel(taskid, tunnelid, interframe_mile, currentcarriagedirection, tmp.planTask.isnormal, 0) == 0)
            {
                tasktunnelid = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelID(tunnelid, currentcollectdate);
                ui->statusArea->append(QObject::tr("创建新计算隧道任务（工程中的隧道任务）成功，tasktunnelid=%1！").arg(tasktunnelid));
            }
            else
            {
                ui->statusArea->append(QObject::tr("创建新计算隧道任务（工程中的隧道任务）失败！重新综合计算文件%1失败，数据库中采集日期为%2的任务组存在，但无法添加采集隧道").arg(QObject::tr(tunnelheightssynname.c_str())).arg(currentcollectdate));
                return;
            }
        }
        
        if (tasktunnelid >= 0)
        {
            int ret0 = TaskTunnelDAO::getTaskTunnelDAOInstance()->updateTaskTunnel(tasktunnelid, taskid, tunnelid, interframe_mile, currentcarriagedirection, tmp.planTask.isnormal, 0);

            //straightdata.showMaps();
            ClearanceData & straightdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceStraightData();
            ClearanceData & leftdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceLeftData();
            ClearanceData & rightdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceRightData();

            LzSynthesis lzsyn;

            // 信号槽
            connect(&lzsyn, SIGNAL(initfc(long long, long long)), this, SLOT(initProgressBar(long long, long long)));
            connect(&lzsyn, SIGNAL(currentfc(long long)), this, SLOT(updateProgressBar(long long)));
				
			// 左右侧有效
			int leftrightvalid = ui->leftRightValidComboBox->currentIndex();


            // @TODO 系数0.5103应改为外部配置文件输入
			//lzsyn.initSynthesis(tunnelheightssynname, m, &tmp, interframe_mile, currentcarriagedirection, this->current_startframeno, this->current_endframeno, leftrightvalid);
            lzsyn.initSynthesis(tunnelheightssynname, m, &tmp, interframe_mile, true, this->current_startframeno, this->current_endframeno, leftrightvalid);
			bool hasstraight = false, hasleft = false, hasright = false;
			lzsyn.synthesis(straightdata, leftdata, rightdata, hasstraight, hasleft, hasright);

            int ret1 = ClearanceOutputDAO::getClearanceOutputDAOInstance()->clearanceDataToDBData(straightdata, tasktunnelid, ClearanceType::Straight_Smallest);
            int ret2 = ClearanceOutputDAO::getClearanceOutputDAOInstance()->clearanceDataToDBData(leftdata, tasktunnelid, ClearanceType::LeftCurve_Smallest);
            int ret3 = ClearanceOutputDAO::getClearanceOutputDAOInstance()->clearanceDataToDBData(rightdata, tasktunnelid, ClearanceType::RightCurve_Smallest);

            qDebug() << "clearanceDataToDBData" << ret1 << ret2 << ret3;
            //straightdata.showMaps();

							
			
            ui->statusArea->append(QObject::tr("重新综合计算文件%1并添加到数据库完成").arg(QObject::tr(tunnelheightssynname.c_str())));
        }
        else 
        {
            ui->statusArea->append(QObject::tr("重新综合计算文件%1失败，计算隧道任务（工程中的隧道任务）找不到ID为%2的tasktunnel").arg(QObject::tr(tunnelheightssynname.c_str()).arg(tasktunnelid)));
        }

        // 进度条
        endProgressBar();

        // 重新显示
        loadSynthesisData();
    }
    else
        ;
}

void SynthesisTunnelWidget::loadGaugeImage(ClearanceData& data, CurveType newtype)
{
    // 如果车厢反向，左右对调重新写会
    //if (!currentcarriagedirection)
    //    data.swapLeftAndRight();

    bool ret1= syntunnelcorrect->ClearanceDataToPointsArray(data, newtype);

    if(ret1)
        syntunnelcorrect->update();//即更新绘图
  



	//@zengwang  2015年10月6日修改
	/*
    ui->red_label->setPixmap(QPixmap(ClientSetting::getSettingInstance()->getParentPath() + "/system/red.jpg"));
    ui->green_label->setPixmap(QPixmap(ClientSetting::getSettingInstance()->getParentPath() + "/system/black.jpg"));
    ui->blue_label->setPixmap(QPixmap(ClientSetting::getSettingInstance()->getParentPath() + "/system/blue.jpg"));
	*/

}

/**
 * 从我们画的感兴趣区域内查找限界半宽更小的“危险”帧数
 */
void SynthesisTunnelWidget::frameSearchInInterestArea()
{
    if (hasinitheightsval == false)
    {
        qDebug() << "can init clearance value map:";
        return;
    }

    qDebug() << "frameSearchInInterestArea   TopLeftPoint.x() = " << TopLeftPoint.x() << "TopLeftPoint.y() = " << TopLeftPoint.y() << "BottomRightPoint.x() = " << BottomRightPoint.x() << "BottomRightPoint.y() = " << BottomRightPoint.y();

    std::vector<BLOCK_KEY> keys;
    _int64 framecounter;
    double mile;
    float centerheight;
    SectionData sectiondata;
    bool ret;

    // 流式文件查看
    LzSerialStorageSynthesis * syn;
    bool isfileopen;
    // 打开文件
    try {
        // 调用读取指定高度流式文件类
        syn = new LzSerialStorageSynthesis();

        // 临时存储当前帧的高度数据
        ret = sectiondata.initMaps(); // 初始化高度，不可缺少
		if (ret == false)
		{
            qDebug() << "can init clearance value map:";
            isfileopen = false;
            return;
        }

        // 打开文件
        // 【注意！】设置缓存大小1M，默认1G太大
        syn->setting(100, 1024*1024, true);
        if (!syn->openFile(filename.c_str()))
        {
            qDebug() << "can not open file:" << QString::fromLocal8Bit(filename.c_str());
            isfileopen = false;
            return;
        }
        isfileopen = true;
        qDebug() << "open file:" << QString::fromLocal8Bit(filename.c_str());
        keys = syn->readKeys();
    }
    catch (LzException & ex)
    {
        qDebug() << "loadSynthesisDataFile exception:" << ex.what();
        return;
    }
    
	emit clearlistwidget();

    int fcindex;

	long long startfc = keys.at(0);
	long long endfc = keys.at(keys.size() - 1);
	if (this->current_startframeno != -1 && current_endframeno != -1)
	{
		startfc = current_startframeno;
		endfc = current_endframeno;
	}

    // 进度条
    initProgressBar(startfc, endfc);

	long long size = keys.size();
	if (this->current_startframeno != -1 && current_endframeno != -1)
		size = current_endframeno - current_startframeno + 1;

	for(int i = 0; i < size; i++)
	{
	    fcindex = i;
		bool ret1 = false;

		if (this->current_startframeno != -1 && current_endframeno != -1)
			ret1 = syn->retrieveMap(current_startframeno + fcindex);
		else
			ret1 = syn->retrieveMap(keys.at(fcindex));//表示显示第2帧(0代表第一帧,1代表第二帧....)

        ret = syn->readMap(framecounter, mile, centerheight, sectiondata);

		//@author 2015年9月2号
		//int  newheight = 0;
		//float newleftval = 0;
		//float newrightval = 0;

		

		//std::cout <<"newHeight = " << newHeight << " newLeftVal = " << newLeftVal << " newRightVal = " << newRightVal << std::endl;

		//sectiondata.updateToMapVals(newHeight, newLeftVal, newRightVal);
		
		//syn->rewriteMapV2(framecounter, mile, centerheight, sectiondata.getMaps());


		//@author 2015年9月2号



        //qDebug()<<"testframecount:"<<testframecount<<endl;
        // 把SectionData加到点数组中
        std::map <int,item>::iterator it = sectiondata.getMaps().begin();
        while(it != sectiondata.getMaps().end())
        {
            std::pair<int,item> pair = (*it);
            if ((pair.first >= BottomRightPoint.y()) && (pair.first <= TopLeftPoint.y()))
			{
                if (currentcarriagedirection)
                {
                    if (TopLeftPoint.x() < 0)
                    {
                        if (pair.second.left >= 0 && (pair.second.left >= (-1)*BottomRightPoint.x()) && (pair.second.left <= (-1)*TopLeftPoint.x()))
                        {
	                        emit updateFramesInSearchArea(framecounter, mile, 1);
	                        break;
                        }
                    }
                    else
                    {
                        if (pair.second.right >= 0 && (pair.second.right >= TopLeftPoint.x()) && (pair.second.right <= BottomRightPoint.x()))
                        {
	                        emit updateFramesInSearchArea(framecounter, mile, 1);
	                        break;
                        }
                    }
                }
                else
                {
                    if (TopLeftPoint.x() < 0)
                    {
                        if (pair.second.right >= 0 && (pair.second.right >= (-1)*BottomRightPoint.x()) && (pair.second.right <= (-1)*TopLeftPoint.x()))
                        {
	                        emit updateFramesInSearchArea(framecounter, mile, 1);
	                        break;
                        }
                    }
                    else
                    {
                        if (pair.second.left >= 0 && (pair.second.left >= TopLeftPoint.x()) && (pair.second.left <= BottomRightPoint.x()))
                        {
	                        emit updateFramesInSearchArea(framecounter, mile, 1);
	                        break;
                        }                        
                    }
                }
            }

            // 进度条
			if (this->current_startframeno != -1 && current_endframeno != -1)
				updateProgressBar(current_startframeno + fcindex);
			else
				updateProgressBar(keys.at(fcindex));

			it++;
	    }
	}
	//qDebug() << "find fc" << framecounter << "," << mile << endl; 
    
    // 关闭文件
    try {
        // 关闭文件
        syn->closeFile();
        isfileopen = false;
        qDebug() << "close file:" << QString::fromLocal8Bit(filename.c_str());
        delete syn;
    }
    catch (LzException & ex)
    {
        qDebug() << "closeSynthesisDataFile exception:" << ex.what(); 
    }

    // 进度条
    endProgressBar();
}

void SynthesisTunnelWidget::getspinBoxvalue(int value)
{
    int scalevalue=value;
    ui->spinBox->setValue(scalevalue);
    emit sendspinBoxvalue(scalevalue);
   
}
void SynthesisTunnelWidget::correctscale(int newsynscale)
{
    correct_scale=newsynscale;
    ui->spinBox->setValue(correct_scale);
    emit sendspinBoxvalue(correct_scale);
}

// 隧道综合界面的基础信息（显示缩放因子scale、pixel2mm、分中坐标原点x、y）
void SynthesisTunnelWidget::getparameterfromtest(int newscale, double newpixeltomm, int newwidthpixel, int neworiginy)
{
    this->scale=newscale;
    this->pixeltomm=newpixeltomm;
    this->widthpixel=newwidthpixel;
    this->originy=neworiginy;
    // 隧道综合界面的基础信息（显示缩放因子scale、pixel2mm、分中坐标原点x、y）
	emit Sendparametertocorrect_clearance( this->scale,this->pixeltomm,this->widthpixel,this->originy);
}

// 按快捷键槽函数（缩小）
void SynthesisTunnelWidget::KeyZoomOut()
{
    int v = ui->spinBox->value();
	if (++v <= ui->spinBox->maximum())
    {
        ui->spinBox->setValue(v);
		ui->horizontalSlider->setValue(v);
    }
}

// 按快捷键槽函数（放大）
void SynthesisTunnelWidget::KeyZoomIn()
{
    int v = ui->spinBox->value();
	if (--v >= ui->spinBox->minimum())
    {
        ui->spinBox->setValue(v);
		ui->horizontalSlider->setValue(v);
    }
}

void SynthesisTunnelWidget::on_finishButton_clicked()
{
    QString date = projectname.mid(projectname.size() - 8);
    ClientProgram::getClientProgramInstance()->finishModify(projectname, tunnelid, tunnelname, date, username);
    emit finish();
}

void SynthesisTunnelWidget::slotSelectedTunnelToSynthesis(int newtunnelid, QString signalfilename, bool isDouble, bool carriagedirect, bool isNormal, double distanceMode, long long startframeno, long long endframeno)
{
    tunnelid = newtunnelid;
    username = ClientSetting::getSettingInstance()->getCurrentUser();
    projectname =  ClientSetting::getSettingInstance()->getCurrentEditingProject();
    tunnelname = ClientSetting::getSettingInstance()->getCurrentEditingTunnel();
    currentcarriagedirection = carriagedirect;
    
	isNormalTravel = isNormal;

	isDoubleLine = isDouble;

    // @author 范翔
    interframe_mile = distanceMode;

	this->current_startframeno = startframeno;
	this->current_endframeno = endframeno;


	//@zengwang 2015年10月8号
	//isNormalTravel = m->getIsNormalTravel();
	//isNormalTravel = tmp.planTask.isnormal;

	ClearanceData & straightdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceStraightData();
    ClearanceData & leftdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceLeftData();
    ClearanceData & rightdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceRightData();

	if(isNormalTravel)         //判断是否正常行驶   正常行驶
	{
		if(isDoubleLine)      //判断是否为双线    双线
		{
			if(currentcarriagedirection)    //判断采集方向与出表方向是否一致     一致
			{
				//如果采集方向与出表方向一致时，数据保持不变，只显示左边数据,其中当currentcarriagedirection为true时，显示左边数据
				//leftdata.resetLeftOrRight(currentcarriagedirection);
				//rightdata.resetLeftOrRight(currentcarriagedirection);

				ui->leftRightValidComboBox->setCurrentIndex(1);
			}
			else      //采集方向与出表方向不一致
			{
				//如果采集方向与出表方向不一致时，先将左右数据进行对调，再显示左边数据（右边数据不显示）
				straightdata.swapLeftAndRight();
				leftdata.swapLeftAndRight();
				rightdata.swapLeftAndRight();
				//straightdata.resetLeftOrRight(!currentcarriagedirection);
				//leftdata.resetLeftOrRight(!currentcarriagedirection);
				//rightdata.resetLeftOrRight(!currentcarriagedirection);

				ui->leftRightValidComboBox->setCurrentIndex(1);
			}
		}
		else       //单线
		{
			if(currentcarriagedirection)     //采集与出表方向一致时，数据保持不变
				ui->leftRightValidComboBox->setCurrentIndex(0);
			else                            //采集与出表方向不一致时，将左右数据对调后进行显示
			{
				straightdata.swapLeftAndRight();
				leftdata.swapLeftAndRight();
				rightdata.swapLeftAndRight();

				ui->leftRightValidComboBox->setCurrentIndex(0);

			}

		}
	}
	else         //非正常行驶
	{
		if(isDoubleLine)            //如果是双线
		{
			if(currentcarriagedirection)   //采集与出表数据一致时，出右边数据
			{
				//straightdata.resetLeftOrRight(!currentcarriagedirection);
				//leftdata.resetLeftOrRight(!currentcarriagedirection);
				//rightdata.resetLeftOrRight(!currentcarriagedirection);

				ui->leftRightValidComboBox->setCurrentIndex(2);
			}
			else              //采集与出表数据不一致时，左右数据对调，出左边数据
			{
				straightdata.swapLeftAndRight();
				leftdata.swapLeftAndRight();
				rightdata.swapLeftAndRight();
				//straightdata.resetLeftOrRight(!currentcarriagedirection);
				//leftdata.resetLeftOrRight(!currentcarriagedirection);
				//rightdata.resetLeftOrRight(!currentcarriagedirection);

				ui->leftRightValidComboBox->setCurrentIndex(1);
			}
		}
		else           //单线不存在非正常行驶这种情况,不做任何处理
			ui->leftRightValidComboBox->setCurrentIndex(0);
	}




    // QDateTime time = QDateTime::fromString(currentProjectModel.getCreateDate(), "yyyy-MM-dd hh:mm:ss");  
    // currentcollectdate = time.toString("yyyyMMdd");
    currentcollectdate = projectname.right(8);
	qDebug() << projectname; 
    //qDebug() << "tunnelid = " << tunnelid << ", username = " << username << ", tunnelname = " << tunnelname << ", projectname = " << projectname << ", currentcollectdate = " << currentcollectdate;

    loadBasicTunnelData();
	// 加载文件并画图
    loadSynthesisData();

    QByteArray ba = signalfilename.toLocal8Bit();
    qDebug() << ba.constData();
    filename = string(ba.constData());
}

void SynthesisTunnelWidget::turnToEditWidget()
{
    if (!ClientSetting::getSettingInstance()->getCanEditing())
    {
        QMessageBox::warning(this, tr("提示"), tr("当前隧道不能修正"));
        return;
    }

    emit startFromFirst(currentcarriagedirection);
}

void SynthesisTunnelWidget::setNewCenterHeight()
{
	newCenterHeight = ui->labelEdit_newCenterHeight->text().toInt();
	qDebug() <<"currentcarriagedirection = " << currentcarriagedirection <<  " ui->setDirectionValComboBox->currentIndex() = " <<ui->setDirectionValComboBox->currentIndex() << endl;
	if (hasinitheightsval == false)
    {
        qDebug() << "can init clearance value map:";
        return;
    }
    
    std::vector<BLOCK_KEY> keys;
    _int64 framecounter;
    double mile;
    float centerheight;
    SectionData sectiondata;
    bool ret;

    // 流式文件查看
    LzSerialStorageSynthesis * syn;
    bool isfileopen;
    // 打开文件
    try {
        // 调用读取指定高度流式文件类
        syn = new LzSerialStorageSynthesis();

        // 临时存储当前帧的高度数据
        ret = sectiondata.initMaps(); // 初始化高度，不可缺少
		if (ret == false)
		{
            qDebug() << "can init clearance value map:";
            isfileopen = false;
            return;
        }

        // 打开文件
        // 【注意！】设置缓存大小1M，默认1G太大
        syn->setting(100, 1024*1024, true);
        if (!syn->openFile(filename.c_str()))
        {
            qDebug() << "can not open file:" << QString::fromLocal8Bit(filename.c_str());
            isfileopen = false;
            return;
        }
        isfileopen = true;
        qDebug() << "open file:" << QString::fromLocal8Bit(filename.c_str());
        keys = syn->readKeys();
    }
    catch (LzException & ex)
    {
        qDebug() << "loadSynthesisDataFile exception:" << ex.what();
        return;
    }
    
	emit clearlistwidget();

    int fcindex;

	long long startfc = keys.at(0);
	long long endfc = keys.at(keys.size() - 1);
	if (this->current_startframeno != -1 && current_endframeno != -1)
	{
		startfc = current_startframeno;
		endfc = current_endframeno;
	}

    // 进度条
    initProgressBar(startfc, endfc);

	long long size = keys.size();
	if (this->current_startframeno != -1 && current_endframeno != -1)
		size = current_endframeno - current_startframeno + 1;

	for(int i = 0; i < size; i++)
	{
	    fcindex = i;
		bool ret1 = false;

		if (this->current_startframeno != -1 && current_endframeno != -1)
			ret1 = syn->retrieveMap(current_startframeno + fcindex);
		else
			ret1 = syn->retrieveMap(keys.at(fcindex));//表示显示第2帧(0代表第一帧,1代表第二帧....)
    
		ret = syn->readMap(framecounter, mile, centerheight, sectiondata);

		//@author 2015年9月2号
		//int  newheight = 0;
		//float newleftval = 0;
		//float newrightval = 0;

		sectiondata.showMaps();

		//qDebug() << "newHeight = " << newHeight << " newLeftVal = " << newLeftVal << " newRightVal = " << newRightVal;
		//qDebug()  << "sectiondata.getMaps().at(newHeight).left = " << sectiondata.getMaps().at(newHeight).left;
		//qDebug()  << "sectiondata.getMaps().at(newHeight).right = " << sectiondata.getMaps().at(newHeight).right;
		//qDebug()  << "centerheight = " << centerheight << endl;
		//if(sectiondata.getMaps().at(newHeight).left < newLeftVal)
		//	sectiondata.getMaps().at(newHeight).left = newLeftVal;
		//if(sectiondata.getMaps().at(newHeight).right < newRightVal)
		//	sectiondata.getMaps().at(newHeight).right = newRightVal;
		if(centerheight < newCenterHeight)
			//centerheight = newCenterHeight;
			centerheight = -1;
		//qDebug() <<"centerheight = " << centerheight << " leftval = " << sectiondata.getMaps().at(newHeight).left << " rightVal = " << sectiondata.getMaps().at(newHeight).right << endl;
		//sectiondata.updateToMapVals(newHeight, sectiondata.getMaps().at(newHeight).left, sectiondata.getMaps().at(newHeight).right);
		
		syn->rewriteMapV2(framecounter, mile, centerheight, sectiondata.getMaps());


		//@author 2015年9月2号

        // 进度条
		if (this->current_startframeno != -1 && current_endframeno != -1)
			updateProgressBar(current_startframeno + fcindex);
		else
			updateProgressBar(keys.at(fcindex));

	}
	//qDebug() << "find fc" << framecounter << "," << mile << endl; 
    
    // 关闭文件
    try {
        // 关闭文件
        syn->closeFile();
        isfileopen = false;
        qDebug() << "close file:" << QString::fromLocal8Bit(filename.c_str());
        delete syn;
    }
    catch (LzException & ex)
    {
        qDebug() << "closeSynthesisDataFile exception:" << ex.what(); 
    }


    // 进度条
    endProgressBar();

	//重新显示
	loadSynthesisData();

}



void SynthesisTunnelWidget::setNewWidth()
{
	//QString newHeigth,newLeftVal,newRightVal;
	newHeight = ui->labelEdit_newHeight->text().toInt();
	
	//@zengwang 2015年10月7号
	//isValidHeight用来检查输入的高度值是不是正确的高度值，若是，则能正确地限定宽度；若不是，则应该在状态栏中提示用户，输入正确的高度再进行批量限宽
	bool isValidHeight = false;
	list<int>::iterator it = OutputHeightsList::getOutputHeightsListInstance()->heightsBegin();
    while (it != OutputHeightsList::getOutputHeightsListInstance()->heightsEnd())
    {
		if(*it == newHeight)
		{
			isValidHeight = true;
			break;
		}
        it++;
    }
	if(!isValidHeight)
	{
		ui->statusArea->append(tr("批量限宽的高度错误，请输入正确的批量限宽高度！"));
		return;
	}
	

	if(currentcarriagedirection)
	{
		if(ui->setDirectionValComboBox->currentIndex() == 0)
		{
			newLeftVal = ui->labelEdit_newWidthVal->text().toInt();
			newRightVal = 0;
		}
		else
		{
			newRightVal = ui->labelEdit_newWidthVal->text().toInt();
			newLeftVal = 0;
		}
			
	}
	else
	{
		if(ui->setDirectionValComboBox->currentIndex() == 0)
		{
			newRightVal = ui->labelEdit_newWidthVal->text().toInt();
			newLeftVal = 0;
		}
		else
		{
			newLeftVal = ui->labelEdit_newWidthVal->text().toInt();
			newRightVal = 0;
		}
			
	}
	qDebug() <<"currentcarriagedirection = " << currentcarriagedirection <<  " ui->setDirectionValComboBox->currentIndex() = " <<ui->setDirectionValComboBox->currentIndex() << endl;
	if (hasinitheightsval == false)
    {
        qDebug() << "can init clearance value map:";
        return;
    }
    
    std::vector<BLOCK_KEY> keys;
    _int64 framecounter;
    double mile;
    float centerheight;
    SectionData sectiondata;
    bool ret;

    // 流式文件查看
    LzSerialStorageSynthesis * syn;
    bool isfileopen;
    // 打开文件
    try {
        // 调用读取指定高度流式文件类
        syn = new LzSerialStorageSynthesis();

        // 临时存储当前帧的高度数据
        ret = sectiondata.initMaps(); // 初始化高度，不可缺少
		if (ret == false)
		{
            qDebug() << "can init clearance value map:";
            isfileopen = false;
            return;
        }

        // 打开文件
        // 【注意！】设置缓存大小1M，默认1G太大
        syn->setting(100, 1024*1024, true);
        if (!syn->openFile(filename.c_str()))
        {
            qDebug() << "can not open file:" << QString::fromLocal8Bit(filename.c_str());
            isfileopen = false;
            return;
        }
        isfileopen = true;
        qDebug() << "open file:" << QString::fromLocal8Bit(filename.c_str());
        keys = syn->readKeys();
    }
    catch (LzException & ex)
    {
        qDebug() << "loadSynthesisDataFile exception:" << ex.what();
        return;
    }
    
	emit clearlistwidget();

    int fcindex;

    long long startfc = keys.at(0);
	long long endfc = keys.at(keys.size() - 1);
	if (this->current_startframeno != -1 && current_endframeno != -1)
	{
		startfc = current_startframeno;
		endfc = current_endframeno;
	}

    // 进度条
    initProgressBar(startfc, endfc);

	long long size = keys.size();
	if (this->current_startframeno != -1 && current_endframeno != -1)
		size = current_endframeno - current_startframeno + 1;

	for(int i = 0; i < size; i++)
	{
	    fcindex = i;
		bool ret1 = false;

		if (this->current_startframeno != -1 && current_endframeno != -1)
			ret1 = syn->retrieveMap(current_startframeno + fcindex);
		else
			ret1 = syn->retrieveMap(keys.at(fcindex));//表示显示第2帧(0代表第一帧,1代表第二帧....)

        ret = syn->readMap(framecounter, mile, centerheight, sectiondata);

		//@author 2015年9月2号
		//int  newheight = 0;
		//float newleftval = 0;
		//float newrightval = 0;

		sectiondata.showMaps();

		qDebug() << "newHeight = " << newHeight << " newLeftVal = " << newLeftVal << " newRightVal = " << newRightVal;
		qDebug()  << "sectiondata.getMaps().at(newHeight).left = " << sectiondata.getMaps().at(newHeight).left;
		qDebug()  << "sectiondata.getMaps().at(newHeight).right = " << sectiondata.getMaps().at(newHeight).right;
		qDebug()  << "centerheight = " << centerheight << endl;
		if(sectiondata.getMaps().at(newHeight).left < newLeftVal)
			sectiondata.getMaps().at(newHeight).left = newLeftVal;
		if(sectiondata.getMaps().at(newHeight).right < newRightVal)
			sectiondata.getMaps().at(newHeight).right = newRightVal;
		if(centerheight < newCenterHeight)
			//centerheight = newCenterHeight;
			centerheight = -1;
		qDebug() <<"centerheight = " << centerheight << " leftval = " << sectiondata.getMaps().at(newHeight).left << " rightVal = " << sectiondata.getMaps().at(newHeight).right << endl;
		sectiondata.updateToMapVals(newHeight, sectiondata.getMaps().at(newHeight).left, sectiondata.getMaps().at(newHeight).right);
		
		syn->rewriteMapV2(framecounter, mile, centerheight, sectiondata.getMaps());


		//@author 2015年9月2号

        // 进度条
		if (this->current_startframeno != -1 && current_endframeno != -1)
			updateProgressBar(current_startframeno + fcindex);
		else
			updateProgressBar(keys.at(fcindex));

	}
	//qDebug() << "find fc" << framecounter << "," << mile << endl; 
    
    // 关闭文件
    try {
        // 关闭文件
        syn->closeFile();
        isfileopen = false;
        qDebug() << "close file:" << QString::fromLocal8Bit(filename.c_str());
        delete syn;
    }
    catch (LzException & ex)
    {
        qDebug() << "closeSynthesisDataFile exception:" << ex.what(); 
    }


    // 进度条
    endProgressBar();

	//重新显示
	loadSynthesisData();
}


void SynthesisTunnelWidget::setDirectionVal()
{
	if(ui->setDirectionValComboBox->currentText() == QObject::tr("左"))
	{
		ui->setDirectionValComboBox->setCurrentIndex(0);
	}
	else 
		ui->setDirectionValComboBox->setCurrentIndex(1);

}

/*
//@zengwang  2015年10月12日
void  SynthesisTunnelWidget::manualSelectData()
{
	// 左右侧有效
	int leftrightvalid = ui->leftRightValidComboBox->currentIndex();
	//bool isleft = true;


	// 隧道综合
    QString projectpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main);
    QByteArray tmpba = projectpath.toLocal8Bit();
    string projectpathstd(tmpba.constData());
    ProjectModel currentProjectModel = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Main);
    bool ret;
    TunnelDataModel * m = ClientSetting::getSettingInstance()->getCorrectTunnelDataModel(ret); 

    ret = loadCheckedTaskTunnelData();
    if (ret)
    {
        CheckedTunnelTaskModel tmp = CheckedTunnelTaskModel();
        LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main).getCheckedTunnelModel(tunnelid, tmp);
        
        // 打开文件名
        string tunnelheightssynname = filename;
        qDebug() << "currentcollectdate:" << currentcollectdate;

        __int64 tasktunnelid = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelID(tunnelid, currentcollectdate);
        __int64 taskid = TaskDAO::getTaskDAOInstance()->getTaskID(tunnelid, currentcollectdate);
    
        if (tasktunnelid >= 0)
        {
            int ret0 = TaskTunnelDAO::getTaskTunnelDAOInstance()->updateTaskTunnel(tasktunnelid, taskid, tunnelid, interframe_mile, true, tmp.planTask.isnormal, 0);

            ClearanceData & straightdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceStraightData();
            ClearanceData & leftdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceLeftData();
            ClearanceData & rightdata = ClientSetting::getSettingInstance()->getSingleTunnelModel().getClearanceRightData();

            LzSynthesis lzsyn;

            // 信号槽
            connect(&lzsyn, SIGNAL(initfc(long long, long long)), this, SLOT(initProgressBar(long long, long long)));
            connect(&lzsyn, SIGNAL(currentfc(long long)), this, SLOT(updateProgressBar(long long)));
            

            // @TODO 系数0.5103应改为外部配置文件输入
			lzsyn.initSynthesis(tunnelheightssynname, m, &tmp, interframe_mile, true, this->current_startframeno, this->current_endframeno, leftrightvalid);
            bool hasstraight = false, hasleft = false, hasright = false;
			lzsyn.synthesis(straightdata, leftdata, rightdata, hasstraight, hasleft, hasright);
			
			/*
			if(leftrightvalid == 1)       //左侧有效
			{
				straightdata.resetLeftOrRight(isleft);
				leftdata.resetLeftOrRight(isleft);
				rightdata.resetLeftOrRight(isleft);
			}
			else if(leftrightvalid == 2)     //右侧有效
			{
				straightdata.resetLeftOrRight(!isleft);
				leftdata.resetLeftOrRight(!isleft);
				rightdata.resetLeftOrRight(!isleft);
			}
			else
				;            //leftrightvalid = 0,全部有效

        }
        

        // 进度条
        endProgressBar();

        // 重新显示
        loadSynthesisData();
    }
    else
        ;

}
*/



// 得到鼠标坐标
void SynthesisTunnelWidget::getMousePos(int newx1, int newy1)
{
    ui->mouseX->setText(QString("%1").arg(newx1));
    ui->mouseY->setText(QString("%1").arg(newy1));
}