#include <QDebug>
#include <QSqlRecord>
#include "daoclearanceoutput.h"
#include "daotasktunnel.h"

/**
 * 建筑限界输出表格数据访问对象类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-11
 */
// singleton设计模式，静态私有实例变量
ClearanceOutputDAO * ClearanceOutputDAO::clearanceOutputDAOInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
ClearanceOutputDAO * ClearanceOutputDAO::getClearanceOutputDAOInstance()
{
    if (clearanceOutputDAOInstance == NULL)
    {
        clearanceOutputDAOInstance = new ClearanceOutputDAO();
    }
    return clearanceOutputDAOInstance;
}


// singleton单一设计模式，构造函数私有
ClearanceOutputDAO::ClearanceOutputDAO(QObject *parent) :
    QObject(parent)
{
    taskTunnelOutputList = new QStringListModel(this);
    clearanceOutputTableModel = new QSqlTableModel(this);
}

ClearanceOutputDAO::~ClearanceOutputDAO()
{
    delete taskTunnelOutputList;
    delete clearanceOutputTableModel;
}

/**
 * 限界值存入数据库表中
 * @return 0 存入数据库成功
 *         1 无法创建ClearanceOutput，因为已经存在该输出表格
 *         2 无法创建ClearanceOutput，因为taskTunnelid不存在
 *         3 创建表格项OutputItem不成功 已经存在该输出表格项
 *         4 创建表格项OutputItem不成功 outputid不存在
 */
int ClearanceOutputDAO::clearanceDataToDBData(ClearanceData & clearancedata, _int64 taskTunnelid, ClearanceType curvetype)
{
    _int64 outputid = getOutputID_P(taskTunnelid, curvetype);
    if (outputid < 0)
    {
        qDebug() << "[warning] clearancedata to dbdata: can not find outputid where task_tunnel_id= " << taskTunnelid << ", curvetype=" << curvetype << "! add it to clearance_output";
        // 找不到OutputID就加入Outputid
        int ret = addOutput(taskTunnelid, curvetype, clearancedata.getMinCenterHeight(), clearancedata.getMinCenterHeightPos(), clearancedata.getMinCenterHeightTunnelID(), "");
        if (ret != 0) // 如果仍然无法加入
            return ret;
        else
        {
            outputid = getOutputID_P(taskTunnelid, curvetype);
            updateOutput(outputid, taskTunnelid, curvetype, clearancedata.getMinCenterHeight(), clearancedata.getMinCenterHeightPos(), clearancedata.getMinCenterHeightTunnelID(), "");
        }
    }
    else
        updateOutput(outputid, taskTunnelid, curvetype, clearancedata.getMinCenterHeight(), clearancedata.getMinCenterHeightPos(), clearancedata.getMinCenterHeightTunnelID(), "");

    int tmpheight;
    float tmplval;
    float tmprval;
    _int64 tmplpos;
    _int64 tmprpos;
    int tmplradius;
    int tmprradius;
    int tmpret;
    std::map<int, ClearanceItem>::iterator it = clearancedata.getMaps().begin();
    while (it != clearancedata.getMaps().end())
    {
        tmpheight = (it->first);
        tmplval = (it->second).leftval;
        tmprval = (it->second).rightval;
        tmplpos = (it->second).leftpos;
        tmprpos = (it->second).rightpos;
		tmplradius = (it->second).leftradius;
		tmprradius = (it->second).rightradius;
        tmpret = OutputItemDAO::getOutputItemDAOInstance()->addOutputItem(outputid, tmpheight, tmplval, tmplpos, tmprval, tmprpos, tmplradius, tmprradius);
        if (tmpret != 0)
        {
            switch (tmpret)
            {
                case 1:OutputItemDAO::getOutputItemDAOInstance()->updateOutputItem(outputid, tmpheight, tmplval, tmplpos, tmprval, tmprpos, tmplradius, tmprradius);
                       it++; continue;
                case 2:return 4;
            }
        }
        it++;
    }

    return 0;
}

/**
 * 从数据库表中读出限界
 * @param clearancedata 待存储的限界
 */
bool ClearanceOutputDAO::DBDataToClearanceData(ClearanceData & clearancedata, _int64 taskTunnelid, ClearanceType curvetype)
{
    _int64 outputid = getOutputID_P(taskTunnelid, curvetype);
    if (outputid < 0) // 如果没有这个表
    {
        qDebug() << "[warning] dbdata to clearancedata: can not find outputid! where task_tunnel_id= " << taskTunnelid << ", curvetype=" << curvetype;
        return false;
    }
    qDebug() << "dbdata to clearancedata: find outputid=" << outputid << "! where task_tunnel_id= " << taskTunnelid << ", curvetype=" << curvetype;;

    clearancedata.setType(curvetype);
    float minHeight;
    double minHeightPos;
    int minHeightTunnelid;
    int tmptunnelid;
    getOutputMinHeight(outputid, tmptunnelid, minHeight, minHeightPos, minHeightTunnelid);
    clearancedata.setMinCenterHeight(minHeight);
    clearancedata.setMinCenterHeightPos(minHeightPos);
    clearancedata.setMinCenterHeightTunnelID(tmptunnelid);
    clearancedata.setTunnelID(tmptunnelid);

    QSqlTableModel * outputTable = new QSqlTableModel();
    // 查找数据库，将结果赋给outputTable
    this->getClearanceOutputTableModel(outputid, outputTable);

    int tmpheight;
    float tmplval;
    float tmprval;
    _int64 tmplpos;
    _int64 tmprpos;
    int tmplradius;
    int tmprradius;
    int tmpltunnelid = -1;
    int tmprtunnelid = -1;
    for (int i = 0; i < outputTable->rowCount(); i++)
    {
        QSqlRecord record = outputTable->record(i);
        tmpheight = record.value(OutputItem_Height).toInt();
        tmplval = record.value(OutputItem_Left_Val).toFloat();
        tmprval = record.value(OutputItem_Right_Val).toFloat();
        tmplpos = record.value(OutputItem_Left_Pos).toLongLong();
        tmprpos = record.value(OutputItem_Right_Pos).toLongLong();
        tmplradius = record.value(OutputItem_Left_Radius).toInt();
        tmprradius = record.value(OutputItem_Right_Radius).toInt();
        // clearancedata.updateToMapVals(tmpheight, tmplval, tmplpos, tunnelid, tmplradius, true);
        // clearancedata.updateToMapVals(tmpheight, tmprval, tmprpos, tunnelid, tmprradius, false);
        qDebug() << "dbdata to clearancedata(curve type=" << curvetype << "):" << tmpheight << tmprval << tmplval << tmprpos << tmplpos << tmpltunnelid << tmprtunnelid;
        bool ret = clearancedata.updateToMapVals(tmpheight, tmplval, tmprval, tmplpos, tmprpos, tmpltunnelid, tmprtunnelid, tmplradius, tmprradius);
        if (ret == false)
        {
            qDebug() << "can not translate to clearance data from dbdata";
            continue;
        }
    }
    //clearancedata.showMaps();
    delete outputTable;

    return true;
}

/**
 * 限界表格及其具体行数据输出模型(1)
 */
QSqlTableModel * ClearanceOutputDAO::getClearanceOutputTableModel(_int64 outputid)
{
    clearanceOutputTableModel->setTable("output_item");

    clearanceOutputTableModel->setFilter(QObject::tr("output_id = %1").arg(outputid));

    clearanceOutputTableModel->setSort(OutputItem_Height, Qt::DescendingOrder);

    clearanceOutputTableModel->setHeaderData(OutputItem_ID, Qt::Horizontal, tr("输出表项ID"));
    clearanceOutputTableModel->setHeaderData(OutputItem_Output_ID, Qt::Horizontal, tr("输出表格ID"));
    clearanceOutputTableModel->setHeaderData(OutputItem_Height, Qt::Horizontal, tr("距轨面高度mm"));
    clearanceOutputTableModel->setHeaderData(OutputItem_Left_Val, Qt::Horizontal, tr("左限界"));
    clearanceOutputTableModel->setHeaderData(OutputItem_Right_Val, Qt::Horizontal, tr("右限界"));
    clearanceOutputTableModel->setHeaderData(OutputItem_Left_Pos, Qt::Horizontal, tr("左控制点"));
    clearanceOutputTableModel->setHeaderData(OutputItem_Right_Pos, Qt::Horizontal, tr("右控制点"));
    clearanceOutputTableModel->setHeaderData(OutputItem_Left_Radius, Qt::Horizontal, tr("左侧半径"));
    clearanceOutputTableModel->setHeaderData(OutputItem_Right_Radius, Qt::Horizontal, tr("右侧半径"));
    clearanceOutputTableModel->select();

    //qDebug() << "getClearanceOutputTableModel" << clearanceOutputTableModel->rowCount() << clearanceOutputTableModel->columnCount();
    return clearanceOutputTableModel;
}

/**
 * 限界表格及其具体行数据输出模型(2)
 * 与上一个函数不同：给传入的QSqlTableModel赋值，返回值为void，防止多次调用，改变界面显示
 */
void ClearanceOutputDAO::getClearanceOutputTableModel(_int64 outputid, QSqlTableModel * tempOutputTableModel)
{
    if (tempOutputTableModel == NULL)
    {
        qDebug() << "null pointer tempOutputTableModel, cannot assign to it!";
        return;
    }
    tempOutputTableModel->setTable("output_item");

    tempOutputTableModel->setFilter(QObject::tr("output_id = %1").arg(outputid));

    tempOutputTableModel->setSort(OutputItem_Height, Qt::DescendingOrder);

    tempOutputTableModel->setHeaderData(OutputItem_ID, Qt::Horizontal, tr("输出表项ID"));
    tempOutputTableModel->setHeaderData(OutputItem_Output_ID, Qt::Horizontal, tr("输出表格ID"));
    tempOutputTableModel->setHeaderData(OutputItem_Height, Qt::Horizontal, tr("距轨面高度mm"));
    tempOutputTableModel->setHeaderData(OutputItem_Left_Val, Qt::Horizontal, tr("左限界"));
    tempOutputTableModel->setHeaderData(OutputItem_Right_Val, Qt::Horizontal, tr("右限界"));
    tempOutputTableModel->setHeaderData(OutputItem_Left_Pos, Qt::Horizontal, tr("左控制点"));
    tempOutputTableModel->setHeaderData(OutputItem_Right_Pos, Qt::Horizontal, tr("右控制点"));
    tempOutputTableModel->setHeaderData(OutputItem_Left_Radius, Qt::Horizontal, tr("左侧半径"));
    tempOutputTableModel->setHeaderData(OutputItem_Right_Radius, Qt::Horizontal, tr("右侧半径"));

    tempOutputTableModel->select();

    //qDebug() << "getClearanceOutputTableModel" << clearanceOutputTableModel->rowCount() << clearanceOutputTableModel->columnCount();
    return;
}

/**
 * 是否有该outputid对应的线路数据库记录
 * @return true有，false没有
 */
bool ClearanceOutputDAO::hasOutputID(_int64 outputid)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM clearance_output WHERE output_id = %1 "
               ).arg(outputid));

    while (query.next()) {
        return true;
    }

    qDebug() << outputid << "can not find in table clearance_output!";
    return false;
}

/**
 * 创建输出表格，没有outputid参数，因为加入数据库时采取主键自增策略
 * @return 0 创建成功
 *         1 创建不成功 已经存在该输出表格
 *         2 创建不成功 taskTunnelid不存在
 */
int ClearanceOutputDAO::addOutput(_int64 taskTunnelid, ClearanceType type, float minHeight, __int64 minHeightPos, int minHeightTunnelID, QString description)
{
    int ret = 0;
    if (getOutputID_P(taskTunnelid, type) > 0)
    {
        ret = 1;
        return ret;
    }
    if (TaskTunnelDAO::getTaskTunnelDAOInstance()->hasTaskTunnelID(taskTunnelid))
    {
        createOutput_P(taskTunnelid, type, minHeight, minHeightPos, minHeightTunnelID, description);
        return ret;
    }
    ret = 2;
    qDebug() << "this taskTunnel has not existed!";
    return ret;
}

/**
 * 更新输出表格
 * @return 0 更新成功
 *         1 更新不成功 当前outputid不存在
 *         2 更新不成功 taskTunnelid不存在
 */
int ClearanceOutputDAO::updateOutput(_int64 outputid, _int64 taskTunnelid, ClearanceType type, float minHeight, __int64 minHeightPos, int minHeightTunnelID, QString description)
{
    int ret = 0;
    if (!hasOutputID(outputid))
    {
        ret = 1;
        return ret;
    }
    if (TaskTunnelDAO::getTaskTunnelDAOInstance()->hasTaskTunnelID(taskTunnelid))
    {
        updateOutput_P(outputid, taskTunnelid, type, minHeight, minHeightPos, minHeightTunnelID, description);
        return ret;
    }
    ret = 2;
    qDebug() << "this taskTunnel has not existed!";
    return ret;
}

QStringListModel * ClearanceOutputDAO::getTaskTunnelOutputList(_int64 taskTunnelid)
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT DISTINCT o.description FROM clearance_output o "
                           "WHERE o.task_tunnel_id = %1 ORDER BY o.description DESC").arg(taskTunnelid));

    QStringList strings;
    while (query.next()) {
        QString line = query.value(0).toString();
        strings << line;
    }
    taskTunnelOutputList->setStringList(strings);
    return taskTunnelOutputList;
}

/**
 * 得到中心最低净高数据及其位置
 * @param outputid 输入的outputid索引
 * @param 引用 minHeight, minHeightPos, minHeightTunnelID 均为输出值
 * @return 0 找到
 *         1 未找到outputid
 */
int ClearanceOutputDAO::getOutputMinHeight(_int64 outputid, int & tunnelid, float & minHeight, double & minHeightPos, int & minHeightTunnelID)
{
    QSqlQuery query;
    query.exec(QString("SELECT t.tunnel_id, co.min_height, co.min_height_pos, co.min_height_tunnelid "
                        "FROM clearance_output co " 
                        "JOIN task_tunnel tt ON co.task_tunnel_id = tt.task_tunnel_id "
                        "JOIN tunnel t ON t.tunnel_id = tt.tunnel_id "
                        "WHERE co.output_id = %1").arg(outputid));

    while (query.next()) {
        tunnelid = query.value(0).toInt();
        minHeight = query.value(1).toFloat();
        minHeightPos = query.value(2).toDouble();
        minHeightTunnelID = query.value(3).toInt();
        return 0;
    }
    return 1;
}

// 是否有这个ouput输出表格，return >0 outputid，-1没有
_int64 ClearanceOutputDAO::getOutputID_P(_int64 taskTunnelid, ClearanceType type)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM clearance_output WHERE task_tunnel_id = %1 AND "
                       "output_type = %2 "
                  ).arg(taskTunnelid).arg(type));

    while (query.next()) {
        _int64 ouputid = query.value(0).toLongLong();
        return ouputid;
    }
    // qDebug() << "can not find tasktunnelid" << taskTunnelid << "and output type" << type << " in table clearance_output!";
    return -1;
}

// 数据库中创建output，没有outputid参数，因为加入数据库时采取主键自增策略
bool ClearanceOutputDAO::createOutput_P(_int64 taskTunnelid, ClearanceType type, float minHeight, __int64 minHeightPos, int minHeightTunnelID, QString description)
{
    QSqlQuery query;
    bool ret = query.exec(QString("INSERT INTO clearance_output (task_tunnel_id, output_type, min_height, min_height_pos, min_height_tunnelid, description) "
                       "VALUES (%1, %2, %3, %4, %5, '%6') "
                       ).arg(taskTunnelid).arg(type).arg(minHeight).arg(minHeightPos).arg(minHeightTunnelID).arg(description));
    qDebug() << "createOutput_P" << taskTunnelid << description << ret;
    return ret;
}

// 数据库中修改output
bool ClearanceOutputDAO::updateOutput_P(_int64 outputid, _int64 taskTunnelid, ClearanceType type, float minHeight, __int64 minHeightPos, int minHeightTunnelID, QString description)
{
    QSqlQuery query;
    bool ret = query.exec(QString("UPDATE clearance_output SET task_tunnel_id = %1, "
               "output_type = %2, min_height = %3, min_height_pos = %4, min_height_tunnelid = %5, description = '%6' "
               "WHERE output_id = %7").arg(taskTunnelid).arg(type).arg(minHeight).arg(minHeightPos).arg(minHeightTunnelID).arg(description).arg(outputid));
    qDebug() << "updateOutput_P" << outputid << taskTunnelid << type << minHeight << ret;
    return ret;
}

// 数据库中删除output
bool ClearanceOutputDAO::deleteOutput_P(_int64 outputid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("DELETE FROM clearance_output WHERE ouput_id = %1").arg(outputid));
    qDebug() << "deleteOutput_P" << outputid << ret;
    return ret;
}
