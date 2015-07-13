#include "imageviewer.h"
#include "ui_imageviewer.h"

#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QPixmap>
#include <QDebug>

#include "file_decompress.h"
#include "lz_project_access.h"

// 图像增强处理
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "MatToQImage.h"
#include "LzCalculator.h"

const int TN_WIDTH = 64;
const int TN_HEIGHT = 160;//图像显示的尺寸

/**
 * 采集文件信息校对界面——原图浏览界面类定义
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 2014-08-19
 */
ImageViewer::ImageViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageViewer)
{
    ui->setupUi(this);

    // 初始化界面显示图片的label大小
    ui->cam1img1->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam1img2->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam1img3->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam1img4->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam1img5->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam2img1->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam2img2->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam2img3->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam2img4->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam2img5->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam3img1->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam3img2->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam3img3->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam3img4->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam3img5->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam4img1->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam4img2->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam4img3->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam4img4->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam4img5->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam5img1->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam5img2->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam5img3->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam5img4->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam5img5->setFixedSize(TN_WIDTH, TN_HEIGHT);

    connect(ui->exit_Button,SIGNAL(clicked()),this,SLOT(close()));
    // 申请数据 TODO TODELETE
    connect(ui->applyimage_Button,SIGNAL(clicked()),this,SLOT(viewImage()));

    // 播放按钮槽函数
    connect(ui->beginFrameLabel,SIGNAL(clicked()),this,SLOT(viewBeginFiveImages()));
    connect(ui->endFrameLabel,SIGNAL(clicked()),this,SLOT(viewEndFiveImages()));
    connect(ui->lastFrameLabel,SIGNAL(clicked()),this,SLOT(viewLastFiveFrameImages()));
    connect(ui->nextFrameLabel,SIGNAL(clicked()),this,SLOT(viewNextFiveFrameImages()));

    // 添加键盘快捷键，左键前一帧，右键后一帧，CTRL+回车播放（暂停）
    QAction* actionLeft = new QAction(this);
    actionLeft->setShortcut(QKeySequence(Qt::Key_Left));
    QAction* actionRight = new QAction(this);
    actionRight->setShortcut(QKeySequence(Qt::Key_Right));
    
    this->addAction(actionLeft);
    this->addAction(actionRight);
    // 添加键盘快捷键信号槽
    connect(actionLeft, SIGNAL(triggered()), this, SLOT(viewLastFiveFrameImages()));
    connect(actionRight, SIGNAL(triggered()), this, SLOT(viewNextFiveFrameImages()));

    // 刷新按钮
    connect(ui->cam1, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCam1andImages(int)));
    connect(ui->cam2, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCam2andImages(int)));
    connect(ui->cam3, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCam3andImages(int)));
    connect(ui->cam4, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCam4andImages(int)));
    connect(ui->cam5, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCam5andImages(int)));

    currentseqno = -1;
    currenttunnelid = -1;
    hasinitmodel = false;

    current_fc_master = 0;
    current_fc_1 = 0;
    current_fc_2 = 0;
    current_fc_3 = 0;
    current_fc_4 = 0;
    current_fc_5 = 0;

    // 默认向后申请
    viewdirection = false;

    // 初始五个相机位置
    ui->cam1->setCurrentIndex(1);  // A2
    ui->cam2->setCurrentIndex(9);  // E2
    ui->cam3->setCurrentIndex(15); // H2
    ui->cam4->setCurrentIndex(23); // L2
    ui->cam5->setCurrentIndex(31); // P2

    // 进出洞检查模式相机位置
    connect(ui->findInOutTunnelModeCheckBox, SIGNAL(toggled(bool)), this, SLOT(findInOutTunnelMode(bool)));

    // 刷新按钮
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(updateCameraandImages()));
    
    // 是否做直方图均衡化图像增强处理
    ifimgenhance = 0;
    connect(ui->imgEnhance_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(imgEnhance(int)));

    // 解压文件对象状态栏
    connect(File_decompress::getFileDecompressDAOInstance(), SIGNAL(finish(QString)), this, SLOT(receivedepressfinish(QString)));
    connect(File_decompress::getFileDecompressDAOInstance(), SIGNAL(signalParsedMsg(QString)), this, SLOT(updateStatus(QString)));

    // 如果是图形修正界面看原图，隐藏播放
    ui->player_groupBox->setVisible(false);
    ui->askRawImagesWidget->setVisible(false);

    // 查找帧号
    connect(ui->frameSearchButton, SIGNAL(clicked()), this, SLOT(frameSearch()));
    connect(ui->frameSearchInput, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(frameSearchEditToggled()));
    // 第一次点击frameSearchInput 设置正则表达式
    firsttime = true;
    ui->frameSearchButton->setEnabled(false);
}

ImageViewer::~ImageViewer()
{
    delete ui;
}

/**
 * 图形修正时候原图浏览设置信息函数，无seqnostart版本
 */
void ImageViewer::setInfo(QString projectpath, QString tmpimg_projectpath, int tunnelid, QString tunnelname)
{
	this->currentprojpath = projectpath;
    this->currentprojectpath_tmpimg = tmpimg_projectpath;
    this->currenttunnelid = tunnelid;
    datetime = projectpath.right(8);
    this->currenttunnelname = tunnelname;

    bool ret = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main).getCheckedTunnelModel(tunnelid, currentCheckedModel);
    if (!ret)
    {
        currentseqno = -1;
        currenttunnelid = -1;
        hasinitmodel = false;

        current_fc_master = 0;
        current_fc_1 = 0;
        current_fc_2 = 0;
        current_fc_3 = 0;
        current_fc_4 = 0;
        current_fc_5 = 0;

        // 默认向后申请
        viewdirection = false;

        qDebug() << QObject::tr("无法获取当前CheckedTunnelModel，其tunnelid为%1").arg(tunnelid);
        hasinitmodel = false; 
    }
    else
    {
        std::list<CheckedItem>::iterator it = currentCheckedModel.begin();
        if (it != currentCheckedModel.end())
        {
            // 还没有找到起始时找起始帧所在文件
            currentseqno = (*it).seqno;
            current_fc_master = (*it).start_frame_master;
            currenttmpimgpath = projectpath + "/tmp_img/" + QString("%1_%2_%3").arg(currentseqno).arg(QObject::tr((*it).tunnel_name.c_str())).arg(datetime);
            setCamerasCurrentFCs(current_fc_master);
        }

        hasinitmodel = true;

        // 更新显示原图
        updateImages();
    }
}

/**
 * 实际计划校正时的原图浏览设置信息函数，有seqnostart版本
 */
void ImageViewer::setInfo(QString projectpath, QString tmpimg_project_path, int tunnelid, int startseqno, QString tunnelname)
{
    this->currentprojpath = projectpath;
    this->currentprojectpath_tmpimg = tmpimg_project_path;
    this->currenttunnelid = tunnelid;
    datetime = projectpath.right(8);
    this->currenttunnelname = tunnelname;

    bool ret = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main).getCheckedTunnelModel(tunnelid, startseqno, currentCheckedModel);
    if (!ret)
    {
        currentseqno = -1;
        currenttunnelid = -1;
        hasinitmodel = false;

        current_fc_master = 0;
        current_fc_1 = 0;
        current_fc_2 = 0;
        current_fc_3 = 0;
        current_fc_4 = 0;
        current_fc_5 = 0;

        // 默认向后申请
        viewdirection = false;

        qDebug() << QObject::tr("无法获取当前CheckedTunnelModel，其tunnelid为%1").arg(tunnelid);
        hasinitmodel = false; 
    }
    else
    {
        std::list<CheckedItem>::iterator it = currentCheckedModel.begin();
        if (it != currentCheckedModel.end())
        {
            // 还没有找到起始时找起始帧所在文件
            currentseqno = (*it).seqno;
            current_fc_master = (*it).start_frame_master;
            currenttmpimgpath = projectpath + "/tmp_img/" + QString("%1_%2_%3").arg(currentseqno).arg(QObject::tr((*it).tunnel_name.c_str())).arg(datetime);
            setCamerasCurrentFCs(current_fc_master);
        }

        hasinitmodel = true;

        // 更新显示原图
        updateImages();
    }
}

/**
 * 设置当前currentfc，外部调用
 */
bool ImageViewer::setCurrentFCs(__int64 master_fc)
{
    // 已经到最前只能向后申请
    viewdirection = false;

    bool ret = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main).getCheckedTunnelModel(currenttunnelid, master_fc, currentCheckedModel);
    if (!ret)
        return false;

    bool findstart = false;
    int tmpseqno;
    string tmptunnelname; // 实际采集时当成的隧道名，非实际隧道名
    __int64 tmpmasterstartfc;
    __int64 tmpmasterendfc;
    std::list<CheckedItem>::iterator it = currentCheckedModel.begin();
    while (it != currentCheckedModel.end())
    {
        // 还没有找到起始时找起始帧所在文件
        if (!findstart && master_fc >= (*it).start_frame_master)
        {
            tmpseqno = (*it).seqno;
            tmptunnelname = (*it).tunnel_name;
            tmpmasterstartfc = (*it).start_frame_master;
            tmpmasterendfc = (*it).end_frame_master;
            findstart = true;
            break;
        }
        it++;
    }

    if (!findstart)
        return false;

    // 还没有找到起始时找起始帧所在文件
    currentseqno = tmpseqno;
    current_fc_master = master_fc;

    setCamerasCurrentFCs(master_fc);
    currenttmpimgpath = this->currentprojpath + "/tmp_img/" + QString("%1_%2_%3").arg(currentseqno).arg(QObject::tr((*it).tunnel_name.c_str())).arg(datetime);
    
    updateImages();
    return true;
}

/**
 * 设置主控fc对应的五组相机fc
 */
bool ImageViewer::setCamerasCurrentFCs(__int64 master_fc)
{
    std::list<CheckedItem>::iterator it = currentCheckedModel.begin();
    while (it != currentCheckedModel.end())
    {
        // 还没有找到起始时找起始帧所在文件
        if (master_fc >= (*it).start_frame_master && master_fc <= (*it).end_frame_master)
        {
            currentseqno = (*it).seqno;
            QString tmpcamid;
            __int64 tmpstartfc;
            __int64 tmpendfc;
            __int64 tmpmasterstartfc = (*it).start_frame_master;
            bool tmpret = false;
            for (int i = 0; i < 5; i++) // 五个相机
            {
                switch (i)
                {
                    case 0:  tmpcamid = ui->cam1->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
                    case 1:  tmpcamid = ui->cam2->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
                    case 2:  tmpcamid = ui->cam3->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
                    case 3:  tmpcamid = ui->cam4->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
                    case 4:  tmpcamid = ui->cam5->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
                    default: break;
                }

                if (tmpcamid.trimmed().compare("") == 0)
                    continue;

                // 换算每个相机的startfc = startnum - tmpmasterstartfc + tmpstartfc
                __int64 & tmpcurrentfc = getTmpCurrentfc(i);

                if (tmpret)
                    tmpcurrentfc = master_fc - tmpmasterstartfc + tmpstartfc;
                else
                    updateStatus(QString("相机%1的帧号找不到对应主控帧号为%2的从控文件帧号").arg(tmpcamid).arg(master_fc));
            }
            return true;
        }
        it++;
    }
    return false;
}

__int64 & ImageViewer::getTmpCurrentfc(int j)
{
    switch (j)
    {
        case 0: return current_fc_1;
        case 1: return current_fc_2;
        case 2: return current_fc_3;
        case 3: return current_fc_4;
        case 4: return current_fc_5;
        default:return current_fc_1;
    }   
}

bool  & ImageViewer::getTmphasfc(int j)
{
    switch (j)
    {
        case 0: return tmphasfc1;
        case 1: return tmphasfc2;
        case 2: return tmphasfc3;
        case 3: return tmphasfc4;
        case 4: return tmphasfc5;
        default:return tmphasfc1;
    }   
}

/**
 * 五组原图显示函数
 */
bool ImageViewer::updateImages()
{
    if (!hasinitmodel)
        return false;

    QString tmpcamid;
    bool ret = true;
    QString tmpcamimgdir;

    QLabel * tmpcamimg1 = NULL;
    QLabel * tmpcamimg2 = NULL;
    QLabel * tmpcamimg3 = NULL;
    QLabel * tmpcamimg4 = NULL;
    QLabel * tmpcamimg5 = NULL;
    QLabel * tmpcamimg = NULL;
    tmphasfc1 = false;
    tmphasfc2 = false;
    tmphasfc3 = false;
    tmphasfc4 = false;
    tmphasfc5 = false;

    for (int i = 0; i < 5; i++)
    {
        switch (i)
        {
            case 0: tmpcamid = ui->cam1->currentText(); 
                tmpcamimg1 = ui->cam1img1;
                tmpcamimg2 = ui->cam1img2;
                tmpcamimg3 = ui->cam1img3;
                tmpcamimg4 = ui->cam1img4;
                tmpcamimg5 = ui->cam1img5;
                break;
            case 1: tmpcamid = ui->cam2->currentText(); 
                tmpcamimg1 = ui->cam2img1;
                tmpcamimg2 = ui->cam2img2;
                tmpcamimg3 = ui->cam2img3;
                tmpcamimg4 = ui->cam2img4;
                tmpcamimg5 = ui->cam2img5;
                break;
            case 2: tmpcamid = ui->cam3->currentText(); 
                tmpcamimg1 = ui->cam3img1;
                tmpcamimg2 = ui->cam3img2;
                tmpcamimg3 = ui->cam3img3;
                tmpcamimg4 = ui->cam3img4;
                tmpcamimg5 = ui->cam3img5;
                break;
            case 3: tmpcamid = ui->cam4->currentText(); 
                tmpcamimg1 = ui->cam4img1;
                tmpcamimg2 = ui->cam4img2;
                tmpcamimg3 = ui->cam4img3;
                tmpcamimg4 = ui->cam4img4;
                tmpcamimg5 = ui->cam4img5;
                break;
            case 4: tmpcamid = ui->cam5->currentText(); 
                tmpcamimg1 = ui->cam5img1;
                tmpcamimg2 = ui->cam5img2;
                tmpcamimg3 = ui->cam5img3;
                tmpcamimg4 = ui->cam5img4;
                tmpcamimg5 = ui->cam5img5;
                break;
            default:break;
        }

        __int64 & tmpcurrentfc = getTmpCurrentfc(i);

        if (tmpcamid.trimmed().compare("") == 0)
        {
            tmpcamimg1->clear();
            tmpcamimg2->clear();
            tmpcamimg3->clear();
            tmpcamimg4->clear();
            tmpcamimg5->clear();
            continue;
        }
        tmpcamimgdir = currenttmpimgpath + "_" + tmpcamid + "/";

        // 每个相机的5张图
        for (int j = -2; j < 3; j++)
        {
            switch (j)
            {
                case -2: tmpcamimg = tmpcamimg1; break;
                case -1: tmpcamimg = tmpcamimg2; break;
                case 0: tmpcamimg = tmpcamimg3; break;
                case 1: tmpcamimg = tmpcamimg4; break;
                case 2: tmpcamimg = tmpcamimg5; break;
                default:break;
            }

            QString filename3 = (QString("%1/%2.jpg").arg(tmpcamimgdir).arg(tmpcurrentfc + j));
            if (!QFile(filename3).exists())
            {
                // 清空显示
                tmpcamimg->clear();
                qDebug() << QObject::tr("图片目录：") << filename3 << QObject::tr("不存在！");

                ret = false;
                continue;
            }
            qDebug() << QObject::tr("图片目录：") << filename3;

            bool & tmphasfc = getTmphasfc(j + 2);
            tmphasfc = true;

            // 做图像增强处理
            QByteArray tmpba = filename3.toLocal8Bit();
            cv::Mat img = cv::imread(tmpba.constData(), CV_LOAD_IMAGE_GRAYSCALE);
            float sclae = 0.3; // 缩放比例
            cv::Size size = cv::Size(img.size().width * sclae, img.size().height * sclae);
            cv::resize(img, img, size);
            //cv::imshow("win1",img);
            //cv::waitKey(0);
            if (ifimgenhance == 2)
            {
                
                double alpha = 2;  
                double beta = 50; 

                // 调整对比度，RGB调整，速度慢
                /*
                result = Mat::zeros(img.size(),img.type()); 
                for (int i = 0;i<img.rows;++i)  
                    for(int j= 0;j<img.cols;++j)  
                        for (int k = 0; k < 3; k++)
                            result.at(i,j)[k] = cv::saturate_cast<uchar>(img.at(i,j)[k]*alpha+beta);  
                cv::cvtColor(result,result, CV_BGRA2GRAY);
                */

                // 调整对比度，灰度调整，速度快
                cv::Mat result = cv::Mat(size, CV_8U);
                for( int i=0;i<img.rows;i++)  
                {  
                    uchar* dataimg = img.ptr<uchar>(i);
                    uchar* dataresult = result.ptr<uchar>(i);  
                    for(int j=0;j<img.cols;j++)  
                    {  
                        dataresult[j] = cv::saturate_cast<uchar>(dataimg[j]*alpha+beta);   
                    }  
                } 
                bigpm = MatToQImage(result);
            
                // 直方图均衡化---效果较差
                //cv::equalizeHist(img, result);
                //cv::imwrite("D:\\2.png", result);
                //cv::imshow("win1",result);
                //cv::waitKey(0);
            }
            else if (ifimgenhance == 1)
            {
                // 直方图规定化处理---By Ding
                cv::Mat result;
                LzCalculator::argument(img, result); 
                //cv::GaussianBlur(result, result, Size(5,5), 1.5, 1.5);
                bigpm = MatToQImage(result);
            }
            else
            {
                bigpm = MatToQImage(img);
            }
            
            smallpm = bigpm.scaled(TN_WIDTH, TN_HEIGHT, Qt::KeepAspectRatio);
            tmpcamimg->setPixmap(QPixmap::fromImage(smallpm));
            tmpcamimg->show();
        }
    }

    qDebug() << "has fc?: " << tmphasfc1 << tmphasfc2 << tmphasfc3 << tmphasfc4 << tmphasfc5;

    QLabel * tmpfclabel = NULL;
    QLabel * tmpmilelabel = NULL;

    // 每个相机的masterfc号
    for (int j = -2; j < 3; j++)
    {
        switch (j)
        {
            case -2: tmpfclabel = ui->fc1; tmpmilelabel = ui->mile1; break;
            case -1: tmpfclabel = ui->fc2; tmpmilelabel = ui->mile2; break;
            case 0: tmpfclabel = ui->fc3; tmpmilelabel = ui->mile3; break;
            case 1: tmpfclabel = ui->fc4; tmpmilelabel = ui->mile4; break;
            case 2: tmpfclabel = ui->fc5; tmpmilelabel = ui->mile5; break;
            default:break;
        }
        bool & tmphasfc = getTmphasfc(j + 2);
        if (tmphasfc)
        {
            tmpfclabel->setText(QString("%1").arg(current_fc_master + j));
            tmpmilelabel->setText(QString("%1").arg(current_fc_master + j));
        }
        else
        {
            tmpfclabel->setText(QString("%1无数据").arg(current_fc_master + j));
            tmpmilelabel->setText(QString("%1无数据").arg(current_fc_master + j));
        }
    }

    // 强制更新显示
    this->repaint();
    
    return ret;
}

// 五组相机选择combobox分别对应的槽函数
void ImageViewer::updateCam1andImages(int index) { tmpUpdateCameraImage_cam1index = index; updateCameraandImages(); }
void ImageViewer::updateCam2andImages(int index) { tmpUpdateCameraImage_cam2index = index; updateCameraandImages(); }
void ImageViewer::updateCam3andImages(int index) { tmpUpdateCameraImage_cam3index = index; updateCameraandImages(); }
void ImageViewer::updateCam4andImages(int index) { tmpUpdateCameraImage_cam4index = index; updateCameraandImages(); }
void ImageViewer::updateCam5andImages(int index) { tmpUpdateCameraImage_cam5index = index; updateCameraandImages(); }

void ImageViewer::updateCameraandImages()
{
    // 关闭所有灯
    changeCamera("", false);

    // 打开灯
    QString cameraindex = ui->cam1->itemText(tmpUpdateCameraImage_cam1index);
    //qDebug() << uicameraindex << cameraindex;
    changeCamera(cameraindex, true);
    cameraindex = ui->cam1->itemText(tmpUpdateCameraImage_cam2index);
    changeCamera(cameraindex, true);
    cameraindex = ui->cam1->itemText(tmpUpdateCameraImage_cam3index);
    changeCamera(cameraindex, true);
    cameraindex = ui->cam1->itemText(tmpUpdateCameraImage_cam4index);
    changeCamera(cameraindex, true);
    cameraindex = ui->cam1->itemText(tmpUpdateCameraImage_cam5index);
    changeCamera(cameraindex, true);

    setCamerasCurrentFCs(current_fc_master);
    updateImages();
}

// 改变相机提示灯
void ImageViewer::changeCamera(QString cameraindex, bool status)
{
    QString img;
    if (status)
        img = ":/image/lightgreen.png";
    else
    {
        img = ":/image/lightyellow.png";
        ui->A1Label_work->setPixmap(QPixmap(img));
        ui->A2Label_work->setPixmap(QPixmap(img));
        ui->B1Label_work->setPixmap(QPixmap(img));
        ui->B2Label_work->setPixmap(QPixmap(img));
        ui->C1Label_work->setPixmap(QPixmap(img));
        ui->C2Label_work->setPixmap(QPixmap(img));
        ui->D1Label_work->setPixmap(QPixmap(img));
        ui->D2Label_work->setPixmap(QPixmap(img));
        ui->E1Label_work->setPixmap(QPixmap(img));
        ui->E2Label_work->setPixmap(QPixmap(img));
        ui->F1Label_work->setPixmap(QPixmap(img));
        ui->F2Label_work->setPixmap(QPixmap(img));
        ui->G1Label_work->setPixmap(QPixmap(img));
        ui->G2Label_work->setPixmap(QPixmap(img));
        ui->H1Label_work->setPixmap(QPixmap(img));
        ui->H2Label_work->setPixmap(QPixmap(img));
        ui->I1Label_work->setPixmap(QPixmap(img));
        ui->I2Label_work->setPixmap(QPixmap(img));
        ui->J1Label_work->setPixmap(QPixmap(img));
        ui->J2Label_work->setPixmap(QPixmap(img));
        ui->K1Label_work->setPixmap(QPixmap(img));
        ui->K2Label_work->setPixmap(QPixmap(img));
        ui->L1Label_work->setPixmap(QPixmap(img));
        ui->L2Label_work->setPixmap(QPixmap(img));
        ui->M1Label_work->setPixmap(QPixmap(img));
        ui->M2Label_work->setPixmap(QPixmap(img));
        ui->N1Label_work->setPixmap(QPixmap(img));
        ui->N2Label_work->setPixmap(QPixmap(img));
        ui->O1Label_work->setPixmap(QPixmap(img));
        ui->O2Label_work->setPixmap(QPixmap(img));
        ui->P1Label_work->setPixmap(QPixmap(img));
        ui->P2Label_work->setPixmap(QPixmap(img));
        ui->Q1Label_work->setPixmap(QPixmap(img));
        ui->Q2Label_work->setPixmap(QPixmap(img));
        ui->R1Label_work->setPixmap(QPixmap(img));
        ui->R2Label_work->setPixmap(QPixmap(img));
        return;
    }
    //qDebug() << cameraindex;
    if (cameraindex.compare("A1") == 0)
        ui->A1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("A2") == 0)
        ui->A2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("B1") == 0)
        ui->B1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("B2") == 0)
        ui->B2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("C1") == 0)
        ui->C1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("C2") == 0)
        ui->C2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("D1") == 0)
        ui->D1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("D2") == 0)
        ui->D2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("E1") == 0)
        ui->E1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("E2") == 0)
        ui->E2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("F1") == 0)
        ui->F1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("F2") == 0)
        ui->F2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("G1") == 0)
        ui->G1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("G2") == 0)
        ui->G2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("H1") == 0)
        ui->H1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("H2") == 0)
        ui->H2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("I1") == 0)
        ui->I1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("I2") == 0)
        ui->I2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("J1") == 0)
        ui->J1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("J2") == 0)
        ui->J2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("K1") == 0)
        ui->K1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("K2") == 0)
        ui->K2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("L1") == 0)
        ui->L1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("L2") == 0)
        ui->L2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("M1") == 0)
        ui->M1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("M2") == 0)
        ui->M2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("N1") == 0)
        ui->N1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("N2") == 0)
        ui->N2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("O1") == 0)
        ui->O1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("O2") == 0)
        ui->O2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("P1") == 0)
        ui->P1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("P2") == 0)
        ui->P2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("Q1") == 0)
        ui->Q1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("Q2") == 0)
        ui->Q2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("R1") == 0)
        ui->R1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("R2") == 0)
        ui->R2Label_work->setPixmap(QPixmap(img));
}

// TODO TODELETE
void ImageViewer::viewImage()
{
    __int64 start = ui->startframe->text().toLongLong();
    __int64 end = ui->endframe->text().toLongLong();
    qDebug() << start << end;
    int num = (int) (end - start);
    qDebug() << "ask for image: start = " << start << ", end = " << end << ", num = " << num << ".";
    bool ret = askforimgae(start, num);
    if (!ret)
        updateStatus(QString("CheckedFile中找不到对应主控帧号从%1到%2").arg(start).arg(end));
    else
        setCamerasCurrentFCs(start); // 转到这一帧
}

bool ImageViewer::askforimgae(__int64 startnum, int numofasked)
{
    if (!hasinitmodel)
        return false;

    bool findstart = false;
    int tmpseqno;
    __int64 tmpmasterstartfc = -1;
    __int64 tmpmasterendfc = -1;
    std::list<CheckedItem>::iterator it = currentCheckedModel.begin();
    while (it != currentCheckedModel.end())
    {
        // 还没有找到起始时找起始帧所在文件
        if (!findstart && startnum >= (*it).start_frame_master && startnum < (*it).end_frame_master)
        {
            tmpseqno = (*it).seqno;
            tmpmasterstartfc = (*it).start_frame_master;
            tmpmasterendfc = (*it).end_frame_master;
            findstart = true;
            break;
        }
        it++;
    }

    if (!findstart)
        return false;

    // 如果存储在多个文件中（校正时做了合并），分成两次申请图像，本次申请头一个文件的
    if (tmpmasterendfc <= startnum + numofasked)
        numofasked = tmpmasterendfc - startnum + 1;

    bool ret = true;

    //TODO
    //ProjectModel currentProjectModel = MasterSetting::getSettingInstance()->getProjectModel();
    //QString realfilename = MasterSetting::getSettingInstance()->getProjectPath() + "/" + currentProjectModel.getRealFilename();

    QString tmpcamid;
    __int64 tmpstartfc;
    __int64 tmpendfc;
    bool tmpret = false;

    //TODO
    /*for (int i = 0; i < 5; i++)
    {
        switch (i)
        {
            case 0:  tmpcamid = ui->cam1->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
            case 1:  tmpcamid = ui->cam2->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
            case 2:  tmpcamid = ui->cam3->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
            case 3:  tmpcamid = ui->cam4->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
            case 4:  tmpcamid = ui->cam5->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
            default: break;
        }

        if (tmpcamid.trimmed().compare("") == 0)
            continue;

        if (!tmpret)
            continue;

        // 暂定自己设置的这5台相机
        if (tmpcamid.trimmed().compare("") != 0)
        {
            // 换算每个相机的startfc = startnum - tmpmasterstartfc + tmpstartfc
            qDebug() << "startnum = " << startnum << ", tmpmasterstartfc = " << tmpmasterstartfc << ", tmpstartfc = " << tmpstartfc << ", tempendfc = " << tmpendfc << ", startfc = (startnum - tmpmasterstartfc + tmpstartfc) = " << startnum - tmpmasterstartfc + tmpstartfc;
            tmpret = MasterProgram::getMasterProgramInstance()->askForRawImages(realfilename, tmpcamid, tmpseqno, currenttunnelid, startnum - tmpmasterstartfc + tmpstartfc, numofasked);
            if (tmpret)
            {
                ui->statusArea->append(QObject::tr("图像申请命令实际文件%1，相机%2的从帧号%3一共%4帧申请发送成功。").arg(realfilename).arg(tmpcamid).arg(startnum).arg(numofasked));
                continue;
            }
        }
        ret = false;
        ui->statusArea->append(QObject::tr("图像申请命令实际文件%1，相机%2的从帧号%3一共%4帧申请发送失败！！").arg(realfilename).arg(tmpcamid).arg(startnum).arg(numofasked));
    }*/
    return ret;
}

/**
 * 起始五帧
 */
void ImageViewer::viewBeginFiveImages()
{
    // 已经到最前只能向后申请
    viewdirection = false;

    // 设置帧号，图片路径
    std::list<CheckedItem>::iterator it = currentCheckedModel.begin();
    if (it != currentCheckedModel.end())
    {
        // 还没有找到起始时找起始帧所在文件
        currentseqno = (*it).seqno;
        current_fc_master = (*it).start_frame_master;

        setCamerasCurrentFCs(current_fc_master);

        currenttmpimgpath = this->currentprojpath + "/tmp_img/" + QString("%1_%2_%3").arg(currentseqno).arg(QObject::tr((*it).tunnel_name.c_str())).arg(datetime);
    }

    updateImages();
}

/**
 * 末尾五帧
 */
void ImageViewer::viewEndFiveImages()
{
    // 已经到最后只能向前申请
    viewdirection = true;

    // 设置帧号，图片路径
    std::list<CheckedItem>::reverse_iterator it = currentCheckedModel.rbegin();
    if (it != currentCheckedModel.rend())
    {
        // 还没有找到起始时找起始帧所在文件
        currentseqno = (*it).seqno;
        current_fc_master = (*it).end_frame_master - 5;

        setCamerasCurrentFCs(current_fc_master);

        currenttmpimgpath = this->currentprojpath + "/tmp_img/" + QString("%1_%2_%3").arg(currentseqno).arg(QObject::tr((*it).tunnel_name.c_str())).arg(datetime);
    }

    updateImages();
}

/**
 * 下五帧
 */
void ImageViewer::viewNextFiveFrameImages()
{
    // 向后申请
    viewdirection = false;

    current_fc_master += 1;
    current_fc_1 += 1;
    current_fc_2 += 1;
    current_fc_3 += 1;
    current_fc_4 += 1;
    current_fc_5 += 1;

    updateImages();
    
    /*if(!info.exists())
    {
        QMessageBox::warning(this,tr("温馨提示"), tr("当前已是最后五帧，请申请图像。"),QMessageBox::Ok|QMessageBox::Cancel);
    }*/
}

/**
 * 上五帧
 */
void ImageViewer::viewLastFiveFrameImages()
{
    // 向前申请
    viewdirection = true;

    current_fc_master -= 1;
    current_fc_1 -= 1;
    current_fc_2 -= 1;
    current_fc_3 -= 1;
    current_fc_4 -= 1;
    current_fc_5 -= 1;

    updateImages();

    /*if(currentstartnumber == value)
    {
        QMessageBox::warning(this,tr("温馨提示"), tr("当前已是初始五帧"),QMessageBox::Ok|QMessageBox::Cancel);
    }*/
}

/**
 * 收到解压完成信号
 */
void ImageViewer::receivedepressfinish(QString dir)
{
    updateImages();
    // 强制更新界面
    this->repaint();
}

void ImageViewer::updateStatus(QString parsedmsg)
{
    ui->statusArea->append(parsedmsg);
}

// 转到帧的相关槽函数
void ImageViewer::frameSearch()
{
    QString tmp = ui->frameSearchInput->text();
    __int64 fc = tmp.toLongLong();
    bool ret = setCurrentFCs(fc);
    if (!ret)
        QMessageBox::warning(this, tr("提示"), tr("当前找不到%1这个帧号").arg(fc));
    firsttime = true;
    ui->frameSearchInput->setText(tr("请输入帧号"));
    ui->frameSearchButton->setEnabled(false);
}

void ImageViewer::frameSearchEditToggled()
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

// 图像增强处理
void ImageViewer::imgEnhance(int newif)
{
    ifimgenhance = newif;
    updateImages();
}

// 查找进出洞模式显示
void ImageViewer::findInOutTunnelMode(bool newif)
{
    if (newif)
    {
        // 初始五个相机位置
        ui->cam1->setCurrentIndex(9);  // E2
        ui->cam2->setCurrentIndex(23); // L2
        ui->cam3->setCurrentIndex(36); // 无
        ui->cam4->setCurrentIndex(36); // 无
        ui->cam5->setCurrentIndex(36); // 无
    }
    else
    {
        // 初始五个相机位置
        ui->cam1->setCurrentIndex(1);  // A2
        ui->cam2->setCurrentIndex(9);  // E2
        ui->cam3->setCurrentIndex(15); // H2
        ui->cam4->setCurrentIndex(23); // L2
        ui->cam5->setCurrentIndex(31); // P2
    }
}