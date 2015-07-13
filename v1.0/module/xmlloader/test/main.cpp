#include <QtGui/QApplication>

#include "xmlfileloader.h"
#include "xmlnetworkfileloader.h"
#include "xmlsynthesisheightsfileloader.h"
#include "xmltaskfileloader.h"
#include "xmlrealtaskfileloader.h"
#include "xmlcheckedtaskfileloader.h"
#include "network_config_list.h"
#include "output_heights_list.h"
#include "plantask_list.h"
#include "checkedtask_list.h"

#include "xmlprojectfileloader.h"

#include <QDebug>
#include <QTextcodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*******************************/
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312")); //当QString变量中含有中文时，需要增加这两行代码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));


    /********计划任务配置*********/
    QString ffname = QObject::tr("兰青_20140121.plan");
    XMLTaskFileLoader * ff1 = new XMLTaskFileLoader(ffname);
    qDebug() << "save plan file" << ffname << ff1->saveFile();
    qDebug() << "load plan file" << ffname << ff1->loadFile();
    delete ff1;

    QList<PlanTask>::iterator planTaskIterator = PlanTaskList::getPlanTaskListInstance()->begin();
    while (planTaskIterator != PlanTaskList::getPlanTaskListInstance()->end())
    {
        qDebug() << planTaskIterator->tunnelnum << QString::fromLocal8Bit(planTaskIterator->tunnelname.c_str());
        planTaskIterator++;
    }

    /********校正文件配置*********/

    CheckedTunnelTaskModel* tmp;
    int i = 0, j = 0;
    for (i = 0; i < 5; i++)
    {
        tmp = new CheckedTunnelTaskModel();
        PlanTask plan;
        plan.datetime = "20140121";
        plan.linename = "西工大";
        plan.linenum = 2222;
        plan.tunnelnum = 2345;
        plan.tunnelname = "范翔计划";
        plan.doubleline = true;
        CalcuFileItem calcuitem;
        calcuitem.cal_filename_prefix = "asdf";
        calcuitem.cal_framecounter_begin = 200*j;
        calcuitem.cal_framecounter_end = 1234*j;
        calcuitem.cal_valid_frames_num = calcuitem.cal_framecounter_end - calcuitem.cal_framecounter_begin + 1;
        for (int t = 0; t < 2; t++,j++)
        {
            CheckedItem *ch = new CheckedItem();
            ch->filename_prefix = "aaaaa";
            ch->seqno = j+1;
            ch->start_frame_A1 = j;
            ch->end_frame_A1 = j+10000;
            ch->start_frame_A2 = j;
            ch->end_frame_A2 = j+10000;
            ch->start_frame_B1 = j;
            ch->end_frame_B1 = j+10000;
            ch->start_frame_B2 = j;
            ch->end_frame_B2 = j+10000;
            ch->start_frame_C1 = j;
            ch->end_frame_C1 = j+10000;
            ch->start_frame_C2 = j;
            ch->end_frame_C2 = j+10000;
            ch->start_frame_D1 = j;
            ch->end_frame_D1 = j+10000;
            ch->start_frame_D2 = j;
            ch->end_frame_D2 = j+10000;
            ch->start_frame_E1 = j;
            ch->end_frame_E1 = j+10000;
            ch->start_frame_E2 = j;
            ch->end_frame_E2 = j+10000;
            ch->start_frame_F1 = j;
            ch->end_frame_F1 = j+10000;
            ch->start_frame_F2 = j;
            ch->end_frame_F2 = j+10000;
            ch->start_frame_G1 = j;
            ch->end_frame_G1 = j+10000;
            ch->start_frame_G2 = j;
            ch->end_frame_G2 = j+10000;
            ch->start_frame_H1 = j;
            ch->end_frame_H1 = j+10000;
            ch->start_frame_H2 = j;
            ch->end_frame_H2 = j+10000;
            ch->start_frame_I1 = j;
            ch->end_frame_I1 = j+10000;
            ch->start_frame_I2 = j;
            ch->end_frame_I2 = j+10000;
            ch->start_frame_J1 = j;
            ch->end_frame_J1 = j+10000;
            ch->start_frame_J2 = j;
            ch->end_frame_J2 = j+10000;
            ch->start_frame_K1 = j;
            ch->end_frame_K1 = j+10000;
            ch->start_frame_K2 = j;
            ch->end_frame_K2 = j+10000;
            ch->start_frame_L1 = j;
            ch->end_frame_L1 = j+10000;
            ch->start_frame_L2 = j;
            ch->end_frame_L2 = j+10000;
            ch->start_frame_M1 = j;
            ch->end_frame_M1 = j+10000;
            ch->start_frame_M2 = j;
            ch->end_frame_M2 = j+10000;
            ch->start_frame_N1 = j;
            ch->end_frame_N1 = j+10000;
            ch->start_frame_N2 = j;
            ch->end_frame_N2 = j+10000;
            ch->start_frame_O1 = j;
            ch->end_frame_O1 = j+10000;
            ch->start_frame_O2 = j;
            ch->end_frame_O2 = j+10000;
            ch->start_frame_P1 = j;
            ch->end_frame_P1 = j+10000;
            ch->start_frame_P2 = j;
            ch->end_frame_P2 = j+10000;
            ch->start_frame_Q1 = j;
            ch->end_frame_Q1 = j+10000;
            ch->start_frame_Q2 = j;
            ch->end_frame_Q2 = j+10000;
            ch->start_frame_R1 = j;
            ch->end_frame_R1 = j+10000;
            ch->start_frame_R2 = j;
            ch->end_frame_R2 = j+10000;
            ch->tunnel_name = "范翔";
            ch->tunnel_id = 123;
            tmp->pushback(*ch);
        }
        tmp->setPlanTask(plan);
        tmp->setCalcuItem(calcuitem);
        CheckedTaskList::getCheckedTaskListInstance()->pushback(*tmp);
    }


    QString ffname3 = QObject::tr("兰青_20140122.checked");
    XMLCheckedTaskFileLoader * ff5 = new XMLCheckedTaskFileLoader(ffname3);
    qDebug() << "save checkedtask" << ff5->saveFile();

    // clear函数也会调用析构函数
    CheckedTaskList::getCheckedTaskListInstance()->clear();

    qDebug() << "load checkedtask" << ff5->loadFile();
    delete ff5;
    QList<CheckedTunnelTaskModel>::iterator checkedTaskIterator = CheckedTaskList::getCheckedTaskListInstance()->begin();
    while (checkedTaskIterator != CheckedTaskList::getCheckedTaskListInstance()->end())
    {
        qDebug() << checkedTaskIterator->planTask.tunnelnum << QString::fromLocal8Bit(checkedTaskIterator->planTask.tunnelname.c_str());
        long checkedstart;
        long checkedend;
        std::list<CheckedItem>::iterator it = checkedTaskIterator->getRealList()->begin();
        if (it != checkedTaskIterator->getRealList()->end())
        {
            checkedstart = it->start_frame_A1;
            checkedend = it->end_frame_A1;
            while (it != checkedTaskIterator->getRealList()->end())
            {
                if (it->start_frame_A1 < checkedstart)
                    checkedstart = it->start_frame_A1;
                if (it->end_frame_A1 > checkedend)
                    checkedend = it->end_frame_A1;

                qDebug() << QString::fromLocal8Bit(it->filename_prefix.c_str());
                ////////TODO/////////
                /*********************************/
                // 打开文件
                qDebug() << "open file:" << QString::fromLocal8Bit(it->filename_prefix.c_str());

                // 关闭文件
                qDebug() << "close file:" << QString::fromLocal8Bit(it->filename_prefix.c_str());
                /*********************************/
                it++;
            }
            qDebug() << checkedTaskIterator->getRealList()->size() << checkedstart << checkedend;
        }
        checkedTaskIterator++;
    }

    /********实际采集任务文件*********/
    QString ffname2 = QObject::tr("兰青_20140122.real");
    XMLRealTaskFileLoader * ff4 = new XMLRealTaskFileLoader(ffname2);
    qDebug() << "save real file" << ffname2 << ff4->saveFile();
    qDebug() << "load real file" << ffname2 << ff4->loadFile();
    delete ff4;

    /********从控网络及相机硬件配置*********/
    XMLNetworkFileLoader* ff2 = new XMLNetworkFileLoader("network_config.xml");
    qDebug() << "save network config file" << "network_config.xml"  << ff2->saveFile();
    qDebug() << "load network config file" << "network_config.xml" << ff2->loadFile();
    delete ff2;
    qDebug() << "ok";

    SlaveModel m = SlaveModel(3);
    qDebug() << Camerasnid::getCamerasnidInstance()->findSlave("10.13.29.210", m);

    qDebug() << "ok2";

    /********指定高度*********/
    //OutputHeightsList::getOutputHeightsListInstance()->resetHeights("output_heights.xml");
    XMLSynthesisHeightsFileLoader * ff3 = new XMLSynthesisHeightsFileLoader("output_heights.xml");
    qDebug() << "save network config file" << "output_heights.xml"<< ff3->saveFile();
    qDebug() << "load network config file" << "output_heights.xml" << ff3->loadFile();
    delete ff3;

    /********工程入口*********/
    ProjectModel projmodel1(1.0);
    projmodel1.setPlanFilename(QObject::tr("兰青_20140121.plan"));
    projmodel1.setRealFilename(QObject::tr("兰青_20140122.real"));
    projmodel1.setCheckedFilename(QObject::tr("兰青_20140122.checked"));
    projmodel1.setCreateDate("20140122");
    ProjectModel projmodel2(1.0);

    XMLProjectFileLoader * ff6 = new XMLProjectFileLoader(QObject::tr("兰青_20140122.proj"));
    qDebug() << "save project file" << QObject::tr("兰青_20140122.proj") << ff6->saveFile(projmodel1);
    qDebug() << "load project file" << QObject::tr("兰青_20140122.proj") << ff6->loadFile(projmodel2);
    qDebug() << projmodel2.getVersion() << projmodel2.getPlanFilename() << projmodel2.getRealFilename() << projmodel2.getCheckedFilename();

    return a.exec();
}
