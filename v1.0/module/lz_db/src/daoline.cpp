#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QDebug>

#include "daoline.h"
#include "daostation.h"

/**
 * 铁道线路数据访问对象类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-08-08
 */
// singleton设计模式，静态私有实例变量
LineDAO * LineDAO::lineDAOInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
LineDAO * LineDAO::getLineDAOInstance()
{
    if (lineDAOInstance == NULL)
    {
        lineDAOInstance = new LineDAO();
    }
    return lineDAOInstance;
}

// singleton单一设计模式，构造函数私有
LineDAO::LineDAO(QObject *parent) : QObject(parent)
{
    linesModel = new QSqlRelationalTableModel(this);
    lineNamesModel = new QStringListModel(this);
}

LineDAO::~LineDAO()
{
    delete linesModel;
    delete lineNamesModel;
}

/**
 * 所有线路模型
 */
QSqlRelationalTableModel * LineDAO::getLines()
{
    linesModel->setTable("railway_line");

    linesModel->setRelation(Line_start_station_std,
            QSqlRelation("railway_station", "station_id", "station_name_std"));

    linesModel->setRelation(Line_end_station_std,
            QSqlRelation("railway_station", "station_id", "station_name_std"));
    // 【注意】!!!
    // 当使用了SqlRelationalTableModel的setRelation()后，再使用setFilter时就要注意了，
    linesModel->setFilter("railway_line.is_valid = TRUE");
    linesModel->setHeaderData(Line_ID, Qt::Horizontal, tr("线路ID"));
    linesModel->setHeaderData(Line_name_std, Qt::Horizontal, tr("线路名称"));
    linesModel->setHeaderData(Line_start_station_std, Qt::Horizontal, tr("起始站点"));
    linesModel->setHeaderData(Line_end_station_std, Qt::Horizontal, tr("终止站点"));
    linesModel->setHeaderData(Line_is_valid, Qt::Horizontal, tr("是否被删除作废"));
    linesModel->setHeaderData(Line_Tunnel_is_new_std, Qt::Horizontal, tr("新旧线"));
    linesModel->setHeaderData(Line_Tunnel_line_type_std, Qt::Horizontal, tr("牵引类型"));

    linesModel->select();

    return linesModel;
}

/**
 * 所有线路名称
 */
QStringListModel * LineDAO::getLineNames()
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT DISTINCT l.line_name_std "
               "FROM railway_line l "
               "WHERE is_valid = TRUE "
               "ORDER BY line_id ASC"));

    QStringList strings;
    while (query.next()) {
        QString line = query.value(0).toString();
        strings << line;
    }
    lineNamesModel->setStringList(strings);
    return lineNamesModel;
}

/**
 * 是否有该lineid对应的线路数据库记录
 * @return true有，false没有
 */
bool LineDAO::hasLineID(int lineid)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM railway_line WHERE line_id = %1 "
               ).arg(lineid));

    while (query.next()) {
        return true;
    }

    qDebug() << lineid << "can not find in table railway_line!";
    return false;
}

int LineDAO::getLineTypeStd(int lineid)
{
    QSqlQuery query;
    query.exec(QString("SELECT line_type_std FROM railway_line WHERE line_id = %1 "
               ).arg(lineid));

    while (query.next()) {
        return query.value(0).toInt();
    }

    qDebug() << lineid << "can not find in table railway_line!";
    return -1;
}

int LineDAO::getIsNewLineStd(int lineid)
{
    QSqlQuery query;
    query.exec(QString("SELECT is_new_std FROM railway_line WHERE line_id = %1 "
               ).arg(lineid));

    while (query.next()) {
        return query.value(0).toInt();
    }

    qDebug() << lineid << "can not find in table railway_line!";
    return -1;
}

/**
 * 查找该linename对应的lineid
 * @return >0 lineid, -1未找到
 */
int LineDAO::getLineID(QString linename)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM railway_line WHERE line_name_std = '%1' "
                       ).arg(linename));

    while (query.next()) {
        int lineid = query.value(0).toInt();
        //qDebug() << lineid;
        return lineid;

    }

    qDebug() << QObject::tr("can not find in table railway_line!");
    return -1;
}

/**
 * 创建铁道线路，没有lineid参数，因为加入数据库时采取主键自增策略
 * @return 0-3 创建成功,  0起始终止站名都已存在创建成功；1起始站名未存在、创建成功；
 *                       2终止站名未存在、创建成功；3起始终止站名都未存在，创建成功
 *         4创建不成功 已经存在这个线路
 */
int LineDAO::addLine(QString lineName, bool isValid, QString startStation, QString endStation, int isNewLine, int lineType)
{
    int ret = 0;
    int startStationID = StationDAO::getStationID(startStation);
    int endStationID = StationDAO::getStationID(endStation);
    int lineid = getLineID_P(lineName, startStationID, endStationID);
    if (lineid < 0)
    {
        if (startStationID < 0)
        {
            startStationID = StationDAO::addStation(startStation, true);
            ret += 1;
        }
        if (endStationID < 0)
        {
            endStationID = StationDAO::addStation(endStation, true);
            ret += 2;
        }
        createLine_P(lineName, isValid, startStationID, endStationID, isNewLine, lineType);
        return ret;
    }
    ret = 4;
    qDebug() << "this line has existed!";
    return ret;
}

/**
 * 更新铁道线路
 * @return 0-3 更新成功,  0起始终止站名都已存在创建成功；1起始站名未存在、创建成功；
 *                       2终止站名未存在、创建成功；3起始终止站名都未存在，创建成功
 *         4更新不成功，当前lineid不存在
 */
int LineDAO::updateLine(int lineid, QString lineName, bool isValid, QString startStation, QString endStation, int isNewLine, int lineType)
{
    if (!hasLineID(lineid))
    {
         qDebug() << "this line has not existed!";
         return 4;
    }
    int ret = 0;
    int startStationID = StationDAO::getStationID(startStation);
    int endStationID = StationDAO::getStationID(endStation);
    if (startStationID < 0)
    {
        startStationID = StationDAO::addStation(startStation, true);
        ret += 1;
    }
    if (endStationID < 0)
    {
        endStationID = StationDAO::addStation(endStation, true);
        ret += 2;
    }
    updateLine_P(lineid, lineName, isValid, startStationID, endStationID, isNewLine, lineType);
    return ret;
}

/**
 * 删除铁道线路，并非真正删除，即is_valid变为false
 * @return 0 成功，1不存在该line
 */
int LineDAO::removeLine(int lineid)
{
    if (hasLineID(lineid) == false)
    {
        qDebug() << "do not has the line id in table railway_station!";
        return 1;
    }
    else
        this->updateLine_P(lineid, false);
    return 0;
}

// 是否有这条线路，return >0 lineid，-1没有
int LineDAO::getLineID_P(QString lineName, int startStationID, int endStationID)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM railway_line WHERE line_name_std = '%1', "
                       "start_station_id = %2, end_station_id = %3 "
                       ).arg(lineName).arg(startStationID).arg(endStationID));

    while (query.next()) {
        int lineid = query.value(0).toInt();
        return lineid;
    }

    qDebug() << lineName << QObject::tr("can not find in table railway_line!");
    return -1;
}

bool LineDAO::createLine_P(QString lineName, bool isValid, int startStationID, int endStationID, int isNewLine, int lineType)
{
    QSqlQuery query;
    bool ret = query.exec(QString("INSERT INTO railway_line (line_name_std, is_valid, start_station_id, end_station_id, is_new_std, line_type_std) "
                       "VALUES ('%1', %2, %3, %4, %5, %6) "
               ).arg(lineName).arg(isValid).arg(startStationID).arg(endStationID).arg(isNewLine).arg(lineType));
    qDebug() << "createLine_P" << lineName << ret;
    return ret;
}

bool LineDAO::updateLine_P(int lineid, QString lineName, bool isValid, int startStationID, int endStationID, int isNewLine, int lineType)
{
    QSqlQuery query;
    bool ret = query.exec(QString("UPDATE railway_line SET line_name_std = '%1', "
               "is_valid = %2, start_station_id = %3, end_station_id = %4, is_new_std = %5, line_type_std = %6 "
               "WHERE line_id = %7").arg(lineName).arg(isValid).arg(startStationID).arg(endStationID).arg(isNewLine).arg(lineType).arg(lineid));
    qDebug() << "updateLine_P" << lineid << lineName << ret;
    return ret;
}

bool LineDAO::updateLine_P(int lineid, bool isValid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("UPDATE railway_line SET is_valid = %1 "
               "WHERE line_id = %2").arg(isValid).arg(lineid));
    qDebug() << "updateLine_P" << lineid << ret;
    return ret;
}

bool LineDAO::deleteLine_P(int lineid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("DELETE FROM railway_line WHERE line_id = %1").arg(lineid));
    qDebug() << "deleteLine_P" << lineid << ret;
    return ret;
}
