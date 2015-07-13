#ifndef DAOCLEARANCEOUTPUT_H
#define DAOCLEARANCEOUTPUT_H

#include <QObject>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QStringListModel>
#include "daooutputitem.h"
#include "clearance_item.h"

/**
 * 建筑限界输出表格数据访问对象类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-11
 */
class ClearanceOutputDAO : public QObject
{
    Q_OBJECT

private:
    // singleton设计模式，静态私有实例变量
    static ClearanceOutputDAO * clearanceOutputDAOInstance;

    QStringListModel * taskTunnelOutputList;

    QSqlTableModel * clearanceOutputTableModel;

public:
    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static ClearanceOutputDAO* getClearanceOutputDAOInstance();

    ~ClearanceOutputDAO();

    /**
     * 限界值存入数据库表中
     * @return 0 存入数据库成功
     *         1 无法创建ClearanceOutput，因为已经存在该输出表格
     *         2 无法创建ClearanceOutput，因为taskTunnelid不存在
     *         3 创建表格项OutputItem不成功 已经存在该输出表格项
     *         4 创建表格项OutputItem不成功 outputid不存在
     */
    int clearanceDataToDBData(ClearanceData & clearancedata, _int64 taskTunnelid, ClearanceType curvetype);

    /**
     * 从数据库表中读出限界
     * @param clearancedata 待存储的限界
     */
    bool DBDataToClearanceData(ClearanceData & clearancedata, _int64 taskTunnelid, ClearanceType curvetype);

    /**
     * 限界表格及其具体行数据输出模型(1)
     */
    QSqlTableModel * getClearanceOutputTableModel(_int64 outputid);
    /**
     * 限界表格及其具体行数据输出模型(2)
     * 与上一个函数不同：给传入的QSqlTableModel赋值，返回值为void，防止多次调用，改变界面显示
     */
    void getClearanceOutputTableModel(_int64 outputid, QSqlTableModel * tempOutputTableModel);

    /**
     * 得到一次测量隧道的多个限界表格
     */
    QStringListModel * getTaskTunnelOutputList(_int64 taskTunnelid);

    /**
     * 得到中心最低净高数据及其位置
     * @param outputid 输入的outputid索引
     * @param 引用 minHeight, minHeightPos, minHeightTunnelID 均为输出值
     * @return 0 找到
     *         1 未找到outputid
     */
    int getOutputMinHeight(_int64 outputid, int & tunnelid, float & minHeight, double & minHeightPos, int & minHeightTunnelID);

    /**
     * 是否有该outputid对应的线路数据库记录
     * @return true有，false没有
     */
    bool hasOutputID(_int64 outputid);

    /**
     * 创建输出表格，没有outputid参数，因为加入数据库时采取主键自增策略
     * @return 0 创建成功
     *         1 创建不成功 已经存在该输出表格
     *         2 创建不成功 taskTunnelid不存在
     */
    int addOutput(_int64 taskTunnelid, ClearanceType type, float minHeight, __int64 minHeightPos, int minHeightTunnelID, QString description);

    /**
     * 更新输出表格
     * @return 0 更新成功
     *         1 更新不成功 当前outputid不存在
     *         2 更新不成功 taskTunnelid不存在
     */
    int updateOutput(_int64 outputid, _int64 taskTunnelid, ClearanceType type, float minHeight, __int64 minHeightPos, int minHeightTunnelID, QString description);

private:
    // singleton单一设计模式，构造函数私有
    explicit ClearanceOutputDAO(QObject *parent = 0);

    // 是否有这个ouput输出表格，return >0 outputid，-1没有
    _int64 getOutputID_P(_int64 taskTunnelid, ClearanceType type);

    // 数据库中创建output，没有outputid参数，因为加入数据库时采取主键自增策略
    bool createOutput_P(_int64 taskTunnelid, ClearanceType type, float minHeight, __int64 minHeightPos, int minHeightTunnelID, QString description);

    // 数据库中修改output
    bool updateOutput_P(_int64 outputid, _int64 taskTunnelid, ClearanceType type, float minHeight, __int64 minHeightPos, int minHeightTunnelID, QString description);

    // 数据库中删除output
    bool deleteOutput_P(_int64 outputid);

signals:

public slots:

};

#endif // DAOCLEARANCEOUTPUT_H
