#ifndef CORRECT_CLEARANCE_H
#define CORRECT_CLEARANCE_H

#include <QWidget>
#include <QSqlTableModel>

#include "drawimage.h"
#include "imageviewer_client.h"
#include "imageviewer_3dtv.h"
#include "clearance_item.h"
#include <QScrollArea>

#include "daooutputitem.h"
#include "LzSerialStorageBase.h"
#include "LzSerialStorageSynthesis.h"

#include "LzSerialStorageMat.h"
#include <opencv2\opencv.hpp>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>

#include "LzException.h"

#include "output_heights_list.h"
#include "clearance_edit_record.h"
#include <QTimer>

#include "lz_output_access.h"
#include "lz_output.h"
#include "textedit.h"
#include "fancybrowser.h"

using namespace cv;

namespace Ui {
    class CorrectClearanceWidget;
}

/**
 * 图形修正（隧道综合之前）界面类声明
 *
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 20140923
 */
class CorrectClearanceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CorrectClearanceWidget(QWidget *parent = 0);
    ~CorrectClearanceWidget();

    void loadSynthesisDataFile();
    void closeSynthesisDataFile();

private:
    Ui::CorrectClearanceWidget *ui;

    // 查看更多原图界面
    ClientImageViewer * finalimage;
    ImageViewer3DTwoViewWidget * image3d;

    // 是否显示RT，融合结果在图像中
    bool ifShowFuse;
    // 是否显示提高度结果
    bool ifShowSynthesis;
    // 是否显示中心高度
    bool ifShowMinHeight;
    // 是否正在修改中心高度
    bool isCreateShowMinHeight;
    
    /**
     * LzSerialStorageMat对应存储下来的Mat
     */
    Mat R_point, T_point, Fuse_pnts;
    /**
     * R矩阵流式存储类
     */
    LzSerialStorageMat * lzMat_R;
    bool isfileopen_R_mat;
    /**
     * T矩阵流式存储类
     */
	LzSerialStorageMat * lzMat_T;
    bool isfileopen_T_mat;
    /**
     * 融合结果流式存储类Mat类型
     */
	LzSerialStorageMat * lzMat_Fuse;
    bool isfileopen_fuse;

    /**
     * syn读出的数据
     */
    SectionData data;

    /**
     * 提高度结果流式文件查看
     */
    LzSerialStorageSynthesis * syn;
    bool isfileopen_syn;
    std::vector<BLOCK_KEY> keys;
    
    /**
     * 当前隧道采集时车厢正反（正向为面向生活区方向，反向为背对生活区方向）
     * 修改限界左右，便于人眼直观回放查看
     */
    bool currentCarriageDirection;

    /**
     * 当前正在修正帧号
     */
    __int64 framecounter;
    
    /**
     * 当前正在修正帧里程
     */
    double mile;

    /**
     * 当前正在修正帧中心线净高
     */
    float centerHeight;

    /**
     * 是否随机遍历framecounter,默认从keys.at(0)开始
     */
    bool israndom;
    
    string synfilename;

    QTimer *timer;

    // 画图界面
    SectionImage *imagesection;

    QScrollArea *scrollArea;

    int scale;//比例
    double pixeltomm;//像素与毫米的转化
    int widthpixel;//像素宽
    int originy;//坐标基准y

    // 是否正在播放
    bool isplay;
    int synscale;

    // 当前界面是否正在显示，若不是，不用在选择切换帧时随时更新画图，若是，则切换
    bool widgetactive;

    // 是否正在编辑限界
    bool iseditingsection;
    int currenttunnelid;
    QString currenttunnelname;
    QString currenttunneldate;
    string logfilename;
    bool hasinitlogfile;

    EditRecordStack * editingrecord;

    /**
     * 兰州隧道限界图表文件输出类
     */
    LzOutputAccess * outputAccess;

    /**
     * PDF预览接口界面
     */
    TextEdit *edit;

    /**
	 * 浏览器界面
	 */
	FancyBrowser * webview;

    /**
     * 限界图表html和excel模板路径
     */
    QString templatepath;

    /**
     * 转到帧相关
     */
    bool firsttime;
private:
    /**
     * makedir，在用textEdit生成html时不选择生成目录，因此可能目录不存在
     */
    void makedir(QString filename);
    
    /**
     * 初始化PDF预览接口
     */
    void initTextEdit();

  	/**
	 * 初始化浏览器界面
	 */
	void initFancyBrowser();

    /**
     * statuslabel显示状态
     */
    void statusShow(QString outfilename, int retval, bool isappend);

private slots:

    // 转到帧的相关槽函数
    void frameSearch();
    void frameSearchEditToggled();

    /**
     * 获得画图界面修改断面数据的高度和限界半宽
     * @param isupdateval 是否更新到Sectiondata中，若否，在界面更新显示坐标，若是，同时更新到SectionData中
     */
    void updateSectionData(int newval, int newheight, bool isleft, bool isupdateval);

    /**
     * 切换播放到随机帧，帧过滤窗口需要
     */
    void viewFrameAt(__int64 frame);

    /**
     * 播放器按钮槽函数
     */
    void viewFirstFrame();
    void viewEndFrame();
    void viewLastFrame();
    void viewNextFrame();
    void playAllframe();
    void stopframe();

    /**
     * 改变播放速度
     */
    void changeTimer(int speedspan);

    /**
     * 保存修改槽函数
     */
    void saveButton();
    /**
     * 撤销一步操作槽函数
     */
    void cancelOneStep();

    /**
     * 单幅断面打印预览槽函数
     */
    void htmlPreview();
    void excelPreview();

    // 关闭文件并，（上一步）切换界面信号槽定义
    void finishButton();

    /**
     * 按快捷键槽函数
     */
    void keyPlayStop();
    void keyZoomIn();
    void keyZoomOut();

    // 看原图
    void showImageWidget(bool);
    // 显示高度辅助线
    void showhelpline(bool);
    // 是否显示底板
    void showFloor(bool);
    // 是否显示融合计算结果
    void showFuse(bool);
    // 是否显示提取高度结果
    void showSynthesis(bool);

    // 显示最小高度
    void showMinHeightValid(bool);

    // 更多原图
    void moreImage();
    void moreImage3D();

    void correctscale(int);

    /**
     * 载入限界图到绘图界面
     */
    void loadGaugeImage();

    /**
     * 得到修正点的最新位置坐标
     * @param int,int 分中坐标系下坐标（单位：毫米）
     */
    void getCorrectPos(int,int);

    /**
     * 接收鼠标坐标值，并更新到显示坐标处
     * 现在的点是像素，要转换成毫米
     * @param int,int 分中坐标系下坐标（单位：毫米）
     */
    void getMousePos(int,int);

    void getparameterfromtest(int,double,int,int);
    
    void getspinBoxvalue(int);

    // 更新GraphsView的AcrollBar槽【弃用】
    void resizeGraphWidget();

    // 下一步界面切换槽函数
    //void on_nextWidgetButton_clicked();

    /**
     * 转变播放器Button样式
     * @param i 播放按钮的ID，依次从左到右为1-6
     */
    void setPlayerButtonStyle(int i);

  	void changedata();

public slots:
    // 从上一步接收文件名参数界面切换槽函数
    void slotSelectedTunnelToEdit(int tunnelid, QString signalfilename, bool carriagedir);
    
    // 设置车厢正反
    void slotSetCarriageDirection(bool newdirect);

    // 未选择综合的帧号，则直接从第一帧浏览
    void startViewFromFirst(bool newdirect);
    // 得到综合界面选择的帧号，在这里直接传入显示改帧
	void startViewFromSelectedFrame(_int64);

    // 参数均为矩形区域左上、右下角的图像坐标（像素）
    void getRectPoint(float topleftx, float toplefty, float bottomrightx, float bottomrighty, bool carriagedirection);

signals:
    void sendspinBoxvalue(int);
    void sendtosyn(int);
    void finish();

    /**
     * 给原图浏览界面发送信号
     */
    void viewRawImageSignalAtBegin();
    void viewRawImageSignalAtEnd();
    void viewRawImageSignalAtLast();
    void viewRawImageSignalAtNext();

    // 下一步界面切换信号【放弃】
    //void signalSelectedTunnelToSynthesis(QString);
};

#endif // CORRECT_CLEARANCE_H
