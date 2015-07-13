#ifndef XML_CHECKED_TUNNEL_TASK_FILELOADER_H
#define XML_CHECKED_TUNNEL_TASK_FILELOADER_H

#include "xmlfileloader.h"
#include "checkedtask_list.h"

/**
 * 实际与计划隧道文件的xml文件生成与解析
 * @author 熊雪
 * @date 20131111
 * @author 范翔
 * @date 20140118
 * @version 1.0.0
 */
class XMLCheckedTaskFileLoader : public XMLFileLoader
{
public:
    explicit XMLCheckedTaskFileLoader(QString filename);
    XMLCheckedTaskFileLoader();//无参的构造函数
    
    ~XMLCheckedTaskFileLoader();

    // 父类函数的纯虚函数，由子类实现
    bool loadFile(CheckedTaskList & list);
    bool saveFile(CheckedTaskList & list);
};

#endif // XML_CHECKED_TUNNEL_TASK_FILELOADER_H
