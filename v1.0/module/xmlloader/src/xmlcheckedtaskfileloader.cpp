#include "xmlcheckedtaskfileloader.h"
#include <QDebug>

#include "plantask_list.h"
#include "xmltaskfileloader.h"
#include "plantask.h"
#include "plantask_list.h"
#include "checkedtask.h"
#include "checkedtask_list.h"
#include <QTextCodec>

/**
 * 这个是将real_task_时间.xml文件解析出来，即有计划隧道数据，
 * 也有实际隧道数据，和计算时候的数据存入listtunnelcheck
 * @author 熊雪 范翔
 * @date 20140122
 * @version 1.0.0
 */
XMLCheckedTaskFileLoader::XMLCheckedTaskFileLoader(QString filename):XMLFileLoader(filename)
{
    setFilename(filename);
}

XMLCheckedTaskFileLoader::XMLCheckedTaskFileLoader()
{

}

XMLCheckedTaskFileLoader::~XMLCheckedTaskFileLoader()
{

}

bool XMLCheckedTaskFileLoader::loadFile(CheckedTaskList & list)
{
    mutex.lock();
    list.clear();
    qDebug() << "checked file loader" << getFilename();
    // 接收中文字符
    TiXmlDocument *my_tunnelcheck = new TiXmlDocument();
    // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
    QByteArray tempstr = getFilename().toLocal8Bit();
    bool oknotfile = my_tunnelcheck->LoadFile(tempstr.constData());
    if(!oknotfile)
    {
        mutex.unlock();
        return false;
    }
    TiXmlElement *my_root = my_tunnelcheck->RootElement();
    // qDebug()<<my_root->Value();
    TiXmlElement *my_tasklist = my_root->FirstChildElement();
    // qDebug()<<my_tasklist->Value();
    CheckedTunnelTaskModel *checkedTask;
    CheckedItem *tempitem;
    PlanTask planTask;
    CalcuFileItem calcuitem;
    int tmpint;//临时换成bool
    const char* temp;
    while(my_tasklist)
    {
        //TiXmlAttribute *my_tasklistattr1 =my_tasklist->FirstAttribute();
        //temp =my_tasklistattr1->Value();
        //_int64 taskid1 = atoi(temp);
        //qDebug()<<my_tasklistattr1->Name()<<my_tasklistattr1->Value()<<taskid1.c_str();//taskid xx xx
        //TiXmlAttribute *my_tasklistattr2 =my_tasklistattr1->Next();
        TiXmlAttribute *my_tasklistattr2 =my_tasklist->FirstAttribute();
        temp =my_tasklistattr2->Value();
        _int64 linenum1 = atoi(temp);
        /*出错情况：二进制“<<”: 没有找到接受“QDebug”类型的左操作数的运算符(或没有可接受的转换),
        ***解决方法是将string类型的值用qDebug()<<输出时，要在变量后面加上c_str,
        如qDebug()<<linenum1.c_str();*/
        //qDebug()<<QObject::tr(my_tasklistattr2->Name())<<my_tasklistattr2->Value()<<linenum1.c_str();//"线路序号" 54 54
        TiXmlAttribute *my_tasklistattr3 =my_tasklistattr2->Next();
        temp =my_tasklistattr3->Value();
        string linename1(temp);
        TiXmlAttribute *my_tasklistattr4 =my_tasklistattr3->Next();
        temp =my_tasklistattr4->Value();
        int tunnelnum1 =atoi(temp);
        TiXmlAttribute *my_tasklistattr5 =my_tasklistattr4->Next();
        temp =my_tasklistattr5->Value();
        string tunnelname1(temp);
        TiXmlAttribute *my_tasklistattr6 =my_tasklistattr5->Next();
        temp =my_tasklistattr6->Value();
        _int64 startdistance1 = atol(temp);
        TiXmlAttribute *my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        _int64 enddistance1 = atol(temp);
        TiXmlAttribute *my_tasklistattr8 =my_tasklistattr7->Next();
        temp =my_tasklistattr8->Value();
        tmpint = atoi(temp);
        bool traindirection1;
        if (tmpint == 0)
            traindirection1 = false;
        else
            traindirection1 = true;
        TiXmlAttribute *my_tasklistattr9 =my_tasklistattr8->Next();
        temp =my_tasklistattr9->Value();
        int newline1 = atoi(temp);
        TiXmlAttribute *my_tasklistattr10 =my_tasklistattr9->Next();
        temp =my_tasklistattr10->Value();
        int linetype1 = atoi(temp);
        TiXmlAttribute *my_tasklistattr11 =my_tasklistattr10->Next();
        temp =my_tasklistattr11->Value();
        tmpint = atoi(temp);
        bool isdoubleline;
        if (tmpint == 0)
            isdoubleline = false;
        else
            isdoubleline = true;
        TiXmlAttribute *my_tasklistattr12 =my_tasklistattr11->Next();
        temp =my_tasklistattr12->Value();
        tmpint = atoi(temp);
        bool isdownlink;
        if (tmpint == 0)
            isdownlink = false;
        else
            isdownlink = true;
        TiXmlAttribute *my_tasklistattr13 =my_tasklistattr12->Next();
        temp =my_tasklistattr13->Value();
        double pulsepermeter1 =atof(temp);
        TiXmlAttribute *my_tasklistattr14 =my_tasklistattr13->Next();
        temp =my_tasklistattr14->Value();
        tmpint = atoi(temp);
        bool isnormal;
        if (tmpint == 0)
            isnormal = false;
        else
            isnormal = true;        
        TiXmlAttribute *my_tasklistattr15 =my_tasklistattr14->Next();
        temp =my_tasklistattr15->Value();
        string datetime1(temp);

        //planTask.taskid=taskid1;
        planTask.linenum =linenum1;
        planTask.linename =linename1;
        planTask.tunnelnum =tunnelnum1;
        planTask.tunnelname =tunnelname1;
        planTask.startdistance =startdistance1;
        planTask.enddistance =enddistance1;
        planTask.traindirection =traindirection1;
        planTask.newline =newline1;
        planTask.linetype =linetype1;
        planTask.doubleline = isdoubleline;
        planTask.downstream = isdownlink;
        planTask.pulsepermeter =pulsepermeter1;
        planTask.isnormal =isnormal;
        planTask.datetime =datetime1;

        TiXmlElement *my_reallist =my_tasklist->FirstChildElement();
        // qDebug()<<my_reallist->Value();//real_list
        TiXmlElement *my_reallist_realfile =my_reallist->FirstChildElement();

        // 新建list项
        checkedTask = new CheckedTunnelTaskModel();

        while(my_reallist_realfile)
        {
            //qDebug()<<my_reallist_realfile->Value();//realfile
            TiXmlAttribute *my_seqno =my_reallist_realfile->FirstAttribute();
            //qDebug()<<my_seqno->Name()<<my_seqno->Value();//seqno x
            temp =my_seqno->Value();
            int seqno1 = atoi(temp);
            TiXmlAttribute *my_filename_prefix =my_seqno->Next();
            temp =my_filename_prefix->Value();
            string filename_prefix1(temp);
            // qDebug()<<my_filename_prefix->Name()<<my_filename_prefix->Value();//filename_prefix xxx
            TiXmlAttribute *my_tunnelid =my_filename_prefix->Next();
            temp =my_tunnelid->Value();
            int tunnelid2 = atoi(temp);
            // qDebug()<<my_filename_prefix->Name()<<my_filename_prefix->Value();//filename_prefix xxx
            TiXmlAttribute *my_tunnelname =my_tunnelid->Next();
            temp =my_tunnelname->Value();
            string tunnelname2(temp);
            // qDebug()<<my_filename_prefix->Name()<<my_filename_prefix->Value();//filename_prefix xxx
            TiXmlAttribute *my_valid_frames_num =my_tunnelname->Next();
            temp =my_valid_frames_num->Value();
            int valid_frames_num1 = atoi(temp);
            //qDebug()<<my_valid_frames_num->Name()<<my_valid_frames_num->Value();//valid_frames_num x
            TiXmlAttribute *my_status =my_valid_frames_num->Next();
            temp =my_status->Value();
            int status1 = atoi(temp);
            // qDebug()<<my_status->Name()<<my_status->Value();//status x
            TiXmlAttribute *my_collectime =my_status->Next();
            temp =my_collectime->Value();
            string collectime1(temp);

            TiXmlAttribute *isvalidxml =my_collectime->Next();//解析出车厢方向
            temp =isvalidxml->Value();
		    tmpint = atoi(temp);
            bool isvalid;
            if (tmpint == 0)
                isvalid = false;
            else
                isvalid = true;

            tempitem = new CheckedItem();
            tempitem->seqno = seqno1;
            tempitem->filename_prefix =filename_prefix1;
            tempitem->tunnel_id=tunnelid2;
            tempitem->tunnel_name=tunnelname2;
            tempitem->valid_frames_num =valid_frames_num1;
            tempitem->status =status1;
            tempitem->collecttime = collectime1;
            tempitem->isvalid = isvalid;

            TiXmlAttribute *my_tasklistattr6 =isvalidxml->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_mile=atof(temp);
            TiXmlAttribute *my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_mile=atof(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_master=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_master=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_A1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_A1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_A2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_A2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_B1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_B1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_B2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_B2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_C1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_C1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_C2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_C2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_D1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_D1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_D2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_D2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_E1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_E1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_E2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_E2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_F1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_F1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_F2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_F2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_G1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_G1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_G2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_G2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_H1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_H1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_H2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_H2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_I1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_I1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_I2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_I2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_J1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_J1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_J2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_J2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_K1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_K1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_K2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_K2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_L1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_L1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_L2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_L2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_M1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_M1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_M2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_M2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_N1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_N1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_N2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_N2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_O1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_O1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_O2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_O2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_P1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_P1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_P2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_P2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_Q1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_Q1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_Q2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_Q2=atol(temp);

            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_R1=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_R1=atol(temp);
            my_tasklistattr6 =my_tasklistattr7->Next();
            temp =my_tasklistattr6->Value();
            tempitem->start_frame_R2=atol(temp);
            my_tasklistattr7 =my_tasklistattr6->Next();
            temp =my_tasklistattr7->Value();
            tempitem->end_frame_R2=atol(temp);

            // 插入校对隧道list项
            checkedTask->pushback((*tempitem));

            my_reallist_realfile=my_reallist_realfile->NextSiblingElement();
        }

        TiXmlElement *my_calculist =my_reallist->NextSiblingElement();
        //qDebug()<<my_calculist->Value();//calcufilelist
        TiXmlElement *my_calcu_realfile =my_calculist->FirstChildElement();
        // qDebug()<<my_calcu_realfile->Value();//calcufile
        TiXmlAttribute *my_calcu_filename_prefix =my_calcu_realfile->FirstAttribute();
        temp=my_calcu_filename_prefix->Value();
        string cal_filename_prefix1(temp);
        //qDebug()<<my_calcu_filename_prefix->Name()<<my_calcu_filename_prefix->Value();//filename_prefix xxx
        TiXmlAttribute *my_calcu_framecounter_begin=my_calcu_filename_prefix->Next();
        temp =my_calcu_framecounter_begin->Value();
        _int64 cal_framecounter_begin1 = atol(temp);
        // qDebug()<<my_calcu_framecounter_begin->Name()<<my_calcu_framecounter_begin->Value();//framecounter_begin x
        TiXmlAttribute *my_calcu_framecounter_end=my_calcu_framecounter_begin->Next();
        temp =my_calcu_framecounter_end->Value();
        _int64 cal_framecounter_end1 = atol(temp);
        // qDebug()<<my_calcu_framecounter_begin->Name()<<my_calcu_framecounter_begin->Value();//framecounter_begin x

        TiXmlAttribute *my_calcu_valid_frames_num =my_calcu_framecounter_end->Next();
        temp =my_calcu_valid_frames_num->Value();
        _int64 cal_valid_frames_num1 = atol(temp);

        TiXmlAttribute *my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        string fusetime1(temp);

        // 计算状态
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.has_backup_calc_A = atoi(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.has_backup_calc_B = atoi(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.has_backup_calc_C = atoi(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.has_backup_calc_D = atoi(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.has_backup_calc_E = atoi(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.has_backup_calc_F = atoi(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.has_backup_calc_G = atoi(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.has_backup_calc_H = atoi(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.has_backup_calc_I = atoi(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.has_backup_calc_J = atoi(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.has_backup_calc_K = atoi(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.has_backup_calc_L = atoi(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.has_backup_calc_M = atoi(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.has_backup_calc_N = atoi(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.has_backup_calc_O = atoi(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.has_backup_calc_P = atoi(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.has_backup_calc_Q = atoi(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.has_backup_calc_R = atoi(temp);

        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.has_backup_calc_RT = atoi(temp);

        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.has_backup_fusecalc = atoi(temp);

        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.backup_calc_pos_A = atol(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.backup_calc_pos_B = atol(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.backup_calc_pos_C = atol(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.backup_calc_pos_D = atol(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.backup_calc_pos_E = atol(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.backup_calc_pos_F = atol(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.backup_calc_pos_G = atol(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.backup_calc_pos_H = atol(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.backup_calc_pos_I = atol(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.backup_calc_pos_J = atol(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.backup_calc_pos_K = atol(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.backup_calc_pos_L = atol(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.backup_calc_pos_M = atol(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.backup_calc_pos_N = atol(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.backup_calc_pos_O = atol(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.backup_calc_pos_P = atol(temp);
        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.backup_calc_pos_Q = atol(temp);
        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_fusetime->Value();
        calcuitem.backup_calc_pos_R = atol(temp);

        my_calcu_valid_frames_num =my_fusetime->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.backup_calc_pos_RT = atol(temp);

        my_fusetime =my_calcu_valid_frames_num->Next();
        temp =my_calcu_valid_frames_num->Value();
        calcuitem.backup_calc_pos_fuse = atol(temp);

        calcuitem.cal_filename_prefix=cal_filename_prefix1;
        calcuitem.cal_framecounter_begin=cal_framecounter_begin1;
        calcuitem.cal_framecounter_end=cal_framecounter_end1;
        calcuitem.cal_valid_frames_num =cal_valid_frames_num1;
        calcuitem.fuse_calculte_time = fusetime1;

        checkedTask->setPlanTask(planTask);
        checkedTask->setCalcuItem(calcuitem);
        //qDebug() << "checked" << QString::fromLocal8Bit(planTask.tunnelname.c_str()) << ",item num:" << checkedTask->getRealList()->size();
        list.pushback((*checkedTask));

        my_tasklist=my_tasklist->NextSiblingElement();
    }
    qDebug() << "checked task list length:" << list.list()->length();
    list.showList();
    mutex.unlock();
    return true;
}

bool XMLCheckedTaskFileLoader::saveFile(CheckedTaskList & list)
{
    mutex.lock();
    TiXmlDocument *my_tunnelcheck =new TiXmlDocument();
    TiXmlDeclaration dec("1.0","gb2312","yes");
    my_tunnelcheck->InsertEndChild(dec);
    TiXmlElement * my_taskreal = new TiXmlElement("task_real");
    my_tunnelcheck->LinkEndChild(my_taskreal);
    /*利用解析计划隧道文件task_currentdatetime.xml文件生成的listtask中的值生成对应的计划隧道文件*/
    qDebug() << "listtask_length" << list.list()->length();
    PlanTask task;
    CalcuFileItem calcuitem;
    CheckedTunnelTaskModel *tmp;
    for(int i=0; i < list.list()->length(); i++)
    {
        // 将const指针，强制转换为非const指针，at函数返回const，但const成员无法使用->begin()等方法
        tmp = const_cast<CheckedTunnelTaskModel *> (&(list.list()->at(i)));

        TiXmlElement * my_taskzi1= new TiXmlElement("plan_task");

        task = tmp->planTask;

        my_taskzi1->SetAttribute("line_id",task.linenum);
        my_taskzi1->SetAttribute("line_name",task.linename.c_str());
        my_taskzi1->SetAttribute("tunnel_id",task.tunnelnum);
        my_taskzi1->SetAttribute("tunnel_name",task.tunnelname.c_str());
        my_taskzi1->SetAttribute("start_mileage",task.startdistance);
        my_taskzi1->SetAttribute("end_mileage",task.enddistance);
        my_taskzi1->SetAttribute("carriage_direction",task.traindirection);
        my_taskzi1->SetAttribute("is_newline",task.newline);
        my_taskzi1->SetAttribute("line_type",task.linetype);
        my_taskzi1->SetAttribute("is_double_line",task.doubleline);
        my_taskzi1->SetAttribute("is_downlink",task.downstream);
        my_taskzi1->SetDoubleAttribute("interval_pluse_num",task.pulsepermeter);
        my_taskzi1->SetAttribute("is_normal",task.isnormal);
        my_taskzi1->SetAttribute("date",task.datetime.c_str());

        //不能将参数 2 从“const std::string”转换为“const char *”,要在后面加上c_str()转换为const char*.
        my_taskreal->LinkEndChild(my_taskzi1);
        
        TiXmlElement* my_reallist1 = new TiXmlElement("checked_list");
        my_taskzi1->LinkEndChild(my_reallist1);

        std::list<CheckedItem>::iterator it = tmp->begin();
        while (it != tmp->end())
        {
            TiXmlElement *my_realfile2= new TiXmlElement("checked_item");
            my_realfile2->SetAttribute("seqno",it->seqno);
            my_realfile2->SetAttribute("filename_prefix",it->filename_prefix.c_str());
            my_realfile2->SetAttribute("tunnel_id",it->tunnel_id);
            my_realfile2->SetAttribute("tunnel_name",it->tunnel_name.c_str());
            my_realfile2->SetAttribute("valid_frames_num",it->valid_frames_num);
            my_realfile2->SetAttribute("status",it->status);
            my_realfile2->SetAttribute("collect_time",it->collecttime.c_str());
            my_realfile2->SetAttribute("is_valid", it->isvalid);

            // 主控记录的起止帧号
            my_realfile2->SetDoubleAttribute("start_mile", it->start_mile);
            my_realfile2->SetDoubleAttribute("end_mile", it->end_mile);
            my_realfile2->SetAttribute("start_frame_master", it->start_frame_master);
            my_realfile2->SetAttribute("end_frame_master", it->end_frame_master);

            // 起止帧号
            my_realfile2->SetAttribute("start_frame_A1", it->start_frame_A1);
            my_realfile2->SetAttribute("end_frame_A1", it->end_frame_A1);
            my_realfile2->SetAttribute("start_frame_A2", it->start_frame_A2);
            my_realfile2->SetAttribute("end_frame_A2", it->end_frame_A2);
            my_realfile2->SetAttribute("start_frame_B1", it->start_frame_B1);
            my_realfile2->SetAttribute("end_frame_B1", it->end_frame_B1);
            my_realfile2->SetAttribute("start_frame_B2", it->start_frame_B2);
            my_realfile2->SetAttribute("end_frame_B2", it->end_frame_B2);
            my_realfile2->SetAttribute("start_frame_C1", it->start_frame_C1);
            my_realfile2->SetAttribute("end_frame_C1", it->end_frame_C1);
            my_realfile2->SetAttribute("start_frame_C2", it->start_frame_C2);
            my_realfile2->SetAttribute("end_frame_C2", it->end_frame_C2);
            my_realfile2->SetAttribute("start_frame_D1", it->start_frame_D1);
            my_realfile2->SetAttribute("end_frame_D1", it->end_frame_D1);
            my_realfile2->SetAttribute("start_frame_D2", it->start_frame_D2);
            my_realfile2->SetAttribute("end_frame_D2", it->end_frame_D2);
            my_realfile2->SetAttribute("start_frame_E1", it->start_frame_E1);
            my_realfile2->SetAttribute("end_frame_E1", it->end_frame_E1);
            my_realfile2->SetAttribute("start_frame_E2", it->start_frame_E2);
            my_realfile2->SetAttribute("end_frame_E2", it->end_frame_E2);
            my_realfile2->SetAttribute("start_frame_F1", it->start_frame_F1);
            my_realfile2->SetAttribute("end_frame_F1", it->end_frame_F1);
            my_realfile2->SetAttribute("start_frame_F2", it->start_frame_F2);
            my_realfile2->SetAttribute("end_frame_F2", it->end_frame_F2);
            my_realfile2->SetAttribute("start_frame_G1", it->start_frame_G1);
            my_realfile2->SetAttribute("end_frame_G1", it->end_frame_G1);
            my_realfile2->SetAttribute("start_frame_G2", it->start_frame_G2);
            my_realfile2->SetAttribute("end_frame_G2", it->end_frame_G2);
            my_realfile2->SetAttribute("start_frame_H1", it->start_frame_H1);
            my_realfile2->SetAttribute("end_frame_H1", it->end_frame_H1);
            my_realfile2->SetAttribute("start_frame_H2", it->start_frame_H2);
            my_realfile2->SetAttribute("end_frame_H2", it->end_frame_H2);
            my_realfile2->SetAttribute("start_frame_I1", it->start_frame_I1);
            my_realfile2->SetAttribute("end_frame_I1", it->end_frame_I1);
            my_realfile2->SetAttribute("start_frame_I2", it->start_frame_I2);
            my_realfile2->SetAttribute("end_frame_I2", it->end_frame_I2);
            my_realfile2->SetAttribute("start_frame_J1", it->start_frame_J1);
            my_realfile2->SetAttribute("end_frame_J1", it->end_frame_J1);
            my_realfile2->SetAttribute("start_frame_J2", it->start_frame_J2);
            my_realfile2->SetAttribute("end_frame_J2", it->end_frame_J2);
            my_realfile2->SetAttribute("start_frame_K1", it->start_frame_K1);
            my_realfile2->SetAttribute("end_frame_K1", it->end_frame_K1);
            my_realfile2->SetAttribute("start_frame_K2", it->start_frame_K2);
            my_realfile2->SetAttribute("end_frame_K2", it->end_frame_K2);
            my_realfile2->SetAttribute("start_frame_L1", it->start_frame_L1);
            my_realfile2->SetAttribute("end_frame_L1", it->end_frame_L1);
            my_realfile2->SetAttribute("start_frame_L2", it->start_frame_L2);
            my_realfile2->SetAttribute("end_frame_L2", it->end_frame_L2);
            my_realfile2->SetAttribute("start_frame_M1", it->start_frame_M1);
            my_realfile2->SetAttribute("end_frame_M1", it->end_frame_M1);
            my_realfile2->SetAttribute("start_frame_M2", it->start_frame_M2);
            my_realfile2->SetAttribute("end_frame_M2", it->end_frame_M2);
            my_realfile2->SetAttribute("start_frame_N1", it->start_frame_N1);
            my_realfile2->SetAttribute("end_frame_N1", it->end_frame_N1);
            my_realfile2->SetAttribute("start_frame_N2", it->start_frame_N2);
            my_realfile2->SetAttribute("end_frame_N2", it->end_frame_N2);
            my_realfile2->SetAttribute("start_frame_O1", it->start_frame_O1);
            my_realfile2->SetAttribute("end_frame_O1", it->end_frame_O1);
            my_realfile2->SetAttribute("start_frame_O2", it->start_frame_O2);
            my_realfile2->SetAttribute("end_frame_O2", it->end_frame_O2);
            my_realfile2->SetAttribute("start_frame_P1", it->start_frame_P1);
            my_realfile2->SetAttribute("end_frame_P1", it->end_frame_P1);
            my_realfile2->SetAttribute("start_frame_P2", it->start_frame_P2);
            my_realfile2->SetAttribute("end_frame_P2", it->end_frame_P2);
            my_realfile2->SetAttribute("start_frame_Q1", it->start_frame_Q1);
            my_realfile2->SetAttribute("end_frame_Q1", it->end_frame_Q1);
            my_realfile2->SetAttribute("start_frame_Q2", it->start_frame_Q2);
            my_realfile2->SetAttribute("end_frame_Q2", it->end_frame_Q2);
            my_realfile2->SetAttribute("start_frame_R1", it->start_frame_R1);
            my_realfile2->SetAttribute("end_frame_R1", it->end_frame_R1);
            my_realfile2->SetAttribute("start_frame_R2", it->start_frame_R2);
            my_realfile2->SetAttribute("end_frame_R2", it->end_frame_R2);

            // 采集文件的备份进度记录
            my_realfile2->SetAttribute("has_backup_A1", it->has_backup_A1);
            my_realfile2->SetAttribute("has_backup_A2", it->has_backup_A2);
            my_realfile2->SetAttribute("has_backup_B1", it->has_backup_B1);
            my_realfile2->SetAttribute("has_backup_B2", it->has_backup_B2);
            my_realfile2->SetAttribute("has_backup_C1", it->has_backup_C1);
            my_realfile2->SetAttribute("has_backup_C2", it->has_backup_C2);
            my_realfile2->SetAttribute("has_backup_D1", it->has_backup_D1);
            my_realfile2->SetAttribute("has_backup_D2", it->has_backup_D2);
            my_realfile2->SetAttribute("has_backup_E1", it->has_backup_E1);
            my_realfile2->SetAttribute("has_backup_E2", it->has_backup_E2);
            my_realfile2->SetAttribute("has_backup_F1", it->has_backup_F1);
            my_realfile2->SetAttribute("has_backup_F2", it->has_backup_F2);
            my_realfile2->SetAttribute("has_backup_G1", it->has_backup_G1);
            my_realfile2->SetAttribute("has_backup_G2", it->has_backup_G2);
            my_realfile2->SetAttribute("has_backup_H1", it->has_backup_H1);
            my_realfile2->SetAttribute("has_backup_H2", it->has_backup_H2);
            my_realfile2->SetAttribute("has_backup_I1", it->has_backup_I1);
            my_realfile2->SetAttribute("has_backup_I2", it->has_backup_I2);
            my_realfile2->SetAttribute("has_backup_J1", it->has_backup_J1);
            my_realfile2->SetAttribute("has_backup_J2", it->has_backup_J2);
            my_realfile2->SetAttribute("has_backup_K1", it->has_backup_K1);
            my_realfile2->SetAttribute("has_backup_K2", it->has_backup_K2);
            my_realfile2->SetAttribute("has_backup_L1", it->has_backup_L1);
            my_realfile2->SetAttribute("has_backup_L2", it->has_backup_L2);
            my_realfile2->SetAttribute("has_backup_M1", it->has_backup_M1);
            my_realfile2->SetAttribute("has_backup_M2", it->has_backup_M2);
            my_realfile2->SetAttribute("has_backup_N1", it->has_backup_N1);
            my_realfile2->SetAttribute("has_backup_N2", it->has_backup_N2);
            my_realfile2->SetAttribute("has_backup_O1", it->has_backup_O1);
            my_realfile2->SetAttribute("has_backup_O2", it->has_backup_O2);
            my_realfile2->SetAttribute("has_backup_P1", it->has_backup_P1);
            my_realfile2->SetAttribute("has_backup_P2", it->has_backup_P2);
            my_realfile2->SetAttribute("has_backup_Q1", it->has_backup_Q1);
            my_realfile2->SetAttribute("has_backup_Q2", it->has_backup_Q2);
            my_realfile2->SetAttribute("has_backup_R1", it->has_backup_R1);
            my_realfile2->SetAttribute("has_backup_R2", it->has_backup_R2);

            // 采集文件的备份中断位置记录
            my_realfile2->SetAttribute("backup_pos_A1", it->backup_pos_A1);
            my_realfile2->SetAttribute("backup_pos_A2", it->backup_pos_A2);
            my_realfile2->SetAttribute("backup_pos_B1", it->backup_pos_B1);
            my_realfile2->SetAttribute("backup_pos_B2", it->backup_pos_B2);
            my_realfile2->SetAttribute("backup_pos_C1", it->backup_pos_C1);
            my_realfile2->SetAttribute("backup_pos_C2", it->backup_pos_C2);
            my_realfile2->SetAttribute("backup_pos_D1", it->backup_pos_D1);
            my_realfile2->SetAttribute("backup_pos_D2", it->backup_pos_D2);
            my_realfile2->SetAttribute("backup_pos_E1", it->backup_pos_E1);
            my_realfile2->SetAttribute("backup_pos_E2", it->backup_pos_E2);
            my_realfile2->SetAttribute("backup_pos_F1", it->backup_pos_F1);
            my_realfile2->SetAttribute("backup_pos_F2", it->backup_pos_F2);
            my_realfile2->SetAttribute("backup_pos_G1", it->backup_pos_G1);
            my_realfile2->SetAttribute("backup_pos_G2", it->backup_pos_G2);
            my_realfile2->SetAttribute("backup_pos_H1", it->backup_pos_H1);
            my_realfile2->SetAttribute("backup_pos_H2", it->backup_pos_H2);
            my_realfile2->SetAttribute("backup_pos_I1", it->backup_pos_I1);
            my_realfile2->SetAttribute("backup_pos_I2", it->backup_pos_I2);
            my_realfile2->SetAttribute("backup_pos_J1", it->backup_pos_J1);
            my_realfile2->SetAttribute("backup_pos_J2", it->backup_pos_J2);
            my_realfile2->SetAttribute("backup_pos_K1", it->backup_pos_K1);
            my_realfile2->SetAttribute("backup_pos_K2", it->backup_pos_K2);
            my_realfile2->SetAttribute("backup_pos_L1", it->backup_pos_L1);
            my_realfile2->SetAttribute("backup_pos_L2", it->backup_pos_L2);
            my_realfile2->SetAttribute("backup_pos_M1", it->backup_pos_M1);
            my_realfile2->SetAttribute("backup_pos_M2", it->backup_pos_M2);
            my_realfile2->SetAttribute("backup_pos_N1", it->backup_pos_N1);
            my_realfile2->SetAttribute("backup_pos_N2", it->backup_pos_N2);
            my_realfile2->SetAttribute("backup_pos_O1", it->backup_pos_O1);
            my_realfile2->SetAttribute("backup_pos_O2", it->backup_pos_O2);
            my_realfile2->SetAttribute("backup_pos_P1", it->backup_pos_P1);
            my_realfile2->SetAttribute("backup_pos_P2", it->backup_pos_P2);
            my_realfile2->SetAttribute("backup_pos_Q1", it->backup_pos_Q1);
            my_realfile2->SetAttribute("backup_pos_Q2", it->backup_pos_Q2);
            my_realfile2->SetAttribute("backup_pos_R1", it->backup_pos_R1);
            my_realfile2->SetAttribute("backup_pos_R2", it->backup_pos_R2);

            my_reallist1->LinkEndChild(my_realfile2);
            it++;
        }

        TiXmlElement * my_calcufile1 = new TiXmlElement("calcufilelist");
        my_taskzi1->LinkEndChild(my_calcufile1);

        calcuitem = tmp->calcuItem;

        TiXmlElement *my_calcufile11 = new TiXmlElement("calcufile");
        my_calcufile11->SetAttribute("filename_prefix",calcuitem.cal_filename_prefix.c_str());
        my_calcufile11->SetAttribute("framecounter_begin",calcuitem.cal_framecounter_begin);
        my_calcufile11->SetAttribute("framecounter_end",calcuitem.cal_framecounter_end);
        my_calcufile11->SetAttribute("valid_frames_num",calcuitem.cal_valid_frames_num);
        my_calcufile11->SetAttribute("calculate_fuse_time",calcuitem.fuse_calculte_time.c_str());
        
        // 计算进度记录
        my_calcufile11->SetAttribute("has_backup_calc_A", calcuitem.has_backup_calc_A);
        my_calcufile11->SetAttribute("has_backup_calc_B", calcuitem.has_backup_calc_B);
        my_calcufile11->SetAttribute("has_backup_calc_C", calcuitem.has_backup_calc_C);
        my_calcufile11->SetAttribute("has_backup_calc_D", calcuitem.has_backup_calc_D);
        my_calcufile11->SetAttribute("has_backup_calc_E", calcuitem.has_backup_calc_E);
        my_calcufile11->SetAttribute("has_backup_calc_F", calcuitem.has_backup_calc_F);
        my_calcufile11->SetAttribute("has_backup_calc_G", calcuitem.has_backup_calc_G);
        my_calcufile11->SetAttribute("has_backup_calc_H", calcuitem.has_backup_calc_H);
        my_calcufile11->SetAttribute("has_backup_calc_I", calcuitem.has_backup_calc_I);
        my_calcufile11->SetAttribute("has_backup_calc_J", calcuitem.has_backup_calc_J);
        my_calcufile11->SetAttribute("has_backup_calc_K", calcuitem.has_backup_calc_K);
        my_calcufile11->SetAttribute("has_backup_calc_L", calcuitem.has_backup_calc_L);
        my_calcufile11->SetAttribute("has_backup_calc_M", calcuitem.has_backup_calc_M);
        my_calcufile11->SetAttribute("has_backup_calc_N", calcuitem.has_backup_calc_N);
        my_calcufile11->SetAttribute("has_backup_calc_O", calcuitem.has_backup_calc_O);
        my_calcufile11->SetAttribute("has_backup_calc_P", calcuitem.has_backup_calc_P);
        my_calcufile11->SetAttribute("has_backup_calc_Q", calcuitem.has_backup_calc_Q);
        my_calcufile11->SetAttribute("has_backup_calc_R", calcuitem.has_backup_calc_R);
        my_calcufile11->SetAttribute("has_backup_calc_RT", calcuitem.has_backup_calc_RT);
        my_calcufile11->SetAttribute("has_backup_calc_fuse", calcuitem.has_backup_fusecalc);

        // 计算进度记录，暂停位置记录
        my_calcufile11->SetAttribute("backup_calc_pos_A", calcuitem.backup_calc_pos_A);
        my_calcufile11->SetAttribute("backup_calc_pos_B", calcuitem.backup_calc_pos_B);
        my_calcufile11->SetAttribute("backup_calc_pos_C", calcuitem.backup_calc_pos_C);
        my_calcufile11->SetAttribute("backup_calc_pos_D", calcuitem.backup_calc_pos_D);
        my_calcufile11->SetAttribute("backup_calc_pos_E", calcuitem.backup_calc_pos_E);
        my_calcufile11->SetAttribute("backup_calc_pos_F", calcuitem.backup_calc_pos_F);
        my_calcufile11->SetAttribute("backup_calc_pos_G", calcuitem.backup_calc_pos_G);
        my_calcufile11->SetAttribute("backup_calc_pos_H", calcuitem.backup_calc_pos_H);
        my_calcufile11->SetAttribute("backup_calc_pos_I", calcuitem.backup_calc_pos_I);
        my_calcufile11->SetAttribute("backup_calc_pos_J", calcuitem.backup_calc_pos_J);
        my_calcufile11->SetAttribute("backup_calc_pos_K", calcuitem.backup_calc_pos_K);
        my_calcufile11->SetAttribute("backup_calc_pos_L", calcuitem.backup_calc_pos_L);
        my_calcufile11->SetAttribute("backup_calc_pos_M", calcuitem.backup_calc_pos_M);
        my_calcufile11->SetAttribute("backup_calc_pos_N", calcuitem.backup_calc_pos_N);
        my_calcufile11->SetAttribute("backup_calc_pos_O", calcuitem.backup_calc_pos_O);
        my_calcufile11->SetAttribute("backup_calc_pos_P", calcuitem.backup_calc_pos_P);
        my_calcufile11->SetAttribute("backup_calc_pos_Q", calcuitem.backup_calc_pos_Q);
        my_calcufile11->SetAttribute("backup_calc_pos_R", calcuitem.backup_calc_pos_R);
        my_calcufile11->SetAttribute("backup_calc_pos_RT", calcuitem.backup_calc_pos_RT);
        my_calcufile11->SetAttribute("backup_calc_pos_fuse", calcuitem.has_backup_fusecalc);

        my_calcufile1->LinkEndChild(my_calcufile11);
    }
    // 【注意！】QString转char不能用QString().toStdString().c_str()，否则可能会出问题，好像不更新中文名字
    QByteArray tempstr = getFilename().toLocal8Bit();
    bool ret = my_tunnelcheck->SaveFile(tempstr.constData());
    mutex.unlock();
    return ret;
}


