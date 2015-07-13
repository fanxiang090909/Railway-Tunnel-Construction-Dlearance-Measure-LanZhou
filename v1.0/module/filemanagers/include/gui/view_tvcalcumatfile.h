#ifndef VIEWTVCALCUDATAWIDGET_H
#define VIEWTVCALCUDATAWIDGET_H

#include <QWidget>

#include <QDir>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QTime>
#include <QScrollArea>

#include "headdefine.h"
#include "LzSerialStorageMat.h"
#include "LzSerialStorageSynthesis.h"

#include "drawimage.h"
#include "loadcmpdata.h"

namespace Ui {
    class ViewTVCalcuMatDataWidget;
}

class ViewTVCalcuMatDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViewTVCalcuMatDataWidget(QWidget *parent = 0);
    ~ViewTVCalcuMatDataWidget();

    void setParentPath(QString path);

signals:
    void sendspinBoxvalue(int);

private slots:

    void openFile();
    void closeSynthesisDataFile();

    /**** TO paste to widget two of masterGUI program ****/
    void appendMsg(QString msg);
    void appendError(QString msg);

    // 转到帧的相关槽函数
    void frameSearch();
    void frameSearchEditToggled();

    void getspinBoxvalue(int value);

    // 显示鼠标位置槽函数
    void getMousePos(int, int);

    // 显示高度辅助线
    void showHeights(bool);

    // 显示综合提取高度结果
    void showHeightsSynResult(bool);

    // 显示对比数据界面
    void loadCompareDataWidget();

public slots:
    
    void loadGaugeImage();

    /**
     * 切换播放到随机帧，帧过滤窗口需要
     */
    bool viewFrameAt(__int64 frame);

    /**
     * 播放器按钮槽函数
     */
    void viewFirstFrame();
    void viewEndFrame();
    void viewLastFrame();
    void viewNextFrame();
    
    /**
     * 按快捷键槽函数
     */
    void keyZoomIn();
    void keyZoomOut();

    /**
     * 得到对比限界值
     */
    void getCompareSectionData();

private:
    Ui::ViewTVCalcuMatDataWidget *ui;

    // 画图界面
    SectionImage *imagesection;

    QScrollArea *scrollArea;

    // 计算时间间隔使用
    QDateTime lastTime;
    int m_time;

    /**
     * 数据目录
     */
    QString tmpopendir;
    // 流式文件绝对路径 
    QString file_full;
    // 如果打开的file_full文件结尾是.fdat，则尝试打开rtfile
    QString rtfile_full;
    // 如果打开的file_full文件结尾是.fdat，则尝试打开synfile
    QString synfile_full;

    /**
     * 融合结果流式存储类Mat类型
     */
	LzSerialStorageMat * lzMat_Fuse;
    bool isfileopen_fuse;

    /**
     * LzSerialStorageMat对应存储下来的Mat
     */
    Mat Fuse_pnts;

    /**
     * RT结果流式存储类Mat类型
     */
	LzSerialStorageMat * lzMat_RT;
    bool isfileopen_RT;
    /**
     * LzSerialStorageMat对应存储下来的Mat
     */
    Mat tmprt;
    QString Rstr, Tstr;

    /**
     * 提高度结果流式存储类对象
     */
    LzSerialStorageSynthesis * lzSyn;
    bool isfileopen_syn;
    SectionData sectiondata;

    DataHead datahead;

    // 第一次点击framesearchInput
    bool firsttime;

    // 当前车厢正反，与原图浏览的坐标系左右相关
    bool currentCarriageDirection;

    // 起始终止帧
    BLOCK_KEY startfr;
    BLOCK_KEY endfr;
    std::vector<BLOCK_KEY> keys;

    __int64 current_fc;

    // Mat中存储数据格式,画图相关
    bool isFuse;

    // Mat 中存储数据格式，画图相关
    bool isQR;

    // 是否显示高度辅助线
    bool ifshowInterestHeights;

    // 是否显示提高度结果
    bool ifshowHeightsSynResult;

    // 是否和对比数据做对比
    bool ifcompareToComareData;
    
    /**
     * 对比数据界面
     */
    LoadCompareDataWidget * loadWidget;
};

#endif // VIEWTVCALCUDATAWIDGET_H
