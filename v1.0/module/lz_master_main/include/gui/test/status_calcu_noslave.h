#ifndef STATUS_CALCU_NOSLAVE_WIDGET_H
#define STATUS_CALCU_NOSLAVE_WIDGET_H

#include <QWidget>

#include "status_calcu.h"

namespace Ui {
    class CalcuStatusNoSlaveWidget;
}

/**
 * 监控界面——计算备份界面类声明
 * @author 范翔
 * @version 1.0.0
 * @date 2014-12-06
 */
class CalcuStatusNoSlaveWidget : public CalcuWidget
{
    Q_OBJECT

public:
    explicit CalcuStatusNoSlaveWidget(QWidget *parent = 0);

    ~CalcuStatusNoSlaveWidget();

protected slots:
    virtual void on_stopCalcuButton_clicked();

    virtual void on_stopFuseButton_clicked();

    virtual void calculate_beginStartAll();

    virtual void calculate_beginStartOneTunnel(int tunnelid);

    virtual void fuse_beginStartAll();

    virtual void fuse_beginStartOneTunnel(int tunnelid);
	
	virtual void extract_beginStartAll();

	virtual void extract_beginStartOneTunnel(int tunnelid);
	
signals:
    /**
     * @param int 类型，0双目全部算，1双目按隧道算 2 fuse全部算，4 fuse按隧道算 
     * @param int tunnelid
     */
    void startcalcu(int, int, bool, bool);

};

#endif // STATUS_CALCU_NOSLAVE_WIDGET_H
