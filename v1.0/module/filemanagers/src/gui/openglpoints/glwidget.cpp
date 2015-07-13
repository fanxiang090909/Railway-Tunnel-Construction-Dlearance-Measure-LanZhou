#include <QtGui>
#include <QtOpenGL>
#include <glut.h>
#include <math.h>
#include <vector>

#include "glwidget.h"
#include "vector3d.h"

#include <qDebug>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

/**
 * OpenGL显示界面
 * @author 范翔
 * @date 20150401
 * @version 1.0.0
 */
GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{

    xRot = 0;
    yRot = 0;
    zRot = 0;
    viewPointLeng = 50;

    //
    vectorGroupCreated = false;
    gridPointGroupCreated = false;

    gridPointList = new QList<GridPoint *>;

    xSpan = 10.0;
    ySpan = 10.0;
    zSpan = 10.0;
    xAxisArrow = new Vector3D(this,4.0,0.1, QColor(1.0,1.0,0.0),false);
    yAxisArrow = new Vector3D(this,4.0,0.1, QColor(1.0,1.0,0.0),false);
    zAxisArrow = new Vector3D(this,4.0,0.1, QColor(1.0,1.0,0.0),false);

    createCoordinateGrid = true;

    _scale = 50;
}

GLWidget::~GLWidget()
{
    delete gridPointList;
    delete xAxisArrow;
    delete yAxisArrow;
    delete zAxisArrow;
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize GLWidget::sizeHint() const
{
    return QSize(800, 800);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

/*void GLWidget::setViewPointLength(int viewPointLength)
{
    if (viewPointLeng != this->viewPointLeng)
    {
        this->viewPointLeng = viewPointLength;
        emit viewPointLengthChanged(viewPointLength);
        paintGL();
    }
}*/

void GLWidget::setScale(int newscale)
{
    _scale = newscale;
    updateGL();
}

void GLWidget::initializeGL()
{
    qglClearColor(Qt::white);

    qreal scale = 0.1;
    qreal speed = 5;

    bool hasTail = false;

    /*for (int i = 0; i < 5; i++)
    {
        Vector3D * vector3d = new Vector3D(this);
        vector3d->setVector3D(speed + i, scale, QColor(1,1,0),hasTail);
        //vector3d->setColor(qtGreen.dark());

        vector3d_list.push_back(vector3d);
    }*/

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void GLWidget::paintGL()
{
    //
    /*int side = qMin(this->width(), this->height());
    // 设置视口
    glViewport((this->width() - side) / 2, (this->height() - side) / 2, side, side);
    // 指定当前矩阵
    glMatrixMode(GL_PROJECTION);
    // 将当前矩阵换成单位阵
    glLoadIdentity();
    glOrtho(-viewPointLeng,+viewPointLeng,
            -viewPointLeng,+viewPointLeng,
            -1000,1000);
    glMatrixMode(GL_MODELVIEW);
*/
    //
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);

    /*for (int i = 0; i < vector3d_list.size(); i++)
    {
        Vector3D*  temp = vector3d_list.at(i);
        glTranslatef(0.0, 0.0, -1.0);
        temp->draw();
    }*/

    //画坐标格
    draw3DCoordinate();

    //if (pivdata != NULL && pivdata->list().length() > 0)
    //    drawVectors(pivdata);
    
    if (pointsdata != NULL)
        drawPointsList(pointsdata);
    
    //qDebug() << "helloworld";
    //qDebug() << "pivdata" << pivdata << "length" << pivdata->list().length();

}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    //int side = qMax(width, height);

    // 设置视口
    //glViewport((width - side) / 2, (height - side) / 2, side, side);
    glViewport(0.0, 0.0, width, height);

    // 指定当前矩阵
    glMatrixMode(GL_PROJECTION);
    // 将当前矩阵换成单位阵
    glLoadIdentity();

    //glOrtho(-1,+1,-1,+1,0,20);
    if (width <= height)
        glOrtho(-viewPointLeng, +viewPointLeng,
            -height/width * viewPointLeng, +height/width * viewPointLeng,
            -1000, +1000);
    else
        glOrtho(-width/height * viewPointLeng, +width/height * viewPointLeng,
            -viewPointLeng, +viewPointLeng,
            -1000, +1000);

    // 指名当前矩阵
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    // 鼠标拖动的旋转速率
    //默认为8
    double rotateRate = 8;

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + rotateRate * dy);
        setYRotation(yRot + rotateRate * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + rotateRate * dy);
        setZRotation(zRot + rotateRate * dx);
    }
    lastPos = event->pos();
}

/*
void GLWidget::setScene(QGraphicsScene *scenePass)
{
    scene = scenePass;
}

void GLWidget::drawGrid(QList<QPoint> pointList, Settings *settings)
{
    // Drawing the grid points
    GridPoint *gridPoint;
    int _intLengthX = settings->intLengthX();
    int _intLengthY = settings->intLengthY();
    if (!gridPointGroupCreated || pointList.size() != currentGridSize) createGridPointGroup(pointList.size());

    int centreX, centreY;
    for (int i = 0; i < pointList.size(); i++)
    {
        gridPoint = gridPointList->value(i);
        centreX = pointList.value(i).rx() + _intLengthX/2;
        centreY = pointList.value(i).ry() + _intLengthY/2;

        // Set the position of a given grid point
        gridPoint->setPos(centreX,centreY);
        // Grid points are added and deleted using visibility rather than actually creating/deleting objects
        gridPoint->setVisible(true);
    }
    emit(gridDrawn());
}*/

void GLWidget::draw3DCoordinate()
{
    //glColor3f(0,0,0);
    GLfloat mat_amb_diff[] = {0.1, 0, 0.8, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);

    // 画坐标轴
    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(xSpan, 0.0, 0.0);
    glEnd();
    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, ySpan, 0.0);
    glEnd();
    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, zSpan);
    glEnd();

    // x轴末端箭头
    glTranslatef(xSpan,0.0,0.0);
    glRotatef(-90.0, 0.0,1.0,0.0);
    xAxisArrow->setVector3D(viewPointLeng + 10.0, 0.1, QColor(Qt::red),false);
    xAxisArrow->draw();
    xAxisArrow->setRotation(0.0,0.0,0.0);
    glRotatef(+90.0, 0.0,1.0,0.0);
    glTranslatef(-xSpan,0.0,0.0);

    // y轴末端箭头
    glTranslatef(0.0,ySpan,0.0);
    glRotatef(+90.0, 1.0,0.0,0.0);
    yAxisArrow->setVector3D(viewPointLeng + 10.0, 0.1, QColor(Qt::green),false);
    yAxisArrow->setRotation(0.0,0.0,0.0);
    yAxisArrow->draw();
    glRotatef(-90.0, 1.0,0.0,0.0);
    glTranslatef(0.0,-ySpan,0.0);

    // z轴末端箭头
    glTranslatef(0.0,0.0,zSpan);
    glRotatef(180.0, 1.0,0.0,0.0);
    zAxisArrow->setVector3D(viewPointLeng + 10.0, 0.1, QColor(Qt::yellow),false);
    zAxisArrow->setRotation(0.0,0.0,0.0);
    zAxisArrow->draw();
    glRotatef(180.0, 1.0,0.0,0.0);
    glTranslatef(0.0,0.0,-zSpan);

    //glColor3f(0,0,0);
    //GLfloat mat_amb_diff[] = {0.1, 0, 0.8, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);

    /*if (!createCoordinateGrid)
        return;

    // 画坐标格
    int i, j;
    for (i = 0; i < ySpan; i++)
        for (j = 0; j < zSpan; j++)
        {
            glBegin(GL_LINES);
                glVertex3f(0.0, (GLfloat)i, (GLfloat)j);
                glVertex3f(xSpan, (GLfloat)i, (GLfloat)j);
            glEnd();
        }

    for (i = 0; i < xSpan; i++)
        for (j = 0; j < ySpan; j++)
        {
            glBegin(GL_LINES);
                glVertex3f((GLfloat)i, (GLfloat)j, 0.0);
                glVertex3f((GLfloat)i, (GLfloat)j, zSpan);
            glEnd();
        }
    for (i = 0; i < xSpan; i++)
        for (j = 0; j < zSpan; j++)
        {
            glBegin(GL_LINES);
                glVertex3f((GLfloat)i, 0.0,(GLfloat)j);
                glVertex3f((GLfloat)i, ySpan,(GLfloat)j);
            glEnd();
        }
        */
}

void GLWidget::setPointsList(std::vector<Point3DGL> *data)
{
    this->pointsdata = data;
}

void GLWidget::drawPointsList(std::vector<Point3DGL> *data)
{
    if (data == NULL)
        return;
    Point3DGL tmp;
    glBegin(GL_POINTS);
    double scale2 = (double)_scale;

    for (int i = 0; i < data->size(); i++)
    {
        tmp = (Point3DGL)data->at(i);
        glVertex3f((GLfloat)tmp.x/scale2, (GLfloat)tmp.y/scale2, (GLfloat)tmp.z/scale2);
    }
    glEnd();
}