#include "xmlsynthesisheightsfileloader.h"

#include <QDebug>
#include <string>
#include <list>
#include "output_heights_list.h"

using namespace std;

/**
 * 隧道综合指定高度配置的XML文件解析实现
 * 继承自XMLFileLoader
 * @see XMLFileLoader
 * @author fanxiang
 * @date 20140105
 * @version 1.0.0
 */
XMLSynthesisHeightsFileLoader::XMLSynthesisHeightsFileLoader(QString filename) : XMLFileLoader(filename)
{

}

XMLSynthesisHeightsFileLoader::~XMLSynthesisHeightsFileLoader()
{

}

// 子类实现父类的函数
bool XMLSynthesisHeightsFileLoader::loadFile()
{
    mutex.lock();
    // 清空原有list
    OutputHeightsList::getOutputHeightsListInstance()->heightsClear();

	TiXmlDocument * doc = new TiXmlDocument();
    // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
    QByteArray tempstr = getFilename().toAscii();
    bool hasfile = doc->LoadFile(tempstr.constData());
	if (!hasfile)
    {
        return false;
    }
    TiXmlElement *my_root1 = doc->RootElement();
    TiXmlAttribute *my_root1Attr1 = my_root1->FirstAttribute();
    const char* temp;
    temp = my_root1Attr1->Value();
    float version = atof(temp);
    OutputHeightsList::getOutputHeightsListInstance()->setHeightsVersion(version);
    //qDebug()<<my_root1->Value();
    TiXmlElement *my_root2 = my_root1->FirstChildElement();
    while(my_root2)
    {
        // heightVal
        TiXmlAttribute * my_root2Attr1 = my_root2->FirstAttribute();
        const char* temp;
        temp = my_root2Attr1->Value();
        int heightVal = atoi(temp);//把字符串转换成整形数
        //  qDebug() << my_root2Attr1->Name() << heightVal;
        OutputHeightsList::getOutputHeightsListInstance()->heightsList_pushback(heightVal);
        my_root2 = my_root2->NextSiblingElement();
    }
    mutex.unlock();
    return true;
}

// 子类实现父类的函数
bool XMLSynthesisHeightsFileLoader::saveFile()
{
    mutex.lock();
    //生成xml文件
    TiXmlDocument *doc = new TiXmlDocument();
    TiXmlDeclaration dec("1.0","gb2312","yes");
    doc->InsertEndChild(dec);
    TiXmlElement *my_root1 = new TiXmlElement("synthesis_heights_config");
    float version = 1.0;
    my_root1->SetAttribute("version",version);

    doc->LinkEndChild(my_root1);

    list<int>::iterator it = OutputHeightsList::getOutputHeightsListInstance()->heightsBegin();
    while (it != OutputHeightsList::getOutputHeightsListInstance()->heightsEnd())
    {
        TiXmlElement *my_root2 = new TiXmlElement("heights");
        my_root2->SetAttribute("height",(*it));

        my_root1->LinkEndChild(my_root2);
        it++;
    }

    // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
    QByteArray tempstr = getFilename().toAscii();
    bool ret = doc->SaveFile(tempstr.constData());
    mutex.unlock();
    return ret;
}
