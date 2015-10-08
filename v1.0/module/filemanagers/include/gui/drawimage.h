#ifndef BaseImage_H
#define BaseImage_H

#include <QDialog>
#include <QMouseEvent>
#include <QPointF>

#include "multiframecorrect.h"
#include "clearance_item.h"
#include "lz_output.h"

#include <opencv2\opencv.hpp>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>

#include <list>

using namespace cv;

enum ShowDataType
{
    AllType = 0,       //所有数据显示
    StraightType = 1,  //直线类型
    LeftType = 2,      //左曲线类型
    RightType = 3      //右曲线类型
};

namespace Ui {
    class BaseImage;
}

class BaseImage : public QDialog
{
    Q_OBJECT

public:
    explicit BaseImage(QWidget *parent = 0);
    virtual ~BaseImage();
   
    /**
     * 保存图片，外部调用函数
     */
    void saveImage(QString outputimgfilename);
    
    /**
     * 设置感兴趣高度
     */
    void setInterestHeights(std::list<int> newheights);

    /**
     * 是否显示高度辅助线
     */
    void setShowheightline(bool ifshow);

    /**
     * 是否显示数据点
     */
    void setPointsArrayVisible(bool ifshow);

    /**
     * 是否显示融合数据点
     */
    void setFusePointArrayVisible(bool ifshow);

    /**
     * 是否显示底板
     */
    void setFloorVisible(bool ifshow);

protected:
    /**
     * 纯虚函数，重写绘图事件
     */
    virtual void paintEvent(QPaintEvent *) = 0;

    // 电力，双层集装箱
    virtual void drawFloor_D_DianLi(QPainter & pp);
    // 内燃，双层集装箱
    virtual void drawFloor_D_NeiRan(QPainter & pp);
    // 电力，基础
    virtual void drawFloor_B_DianLi(QPainter & pp);
    // 内燃，基础
    virtual void drawFloor_B_NeiRan(QPainter & pp);

    /**
     * 画底板
     */
    virtual void Draw_Floor(QPainter & pp);

    /**
     * 画高度辅助线
     */
    virtual void drawHeightsLine(QPainter & pp);

    /**
     * 保存图片
     */
    virtual void saveImage(QPixmap & pixmap);

    /**
     * 将像素转换为高度与距离
     */
    void ConvertPixeltoMM();

    /**
     * 画查找框
     */
    void Draw_SearchRect(QPainter & pp);

public slots:
    /**
     * 更新scale相应的参数
     */
    virtual void UpdateScaleRelativeParameter(int newscale);
    /**
     * 获得底板的值
     */
    virtual void setfloornumber(OutputClearanceImageType);
    /**
     * 得到显示图像类型
     * @param 参数为 0, 全部显示 1, 只显示直线 2, 只显示左转曲线 3, 只显示右转曲线
     */
    virtual void setImageType(ShowDataType);

protected:
    Ui::BaseImage *ui;
    /**
     * 实际与图像的比例
     */
    double scale;       

    static double pixeltomm;   // 像素与毫米的转化

    double widthpixel;  // 图像宽
    double heightpixel; // 图像高

    /**
     * 分中坐标系原点
     */
    double originx;     // 分中坐标基准x
    double originy;     // 分中坐标基准y
    /**
     * 接触网结构高度???
     */
    double h;

    /**
     * 哪种底板
     */
    OutputClearanceImageType floornumber;    
    /**
     * 是否画查找框
     */
    bool  isDrawRect;
    /**
     * 查看图像类型
     */
    int imagetype;
    bool  lineimage,leftimage,rightimage;

    /**
     * 保存图片文件路径名
     */
    QString saveimgpathfilename;
    bool cansaveimg;

    /**
     * 是否显示高度辅助线
     */
    bool isshowheightline;

    /**
     * 是否显示数据点
     */
    bool ifshowpointsarray;
    bool ifshowfusepointarray;

    /**
     * 是否显示底板
     */
    bool ifshowfloor;

    /**
     * 感兴趣高度
     */
    std::list<int> interestHeights; 

    /**
     * 查找矩形框修正界面的参数
     */
    QPointF TopLeftPoint, BottomRightPoint;

    /**
     * 记录查找框的起点和终点
     */
	QPointF lastRectPoint, endRectPoint;
};

class ClearanceImage : public BaseImage//单隧道综合选择需要修正的图形
{
	Q_OBJECT

public:
    explicit ClearanceImage(QWidget *parent = 0);
    virtual ~ClearanceImage();

    /**
     * 在ClearanceImage中直线的给2个PointsArray数组赋值
     * @param data 限界数据类型
     * @param type 曲线类型（直线，左转曲线，右转曲线）
     */
    bool ClearanceDataToPointsArray(ClearanceData& data, CurveType type);

    /**
     * 初始化直线数组
     * @param arraysize 数组长度
     * @param type 曲线类型（直线，左转曲线，右转曲线）
     */
    void initPointsArray(int arraysize, CurveType type);

    
    /**
     * 初始化所有左右直线数据
     */
    void clearPointsArrayAll();

protected:
    
    /**
     * 虚函数，重写绘图事件
     */
    virtual void paintEvent(QPaintEvent *);
    
    /**
     * 画一组点数组的左右侧数据
     * @param type 曲线类型（直线，左转曲线，右转曲线）
     */
    void Draw_PointsArray(QPainter & pp, CurveType type);

    /**
     * 画查找框，鼠标按下事件
     */
	void mousePressEvent(QMouseEvent *);
    /**
     * 画查找框，鼠标移动事件
     */
	void mouseMoveEvent(QMouseEvent *);
    /**
     * 画查找框，鼠标释放事件
     */
	void mouseReleaseEvent(QMouseEvent *event);
    
    /**
     * 画高度辅助线
     */
    virtual void drawHeightsLine(QPainter & pp);

public:
    /**
     * 直线数据个数
     */
    int rowcount;
    /**
     * 左曲线数据个数
     */
    int row1count;
    /**
     * 右曲线数据个数
     */
    int row2count;
    /**
     * 是否是图表输出界面
     */
    bool isoutputcorrect;
    /**
     * 以下6个变量存储具体数据
     */
    QPointF *pointintstwo_rowcount1, *pointintstwo_rowcount2;
    QPointF *pointintstwo_rowcount3, *pointintstwo_rowcount4;
    QPointF *pointintstwo_rowcount5, *pointintstwo_rowcount6;
    /**
     * 初始化直线高度数据
     */
    bool hasstraightinit;
    /**
     * 最低净高直线高度数据
     */
    float minHeight_straight;
    /**
     * 是否有最低净高数据
     */
    bool hasMinHeight_straight;

    /**
     * 初始化左曲线高度数据
     */
    bool hasleftinit;
    /**
     * 最低净高直线高度数据
     */
    float minHeight_left;
    /**
     * 是否有最低净高数据
     */
    bool hasMinHeight_left;

    /**
     * 初始化右曲线高度数据
     */
    bool hasrightinit;
    /**
     * 最低净高直线高度数据
     */
    float minHeight_right;
    /**
     * 是否有最低净高数据
     */
    bool hasMinHeight_right;

signals:
    /**
     * 发送查找框的TopLeft_BottomRight坐标到修正界面
     */
    void sendInterestRectangle(float topleft_x, float topleft_y, float bottomright_x, float bottomright_y);

    /**
     * 将rowcount,row1count,row2count发送到单隧道综合界面，判断是否有各种曲线
     */
    void LineTypeCount(int, int, int);

    /**
     * 传递实时鼠标坐标像素值到修正图像界面显示
     * @param int,int 分中坐标系下坐标（单位：毫米）
     */
    void sendMousePos(int,int);

private:
   
    /**
     * 画布变量
     */
	QPixmap pix; 
};

/**
 * 图形修正界面使用的画图类
 */
class SectionImage : public BaseImage
{
    Q_OBJECT
public:
    explicit SectionImage(QWidget *parent = 0);
    virtual ~SectionImage();

    /**
     * 设置该界面是否显示综合结果提高度数据
     */
    void setShowSynHeightsResultData(bool ifshow);

    /**
     * 该界面是否能够进行图形修正
     */
    void CorrectorNot(bool);

    /**
     * 画出感兴趣区域，矩形
     * @param newx1 newy1 newx2 newy2 前四个int型为坐上右下的矩形坐标
     * @param neworiginy 最后一个为原点坐标？
     */
    void setInterestRectangle(float topleft_x, float topleft_y, float bottomright_x, float bottomright_y, int neworiginy);

    /**
     * SectionData数组在SectionImage中给2个PointsArray数组赋值
     * @param data 自定义数据结构-断面限界数据，一个map
     */
    bool SectionDataToPointsArray(SectionData & data, bool iscompared = false);

    /**
     * 初始化数组
     */
    void initPointsArray(int arraysize, bool iscompared = false);

    /**
     * Mat矩阵给pointsFuseResult数组赋值
     * @param fuse_pnts Mat矩阵
     * @param isFuse 矩阵中存储的数据是float型还是double型。mdat文件是float型，fdat文件时double型
     * @param isQR 是不是QR。格式不一样
     * @param isnotswapleftright 是否互换左右坐标（x轴乘以-1），车厢正向时，不做翻转（ifnotswapleftright == true）
     */
    bool MatDataToAccuratePointsArray(Mat & fuse_pnts, bool isFuse, bool isQR, bool ifnotswapleftright);

    /**
     * 是否显示最小高度
     */
    void setShowMinHeight(bool ifshow);

private:
    /**
     * 虚函数，重写绘图事件
     */
    virtual void paintEvent(QPaintEvent *);

    /**
     * 画车厢，验证车底高差分中测量得到的RT
     */
    virtual void Draw_Coach(QPainter & pp);

    /**
     * 画直线的左右侧数据
     */
    void Draw_Straight(QPainter & pp, bool iscompared = false);
    
    /**
     * 画融合测量限界点
     */
    void Draw_Fuse(QPainter &pp);

    /**
     * 图形修正，鼠标按下事件
     */
    void mousePressEvent(QMouseEvent *);
    /**
     * 图形修正，鼠标移动事件
     */
    void mouseMoveEvent(QMouseEvent *);
    /**
     * 图形修正，鼠标释放事件
     */
	void mouseReleaseEvent(QMouseEvent *);
    /**
     * 点击鼠标右键，弹出菜单
     */
    void contextMenuEvent(QContextMenuEvent *event);
    /**
     * 图形修正鼠标点击之后，找到感兴趣点（修正点）
     */
    void findCorrectPoint();
    /**
     * 图形修正鼠标移动时，更新感兴趣点位置画虚线（修正点）
     * 图像修正鼠标释放时，更新感兴趣点位置，画实现
     * @param ismouserelease 鼠标移动或释放
     */
    void updateCorrectPoint(bool ismouserelease);

    /**
     * 画高度辅助线
     */
    virtual void drawHeightsLine(QPainter & pp);
    /**
     * 画正在移动点，鼠标拖动时的三点虚线
     */
    void drawMovingInterestPointsLine(QPainter & pp);

private slots:
    /**
     * 点击右键，多帧修正的槽
     */
    void multiframecorrect();

signals:
    /**
     * 传递起点坐标值到修正界面
     * @param int,int 分中坐标系下坐标（单位：毫米）
     */
    void sendCorrectPos(int,int);

    /**
     * 传递修正之后左侧右侧的值到修正界面
     * @param x 转到分中下的X
     * @param y 转到分中下的Y, 高度
     * @param isleft true 是左侧数据
     * @param isupdateval true 更新到sectiondata中
     */
    void sendUpdateSectionDataVal(int x, int y, bool isleft, bool isupdateval);

    /**
     * 当widget大小改变时，告知外层界面容器，更新scrollbar信号槽
     */
    void resizeWidget();
    
    /**
     * 传递实时鼠标坐标像素值到修正图像界面显示
     * @param int,int 分中坐标系下坐标（单位：毫米）
     */
    void sendMousePos(int,int);
	
public:

    bool isoutputcorrect;
    
    /**
     * 提高度数据是否加载
     */
    bool hasinit;
    /**
     * 提高度数据点个数
     */
    int rowcount;
    /**
     * 中心净高
     */
    float minHeight;
    bool hasMinHeightVal;

    // 是否显示最小高度
    bool ifShowMinHeight;

    /**
     * 限界点数组（分中坐标系下左侧）
     */
    QPointF *pointstwo_rowcountStraight1;
    /**
     * 限界点数组（分中坐标系下右侧）
     */
    QPointF *pointstwo_rowcountStraight2;

    /**
     * 实际点数组
     */
    vector<QPointF *>pointsFuseResult;
    bool hasinitfuse;
    vector<int> fuse_count;
    vector<int> fuse_points_cameragroup;

    /**
     * 对比数据是否加载
     */
    bool compared_hasinit;
    /**
     * 对比数据点个数
     */
    int compared_rowcount;
    /**
     * 对比中心净高
     */
    float compared_minHeight;
    bool compared_hasMinHeightVal;
    /**
     * 对比（断面仪）限界点数组（分中坐标系下左侧）
     */
    QPointF *compared_rowcountStraight1;
    /**
     *对比（断面仪） 限界点数组（分中坐标系下右侧）
     */
    QPointF *compared_rowcountStraight2;

    /**
     * 单隧道综合界面是否画了查找框
     */
    bool ishaveRect;

private:
    QPointF  lastPoint, endPoint;
    MultiframeCorrect *multilframe;

    // 是否在鼠标拖动时找到correctPoint
    bool isFindCorrectPoint;
    // correctPoint_1和correctPoint_2是correctPoint的前后两点
    // 注：分中坐标系下的值，rx为负代表分中左侧，为正代表分中右侧
    QPointF correctPoint_1_FZ, correctPoint_FZ, correctPoint_2_FZ;

    /**
     * 是否显示提高度综合数据
     */
    bool ifshowsyndata;
};

#endif // BaseImage_H
