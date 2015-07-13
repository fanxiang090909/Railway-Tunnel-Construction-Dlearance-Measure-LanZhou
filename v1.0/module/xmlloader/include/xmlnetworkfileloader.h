#ifndef XMLNETWORKFILELOADER_H
#define XMLNETWORKFILELOADER_H

#include "xmlfileloader.h"

/**
 * CameraSNid的XML文件解析声明
 * 继承自XMLFileLoader
 * @see XMLFileLoader
 * @author xiongxue
 * @author fanxiang
 * @date 20131107
 * @version 1.0.0
 */
class XMLNetworkFileLoader : public XMLFileLoader
{
public:
    explicit XMLNetworkFileLoader(QString filename);

    ~XMLNetworkFileLoader();

    // 子类实现父类的函数
    bool loadFile();

    // 子类实现父类的函数
    bool saveFile();

};

#endif // XMLNETWORKFILELOADER_H
