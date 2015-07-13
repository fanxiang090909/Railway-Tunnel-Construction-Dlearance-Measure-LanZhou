#ifndef XMLREALCOLLECTFILELOADER_H
#define XMLREALCOLLECTFILELOADER_H

#include "xmlfileloader.h"
#include "realtask_list.h"

/**
 * 实际采集任务记录的XML文件解析类声明
 * 继承自XMLFileLoader
 * @see XMLFileLoader
 * @author  熊雪 范翔
 * @date 20140105
 * @version 1.0.0
 */
class XMLRealTaskFileLoader : public XMLFileLoader
{
public:
    explicit XMLRealTaskFileLoader(QString filename);
    ~XMLRealTaskFileLoader();

    bool saveFile(RealTaskList & list);
    bool loadFile(RealTaskList & list);
};

#endif // XMLREALCOLLECTFILELOADER_H
