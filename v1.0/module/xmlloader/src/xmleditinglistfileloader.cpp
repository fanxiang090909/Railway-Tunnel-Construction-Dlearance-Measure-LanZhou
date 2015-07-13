#include "xmleditinglistfileloader.h"

#include "editing_list.h"

#include <QDebug>
#include <QTextCodec>
#include <QDateTime>

/**
 * 服务器保存解析正在修正隧道列表
 * @author 范翔
 * @date 2014-4-5
 */
XMLEditingListFileLoader::XMLEditingListFileLoader (QString filename):XMLFileLoader(filename)
{

}

XMLEditingListFileLoader ::~XMLEditingListFileLoader ()
{

}

bool XMLEditingListFileLoader ::saveFile()
{
    TiXmlDocument * m_xmlDoc = new TiXmlDocument();
    //创建版本号
    TiXmlDeclaration dec("1.0","gb2312","yes");
    m_xmlDoc->InsertEndChild(dec);
    //创建一个根元素并连接
    TiXmlElement *m_tasks = new TiXmlElement("editing_list");
    m_xmlDoc->LinkEndChild(m_tasks);
    EditingItem item;
    QByteArray tmpba;


    for(int i = 0 ; i < EditingList::getEditingListInstance()->list()->length(); i++)//TabWidget默认的编号是从0开始的
    {
        //创建第一个task元素并连接
        TiXmlElement *taskElement = new TiXmlElement("editing_item");
        //qDebug()<<a;
        //设置task元素的属性
        item = EditingList::getEditingListInstance()->list()->at(i);
        tmpba = item.tunnelname.toLocal8Bit();
        taskElement->SetAttribute("tunnel_name", tmpba.constData());

        tmpba = item.collectdate.toLocal8Bit();
        taskElement->SetAttribute("collect_date", tmpba.constData());

        tmpba = item.collectdate.toLocal8Bit();
        taskElement->SetAttribute("user_id", tmpba.constData());

        tmpba = item.collectdate.toLocal8Bit();
        taskElement->SetAttribute("start_time", tmpba.constData());

        tmpba = item.collectdate.toLocal8Bit();
        taskElement->SetAttribute("end_time", tmpba.constData());

        m_tasks->LinkEndChild(taskElement);
    }

    // qDebug()<<QObject::tr(ui->taskTunnelTableWidget->item(0,9)->text().toLocal8Bit().data());
    /*含有中文的计划隧道文件，后续调用时不行*/
    QByteArray tempstr = getFilename().toLocal8Bit();
    //qDebug() << tempstr.constData();
    bool ret = m_xmlDoc->SaveFile(tempstr.constData());
    return ret;
}

bool XMLEditingListFileLoader ::loadFile()
{
    EditingList::getEditingListInstance()->clear();

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
        return false;
    }
    //获得根元素，即taskAssignment
    TiXmlElement *RootElement =my_task->RootElement();
    TiXmlElement *Firsttask =RootElement->FirstChildElement();
    int tmpint;//临时换成bool
    while(Firsttask)
    {
        TiXmlAttribute *attr1 =Firsttask->FirstAttribute();//解析出线路序号 lineNum->Name为"线路序号"，lineNum->Value为线路序号的值即"54"
        const char* temp;
        temp =attr1->Value();
        QString strtunnelname(temp);
        TiXmlAttribute *attr2 =attr1->Next();
        temp =attr2->Value();
        QString strcollectdate(temp);
        TiXmlAttribute *attr3 =attr2->Next();
        temp =attr3->Value();
        QString struserid(temp);

        TiXmlAttribute *attr4 =attr3->Next();
        temp =attr4->Value();
        QString strstarttime(temp);
        TiXmlAttribute *attr5 =attr4->Next();
        temp =attr5->Value();
        QString strendtime(temp);

        EditingItem item;
        item.tunnelname = strtunnelname;
        item.collectdate = strcollectdate;
        item.userid = struserid;
        item.begintime = strstarttime;
        item.endtime = strendtime;

        EditingList::getEditingListInstance()->pushBack(item);

        Firsttask=Firsttask->NextSiblingElement();//为了解析出所有任务，使用while循环
    }
    qDebug() << "edit list length:" << EditingList::getEditingListInstance()->list()->length();

    return true;
}
