#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QDialog>
#include <QPixmap>
#include <QImage>
#include "checkedtask_list.h"

namespace Ui {
    class ImageViewer;
}

/**
 * 采集文件信息校对界面——原图浏览界面类声明
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 2014-08-19
 */
class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewer(QWidget *parent = 0);
    ~ImageViewer();

    /**
	 * 图形修正时候原图浏览设置信息函数，无seqnostart版本
	 */
    void setInfo(QString projectpath, QString tmpimg_projectpath, int tunnelid, QString tunnelname);

	/**
	 * 实际计划校正时的原图浏览设置信息函数，有seqnostart版本
	 */
    void setInfo(QString projectpath, QString tmpimg_projectpath, int tunnelid, int startseqno, QString tunnelname);
    
public slots:
    
    /**
     * 起始五帧
     */
    void viewBeginFiveImages();
    /**
     * 末尾五帧
     */
    void viewEndFiveImages();
    /**
     * 下五帧
     */
    void viewNextFiveFrameImages();
    /**
     * 上五帧
     */
    void viewLastFiveFrameImages();

protected slots:

    // TODO TODELETE
    void viewImage();

    /**
     * 收到解压完成信号
     */
    void receivedepressfinish(QString);

    /**
     * 更新状态栏，接收File_depressing对象的状态信号
     */
    void updateStatus(QString);
    
    /**
     * 更新相机组及对应fc
     */
    void updateCameraandImages();
    // 分别5个相机combobox选择对应的槽函数
    void updateCam1andImages(int);
    void updateCam2andImages(int);
    void updateCam3andImages(int);
    void updateCam4andImages(int);
    void updateCam5andImages(int);

    // 转到帧的相关槽函数
    void frameSearch();
    void frameSearchEditToggled();

    // 图像增强处理
    void imgEnhance(int);

    // 查找进出洞模式显示
    void findInOutTunnelMode(bool);

public:

    /**
     * 设置当前currentfc，外部调用
     */
    bool setCurrentFCs(__int64 master_fc);

protected:

    /**
     * 设置主控fc对应的五组相机fc
     */
    bool setCamerasCurrentFCs(__int64 master_fc);

    /**
     * 五组原图显示函数
     */
    bool updateImages();

    /**
     * 申请查看原图
     */
    virtual bool askforimgae(__int64 startnum, int numofasked);

    /**
     * 改变相机提示灯
     */
    void changeCamera(QString cameraindex, bool status);
    
    Ui::ImageViewer *ui;

    // 是否进行图像增强
    int ifimgenhance;

    // 当前CheckedTaskModel
    CheckedTunnelTaskModel currentCheckedModel;
    bool hasinitmodel;

    QString currentprojpath;
    QString currentprojectpath_tmpimg;
    QString currenttmpimgpath;

    int currentseqno;

    // 主控fc显示
    __int64 current_fc_master;
    // 四组fc，实际文件号对应
    __int64 current_fc_1;
    __int64 current_fc_2;
    __int64 current_fc_3;
    __int64 current_fc_4;
    __int64 current_fc_5;
    __int64 & getTmpCurrentfc(int i);

    bool tmphasfc1;
    bool tmphasfc2;
    bool tmphasfc3;
    bool tmphasfc4;
    bool tmphasfc5;
    bool  & getTmphasfc(int i);

    int currenttunnelid;
    QString currenttunnelname;
    int lineid;
    QString datetime;

    // 原图预览用
    QImage bigpm, smallpm;

    /**
     * 申请原图方向
     * true向前申请,false为向后申请，默认向后
     */
    bool viewdirection;

    /**
     * 相机位置提示图需要
     */
    int tmpUpdateCameraImage_cam1index;
    int tmpUpdateCameraImage_cam2index;
    int tmpUpdateCameraImage_cam3index;
    int tmpUpdateCameraImage_cam4index;
    int tmpUpdateCameraImage_cam5index;

    // 第一次点击framesearchInput
    bool firsttime;
};

#endif // IMAGEVIEWER_H
