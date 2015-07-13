#include <QDebug>
#include "daooutputitem.h"
#include "daoclearanceoutput.h"

/**
 * 建筑限界输出表格项-每行数据数据访问对象类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-11
 */
// singleton设计模式，静态私有实例变量
OutputItemDAO * OutputItemDAO::outputItemDAOInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
OutputItemDAO * OutputItemDAO::getOutputItemDAOInstance()
{
    if (outputItemDAOInstance == NULL)
    {
        outputItemDAOInstance = new OutputItemDAO();
    }
    return outputItemDAOInstance;
}

// singleton单一设计模式，构造函数私有
OutputItemDAO::OutputItemDAO(QObject *parent) :
    QObject(parent)
{
}

OutputItemDAO::~OutputItemDAO()
{
}

/**
 * 是否有该outputitemid对应的线路数据库记录
 * @return true有，false没有
 */
bool OutputItemDAO::hasOutputItemID(_int64 outputitemid)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM output_item WHERE output_item_id = %1 "
               ).arg(outputitemid));

    while (query.next()) {
        return true;
    }

    qDebug() << "can not find" << outputitemid << " in table output_item!";
    return false;
}

/**
 * 创建输出表格项目，没有outputitemid参数，因为加入数据库时采取主键自增策略
 * @return 0 创建成功
 *         1 创建不成功 已经存在该输出表格项
 *         2 创建不成功 outputid不存在
 */
int OutputItemDAO::addOutputItem(_int64 outputid, int height, float leftval, _int64 leftpos, float rightval, _int64 rightpos, int leftradius, int rightradius)
{
    int ret = 0;
    if (getOutputItemID_P(outputid, height) > 0)
    {
        ret = 1;
        return ret;
    }
    if (ClearanceOutputDAO::getClearanceOutputDAOInstance()->hasOutputID(outputid))
    {
        createOutputItem_P(outputid, height, leftval, leftpos, rightval, rightpos, leftradius, rightradius);
        return ret;
    }
    ret = 2;
    qDebug() << "this outputid has not existed!";
    return ret;
}

/**
 * 更新输出表格项目
 * @return 0 更新成功
 *         1 更新不成功 当前outputid 及对应高度的outputitemid不存在
 *         2 更新不成功 outputid不存在
 */
int OutputItemDAO::updateOutputItem(_int64 outputid, int height, float leftval, _int64 leftpos, float rightval, _int64 rightpos, int leftradius, int rightradius)
{
    int ret = 0;
    if (!ClearanceOutputDAO::getClearanceOutputDAOInstance()->hasOutputID(outputid))
    {
        ret = 2;
        qDebug() << "this outputid has not existed!";
       return ret;
    }
    _int64 outputitemid = getOutputItemID_P(outputid, height);
    if (outputitemid < 0)
    {
        ret = 1;
        return ret;
    }
    updateOutputItem_P(outputitemid, outputid, height, leftval, leftpos, rightval, rightpos, leftradius, rightradius);
    return ret;
}

// 是否有这个ouputitem输出表格，return >0 outputitemid，-1没有
_int64 OutputItemDAO::getOutputItemID_P(_int64 outputid, int height)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM output_item WHERE output_id = %1 AND "
                       "height = %2 "
                  ).arg(outputid).arg(height));

    while (query.next()) {
        _int64 ouputitemid = query.value(0).toLongLong();
        return ouputitemid;
    }
    qDebug() << "can not find in table output_item!";
    return -1;
}

// 数据库中创建outputitem，没有outputitemid参数，因为加入数据库时采取主键自增策略
bool OutputItemDAO::createOutputItem_P(_int64 outputid, int height, float leftval, _int64 leftpos, float rightval, _int64 rightpos, int leftradius, int rightradius)
{
    QSqlQuery query;
    bool ret = query.exec(QString("INSERT INTO output_item (output_id, height, lval, rval, lpos, rpos, lradius, rradius) "
                       "VALUES (%1, %2, %3, %4, %5, %6, %7, %8) "
               ).arg(outputid).arg(height).arg(leftval).arg(rightval).arg(leftpos).arg(rightpos).arg(leftradius).arg(rightradius));
    qDebug() << "createOutputItem_P" << outputid << height << leftval << leftpos << rightval << rightpos << leftradius << rightradius << ret;
    return ret;
}

// 数据库中修改outputitem
bool OutputItemDAO::updateOutputItem_P(_int64 outputitemid, _int64 outputid, int height, float leftval, _int64 leftpos, float rightval, _int64 rightpos, int leftradius, int rightradius)
{
    QSqlQuery query;
    bool ret = query.exec(QString("UPDATE output_item SET lval = %1, lpos = %2, lradius = %3, rval = %4, rpos = %5, rradius = %6 "
                       "WHERE output_item_id = %7 AND output_id = %8 AND height = %9 "
              ).arg(leftval).arg(leftpos).arg(leftradius).arg(rightval).arg(rightpos).arg(rightradius).arg(outputitemid).arg(outputid).arg(height));
    qDebug() << "updateOutputItem_P" << outputitemid << outputid << height << leftval << leftpos << rightval << rightpos << leftradius << rightradius << ret;
    return ret;
}

// 数据库中删除outputitem
bool OutputItemDAO::deleteOutputItem_P(_int64 outputitemid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("DELETE FROM output_item WHERE ouput_item_id = %1").arg(outputitemid));
    qDebug() << "deleteOutputItem_P" << outputitemid << ret;
    return ret;
}
