#ifndef LOADCOMPAREDATAWIDGET_H
#define LOADCOMPAREDATAWIDGET_H

#include <QDialog>
#include "clearance_item.h"

namespace Ui {
    class LoadCompareDataWidget;
}

/**
 * 加载断面对比数据
 *
 * @author 范翔
 * @version 1.0.0
 * @date 20150429
 */
class LoadCompareDataWidget : public QDialog
{
    Q_OBJECT

public:
    explicit LoadCompareDataWidget(QWidget *parent = 0);
    ~LoadCompareDataWidget();

    void setParentPath(QString path);

    void setData2(SectionData & data);

    void updateAllView();
    void updateView(int i);

    SectionData & getData1();

signals:
    void updateData1();

private slots:

    void openFile();

private:
    Ui::LoadCompareDataWidget *ui;

    SectionData data1;

    SectionData data2;

    QString tmpopendir;
};

#endif // LOADCOMPAREDATAWIDGET_H
