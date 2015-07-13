#include "imageviewer_3dtv.h"

#include "ui_imageviewer_3dtv.h"

#include "setting_client.h"
#include "lz_project_access.h"
#include "LzSerialStorageAcqui.h"
#include "LzSerialStorageMat.h"
#include "checkedtask_list.h"
#include <QDebug>
#include "LzCalculator.h"
#include "MatToQImage.h"

#include "show_3dpoints.h"
#include "network_config_list.h"
#include "xmlnetworkfileloader.h"

const int TN_WIDTH = 192;
const int TN_HEIGHT = 480;//图像显示的尺寸

/**
 * 采集文件信息校对界面——原图浏览界面类定义
 * @author 范翔
 * @version 1.0.0
 * @date 2015-03-24
 */
ImageViewer3DTwoViewWidget::ImageViewer3DTwoViewWidget(QWidget *parent) : 
    QWidget(parent), ui(new Ui::ImageViewer3DTwoViewWidget)
{
    ui->setupUi(this);

    widget = new Show3DPointsWidget(this);
    QGridLayout *layout1 = new QGridLayout();
    layout1->addWidget(widget, 0, 0, 0);
    ui->pointsViewWidget->setLayout(layout1);

    // 初始化界面显示图片的label大小
    ui->cam1img1->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam2img1->setFixedSize(TN_WIDTH, TN_HEIGHT);

    currentseqno = -1;
    currenttunnelid = -1;
    hasinitmodel = false;

    current_fc_master = 0;

    currentpointsfile = NULL;
    currentimgfilel = NULL;
    currentimgfiler = NULL;
    fileopen = false;
    fileopenl = false;
    fileopenr = false;

    currentprojpath = "";
    currenttmpimgpath = "";
    currenttmpimgpath_l = "";
    currenttmpimgpath_r = "";

    // 是否做直方图均衡化图像增强处理
    ifimgenhance = 0;
    connect(ui->imgEnhance_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(imgEnhance(int)));

    // 相机改变
    connect(ui->cam1, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCam1andImages(int)));
    // 先是A号相机，默认的 TODO
    updateCam1andImages(0);

    // 播放按钮槽函数
    connect(ui->beginFrameLabel,SIGNAL(clicked()),this,SLOT(viewBeginImages()));
    connect(ui->endFrameLabel,SIGNAL(clicked()),this,SLOT(viewEndImages()));
    connect(ui->lastFrameLabel,SIGNAL(clicked()),this,SLOT(viewLastFrameImages()));
    connect(ui->nextFrameLabel,SIGNAL(clicked()),this,SLOT(viewNextFrameImages()));

    // 添加键盘快捷键，左键前一帧，右键后一帧，CTRL+回车播放（暂停）
    QAction* actionLeft = new QAction(this);
    actionLeft->setShortcut(QKeySequence(Qt::Key_Left));
    QAction* actionRight = new QAction(this);
    actionRight->setShortcut(QKeySequence(Qt::Key_Right));
    
    this->addAction(actionLeft);
    this->addAction(actionRight);
    // 添加键盘快捷键信号槽
    connect(actionLeft, SIGNAL(triggered()), this, SLOT(viewLastFrameImages()));
    connect(actionRight, SIGNAL(triggered()), this, SLOT(viewNextFrameImages()));

    // 默认向后申请
    viewdirection = false;
}

ImageViewer3DTwoViewWidget::~ImageViewer3DTwoViewWidget()
{
    if (currentpointsfile != NULL)
        delete currentpointsfile;
    if (currentimgfilel != NULL)
        delete currentimgfilel;
    if (currentimgfiler != NULL)
        delete currentimgfiler;

    delete widget;
}

void ImageViewer3DTwoViewWidget::updateStatus(QString parsedmsg)
{
    ui->statusArea->append(parsedmsg);
}

void ImageViewer3DTwoViewWidget::updateCam1andImages(int index) { tmpUpdateCameraImage_cam1index = index; updateCameraandImages(); }

void ImageViewer3DTwoViewWidget::updateCameraandImages()
{
    // 关闭所有灯
    changeCamera("", false);

    // 打开灯
    QString cameraindex = ui->cam1->itemText(tmpUpdateCameraImage_cam1index);
    //qDebug() << uicameraindex << cameraindex;
    changeCamera(cameraindex, true);

    setCamerasCurrentFCs(current_fc_master);
    updateImages();
}

/**
 * 图形修正时候原图浏览设置信息函数，无seqnostart版本
 */
void ImageViewer3DTwoViewWidget::setInfo(QString projectpath, QString tmpimg_projectpath, int tunnelid, QString tunnelname)
{
	this->currentprojpath = projectpath;
    this->currenttmpimgpath = tmpimg_projectpath;
    this->currenttunnelid = tunnelid;
    datetime = projectpath.right(8);
    this->currenttunnelname = tunnelname;

    // 默认向后申请
    viewdirection = false;

    bool ret = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main).getCheckedTunnelModel(tunnelid, currentCheckedModel);
    if (!ret)
    {
        currentseqno = -1;
        currenttunnelid = -1;
        hasinitmodel = false;

        current_fc_master = 0;
        current_fc_l = 0;
        current_fc_r = 0;

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
            currenttmpimgpath = projectpath + "/mid_calcu/" + QString::fromLocal8Bit(currentCheckedModel.calcuItem.cal_filename_prefix.c_str());
            currenttmpimgpath_l = projectpath + "/collect/" + QString("%1_%2_%3").arg(currentseqno).arg(QObject::tr((*it).tunnel_name.c_str())).arg(datetime);
            currenttmpimgpath_r = projectpath + "/collect/" + QString("%1_%2_%3").arg(currentseqno).arg(QObject::tr((*it).tunnel_name.c_str())).arg(datetime);

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
bool ImageViewer3DTwoViewWidget::setCurrentFCs(__int64 master_fc)
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
    
    updateImages();
    return true;
}

/**
 * 起始帧
 */
void ImageViewer3DTwoViewWidget::viewBeginImages()
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
    }

    updateImages();
}

/**
 * 末尾帧
 */
void ImageViewer3DTwoViewWidget::viewEndImages()
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
    }

    updateImages();
}

/**
 * 下帧
 */
void ImageViewer3DTwoViewWidget::viewNextFrameImages()
{
    // 向后申请
    viewdirection = false;

    current_fc_master += 1;
    current_fc_l += 1;
    current_fc_r += 1;

    updateImages();
}

/**
 * 上帧
 */
void ImageViewer3DTwoViewWidget::viewLastFrameImages()
{
    // 向前申请
    viewdirection = true;

    current_fc_master -= 1;
    current_fc_l -= 1;
    current_fc_r -= 1;

    updateImages();
}

/**
 * 设置主控fc对应的五组相机fc
 */
bool ImageViewer3DTwoViewWidget::setCamerasCurrentFCs(__int64 master_fc)
{
    std::list<CheckedItem>::iterator it = currentCheckedModel.begin();
    while (it != currentCheckedModel.end())
    {
        // 还没有找到起始时找起始帧所在文件
        if (master_fc >= (*it).start_frame_master && master_fc <= (*it).end_frame_master)
        {
            currentseqno = (*it).seqno;
            QString tmpcamid = ui->cam1->currentText();

            XMLNetworkFileLoader * fileloader = new XMLNetworkFileLoader(this->currentprojpath + "/network_config.xml");
            fileloader->loadFile();
            SlaveModel  tmpmodel = SlaveModel(1.0);
            int find = 0;
            for (int i = 0; i <  NetworkConfigList::getNetworkConfigListInstance()->listsnid()->size(); i++)
            {
                tmpmodel = NetworkConfigList::getNetworkConfigListInstance()->listsnid()->at(i);
                if (QString(tmpmodel.box1.boxindex) == tmpcamid)
                {
                    find = 1;
                    break;
                }
                else if (QString(tmpmodel.box2.boxindex) == tmpcamid)
                {
                    find = 2;
                    break;
                }
            }
            // 清空上一个文件
            if (currentpointsfile != NULL)
            {
                if (fileopen)
                {
                    currentpointsfile->closeFile();
                    fileopen = false;
                }
                delete currentpointsfile;
            }
            //else
            {
                currentpointsfile = new LzSerialStorageMat();
                currentpointsfile->setting(250, 1024*1024*50, true);
            }   
            if (currentimgfilel != NULL)
            {
                if (fileopenl)
                {
                    currentimgfilel->closeFile();
                    fileopenl = false;
                }
                delete currentimgfilel;
            }
            //else
            {
                currentimgfilel = new LzSerialStorageAcqui();
                currentimgfilel->setting(250, 1024*1024*100, true);
            }   
            if (currentimgfiler != NULL)
            {
                if (fileopenr)
                {
                    currentimgfiler->closeFile();
                    fileopenr = false;
                }
                delete currentimgfiler;
            }
            //else
            {
                currentimgfiler = new LzSerialStorageAcqui();
                currentimgfiler->setting(250, 1024*1024*100, true);
            }

            if (find == 1)
            {
                QByteArray tmpba = (currenttmpimgpath + "_" + QString(tmpmodel.box1.boxindex)).toLocal8Bit() + ".mdat";
                fileopen = currentpointsfile->openFile(tmpba.constData());
                if (!fileopen)
                    updateStatus(QObject::tr("文件%1打不开").arg(tmpba.constData()));
                camidl = tmpmodel.box1.camera_ref.c_str();
                tmpba = (currenttmpimgpath_l + "_" + camidl.c_str() + ".dat").toLocal8Bit();
                fileopenl = currentimgfilel->openFile(tmpba.constData());
                if (!fileopenl)
                    updateStatus(QObject::tr("文件%1打不开").arg(tmpba.constData()));
                camidr = tmpmodel.box1.camera.c_str();
                tmpba = (currenttmpimgpath_r + "_" + camidr.c_str() + ".dat").toLocal8Bit();
                fileopenr = currentimgfiler->openFile(tmpba.constData());
                if (!fileopenr)
                    updateStatus(QObject::tr("文件%1打不开").arg(tmpba.constData()));
                updateStatus(QObject::tr("network_config.xml已找到左侧相机%1，右侧相机%2").arg(camidl.c_str()).arg(camidr.c_str()));

            }
            else if (find == 2)
            {
                QByteArray tmpba = (currenttmpimgpath + "_" + QString(tmpmodel.box2.boxindex)).toLocal8Bit() + ".mdat";
                fileopen = currentpointsfile->openFile(tmpba.constData());
                if (!fileopen)
                    updateStatus(QObject::tr("文件%1打不开").arg(tmpba.constData()));
                camidl = tmpmodel.box2.camera_ref.c_str();
                tmpba = (currenttmpimgpath_l + "_" + camidl.c_str() + ".dat").toLocal8Bit();
                fileopenl = currentimgfilel->openFile(tmpba.constData());
                if (!fileopenl)
                    updateStatus(QObject::tr("文件%1打不开").arg(tmpba.constData()));
                camidr = tmpmodel.box2.camera.c_str();
                tmpba = (currenttmpimgpath_r + "_" + camidr.c_str() + ".dat").toLocal8Bit();
                fileopenr = currentimgfiler->openFile(tmpba.constData());
                if (!fileopenr)
                    updateStatus(QObject::tr("文件%1打不开").arg(tmpba.constData()));
                updateStatus(QObject::tr("network_config.xml已找到左侧相机%1，右侧相机%2").arg(camidl.c_str()).arg(camidr.c_str()));

            }
            else
            {
                updateStatus(QObject::tr("network_config.xml中找不到相机%1").arg(ui->cam1->currentText()));
                return false;
            }

            __int64 tmpstartfc;
            __int64 tmpendfc;
            __int64 tmpmasterstartfc = (*it).start_frame_master;
            bool tmpret = false;
               
            tmpret = (*it).getStartEndVal(camidl, tmpstartfc, tmpendfc);
            if (tmpret)
                getTmpCurrentfc(0) = master_fc - tmpmasterstartfc + tmpstartfc;
            else
                updateStatus(QString("相机%1的帧号找不到对应主控帧号为%2的从控文件帧号").arg(camidl.c_str()).arg(master_fc));
            tmpret = (*it).getStartEndVal(camidr, tmpstartfc, tmpendfc);
            if (tmpret)
                getTmpCurrentfc(1) = master_fc - tmpmasterstartfc + tmpstartfc;
            else
                updateStatus(QString("相机%1的帧号找不到对应主控帧号为%2的从控文件帧号").arg(camidr.c_str()).arg(master_fc));

        }
        it++;
    }
    return false;
}

__int64 & ImageViewer3DTwoViewWidget::getTmpCurrentfc(int j)
{
    switch (j)
    {
        case 0: return current_fc_l;
        case 1: return current_fc_r;
        default:return current_fc_l;
    }   
}

bool  & ImageViewer3DTwoViewWidget::getTmphasfc(int j)
{
    switch (j)
    {
        case 0: return tmphasfcl;
        case 1: return tmphasfcr;
        default:return tmphasfcl;
    }   
}

/**
 * 五组原图显示函数
 */
bool ImageViewer3DTwoViewWidget::updateImages()
{
    if (!hasinitmodel)
        return false;

    QString tmpcamid;
    bool ret = true;
    QString tmpcamimgdir;

    QLabel * tmpcamimg = NULL;
    QLabel * tmpcamimg1 = NULL;
    QLabel * tmpcamimg2 = NULL;
    LzSerialStorageAcqui * tmpfileimg = NULL;
    tmphasfcl = false;
    tmphasfcr = false;

    tmpcamid = ui->cam1->currentText(); 
    tmpcamimg1 = ui->cam1img1;
    tmpcamimg2 = ui->cam2img1;

    __int64 & tmpcurrentfc = current_fc_master;

    if (fileopen)
    {
        if (!currentpointsfile->retrieveMat(tmpcurrentfc))
            return false;

        // 当前点
        cv::Mat currentpoints;
        BlockInfo currentblockinfo;

        ret = currentpointsfile->readMat(currentpoints, currentblockinfo);
        if (!ret)
            return false;
        else if (widget != NULL)
        {
            widget->setCurrentMat(currentpoints, true);
            widget->updateGL();
        }
    }

    if (tmpcamid.trimmed().compare("") == 0)
    {
        tmpcamimg1->clear();
        tmpcamimg2->clear();
    }

    // 每个相机的5张图
    for (int j = 0; j < 2; j++)
    {
        bool & tmphasfc = getTmphasfc(j);
        tmphasfc = true;
        if (tmphasfc)
            __int64 & tmpcurrentfc = getTmpCurrentfc(j);
        else
            continue;

        switch (j)
        {
            case 0: tmpcamimg = tmpcamimg1; 
                    if (fileopenl)
                    {
                        if ( !currentimgfilel->retrieveFrame(tmpcurrentfc) ) // TODO
			                return false;
                    }
                    else
                        continue;
                    tmpfileimg = currentimgfilel;
                    break;
            case 1: tmpcamimg = tmpcamimg2; 
                    if (fileopenr)
                    {
                        if ( !currentimgfiler->retrieveFrame(tmpcurrentfc) ) // TODO
			                return false;
                    }
                    else
                        continue;
                    tmpfileimg = currentimgfiler;
                    break;    
            default:break;
        }

        // 做图像增强处理
        cv::Mat img;
        ret = tmpfileimg->readFrame(img);//cv::imread(tmpba.constData(), CV_LOAD_IMAGE_GRAYSCALE);
        if (!ret)
        {
            if (j == 0)
                updateStatus(QObject::tr("相机%1的dat文件无%2帧图片").arg(camidl.c_str()).arg(tmpcurrentfc));
            else
                updateStatus(QObject::tr("相机%1的dat文件无%2帧图片").arg(camidr.c_str()).arg(tmpcurrentfc));
            continue;
        }

        float sclae = 0.3; // 缩放比例
        cv::Size size = cv::Size(img.size().width * sclae, img.size().height * sclae);
        cv::resize(img, img, size);
        //cv::imshow("win1",img);
        //cv::waitKey(0);
        if (ifimgenhance == 1)
        {
            // 直方图规定化处理---By Ding
            cv::Mat result;
            LzCalculator::argument(img, result); 
            //cv::GaussianBlur(result, result, Size(5,5), 1.5, 1.5);
            bigpm = MatToQImage(result);
        }
        else if (ifimgenhance == 2)
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
        else
        {
            bigpm = MatToQImage(img);
        }
            
        smallpm = bigpm.scaled(TN_WIDTH, TN_HEIGHT, Qt::KeepAspectRatio);
        tmpcamimg->setPixmap(QPixmap::fromImage(smallpm));
        tmpcamimg->show();
    }

    qDebug() << "has fc?: " << tmphasfcl << tmphasfcr;

    QLabel * tmpfclabel = NULL;
    QLabel * tmpmilelabel = NULL;

    tmpfclabel = ui->fc1; tmpmilelabel = ui->mile1;

    // 每个相机的masterfc号
    for (int j = 0; j < 2; j++)
    {
        bool & tmphasfc = getTmphasfc(j);
        if (tmphasfc)
        {
            tmpfclabel->setText(QString("%1").arg(current_fc_master));
            tmpmilelabel->setText(QString("%1").arg(current_fc_master));
        }
        else
        {
            tmpfclabel->setText(QString("%1无数据").arg(current_fc_master));
            tmpmilelabel->setText(QString("%1无数据").arg(current_fc_master));
        }
    }

    // 强制更新显示
    this->repaint();
    
    return ret;
}

// 改变相机提示灯
void ImageViewer3DTwoViewWidget::changeCamera(QString cameraindex, bool status)
{
    QString img;
    if (status)
        img = ":/image/lightgreen.png";
    else
    {
        img = ":/image/lightyellow.png";
        ui->A2Label_work->setPixmap(QPixmap(img));
        ui->B2Label_work->setPixmap(QPixmap(img));
        ui->C2Label_work->setPixmap(QPixmap(img));
        ui->D2Label_work->setPixmap(QPixmap(img));
        ui->E2Label_work->setPixmap(QPixmap(img));
        ui->F2Label_work->setPixmap(QPixmap(img));
        ui->G2Label_work->setPixmap(QPixmap(img));
        ui->H2Label_work->setPixmap(QPixmap(img));
        ui->I2Label_work->setPixmap(QPixmap(img));
        ui->J2Label_work->setPixmap(QPixmap(img));
        ui->K2Label_work->setPixmap(QPixmap(img));
        ui->L2Label_work->setPixmap(QPixmap(img));
        ui->M2Label_work->setPixmap(QPixmap(img));
        ui->N2Label_work->setPixmap(QPixmap(img));
        ui->O2Label_work->setPixmap(QPixmap(img));
        ui->P2Label_work->setPixmap(QPixmap(img));
        ui->Q2Label_work->setPixmap(QPixmap(img));
        ui->R2Label_work->setPixmap(QPixmap(img));
        return;
    }
    //qDebug() << cameraindex;
    if (cameraindex.compare("A") == 0)
        ui->A2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("B") == 0)
        ui->B2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("C") == 0)
        ui->C2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("D") == 0)
        ui->D2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("E") == 0)
        ui->E2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("F") == 0)
        ui->F2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("G") == 0)
        ui->G2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("H") == 0)
        ui->H2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("I") == 0)
        ui->I2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("J") == 0)
        ui->J2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("K") == 0)
        ui->K2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("L") == 0)
        ui->L2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("M") == 0)
        ui->M2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("N") == 0)
        ui->N2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("O") == 0)
        ui->O2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("P") == 0)
        ui->P2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("Q") == 0)
        ui->Q2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("R") == 0)
        ui->R2Label_work->setPixmap(QPixmap(img));
}

// 图像增强处理
void ImageViewer3DTwoViewWidget::imgEnhance(int newif)
{
    ifimgenhance = newif;
    updateImages();
}
