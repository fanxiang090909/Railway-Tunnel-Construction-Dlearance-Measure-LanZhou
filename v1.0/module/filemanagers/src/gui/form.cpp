#include "form.h"
#include "ui_form.h"

#include <QTextCodec>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegExpValidator>

#include "LzSerialStorageAcqui.h"
#include "MatToQImage.h"

using namespace std;

const int TN_WIDTH = 128;
const int TN_HEIGHT = 320;//图像显示的尺寸

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    
    QIcon icon;
    icon.addFile(QString::fromUtf8(":player/player_play.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);

    // 初始化界面显示图片的label大小
    ui->cam1img1->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam1img2->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam1img3->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam1img4->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->cam1img5->setFixedSize(TN_WIDTH, TN_HEIGHT);

    // 播放按钮槽函数
    connect(ui->beginFrameLabel_2,SIGNAL(clicked()),this,SLOT(viewBeginFiveImages()));
    connect(ui->endFrameLabel_2,SIGNAL(clicked()),this,SLOT(viewEndFiveImages()));
    connect(ui->lastFrameLabel_2,SIGNAL(clicked()),this,SLOT(viewLastFiveFrameImages()));
    connect(ui->nextFrameLabel_2,SIGNAL(clicked()),this,SLOT(viewNextFiveFrameImages()));

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

    // 是否做直方图均衡化图像增强处理
    ifimgenhance = false;
    connect(ui->imgenhanceButton_2, SIGNAL(toggled(bool)), this, SLOT(imgEnhance(bool)));

    // 如果是图形修正界面看原图，隐藏播放
    ui->widget->setVisible(false);

    // 查找帧号
    connect(ui->frameSearchButton_2, SIGNAL(clicked()), this, SLOT(frameSearch()));
    connect(ui->frameSearchInput_2, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(frameSearchEditToggled()));
    // 第一次点击frameSearchInput 设置正则表达式
    firsttime = true;
    ui->frameSearchButton_2->setEnabled(false);
    
    tmpopendir = "";
    file_full = "";
    depressdir = "";

    connect(ui->openFileButton, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->loadFileButton, SIGNAL(clicked()), this, SLOT(loadFile()));
    connect(ui->depressFileButton, SIGNAL(clicked()), this, SLOT(depressFile()));
    connect(ui->depressdirButton, SIGNAL(clicked()), this, SLOT(setDepressDir()));

    ui->openFileButton->setEnabled(true);
    ui->loadFileButton->setEnabled(false);
    ui->depressFileButton->setEnabled(false);

    viewdirection = true;

    // 默认文件起始终止帧
    startfr = 0;
    endfr = 0;
    current_fc = 0;
}

Form::~Form()
{
    delete ui;
}

void Form::setParentPath(QString newpath)
{
    tmpopendir = newpath;
}

/*******************************/
void Form::appendMsg(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
}

void Form::appendError(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
}

void Form::openFile()
{
    file_full = QFileDialog::getOpenFileName(this, QObject::tr("打开流式文件"), tmpopendir, "Document files (*.dat)");
    tmpopendir = QFileInfo(file_full).absoluteFilePath();
    ui->currentFile->setText(file_full);
    
    QDir my_dir = QDir(QFileInfo(file_full).filePath());
    QString filename = QFileInfo(file_full).fileName();
    filename = filename.left(filename.size() - 4);
    qDebug() << my_dir.absolutePath() << filename;

    // 更新相机号显示
    ui->cam1->setText(filename.mid(filename.length() - 2));
    
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

    if (ret)
        depressdir = rootdir + "/tmp_img/" + filename + "/";
    else
        depressdir = QFileInfo(file_full).filePath() + "/tmp_img/" + filename + "/"; 
    ui->depressdir->setText(depressdir);
   
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
    LzSerialStorageAcqui *lzserialimg = new LzSerialStorageAcqui();
    try {
            
        bool ret = lzserialimg->openFile(openfilename.c_str());
        if (!ret)
        {
             delete lzserialimg;
             appendMsg(tr("无法打开文件%1！").arg(file_full));
             return;
        }

        lzserialimg->readHead(&datahead);
        qDebug() << "data head frame num :" << datahead.frame_num;

        std::vector<BLOCK_KEY> keys = lzserialimg->readKeys();
        qDebug() << "file frames vector size:" << keys.size();
        if (keys.size() <= 0)
        {
             delete lzserialimg;
             appendMsg(tr("文件内无有效帧！"));
             return;
        }
        startfr = keys.at(0);
        endfr = keys.at(keys.size() - 1);

        ui->linename->setText(QString::fromLocal8Bit(datahead.line_name));
        ui->tunnelname->setText(QString::fromLocal8Bit(datahead.tunnel_name));
        bool isdouble = datahead.is_double_line;
        if (isdouble)
            ui->isdouble->setText(tr("双线"));
        else
            ui->isdouble->setText(tr("单线"));
        
        bool isnormal = datahead.is_normal;
        if (isnormal && isdouble)
            ui->isnormal->setText(tr("正常拍摄"));
        else if (!isnormal && isdouble)
            ui->isnormal->setText(tr("非正常拍摄"));
        else if (!isnormal && !isdouble)
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
    delete lzserialimg;

    ui->loadFileButton->setEnabled(true);
    ui->depressFileButton->setEnabled(true);
}

void Form::loadFile()
{
    ui->widget->setVisible(true);

    setCurrentFCs(startfr);
    bool ret = updateImages();
    if (ret)
        appendMsg("加载成功!");
    else
        appendMsg("加载失败!不存在小图片,请解压后加载");
}

void Form::depressFile()
{
    if (!QFile(file_full).exists())
    {
        QMessageBox::warning(this, tr("提示"), tr("打开文件%1不存在，请重新设置！").arg(file_full));
        return;
    }

    QDir my_dir;
    if (!my_dir.exists(depressdir))
    {
		my_dir.mkpath(depressdir);
        appendMsg(QObject::tr("源图像存储目录成功生成%1。").arg(depressdir));
    }

    QByteArray tmpba = file_full.toLocal8Bit();
    std::string openfilename = tmpba.constData();
    tmpba = depressdir.toLocal8Bit();
    std::string outputdir = tmpba.constData();

    startfr = 0;
    endfr = 0;
    // 查找是否有该线路
    LzSerialStorageAcqui *lzserialimg = new LzSerialStorageAcqui();
    try {
            
        bool ret = lzserialimg->openFile(openfilename.c_str());
        if (!ret)
        {
             delete lzserialimg;
             appendMsg(tr("无法打开文件%1！").arg(file_full));
             return;
        }

        //lzserialimg->readHead(&datahead);
        //qDebug() << "data head frame num :" << datahead.frame_num;

        std::vector<BLOCK_KEY> keys = lzserialimg->readKeys();
        qDebug() << "file frames vector size:" << keys.size();
        if (keys.size() <= 0)
        {
             delete lzserialimg;
             appendMsg(tr("文件内无有效帧！"));
             return;
        }
        startfr = keys.at(0);
        endfr = keys.at(keys.size() - 1);
        qDebug() << "file frames vector startfc: " << startfr << ", endfc: " << endfr;
        
        ret = lzserialimg->readFrameSaveAsImage(startfr, endfr, outputdir.c_str(), true);

        lzserialimg->closeFile();
        
        if (ret)
        {
            if (ifimgenhance)
                appendMsg(QObject::tr("从流式文件%1生成临时小图片成功，图像增强结果").arg(file_full));
            else
            {
                appendMsg(QObject::tr("从流式文件%1生成临时小图片成功").arg(file_full));
                loadFile();
                ui->loadFileButton->setEnabled(false);
            }
        }
        else
            appendMsg(QObject::tr("从流式文件%1生成临时小图片失败").arg(file_full));
    }
    catch (LzException & ex)
    {
        appendError(QObject::tr("从流式文件%1生成临时小图片失败").arg(file_full));
    }
    delete lzserialimg;
}

void Form::setDepressDir()
{
    // 选择创建目录，默认为刚刚创建的目录
    QFileDialog *fd = new QFileDialog(this, tr("创建新工程，请选择工程存放目录"));
    fd->setFileMode(QFileDialog::DirectoryOnly);
    fd->setDirectory(depressdir);
    if(fd->exec() == QFileDialog::Accepted) // ok
    {
        QStringList folders = fd->selectedFiles();
        if(folders.size() <= 0)
        {
            appendMsg(tr("未选择解压目录。"));
            delete fd;
            return;
        }
        depressdir = folders[0];
        qDebug() << "directory" << depressdir;
        ui->depressdir->setText(depressdir);
    }
    else
    {
        delete fd;
        return;
    }
    delete fd;
}

/**
 * 设置主控fc对应的五组相机fc
 */
bool Form::setCurrentFCs(__int64 master_fc)
{
    if (master_fc <= endfr && master_fc >= startfr)
    {    
        current_fc = master_fc;
        return true;
    }
    else
        return false;
}

__int64 & Form::getTmpCurrentfc(int j)
{
    return current_fc;
}

bool  & Form::getTmphasfc(int j)
{
    switch (j)
    {
        case 0: return tmphasfc1;
        case 1: return tmphasfc2;
        case 2: return tmphasfc3;
        case 3: return tmphasfc4;
        case 4: return tmphasfc5;
        case 5: return tmphasfc6;
        case 6: return tmphasfc7;
        case 7: return tmphasfc8;
        case 8: return tmphasfc9;
        default:return tmphasfc1;
    }   
}

/**
 * 五组原图显示函数
 */
bool Form::updateImages()
{
    QString tmpcamid = ui->cam1->text();
    bool ret = true;
    QString tmpcamimgdir;

    QLabel * tmpcamimg1 = ui->cam1img1;
    QLabel * tmpcamimg2 = ui->cam1img2;
    QLabel * tmpcamimg3 = ui->cam1img3;
    QLabel * tmpcamimg4 = ui->cam1img4;
    QLabel * tmpcamimg5 = ui->cam1img5;
    QLabel * tmpcamimg6 = ui->cam1img6;
    QLabel * tmpcamimg7 = ui->cam1img7;
    QLabel * tmpcamimg8 = ui->cam1img8;
    QLabel * tmpcamimg9 = ui->cam1img9;
    QLabel * tmpcamimg = NULL;
    tmphasfc1 = false;
    tmphasfc2 = false;
    tmphasfc3 = false;
    tmphasfc4 = false;
    tmphasfc5 = false;
    tmphasfc6 = false;
    tmphasfc7 = false;
    tmphasfc8 = false;
    tmphasfc9 = false;

    __int64 & tmpcurrentfc = getTmpCurrentfc(0);

    tmpcamimgdir = depressdir;

    // 每个相机的5张图
    for (int j = -4; j < 5; j++)
    {
        switch (j)
        {
            case -4: tmpcamimg = tmpcamimg1; break;
            case -3: tmpcamimg = tmpcamimg2; break;
            case -2: tmpcamimg = tmpcamimg3; break;
            case -1: tmpcamimg = tmpcamimg4; break;
            case 0: tmpcamimg = tmpcamimg5; break;
            case 1: tmpcamimg = tmpcamimg6; break;
            case 2: tmpcamimg = tmpcamimg7; break;
            case 3: tmpcamimg = tmpcamimg8; break;
            case 4: tmpcamimg = tmpcamimg9; break;
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

        bool & tmphasfc = getTmphasfc(j + 4);
        tmphasfc = true;

        // 做图像增强处理
        QByteArray tmpba = filename3.toLocal8Bit();
        cv::Mat img = cv::imread(tmpba.constData(), CV_LOAD_IMAGE_GRAYSCALE);
        float sclae = 0.3; // 缩放比例
        cv::Size size = cv::Size(img.size().width * sclae, img.size().height * sclae);
        cv::resize(img, img, size);
        //cv::imshow("win1",img);
        //cv::waitKey(0);
        QImage smallpm, bigpm;
        if (ifimgenhance)
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

            // 直方图均衡化---效果较差
            //cv::equalizeHist(img, result);
            //cv::imwrite("D:\\2.png", result);
            //cv::imshow("win1",result);
            //cv::waitKey(0);
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

    qDebug() << "has fc?: " << tmphasfc1 << tmphasfc2 << tmphasfc3 << tmphasfc4 << tmphasfc5;

    QLabel * tmpfclabel = NULL;

    // 每个相机的masterfc号
    for (int j = -4; j < 5; j++)
    {
        switch (j)
        {
            case -4: tmpfclabel = ui->fc1; break;
            case -3: tmpfclabel = ui->fc2; break;
            case -2: tmpfclabel = ui->fc3; break;
            case -1: tmpfclabel = ui->fc4; break;
            case 0: tmpfclabel = ui->fc5; break;
            case 1: tmpfclabel = ui->fc6; break;
            case 2: tmpfclabel = ui->fc7; break;
            case 3: tmpfclabel = ui->fc8; break;
            case 4: tmpfclabel = ui->fc9; break;
            default:break;
        }
        bool & tmphasfc = getTmphasfc(j + 4);
        if (tmphasfc)
        {
            tmpfclabel->setText(QString("%1").arg(current_fc + j));
        }
        else
        {
            tmpfclabel->setText(QString("%1无数据").arg(current_fc + j));
        }
    }
    // 更新界面
    this->repaint();
    return ret;
}

/**
 * 起始五帧
 */
void Form::viewBeginFiveImages()
{
    // 已经到最前只能向后申请
    viewdirection = false;

    current_fc = startfr;

    setCurrentFCs(current_fc);

    updateImages();
}

/**
 * 末尾五帧
 */
void Form::viewEndFiveImages()
{
    // 已经到最后只能向前申请
    viewdirection = true;

    // 设置帧号，图片路径
    current_fc = endfr;

    setCurrentFCs(current_fc);

    updateImages();
}

/**
 * 下五帧
 */
void Form::viewNextFiveFrameImages()
{
    // 向后申请
    viewdirection = false;

    current_fc += 1;

    updateImages();
}

/**
 * 上五帧
 */
void Form::viewLastFiveFrameImages()
{
    // 向前申请
    viewdirection = true;

    current_fc -= 1;

    updateImages();

}

// 转到帧的相关槽函数
void Form::frameSearch()
{
    QString tmp = ui->frameSearchInput_2->text();
    __int64 fc = tmp.toLongLong();
    bool ret = setCurrentFCs(fc);
    if (!ret)
        QMessageBox::warning(this, tr("提示"), tr("当前找不到%1这个帧号").arg(fc));
    firsttime = true;
    ui->frameSearchInput_2->setText(tr("请输入帧号"));
    ui->frameSearchButton_2->setEnabled(false);
    // 更新图像
    updateImages();
}

void Form::frameSearchEditToggled()
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

// 图像增强处理
void Form::imgEnhance(bool newif)
{
    ifimgenhance = newif;

    if (ifimgenhance)
        appendMsg(QObject::tr("设置为增强模式"));
    else
        appendMsg(QObject::tr("设置为原图模式"));

    updateImages();
}