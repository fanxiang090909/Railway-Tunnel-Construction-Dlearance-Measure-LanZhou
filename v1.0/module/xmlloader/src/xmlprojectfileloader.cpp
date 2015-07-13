#include "xmlprojectfileloader.h"

#include <QDebug>

using namespace std;

/**
 * 工程入口配置的XML文件解析类实现
 * @see ProjectModel
 * @author fanxiang
 * @date 20140327
 * @version 1.0.0
 */
XMLProjectFileLoader::XMLProjectFileLoader(QString filename)
{
    setFilename(filename);
}

XMLProjectFileLoader::~XMLProjectFileLoader()
{
}

void XMLProjectFileLoader::setFilename(QString filename)
{
    this->filename = filename;
    // QString转cstring
    // 方案一：
    // QString str = "xxx";
    // QByteArray ba = str.toAscii();
    // const char *cstr = strcpy(ba.constData());
    // 方案二:
    // QString str = "xxx";
    // QByteArray ba = str.toAscii();
    // someCallThatNeedsACStr(ba.constData());
    // 参见http://www.qtcentre.org/archive/index.php/t-33924.html
    // 【注意！】QString转char不能用QString().toStdString().c_str()，否则可能会出问题
}

QString XMLProjectFileLoader::getFilename()
{
    return filename;
}

/**
 * 加载文件，从文件中读取信息
 * @param ProjectModel 代表工程目录，其中包含工程相关信息（@see ProjectModel）
 */
bool XMLProjectFileLoader::loadFile(ProjectModel & projectmodel)
{
    mutex.lock();
    TiXmlDocument * doc = new TiXmlDocument();
    // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
    QByteArray tempstr = getFilename().toLocal8Bit();
    bool hasfile = doc->LoadFile(tempstr.constData());
    if (!hasfile)
    {
        mutex.unlock();
        return false;
    }
    TiXmlElement *my_root1 = doc->RootElement();
    TiXmlAttribute *my_root1Attr1 = my_root1->FirstAttribute();
    const char* temp;
    temp = my_root1Attr1->Value();
    float version = atof(temp);
    //qDebug()<<my_root1->Value();
    TiXmlAttribute *my_root1Attr2 = my_root1Attr1->Next();
    temp = my_root1Attr2->Value();
    QString planfile = QString::fromLocal8Bit(temp);
    TiXmlAttribute *my_root1Attr3 = my_root1Attr2->Next();
    temp = my_root1Attr3->Value();
    QString realfile = QString::fromLocal8Bit(temp);
    TiXmlAttribute *my_root1Attr4 = my_root1Attr3->Next();
    temp = my_root1Attr4->Value();
    QString checkfile = QString::fromLocal8Bit(temp);

    TiXmlAttribute *my_root1Attr12 = my_root1Attr4->Next();
    temp = my_root1Attr12->Value();
    QString pulzerecorderfile = QString::fromLocal8Bit(temp);

    TiXmlAttribute *my_root1Attr5 = my_root1Attr12->Next();
    temp = my_root1Attr5->Value();
    QString createdate = QString::fromLocal8Bit(temp);
    TiXmlAttribute *my_root1Attr6 = my_root1Attr5->Next();
    temp = my_root1Attr6->Value();
    QString realcollectdate = QString::fromLocal8Bit(temp);
    TiXmlAttribute *my_root1Attr7 = my_root1Attr6->Next();
    temp = my_root1Attr7->Value();
    QString lastcheckdate = QString::fromLocal8Bit(temp);
    TiXmlAttribute *my_root1Attr8 = my_root1Attr7->Next();
    temp = my_root1Attr8->Value();
    QString endcalculatedate = QString::fromLocal8Bit(temp);
    TiXmlAttribute *my_root1Attr9 = my_root1Attr8->Next();
    temp = my_root1Attr9->Value();
    QString lasteditdate = QString::fromLocal8Bit(temp);

    TiXmlAttribute *my_root1Attr10 = my_root1Attr9->Next();
    temp = my_root1Attr10->Value();
    QString createuser = QString::fromLocal8Bit(temp);
    TiXmlAttribute *my_root1Attr11 = my_root1Attr10->Next();
    temp = my_root1Attr11->Value();
    QString lastcheckuser = QString::fromLocal8Bit(temp);

    // 赋值
    if (version != projectmodel.getVersion())
    {
        qDebug() << QObject::tr("版本不符，不能加载Project Model xml文件！");
        mutex.unlock();
        return false;
    }
    projectmodel.setPlanFilename(planfile);
    projectmodel.setRealFilename(realfile);
    projectmodel.setCheckedFilename(checkfile);
    projectmodel.setPulzeRecorderFilename(pulzerecorderfile);
    projectmodel.setCreateDate(createdate);
    projectmodel.setRealCollectDate(realcollectdate);
    projectmodel.setLastCheckDate(lastcheckdate);
    projectmodel.setEndCalculateDate(endcalculatedate);
    projectmodel.setLastEditDate(lasteditdate);
    projectmodel.setCreateUser(createuser);
    projectmodel.setLastCheckUser(lastcheckuser);
    mutex.unlock();
    return true;
}

/**
 * 存储文件，从projectModel中读取信息存入文件
 * @param ProjectModel 代表工程目录，其中包含工程相关信息（@see ProjectModel）
 */
bool XMLProjectFileLoader::saveFile(ProjectModel & projectmodel)
{
    mutex.lock();
    QByteArray tempstr;

    //生成xml文件
    TiXmlDocument *doc = new TiXmlDocument();
    TiXmlDeclaration dec("1.0", "gb2312", "yes");
    doc->InsertEndChild(dec);
    TiXmlElement *my_root1 = new TiXmlElement("project_config");
    my_root1->SetAttribute("version", projectmodel.getVersion());
    tempstr = projectmodel.getPlanFilename().toLocal8Bit();
    my_root1->SetAttribute("plan_task_filename", tempstr.constData());
    tempstr = projectmodel.getRealFilename().toLocal8Bit();
    my_root1->SetAttribute("real_task_filename", tempstr.constData());
    tempstr = projectmodel.getCheckedFilename().toLocal8Bit();
    my_root1->SetAttribute("checked_task_filename", tempstr.constData());
    tempstr = projectmodel.getPulzeRecorderFilename().toLocal8Bit();
    my_root1->SetAttribute("pulze_recorder_file", tempstr.constData());
    tempstr = projectmodel.getCreateDate().toLocal8Bit();
    my_root1->SetAttribute("pro_create_date", tempstr.constData());
    tempstr = projectmodel.getRealCollectDate().toLocal8Bit();
    my_root1->SetAttribute("pro_real_collect_date", tempstr.constData());
    tempstr = projectmodel.getEndCalculateDate().toLocal8Bit();
    my_root1->SetAttribute("pro_last_check_date", tempstr.constData());
    tempstr = projectmodel.getRealCollectDate().toLocal8Bit();
    my_root1->SetAttribute("pro_end_calculate_date", tempstr.constData());
    tempstr = projectmodel.getLastEditDate().toLocal8Bit();
    my_root1->SetAttribute("pro_last_edit_date", tempstr.constData());

    my_root1->SetAttribute("pro_create_user", tempstr.constData());
    tempstr = projectmodel.getRealCollectDate().toLocal8Bit();
    my_root1->SetAttribute("pro_last_check_user", tempstr.constData());
    tempstr = projectmodel.getRealCollectDate().toLocal8Bit();

    doc->LinkEndChild(my_root1);

    // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
    tempstr = getFilename().toLocal8Bit();
    //qDebug() << "save to " << getFilename();
    bool ret = doc->SaveFile(tempstr.constData());
    mutex.unlock();
    return ret;
}
