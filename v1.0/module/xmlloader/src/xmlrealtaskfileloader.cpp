#include "xmlrealtaskfileloader.h"
#include "realtask.h"
#include "realtask_list.h"
#include <QDebug>
#include <QtDebug>
#include <QTextCodec>

/**
 * 实际采集任务记录的XML文件解析类定义
 * 继承自XMLFileLoader
 * @see XMLFileLoader
 * @author  熊雪 范翔
 * @date 20140105
 * @version 1.0.0
 */
XMLRealTaskFileLoader::XMLRealTaskFileLoader(QString filename) : XMLFileLoader(filename)
{
    setFilename(filename);
}

XMLRealTaskFileLoader::~XMLRealTaskFileLoader()
{
}

bool XMLRealTaskFileLoader::saveFile(RealTaskList & list)
{
    mutex.lock();

    TiXmlDocument  *m_xmlDoc =new TiXmlDocument();
    //创建版本号
    TiXmlDeclaration dec("1.0","gb2312","yes");
    m_xmlDoc->InsertEndChild(dec);
    //创建一个根元素并连接
    TiXmlElement *m_tasks =new TiXmlElement("real_task_list");
    m_xmlDoc->LinkEndChild(m_tasks);
    RealTask tmp;
    for(int i = 0; i < list.list()->length(); i++)
    {
        //创建第一个task元素并连接
        TiXmlElement *taskElement = new TiXmlElement("real_task");
        m_tasks->LinkEndChild(taskElement);
        //
        tmp = list.list()->at(i);
        //设置task元素的属性
        taskElement->SetAttribute("tunnel_id", tmp.tunnelid);
        taskElement->SetAttribute("tunnel_name", tmp.tunnelname.c_str());
        taskElement->SetAttribute("seqno", tmp.seqno);
        taskElement->SetAttribute("valid_frames", tmp.valid_frames);
        taskElement->SetAttribute("collect_time", tmp.datetime.c_str());
        taskElement->SetAttribute("is_valid", tmp.isvalid);

        taskElement->SetDoubleAttribute("start_mile", tmp.start_mile);
        taskElement->SetDoubleAttribute("end_mile", tmp.end_mile);
        taskElement->SetAttribute("start_frame_master", tmp.start_frame_master);
        taskElement->SetAttribute("end_frame_master", tmp.end_frame_master);

        taskElement->SetAttribute("start_frame_A1", tmp.start_frame_A1);
        taskElement->SetAttribute("end_frame_A1", tmp.end_frame_A1);
        taskElement->SetAttribute("start_frame_A2", tmp.start_frame_A2);
        taskElement->SetAttribute("end_frame_A2", tmp.end_frame_A2);
        taskElement->SetAttribute("start_frame_B1", tmp.start_frame_B1);
        taskElement->SetAttribute("end_frame_B1", tmp.end_frame_B1);
        taskElement->SetAttribute("start_frame_B2", tmp.start_frame_B2);
        taskElement->SetAttribute("end_frame_B2", tmp.end_frame_B2);
        taskElement->SetAttribute("start_frame_C1", tmp.start_frame_C1);
        taskElement->SetAttribute("end_frame_C1", tmp.end_frame_C1);
        taskElement->SetAttribute("start_frame_C2", tmp.start_frame_C2);
        taskElement->SetAttribute("end_frame_C2", tmp.end_frame_C2);
        taskElement->SetAttribute("start_frame_D1", tmp.start_frame_D1);
        taskElement->SetAttribute("end_frame_D1", tmp.end_frame_D1);
        taskElement->SetAttribute("start_frame_D2", tmp.start_frame_D2);
        taskElement->SetAttribute("end_frame_D2", tmp.end_frame_D2);
        taskElement->SetAttribute("start_frame_E1", tmp.start_frame_E1);
        taskElement->SetAttribute("end_frame_E1", tmp.end_frame_E1);
        taskElement->SetAttribute("start_frame_E2", tmp.start_frame_E2);
        taskElement->SetAttribute("end_frame_E2", tmp.end_frame_E2);
        taskElement->SetAttribute("start_frame_F1", tmp.start_frame_F1);
        taskElement->SetAttribute("end_frame_F1", tmp.end_frame_F1);
        taskElement->SetAttribute("start_frame_F2", tmp.start_frame_F2);
        taskElement->SetAttribute("end_frame_F2", tmp.end_frame_F2);
        taskElement->SetAttribute("start_frame_G1", tmp.start_frame_G1);
        taskElement->SetAttribute("end_frame_G1", tmp.end_frame_G1);
        taskElement->SetAttribute("start_frame_G2", tmp.start_frame_G2);
        taskElement->SetAttribute("end_frame_G2", tmp.end_frame_G2);
        taskElement->SetAttribute("start_frame_H1", tmp.start_frame_H1);
        taskElement->SetAttribute("end_frame_H1", tmp.end_frame_H1);
        taskElement->SetAttribute("start_frame_H2", tmp.start_frame_H2);
        taskElement->SetAttribute("end_frame_H2", tmp.end_frame_H2);
        taskElement->SetAttribute("start_frame_I1", tmp.start_frame_I1);
        taskElement->SetAttribute("end_frame_I1", tmp.end_frame_I1);
        taskElement->SetAttribute("start_frame_I2", tmp.start_frame_I2);
        taskElement->SetAttribute("end_frame_I2", tmp.end_frame_I2);
        taskElement->SetAttribute("start_frame_J1", tmp.start_frame_J1);
        taskElement->SetAttribute("end_frame_J1", tmp.end_frame_J1);
        taskElement->SetAttribute("start_frame_J2", tmp.start_frame_J2);
        taskElement->SetAttribute("end_frame_J2", tmp.end_frame_J2);
        taskElement->SetAttribute("start_frame_K1", tmp.start_frame_K1);
        taskElement->SetAttribute("end_frame_K1", tmp.end_frame_K1);
        taskElement->SetAttribute("start_frame_K2", tmp.start_frame_K2);
        taskElement->SetAttribute("end_frame_K2", tmp.end_frame_K2);
        taskElement->SetAttribute("start_frame_L1", tmp.start_frame_L1);
        taskElement->SetAttribute("end_frame_L1", tmp.end_frame_L1);
        taskElement->SetAttribute("start_frame_L2", tmp.start_frame_L2);
        taskElement->SetAttribute("end_frame_L2", tmp.end_frame_L2);
        taskElement->SetAttribute("start_frame_M1", tmp.start_frame_M1);
        taskElement->SetAttribute("end_frame_M1", tmp.end_frame_M1);
        taskElement->SetAttribute("start_frame_M2", tmp.start_frame_M2);
        taskElement->SetAttribute("end_frame_M2", tmp.end_frame_M2);
        taskElement->SetAttribute("start_frame_N1", tmp.start_frame_N1);
        taskElement->SetAttribute("end_frame_N1", tmp.end_frame_N1);
        taskElement->SetAttribute("start_frame_N2", tmp.start_frame_N2);
        taskElement->SetAttribute("end_frame_N2", tmp.end_frame_N2);
        taskElement->SetAttribute("start_frame_O1", tmp.start_frame_O1);
        taskElement->SetAttribute("end_frame_O1", tmp.end_frame_O1);
        taskElement->SetAttribute("start_frame_O2", tmp.start_frame_O2);
        taskElement->SetAttribute("end_frame_O2", tmp.end_frame_O2);
        taskElement->SetAttribute("start_frame_P1", tmp.start_frame_P1);
        taskElement->SetAttribute("end_frame_P1", tmp.end_frame_P1);
        taskElement->SetAttribute("start_frame_P2", tmp.start_frame_P2);
        taskElement->SetAttribute("end_frame_P2", tmp.end_frame_P2);
        taskElement->SetAttribute("start_frame_Q1", tmp.start_frame_Q1);
        taskElement->SetAttribute("end_frame_Q1", tmp.end_frame_Q1);
        taskElement->SetAttribute("start_frame_Q2", tmp.start_frame_Q2);
        taskElement->SetAttribute("end_frame_Q2", tmp.end_frame_Q2);
        taskElement->SetAttribute("start_frame_R1", tmp.start_frame_R1);
        taskElement->SetAttribute("end_frame_R1", tmp.end_frame_R1);
        taskElement->SetAttribute("start_frame_R2", tmp.start_frame_R2);
        taskElement->SetAttribute("end_frame_R2", tmp.end_frame_R2);

        taskElement->SetAttribute("has_backup_A1", tmp.has_backup_A1);
        taskElement->SetAttribute("has_backup_A2", tmp.has_backup_A2);
        taskElement->SetAttribute("has_backup_B1", tmp.has_backup_B1);
        taskElement->SetAttribute("has_backup_B2", tmp.has_backup_B2);
        taskElement->SetAttribute("has_backup_C1", tmp.has_backup_C1);
        taskElement->SetAttribute("has_backup_C2", tmp.has_backup_C2);
        taskElement->SetAttribute("has_backup_D1", tmp.has_backup_D1);
        taskElement->SetAttribute("has_backup_D2", tmp.has_backup_D2);
        taskElement->SetAttribute("has_backup_E1", tmp.has_backup_E1);
        taskElement->SetAttribute("has_backup_E2", tmp.has_backup_E2);
        taskElement->SetAttribute("has_backup_F1", tmp.has_backup_F1);
        taskElement->SetAttribute("has_backup_F2", tmp.has_backup_F2);
        taskElement->SetAttribute("has_backup_G1", tmp.has_backup_G1);
        taskElement->SetAttribute("has_backup_G2", tmp.has_backup_G2);
        taskElement->SetAttribute("has_backup_H1", tmp.has_backup_H1);
        taskElement->SetAttribute("has_backup_H2", tmp.has_backup_H2);
        taskElement->SetAttribute("has_backup_I1", tmp.has_backup_I1);
        taskElement->SetAttribute("has_backup_I2", tmp.has_backup_I2);
        taskElement->SetAttribute("has_backup_J1", tmp.has_backup_J1);
        taskElement->SetAttribute("has_backup_J2", tmp.has_backup_J2);
        taskElement->SetAttribute("has_backup_K1", tmp.has_backup_K1);
        taskElement->SetAttribute("has_backup_K2", tmp.has_backup_K2);
        taskElement->SetAttribute("has_backup_L1", tmp.has_backup_L1);
        taskElement->SetAttribute("has_backup_L2", tmp.has_backup_L2);
        taskElement->SetAttribute("has_backup_M1", tmp.has_backup_M1);
        taskElement->SetAttribute("has_backup_M2", tmp.has_backup_M2);
        taskElement->SetAttribute("has_backup_N1", tmp.has_backup_N1);
        taskElement->SetAttribute("has_backup_N2", tmp.has_backup_N2);
        taskElement->SetAttribute("has_backup_O1", tmp.has_backup_O1);
        taskElement->SetAttribute("has_backup_O2", tmp.has_backup_O2);
        taskElement->SetAttribute("has_backup_P1", tmp.has_backup_P1);
        taskElement->SetAttribute("has_backup_P2", tmp.has_backup_P2);
        taskElement->SetAttribute("has_backup_Q1", tmp.has_backup_Q1);
        taskElement->SetAttribute("has_backup_Q2", tmp.has_backup_Q2);
        taskElement->SetAttribute("has_backup_R1", tmp.has_backup_R1);
        taskElement->SetAttribute("has_backup_R2", tmp.has_backup_R2);
    }
    // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
    QByteArray tempstr = getFilename().toLocal8Bit();
    bool ret =  m_xmlDoc->SaveFile(tempstr.constData());
    mutex.unlock();

    return ret;
}

bool XMLRealTaskFileLoader::loadFile(RealTaskList & list)//解析xml文件生成listcollect,这个list是实际采集隧道的数据
{
    mutex.lock();

    list.clear();
    TiXmlDocument *my_tunnelcheck = new TiXmlDocument();
    // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
    QByteArray tempstr = getFilename().toLocal8Bit();
    bool oknotfile = my_tunnelcheck->LoadFile(tempstr.constData());
    qDebug() << "real file loader" << tempstr.constData();
    if(!oknotfile)
    {
        mutex.unlock();
        return false;
    }
    TiXmlElement *my_root =my_tunnelcheck->RootElement();
    // qDebug()<<my_root->Value();
    TiXmlElement *my_tasklist =my_root->FirstChildElement();
    // qDebug()<<my_tasklist->Value();
    //qDebug()<<"error";
    RealTask real_list1 = RealTask();
    const char* temp;
    while(my_tasklist)
    {
        TiXmlAttribute *my_tasklistattr1 =my_tasklist->FirstAttribute();
        temp =my_tasklistattr1->Value();
        /*出错情况：二进制“<<”: 没有找到接受“QDebug”类型的左操作数的运算符(或没有可接受的转换),
        ***解决方法是将string类型的值用qDebug()<<输出时，要在变量后面加上c_str,
        如qDebug()<<linenum1.c_str();*/
        //qDebug()<<QObject::tr(my_tasklistattr2->Name())<<my_tasklistattr2->Value()<<linenum1.c_str();//"线路序号" 54 54
        int tunnelnum1 =atoi(temp);
        TiXmlAttribute *my_tasklistattr5 =my_tasklistattr1->Next();
        temp =my_tasklistattr5->Value();
        string tunnelname1(temp);
        TiXmlAttribute *my_tasklistattr2 =my_tasklistattr5->Next();
        temp =my_tasklistattr2->Value();
        int seqno = atoi(temp);

        TiXmlAttribute *my_tasklistattr15 =my_tasklistattr2->Next();
        temp =my_tasklistattr15->Value();
        int validframes = atoi(temp);
        //qDebug() << temp << validframes;
        TiXmlAttribute *my_tasklistattr16 =my_tasklistattr15->Next();
        temp =my_tasklistattr16->Value();
        string tunnelcollecttime(temp);
        //qDebug() << temp << tunnelcollecttime.c_str();
        TiXmlAttribute *traindirection =my_tasklistattr16->Next();//解析出车厢方向
        temp =traindirection->Value();
		int tmpint = atoi(temp);
        bool isvaliddd;
        if (tmpint == 0)
            isvaliddd = false;
        else
            isvaliddd = true;

        //real_collectlist real_list1;
        real_list1.tunnelid=tunnelnum1;
        real_list1.tunnelname=tunnelname1;
        real_list1.seqno=seqno;
        real_list1.valid_frames=validframes;
        real_list1.datetime = tunnelcollecttime;
        real_list1.isvalid = isvaliddd;

        TiXmlAttribute *my_tasklistattr6 = traindirection->Next();
        temp = my_tasklistattr6->Value();
        real_list1.start_mile = atof(temp);
        TiXmlAttribute *my_tasklistattr7 = my_tasklistattr6->Next();
        temp = my_tasklistattr7->Value();
        real_list1.end_mile = atof(temp);
        
        // start frame and end frame
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_master=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_master=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_A1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_A1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_A2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_A2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_B1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_B1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_B2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_B2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_C1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_C1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_C2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_C2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_D1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_D1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_D2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_D2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_E1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_E1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_E2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_E2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_F1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_F1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_F2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_F2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_G1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_G1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_G2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_G2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_H1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_H1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_H2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_H2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_I1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_I1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_I2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_I2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_J1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_J1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_J2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_J2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_K1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_K1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_K2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_K2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_L1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_L1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_L2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_L2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_M1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_M1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_M2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_M2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_N1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_N1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_N2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_N2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_O1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_O1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_O2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_O2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_P1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_P1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_P2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_P2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_Q1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_Q1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_Q2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_Q2=atol(temp);

        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_R1=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_R1=atol(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.start_frame_R2=atol(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.end_frame_R2=atol(temp);

        // has backup
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_A1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_A2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_B1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_B2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_C1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_C2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_D1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_D2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_E1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_E2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_F1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_F2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_G1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_G2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_H1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_H2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_I1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_I2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_J1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_J2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_K1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_K2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_L1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_L2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_M1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_M2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_N1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_N2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_O1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_O2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_P1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_P2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_Q1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_Q2= atoi(temp);
        my_tasklistattr6 =my_tasklistattr7->Next();
        temp =my_tasklistattr6->Value();
        real_list1.has_backup_R1= atoi(temp);
        my_tasklistattr7 =my_tasklistattr6->Next();
        temp =my_tasklistattr7->Value();
        real_list1.has_backup_R2= atoi(temp);


        qDebug() << "real task :" << tunnelnum1 << ", " << QString::fromLocal8Bit(tunnelname1.c_str())
            << ", " << seqno << ", " << real_list1.start_frame_A1 << ", " << real_list1.end_frame_A1 << ", " << validframes << ", " << QString::fromLocal8Bit(tunnelcollecttime.c_str());

        list.pushBack(real_list1);

        my_tasklist=my_tasklist->NextSiblingElement();
        // qDebug()<<"error1";
    }
    qDebug() << "real task list length" << list.list()->length();
    mutex.unlock();

    return true;
}
