#ifndef CORRECT_CLEARANCE_BATCHOUTPUT_H
#define CORRECT_CLEARANCE_BATCHOUTPUT_H

#include <QDialog>
#include <QButtonGroup>

namespace Ui {
    class CorrectClearanceBatchOutputWidget;
}

/**
 * 图形修正界面——EXCEL批量输出
 * @author 范翔
 * @version 1.0.0
 * @date 2015-10-07
 */
class CorrectClearanceBatchOutputWidget : public QDialog
{
    Q_OBJECT

public:
    explicit CorrectClearanceBatchOutputWidget(QWidget *parent = 0);
    
    ~CorrectClearanceBatchOutputWidget();

    void setMinUnit(float);

    void initProgressBar(long long start, long long end);

    void setProgressBar(long long current);

    void endProgressBar();

private slots:

    void exportAll();

signals:
    void exportAll(int interframe_mile_mm);

private:
    Ui::CorrectClearanceBatchOutputWidget *ui;

    QButtonGroup * group;

    /**
     * 里程折算系数，相邻帧间隔里程
     */
    double interframe_mile;
};

#endif // CORRECT_CLEARANCE_BATCHOUTPUT_H
