#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

#include <vector>
#include "Vector3D.h"

using namespace std;

class Vector3D;

//! Reimplementation of QGraphicsItem to draw grid points
class GridPoint //: public QGraphicsItem
{
    //! Reimplemented from QGraphicsItem
    //QRectF boundingRect() const;
    //! Reimplemented from QGraphicsItem
    //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

/**
 * OpenGL显示界面
 * @author 范翔
 * @date 20150401
 * @version 1.0.0
 */
class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    //! Function to pass a pointer to the QGraphicsScene on which to paint items
    //void setScene(QGraphicsScene *scenePass);
    //! Handles drawing the grid points
    //void drawGrid(QList<QPoint> pointList, Settings *settings);
    //! Handles removal of the grid
    //void removeGrid();
    //! Handles drawing the vectors
    //void drawVectors(PivData *pivData, Settings *settings);

    void draw3DCoordinate();

    //! Handles removal of the vectors
    void removeVectors();

    void setPointsList(std::vector<Point3DGL> *data);
    void drawPointsList(std::vector<Point3DGL> *data);

    std::vector<Point3DGL> * pointsdata;

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    //void setViewPointLength(int viewPointLength);

    void setScale(int);
signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
    void viewPointLengthChanged(int viewPointLength);

    // copy from itempaint.h

    //! Signal emitted when the grid is drawn or updated
    //void gridDrawn();
    //! Signal emitted when the vectors are drawn or updated
    //void vectorsDrawn();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    // copy from itempaint.h

    //! Creates the QGraphicsItemGroup for the vectors
    void createVectorGroup(int num);
    //! Creates the QGraphicsItemGroup for the grid points
    //void createGridPointGroup(int num);

private:

    int xRot;
    int yRot;
    int zRot;
    int viewPointLeng;
    QPoint lastPos;

    // xAxis ,yAxis, zAxis Arrow,   // 实际有速度数据的窗格范围
    qreal xSpan, ySpan, zSpan;
    Vector3D *xAxisArrow;
    Vector3D *yAxisArrow;
    Vector3D *zAxisArrow;

    bool createCoordinateGrid;

    // copy from itempaint.h

    //QGraphicsScene *scene;
    //QGraphicsItemGroup *gridPointGroup;
    //QGraphicsItemGroup *vectorGroup;
    bool gridPointGroupCreated;
    bool vectorGroupCreated;
    QList<GridPoint *> *gridPointList;
    int currentGridSize;
    //QList<Vector *> *vectorList;

    int _scale;
};

#endif
