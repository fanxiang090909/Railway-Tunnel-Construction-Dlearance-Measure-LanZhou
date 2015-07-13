#ifndef XMLTASKFILELOADER_H
#define XMLTASKFILELOADER_H

#include "xmlfileloader.h"

#include "plantask_list.h"

/**
 * 计划任务配置的XML读写类声明
 * @see XMLFileLoader
 * @author 熊雪 范翔
 * @date 2013-11-13
 */
class XMLTaskFileLoader : public XMLFileLoader
{
public:
    explicit XMLTaskFileLoader(QString filename);
    
    ~XMLTaskFileLoader();
    
    // 子类实现父类的纯虚函数
    bool saveFile(PlanTaskList & list);

    bool loadFile(PlanTaskList & list);
};

#endif // XMLTASKFILELOADER_H
