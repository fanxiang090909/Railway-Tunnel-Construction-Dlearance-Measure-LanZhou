#ifndef VIEW3DTVCALCUDATAWIDGET_H
#define VIEW3DTVCALCUDATAWIDGET_H

#include <QWidget>

#include "imageviewer_3dtv.h"
#include "select_tunnel.h"

namespace Ui {
    class View3DTVCalcuMatDataWidget;
}

class View3DTVCalcuMatDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit View3DTVCalcuMatDataWidget(QWidget *parent = 0);
    ~View3DTVCalcuMatDataWidget();

signals:
    void my_close();

public slots:
    void my_close_slot();

private slots:

    void button4Clicked();
    void button5Clicked();

    void slotSelectedTunnelToSynthesis(int, QString, bool, bool, bool, double, long long, long long);

private:
    Ui::View3DTVCalcuMatDataWidget *ui;

    // 转变TabButton格式
    void setTabButtonStyle(int i);

    SelectAvaliableTunnelWidget * widget4;
    ImageViewer3DTwoViewWidget * widget56;
};

#endif // VIEW3DTVCALCUDATAWIDGET_H
