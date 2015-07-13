#ifndef DAOCOURSE_H
#define DAOCOURSE_H

#include <QSqlQuery>
#include <QtSql/QSqlRelationalTableModel>
#include <QSqlQueryModel>
#include <QStringListModel>

#include "daostation.h"

/**
 * 数据库线路表头索引枚举类型
 */
enum {
    Line_ID = 0,
    Line_start_station_std = 1,
    Line_end_station_std = 2,
    Line_name_std = 3,
    Line_is_valid = 4,
    // @author 范翔 @date 20150527 添加，新旧线，内燃牵引电力牵引
    // 改为线路属性，同时保留其在隧道表格下的属性列
    Line_Tunnel_is_new_std = 5,
    Line_Tunnel_line_type_std = 6
};

/**
 * 铁道线路数据访问对象类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-8
 */
class LineDAO : public QObject
{
    Q_OBJECT

private :

    // singleton设计模式，静态私有实例变量
    static LineDAO * lineDAOInstance;

    QSqlRelationalTableModel * linesModel;
    QStringListModel * lineNamesModel;

public :

    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static LineDAO * getLineDAOInstance();

    ~LineDAO();

    /**
     * 所有线路模型
     */
    QSqlRelationalTableModel * getLines();

    /**
     * 所有线路名称
     */
    QStringListModel * getLineNames();

    /**
     * 是否有该lineid对应的线路数据库记录
     * @return true有，false没有
     */
    bool hasLineID(int lineid);

    int getLineTypeStd(int lineid);

    int getIsNewLineStd(int lineid);

    /**
     * 查找该linename对应的lineid
     * @return >0 lineid, -1未找到
     */
    int getLineID(QString linename);

    /**
     * 创建铁道线路，没有lineid参数，因为加入数据库时采取主键自增策略
     * @return 0-3 创建成功,  0起始终止站名都已存在创建成功；1起始站名未存在、创建成功；
     *                       2终止站名未存在、创建成功；3起始终止站名都未存在，创建成功
     *         4创建不成功 已经存在该线路
     */
    int addLine(QString lineName, bool isValid, QString startStation, QString endStation, int isNewLine, int lineType);

    /**
     * 更新铁道线路
     * @return 0-3 更新成功,  0起始终止站名都已存在创建成功；1起始站名未存在、创建成功；
     *                       2终止站名未存在、创建成功；3起始终止站名都未存在，创建成功
     *         4更新不成功，当前lineid不存在
     */
    int updateLine(int lineid, QString lineName, bool isValid, QString startStation, QString endStation, int isNewLine, int lineType);

    /**
     * 删除铁道线路，并非真正删除，即is_valid变为false
     * @return 0 成功，1不存在该line
     */
    int removeLine(int lineid);

private:

    // singleton单一设计模式，构造函数私有
    explicit LineDAO(QObject *parent = 0);

    // 是否有这条线路，return >0 lineid，-1没有
    int getLineID_P(QString lineName, int startStationID, int endStationID);

    // 数据库中创建新线路，没有lineid参数，因为加入数据库时采取主键自增策略
    bool createLine_P(QString lineName, bool isValid, int startStationID, int endStationID, int isNewLine, int lineType);

    // 数据库中修改line
    bool updateLine_P(int lineid, QString lineName, bool isValid, int startStationID, int endStationID, int isNewLine, int lineType);
    // 数据库中修改line
    bool updateLine_P(int lineid, bool isValid);

    // 数据库中删除line
    bool deleteLine_P(int lineid);

};

#endif // DAOCOURSE_H
