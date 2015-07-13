#include <QDebug>
#include "daostation.h"

/**
 * 车站数据访问对象类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-8
 */
// singleton单一设计模式，构造函数私有
StationDAO::StationDAO(QObject *parent) : QObject(parent)
{

}

StationDAO::~StationDAO()
{

}

/**
 * 通过站名获得站的ID
 * @return stationID(>0), -1没有找到该站名
 */
int StationDAO::getStationID(QString stationName)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM railway_station WHERE station_name_std = '%1' "
               ).arg(stationName));

    while (query.next()) {
        int stationid = query.value(0).toInt();
        return stationid;
    }

    qDebug() << stationName << "can not find in table railway_station!";
    return -1;
}

/**
 * 是否存在车站
 * @return true有这个车站, false没有找到该站
 */
bool StationDAO::hasStation(int stationID)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM railway_station WHERE station_id = %1 "
               ).arg(stationID));

    while (query.next()) {
        return true;
    }

    qDebug() << stationID << "can not find in table railway_station!";
    return false;
}

/**
 * 创建铁道车站，没有Stationid参数，因为加入数据库时采取主键自增策略
 * @return 新的stationid > 0
 */
int StationDAO::addStation(QString stationName, bool isValid)
{
    createStation_P(stationName, isValid);
    return getStationID(stationName);
}

/**
 * 对铁路车站的增删改操作
 * @return 0 成功，1没有这个stationid号的station
 */
int StationDAO::updateStation(QString stationName, bool isValid)
{
    int stationid = getStationID(stationName);
    if (stationid == -1)
    {
        qDebug() << "do not has the station name in table railway_station!";
        return 1;
    }
    else
        updateStation_P(stationid, stationName, isValid);
    return 0;
}

/**
 * 对铁路车站的删除操作
 * @return 0 成功，1不存在该stationid号的station
 */
int StationDAO::removeStation(QString stationName)
{
    int stationid = getStationID(stationName);
    if (stationid == -1)
    {
        qDebug() << "do not has the station name in table railway_station!";
        return 1;
    }
    else
        updateStation_P(stationid, false);
    return 0;
}

bool StationDAO::createStation_P(QString stationName, bool isValid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("INSERT INTO railway_station (station_name_std, is_valid) VALUES('%1', %2)"
               ).arg(stationName).arg(isValid));
    qDebug() << "createStation_P" << stationName << ret;
    return ret;
}

bool StationDAO::updateStation_P(int stationid, QString stationName, bool isValid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("UPDATE railway_station SET station_name_std = '%1', "
               "is_valid = %2 "
               "WHERE station_id = %3").arg(stationName).arg(isValid).arg(stationid));
    qDebug() << "updateStation_P" << stationid << stationName << ret;
    return ret;
}

bool StationDAO::updateStation_P(int stationid, bool isValid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("UPDATE railway_station SET is_valid = %1 "
               "WHERE station_id = %2").arg(isValid).arg(stationid));
    qDebug() << "updateStation_P" << stationid << ret;
    return ret;
}

bool StationDAO::deleteStation_P(int stationid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("DELETE FROM railway_station WHERE station_id = %1").arg(stationid));
    qDebug() << "delete_P" << stationid << ret;
    return ret;
}
