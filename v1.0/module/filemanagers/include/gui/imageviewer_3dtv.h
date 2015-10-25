#ifndef IMAGEVIEWER_3D_TWOVIEW_H
#define IMAGEVIEWER_3D_TWOVIEW_H

#include <QWidget>
#include <QPixmap>
#include <QImage>
#include "checkedtask_list.h"
#include "show_3dpoints.h"
#include "LzSerialStorageMat.h"
#include "LzSerialStorageAcqui.h"

namespace Ui {
    class ImageViewer3DTwoViewWidget;
}

/**
 * 采集文件信息校对界面——原图浏览界面类声明
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 2014-08-19
 */
class ImageViewer3DTwoViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewer3DTwoViewWidget(QWidget *parent = 0);
    ~ImageViewer3DTwoViewWidget();

    /**
	 * 图形修正时候原图浏览设置信息函数，无seqnostart版本
	 */
    void setInfo(bool isTwoNasMode, QString projectpath, QString projectname, QString tmpimg_projectpath, int tunnelid, QString tunnelname);

public:

    /**
     * 设置当前currentfc，外部调用
     */
    bool setCurrentFCs(__int64 master_fc);

	//@zengwang 2015年10月13日
	//得到Nas工作模式，具体读写是一个Nas还是两个Nas
	bool getNasMode();

signals:
    void finish();

public slots:
    
    /**
     * 起始帧
     */
    void viewBeginImages();
    /**
     * 末尾帧
     */
    void viewEndImages();
    /**
     * 下帧
     */
    void viewNextFrameImages();
    /**
     * 上帧
     */
    void viewLastFrameImages();

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
     * 改变相机提示灯
     */
    void changeCamera(QString cameraindex, bool status);

    /**
     * 更新状态栏，接收File_depressing对象的状态信号
     */
    void updateStatus(QString);

	

private slots:
    // 图像增强处理
    void imgEnhance(int);

    // 改变加载图像和点云数据
    void updateCam1andImages(int index);

    void updateCameraandImages();

    void exitOuter();

    // 转到帧的相关槽函数
    void frameSearch();
    void frameSearchEditToggled();

private:
    Ui::ImageViewer3DTwoViewWidget *ui;

    /**
     * 浏览原图方向
     * true向前申请,false为向后申请，默认向后
     */
    bool viewdirection;

    // 是否进行图像增强
    int ifimgenhance;

    // 当前CheckedTaskModel
    CheckedTunnelTaskModel currentCheckedModel;
    bool hasinitmodel;

	QString projectname;
    QString currentprojpath;
    QString currenttmpimgpath;
    QString currenttmpimgpath_l;
    QString currenttmpimgpath_r;

    int currentseqno;
    // 主控fc显示
    __int64 current_fc_master;
    // 四组fc，实际文件号对应
    __int64 current_fc_l;
    __int64 current_fc_r;
    __int64 & getTmpCurrentfc(int i);

    bool tmphasfcl;
    bool tmphasfcr;
    bool  & getTmphasfc(int i);

    LzSerialStorageMat * currentpointsfile;
    LzSerialStorageAcqui * currentimgfilel;
    LzSerialStorageAcqui * currentimgfiler;
    string camidl;
    string camidr;
    bool fileopen;
    bool fileopenl;
    bool fileopenr;

    int currenttunnelid;
    QString currenttunnelname;
    int lineid;
    QString datetime;

    // 原图预览用
    QImage bigpm, smallpm;

    Show3DPointsWidget * widget;

    // 当前选择的相机
    int tmpUpdateCameraImage_cam1index;

	//@zengwang 2015年10月13日
	//设定当前是一个Nas模式还是两个Nas模式
	bool isTwoNasMode;

    // 转到帧的相关
    bool firsttime;
};

#endif // IMAGEVIEWER_3D_TWOVIEW_H
