#include "editing_list.h"
#include <QDebug>

/**
 * 正在修正隧道list类实现
 * @author fanxiang
 * @date 2014-3-27
 * @version 1.0.0
 */

// singleton设计模式，静态私有实例变量
EditingList * EditingList::editlistInstance = NULL;

EditingList * EditingList::getEditingListInstance()
{
    if (editlistInstance == NULL)
    {
        editlistInstance = new EditingList();
    }
    return editlistInstance;
}

EditingList::EditingList(QObject *parent) :
    QObject(parent)
{
    editingList = new QList<EditingItem>();
}

EditingList::~EditingList()
{
    delete editingList;
}

const QList<EditingItem>* EditingList::list()
{
    return editingList;
}

bool EditingList::pushBack(EditingItem slavetaskModel)
{
    editingList->push_back(slavetaskModel);
    return true;
}

/**
 * 删除某一item
 * @param deleteitem 待删除的项，只要projectname和tunnelname对应上，就算是
 * @return true 删除成功
 */
bool EditingList::remove(EditingItem deleteitem)
{
    for (int i = 0; i < editingList->length(); i++)
    {
        EditingItem tmp = editingList->at(i);
        if (deleteitem.tunnelid == tmp.tunnelid && deleteitem.projectname.compare(tmp.projectname) == 0 && deleteitem.tunnelname.compare(tmp.tunnelname) == 0)
        {
            editingList->removeAt(i);
            return true;
        }
    }
    return false;
}

void EditingList::clear()
{
    editingList->clear();
}

QList<EditingItem>::iterator EditingList::begin() { return editingList->begin(); }
QList<EditingItem>::iterator EditingList::end() { return editingList->end(); }

/**
 * 检查list中是否存在相同的条目
 * @param newitem 待检查的EditingItem
 * @param itemUser 输出正在修正的用户
 * @param startTime 输出正在修正的隧道的开始申请修正时间
 * @return true 存在
 */
bool EditingList::exist(EditingItem newitem, QString & itemUser, QString & startTime)
{
    for (int i = 0; i < editingList->length(); i++)
    {
        EditingItem tmp = editingList->at(i);
        if (newitem.tunnelid == tmp.tunnelid && newitem.projectname.compare(tmp.projectname) == 0 && newitem.tunnelname.compare(tmp.tunnelname) == 0)
        {
            itemUser = editingList->at(i).userid;
            startTime = editingList->at(i).begintime;
            return true;
        }
    }
    return false;
}
