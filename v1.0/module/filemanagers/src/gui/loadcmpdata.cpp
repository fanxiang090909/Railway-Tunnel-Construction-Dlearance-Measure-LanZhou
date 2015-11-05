#include "loadcmpdata.h"
#include "ui_loadcmpdata.h"

#include <QFileDialog>
#include <QTextStream>
#include <QDebug>

// 高度行高
#ifndef TABLEWIDGET_ROW_HEIGHT
#define TABLEWIDGET_ROW_HEIGHT 15
#endif

/**
 * 加载断面对比数据
 *
 * @author 范翔
 * @version 1.0.0
 * @date 20150429
 */
LoadCompareDataWidget::LoadCompareDataWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadCompareDataWidget)
{
    ui->setupUi(this);

    bool ret1 = data1.initMaps();
    bool ret2 = data2.initMaps();

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(openFile()));
}

LoadCompareDataWidget::~LoadCompareDataWidget()
{
    delete ui;
}

void LoadCompareDataWidget::setParentPath(QString newpath)
{
    tmpopendir = newpath;
}

void LoadCompareDataWidget::openFile()
{
    QString filename = QFileDialog::getOpenFileName(this, QObject::tr("载入单帧断面数据"), tmpopendir, "Document files (*.sec)");
    tmpopendir = QFileInfo(filename).absoluteFilePath();
    
    ui->secfilename->setText(filename);
    // Function that reads in ASCII file data

    QFile file;
    QString in;

    file.setFileName(filename);

    // Currently here for debugging purposes
    qDebug() << "Reading file: " << filename.toLocal8Bit().data() << endl;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream inStream(&file);
        
        // readfile
        
        in = inStream.readLine();

        int height;
        int left;
        int right;

        while(!(in.isNull()))
        {
            height = in.section("\t",0,0).toInt();
            left = in.section("\t",1,1).toInt();
            // Reads in fifth column: signal-to-noise ratio
            right = in.section("\t",2,2).toInt();
            
            data1.updateToMapVals(height, (float)left, (float)right);

            in = inStream.readLine();
        }

        file.close();

        qDebug() << "read_end" << endl;
    }
    // Do some proper debugging here at some point...
    else std::cout << "Problem reading file: " << filename.toLocal8Bit().data() << std::endl;

    emit updateData1();

    updateView(1);
}

SectionData & LoadCompareDataWidget::getData1()
{
    return data1;
}

void LoadCompareDataWidget::setData2(SectionData & data)
{
    std::map <int,item>::iterator it = data.getMaps().begin();
    while(it != data.getMaps().end())
    {
        std::pair<int,item> pair = (*it);
        data2.updateToMapVals(pair.first, pair.second.left, pair.second.right);
        it++;
    }

    // 更新显示
    updateAllView();
}

void LoadCompareDataWidget::updateAllView()
{
    updateView(2);
    
    // 清空
    int size = ui->tableWidget_3->rowCount();
    for (int j = 0; j < size; ++j)
        ui->tableWidget_3->removeRow(0);

    std::map <int,item>::iterator it1 = data1.getMaps().begin();
    std::map <int,item>::iterator it2 = data2.getMaps().begin();

    while(it1 != data1.getMaps().end())
    {
        std::pair<int,item> pair1 = (*it1);
        std::pair<int,item> pair2 = (*it2);

        ui->tableWidget_3->insertRow(0);
        ui->tableWidget_3->setItem(0, 0, new QTableWidgetItem(QString("%1").arg(pair1.first)));
        QTableWidgetItem * tmp1 = new QTableWidgetItem(QString("%1").arg((int)pair1.second.left - (int)pair2.second.left));
        if (abs(pair1.second.left - pair2.second.left) > 40 && abs(pair1.second.left - pair2.second.left) < 500)
            tmp1->setBackgroundColor(QColor(255,114,101));
        else if (abs(pair1.second.left - pair2.second.left) >= 500)
            tmp1->setBackgroundColor(QColor(70,130,180));
        else
            tmp1->setBackgroundColor(Qt::white);
        ui->tableWidget_3->setItem(0, 1, tmp1);

        QTableWidgetItem * tmp2 = new QTableWidgetItem(QString("%1").arg((int)pair1.second.right - (int)pair2.second.right));
        if (abs(pair1.second.right - pair2.second.right) > 40 && abs(pair1.second.right - pair2.second.right) < 500)
            tmp2->setBackgroundColor(QColor(255,114,101));
        else if (abs(pair1.second.right - pair2.second.right) >= 500)
            tmp2->setBackgroundColor(QColor(70,130,180));
        else
            tmp2->setBackgroundColor(Qt::white);
        ui->tableWidget_3->setItem(0, 2, tmp2);
 
        QTableWidgetItem * tmp3 = new QTableWidgetItem(QString("%1").arg((int)((pair1.second.left + pair1.second.right) - (pair2.second.left + pair2.second.right))));
        if (abs((pair1.second.left + pair1.second.right) - (pair2.second.left + pair2.second.right)) > 100 && abs((pair1.second.left + pair1.second.right) - (pair2.second.left + pair2.second.right)) < 1000)
            tmp3->setBackgroundColor(QColor(255,114,101));
        else if (abs((pair1.second.left + pair1.second.right) - (pair2.second.left + pair2.second.right)) >= 1000)
            tmp3->setBackgroundColor(QColor(70,130,180));
        else
            tmp3->setBackgroundColor(Qt::white);

        ui->tableWidget_3->setItem(0, 3, tmp3);
        ui->tableWidget_3->setRowHeight(0, TABLEWIDGET_ROW_HEIGHT);

        it1++;
        it2++;
    }
    ui->tableWidget_3->hideColumn(0);
    ui->tableWidget_3->resizeColumnsToContents();
}

void LoadCompareDataWidget::updateView(int i)
{
    QTableWidget * tmpwidget;
    SectionData * data;
    if (i == 1)
    {
        tmpwidget = ui->tableWidget;
        data = &data1;
    }
    else
    {
        tmpwidget = ui->tableWidget_2;
        data = &data2;
    }

    // 清空
    int size = tmpwidget->rowCount();
    for (int j = 0; j < size; ++j)
        tmpwidget->removeRow(0);

    // 插入
    std::map <int,item>::iterator it = data->getMaps().begin();
    int k=0;

    while(it != data->getMaps().end())
    {
        std::pair<int,item> pair = (*it);
        tmpwidget->insertRow(0);
        tmpwidget->setItem(0, 0, new QTableWidgetItem(QString("%1").arg(pair.first)));
        tmpwidget->setItem(0, 1, new QTableWidgetItem(QString("%1").arg(pair.second.left)));
        tmpwidget->setItem(0, 2, new QTableWidgetItem(QString("%1").arg(pair.second.right)));
        tmpwidget->setItem(0, 3, new QTableWidgetItem(QString("%1").arg(pair.second.left + pair.second.right)));
        tmpwidget->setRowHeight(0, TABLEWIDGET_ROW_HEIGHT);

        //qDebug() << pair.first << pair.second.left << pair.second.right;
        it++;
    }

    if (i == 2)
        tmpwidget->hideColumn(0);

    tmpwidget->resizeColumnsToContents();

    this->repaint();
}