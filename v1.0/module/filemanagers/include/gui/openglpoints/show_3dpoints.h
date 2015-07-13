#ifndef SHOW_3D_POINTS_WIDGET_H
#define SHOW_3D_POINTS_WIDGET_H

#include <QWidget>
#include "glwidget.h"
#include "LzSerialStorageMat.h"

namespace Ui {
    class Show3DPointsWidget;
}

/**
 * 三维点显示界面OpenGL绘制
 * @author 范翔
 * @date 20150401
 * @version 1.0.0
 */
class Show3DPointsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Show3DPointsWidget(QWidget *parent = 0);
    ~Show3DPointsWidget();
    
    void read(QString filename);

    void setCurrentMat(cv::Mat & currentmat, bool carriagedirection);

    void updateGL();
private:
    Ui::Show3DPointsWidget *ui;

    GLWidget *glWidget;

    std::vector<Point3DGL> * _data;

signals:
    void emit_scale(int newscale);
};

#endif // SHOW_3D_POINTS_WIDGET_H
