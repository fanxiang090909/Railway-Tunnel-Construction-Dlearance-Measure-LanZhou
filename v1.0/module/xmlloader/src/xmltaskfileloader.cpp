#include "xmltaskfileloader.h"

#include "plantask.h"
#include "plantask_list.h"
#include <QDebug>
#include <QtDebug>
#include <QTextCodec>
#include <QDateTime>

/**
 * 计划任务配置的XML读写类定义
 * @see XMLFileLoader
 * @author 熊雪 范翔
 * @date 2013-11-13
 */
XMLTaskFileLoader::XMLTaskFileLoader(QString filename):XMLFileLoader(filename)
{

}

XMLTaskFileLoader::~XMLTaskFileLoader()
{

}

bool XMLTaskFileLoader::saveFile(PlanTaskList & list)
{
    mutex.lock();

    TiXmlDocument * m_xmlDoc = new TiXmlDocument();
    //创建版本号
    TiXmlDeclaration dec("1.0","gb2312","yes");
    m_xmlDoc->InsertEndChild(dec);
    //创建一个根元素并连接
    TiXmlElement *m_tasks = new TiXmlElement("plan_task_list");
    m_xmlDoc->LinkEndChild(m_tasks);
    string linename = "";
    PlanTask task;
    for(int i = 0 ; i < list.list()->length(); i++)//TabWidget默认的编号是从0开始的
    {
        //创建第一个task元素并连接
        TiXmlElement *taskElement = new TiXmlElement("plan_task");
        //qDebug()<<a;
        //设置task元素的属性
        task = list.list()->at(i);
        taskElement->SetAttribute("line_id",task.linenum);
        taskElement->SetAttribute("line_name",task.linename.c_str());
        if (linename.compare("") != 0)
            linename = task.linename;
        taskElement->SetAttribute("tunnel_id",task.tunnelnum);
        taskElement->SetAttribute("tunnel_name",task.tunnelname.c_str());
        taskElement->SetAttribute("start_mileage",task.startdistance);
        taskElement->SetAttribute("end_mileage",task.enddistance);
        taskElement->SetAttribute("carriage_direction",task.traindirection);
        taskElement->SetAttribute("is_newline",task.newline);
        taskElement->SetAttribute("line_type",task.linetype);
        taskElement->SetAttribute("is_double_line",task.doubleline);
        taskElement->SetAttribute("is_downlink",task.downstream);
        taskElement->SetDoubleAttribute("interval_pulse_num",task.pulsepermeter);
        taskElement->SetAttribute("is_normal",task.isnormal);
        taskElement->SetAttribute("date",task.datetime.c_str());
        m_tasks->LinkEndChild(taskElement);
    }

    // qDebug()<<QObject::tr(ui->taskTunnelTableWidget->item(0,9)->text().toLocal8Bit().data());
    /*含有中文的计划隧道文件，后续调用时不行*/
    //QString qianzhui = QString(linename.c_str());//获得线路名称
    //QDateTime realtime = QDateTime::currentDateTime();//获取系统当前时间
    //QString realstr =realtime.toString("yyyyMMdd");//设置显示格式 //MM为大写才能显示为正确的月份，若在月份中使用mm则会被误认为是分钟
    //QString houzhui =".xml";
    //QString taskname=qianzhui+"_"+realstr+houzhui;//XMl命名为线路名称_时间.xml
    ////QString类型转换为const char *  当QString中含有中文时，name.toLocal8Bit().data()
    QByteArray tempstr = getFilename().toLocal8Bit();
    //qDebug() << tempstr.constData();
    bool ret = m_xmlDoc->SaveFile(tempstr.constData());
    mutex.unlock();
    return ret;
}

bool XMLTaskFileLoader::loadFile(PlanTaskList & list)
{
    mutex.lock();

    list.clear();

    TiXmlDocument *my_task = new TiXmlDocument();
    //std::string strStd = getFilename().toStdString();
    //if (code)
    //    strStd = code->fromUnicode(getFilename()).data();
    // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
    QByteArray tempstr = getFilename().toLocal8Bit();
    //qDebug() << tempstr.constData();
    bool okfile = my_task->LoadFile(tempstr.constData());
    if(!okfile)
    {
        qDebug()<<QObject::tr("暂未生成当前时间下的采集任务，请先生成最新的采集任务。");
        mutex.unlock();
        return false;
    }
    //获得根元素，即taskAssignment
    TiXmlElement *RootElement =my_task->RootElement();
    TiXmlElement *Firsttask =RootElement->FirstChildElement();
    int tmpint;//临时换成bool
    while(Firsttask)
    {
        TiXmlAttribute *lineNum =Firsttask->FirstAttribute();//解析出线路序号 lineNum->Name为"线路序号"，lineNum->Value为线路序号的值即"54"
        const char* temp;
        temp=lineNum->Value();
        int lineid =atoi(temp);
        //qDebug()<<QObject::tr("线路序号:")<<lineNum->Value()<<lineid;
        TiXmlAttribute *lineName =lineNum->Next();//获得同一个task任务中的下一个属性，即线路名称
        temp =lineName->Value();
        string linename(temp);
        //qDebug()<<QObject::tr("线路名称:")<<QString::fromLocal8Bit(lineName->Value())<<QString::fromLocal8Bit(linename.c_str());
        TiXmlAttribute *tunnelNum =lineName->Next();//解析出隧道编号
        temp =tunnelNum->Value();
        int tunnelid =atoi(temp);
        //qDebug()<<QObject::tr("隧道ID:")<<tunnelNum->Value()<<tunnelid;
        TiXmlAttribute *tunnelName =tunnelNum->Next();//解析出隧道名称
        temp =tunnelName->Value();
        string tunnelname(temp);
        //qDebug()<<QObject::tr("隧道名称:")<<QString::fromLocal8Bit(tunnelName->Value())<<QString::fromLocal8Bit(tunnelname.c_str());
        TiXmlAttribute *startdistance =tunnelName->Next();//解析出起始里程
        temp =startdistance->Value();
        _int64 startlength = atol(temp);
        //qDebug()<<QObject::tr("隧道起始里程:")<<QString("%1").arg(startdistance->Value())<<QString("%1").arg(startlength);
        TiXmlAttribute *enddistance =startdistance->Next();//解析出终止里程
        temp =enddistance->Value();
        _int64 endlength = atol(temp);
        //qDebug()<<QObject::tr("隧道终止里程:")<<QString("%1").arg(enddistance->Value())<<QString("%1").arg(endlength);
        TiXmlAttribute *traindirection =enddistance->Next();//解析出车厢方向
        temp =traindirection->Value();
		tmpint = atoi(temp);
        bool trainorientation;
        if (tmpint == 0)
            trainorientation = false;
        else
            trainorientation = true;
        TiXmlAttribute *newline =traindirection->Next();//解析出是否为新线
        temp =newline->Value();
        int newlinenot = atoi(temp);
        TiXmlAttribute *linetype =newline->Next();//线路类型
        temp =linetype->Value();
        int lineform = atoi(temp);
        TiXmlAttribute *doubleline =linetype->Next();//是否为双线
        temp =doubleline->Value();
        tmpint = atoi(temp);
        bool isdoubleline;
        if (tmpint == 0)
            isdoubleline = false;
        else
            isdoubleline = true;
        TiXmlAttribute *downstream =doubleline->Next();//是否为下行
        temp =downstream->Value();
        tmpint = atoi(temp);
        bool isdownlink;
        if (tmpint == 0)
            isdownlink = false;
        else
            isdownlink = true;
        TiXmlAttribute *pulsePerMeter =downstream->Next();//每米脉冲数
        temp =pulsePerMeter->Value();
        float pulsenumber = atof(temp);
        TiXmlAttribute *zhengchangnot =pulsePerMeter->Next();//是否正常
        temp =zhengchangnot->Value();
        tmpint = atoi(temp);
        bool isnormal;
        if (tmpint == 0)
            isnormal = false;
        else
            isnormal = true;
        TiXmlAttribute *datetime =Firsttask->LastAttribute();
        temp =datetime->Value();//2013-10-14
        string date(temp);
        //qDebug()<<QObject::tr("日期:")<<datetime->Value()<<date.c_str();
        PlanTask list1;
        list1.linenum=lineid;
        list1.linename =linename;
        list1.tunnelnum =tunnelid;
        list1.tunnelname =tunnelname;
        list1.startdistance =startlength;
        list1.enddistance =endlength;
        list1.traindirection =trainorientation;
        list1.newline =newlinenot;
        list1.linetype =lineform;
        list1.doubleline = isdoubleline;
        list1.downstream = isdownlink;
        list1.pulsepermeter =pulsenumber;
        list1.isnormal = isnormal;
        list1.datetime =date;
        list.pushBack(list1);
        Firsttask=Firsttask->NextSiblingElement();//为了解析出所有任务，使用while循环
    }
    qDebug() << "listtask_tasklength:" << list.list()->length();
    mutex.unlock();

    return true;
}
