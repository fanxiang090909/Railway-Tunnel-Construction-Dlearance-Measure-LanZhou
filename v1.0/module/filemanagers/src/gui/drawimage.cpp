#include "drawImage.h"
#include "ui_drawImage.h"
#include <QPainter>
#include <QRect>
#include <QPointF>
#include <QLineF>
#include <QMenu>
#include <QPixmap>
#include <QDebug>
#include <QMessageBox>

double BaseImage::pixeltomm = 0.35;

BaseImage::BaseImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BaseImage)
{
    ui->setupUi(this);

    // 默认底板
    floornumber = OutType_B_DianLi;

    // 初始不保存图片
    cansaveimg = false;
    saveimgpathfilename = "";

    // 初始不显示高度辅助线底板
    isshowheightline = false;
    // 初始显示底板
    ifshowfloor = true;

    TopLeftPoint = QPointF(0,0);
    BottomRightPoint = QPointF(0,0);
}

BaseImage::~BaseImage()
{
    delete ui;
}

/**
 * 保存图片，外部调用函数
 */
void BaseImage::saveImage(QString outputimgfilename)
{
    saveimgpathfilename = outputimgfilename;

    // 记录当前显示参数
    bool tmpishowheightline = isshowheightline;
    int tmpnewscale = scale;
    bool tmpisDrawRect = isDrawRect;
    
    // 图片更新
    scale = 34;
    isDrawRect = false;
    isshowheightline = false;
    repaint();

    // 存储图片
    cansaveimg = true;
    repaint();
    cansaveimg = false;

    // 恢复原显示参数
    isshowheightline = tmpishowheightline;
    scale = tmpnewscale;
    isDrawRect = tmpisDrawRect;
    repaint();
}

void BaseImage::drawFloor_D_DianLi(QPainter & pp)
{
    QBrush bruch(Qt::FDiagPattern);//画刷
    h=0;//暂时默认接触网高度为0
    originy=((80+(6860+h)/scale))/pixeltomm;
    // 画水平的基准线
    pp.setPen(QPen(Qt::gray,1));//两个参数为画笔的颜色和粗细
    bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
    pp.setBrush(bruch);
    QPointF pointsthree[3]={QPointF(100.0,originy),QPointF(widthpixel/2,originy),QPointF((widthpixel-100),originy)};
    pp.drawPolyline(pointsthree,3);

    // 画一个小三角形，即多边形
    pp.setPen(QPen(Qt::gray,1));
    QPolygonF polygon;
    polygon<<QPointF((widthpixel-110),(originy-7))<<QPointF((widthpixel-105),originy)<<QPointF((widthpixel-100),(originy-7));
    bruch.setStyle(Qt::CrossPattern);
    bruch.setStyle(Qt::NoBrush);
    pp.setBrush(bruch);
    pp.drawPolygon(polygon,Qt::WindingFill);
    pp.setPen(QPen(Qt::black,1));//两个参数为画笔的颜色和粗细
    pp.drawText((widthpixel-100),originy,tr("轨面"));

    // 画垂直的基准线
    pp.setPen(QPen(Qt::gray,1));//两个参数为画笔的颜色和粗细
    QPointF pointstwo[2]={QPointF(widthpixel/2,originy),QPointF(widthpixel/2,(80/pixeltomm))};
    pp.drawPolyline(pointstwo,2);

    if (!ifshowfloor)
        return;

    // 标准限界实线部分
    pp.setPen(QPen(Qt::gray,1));//两个参数为画笔的颜色和粗细
    QPointF pointsthree1[3]={QPointF((widthpixel/2)-((1680/scale)/pixeltomm),originy),QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((1210/scale)/pixeltomm)),QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((4220/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree1,3);
    QPointF pointsthree2[3]={QPointF((widthpixel/2)+((1680/scale)/pixeltomm),originy),QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((1210/scale)/pixeltomm)),QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((4220/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree2,3);
    //绘制圆弧
    // pp.drawEllipse((widthpixel/2)-((2440/scale)/pixeltomm),(80/pixeltomm),(((2440*2)/scale)/pixeltomm),(((2440*2)/scale)/pixeltomm));
    pp.drawArc((widthpixel/2)-((2440/scale)/pixeltomm),((80/pixeltomm)+(200/scale)/pixeltomm),(((2440*2)/scale)/pixeltomm),(((2440*2)/scale)/pixeltomm),0*16,180*16);

    //标准限界虚线部分
    pp.setPen(QPen(Qt::gray,1,Qt::DashDotDotLine,Qt::RoundCap));
    QPointF pointsthree3[8]={QPointF((widthpixel/2),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)-((1400/scale)/pixeltomm),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)-((1500/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)-((1725/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)-((1725/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)-((1875/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)-((1875/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)-((2370/scale)/pixeltomm),originy-((1100/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree3,8);
    QPointF pointsthree4[8]={QPointF((widthpixel/2),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)+((1400/scale)/pixeltomm),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)+((1500/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)+((1725/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)+((1725/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)+((1875/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)+((1875/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)+((2370/scale)/pixeltomm),originy-((1100/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree4,8);
    QPointF pointsthree5[4]={QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((3000/scale)/pixeltomm)),QPointF((widthpixel/2)-((2000/scale)/pixeltomm),originy-((4500/scale)/pixeltomm)),QPointF((widthpixel/2)-((1760/scale)/pixeltomm),originy-((4900/scale)/pixeltomm)),QPointF((widthpixel/2)-((1760/scale)/pixeltomm),originy-((6500/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree5,4);
    QPointF pointsthree6[4]={QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((3000/scale)/pixeltomm)),QPointF((widthpixel/2)+((2000/scale)/pixeltomm),originy-((4500/scale)/pixeltomm)),QPointF((widthpixel/2)+((1760/scale)/pixeltomm),originy-((4900/scale)/pixeltomm)),QPointF((widthpixel/2)+((1760/scale)/pixeltomm),originy-((6500/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree6,4);
    QPointF pointsthree7[2]={QPointF((widthpixel/2)-((1760/scale)/pixeltomm),originy-((5300/scale)/pixeltomm)),QPointF((widthpixel/2),originy-((5300/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree7,2);
    QPointF pointsthree8[2]={QPointF((widthpixel/2)+((1760/scale)/pixeltomm),originy-((5300/scale)/pixeltomm)),QPointF((widthpixel/2),originy-((5300/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree8,2);
    // 绘制圆弧
    pp.drawArc((widthpixel/2)-((3200/scale)/pixeltomm),(80/pixeltomm),(((3200*2)/scale)/pixeltomm),(((2300*2)/scale)/pixeltomm),57*16,66*16);
}

void BaseImage::drawFloor_D_NeiRan(QPainter & pp)
{
    originy=((80+(6700)/scale))/pixeltomm;
    // 画水平的基准线
           
    pp.setPen(QPen(Qt::gray,1));//两个参数为画笔的颜色和粗细
    QBrush bruch(Qt::FDiagPattern);//画刷
    bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
    pp.setBrush(bruch);
    QPointF pointsthree[3]={QPointF(100.0,originy),QPointF(widthpixel/2,originy),QPointF((widthpixel-100),originy)};
    pp.drawPolyline(pointsthree,3);

    // 画一个小三角形，即多边形
    pp.setPen(QPen(Qt::gray,1));
    QPolygonF polygon;
    polygon<<QPointF((widthpixel-110),(originy-7))<<QPointF((widthpixel-105),originy)<<QPointF((widthpixel-100),(originy-7));
    bruch.setStyle(Qt::CrossPattern);
    bruch.setStyle(Qt::NoBrush);
    pp.setBrush(bruch);
    pp.drawPolygon(polygon,Qt::WindingFill);
    pp.setPen(QPen(Qt::black,3));//两个参数为画笔的颜色和粗细
    pp.drawText((widthpixel-100),originy,tr("轨面"));

    // 画垂直的基准线
    pp.setPen(QPen(Qt::gray,1));//两个参数为画笔的颜色和粗细
    QPointF pointstwo[2]={QPointF(widthpixel/2,originy),QPointF(widthpixel/2,(80/pixeltomm))};
    pp.drawPolyline(pointstwo,2);

    if (!ifshowfloor)
        return;

    //标准限界实线部分
    pp.setPen(QPen(Qt::gray,1));//两个参数为画笔的颜色和粗细
    QPointF pointsthree1[3]={QPointF((widthpixel/2)-((1680/scale)/pixeltomm),originy),QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((1210/scale)/pixeltomm)),QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((4500/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree1,3);
    QPointF pointsthree2[3]={QPointF((widthpixel/2)+((1680/scale)/pixeltomm),originy),QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((1210/scale)/pixeltomm)),QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((4500/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree2,3);
    //绘制圆弧
    //pp.drawEllipse((widthpixel/2)-((2440/scale)/pixeltomm),(80/pixeltomm),(((2440*2)/scale)/pixeltomm),(((2440*2)/scale)/pixeltomm));
    pp.drawArc((widthpixel/2)-((2440/scale)/pixeltomm),(80/pixeltomm),(((2440*2)/scale)/pixeltomm),(((2440*2)/scale)/pixeltomm),0*16,180*16);

    //标准限界虚线部分
    pp.setPen(QPen(Qt::gray,1,Qt::DashDotDotLine,Qt::RoundCap));
    QPointF pointsthree3[8]={QPointF((widthpixel/2),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)-((1400/scale)/pixeltomm),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)-((1500/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)-((1725/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)-((1725/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)-((1875/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)-((1875/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)-((2370/scale)/pixeltomm),originy-((1100/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree3,8);
    QPointF pointsthree4[8]={QPointF((widthpixel/2),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)+((1400/scale)/pixeltomm),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)+((1500/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)+((1725/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)+((1725/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)+((1875/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)+((1875/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)+((2370/scale)/pixeltomm),originy-((1100/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree4,8);
    QPointF pointsthree5[4]={QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((3000/scale)/pixeltomm)),QPointF((widthpixel/2)-((2000/scale)/pixeltomm),originy-((4500/scale)/pixeltomm)),QPointF((widthpixel/2)-((1650/scale)/pixeltomm),originy-((6050/scale)/pixeltomm)),QPointF((widthpixel/2),originy-((6050/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree5,4);
    QPointF pointsthree6[4]={QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((3000/scale)/pixeltomm)),QPointF((widthpixel/2)+((2000/scale)/pixeltomm),originy-((4500/scale)/pixeltomm)),QPointF((widthpixel/2)+((1650/scale)/pixeltomm),originy-((6050/scale)/pixeltomm)),QPointF((widthpixel/2),originy-((6050/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree6,4);
}

void BaseImage::drawFloor_B_DianLi(QPainter & pp)
{
    originy=((80+(6550)/scale))/pixeltomm;
    // 画水平的基准线
    pp.setPen(QPen(Qt::gray,1));//两个参数为画笔的颜色和粗细
    QBrush bruch(Qt::FDiagPattern);//画刷
    bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
    pp.setBrush(bruch);
    QPointF pointsthree[3]={QPointF(100.0,originy),QPointF(widthpixel/2,originy),QPointF((widthpixel-100),originy)};
    pp.drawPolyline(pointsthree,3);

    // 画一个小三角形，即多边形
    pp.setPen(QPen(Qt::gray,1));
    QPolygonF polygon;
    polygon<<QPointF((widthpixel-110),(originy-7))<<QPointF((widthpixel-105),originy)<<QPointF((widthpixel-100),(originy-7));
    bruch.setStyle(Qt::CrossPattern);
    bruch.setStyle(Qt::NoBrush);
    pp.setBrush(bruch);
    pp.drawPolygon(polygon,Qt::WindingFill);
    pp.setPen(QPen(Qt::black,3));//两个参数为画笔的颜色和粗细
    pp.drawText((widthpixel-100),originy,tr("轨面"));

    // 画垂直的基准线
    pp.setPen(QPen(Qt::gray,1));//两个参数为画笔的颜色和粗细
    QPointF pointstwo[2]={QPointF(widthpixel/2,originy),QPointF(widthpixel/2,(80/pixeltomm))};
    pp.drawPolyline(pointstwo,2);

    if (!ifshowfloor)
        return;

    //标准限界实线部分
    pp.setPen(QPen(Qt::gray,1));//两个参数为画笔的颜色和粗细
    QPointF pointsthree1[5]={QPointF((widthpixel/2)-((2250/scale)/pixeltomm),originy),QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((4000/scale)/pixeltomm)),QPointF((widthpixel/2)-((2400/scale)/pixeltomm),originy-((4550/scale)/pixeltomm)),QPointF((widthpixel/2)-((1700/scale)/pixeltomm),originy-((5800/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree1,5);
    QPointF pointsthree2[5]={QPointF((widthpixel/2)+((2250/scale)/pixeltomm),originy),QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((4000/scale)/pixeltomm)),QPointF((widthpixel/2)+((2400/scale)/pixeltomm),originy-((4550/scale)/pixeltomm)),QPointF((widthpixel/2)+((1700/scale)/pixeltomm),originy-((5800/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree2,5);
    //绘制圆弧
    //pp.drawEllipse((widthpixel/2)-((2300/scale)/pixeltomm),(80/pixeltomm),(((2300*2)/scale)/pixeltomm),(((2300*2)/scale)/pixeltomm));
    pp.drawArc((widthpixel/2)-((2300/scale)/pixeltomm),(80/pixeltomm),(((2300*2)/scale)/pixeltomm),(((2300*2)/scale)/pixeltomm),42.3*16,95.2*16);


    //标准限界虚线部分
    pp.setPen(QPen(Qt::gray,1,Qt::DashDotDotLine,Qt::RoundCap));
    QPointF pointsthree3[9]={QPointF((widthpixel/2),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)-((1400/scale)/pixeltomm),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)-((1500/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)-((1725/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)-((1750/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)-((1875/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)-((1875/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)-((2370/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((1250/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree3,9);
    QPointF pointsthree4[9]={QPointF((widthpixel/2),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)+((1400/scale)/pixeltomm),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)+((1500/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)+((1725/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)+((1750/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)+((1875/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)+((1875/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)+((2370/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((1250/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree4,9);
    QPointF pointsthree5[4]={QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((3000/scale)/pixeltomm)),QPointF((widthpixel/2)-((2000/scale)/pixeltomm),originy-((4500/scale)/pixeltomm)),QPointF((widthpixel/2)-((1400/scale)/pixeltomm),originy-((5500/scale)/pixeltomm)),QPointF((widthpixel/2),originy-((5500/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree5,4);
    QPointF pointsthree6[4]={QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((3000/scale)/pixeltomm)),QPointF((widthpixel/2)+((2000/scale)/pixeltomm),originy-((4500/scale)/pixeltomm)),QPointF((widthpixel/2)+((1400/scale)/pixeltomm),originy-((5500/scale)/pixeltomm)),QPointF((widthpixel/2),originy-((5500/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree6,4);
}

void BaseImage::drawFloor_B_NeiRan(QPainter & pp)
{
    originy=((80+(6000)/scale))/pixeltomm;
    // 画水平的基准线
    pp.setPen(QPen(Qt::gray,1));//两个参数为画笔的颜色和粗细
    QBrush bruch(Qt::FDiagPattern);//画刷
    bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
    pp.setBrush(bruch);
    QPointF pointsthree[3]={QPointF(100.0,originy),QPointF(widthpixel/2,originy),QPointF((widthpixel-100),originy)};
    pp.drawPolyline(pointsthree,3);

    // 画一个小三角形，即多边形
    pp.setPen(QPen(Qt::gray,1));
    QPolygonF polygon;
    polygon<<QPointF((widthpixel-110),(originy-7))<<QPointF((widthpixel-105),originy)<<QPointF((widthpixel-100),(originy-7));
    bruch.setStyle(Qt::CrossPattern);
    bruch.setStyle(Qt::NoBrush);
    pp.setBrush(bruch);
    pp.drawPolygon(polygon,Qt::WindingFill);
    pp.setPen(QPen(Qt::black,3));//两个参数为画笔的颜色和粗细
    pp.drawText((widthpixel-100),originy,tr("轨面"));
       
    // 画垂直的基准线
    pp.setPen(QPen(Qt::gray,1));//两个参数为画笔的颜色和粗细
    QPointF pointstwo[2]={QPointF(widthpixel/2,originy),QPointF(widthpixel/2,(80/pixeltomm))};
    pp.drawPolyline(pointstwo,2);

    if (!ifshowfloor)
        return;

    //标准限界实线部分
    pp.setPen(QPen(Qt::gray,1));//两个参数为画笔的颜色和粗细
    QPointF pointsthree1[3]={QPointF((widthpixel/2)-((2250/scale)/pixeltomm),originy),QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((3900/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree1,3);
    QPointF pointsthree2[3]={QPointF((widthpixel/2)+((2250/scale)/pixeltomm),originy),QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((3900/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree2,3);
    //绘制圆弧
    //pp.drawEllipse((widthpixel/2)-((2440/scale)/pixeltomm),(80/pixeltomm),(((2440*2)/scale)/pixeltomm),(((2440*2)/scale)/pixeltomm));
    pp.drawArc((widthpixel/2)-((2440/scale)/pixeltomm),(80/pixeltomm),(((2440*2)/scale)/pixeltomm),(((2440*2)/scale)/pixeltomm),0*16,180*16);

    //标准限界虚线部分
    pp.setPen(QPen(Qt::gray,1,Qt::DashDotDotLine,Qt::RoundCap));
    QPointF pointsthree3[9]={QPointF((widthpixel/2),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)-((1400/scale)/pixeltomm),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)-((1500/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)-((1725/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)-((1750/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)-((1875/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)-((1875/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)-((2370/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((1250/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree3,9);
    QPointF pointsthree4[9]={QPointF((widthpixel/2),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)+((1400/scale)/pixeltomm),originy-((25/scale)/pixeltomm)),QPointF((widthpixel/2)+((1500/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)+((1725/scale)/pixeltomm),originy-((200/scale)/pixeltomm)),QPointF((widthpixel/2)+((1750/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)+((1875/scale)/pixeltomm),originy-((350/scale)/pixeltomm)),QPointF((widthpixel/2)+((1875/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)+((2370/scale)/pixeltomm),originy-((1100/scale)/pixeltomm)),QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((1250/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree4,9);
    QPointF pointsthree5[4]={QPointF((widthpixel/2)-((2440/scale)/pixeltomm),originy-((3000/scale)/pixeltomm)),QPointF((widthpixel/2)-((2000/scale)/pixeltomm),originy-((4500/scale)/pixeltomm)),QPointF((widthpixel/2)-((1400/scale)/pixeltomm),originy-((5500/scale)/pixeltomm)),QPointF((widthpixel/2),originy-((5500/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree5,4);
    QPointF pointsthree6[4]={QPointF((widthpixel/2)+((2440/scale)/pixeltomm),originy-((3000/scale)/pixeltomm)),QPointF((widthpixel/2)+((2000/scale)/pixeltomm),originy-((4500/scale)/pixeltomm)),QPointF((widthpixel/2)+((1400/scale)/pixeltomm),originy-((5500/scale)/pixeltomm)),QPointF((widthpixel/2),originy-((5500/scale)/pixeltomm))};
    pp.drawPolyline(pointsthree6,4);
}

void BaseImage::Draw_Floor(QPainter & pp)
{
    switch(floornumber)
    {
        case OutType_D_DianLi:
        {
            drawFloor_D_DianLi(pp);
            break;
        }
        case OutType_D_NeiRan:
        {
            drawFloor_D_NeiRan(pp);
            break;
        }
        case OutType_B_DianLi:
        {
            drawFloor_B_DianLi(pp);
            break;
        }
        case OutType_B_NeiRan:
        {
            drawFloor_B_NeiRan(pp);
            break;
        }
    }
}

/**
 * 画高度辅助线
 */
void BaseImage::drawHeightsLine(QPainter & pp)
{
    if (isshowheightline)
    {
        pp.setPen(QPen(Qt::black,1));//两个参数为画笔的颜色和粗细
        QBrush bruch(Qt::FDiagPattern);//画刷
        bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
        pp.setBrush(bruch);
        // 画数字的标记线
        int count = interestHeights.size();
        QLineF *linefheight =new QLineF[count];
        int line_i = 0;
	    for (list<int>::iterator it = interestHeights.begin(); it != interestHeights.end(); it++, line_i++)                      //对List进行遍历获取点
        {
            qreal ih = (*it);
            QPointF pointsthreeheightline[3]={QPointF(0.0,(originy-((ih)/scale)/pixeltomm)),QPointF(widthpixel/2,(originy-((ih/scale)/pixeltomm))),QPointF((widthpixel),(originy-((ih/scale)/pixeltomm)))};
            pp.drawPolyline(pointsthreeheightline,3);
          
            linefheight[line_i].setP1(QPointF((widthpixel/2),(originy-((ih/scale)/pixeltomm))));
            linefheight[line_i].setP2(QPointF(((widthpixel/2)-5),(originy-((ih/scale)/pixeltomm))));
            pp.drawText((widthpixel/2),(originy-(ih/scale)/pixeltomm),QString("%1").arg(ih));//标记时为了使数字在横线下方可以改为810
            // 如果图形面板太小，隔一个点画一个
            if (30 < scale)
            {
                line_i = line_i+1;
                it++;
                if (it == interestHeights.end())
                    break;
            }
            if (60 < scale)
            {
                line_i = line_i+1;
                it++;
                if (it == interestHeights.end())
                    break;
            }
        }
        pp.drawLines(linefheight,count);
    }
}

void BaseImage::Draw_SearchRect(QPainter & pp)
{
    pp.setPen(QPen(Qt::black,1.5));

    QPointF TopLeftPointImg, BottomRightPointImg;
    TopLeftPointImg.setX((TopLeftPoint.x()/pixeltomm/scale) + widthpixel / 2); 
    TopLeftPointImg.setY(originy - (TopLeftPoint.y()/pixeltomm/scale)); 
    BottomRightPointImg.setX((BottomRightPoint.x()/pixeltomm/scale) + widthpixel / 2); 
    BottomRightPointImg.setY(originy - (BottomRightPoint.y()/pixeltomm/scale)); 

    pp.drawRect(TopLeftPointImg.x(), TopLeftPointImg.y(), BottomRightPointImg.x() - TopLeftPointImg.x(), BottomRightPointImg.y() - TopLeftPointImg.y());
}

void BaseImage::ConvertPixeltoMM()
{
    if (lastRectPoint.x() < widthpixel / 2)
    {
        if (endRectPoint.x() >= widthpixel / 2)
            endRectPoint.setX(widthpixel /2);
    }
    else
    {
        if (endRectPoint.x() <= widthpixel / 2)
            endRectPoint.setX(widthpixel /2);
    }
    
    if (lastRectPoint.x() < endRectPoint.x())
    {
        TopLeftPoint.setX(lastRectPoint.x());
        BottomRightPoint.setX(endRectPoint.x());
    }
    else
    {
        TopLeftPoint.setX(endRectPoint.x());
        BottomRightPoint.setX(lastRectPoint.x());
    }

    if (lastRectPoint.y() < endRectPoint.y())
    {
        TopLeftPoint.setY(lastRectPoint.y());
        BottomRightPoint.setY(endRectPoint.y());
    }
    else
    {
        TopLeftPoint.setY(endRectPoint.y());
        BottomRightPoint.setY(lastRectPoint.y());
    }


    TopLeftPoint.setX((TopLeftPoint.x() - widthpixel / 2)*pixeltomm*scale); 
    TopLeftPoint.setY((originy - TopLeftPoint.y())*pixeltomm*scale); 
    BottomRightPoint.setX((BottomRightPoint.x() - widthpixel / 2)*pixeltomm*scale); 
    BottomRightPoint.setY((originy - BottomRightPoint.y())*pixeltomm*scale); 

    //qDebug() << "TOPLEFTPOINT" << TopLeftPoint.x() << ", " << TopLeftPoint.y() << "BOTTOMRIGHT" << BottomRightPoint.x() << ", " << BottomRightPoint.y();
}

/**
 * 保存图片
 */
void BaseImage::saveImage(QPixmap & pixmap)
{
	qDebug() << saveimgpathfilename;
    bool ret = pixmap.save(saveimgpathfilename);
}

/**
 * 设置感兴趣高度
 */
void BaseImage::setInterestHeights(std::list<int> newheights)
{
    interestHeights = newheights;
}

void BaseImage::setShowheightline(bool isornot)
{
    isshowheightline = isornot;
}

/**
 * 是否显示数据点
 */
void BaseImage::setPointsArrayVisible(bool ifshow)
{
    ifshowpointsarray = ifshow;
}

void BaseImage::setFusePointArrayVisible(bool ifshow)
{
    ifshowfusepointarray = ifshow;
}

void BaseImage::setFloorVisible(bool ifshow)
{
    ifshowfloor = ifshow;
}

void BaseImage::UpdateScaleRelativeParameter(int newscale)
{
    scale=newscale;
    if (scale < 40)
    {
        widthpixel = ((double)40/(double)scale) * 680;
        heightpixel = ((double)40/(double)scale) * 790;
    }
    else
    {
        widthpixel = 680;
        heightpixel = 790;
    }
    setFixedSize(widthpixel,heightpixel);
    isDrawRect=true;
    this->update();
}

void BaseImage::setfloornumber(OutputClearanceImageType newfloornumber)
{
	this->floornumber = newfloornumber;
    update();
}

ClearanceImage::ClearanceImage(QWidget *parent):
	BaseImage(parent)
{
	scale=40;
    widthpixel=680;
    heightpixel=790;
    setFixedSize(widthpixel,heightpixel);//即是宽297.5毫米，长280毫米
    rowcount=0;
    row1count=0;
    row2count=0;
    isoutputcorrect=true;//是否保存图片
    imagetype=AllType;//默认所有图像数据全部显示,1表示直线，2表示左曲线，3表示右曲线
    lineimage=true;
    leftimage=true;
    rightimage=true;

    isDrawRect=false;
    hasstraightinit = false;
    hasleftinit = false;
    hasrightinit=false;
    pix=QPixmap(widthpixel,heightpixel);

    // 中心净高
    minHeight_straight = -1;
    hasMinHeight_straight = false;
    minHeight_left = -1;
    hasMinHeight_left = false;
    minHeight_right = -1;
    hasMinHeight_right = false;
}

ClearanceImage::~ClearanceImage()
{ 
    if (hasstraightinit)
    {
        delete []pointintstwo_rowcount1;
        delete []pointintstwo_rowcount2;
    }
    if (hasleftinit)
    {
        delete []pointintstwo_rowcount3;
        delete []pointintstwo_rowcount4;
    }
    if (hasrightinit)
    {
        delete []pointintstwo_rowcount5;
        delete []pointintstwo_rowcount6;
    }
}

void BaseImage::setImageType(ShowDataType newimagetype)
{
    this->imagetype=newimagetype;
    if(imagetype==AllType)
    { 
        lineimage=true;
        leftimage=true;
        rightimage=true;
    }
    else if(imagetype==StraightType)
    {
        lineimage=true;
        leftimage=false;
        rightimage=false;
    }
    else if(imagetype==LeftType)
    {
        lineimage=false;
        leftimage=true;
        rightimage=false;
    }
    else
    {
        lineimage=false;
        leftimage=false;
        rightimage=true;
    }
    update();//获得新的图像类型之后，更新图像
}

/**
 * 初始化数组
 */
void ClearanceImage::initPointsArray(int arraysize, CurveType type)
{
    if (arraysize <= 0)
        return;
    switch (type)
    {
        case CurveType::Curve_Straight:
        {
            if (hasstraightinit)
                return;
            pointintstwo_rowcount1=new QPointF[arraysize];//左侧限界线
            pointintstwo_rowcount2=new QPointF[arraysize];//右侧限界线
            hasstraightinit = true;
            break;
        }
        case CurveType::Curve_Left:
        {
            if (hasleftinit)
                return;
            pointintstwo_rowcount3=new QPointF[arraysize];//左侧限界线
            pointintstwo_rowcount4=new QPointF[arraysize];//右侧限界线
            hasleftinit = true;
            break;
        }
        case CurveType::Curve_Right:
        {
            if (hasrightinit)
                return;
            pointintstwo_rowcount5=new QPointF[arraysize];//左侧限界线
            pointintstwo_rowcount6=new QPointF[arraysize];//右侧限界线
            hasrightinit = true;
            break;
        }
    }
}

bool ClearanceImage::ClearanceDataToPointsArray(ClearanceData& data, CurveType type)
{
    switch (type)
    {
        case CurveType::Curve_Straight : 
        {
            if (!hasstraightinit)
            {
                hasMinHeight_straight = false; 
                return false;
            }
            // @author 范翔注释 20150629
            //minHeight_straight = -1;
            minHeight_straight = data.getMinCenterHeight();
            if (minHeight_straight < 0)
            {
                hasstraightinit = false;
                hasMinHeight_straight = false;
                return false;
            }

            if (minHeight_straight < 0)
                hasMinHeight_straight = false;
            else
                hasMinHeight_straight = true;
            break;
        }
        case CurveType::Curve_Left : 
        {
            if (!hasleftinit)
            {
                hasMinHeight_left = false; 
                return false;
            }
            // @author 范翔注释 20150629
            //minHeight_left = -1;
            minHeight_left = data.getMinCenterHeight();
            if (minHeight_left < 0)
            {
                hasleftinit = false;
                hasMinHeight_left = false;
                return false;
            }

            if (minHeight_left <= 0)
                hasMinHeight_left = false;
            else
                hasMinHeight_left = true;
            break;
        }
        case CurveType::Curve_Right :
        {
            if (!hasrightinit)
            {
                hasMinHeight_right = false; 
                return false;
            }
            // @author 范翔注释 20150629
            //minHeight_right = -1;
            minHeight_right = data.getMinCenterHeight();
            if (minHeight_right < 0)
            {
                hasrightinit = false;
                hasMinHeight_right = false;
                return false;
            }
            if (minHeight_right < 0)
                hasMinHeight_right = false;
            else
                hasMinHeight_right = true;
            break;
        }
        default: break;
    }

    int szcount = data.getMaps().size();
    std::map <int,ClearanceItem>::iterator it = data.getMaps().begin();
    int j = 0;

    //data.showMaps();
    switch (type)
    {
        case CurveType::Curve_Straight :
        {

            while(it!=data.getMaps().end())
            {
                std::pair<int,ClearanceItem> pair = (*it);
                pointintstwo_rowcount1[j]=QPoint(pair.first,pair.second.leftval);
                pointintstwo_rowcount2[j]=QPoint(pair.first,pair.second.rightval);
                it++;
                j++;
            }
            this->rowcount = szcount;
            break;
        }
        case CurveType::Curve_Left :
        {
            while(it!=data.getMaps().end())
            {

                std::pair<int,ClearanceItem> pair = (*it);
                pointintstwo_rowcount3[j]=QPoint(pair.first,pair.second.leftval);
                pointintstwo_rowcount4[j]=QPoint(pair.first,pair.second.rightval);
                it++;
                j++;
            }
            this->row1count = szcount;
            break;    
        }
        case CurveType::Curve_Right :
        {
            while(it!=data.getMaps().end())
            {

                std::pair<int,ClearanceItem> pair = (*it);
                pointintstwo_rowcount5[j]=QPoint(pair.first,pair.second.leftval);
                pointintstwo_rowcount6[j]=QPoint(pair.first,pair.second.rightval);
                it++;
                j++;
            }
            this->row2count = szcount;
            break;    
        }
   }
   return true;
}


/**
 * 初始化所有左右直线数据
 */
void ClearanceImage::clearPointsArrayAll()
{
    hasMinHeight_left = false;
    hasMinHeight_right = false;
    hasMinHeight_straight = false;

    hasleftinit = false;
    hasrightinit = false;
    hasstraightinit = false;

}

void ClearanceImage::Draw_PointsArray(QPainter & pp, CurveType type)
{
    QPointF * tmpArray1;
    QPointF * tmpArray2;
    int tmprowcount;
    bool tmpHasMinHeight;
    float tmpMinHeightVal;
    QPen tmppen = QPen(Qt::red,1); // 画笔颜色
    switch (type)
    {
        case Curve_Straight:
            tmpArray1 = pointintstwo_rowcount1;
            tmpArray2 = pointintstwo_rowcount2;
            tmprowcount = rowcount;
            // 画中心净高
            if (hasMinHeight_straight)
            {
                // 大于最高和最低的赋零
                int tempkey;
                for (int i = 0; i < rowcount; i++)
                {
                    tempkey = pointintstwo_rowcount1[i].rx();
                    if (tempkey > minHeight_straight)
                    {
                        tmprowcount--;
                    }
                }
            }
            tmpHasMinHeight = hasMinHeight_straight;
            tmpMinHeightVal = minHeight_straight;
            tmppen = QPen(Qt::red,1);
            break;
        case Curve_Left:
            tmpArray1 = pointintstwo_rowcount3;
            tmpArray2 = pointintstwo_rowcount4;
            tmprowcount = row1count;
            // 画中心净高
            if (hasMinHeight_left)
            {
                // 大于最高和最低的赋零
                int tempkey;
                for (int i = 0; i < row1count; i++)
                {
                    tempkey = pointintstwo_rowcount3[i].rx();
                    if (tempkey > minHeight_left)
                    {
                        tmprowcount--;
                    }
                }
            }
            tmpHasMinHeight = hasMinHeight_left;
            tmpMinHeightVal = minHeight_left;
            tmppen = QPen(Qt::blue,1,Qt::DashLine,Qt::RoundCap);
            break;
        case Curve_Right:
            tmpArray1 = pointintstwo_rowcount5;
            tmpArray2 = pointintstwo_rowcount6;
            tmprowcount = row2count;
            // 画中心净高
            if (hasMinHeight_right)
            {
                // 大于最高和最低的赋零
                int tempkey;
                for (int i = 0; i < row2count; i++)
                {
                    tempkey = pointintstwo_rowcount5[i].rx();
                    if (tempkey > minHeight_right)
                    {
                        tmprowcount--;
                    }
                }
            }
            tmpHasMinHeight = hasMinHeight_right;
            tmpMinHeightVal = minHeight_right;
            tmppen = QPen(Qt::black,1,Qt::DashDotDotLine,Qt::RoundCap);

            break;
        default:return;// 都不是直接退出
    }

    // 画直线数据
    // 画数字的标记线
    QLineF *linef1 = new QLineF[tmprowcount];
    for(int i=0;i<tmprowcount;i=i+3)
    {
        pp.setPen(QPen(Qt::black,1.5));
        linef1[i].setP1(QPointF((widthpixel/2),(originy-((tmpArray1[i].rx()/scale)/pixeltomm))));
        linef1[i].setP2(QPointF(((widthpixel/2)-5),(originy-((tmpArray1[i].rx()/scale)/pixeltomm))));
        pp.setPen(QPen(Qt::black,1.5));//两个参数为画笔的颜色和粗细
        pp.drawText((widthpixel/2),(originy-(tmpArray1[i].rx()/scale)/pixeltomm),QString("%1").arg(tmpArray1[i].rx()));//标记时为了使数字在横线下方可以改为810
    }
    pp.drawLines(linef1,tmprowcount);
    delete [] linef1;
    // 开始画实际的隧道数据,左侧直线
    pp.setPen(tmppen);
    QBrush bruch(Qt::FDiagPattern);//画刷
    bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
    pp.setBrush(bruch);
    QLineF *linef =new QLineF[tmprowcount];
    QLineF *liner =new QLineF[tmprowcount];

    for(int j=0;j<tmprowcount-1;j++)
    {
        //                linef[j].setLine(((widthpixel/2)-((pointstwo_tmprowcount.at(j).y()/scale)/pixeltomm)),(originy-((pointstwo_tmprowcount.at(j).x()/scale)/pixeltomm)),
        //                               ((widthpixel/2)-((pointstwo_tmprowcount.at(j+1).y()/scale)/pixeltomm)),(originy-((pointstwo_tmprowcount.at(j+1).x()/scale)/pixeltomm)));
        if(tmpArray1[j].ry()<0)
        {
            tmpArray1[j].ry()=0;
        }
        if(j==(tmprowcount-2))
        {
            if(tmpArray1[j+1].ry()<0)
            {
                tmpArray1[j+1].ry()=0;
            }
        }

        // 测试校正图形而修改的代码，使用pointstwo_tmprowcount1变量
        linef[j].setLine(((widthpixel/2)-((tmpArray1[j].ry()/scale)/pixeltomm)),(originy-((tmpArray1[j].rx()/scale)/pixeltomm)),
                            ((widthpixel/2)-((tmpArray1[j+1].ry()/scale)/pixeltomm)),(originy-((tmpArray1[j+1].rx()/scale)/pixeltomm)));
        // 或者下面这两行这种方式
        // linef[j].setP1(QPointF(400-(pointstwo_tmprowcount.at(j).y()/10),820-(pointstwo_tmprowcount.at(j).x()/10)));
        // linef[j].setP2(QPointF(400-(pointstwo_tmprowcount.at(j+1).y()/10),820-(pointstwo_tmprowcount.at(j+1).x()/10)));
    }
    pp.drawLines(linef,tmprowcount);
    delete []linef;
    //画右侧的那条直线
    pp.setPen(tmppen);
    bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
    pp.setBrush(bruch);
    for(int j=0;j<tmprowcount-1;j++)
    {
        if(tmpArray2[j].ry()<0)
        {
            tmpArray2[j].ry()=0;
        }
        if(j==(tmprowcount-2))
        {
            if(tmpArray2[j+1].ry()<0)
            {
                tmpArray2[j+1].ry()=0;
            }
        }
        liner[j].setLine(((widthpixel/2)+((tmpArray2[j].ry()/scale)/pixeltomm)),(originy-((tmpArray2[j].rx()/scale)/pixeltomm)),
                                ((widthpixel/2)+((tmpArray2[j+1].ry()/scale)/pixeltomm)),(originy-((tmpArray2[j+1].rx()/scale)/pixeltomm)));
    }
    pp.drawLines(liner,tmprowcount);
    delete []liner;

    // 连接左中右
    if (tmpHasMinHeight)
    {
        QPointF pointtop;
        pointtop.setX(widthpixel/2);
        pointtop.setY(originy - tmpMinHeightVal/scale/pixeltomm);
        QPointF pointtmp1;
        pointtmp1.setX((widthpixel/2) - ((tmpArray1[tmprowcount - 1].ry()/scale)/pixeltomm));
        pointtmp1.setY(originy-((tmpArray1[tmprowcount - 1].rx()/scale)/pixeltomm));
        pp.drawLine(pointtop, pointtmp1);

        QPointF pointtmp2;
        pointtmp2.setX((widthpixel/2) + ((tmpArray2[tmprowcount - 1].ry()/scale)/pixeltomm));
        pointtmp2.setY(originy-((tmpArray2[tmprowcount - 1].rx()/scale)/pixeltomm));
        pp.drawLine(pointtop, pointtmp2);
    }

    // 不画图时界面显示左右限界半宽点，否则出表时防止彩色打印成断线，于是输入半宽值
    if (!cansaveimg)
    {
        //标记那些点,以便于修正
        pp.setPen(QPen(Qt::green, 3));
        bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
        pp.setBrush(bruch);
        QPoint *point =new  QPoint[tmprowcount];
        QPoint *pointr =new QPoint[tmprowcount];
        for(int i=0;i<tmprowcount;i++)
        {
            point[i].setX(((widthpixel/2)-((tmpArray1[i].ry()/scale)/pixeltomm)));
            point[i].setY((originy-((tmpArray1[i].rx()/scale)/pixeltomm)));
            pointr[i].setX(((widthpixel/2)+((tmpArray2[i].ry()/scale)/pixeltomm)));
            pointr[i].setY((originy-((tmpArray2[i].rx()/scale)/pixeltomm)));
        }
        pp.drawPoints(point,tmprowcount);
        pp.drawPoints(pointr,tmprowcount);
        delete []point;
        delete []pointr;
    }

    if (tmpHasMinHeight)
    {
        pp.setPen(QPen(Qt::yellow, 3));
        QPointF pointtop;
        pointtop.setX(widthpixel/2);
        pointtop.setY(originy - tmpMinHeightVal/scale/pixeltomm);
        pp.drawPoint(pointtop);
        // 显示文字
        QColor color(128, 64, 0);
        pp.setPen(QPen(color, 3));
        // -50表示让出距离
        pp.drawText((widthpixel/2)-50,(originy-(tmpMinHeightVal/scale)/pixeltomm),QString("%1").arg((int)tmpMinHeightVal));//标记时为了使数字在横线下方可以改为810
    }
}

/**
 * 是否显示高度辅助线
 */
void ClearanceImage::drawHeightsLine(QPainter & pp)
{
    BaseImage::drawHeightsLine(pp);
}

void ClearanceImage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);//创建QPinter的一个对象,this就表明了是在窗口上进行绘图，所以利用painter进行的绘图都是在窗口上的，painter进行的坐标变化，是变化的窗口的坐标系.
    QPixmap pix(widthpixel,heightpixel);
    pix.fill(Qt::white);//背景填充为白色
    QPainter pp(&pix);//新建QPainter类对象，在pix上进行绘图
    QBrush bruch(Qt::FDiagPattern);//画刷
    // 画底板
    Draw_Floor(pp);

    // 是否显示高度辅助线
    drawHeightsLine(pp);

    if (ifshowpointsarray)
    {

        // 用红粗线画实际的隧道数据
        if(rowcount==0&&row1count==0&&row2count==0)
        {
            painter.drawPixmap(0.0,0.0,pix);
            return;
        }
        else
        {
            if(rowcount!=0&&lineimage)
            {
                Draw_PointsArray(pp, Curve_Straight);
            }
            if(row1count!=0&&leftimage)
            {
                Draw_PointsArray(pp, Curve_Left);
            }
            if(row2count!=0&&rightimage)
            {
                Draw_PointsArray(pp, Curve_Right);
            }  
            emit  LineTypeCount(rowcount,row1count,row2count);
        }
        painter.drawPixmap(0.0,0.0,pix);
        // 是否保存图片
        if (cansaveimg)
            saveImage(pix);

        if(isDrawRect)
        {
            Draw_SearchRect(pp);
        }
        painter.drawPixmap(0.0,0.0,pix);
    }
    else
        painter.drawPixmap(0.0,0.0,pix);
}

void ClearanceImage::mousePressEvent(QMouseEvent *event)
{
	if(event->button()==Qt::LeftButton)
	{
        isDrawRect=true;
		lastRectPoint=event->pos();
        //emit RectLastPoint(lastRectPoint.x(),lastRectPoint.y());
	}
}

void ClearanceImage::mouseMoveEvent(QMouseEvent *event)
{
	int newx=event->pos().x();
    int newy=event->pos().y();
        
    int gui_height, gui_width;
    if(newx<=widthpixel/2)
    {
        gui_width=(int)(((newx)-(widthpixel/2))*pixeltomm*scale);
        gui_height=(int)((originy-(newy))*pixeltomm*scale);//距轨面高度
    }
    else
    {
        gui_width=(int)(((newx)-(widthpixel/2))*pixeltomm*scale);
        gui_height=(int)((originy-(newy))*pixeltomm*scale);
    }
    emit sendMousePos(gui_width, gui_height);
    setCursor(QCursor(Qt::ArrowCursor));//加上这一句才行
	update();

	isDrawRect=true;
	if(event->buttons()&Qt::LeftButton)//鼠标左键按下的同时移动鼠标
	{
		endRectPoint = event->pos();
        ConvertPixeltoMM();
		update();
	}
}

void ClearanceImage::mouseReleaseEvent(QMouseEvent *event)
{
    isDrawRect=true;
	if (event->button()==Qt::LeftButton)//鼠标左键释放
	{
	    endRectPoint = event->pos();
        ConvertPixeltoMM();

        qDebug() << "TOPLEFTPOINT" << TopLeftPoint.x() << ", " << TopLeftPoint.y() << "BOTTOMRIGHT" << BottomRightPoint.x() << ", " << BottomRightPoint.y();
        emit sendInterestRectangle(this->TopLeftPoint.x(), this->TopLeftPoint.y(), this->BottomRightPoint.x(), this->BottomRightPoint.y());
		update();
	}
}

SectionImage::SectionImage(QWidget *parent):
    BaseImage(parent)
{
    scale=40;
    widthpixel=680;
    heightpixel=790;
    setFixedSize(widthpixel,heightpixel);//即是宽297.5毫米，长280毫米

    // 提高度数据初始化
    hasinit = false;
    rowcount = 0;
    // 中心净高
    minHeight = -1;
    hasMinHeightVal = false;

    // 对比数据初始化
    compared_hasinit = false;
    compared_rowcount = 0;
    // 对比数据中心净高
    compared_minHeight = -1;
    compared_hasMinHeightVal = false;

    isoutputcorrect = true;//是否保存图片
    ishaveRect = false;

    // 界面上默认的是1(基本限界电力牵引）
    floornumber = OutputClearanceImageType::OutType_B_DianLi;
    multilframe = NULL;
    
    // 实际点集合数组
    pointsFuseResult = vector<QPointF*>();
    hasinitfuse = false;
    fuse_count = vector<int>();
    fuse_points_cameragroup = vector<int>();

    // 默认不显示高度辅助线
    isshowheightline = false;
    
    // 默认显示最小高度
    ifShowMinHeight = true;

    // 还未查找到正在移动点
    isFindCorrectPoint = false;

    setMouseTracking(true);

    TopLeftPoint.setX(0);
    TopLeftPoint.setY(0);
    BottomRightPoint.setX(0);
    BottomRightPoint.setY(0);
}

SectionImage::~SectionImage()
{
    if (hasinit)
    {
        delete [] pointstwo_rowcountStraight1;
        delete [] pointstwo_rowcountStraight2;
    }

    if (compared_hasinit)
    {
        delete [] compared_rowcountStraight1;
        delete [] compared_rowcountStraight2;
    }

    if (hasinitfuse)
    {
        for (int i = 0; i < pointsFuseResult.size(); i++)
            delete [] pointsFuseResult.at(i);
        pointsFuseResult.clear();
        fuse_count.clear();
        fuse_points_cameragroup.clear();
    }
}

/**
 * 初始化数组
 */
void SectionImage::initPointsArray(int arraysize, bool iscompared)
{
    if (arraysize <= 0)
        return;

    if (!iscompared)
    {
        if (hasinit)
            return;
        pointstwo_rowcountStraight1 = new QPointF[arraysize];//左侧限界线
        pointstwo_rowcountStraight2 = new QPointF[arraysize];//右侧限界线
        hasinit = true;
    }
    else
    {
        if (compared_hasinit)
            return;
        compared_rowcountStraight1 = new QPointF[arraysize];//左侧限界线
        compared_rowcountStraight2 = new QPointF[arraysize];//右侧限界线
        compared_hasinit = true;
    }
}

/**
 * Mat矩阵给pointsFuseResult数组赋值
 */
bool SectionImage::MatDataToAccuratePointsArray(Mat & fuse_pnts, bool isFuse, bool isQR, bool ifnotswapleftright)
{
    if (hasinitfuse)
    {
        for (int i = 0; i < pointsFuseResult.size(); i++)
            delete [] pointsFuseResult.at(i);
        pointsFuseResult.clear();
        fuse_count.clear();
        fuse_points_cameragroup.clear();
    }

    int fuse_count2 = fuse_pnts.cols;
    pointsFuseResult = vector<QPointF*>();//[fuse_count];
    hasinitfuse = true;

    QPointF * tmppointarray = new QPointF[fuse_count2];
    QPointF tmppoint;
    int tmpgroupid = -1, lastgroupid = -1;
    bool isnewgroup;
    int tmpcount = 0;
    // 是否置换x的正负（浏览时坐标系由于车厢正反会有影响，浏览时的坐标与人眼按轨道行驶顺序观察一直）
    int t = 1;
    if (!ifnotswapleftright)
        t = -1;
    //Point3d point;
    QPointF tmppoint_QR_rail;
    int tmpcount_QR_rail = 0;
    QPointF * tmppointarray2 = new QPointF[fuse_count2];
    if (isQR)
    {
        ;
    }

    for (int i = 0; i < fuse_pnts.cols; i++)                          
	{
        if (isFuse) // 融合结果，多种颜色表示
        {
            tmppoint.setX(* fuse_pnts.ptr<double>(0,i));
		    tmppoint.setY(t * (* fuse_pnts.ptr<double>(1,i)));
		    tmpgroupid = (int) *fuse_pnts.ptr<double>(2,i);
            if (lastgroupid == -1)
            {
                lastgroupid = tmpgroupid;
                qDebug() << "new group id" << tmpgroupid;
            }
            if (lastgroupid == tmpgroupid)
            {
                tmppointarray[tmpcount] = tmppoint;
                tmpcount++;
            }
            else
            {
                qDebug() << "new group id" << tmpgroupid;
                if (tmpcount != 0)
                {
                    QPointF * tointertpointarray = new QPointF[tmpcount];
                    for (int j = 0; j < tmpcount; j++)
                        tointertpointarray[j] = tmppointarray[j];
                    pointsFuseResult.push_back(tointertpointarray);
                    fuse_count.push_back(tmpcount);
                    fuse_points_cameragroup.push_back(lastgroupid);
                }
                tmpcount = 0;
                tmppointarray[tmpcount] = tmppoint;
                tmpcount++;
                lastgroupid = tmpgroupid;
            }
        }
        else // 双面计算中间结果、车底计算中间结果，只用一种颜色表示
        {
            //if ((int)*fuse_pnts.ptr<float>(2,i) == -1)
            {
                //qDebug() << *fuse_pnts.ptr<float>(0,i) << *fuse_pnts.ptr<float>(1,i) << *fuse_pnts.ptr<float>(2,i); 
                tmppoint.setX(* fuse_pnts.ptr<float>(0,i));
		        tmppoint.setY(t * (* fuse_pnts.ptr<float>(1,i)));
		        //point.z = * fuse_pnts.ptr<float>(2,i);
                tmppointarray[tmpcount] = tmppoint;
                tmpcount++;
            }
            if (isQR)
            {
                if ((int)*fuse_pnts.ptr<float>(2,i) == -1)
                {
                    tmppoint_QR_rail.setX(* fuse_pnts.ptr<float>(0,i));
		            tmppoint_QR_rail.setY(t * (* fuse_pnts.ptr<float>(1,i)));
                    tmppointarray2[tmpcount_QR_rail] = tmppoint_QR_rail;
                    tmpcount_QR_rail++;
                }
            }

        }
	}

    //if (!isFuse)
    //    tmpcount = 0;

    if (tmpcount != 0)
    {
        QPointF * tointertpointarray = new QPointF[tmpcount];
        for (int j = 0; j < tmpcount; j++)
            tointertpointarray[j] = tmppointarray[j];
        pointsFuseResult.push_back(tointertpointarray);
        fuse_count.push_back(tmpcount);
        fuse_points_cameragroup.push_back(lastgroupid);
    }

    if (isQR && tmpcount_QR_rail != 0)
    {
        QPointF * tointertpointarray = new QPointF[tmpcount_QR_rail];
        for (int j = 0; j < tmpcount_QR_rail; j++)
            tointertpointarray[j] = tmppointarray2[j];
        pointsFuseResult.push_back(tointertpointarray);
        fuse_count.push_back(tmpcount_QR_rail);
        fuse_points_cameragroup.push_back(0);
        tmpcount_QR_rail = 0;
    }

    tmpcount = 0;

    return true;
}

/**
 * 是否显示高度辅助线
 */
void SectionImage::drawHeightsLine(QPainter & pp)
{
    BaseImage::drawHeightsLine(pp);

    // 画顶高辅助线
    if (isshowheightline)
    {
        if (!cansaveimg)
        {
            QColor color(128, 64, 0);
            pp.setPen(QPen(color, 1));
            QBrush bruch(Qt::FDiagPattern);//画刷
            bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
            pp.setBrush(bruch);
            qreal minheightpix = originy - minHeight/scale/pixeltomm;
            QPointF pointsthree[3]={QPointF(100.0,minheightpix),QPointF(widthpixel/2,minheightpix),QPointF((widthpixel-100),minheightpix)};
            pp.drawPolyline(pointsthree,1.5);
        }
    }
}

void SectionImage::setInterestRectangle(float topleft_x, float topleft_y, float bottomright_x, float bottomright_y, int neworiginy)
{
    this->originy = neworiginy;

    ishaveRect = true;

    TopLeftPoint.setX(topleft_x);
    TopLeftPoint.setY(topleft_y);
    BottomRightPoint.setX(bottomright_x);
    BottomRightPoint.setY(bottomright_y);
}

/**
 * 画车厢，验证车底高差分中测量得到的RT
 */
void SectionImage::Draw_Coach(QPainter & pp)
{
}

void SectionImage::Draw_Straight(QPainter & pp, bool iscompared)
{
    bool * hinit = &hasinit;
    float * minh = &minHeight;
    bool * hasminh = &hasMinHeightVal;
    int * rcount = &rowcount;
    QPointF * points_s1 = pointstwo_rowcountStraight1;
    QPointF * points_s2 = pointstwo_rowcountStraight2;

    if (iscompared)
    {
        hinit = &compared_hasinit;
        minh = &compared_minHeight;
        hasminh = &compared_hasMinHeightVal;
        rcount = &compared_rowcount;
        points_s1 = compared_rowcountStraight1;
        points_s2 = compared_rowcountStraight2;
    }
    else
    {
        // 画数字的标记线
        QLineF *linef1 = new QLineF[rowcount];
        for(int i=0;i<rowcount;i=i+3)
        {
            pp.setPen(QPen(Qt::black,1.5));
            //static const QPointF pointstwo_remark[2]={QPointF(400.0,820-(pointstwo_rowcount.at(i).x()/10)),QPointF(395.0,820-(pointstwo_rowcount.at(i).x()/10))};
            // pp.drawPolyline(pointstwo_remark,2);
            //画水平的标记线//只能画一个点，因为是静态的
            linef1[i].setP1(QPointF((widthpixel/2),(originy-((pointstwo_rowcountStraight1[i].rx()/scale)/pixeltomm))));
            linef1[i].setP2(QPointF(((widthpixel/2)-5),(originy-((pointstwo_rowcountStraight1[i].rx()/scale)/pixeltomm))));
            pp.setPen(QPen(Qt::black,1.5));//两个参数为画笔的颜色和粗细
            pp.drawText((widthpixel/2),(originy-(pointstwo_rowcountStraight1[i].rx()/scale)/pixeltomm),QString("%1").arg(pointstwo_rowcountStraight1[i].rx()));//标记时为了使数字在横线下方可以改为810
        }
        pp.drawLines(linef1,rowcount);
        delete []linef1;
    }

    int drawrowcount = *rcount;
    
    // 是否显示中心高度
    if (ifShowMinHeight)
    {
        // 画中心净高
        if (*hasminh)
        {
            // 大于最高和最低的赋零
            int tempkey;
            for (int i = 0; i < rowcount; i++)
            {
                tempkey = points_s1[i].rx();
                if (tempkey > minHeight)
                {
                    drawrowcount--;
                }
            }
        }
    }

    // 开始画实际的隧道数据,左侧直线
    if (iscompared)
        pp.setPen(QPen(Qt::gray, 1));
    else
        pp.setPen(QPen(Qt::red,1));
    QBrush bruch(Qt::FDiagPattern);//画刷
    bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
    pp.setBrush(bruch);
    QLineF *linef =new QLineF[drawrowcount];
    QLineF *liner =new QLineF[drawrowcount];

    for(int j=0;j<drawrowcount-1;j++)
    {
        if(points_s1[j].ry()<0)
        {
            points_s1[j].ry()=0;
        }
        if(j==(drawrowcount-2))
        {
            if(points_s1[j+1].ry()<0)
            {
                 points_s1[j+1].ry()=0;
            }
        }

        // 测试校正图形而修改的代码，使用pointstwo_rowcount1变量
        linef[j].setLine(((widthpixel/2)-((points_s1[j].ry()/scale)/pixeltomm)),(originy-((points_s1[j].rx()/scale)/pixeltomm)),
                            ((widthpixel/2)-((points_s1[j+1].ry()/scale)/pixeltomm)),(originy-((points_s1[j+1].rx()/scale)/pixeltomm)));
        // 或者下面这两行这种方式
        // linef[j].setP1(QPointF(400-(pointstwo_rowcount.at(j).y()/10),820-(pointstwo_rowcount.at(j).x()/10)));
        // linef[j].setP2(QPointF(400-(pointstwo_rowcount.at(j+1).y()/10),820-(pointstwo_rowcount.at(j+1).x()/10)));
    }
    pp.drawLines(linef,drawrowcount);
    delete []linef;
    //画右侧的那条直线
    if (iscompared)
        pp.setPen(QPen(Qt::gray, 1));
    else
        pp.setPen(QPen(Qt::red,1));
    bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
    pp.setBrush(bruch);
    for(int j=0;j<drawrowcount-1;j++)
    {
        if(points_s2[j].ry()<0)
        {
            points_s2[j].ry()=0;
        }
        if(j==(drawrowcount-2))
        {
            if(points_s2[j+1].ry()<0)
            {
                points_s2[j+1].ry()=0;
            }
        }
        liner[j].setLine(((widthpixel/2)+((points_s2[j].ry()/scale)/pixeltomm)),(originy-((points_s2[j].rx()/scale)/pixeltomm)),
                                ((widthpixel/2)+((points_s2[j+1].ry()/scale)/pixeltomm)),(originy-((points_s2[j+1].rx()/scale)/pixeltomm)));
    }
    pp.drawLines(liner,drawrowcount);
    delete []liner;

    // 连接左中右
    if (*hasminh)
    {
        QPointF pointtop;
        pointtop.setX(widthpixel/2);
        pointtop.setY(originy - (*minh)/scale/pixeltomm);
        QPointF pointtmp1;
        pointtmp1.setX((widthpixel/2) - ((points_s1[drawrowcount - 1].ry()/scale)/pixeltomm));
        pointtmp1.setY(originy-((points_s1[drawrowcount - 1].rx()/scale)/pixeltomm));
        pp.drawLine(pointtop, pointtmp1);

        QPointF pointtmp2;
        pointtmp2.setX((widthpixel/2) + ((points_s2[drawrowcount - 1].ry()/scale)/pixeltomm));
        pointtmp2.setY(originy-((points_s2[drawrowcount - 1].rx()/scale)/pixeltomm));
        pp.drawLine(pointtop, pointtmp2);
    }

    //标记那些点,以便于修正
    if (!iscompared)
    {
        pp.setPen(QPen(Qt::green,1));
        bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
        pp.setBrush(bruch);
        QPointF *point = new  QPointF[drawrowcount];
        QPointF *pointr = new QPointF[drawrowcount];
        for(int i=0;i<drawrowcount;i++)
        {
            point[i].setX(((widthpixel/2)-((pointstwo_rowcountStraight1[i].ry()/scale)/pixeltomm)));
            point[i].setY((originy-((pointstwo_rowcountStraight1[i].rx()/scale)/pixeltomm)));
            pointr[i].setX(((widthpixel/2)+((pointstwo_rowcountStraight2[i].ry()/scale)/pixeltomm)));
            pointr[i].setY((originy-((pointstwo_rowcountStraight2[i].rx()/scale)/pixeltomm)));
        }
        pp.drawPoints(point,drawrowcount);
        pp.drawPoints(pointr,drawrowcount);
        delete []point;
        delete []pointr;
    }
    if (*hasminh)
    {
        pp.setPen(QPen(Qt::yellow, 3));
        QPointF pointtop;
        pointtop.setX(widthpixel/2);
        pointtop.setY(originy - (*minh)/scale/pixeltomm);
        pp.drawPoint(pointtop);
        // 显示文字
        QColor color(128, 64, 0);
        pp.setPen(QPen(color, 3));
        // -50表示让出距离
        pp.drawText((widthpixel/2) - 50,(originy-((*minh)/scale)/pixeltomm),QString("%1").arg((int)(*minh)));//标记时为了使数字在横线下方可以改为810
    }
}

/**
 * 画融合测量限界点
 */
void SectionImage::Draw_Fuse(QPainter &pp)
{
    if (hasinitfuse)
    {
        for (int i = 0; i < pointsFuseResult.size(); i++)
        {
            // 开始画实际的隧道数据
            int groupindex = fuse_points_cameragroup.at(i);
            if (groupindex < 1 || groupindex > 16)
                continue;

            QColor color(0, 0, 255);
            if (groupindex%3 == 0)
                color = QColor(255, 0, 255);
            else if (groupindex%3 == 2)
                color = QColor(0, 255, 255);

            pp.setPen(QPen(color, 1));
            QBrush bruch(Qt::FDiagPattern);//画刷
            bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
            pp.setBrush(bruch);
            QPointF* tmppoints2 = pointsFuseResult.at(i);
            QPointF* tmppoints = new QPointF[fuse_count.at(i)];
            for(int j = 0; j < fuse_count.at(i); j++)
            {
                // 因为有负值，不分左右
                tmppoints[j] = QPointF((widthpixel/2)+((tmppoints2[j].ry()/scale)/pixeltomm), originy-((tmppoints2[j].rx()/scale)/pixeltomm));
            }
            pp.drawPoints(tmppoints, fuse_count.at(i));
            delete[] tmppoints;
        }
    }
}

void SectionImage::drawMovingInterestPointsLine(QPainter & pp)
{
    QBrush bruch(Qt::FDiagPattern);//画刷
    pp.setPen(QPen(Qt::black,3,Qt::DotLine));//两个参数为画笔的颜色和粗细
    bruch.setStyle(Qt::NoBrush);//将画刷设置成NULL
    pp.setBrush(bruch);

    QPointF pointsthree[3]={
                                QPointF(((widthpixel/2)+((correctPoint_1_FZ.rx()/scale)/pixeltomm)), (originy-((correctPoint_1_FZ.ry()/scale)/pixeltomm))),
                                QPointF(((widthpixel/2)+((correctPoint_FZ.rx()/scale)/pixeltomm)),(originy-((correctPoint_FZ.ry()/scale)/pixeltomm))),
                                QPointF(((widthpixel/2)+((correctPoint_2_FZ.rx()/scale)/pixeltomm)),(originy-((correctPoint_2_FZ.ry()/scale)/pixeltomm)))
                            };
    pp.drawPolyline(pointsthree,3);
}

void SectionImage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);//创建QPinter的一个对象,this就表明了是在窗口上进行绘图，所以利用painter进行的绘图都是在窗口上的，painter进行的坐标变化，是变化的窗口的坐标系.
    QPixmap pix(widthpixel,heightpixel);
    pix.fill(Qt::white);//背景填充为白色
    QPainter pp(&pix);//新建QPainter类对象，在pix上进行绘图
    QBrush bruch(Qt::FDiagPattern);//画刷
   
    // 画底板
    Draw_Floor(pp);
    // 是否显示高度辅助线
    drawHeightsLine(pp);

    if (ifshowpointsarray)
    {
        // 蓝线画融合数据
        if (/*!cansaveimg && TODO TODELETE*/hasinitfuse && ifshowfusepointarray)
            Draw_Fuse(pp);
  
        // 用红粗线画实际的隧道数据
        if (rowcount==0)
        {
            painter.drawPixmap(0.0,0.0,pix);//表示从widget窗口的原点（0，0）点添加该QPixmap类对象
        }
        else
        {
            if (ifshowsyndata)
            {
                if (compared_hasinit)
                    Draw_Straight(pp, true);

                Draw_Straight(pp);

                // 是否保存图片
                if (cansaveimg)
                    saveImage(pix);
            }
        }
        if (isFindCorrectPoint)// 如果找到有正常移动的点，说明鼠标正在拖动，话虚线
        {
            drawMovingInterestPointsLine(pp);
            painter.drawPixmap(0.0,0.0,pix);
        }
        painter.drawPixmap(0.0,0.0,pix);//表示从widget窗口的原点（0，0）点添加该QPixmap类对象

        if(ishaveRect)
        {
            Draw_SearchRect(pp);
        }
        painter.drawPixmap(0.0,0.0,pix);
    }
    else
        painter.drawPixmap(0.0,0.0,pix);
}

void SectionImage::setShowSynHeightsResultData(bool ifshow)
{
    this->ifshowsyndata = ifshow;
}

void SectionImage::CorrectorNot(bool iscorrect)
{
    this->isoutputcorrect=iscorrect;
}

void SectionImage::mousePressEvent(QMouseEvent *event)
{
    // 按下时还未查找到正在移动点
    isFindCorrectPoint = false;
   
    if (isoutputcorrect==false)
    {
        return;
    }
    else
    {
        if (event->button() == Qt::LeftButton)//当鼠标左键按下
        {
            lastPoint=event->pos();//获得鼠标指针的当前坐标作为起始坐标

            findCorrectPoint();
        }
    }
}

void SectionImage::mouseMoveEvent(QMouseEvent *event)
{
    if (isoutputcorrect==false)
    {
        return;
    }
    else
    {
        endPoint=event->pos();
        if (isFindCorrectPoint)
        {
            updateCorrectPoint(false);
            update();
            return;
        }

        int newx=event->pos().x();
        int newy=event->pos().y();
        
        int gui_height, gui_width;
        if(newx<=widthpixel/2)
        {
            gui_width=(int)(((newx)-(widthpixel/2))*pixeltomm*scale);
            gui_height=(int)((originy-(newy))*pixeltomm*scale);//距轨面高度
        }
        else
        {
            gui_width=(int)(((newx)-(widthpixel/2))*pixeltomm*scale);
            gui_height=(int)((originy-(newy))*pixeltomm*scale);
        }
        emit sendMousePos(gui_width, gui_height);
        setCursor(QCursor(Qt::ArrowCursor));//加上这一句才行
        
        // 找兴趣点，修正点
        for(int i=0;i<rowcount;i++)//现在的点是像素，要转换成毫米
        {
            if(newx<=(widthpixel/2))//左侧距离
            {
                int x=(int)((widthpixel/2)-((pointstwo_rowcountStraight1[i].ry()/scale)/pixeltomm));
                int y=(int)(originy-((pointstwo_rowcountStraight1[i].rx()/scale)/pixeltomm));
                // 找到临近位置
                if(qAbs((x-newx))<=5&&qAbs((y-newy))<=5) 
                {
                    setCursor(QCursor(Qt::CrossCursor));//手形
                }
            }
            else//右侧距离
            {
                int x=(int)((widthpixel/2)+((pointstwo_rowcountStraight2[i].ry()/scale)/pixeltomm));
                int y=(int)(originy-((pointstwo_rowcountStraight2[i].rx()/scale)/pixeltomm));
                // 找到临近位置
                if(qAbs((x-newx))<=5&&qAbs((y-newy))<=5)
                {
                    setCursor(QCursor(Qt::CrossCursor));//手形
                }
            }
        }
    }
}

void SectionImage::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        endPoint=event->pos();
        if (isFindCorrectPoint)
        {
            updateCorrectPoint(true);
            update();
        }
    }
    // 鼠标抬起时设置为未查找到正在移动点
    isFindCorrectPoint = false;
}

void SectionImage::contextMenuEvent(QContextMenuEvent *event)//鼠标右键，弹出式菜单
{
    // @author 范翔注释，暂时不用
    /*if(isoutputcorrect==false)
    {
        return;
    }
    else
    {
        QMenu *menu = new QMenu(this);
        QAction *multiframe = new QAction(tr("多帧修正"),menu);
        QAction *imageview = new QAction(tr("原图浏览"),menu);
        menu->addAction(multiframe);
        menu->addAction(imageview);
        connect(multiframe,SIGNAL(triggered()),this,SLOT(multiframecorrect()));
        menu->move(cursor().pos()); //让菜单显示的位置在鼠标的坐标上
        menu->show();
    }*/
}

void SectionImage::findCorrectPoint()
{
    for(int i = 0; i < rowcount; i++)//现在的点是像素，要转换成毫米
    {
        int gui_width;
        if (lastPoint.x() <= (widthpixel/2))//左侧距离
        {
            int x = (int)((widthpixel/2)-((pointstwo_rowcountStraight1[i].ry()/scale)/pixeltomm));
            int y = (int)(originy-((pointstwo_rowcountStraight1[i].rx()/scale)/pixeltomm));
            // 找到临近位置
            if (qAbs((x-lastPoint.x()))<=5&&qAbs((y-lastPoint.y()))<=5)
            {
                if (endPoint.x()>=widthpixel/2)
                {
                    endPoint=QPointF(widthpixel/2,endPoint.y());
                }
                // 记录当前点
                correctPoint_FZ.setX((-1)*pointstwo_rowcountStraight1[i].ry());
                correctPoint_FZ.setY(pointstwo_rowcountStraight1[i].rx());
                // 记录相邻点
                if (i > 0)
                {
                    correctPoint_1_FZ.setX((-1)*pointstwo_rowcountStraight1[i-1].ry());
                    correctPoint_1_FZ.setY(pointstwo_rowcountStraight1[i-1].rx());
                }
                else
                    correctPoint_1_FZ = correctPoint_FZ;
                if (i < rowcount - 1)
                {
                    correctPoint_2_FZ.setX((-1)*pointstwo_rowcountStraight1[i+1].ry());
                    correctPoint_2_FZ.setY(pointstwo_rowcountStraight1[i+1].rx());
                }
                else
                    correctPoint_2_FZ = correctPoint_FZ;
                    
                emit sendUpdateSectionDataVal((-1)*correctPoint_FZ.rx(), correctPoint_FZ.ry(), true, false);
                isFindCorrectPoint = true;
                return;
            }
        }
        else//右侧距离
        {
            int x=(int)((widthpixel/2)+(( pointstwo_rowcountStraight2[i].ry()/scale)/pixeltomm));
            int y=(int)(originy-(( pointstwo_rowcountStraight2[i].rx()/scale)/pixeltomm));
            // 找到临近位置
            if(qAbs((x-lastPoint.x()))<=5&&qAbs((y-lastPoint.y()))<=5)
            {
                if(endPoint.x()<=widthpixel/2)
                {
                    endPoint=QPointF(widthpixel/2,endPoint.y());
                }
                // 记录当前点
                correctPoint_FZ.setX(pointstwo_rowcountStraight2[i].ry());
                correctPoint_FZ.setY(pointstwo_rowcountStraight2[i].rx());
                // 记录相邻点
                if (i > 0)
                {
                    correctPoint_1_FZ.setX(pointstwo_rowcountStraight2[i-1].ry());
                    correctPoint_1_FZ.setY(pointstwo_rowcountStraight2[i-1].rx());
                }
                else
                    correctPoint_1_FZ = correctPoint_FZ;
                if (i < rowcount - 1)
                {
                    correctPoint_2_FZ.setX(pointstwo_rowcountStraight2[i+1].ry());
                    correctPoint_2_FZ.setY(pointstwo_rowcountStraight2[i+1].rx());
                }
                else
                    correctPoint_2_FZ = correctPoint_FZ;

                emit sendUpdateSectionDataVal(correctPoint_FZ.rx(), correctPoint_FZ.ry(), false, false);
                isFindCorrectPoint = true;
                return;
            }
        }
    }
}

void SectionImage::updateCorrectPoint(bool ismouserelease)
{
    // 如果是鼠标释放，更新数组可实现显示
    if (ismouserelease)
    {
        for(int i = 0; i < rowcount; i++)//现在的点是像素，要转换成毫米
        {
            int gui_width;
            if (lastPoint.x() <= (widthpixel/2))//左侧距离
            {
                int x = (int)((widthpixel/2)-((pointstwo_rowcountStraight1[i].ry()/scale)/pixeltomm));
                int y = (int)(originy-((pointstwo_rowcountStraight1[i].rx()/scale)/pixeltomm));
                // 找到临近位置
                if (qAbs((x-lastPoint.x()))<=5&&qAbs((y-lastPoint.y()))<=5)
                {
                    if (endPoint.x()>=widthpixel/2)
                    {
                        endPoint=QPointF(widthpixel/2,endPoint.y());
                    }
                    int endy = (int)(((widthpixel/2)-endPoint.x())*pixeltomm*scale);
                    pointstwo_rowcountStraight1[i].ry()=endy;
                    emit sendUpdateSectionDataVal(pointstwo_rowcountStraight1[i].ry(), pointstwo_rowcountStraight1[i].rx(), true, true);
                }
            }
            else//右侧距离
            {
                int x=(int)((widthpixel/2)+(( pointstwo_rowcountStraight2[i].ry()/scale)/pixeltomm));
                int y=(int)(originy-(( pointstwo_rowcountStraight2[i].rx()/scale)/pixeltomm));
                // 找到临近位置
                if(qAbs((x-lastPoint.x()))<=5&&qAbs((y-lastPoint.y()))<=5)
                {
                    if(endPoint.x()<=widthpixel/2)
                    {
                        endPoint=QPointF(widthpixel/2,endPoint.y());
                    }
                    int endy=(int)((endPoint.x()-(widthpixel/2))*pixeltomm*scale);
                    pointstwo_rowcountStraight2[i].ry()=endy;
                    emit sendUpdateSectionDataVal(pointstwo_rowcountStraight2[i].ry(),pointstwo_rowcountStraight2[i].rx(), false, true);
                }
            }
        }
    }
    else // 鼠标移动，只更新虚线点
    {
        int gui_width;
        if (lastPoint.x() <= (widthpixel/2))//左侧距离
        {
            if (endPoint.x()>=widthpixel/2)
            {
                endPoint=QPointF(widthpixel/2,endPoint.y());
            }
            int endy = (int)(((widthpixel/2)-endPoint.x())*pixeltomm*scale);
            correctPoint_FZ.setX((-1)*endy);
            emit sendUpdateSectionDataVal((-1)*correctPoint_FZ.rx(), correctPoint_FZ.ry(), true, false);
        }
        else//右侧距离
        {
            if(endPoint.x()<=widthpixel/2)
            {
                endPoint=QPointF(widthpixel/2,endPoint.y());
            }
            int endy=(int)((endPoint.x()-(widthpixel/2))*pixeltomm*scale);
            correctPoint_FZ.setX(endy);
            emit sendUpdateSectionDataVal(correctPoint_FZ.rx(), correctPoint_FZ.ry(), false, false);
        }
    }
}

void SectionImage::multiframecorrect()//单点修正函数
{
    multilframe = new MultiframeCorrect();
    multilframe->show();
}

/**
 * SectionData数组在SectionImage中给2个PointsArray数组赋值
 * @param data 自定义数据结构-断面限界数据，一个map
 */
bool SectionImage::SectionDataToPointsArray(SectionData & data, bool iscompared)
{
    bool * hinit = &hasinit;
    float * minh = &minHeight;
    bool * hasminh = &hasMinHeightVal;
    int * rcount = &rowcount;
    QPointF * points_s1 = pointstwo_rowcountStraight1;
    QPointF * points_s2 = pointstwo_rowcountStraight2;

    if (iscompared)
    {
        hinit = &compared_hasinit;
        minh = &compared_minHeight;
        hasminh = &compared_hasMinHeightVal;
        rcount = &compared_rowcount;
        points_s1 = compared_rowcountStraight1;
        points_s2 = compared_rowcountStraight2;
    }

    if (!(*hinit))
    {
        *hasminh = false;
        return false;
    }
    
    // 中心净高赋值
    *minh = data.getCenterHeight();
    if (*minh < 0)
        *hasminh = false;
    else
        *hasminh = true;

    int szcount = data.getMaps().size();
    data.showMaps();
    std::map <int,item>::iterator it = data.getMaps().begin();
    int j=0;

    while(it!=data.getMaps().end())
    {
        std::pair<int,item> pair = (*it);
        points_s1[j]=QPointF(pair.first,pair.second.left);
        points_s2[j]=QPointF(pair.first,pair.second.right);
        it++;
        j++;
	}
    *rcount = szcount;
    return true;
}

/**
 * 是否显示最小高度
 */
void SectionImage::setShowMinHeight(bool ifshow)
{
    ifShowMinHeight = ifshow;
}