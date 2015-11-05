#include "correct_clearance.h"
#include "ui_correct_clearance.h"

#include <QFileDialog>
#include <QPainter>
#include <QMessageBox>
#include <QDebug>
#include <QVariant>
#include <QPointF>
#include <vector>
#include <string>
#include <QMenu>
#include <QKeyEvent>
#include <QTimer>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "setting_client.h"
#include "lz_project_access.h"

using namespace std;

/**
 * 图形修正（隧道综合之前）界面类实现
 *
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 20140923
 */
CorrectClearanceWidget::CorrectClearanceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CorrectClearanceWidget)
{
    ui->setupUi(this);

    ifShowFuse = true;

    // 文件未被打开标识
    isfileopen_syn = false;
    isplay = false;
    
    // 初始未在编辑
    iseditingsection = false;

    // 日志记录类
    editingrecord = NULL;

    // 初始没有在编辑
    iseditingsection = false;
    hasinitlogfile = false;
    
    // 默认编辑隧道相关信息
    currenttunnelid = -1;
    currenttunnelname = "";
    currenttunneldate = "";
    // 默认正在编辑帧号为-1
    framecounter = -1;
	current_startframeno = -1;
	current_endframeno = -1;

    // 默认车厢为正向 // TODO TOADDSTH
    currentCarriageDirection = true;

    // 默认从文件第一帧开始读取
    israndom = false;
    
    // 指定高度流式存储类赋初值NULL
    syn = NULL;
    lzMat_Fuse = NULL;
    lzMat_R = NULL;
    lzMat_T = NULL;

    isfileopen_R_mat = false, isfileopen_T_mat = false, isfileopen_fuse = false;
    isfileopen_syn = false;

    imagesection = NULL;
    scrollArea = NULL;

    // 画图，进行修正
    scrollArea = new QScrollArea(ui->graphicsView);
    imagesection = new SectionImage(ui->graphicsView);//生成对象第一次绘图
    // 调整scroll的滚动条大小范围
    scrollArea->setWidget(imagesection);
    ui->gridLayout_6->addWidget(scrollArea, 0, 0, 0, 0);
    
    // 预览界面
    edit = NULL;
    webview = NULL;
    // PDF EXCEL生成接口，在未点击时默认为NULL
    templatepath = ClientSetting::getSettingInstance()->getOutExcelTemplatePath();
    outputAccess = new LzOutputAccess();
    outputAccess->setTemplatePath(templatepath);
    
    // 实时获得画图界面的鼠标位置
    connect(imagesection, SIGNAL(sendMousePos(int, int)), this, SLOT(getMousePos(int, int)));

    // 得到修正位置坐标
    connect(imagesection, SIGNAL(sendCorrectPos(int, int)), this, SLOT(getCorrectPos(int, int)));

    // 获得画图界面修改断面数据的高度和限界半宽
    connect(imagesection, SIGNAL(sendUpdateSectionDataVal(int, int, bool, bool)), this, SLOT(updateSectionData(int, int, bool, bool)));
    
    ui->horizontalSlider->setRange(5, 150);
    ui->spinBox->setRange(5, 150);
    ui->spinBox->setVisible(false);
   
    connect(ui->spinBox, SIGNAL(valueChanged(int)), ui->horizontalSlider, SLOT(setValue(int)));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), ui->spinBox, SLOT(setValue(int)));
    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(getspinBoxvalue(int)));
    // 根据scale改变图形界面的大小
    connect(this,SIGNAL(sendspinBoxvalue(int)),imagesection,SLOT(UpdateScaleRelativeParameter(int)));
    // 当widget大小改变时，告知外层界面容器，更新scrollbar信号槽
    connect(imagesection, SIGNAL(resizeWidget()), this, SLOT(resizeGraphWidget()));

    imagesection->UpdateScaleRelativeParameter(80);


    // 播放按钮信号槽
    connect(ui->beginFrameLabel, SIGNAL(clicked()), this, SLOT(viewFirstFrame()));
    connect(ui->endFrameLabel, SIGNAL(clicked()), this, SLOT(viewEndFrame()));
    connect(ui->lastFrameLabel, SIGNAL(clicked()), this, SLOT(viewLastFrame()));
    connect(ui->nextFrameLabel, SIGNAL(clicked()), this, SLOT(viewNextFrame()));
    connect(ui->playLabel, SIGNAL(clicked()), this, SLOT(playAllframe()));
    connect(ui->stopLabel, SIGNAL(clicked()), this, SLOT(stopframe()));
    // 保存按钮
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveButton()));
    // 撤销按钮
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelOneStep()));
    
    // 单幅断面打印预览
    connect(ui->htmlPreviewButton, SIGNAL(clicked()), this, SLOT(htmlPreview()));
    connect(ui->excelPreviewButton, SIGNAL(clicked()), this, SLOT(excelPreview()));
    connect(ui->excelOutputAllSectionsButton, SIGNAL(clicked()), this, SLOT(excelExportAll()));
    // 初始不能点击
    ui->htmlPreviewButton->setEnabled(false);
    ui->excelPreviewButton->setEnabled(false);
    ui->excelOutputAllSectionsButton->setEnabled(false);

    // 批量输出界面
    batchoutputWidget = NULL;
    interframe_mile = 0.5103; // 默认

    // 关闭文件并，（上一步）切换界面信号槽定义
    connect(ui->finishButton, SIGNAL(clicked()), this, SLOT(finishButton()));
    // connect(ui->openFileButton, SIGNAL(clicked()), this, SLOT(openClearanceFile()));

    // 添加键盘快捷键，左键前一帧，右键后一帧，CTRL+回车播放（暂停）
    QAction* actionLeft = new QAction(this);
    actionLeft->setShortcut(QKeySequence(Qt::Key_Left));
    QAction* actionRight = new QAction(this);
    actionRight->setShortcut(QKeySequence(Qt::Key_Right));
    QAction* actionPlayStop = new QAction(this);
    actionPlayStop->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Enter));
    // 添加键盘快捷键，CTRL+上键放大，CTRL+向下暂停
    QAction* actionZoomIn = new QAction(this);
    actionZoomIn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up));
    QAction* actionZoomOut = new QAction(this);
    actionZoomOut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down));
    this->addAction(actionLeft);
    this->addAction(actionRight);
    this->addAction(actionPlayStop);
	this->addAction(actionZoomIn);
    this->addAction(actionZoomOut);
    // 添加键盘快捷键信号槽
    connect(actionLeft, SIGNAL(triggered()), this, SLOT(viewLastFrame()));
    connect(actionRight, SIGNAL(triggered()), this, SLOT(viewNextFrame()));
    connect(actionPlayStop, SIGNAL(triggered()), this, SLOT(keyPlayStop()));
    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(keyZoomIn()));
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(keyZoomOut()));

    timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(viewNextFrame()));

    // 播放速度
    connect(ui->playSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(changeTimer(int)));

    ui->widget_2->setVisible(false);

    // 显示原图
    connect(ui->showRawImageRadioButton, SIGNAL(toggled(bool)), this, SLOT(showImageWidget(bool)));
    
    // 默认不显示高度辅助线
    showhelpline(false);
    // 显示高度辅助线
    connect(ui->heighthelplineButton,SIGNAL(toggled(bool)),this,SLOT(showhelpline(bool)));
    list<int> Item = OutputHeightsList::getOutputHeightsListInstance()->list();
    imagesection->setInterestHeights(Item);

    // 显示融合结果
    connect(ui->showFuseButton,SIGNAL(toggled(bool)),this,SLOT(showFuse(bool)));
    ui->showFuseButton->setChecked(true);
    // 显示底板
    connect(ui->showFloorButton,SIGNAL(toggled(bool)),this,SLOT(showFloor(bool)));
    ui->showFloorButton->setChecked(true);

    connect(ui->showSynthesisButton, SIGNAL(toggled(bool)), this, SLOT(showSynthesis(bool)));
    ui->showSynthesisButton->setChecked(true);
    showSynthesis(true);
    
    // 设置最小高度有效
    isCreateShowMinHeight = false;
    connect(ui->minHeightValidCheckBox, SIGNAL(toggled(bool)), this, SLOT(showMinHeightValid(bool)));
    ui->showSynthesisButton->setChecked(true);
    ifShowMinHeight = true;
    showMinHeightValid(true);

    // 更多原图
    finalimage = NULL;
    image3d = NULL;
    connect(ui->moreRawImageButton, SIGNAL(clicked()), this, SLOT(moreImage()));

    ui->cancelButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->htmlPreviewButton->setEnabled(true);
    ui->excelPreviewButton->setEnabled(true);
    ui->excelOutputAllSectionsButton->setEnabled(true);

    // 初始化界面
    ui->lineEdit_name->setText("");
    ui->lineEdit_startPoint->setText("0");
    ui->lineEdit_endPoint->setText("0");
    ui->framenumberlabel->setText("0");
    ui->milelabel->setText("0");

    // 查找帧号
    connect(ui->frameSearchButton, SIGNAL(clicked()), this, SLOT(frameSearch()));
    connect(ui->frameSearchInput, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(frameSearchEditToggled()));
    // 第一次点击frameSearchInput 设置正则表达式
    firsttime = true;
    ui->frameSearchButton->setEnabled(false);
}

CorrectClearanceWidget::~CorrectClearanceWidget()
{
    if(imagesection != NULL)
        delete imagesection;

    if(scrollArea != NULL)
        delete scrollArea;

    if (isfileopen_syn)
        closeSynthesisDataFile();
    // 日志记录
    if (editingrecord != NULL)
        delete editingrecord;

    // 删除PDF和EXCEL预览接口
    if (edit != NULL)
        delete edit;
    if (webview != NULL)
        delete webview;

    if (batchoutputWidget != NULL)
        delete batchoutputWidget;

    // 删除outputAccess
    if (outputAccess != NULL)
        delete outputAccess;

    delete ui;
}

// 得到鼠标坐标
void CorrectClearanceWidget::getMousePos(int newx1, int newy1)
{
    ui->mouseX->setText(QString("%1").arg(newx1));
    ui->mouseY->setText(QString("%1").arg(newy1));
}

// 把单隧道综合界面的矩形传递到修正界面
void CorrectClearanceWidget::getRectPoint(float topleftx, float toplefty, float bottomrightx, float bottomrighty, bool carriagedirection)
{
    // @author 范翔 20150902 不做区分
    imagesection->setInterestRectangle(topleftx, toplefty, bottomrightx, bottomrighty, this->originy);

    // 继续传递到画图界面
    //if (carriagedirection)
    //    imagesection->setInterestRectangle(topleftx, toplefty, bottomrightx, bottomrighty, this->originy);
    //else
    //    imagesection->setInterestRectangle((-1)*bottomrightx, toplefty, (-1)*topleftx, bottomrighty, this->originy);
}

void CorrectClearanceWidget::startViewFromFirst(bool newdirection)
{
    widgetactive = true;
    currentCarriageDirection = newdirection;
    // 默认从文件第一帧开始读取
    israndom = false;
    if (isfileopen_syn)
        return;
  	loadSynthesisDataFile();
	loadGaugeImage();
}

void CorrectClearanceWidget::startViewFromSelectedFrame(__int64 frame)
{
    widgetactive = false;

    // 从文件中间帧开始读取
    israndom = true;
    framecounter = frame;
    viewFrameAt(frame);
}

void CorrectClearanceWidget::loadSynthesisDataFile()
{
    bool ret = false;
    try {
        syn = new LzSerialStorageSynthesis();
        syn->setting(250, 1024*1024*50, true);

        // 临时存储当前帧的高度数据
        ret = data.initMaps(); // 初始化高度，不可缺少

        // 给ImageSection界面的数组分配内存空间
        imagesection->initPointsArray(data.getMaps().size());

        if (ret == false)
        {
            qDebug() << "can init clearance value map:";
            isfileopen_syn = false;
            ui->status->setText("can init clearance value map");
            return;
        }

        // 打开文件
        // 【注意！】设置缓存大小1M，默认1G太大
        syn->setting(100, 1024*1024, true);
        if (!syn->openFile(synfilename.c_str()))
        {
            qDebug() << "can not open file:" << QString::fromLocal8Bit(synfilename.c_str());
            isfileopen_syn = false;
            ui->status->append(tr("文件%1打开失败！").arg(QString::fromLocal8Bit(synfilename.c_str())));
            return;
        }
        isfileopen_syn = true;
        // 可以输出图表
        ui->htmlPreviewButton->setEnabled(true);
        ui->excelPreviewButton->setEnabled(true);
        ui->excelOutputAllSectionsButton->setEnabled(true);

        qDebug() << "open file:" << QString::fromLocal8Bit(synfilename.c_str());

        keys = syn->readKeys();
        qDebug() << "size:" << keys.size();
        //for (int i = 0; i < keys.size(); i++)
        //    qDebug() << keys.at(i);
        // 文件取出帧数
        if (!israndom)
		{
            framecounter = keys.at(0);
			if (current_startframeno != -1 && current_endframeno != -1)
				framecounter = current_startframeno;
		}

        // 在readMap中调用的blocktomap中已调用data.resetMaps();
        //framercounti=this->correct_frame;
        bool ret_height = false;
        ret_height = syn->retrieveMap(framecounter);//表示显示第2帧(0代表第一帧,1代表第二帧....)*/
        if (ret_height == false)
        {
            qDebug() << "retrive mile error:" << framecounter << "in file:" << QString::fromLocal8Bit(synfilename.c_str());
            return;
        }
        else
        {
            ret = syn->readMap(framecounter, mile, centerHeight, data);
            // 如果车厢反向，左右对调重新写会
            if (!currentCarriageDirection)
                data.swapLeftAndRight();
        }

        if (ifShowFuse)
        {
            QString projectname = ClientSetting::getSettingInstance()->getCurrentEditingProject();
            QStringList strlist = projectname.split("_");
            int tmpsize = strlist.length();
            QString collectdate = strlist.at(tmpsize - 1).left( strlist.at(tmpsize - 1).length());
            QString tunnelname = ClientSetting::getSettingInstance()->getCurrentEditingTunnel();
            qDebug() << "editing projectname :" << projectname << ", collectdate:" << collectdate << ", tunnelname" << tunnelname;
            QString projectdir = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main);

            QString filename = tunnelname + "_" + collectdate;
            QString Fuse_filename = projectdir + "/fuse_calcu/" + filename + ".fdat";
            QString R_filename = projectdir + "/mid_calcu/" + filename + "_R.mdat";
            QString T_filename = projectdir + "/mid_calcu/" + filename + "_T.mdat";
            qDebug() << "to open filename" << Fuse_filename; 

            lzMat_R = new LzSerialStorageMat();
            lzMat_R->setting(250, 1024*1024*50, true);
            lzMat_T = new LzSerialStorageMat();
            lzMat_T->setting(250, 1024*1024*50, true);
            lzMat_Fuse = new LzSerialStorageMat();
            lzMat_Fuse->setting(250, 1024*1024*50, true); 

            // 打开文件
            isfileopen_R_mat = false, isfileopen_T_mat = false, isfileopen_fuse = false;
            isfileopen_R_mat = lzMat_R->openFile(R_filename.toLocal8Bit().constData());
    	    isfileopen_T_mat = lzMat_T->openFile(T_filename.toLocal8Bit().constData());
            isfileopen_fuse = lzMat_Fuse->openFile(Fuse_filename.toLocal8Bit().constData());
            // 找到RT矩阵的起始位置
            if (isfileopen_R_mat)
           	    lzMat_R->retrieveBlock(framecounter);
            if (isfileopen_T_mat)
        	    lzMat_T->retrieveBlock(framecounter);
            if (isfileopen_fuse)
                lzMat_Fuse->retrieveBlock(framecounter);

            // TODO TOADD
            //if (!isfileopen_R_mat || !isfileopen_T_mat || !isfileopen_fuse)
                qDebug() << "retrive RT FuseData error:" << framecounter << "in file:" << QString::fromLocal8Bit(synfilename.c_str());
            //else
            {
                // 读出Mat
                if (isfileopen_R_mat)
      	            lzMat_R->readMat(R_point);
                if (isfileopen_T_mat)
    	            lzMat_T->readMat(T_point);
                if (isfileopen_fuse)
                    lzMat_Fuse->readMat(Fuse_pnts);
            }
        }

        // 打开修正日志操作记录文件
        editingrecord = new EditRecordStack();
        QByteArray ba = ClientSetting::getSettingInstance()->getCurrentUser().toLocal8Bit();
        string currentuser(ba.constData());
        ret_height = editingrecord->initLogger(logfilename, currentuser);
        if (!ret_height)
        {
            QMessageBox::warning(this, tr("提示"), tr("修正日志文件未加载%1").arg(logfilename.c_str()));
            ui->cancelButton->setEnabled(false);
            hasinitlogfile = false;
        }
        else
        {
            hasinitlogfile = true;
        }
    }
    catch (LzException & ex)
    {
        qDebug() << "loadSynthesisDataFile eexception:" << ex.what();
    }

    ui->framenumberlabel->setText(QString("%1").arg(framecounter));
    ui->milelabel->setText(QString("%1").arg((long long)mile));

    qDebug()<< "read map ret=" << ret << ", mile=" << mile << ", framecounter=" << framecounter;
}

void CorrectClearanceWidget::closeSynthesisDataFile()
{
    if (!isfileopen_syn)
        return;
    try {
        // 关闭文件
        syn->closeFile();
        isfileopen_syn = false;
        framecounter = 0;
		current_startframeno = -1;
		current_endframeno = -1;

        qDebug() << "close file:" << QString::fromLocal8Bit(synfilename.c_str());
        delete syn;
        syn = NULL;

        if (ifShowFuse)
        {
            if (isfileopen_R_mat)
            {
                lzMat_R->closeFile();
                delete lzMat_R; 
                lzMat_R = NULL;
            }
            if (isfileopen_T_mat)
            {
                lzMat_T->closeFile();
                delete lzMat_T; 
                lzMat_T = NULL;
            }
            if (isfileopen_fuse)
            {
                lzMat_Fuse->closeFile();
                delete lzMat_Fuse;
                lzMat_Fuse = NULL;
            }
        }

        // 关闭log文件
        delete editingrecord;
        editingrecord = NULL;
        ui->cancelButton->setEnabled(false);
        ui->saveButton->setEnabled(false);
        ui->htmlPreviewButton->setEnabled(true);
        ui->excelPreviewButton->setEnabled(true);
        ui->excelOutputAllSectionsButton->setEnabled(true);
    }
    catch (LzException & ex)
    {
        qDebug() << "closeSynthesisDataFile eexception:" << ex.what(); 
    }
}

void CorrectClearanceWidget::viewFrameAt(__int64 frame)
{
    qDebug() << "viewFrameAt: framecounter=" << frame;
    if (!isfileopen_syn)
    {
        loadSynthesisDataFile();
        // 加载图形并显示
        loadGaugeImage();
    }
    else
    {
        // 如果正在编辑，不能修改
        if (iseditingsection)
        {
            QMessageBox::StandardButton rb = QMessageBox::question(NULL, tr("提示"), tr("当前断面已经修改还未保存，是否放弃？"),QMessageBox::Yes|QMessageBox::No);
    
            if (rb == QMessageBox::No)
                return;
            iseditingsection = false;
        }

        bool ret = false, ret1 = false;
        framecounter = frame;
        try {
            ret = syn->retrieveMap(framecounter);
            ret1 = syn->readMap(framecounter, mile, centerHeight, data);

            // 判断中心高度是否有效，更新界面
            if (ret1)
            {
                isCreateShowMinHeight = true;
                if (centerHeight < 0)
                    ui->minHeightValidCheckBox->setChecked(false);
                else
                    ui->minHeightValidCheckBox->setChecked(true);
                isCreateShowMinHeight = false;
            }

            // 如果车厢反向，左右对调重新写会
            if (!currentCarriageDirection)
                data.swapLeftAndRight();

            if (ifShowFuse)
            {
                // 找到RT矩阵的起始位置
                if (isfileopen_R_mat)
           	        lzMat_R->retrieveBlock(framecounter);                                
                if (isfileopen_T_mat)
                    lzMat_T->retrieveBlock(framecounter);
                if (isfileopen_fuse)
                    lzMat_Fuse->retrieveBlock(framecounter);

                // TODO TOADD
                //if (isfileopen_R_mat && isfileopen_T_mat && isfileopen_fuse)
                {
                    // 读出Mat
                    if (isfileopen_R_mat)
      	                lzMat_R->readMat(R_point);
                    if (isfileopen_T_mat)
    	                lzMat_T->readMat(T_point);
                    if (isfileopen_fuse)
                    {
                        lzMat_Fuse->readMat(Fuse_pnts);

                        /*for(int i=0; i<Fuse_pnts.cols; i++)
	                    {
			                qDebug()<<"第"<<i<<"点坐标："<<endl;
			                qDebug()<<"高度： "<<*Fuse_pnts.ptr<double>(0,i)<<endl;
			                qDebug()<<"水平： "<<*Fuse_pnts.ptr<double>(1,i)<<endl;
			                qDebug()<<"Z坐标："<<*Fuse_pnts.ptr<double>(2,i)<<endl;
			                qDebug()<<endl;

		                }*/
                    }
                }
            }
        }
        catch (LzException & ex)
        { 
            qDebug() << "retriveMap & readMap eexception:" << ex.what();
        }
        qDebug() << "retriveMap:" << ret << ", readMap:" << ret1;
        // 加载图形并显示
        loadGaugeImage();
    }
}

void CorrectClearanceWidget::viewFirstFrame()
{
    if (isfileopen_syn == false)
    {
        qDebug() << "the file " << QString::fromLocal8Bit(synfilename.c_str()) << "is not open";
        return;
    }

	if (current_startframeno == -1 || current_endframeno == -1)
	{
		if (framecounter <= keys.at(0))
		{
			qDebug() << "it is the first frame, cannot move to last one";
			return;
		}
		viewFrameAt(keys.at(0));
	}
	else
	{
		if (framecounter <= current_startframeno)
		{
			qDebug() << "it is the first frame, cannot move to last one";
			return;
		}
		viewFrameAt(current_startframeno);
	}

	emit viewRawImageSignalAtBegin();
}

void CorrectClearanceWidget::viewEndFrame()
{
    if (isfileopen_syn == false)
    {
        qDebug() << "the file " << QString::fromLocal8Bit(synfilename.c_str()) << "is not open";
        return;
    }

	if (current_startframeno == -1 || current_endframeno == -1)
    {
		if (framecounter >= (keys.at(keys.size() - 1)))
		{
			qDebug() << "the frame cannot move to the next, it's already the last one";
			return;
		}
		viewFrameAt(keys.at(keys.size() - 1));
	}
	else 
	{
		if (framecounter >= current_endframeno)
		{
			qDebug() << "the frame cannot move to the next, it's already the last one";
			return;
		}
		viewFrameAt(current_endframeno);
	}

	emit viewRawImageSignalAtEnd();
}

void CorrectClearanceWidget::viewLastFrame()
{
    qDebug() << "ViewLastframe: framecounter=" << framecounter << "keys.size=" << keys.size();
    if (isfileopen_syn == false)
    {
        qDebug() << "the file " << QString::fromLocal8Bit(synfilename.c_str()) << "is not open";
        return;
    }

	if (current_startframeno == -1 || current_endframeno == -1)
	{
		if (framecounter <= keys.at(0))
		{
			qDebug() << "it is the first frame, cannot move to last one";
			return;
		}
	}
	else
	{
		if (framecounter <= current_startframeno)
		{
			qDebug() << "it is the first frame, cannot move to last one";
			return;
		}
	}

	viewFrameAt(framecounter - 1);
	emit viewRawImageSignalAtLast();
}

void CorrectClearanceWidget::viewNextFrame()
{
    qDebug() << "ViewNextframe  framecounter=" << framecounter << "keys.size=" << keys.size();
    if (isfileopen_syn == false)
    {
        qDebug() << "the file " << QString::fromLocal8Bit(synfilename.c_str()) << "is not open";
        return;
    }

	if (current_startframeno == -1 || current_endframeno == -1)
	{
		if (framecounter >= (keys.at(keys.size() - 1)))
		{
			qDebug() << "the frame cannot move to the next, it's already the last one";
			ui->stopLabel->click();
			return;
		}
	}
	else
	{
		if (framecounter >= current_endframeno)
		{
			qDebug() << "the frame cannot move to the next, it's already the last one";
			ui->stopLabel->click();
			return;
		}
	}

	viewFrameAt(framecounter + 1);
    emit viewRawImageSignalAtNext();
}

void CorrectClearanceWidget::playAllframe()
{
    if (isfileopen_syn == false)
    {
        qDebug() << "the file " << QString::fromLocal8Bit(synfilename.c_str()) << "is not open";
        return;
    }
    if (framecounter == (keys.size() - 1))
    {
        qDebug() << "the frame cannot move to the next, it's already the last one";
        return;
    }
    // 变换播放器按钮样式
    setPlayerButtonStyle(5);
    //ui->groupBox->setVisible(true);
	isplay = true;
    timer->start();
}

/**
 * 改变播放速度
 */
void CorrectClearanceWidget::changeTimer(int speedspan)
{
    timer->stop();
    delete timer;
    timer =  new QTimer();
    timer->setInterval(speedspan);
}

void CorrectClearanceWidget::stopframe()
{
    if (isfileopen_syn == false)
    {
        qDebug() << "the file " << QString::fromLocal8Bit(synfilename.c_str()) << "is not open";
        return;
    }
    timer->stop();
    // 变换播放器按钮样式
    setPlayerButtonStyle(6);
    isplay = false;
}

// 按快捷键槽函数（开始/暂停）
void CorrectClearanceWidget::keyPlayStop()
{
    qDebug() << "key play stop" << isplay;
	if (isplay)
        stopframe();
	else
        playAllframe();
}

// 看原图
void CorrectClearanceWidget::showImageWidget(bool clicked)
{
    //TODO
    //qDebug() << "showImageWidget" << clicked;
    if (clicked)
    {
        //ui->widget_2->setVisible(true);
        //if (finalimage == NULL)
            //moreImage();
        if (image3d == NULL)
            moreImage3D();
    }
    else
    {
        //ui->widget_2->setVisible(false);
        //if (finalimage != NULL)
        //{
        //    delete finalimage;
        //    finalimage = NULL;
        //}
        if (image3d != NULL)
        {
            delete image3d;
            image3d = NULL;
        }
    }
}

// 是否显示高度辅助线
void CorrectClearanceWidget::showhelpline(bool ishow)
{
    imagesection->setShowheightline(ishow);
    imagesection->update();
}

// 是否显示底板
void CorrectClearanceWidget::showFloor(bool ishow)
{
    imagesection->setFloorVisible(ishow);
    imagesection->update();
}

// 是否显示融合计算结果
void CorrectClearanceWidget::showFuse(bool ishow)
{
    ifShowFuse = ishow;
    viewFrameAt(framecounter); 
}

// 是否显示提取高度结果
void CorrectClearanceWidget::showSynthesis(bool isshow)
{
    ifShowSynthesis = isshow;
    imagesection->setShowSynHeightsResultData(isshow);
    imagesection->update();
}

// 显示最小高度
void CorrectClearanceWidget::showMinHeightValid(bool isshow)
{
    if (isCreateShowMinHeight)
        return;

    ifShowMinHeight = isshow;

    if (hasinitlogfile)
    {
        EditRecord rec;
        rec.height = -1;
        rec.isleft = true;
        rec.newval = -1;
        rec.oldval = data.getCenterHeight();
        editingrecord->addOneStep(rec);
    }
    data.setCenterHeight(-1);
    centerHeight = -1;

    // 标记正在编辑
    iseditingsection = true;
    ui->cancelButton->setEnabled(true);
    ui->saveButton->setEnabled(true);
    ui->htmlPreviewButton->setEnabled(false);
    ui->excelPreviewButton->setEnabled(false);
    ui->excelOutputAllSectionsButton->setEnabled(false);

    //imagesection->setShowMinHeight(isshow);

    loadGaugeImage();
}

// 更多原图
void CorrectClearanceWidget::moreImage()
{
    finalimage = new ClientImageViewer();

    // 界面player控制信号槽
    connect(this, SIGNAL(viewRawImageSignalAtBegin()), finalimage, SLOT(viewBeginFiveImages()));
    connect(this, SIGNAL(viewRawImageSignalAtEnd()), finalimage, SLOT(viewEndFiveImages()));
    connect(this, SIGNAL(viewRawImageSignalAtLast()), finalimage, SLOT(viewLastFiveFrameImages()));
    connect(this, SIGNAL(viewRawImageSignalAtNext()), finalimage, SLOT(viewNextFiveFrameImages()));

    QString projectpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main);
    QString tmpimg_projectpath = ClientSetting::getSettingInstance()->getClientTmpLocalParentPath();
    qDebug() << projectpath << tmpimg_projectpath;
    finalimage->setInfo(projectpath, tmpimg_projectpath, currenttunnelid, currenttunnelname);
    finalimage->setCurrentFCs(framecounter);

    finalimage->show();
}

void CorrectClearanceWidget::moreImage3D()
{
    image3d = new ImageViewer3DTwoViewWidget("", false, true);

    QString projectpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main);
	QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Main);
	
    QString tmpimg_projectpath = ClientSetting::getSettingInstance()->getClientTmpLocalParentPath();
	bool isTwoNasMode = image3d->getNasMode();

	qDebug() << projectpath << tmpimg_projectpath << projectname.left(projectname.size() - 5);
    image3d->setInfo(projectpath, projectname.left(projectname.size() - 5), tmpimg_projectpath, currenttunnelid, currenttunnelname);
    image3d->setCurrentFCs(framecounter);
    image3d->show();
}

void CorrectClearanceWidget::correctscale(int newsynscale)
{
    synscale=newsynscale;
    ui->spinBox->setValue(synscale);
    emit sendspinBoxvalue(synscale);
}

// 按快捷键槽函数（缩小）
void CorrectClearanceWidget::keyZoomOut()
{
    if (isfileopen_syn == false)
    {
        qDebug() << "the file " << QString::fromLocal8Bit(synfilename.c_str()) << "is not open";
        return;
    }
    int v = ui->spinBox->value();
	if (++v <= ui->spinBox->maximum())
    {
        ui->spinBox->setValue(v);
		ui->horizontalSlider->setValue(v);
    }
}

// 按快捷键槽函数（放大）
void CorrectClearanceWidget::keyZoomIn()
{
    if (isfileopen_syn == false)
    {
        qDebug() << "the file " << QString::fromLocal8Bit(synfilename.c_str()) << "is not open";
        return;
    }
    int v = ui->spinBox->value();
	if (--v >= ui->spinBox->minimum())
    {
        ui->spinBox->setValue(v);
		ui->horizontalSlider->setValue(v);
    }
}

void CorrectClearanceWidget::saveButton()
{
	//qDebug() << "isfileopen_syn" << isfileopen_syn << endl;
    if (isfileopen_syn == true)
    {
        try {
            // 重写某帧指定高度数据到文件中
            // 如果车厢反向，左右对调重新写会
            if (!currentCarriageDirection)
                data.swapLeftAndRight();
            // 将中心高度重写到文件中
            syn->rewriteMapV2(framecounter, mile, centerHeight, data.getMaps());
            if (hasinitlogfile)
                editingrecord->saveAll(framecounter, mile, currentCarriageDirection);
            iseditingsection = false;
        }
        catch (LzException & ex)
        {
            qDebug() << "rewrite map exception:" << ex.what();
        }
        changedata();
        ui->saveButton->setEnabled(false);
        ui->cancelButton->setEnabled(false);
        ui->htmlPreviewButton->setEnabled(true);
        ui->excelPreviewButton->setEnabled(true);
        ui->excelOutputAllSectionsButton->setEnabled(true);
    }
}

/**
 * 撤销一步操作槽函数
 */
void CorrectClearanceWidget::cancelOneStep()
{
    qDebug() << "isfileopen_syn" << isfileopen_syn << endl;
    if (isfileopen_syn == true)
    {
        if (hasinitlogfile)
        {
            bool ret = editingrecord->cancelOneStep(data);
            if (ret)
                loadGaugeImage();
            if (editingrecord->isEmpty())
            {
                qDebug() << "editingrecord stack is empty now!";
                ui->cancelButton->setEnabled(false);
                ui->saveButton->setEnabled(false);
                ui->htmlPreviewButton->setEnabled(true);
                ui->excelPreviewButton->setEnabled(true);
                ui->excelOutputAllSectionsButton->setEnabled(true);
                iseditingsection = false;
            }
        }
    }
}

// 关闭文件并，（上一步）切换界面信号槽定义
void CorrectClearanceWidget::finishButton()
{
    // 如果正在编辑，不能修改
    if (iseditingsection)
    {
        QMessageBox::StandardButton rb = QMessageBox::question(NULL, tr("提示"), tr("当前断面已经修改还未保存，是否放弃？"),QMessageBox::Yes|QMessageBox::No);
    
        if (rb == QMessageBox::No)
        {
            return;
        }
        iseditingsection = false;
    }

    closeSynthesisDataFile();
    emit finish();
}

void CorrectClearanceWidget::changedata()
{
	bool ret = false, ret1 = false;
    try {
        ret = syn->retrieveMap(this->framecounter);
        ret1 = syn->readMap(framecounter, mile, centerHeight, data);
        // 如果车厢反向，左右对调重新写会
        if (!currentCarriageDirection)
            data.swapLeftAndRight();
    }
	catch (LzException & ex)
    { 
        qDebug() << "retriveMap & readMap eexception:" << ex.what();
    }
}

void CorrectClearanceWidget::getspinBoxvalue(int value)
{
    int scalevalue=value;
    //qDebug()<<"scaletest1:"<<scalevalue;
    emit  sendtosyn(scalevalue);
    // emit sendspinBoxvalue(scalevalue);
}

void CorrectClearanceWidget::getparameterfromtest(int newscale, double newpixeltomm, int newwidthpixel, int neworiginy)
{
    this->scale=newscale;
    this->pixeltomm=newpixeltomm;
    this->widthpixel=newwidthpixel;
    this->originy=neworiginy;
    //qDebug()<<"xx"<<this->scale<<this->pixeltomm<< this->widthpixel<<this->originy;
}

// 得到修正位置坐标
void CorrectClearanceWidget::getCorrectPos(int x, int y)
{
    // 调整scroll的滚动条大小范围
    scrollArea->setWidget(imagesection);
    ui->pointY->setText(QString("%1").arg(y));
    ui->pointX->setText(QString("%1").arg(x));
}

void CorrectClearanceWidget::loadGaugeImage()
{
    if (!isfileopen_syn)
        imagesection->setPointsArrayVisible(false);
    else
        imagesection->setPointsArrayVisible(true);

    ui->framenumberlabel->setText(QString("%1").arg(framecounter));
    ui->milelabel->setText(QString("%1").arg((long long)mile));

    // 在ImageSection中给2个PointsArray数组赋值
    bool ret = imagesection->SectionDataToPointsArray(data);

    if (ifShowFuse)
    {
        if (isfileopen_fuse)
        {
            imagesection->MatDataToAccuratePointsArray(Fuse_pnts, true, false, currentCarriageDirection);

            // TODO TODELETE
            //////////
            /*QString framecounterstr = QString("%1").arg(framecounter);
            QString outputfilenametemp = "";//QObject::tr("%1_里程%2_帧号%3.txt").arg(synfilename).arg(mile).arg(framecounterstr);
            QByteArray tmpba = outputfilenametemp.toLocal8Bit();
            std::string outputfilenametmp = string("C:\\Users\\Administrator\\Desktop\\Output.txt");// + string(tmpba.constData());
            std::ofstream SaveFile(outputfilenametmp.c_str());
            for(int i = 0; i< Fuse_pnts.cols; i++)
            {
                SaveFile<<"第"<<i<<"点坐标："<<endl;
                SaveFile<<"高度： "<<*Fuse_pnts.ptr<float>(0,i)<<endl;
                SaveFile<<"水平： "<<*Fuse_pnts.ptr<float>(1,i)<<endl;
                SaveFile<<"Z坐标："<<*Fuse_pnts.ptr<float>(2,i)<<endl;
                SaveFile<<endl;

            }
            SaveFile.close();*/
            //////////

            imagesection->setFusePointArrayVisible(true);
        }
        else
            imagesection->setFusePointArrayVisible(false);
    }
    else
        imagesection->setFusePointArrayVisible(false);

    if (ret)
        imagesection->repaint();//即更新绘图
}

// 转到帧的相关槽函数
void CorrectClearanceWidget::frameSearch()
{
    QString tmp = ui->frameSearchInput->text();
    __int64 fc = tmp.toLongLong();
    viewFrameAt(fc);
    firsttime = true;
    ui->frameSearchInput->setText(tr("请输入帧号"));
    ui->frameSearchButton->setEnabled(false);
}

void CorrectClearanceWidget::frameSearchEditToggled()
{
    if (firsttime)
    {
        ui->frameSearchInput->setText("");
        // 输入帧号
        // 非正整数（负整数 + 0） <br>
        QRegExp regExp("[0-9]{0,10}");
        //绑入lineEdit :
        ui->frameSearchInput->setValidator(new QRegExpValidator(regExp, this));
        firsttime = false;
    }
    ui->frameSearchButton->setEnabled(true);
}

/**
 * 获得画图界面修改断面数据的高度和限界半宽
 * @param isupdateval 是否更新到Sectiondata中，若否，在界面更新显示坐标，若是，同时更新到SectionData中
 */
void CorrectClearanceWidget::updateSectionData(int newval, int newheight, bool isleft, bool isupdateval)
{
    ui->pointY->setText(QString("%1").arg(newheight));
    ui->pointX->setText(QString("%1").arg(newval));
    if (isupdateval)
    {
        if (newval < 0)
            newval = 0 - newval;
        float oldval;
        // 此处与车厢正向反向无关，则左侧为左侧，else车厢反向，左侧为右侧
        if (isleft)
        {
            oldval = data.getMaps().at(newheight).left;
            data.updateToMapVals(newheight, newval, true);
        }
        else
        {
            oldval = data.getMaps().at(newheight).right;
            data.updateToMapVals(newheight, newval, false);
        }
        
        if (hasinitlogfile)
        {
            EditRecord rec;
            rec.height = newheight;
            rec.isleft = isleft;
            rec.newval = newval;
            rec.oldval = oldval;
            editingrecord->addOneStep(rec);
        }
        // 标记正在编辑
        iseditingsection = true;
        ui->cancelButton->setEnabled(true);
        ui->saveButton->setEnabled(true);
        ui->htmlPreviewButton->setEnabled(false);
        ui->excelPreviewButton->setEnabled(false);
        ui->excelOutputAllSectionsButton->setEnabled(false);
    }
}

// 更新GraphsView的AcrollBar槽【弃用】
void CorrectClearanceWidget::resizeGraphWidget()
{
    // 调整scroll的滚动条大小范围

	//qDebug() << "resizeGraphWidget";
    //scrollArea->setWidget(testsix);
    //ui->tab_2->layout()->addWidget(scrollArea);
}

// 下一步界面切换槽函数【放弃】
/*void CorrectClearanceWidget::on_nextWidgetButton_clicked()
{
    QString filename2 = QString::fromLocal8Bit(filename.c_str());
    emit signalSelectedTunnelToSynthesis(filename2);
}*/

void CorrectClearanceWidget::slotSetCarriageDirection(bool newdirect)
{
    this->currentCarriageDirection = newdirect;
}

// 从上一步接收文件名参数界面切换槽函数
void CorrectClearanceWidget::slotSelectedTunnelToEdit(int tunnelid, QString signalfilename, bool isDouble, bool carriagedir, bool isNormal, double distanceMode, long long startframeno, long long endframeno)
{
    if (isfileopen_syn)
    {
        qDebug() << "There is other file open, close it first please.!";
        return;
    }
    QByteArray ba = signalfilename.toLocal8Bit();
    synfilename = string(ba.constData());
    QFileInfo info(signalfilename);
    QString logname = QStringList(info.fileName().split(".")).at(0);
    
    currenttunnelid = tunnelid;
    currenttunnelname = QStringList(logname.split("_")).at(0);
    currenttunneldate = QStringList(logname.split("_")).at(1);
    currentCarriageDirection = carriagedir;

	isNormalTravel = isNormal;

	isDoubleLine = isDouble;

    interframe_mile = distanceMode;

    mile = 0;
    framecounter = 0;
	this->current_startframeno = startframeno;
	this->current_endframeno = endframeno;

    QString logfilename1 = info.path() + "/" + logname + "_correct.log";
    ba = logfilename1.toLocal8Bit();
    logfilename = string(ba.constData());
    qDebug() << "CorrectClearanceWidget slotSelectedTunnelToEdit Receive signal QString" << signalfilename << ",logfilename" << logfilename1;

    bool ret;
    TunnelDataModel *tunnelDataModel = ClientSetting::getSettingInstance()->getCorrectTunnelDataModel(ret);
    if (ret)
    {
        ui->lineEdit_name->setText(QString(tr("%1")).arg(QString::fromLocal8Bit(tunnelDataModel->getName().c_str())));
        ui->lineEdit_startPoint->setText(QString("%1").arg(tunnelDataModel->getStartPoint()));
        ui->lineEdit_endPoint->setText(QString("%1").arg(tunnelDataModel->getEndPoint()));
    }
    else
    {
        ui->lineEdit_name->setText("");
        ui->lineEdit_startPoint->setText("0");
        ui->lineEdit_endPoint->setText("0");
    }
    ui->framenumberlabel->setText(QString("%1").arg(framecounter));
    ui->milelabel->setText(QString("%1").arg((long long)mile));

    ui->htmlPreviewButton->setEnabled(false);
    ui->excelPreviewButton->setEnabled(false);
    ui->excelOutputAllSectionsButton->setEnabled(false);
    // 初始未在编辑
    iseditingsection = false;
}

/**
 * 转变播放器Button样式
 * @param i 播放按钮的ID，依次从左到右为1-6
 */
void CorrectClearanceWidget::setPlayerButtonStyle(int i)
{
    /*
    ui->lastFrameLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_back.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_back.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_back.png)}");
    ui->nextFrameLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_forward.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_forward.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_forward.png)}");
    ui->playLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_play.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_play.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_play.png)}");
    ui->stopLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_stop.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_stop.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_stop.png)}");*/
    switch(i)
    {
        /* 下一帧上一帧不需要
		case 1: ui->lastFrameLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_back_down.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_back_down.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_back_down.png)}");
                break;
        case 2: ui->nextFrameLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_forward_down.png)4 4 4 4 stretch stretch;}"
                         "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_forward_down.png)}"
                         "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_forward_down.png)}");
                break;*/
        case 5: ui->beginFrameLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_begin_down.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_begin_down.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_begin_down.png)}");
                ui->endFrameLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_end_down.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_end_down.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_end_down.png)}"); 
                ui->playLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_play_down.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_play_down.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_play_down.png)}");
                ui->stopLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_stop.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_stop.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_stop.png)}");
                ui->lastFrameLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_back_down.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_back_down.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_back_down.png)}");
                ui->nextFrameLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_forward_down.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_forward_down.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_forward_down.png)}");
                ui->lastFrameLabel->setEnabled(false);
                ui->nextFrameLabel->setEnabled(false);
                ui->beginFrameLabel->setEnabled(false);
                ui->endFrameLabel->setEnabled(false);
                break;
        
		case 6: ui->beginFrameLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_begin.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_begin.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_begin.png)}");
                ui->endFrameLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_end.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_end.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_end.png)}"); 
                ui->stopLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_stop_down.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_stop_down.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_stop_down.png)}");
                ui->playLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_play.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_play.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_play.png)}");
                ui->lastFrameLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_back.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_back.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_back.png)}");
                ui->nextFrameLabel->setStyleSheet("QPushButton{border-width:4px;border-image:url(:player/player_forward.png)4 4 4 4 stretch stretch;}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_forward.png)}"
                             "QPushButton:hover{border: 0.1px solid;border-image:url(:player/player_forward.png)}");
                ui->lastFrameLabel->setEnabled(true);
                ui->nextFrameLabel->setEnabled(true);
                ui->beginFrameLabel->setEnabled(true);
                ui->endFrameLabel->setEnabled(true);
                break;
        default:break;
    }
}

/*********单幅断面预览相关*************/
/**
 * makedir，在用textEdit生成html时不选择生成目录，因此可能目录不存在
 */
void CorrectClearanceWidget::makedir(QString filename)
{
    QDir mydir;
    if (!mydir.exists(QFileInfo(filename).path()))
        mydir.mkpath(QFileInfo(filename).path());
}

/**
 * 初始化PDF预览接口
 */
void CorrectClearanceWidget::initTextEdit()
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
void CorrectClearanceWidget::initFancyBrowser()
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
 * 单幅断面打印预览槽函数
 */
void CorrectClearanceWidget::htmlPreview()
{
    // 如果正在编辑，不能修改
    if (iseditingsection)
    {
        QMessageBox::StandardButton rb = QMessageBox::question(NULL, tr("提示"), tr("当前断面已经修改还未保存，是否放弃？"),QMessageBox::Yes|QMessageBox::No);
    
        if (rb == QMessageBox::No)
        {
            return;
        }
        iseditingsection = false;
    }

    QString outputfilename;
    int ret = -1;
    
    outputAccess->initOutput(LzOutputAccess::OutputInHTML);

    // 获得输出文件名
    outputfilename = ClientSetting::getSettingInstance()->getParentPath() + "/output/" + currenttunnelname + "_" + currenttunneldate + QObject::tr("_里程%1.html").arg(mile);
    makedir(outputfilename);

    // 保存图片
    QString tmpinsertimgfile = ClientSetting::getSettingInstance()->getParentPath() + "/output/" + currenttunnelname + "_" + currenttunneldate + QObject::tr("_里程%1.png").arg(mile);
    imagesection->saveImage(tmpinsertimgfile);

    bool ret1;
    TunnelDataModel * tmptunneldatamodel = ClientSetting::getSettingInstance()->getCorrectTunnelDataModel(ret1);
    ret = outputAccess->outputSingleSection(tmptunneldatamodel, currenttunneldate, data, outputfilename, tmpinsertimgfile);
        
    statusShow(outputfilename, ret, false);
 
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
	webview->setUrl(QUrl("file:///" + outputfilename));
	webview->show();
}

/**
 * 限界输出预览槽函数，EXCEL方式
 */
void CorrectClearanceWidget::excelPreview()
{
    QString outputfilename;
    int ret = -1;
    
    outputAccess->initOutput(LzOutputAccess::OutputInExcel);

    // 获得输出文件名
    QString openFileDir = ClientSetting::getSettingInstance()->getParentPath() + "/output/" + currenttunnelname + "_" + currenttunneldate + QObject::tr("_里程%1.xls").arg(mile);
    outputfilename = QFileDialog::getSaveFileName(this, tr("导出到文件"),openFileDir, tr("EXCEL (*.xls)"));
    
    // 保存图片
    QString tmpinsertimgfile = ClientSetting::getSettingInstance()->getParentPath() + "/output/" + currenttunnelname + "_" + currenttunneldate + QObject::tr("_里程%1.png").arg(mile);
    imagesection->saveImage(tmpinsertimgfile);
    
    bool ret1;
    TunnelDataModel * tmptunneldatamodel = ClientSetting::getSettingInstance()->getCorrectTunnelDataModel(ret1);
    ret = outputAccess->outputSingleSection(tmptunneldatamodel, currenttunneldate, data, outputfilename, tmpinsertimgfile);
        
    statusShow(outputfilename, ret, false);
}

/**
 * excel批量导出
 */
void CorrectClearanceWidget::excelExportAll()
{
    if (batchoutputWidget != NULL)
        delete batchoutputWidget;

    batchoutputWidget = new CorrectClearanceBatchOutputWidget(this);

    connect(batchoutputWidget, SIGNAL(exportAll(int)), this, SLOT(slotExportAll(int)));

    batchoutputWidget->setMinUnit(this->interframe_mile);

    batchoutputWidget->show();

}

/**
 * 批量输出Excel槽函数
 */
void CorrectClearanceWidget::slotExportAll(int param)
{
    long long tmpframecount = framecounter;

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
                    framecounter = tmpframecount;
                    viewFrameAt(framecounter);
                    return;
                }
                openFileDir = folders[0];
                qDebug() << "directory" << openFileDir;
                QString outputfilename = openFileDir;

                // 设置输出方式为Excel
                outputAccess->initOutput(LzOutputAccess::OutputInExcel);

                if (batchoutputWidget != NULL)
                    batchoutputWidget->initProgressBar(current_startframeno, current_endframeno);

                // 遍历隧道准备单隧道数据输出
                QString tmptunnelname;
                QString tmpdate;
                QString tmplinename;
                QString insertimgfilepath = ClientSetting::getSettingInstance()->getParentPath() + "/output/";
                QString tmpinsertimgfile;
                // 批量输出
                for (framecounter = current_startframeno; framecounter != current_endframeno; framecounter++)
                {
                    // 改mile值
                    viewFrameAt(framecounter);

                    outputfilename = openFileDir + "/" + currenttunnelname + "_" + currenttunneldate + QObject::tr("_里程%1.xls").arg(mile);

                    // 保存图片
                    tmpinsertimgfile = ClientSetting::getSettingInstance()->getParentPath() + "/output/" + currenttunnelname + "_" + currenttunneldate + QObject::tr("_里程%1.png").arg(mile);
                    imagesection->saveImage(tmpinsertimgfile);

                    bool ret1;
                    ret = outputAccess->outputSingleSection(ClientSetting::getSettingInstance()->getCorrectTunnelDataModel(ret1), currenttunneldate, data, outputfilename, tmpinsertimgfile);

                    if (batchoutputWidget != NULL)
                        batchoutputWidget->setProgressBar(framecounter);

                    statusShow(outputfilename, ret, true);
                }
            }
            else
            {
                ui->status->setText(tr("全部导出到Excel终止，目录未选择。"));
                delete fd;
                framecounter = tmpframecount;
                viewFrameAt(framecounter);
                return;
            }
            delete fd;
        }
    }

    if (batchoutputWidget != NULL)
        batchoutputWidget->endProgressBar();

    framecounter = tmpframecount;
    viewFrameAt(framecounter);
}

/**
 * statuslabel显示状态
 */
void CorrectClearanceWidget::statusShow(QString outfilename, int retval, bool isappend)
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
/*********单幅断面预览相关*************/
