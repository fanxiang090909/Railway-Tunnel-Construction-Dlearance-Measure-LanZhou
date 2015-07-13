#ifndef XMLSYNTHESISHEIGHTSFILELOADER_H
#define XMLSYNTHESISHEIGHTSFILELOADER_H

#include "xmlfileloader.h"

/**
 * 隧道综合指定高度配置的XML文件解析类声明
 * 继承自XMLFileLoader
 * @see XMLFileLoader
 * @author fanxiang
 * @date 20140105
 * @version 1.0.0
 */
class XMLSynthesisHeightsFileLoader : public XMLFileLoader
{
public:
    XMLSynthesisHeightsFileLoader(QString filename);

    ~ XMLSynthesisHeightsFileLoader();

    // 子类实现父类的函数
    bool loadFile();

    // 子类实现父类的函数
    bool saveFile();
};

#endif // XMLSYNTHESISHEIGHTSFILELOADER_H
