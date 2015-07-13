#ifndef SYNTHESIS_TUNNEL_H
#define SYNTHESIS_TUNNEL_H

#include <QWidget>
#include <string>
#include <QScrollArea>
#include "drawimage.h"
#include "clearance_item.h"

#include <QSqlTableModel>
#include "daooutputitem.h"
#include "LzSerialStorageBase.h"
#include "LzSerialStorageSynthesis.h"
#include "LzException.h"
#include "output_heights_list.h"
#include "setting_client.h"

using namespace std;

namespace Ui {
    class SynthesisTunnelWidget;
}

/**
 * 隧道综合选择界面类声明
 *
 * @author 范翔
 * @version 1.0.0
 * @date 20140403
 */
class SynthesisTunnelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SynthesisTunnelWidget(QWidget *parent = 0);
    ~SynthesisTunnelWidget();

    /**
     * 加载隧道基本数据
     */
    void loadBasicTunnelData();
    /**
     * 加载综合结果数据，从数据库中读出
     */
    void loadSynthesisData();
   
    /**
     * 添加实际采集“隧道”记录文件，并显示
     */
    bool loadCheckedTaskTunnelData();
    /**
     * 显示综合结果数据或断面数据载入画图数据
     */
    void loadGaugeImage(ClearanceData& data, CurveType newtype);

public slots:
    /**
     * 界面之间消息传递槽函数
     */
    void slotSelectedTunnelToSynthesis(int newtunnelid, QString, bool);
    void activatecombox();
    void setLineType(int,int,int);

    /**
     * 转发消息
     */
    void getInterestRectangle(float, float, float, float);

signals:
    // 界面切换信号
    // 返回到选择
    void finish();
    
    // 转到修正界面开始浏览
    void startFromFirst(bool carriagedireciton);
    // 转到修正界面开始浏览，选择帧号信号
    void startFromSelectedFrame(_int64);
    
    // 图形放大缩小重绘
    void sendspinBoxvalue(int);

    // 隧道综合界面的基础信息（显示缩放因子scale、pixel2mm、分中坐标原点x、y）
    void Sendparametertocorrect_clearance(int,double,int,int);

    /**
     * 告知外部界面筛选出的帧的帧号、里程、及左右侧位置
     */
    void updateFramesInSearchArea(__int64, float, int);
    void clearlistwidget();
    

    // 参数均为矩形区域左上、右下角的图像坐标（像素）
    // bool 车厢正反
    void sendRectPoint(float, float, float, float, bool);

    void sendfloornumber(int);
    void sendimagetype(int);

private:
    Ui::SynthesisTunnelWidget *ui;   

    /**
     * 当前正在编辑用户
     */
    QString username;
    /**
     * 当前正在编辑隧道所在工程名
     */
    QString projectname;
    /**
     * 当前正在编辑隧道ID
     */
    int tunnelid;
    /**
     * 当前正在编辑隧道名
     */
    QString tunnelname;
    /**
     * 当前正在编辑隧道的采集车厢方向，正、反
     */
    bool currentcarriagedirection;
    bool carriagedirectionlock;

    /**
     * 当前隧道采集日期
     */
    QString currentcollectdate;

    // 从外部传入的文件名
    string filename;
    
    /**
     * 单隧道综合限界模型，包括三个ClearanceData
     */
    ClearanceSingleTunnel & singleTunnelModel;
    
    bool hasinitheightsval;

    // 画图界面
    ClearanceImage *syntunnelcorrect;
    QScrollArea *scrollArea;

    int scale;//比例
    double pixeltomm;//像素与毫米的转化
    int widthpixel;//像素宽
    int originy;//坐标基准y

    int szcount;
    int testszcount;
    int floornumber;

    int correct_scale;
    int rowcount,row1count,row2count;

    ShowDataType datatype;

    /**
     * 查找框的边界点
     */
    QPointF TopLeftPoint, BottomRightPoint;

private slots:
  
    /**
     * 重新综合
     */
    void calculateSynthesisData();

    /**
     * 放大缩小按快捷键槽函数
     */
    void KeyZoomIn();
    /**
     * 放大缩小按快捷键槽函数
     */
    void KeyZoomOut();

    // 隧道综合界面的基础信息（显示缩放因子scale、pixel2mm、分中坐标原点x、y）
    void getparameterfromtest(int,double,int,int);
    void getspinBoxvalue(int);
    void correctscale(int);
    void on_finishButton_clicked();
    void turnToEditWidget();

    /**
     * 从我们画的感兴趣区域内查找限界半宽更小的“危险”帧数
     */
    void frameSearchInInterestArea();

    /**
     * 显示高度辅助线
     */
    void showhelpline(bool ishow);

    /**
     * 修改车厢方向
     */
    void carriageDirectionChanged(int);

    // 更新计算进度条
    void initProgressBar(long long, long long);
    // 更新计算进度条
    void updateProgressBar(long long);
    // 更新进度条
    void endProgressBar();
};

#endif // SYNTHESIS_TUNNEL_H
