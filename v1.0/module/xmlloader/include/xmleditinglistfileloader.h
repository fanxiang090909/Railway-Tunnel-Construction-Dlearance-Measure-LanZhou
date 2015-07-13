#ifndef XML_EDITING_LIST_FILELOADER_H
#define XML_EDITING_LIST_FILELOADER_H

#include "xmlfileloader.h"

/**
 * 服务器保存解析正在修正隧道列表
 * @author 范翔
 * @date 2014-4-5
 */
class XMLEditingListFileLoader : public XMLFileLoader
{
public:
    explicit XMLEditingListFileLoader (QString filename);
    ~XMLEditingListFileLoader ();
    //子类实现父类的纯虚函数
    bool saveFile();
    bool loadFile();
};

#endif // XML_EDITING_LIST_FILELOADER_H
