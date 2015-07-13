#ifndef DAOOUTPUTITEM_H
#define DAOOUTPUTITEM_H

#include <QObject>
#include <QSqlQuery>
#include <QSqlTableModel>

enum
{
    OutputItem_ID = 0,
    OutputItem_Output_ID = 1,
    OutputItem_Height = 2,
    OutputItem_Left_Val = 3,
    OutputItem_Right_Val = 4,
    OutputItem_Left_Pos = 5,
    OutputItem_Right_Pos = 6,
    OutputItem_Left_Radius = 7,
    OutputItem_Right_Radius = 8
};

/**
 * 建筑限界输出表格项-每行数据数据访问对象类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-11
 */
class OutputItemDAO : public QObject
{
    Q_OBJECT
private:
    // singleton设计模式，静态私有实例变量
    static OutputItemDAO * outputItemDAOInstance;

public:
    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static OutputItemDAO * getOutputItemDAOInstance();

    ~OutputItemDAO();

    /**
     * 是否有该outputitemid对应的线路数据库记录
     * @return true有，false没有
     */
    bool hasOutputItemID(_int64 outputitemid);

    /**
     * 创建输出表格项目，没有outputitemid参数，因为加入数据库时采取主键自增策略
     * @return 0 创建成功
     *         1 创建不成功 已经存在该输出表格
     *         2 创建不成功 outputid不存在
     */
    int addOutputItem(_int64 outputid, int height, float leftval, _int64 leftpos, float rightval, _int64 rightpos, int leftradius, int rightradius);

    /**
     * 更新输出表格项目
     * @return 0 更新成功
     *         1 更新不成功 当前outputid 及对应高度的outputitemid不存在
     *         2 更新不成功 outputid不存在
     */
    int updateOutputItem(_int64 outputid, int height, float leftval, _int64 leftpos, float rightval, _int64 rightpos, int leftradius, int rightradius);

private:

    // singleton单一设计模式，构造函数私有
    explicit OutputItemDAO(QObject *parent = 0);

    // 是否有这个ouputitem输出表格项目，return >0 outputitemid，-1没有
    _int64 getOutputItemID_P(_int64 outputid, int height);

    // 数据库中创建outputitem，没有outputitemid参数，因为加入数据库时采取主键自增策略
    bool createOutputItem_P(_int64 outputid, int height, float leftval, _int64 leftpos, float rightval, _int64 rightpos, int leftradius, int rightradius);

    // 数据库中修改outputitem
    bool updateOutputItem_P(_int64 outputitemid, _int64 outputid, int height, float leftval, _int64 leftpos, float rightval, _int64 rightpos, int leftradius, int rightradius);

    // 数据库中删除outputitem
    bool deleteOutputItem_P(_int64 outputitemid);

signals:

public slots:

};

#endif // DAOOUTPUTITEM_H
