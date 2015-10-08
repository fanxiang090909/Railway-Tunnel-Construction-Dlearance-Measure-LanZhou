#ifndef STATUS_CALCU_MASTER_WIDGET_H
#define STATUS_CALCU_MASTER_WIDGET_H

#include "masterprogram.h"
#include <QWidget>

#include "status_calcu.h"

namespace Ui {
    class CalcuMasterWidget;
}

/**
 * 监控界面——计算备份界面类声明
 * @author 范翔
 * @version 1.0.0
 * @date 2014-12-06
 */
class CalcuMasterWidget : public CalcuWidget
{
    Q_OBJECT

public:
    explicit CalcuMasterWidget(QWidget *parent = 0);

    ~CalcuMasterWidget();

protected slots:

    virtual void saveResetFile();

    virtual void on_stopCalcuButton_clicked();

    virtual void on_stopFuseButton_clicked();

    virtual void calculate_beginStartAll();

    virtual void calculate_beginStartOneTunnel(int tunnelid);

    virtual void fuse_beginStartAll();

    virtual void fuse_beginStartOneTunnel(int tunnelid);

   	virtual void extract_beginStartAll();

    virtual void extract_beginStartOneTunnel(int tunnelid);
};

#endif // STATUS_CALCU_MASTER_WIDGET_H
