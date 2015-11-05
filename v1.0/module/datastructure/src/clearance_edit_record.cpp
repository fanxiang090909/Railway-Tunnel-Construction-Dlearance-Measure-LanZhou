#include "clearance_edit_record.h"

#include "lz_logger.h"
#include <QDebug>

/**
 * 某个限界的编辑操作记录list类定义
 * @author fanxiang
 * @date 2014-6-15
 * @version 1.0.0
 */
EditRecordStack::EditRecordStack()
{
    logger = NULL;
    hasinitlog = false;
}

EditRecordStack::~EditRecordStack()
{
    if (logger != NULL)
    {
        if (logger->isLogging())
            logger->close();
        delete logger;
    }
}

/**
 * 初始化日志输出类
 */
bool EditRecordStack::initLogger(string filename, string username)
{
    if (logger != NULL)
    {
        if (logger->isLogging())
            logger->close();
        delete logger;
    }

    logger = new LzLogger(LzLogger::SynthesisCorrect);
    logger->setFilename(filename);
    if (logger->open() == 0)
    {
        hasinitlog = true;
        logger->log(string("正在修正用户") + username);
        return true;
    }
    else
    {
        hasinitlog = false;
        return false;
    }
}

/**
 * 是否还有剩余操作未保存
 */
bool EditRecordStack::isEmpty()
{
    return editingStack.empty();
}

/**
 * 加入新的修改操作
 * （1）插入到队列尾部
 */
bool EditRecordStack::addOneStep(EditRecord item)
{
    editingStack.push(item);
    return false;
}

/**
 * 全部保存已经修改的内容
 * （1）写入修正记录日志
 * （2）清空list
 */
bool EditRecordStack::saveAll(__int64 currentframecounter, float currentmile, bool carriagedirection)
{
    int i = 0;
    while (!editingStack.empty())
    {
        EditRecord item = editingStack.top();
        qDebug() << "save all" << item.height << ", isleft=" << item.isleft << ", old value=" << item.oldval << ", new value=" << item.newval; 
        if (hasinitlog)
        {
            if (item.height > 0)
            {
                QString left; 
                // 日志记录的的左右方向符合人眼观察。与车厢正向反向相关
                if ((carriagedirection && item.isleft) || (!carriagedirection && item.isleft))
                    left = QObject::tr("左");
                else
                    left = QObject::tr("右");
                QByteArray tmpba = (QString("修改帧号%1，里程%2的数据：高度%3的%4侧原值%5到%6").arg(currentframecounter).arg(currentmile).arg(item.height).arg(left).arg(item.oldval).arg(item.newval)).toLocal8Bit();
                logger->log(tmpba.constData());
            }
            else
            {
                QByteArray tmpba = (QString("修改帧号%1，里程%2的数据：中心高度原值%3为无效").arg(currentframecounter).arg(currentmile).arg(item.oldval)).toLocal8Bit();
                logger->log(tmpba.constData());
            }
        }
        editingStack.pop();
    }
    return false;
}

/**
 * 全部撤销原来操作
 * （1）清空list
 */
void EditRecordStack::cancelAll(SectionData &data)
{
    while (!editingStack.empty())
    {
        EditRecord item = editingStack.top();
        if (item.height < -1) // 表示中心高度
           data.setCenterHeight(item.oldval);
        else        
           data.updateToMapVals(item.height, item.oldval, item.isleft);

        editingStack.pop();
    }
}

/**
 * 撤销前一操作，重新把旧值赋给data中
 * @param data 同时作为输入和输出：输入已经修改好的限界
 *                                 输出撤销后的限界
 * @return true 撤销一步成功
 *         false 前面没有更多，不能再撤销了
 */
bool EditRecordStack::cancelOneStep(SectionData &data)
{
    if (!editingStack.empty())
    {
        EditRecord item = editingStack.top();
        if (item.height > 0)
            data.updateToMapVals(item.height, item.oldval, item.isleft);
        else
            data.setCenterHeight(item.oldval);
        editingStack.pop();

        return true;
    }
    else
        return false;
}

LzLogger * EditRecordStack::getLogger()
{
    return logger;
}