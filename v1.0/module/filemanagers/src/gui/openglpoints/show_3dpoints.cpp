#include "show_3dpoints.h"
#include "ui_show_3dpoints.h"

#include <QFile>
#include <QDebug>
#include <iostream>

/**
 * 三维点显示界面OpenGL绘制
 * @author 范翔
 * @date 20150401
 * @version 1.0.0
 */
Show3DPointsWidget::Show3DPointsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Show3DPointsWidget)
{
    ui->setupUi(this);

    glWidget = new GLWidget;

    //connect(glWidget, SIGNAL(xRotationChanged(int)), glWidget, SLOT(setXRotation(int)));
    //connect(glWidget, SIGNAL(yRotationChanged(int)), glWidget, SLOT(setYRotation(int)));
    //connect(glWidget, SIGNAL(zRotationChanged(int)), glWidget, SLOT(setZRotation(int)));

    connect(ui->scaleSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setScale(int)));
    //connect(glWidget, SIGNAL(viewPointLengthChanged(int)), ui->scaleSlider, SLOT(setValue(int)));

    ui->gridLayout->addWidget(glWidget, 0, 0, 0);

    Point3DGL pivpoint1;
    pivpoint1.x = 1;
    pivpoint1.y = 1;
    pivpoint1.z = 1;

    Point3DGL pivpoint2;
    pivpoint2.x = 2;
    pivpoint2.y = 1;
    pivpoint2.z = 1;

    Point3DGL pivpoint3;
    pivpoint3.x = 3;
    pivpoint3.y = 2;
    pivpoint3.z = 1;

    Point3DGL pivpoint4;
    pivpoint4.x = 4;
    pivpoint4.y = 4;
    pivpoint4.z = 1;

    Point3DGL pivpoint5;
    pivpoint5.x = 3;
    pivpoint5.y = 4;
    pivpoint5.z = 2;

    _data = new std::vector<Point3DGL>();
    /*_data->push_back(pivpoint1);
    _data->push_back(pivpoint2);
    _data->push_back(pivpoint3);
    _data->push_back(pivpoint4);
    _data->push_back(pivpoint5);*/

    glWidget->setPointsList(_data);


    //w.updateGL();
}

Show3DPointsWidget::~Show3DPointsWidget()
{
    if (_data != NULL)
        delete _data;
    delete glWidget;
    delete ui;
}

void Show3DPointsWidget::updateGL()
{
    glWidget->updateGL();
}

void Show3DPointsWidget::read(QString filename)
{
    // Function that reads in ASCII file data

    QFile file;
    QString in;

    file.setFileName(filename);

    // Currently here for debugging purposes
    qDebug() << "Reading file: " << filename.toLocal8Bit().data() << endl;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream inStream(&file);
        in = inStream.readLine();

        Point3DGL tmp;
        while(!(in.isNull()))
        {
            // Reads in fifth column: signal-to-noise ratio
            tmp.x = in.section(",",0,0).toDouble();
            // Reads in fifth column: signal-to-noise ratio
            tmp.y = in.section(",",1,1).toDouble();
            // Reads in fifth column: signal-to-noise ratio
            tmp.z = in.section(",",2,2).toDouble();

            _data->push_back(tmp);

            //qDebug() << "x=" << tmp.x << ", y=" << tmp.y << ", z=" << tmp.z;
            in = inStream.readLine();
        }
        file.close();
        // Point list is sent to be gridded
        //toGrids(pointList);
        //_list = pointList;

        qDebug() << "read_end" << endl;
    }
    // Do some proper debugging here at some point...
    else std::cout << "Problem reading file: " << filename.toLocal8Bit().data() << std::endl;
}

void Show3DPointsWidget::setCurrentMat(cv::Mat & mat_point, bool carriagedirection)
{
    _data->clear();

    int a = 1;
    if (!carriagedirection)
        a = -1;

    Point3DGL tmp;
    if(4 != mat_point.cols)
        for(int i=0; i<mat_point.cols; i++)                          
        {
            
            tmp.y = (*mat_point.ptr<float>(0,i));
            //////////TODO TO DELETE////////
            // (-1) 是因为外标定的坐标系与车厢正厢正向坐标系相反，建议下次外标定点测量时采用所规定的车厢正向坐标系
            tmp.z = a*(-1)*(*mat_point.ptr<float>(1,i));
            tmp.x = (*mat_point.ptr<float>(2,i));//*mat_point.ptr<float>(2,i);
            _data->push_back(tmp);
        }
}