#include "view_tvcalcumatfile.h"
#include "ui_view_tvcalcumatfile.h"

#include <QTextCodec>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QScrollArea>

#include "LzSerialStorageMat.h"
#include "lz_exception.h"
#include "output_heights_list.h"

using namespace std;

ViewTVCalcuMatDataWidget::ViewTVCalcuMatDataWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewTVCalcuMatDataWidget)
{
    ui->setupUi(this);
    
    QIcon icon;
    icon.addFile(QString::fromUtf8(":player/player_play.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);

    // 指定高度流式存储类赋初值NULL
    lzMat_Fuse = NULL;
    lzMat_RT = NULL;
    lzSyn = NULL;

    isfileopen_fuse = false;
    isfileopen_RT = false;
    isfileopen_syn = false;

    imagesection = NULL;
    scrollArea = NULL;

    // 画图，进行修正
    scrollArea = new QScrollArea(ui->graphicsView);
    imagesection = new SectionImage(ui->graphicsView);//生成对象第一次绘图
    // 调整scroll的滚动条大小范围
    scrollArea->setWidget(imagesection);
    ui->gridLayout_5->addWidget(scrollArea, 1, 0);
    imagesection->update();
    
    // 实时获得画图界面的鼠标位置
    connect(imagesection, SIGNAL(sendMousePos(int, int)), this, SLOT(getMousePos(int, int)));

    // 得到修正位置坐标
    //connect(imagesection, SIGNAL(sendCorrectPos(int, int)), this, SLOT(getCorrectPos(int, int)));

    ui->horizontalSlider->setRange(5, 150);
    ui->spinBox->setRange(5, 150);
   
    connect(ui->spinBox, SIGNAL(valueChanged(int)), ui->horizontalSlider, SLOT(setValue(int)));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), ui->spinBox, SLOT(setValue(int)));
    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(getspinBoxvalue(int)));
    // 根据scale改变图形界面的大小
    connect(this,SIGNAL(sendspinBoxvalue(int)),imagesection,SLOT(UpdateScaleRelativeParameter(int)));
    // 当widget大小改变时，告知外层界面容器，更新scrollbar信号槽
    connect(imagesection, SIGNAL(resizeWidget()), this, SLOT(resizeGraphWidget()));
    // 播放按钮信号槽
    connect(ui->beginFrameLabel_2, SIGNAL(clicked()), this, SLOT(viewFirstFrame()));
    connect(ui->endFrameLabel_2, SIGNAL(clicked()), this, SLOT(viewEndFrame()));
    connect(ui->lastFrameLabel_2, SIGNAL(clicked()), this, SLOT(viewLastFrame()));
    connect(ui->nextFrameLabel_2, SIGNAL(clicked()), this, SLOT(viewNextFrame()));

    // 添加键盘快捷键，左键前一帧，右键后一帧，CTRL+回车播放（暂停）
    QAction* actionLeft = new QAction(this);
    actionLeft->setShortcut(QKeySequence(Qt::Key_Left));
    QAction* actionRight = new QAction(this);
    actionRight->setShortcut(QKeySequence(Qt::Key_Right));
    // 添加键盘快捷键，CTRL+上键放大，CTRL+向下暂停
    QAction* actionZoomIn = new QAction(this);
    actionZoomIn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up));
    QAction* actionZoomOut = new QAction(this);
    actionZoomOut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down));
    this->addAction(actionLeft);
    this->addAction(actionRight);
	this->addAction(actionZoomIn);
    this->addAction(actionZoomOut);
    // 添加键盘快捷键信号槽
    connect(actionLeft, SIGNAL(triggered()), this, SLOT(viewLastFrame()));
    connect(actionRight, SIGNAL(triggered()), this, SLOT(viewNextFrame()));
    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(keyZoomIn()));
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(keyZoomOut()));

    // 如果是图形修正界面看原图，隐藏播放
    ui->widget->setVisible(true);

    // 查找帧号
    connect(ui->frameSearchButton_2, SIGNAL(clicked()), this, SLOT(frameSearch()));
    connect(ui->frameSearchInput_2, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(frameSearchEditToggled()));
    // 第一次点击frameSearchInput 设置正则表达式
    firsttime = true;
    ui->frameSearchButton_2->setEnabled(false);
    
    tmpopendir = "";
    file_full = "";
    rtfile_full = "";

    connect(ui->openFileButton, SIGNAL(clicked()), this, SLOT(openFile()));

    ui->openFileButton->setEnabled(true);

    // 默认车厢正向 // TODO TOADDSTH
    currentCarriageDirection = true;

    // 默认文件起始终止帧
    startfr = 0;
    endfr = 0;
    current_fc = 0;
    
    // Mat中存储数据格式,画图相关
    isFuse = false;
    isQR = false;

    // 默认不显示高度辅助线
    ifshowInterestHeights = false;
    showHeights(ifshowInterestHeights);
    connect(ui->interestHeightsButton, SIGNAL(toggled(bool)), this, SLOT(showHeights(bool)));

    list<int> Item = OutputHeightsList::getOutputHeightsListInstance()->list();
    imagesection->setInterestHeights(Item);

    connect(ui->synResultButton, SIGNAL(toggled(bool)), this, SLOT(showHeightsSynResult(bool)));
    ui->synResultButton->setVisible(false);
    ui->RmatrixText->setVisible(false);
    ui->TmatrixText->setVisible(false);

    // 载入对比数据
    loadWidget = NULL;
    ifcompareToComareData = false;
    ui->loadCompareDataButton->setVisible(false);
    connect(ui->loadCompareDataButton, SIGNAL(clicked()), this, SLOT(loadCompareDataWidget()));
}

ViewTVCalcuMatDataWidget::~ViewTVCalcuMatDataWidget()
{
    delete ui;
    closeSynthesisDataFile();

    if (loadWidget != NULL)
        delete loadWidget;
}

void ViewTVCalcuMatDataWidget::setParentPath(QString newpath)
{
    tmpopendir = newpath;
}

/*******************************/
void ViewTVCalcuMatDataWidget::appendMsg(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
}

void ViewTVCalcuMatDataWidget::appendError(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
}

void ViewTVCalcuMatDataWidget::openFile()
{
    file_full = QFileDialog::getOpenFileName(this, QObject::tr("打开流式文件"), tmpopendir, "Document files (*.mdat *.fdat)");
    tmpopendir = QFileInfo(file_full).absoluteFilePath();
    ui->currentFile->setText(file_full);
    
    QDir my_dir = QDir(QFileInfo(file_full).filePath());
    QString filename = QFileInfo(file_full).fileName();
    filename = filename.left(filename.size() - 5);
    qDebug() << my_dir.absolutePath() << filename;

    // Mat中存储数据格式,画图相关
    if (file_full.endsWith(".fdat"))
    {
        isFuse = true;

        QStringList strList = tmpopendir.split("/", QString::SkipEmptyParts);
        int length = strList.size();
        bool ret = false;
        if (length > 2);
        {
            ret = true;
            strList.removeAt(length - 1);
            strList.removeAt(length - 2);
        }

        QString rootdir = "";
        if (tmpopendir.startsWith("//"))
            rootdir = "//" + strList.join("/");
        else
            rootdir = strList.join("/");
        qDebug() << rootdir;

        if (ret)
        {
            rtfile_full = rootdir + "/mid_calcu/" + filename + "_RT.mdat";
            synfile_full = rootdir + "/syn_data/" + filename + ".syn";
            showHeightsSynResult(true);
            qDebug() << rtfile_full << synfile_full;
        }
        else
        {
            rtfile_full = "";
            synfile_full = "";
            showHeightsSynResult(false);
        }
        ui->synResultButton->setVisible(true);
        ui->RmatrixText->setVisible(true);
        ui->TmatrixText->setVisible(true);

    }
    else
    {
        isFuse = false;
        if (file_full.endsWith("Q.mdat") || file_full.endsWith("R.mdat"))
            isQR = true;
        else
            isQR = false;
        showHeightsSynResult(false);
        ui->synResultButton->setVisible(false);
        ui->RmatrixText->setVisible(false);
        ui->TmatrixText->setVisible(false);

        // 对比数据
        ui->loadCompareDataButton->setVisible(false);
    }

    QStringList strList = my_dir.absolutePath().split("/", QString::SkipEmptyParts);
    int length = strList.size();
    bool ret = false;
    if (length > 2);
    {
        ret = true;
        strList.removeAt(length - 1);
        strList.removeAt(length - 2);
    }

    QString rootdir = "";
    if (my_dir.absolutePath().startsWith("//"))
        rootdir = "//" + strList.join("/");
    else
        rootdir = strList.join("/");
    qDebug() << rootdir;
   
    ui->linename->setText("");
    ui->tunnelname->setText("");
    ui->isdouble->setText("");
    ui->isnormal->setText("");
    ui->direction->setText("");

    ui->pulsenum->setText("");
    ui->seqno->setText("");
    ui->datetime->setText("");
   
    ui->totalFrame->setText("");
    ui->startFrame->setText("");
    ui->endFrame->setText("");

    // 更新界面
    this->repaint();
    
    // 读取基本信息
    // 查找是否有该线路
    QByteArray tmpba = file_full.toLocal8Bit();
    std::string openfilename = tmpba.constData();
    std::string openfilenamert = rtfile_full.toLocal8Bit().constData();
    std::string openfilenamesyn = synfile_full.toLocal8Bit().constData();
    lzMat_Fuse = new LzSerialStorageMat();
    lzMat_Fuse->setting(250, 1024*1024*50, true);
    lzMat_RT = new LzSerialStorageMat();
    lzMat_RT->setting(250, 1024*1024*50, true);
    lzSyn = new LzSerialStorageSynthesis();
    try {
            
        isfileopen_fuse = lzMat_Fuse->openFile(openfilename.c_str());
        bool ret = isfileopen_fuse;
        if (!ret)
        {
             delete lzMat_Fuse;
             appendMsg(tr("无法打开文件%1！").arg(file_full));
             return;
        }

        if (isFuse == true) // 融合结果
        {
            isfileopen_RT = lzMat_RT->openFile(openfilenamert.c_str());
            if (!isfileopen_RT)
            {
                delete lzMat_RT;
                appendMsg(tr("无法打开RT文件%1！").arg(rtfile_full));
            }

            isfileopen_syn = lzSyn->openFile(openfilenamesyn.c_str());
            if (!isfileopen_syn)
            {
                delete lzSyn;
                appendMsg(tr("无法打开Syn文件%1！").arg(synfile_full));
            }
            else
            {
                // 对比数据
                ui->loadCompareDataButton->setVisible(true);
        
                bool ret2 = sectiondata.initMaps();
                if (ret2)
                    imagesection->initPointsArray(sectiondata.getMaps().size());
            }
        }

        lzMat_Fuse->readHead((char *)&datahead);
        qDebug() << "data head frame num :" << datahead.frame_num;

        keys = lzMat_Fuse->readKeys();
        qDebug() << "file frames vector size:" << keys.size();
        if (keys.size() <= 0)
        {
             delete lzMat_Fuse;
             appendMsg(tr("文件内无有效帧！"));
             return;
        }
        startfr = keys.at(0);
        endfr = keys.at(keys.size() - 1);
        current_fc = keys.at(0);

        ui->linename->setText(QString::fromLocal8Bit(datahead.line_name));
        ui->tunnelname->setText(QString::fromLocal8Bit(datahead.tunnel_name));
        bool isdouble1 = datahead.is_double_line;
        if (isdouble1)
            ui->isdouble->setText(tr("双线"));
        else
            ui->isdouble->setText(tr("单线"));
        
        bool isnormal = datahead.is_normal;
        if (isnormal && isdouble1)
            ui->isnormal->setText(tr("正常拍摄"));
        else if (!isnormal && isdouble1)
            ui->isnormal->setText(tr("非正常拍摄"));
        else if (!isnormal && !isdouble1)
            ui->isnormal->setText(tr("逆序"));
        else
            ui->isnormal->setText(tr("正序"));
        
        bool direct = datahead.carriage_direction;
        if (direct)
            ui->direction->setText(tr("正向"));
        else
            ui->direction->setText(tr("反向"));

        ui->pulsenum->setText(QString("%1").arg(datahead.interval_pulse_num));
        ui->seqno->setText(QString("%1").arg(datahead.seqno));
        ui->datetime->setText(QString::fromLocal8Bit(datahead.datetime));
        
        ui->totalFrame->setText(QString("%1").arg(endfr - startfr + 1));
        ui->startFrame->setText(QString("%1").arg(startfr));
        ui->endFrame->setText(QString("%1").arg(endfr));
        
        qDebug() << "file frames vector startfc: " << startfr << ", endfc: " << endfr;

    }
    catch (LzException & ex)
    {
        appendError(QObject::tr("从流式文件%1生成临时小图片失败").arg(file_full));
    }
    // 如果是图形修正界面看原图，隐藏播放
    viewFrameAt(current_fc);
}

void ViewTVCalcuMatDataWidget::closeSynthesisDataFile()
{
    try {
        if (isfileopen_fuse)
        {
            lzMat_Fuse->closeFile();
            delete lzMat_Fuse;
            lzMat_Fuse = NULL;
        }

        if (isfileopen_RT)
        {
            lzMat_RT->closeFile();
            delete lzMat_RT;
            lzMat_RT = NULL;
        }

        if (isfileopen_syn)
        {
            lzSyn->closeFile();
            delete lzSyn;
            lzSyn = NULL;
        }
    }
    catch (LzException & ex)
    {
        qDebug() << "closeSynthesisDataFile eexception:" << ex.what(); 
    }
}

// 转到帧的相关槽函数
void ViewTVCalcuMatDataWidget::frameSearch()
{
    QString tmp = ui->frameSearchInput_2->text();
    __int64 fc = tmp.toLongLong();
    if (fc >= startfr && fc <= endfr)
    {
        bool ret = viewFrameAt(fc);
        if (!ret)
            QMessageBox::warning(this, tr("提示"), tr("当前找不到%1这个帧号").arg(fc));
    }
    else
        QMessageBox::warning(this, tr("提示"), tr("当前找不到%1这个帧号").arg(fc));

    firsttime = true;
    ui->frameSearchInput_2->setText(tr("请输入帧号"));
    ui->frameSearchButton_2->setEnabled(false);
}

void ViewTVCalcuMatDataWidget::frameSearchEditToggled()
{
    if (firsttime)
    {
        ui->frameSearchInput_2->setText("");
        // 输入帧号
        // 非正整数（负整数 + 0） <br>
        QRegExp regExp("[0-9]{0,10}");
        //绑入lineEdit :
        ui->frameSearchInput_2->setValidator(new QRegExpValidator(regExp, this));
        firsttime = false;
    }
    ui->frameSearchButton_2->setEnabled(true);
}

// 按快捷键槽函数（缩小）
void ViewTVCalcuMatDataWidget::keyZoomOut()
{
    if (isfileopen_fuse == false)
    {
        qDebug() << "the file " << file_full << "is not open";
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
void ViewTVCalcuMatDataWidget::keyZoomIn()
{
    if (isfileopen_fuse == false)
    {
        qDebug() << "the file " << file_full << "is not open";
        return;
    }
    int v = ui->spinBox->value();
	if (--v >= ui->spinBox->minimum())
    {
        ui->spinBox->setValue(v);
		ui->horizontalSlider->setValue(v);
    }
}

/**
 * 得到对比限界值
 */
void ViewTVCalcuMatDataWidget::getCompareSectionData()
{
    if (isfileopen_fuse)
    {
        SectionData & data = loadWidget->getData1();

        if (isfileopen_syn)
        {
            imagesection->initPointsArray(data.getMaps().size(), true);
            imagesection->SectionDataToPointsArray(data, true);
        }
    }
}

bool ViewTVCalcuMatDataWidget::viewFrameAt(__int64 frame)
{
    qDebug() << "viewFrameAt: framecounter=" << frame;
    bool ret = false;
    current_fc = frame;
    try
    {
        Fuse_pnts.empty();

        // 找到RT矩阵的起始位置
        if (isfileopen_fuse)
            ret = lzMat_Fuse->retrieveBlock(frame);

        if (isFuse == true)
        {
            if (isfileopen_RT)
                lzMat_RT->retrieveBlock(frame);

            if (isfileopen_syn)
                lzSyn->retrieveMap(frame);
        }

        if (ret)
        {
            // TODO TOADD
            //if (isfileopen_R_mat && isfileopen_T_mat && isfileopen_fuse)
            {
                if (isfileopen_fuse)
                {
                    // @author范翔 TODELETE
                    //ret = false; 
                    ret = lzMat_Fuse->readMat(Fuse_pnts);
                    
                    if (isFuse == true)
                    {
                        Rstr = "";
                        if (isfileopen_RT)
                        {
                            bool ret2 = lzMat_RT->readMat(tmprt);
                            if (ret2)
                            {
                                for(int j=0; j<3 ;j++)
                                    Rstr += QString("%1").arg(*tmprt.ptr<double>(0,j)) + "\t";
                            }
                        }
                        ui->RmatrixText->clear();
                        ui->RmatrixText->appendPlainText(Rstr);

                        if (isfileopen_syn)
                        {
                            long long tmpll; double tmpf; float centerheight;
                            bool ret3 = lzSyn->readMap(tmpll, tmpf, centerheight, sectiondata);
                            // 如果车厢反向，左右对调重新写绘
                            if (!currentCarriageDirection)
                                sectiondata.swapLeftAndRight();

                            // 数据对比
                            if (ifcompareToComareData && loadWidget != NULL)
                                loadWidget->setData2(sectiondata);
                        }
                    }

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
        appendError(QObject::tr("第%1帧retriveMap & readMap eexception").arg(frame));
    }
    qDebug() << "retriveMap:" << ret;
    if (!ret)
        appendError(QObject::tr("第%1帧retriveMap失败").arg(frame));

    // 加载图形并显示
    loadGaugeImage();
    return ret;
}

void ViewTVCalcuMatDataWidget::viewFirstFrame()
{
    if (isfileopen_fuse == false)
    {
        qDebug() << "the file " << file_full << "is not open";
        return;
    }
    if (current_fc <= keys.at(0))
    {
        qDebug() << "it is the first frame, cannot move to last one";
        return;
    }
    viewFrameAt(keys.at(0));
}

void ViewTVCalcuMatDataWidget::viewEndFrame()
{
    if (isfileopen_fuse == false)
    {
        qDebug() << "the file " << file_full << "is not open";
        return;
    }
    if (current_fc >= (keys.at(keys.size() - 1)))
    {
        qDebug() << "the frame cannot move to the next, it's already the last one";
        return;
    }
    viewFrameAt(keys.at(keys.size() - 1));
}

void ViewTVCalcuMatDataWidget::viewLastFrame()
{
    qDebug() << "ViewLastframe: framecounter=" << current_fc << "keys.size=" << keys.size();
    if (isfileopen_fuse == false)
    {
        qDebug() << "the file " << file_full << "is not open";
        return;
    }
    if (current_fc <= keys.at(0))
    {
        qDebug() << "it is the first frame, cannot move to last one";
        return;
    }
    else
    {
        viewFrameAt(current_fc - 1);
    }
}

void ViewTVCalcuMatDataWidget::viewNextFrame()
{
    qDebug() << "ViewNextframe  framecounter=" << current_fc << "keys.size=" << keys.size();
    if (isfileopen_fuse == false)
    {
        qDebug() << "the file " << file_full << "is not open";
        return;
    }
    if (current_fc >= (keys.at(keys.size() - 1)))
    {
        qDebug() << "the frame cannot move to the next, it's already the last one";
        return;
    }
    else
    {
        viewFrameAt(current_fc + 1);
    }
}

void ViewTVCalcuMatDataWidget::loadGaugeImage()
{
    ui->framenumberlabel->setText(QString("%1").arg(current_fc));
    
    bool ret = false;
    if (isfileopen_fuse)
    {
        //@author 范翔 TODELETE
        //ret = false;
        ret = imagesection->MatDataToAccuratePointsArray(Fuse_pnts, isFuse, isQR, currentCarriageDirection);

        // TODO TODELETE
        //////////
        /*QString framecounterstr = QString("%1").arg(framecounter);
        QString outputfilenametemp = "";//QObject::tr("%1_里程%2_帧号%3.txt").arg(synfilename).arg(mile).arg(framecounterstr);
        QByteArray tmpba = outputfilenametemp.toLocal8Bit();
        std::string outputfilenametmp = string("C:\\Users\\Administrator\\Desktop\\Output.txt");// + string(tmpba.constData());
        std::ofstream SaveFile(outputfilenametmp.c_str());*/
        /*for(int i = 0; i< Fuse_pnts.cols; i++)
        {
            qDebug()<<"第"<<i<<"点坐标：";
            qDebug()<<"高度： "<<*Fuse_pnts.ptr<float>(0,i);
            qDebug()<<"水平： "<<*Fuse_pnts.ptr<float>(1,i);
            qDebug()<<"Z坐标："<<*Fuse_pnts.ptr<float>(2,i);
            qDebug()<<endl;
        }*/
        //////////

        imagesection->setFusePointArrayVisible(true);

        if (isfileopen_syn)
            imagesection->SectionDataToPointsArray(sectiondata);
    }
    else
        imagesection->setFusePointArrayVisible(false);

    //@author 范翔 TODELETE
    if (ret)
        imagesection->repaint();//即更新绘图
}

void ViewTVCalcuMatDataWidget::getspinBoxvalue(int value)
{
    //qDebug()<<"scaletest1:"<<scalevalue;
    //emit  sendtosyn(scalevalue);
    emit sendspinBoxvalue(value);
}

void ViewTVCalcuMatDataWidget::getMousePos(int newx, int newy)
{
    ui->mouseX->setText(QString("%1").arg(newx));
    ui->mouseY->setText(QString("%1").arg(newy));
}

// 显示高度辅助线
void ViewTVCalcuMatDataWidget::showHeights(bool newif)
{
    ifshowInterestHeights = newif;
    imagesection->setShowheightline(ifshowInterestHeights);
    imagesection->update();
}

// 显示提高度综合结果
void ViewTVCalcuMatDataWidget::showHeightsSynResult(bool newif)
{
    ifshowHeightsSynResult = newif;
    imagesection->setShowSynHeightsResultData(newif);
    imagesection->update();
}

// 显示对比数据界面
void ViewTVCalcuMatDataWidget::loadCompareDataWidget()
{
    if (loadWidget == NULL)
    {
        loadWidget = new LoadCompareDataWidget(this);
        connect(loadWidget, SIGNAL(updateData1()), this, SLOT(getCompareSectionData()));
    }

    loadWidget->show();

    ifcompareToComareData = true;
    loadWidget->setParentPath(tmpopendir);
}