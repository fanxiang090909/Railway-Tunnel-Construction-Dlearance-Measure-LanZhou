#ifndef DAOSTATION_H
#define DAOSTATION_H

#include <QSqlQuery>
#include <QtSql/QSqlRelationalTableModel>
#include <QSqlQueryModel>
#include <QStringListModel>


enum {
    Station_ID = 0,
    Station_name_std = 1,
    Station_is_valid = 2
};

/**
 * 铁道车站数据访问对象类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-4
 */
class StationDAO : public QObject
{
    Q_OBJECT

public:
    explicit StationDAO(QObject *parent = 0);

    ~StationDAO();

    /**
     * 通过站名获得站的ID
     * @return stationID(>0), -1没有找到该站名
     */
    static int getStationID(QString stationName);

    /**
     * 是否存在车站
     * @return true有这个车站, false没有找到该站
     */
    static bool hasStation(int stationID);

    /**
     * 创建铁道车站，没有Stationid参数，因为加入数据库时采取主键自增策略
     * @return 新的stationid > 0
     */
    static int addStation(QString stationName, bool isValid);

    /**
     * 对铁路车站的增删改操作
     * @return 0 成功，1没有这个stationid号的station
     */
    static int updateStation(QString stationName, bool isValid);

    /**
     * 对铁路车站的删除操作，并非真正删除，即is_valid变为flase
     * @return 0 成功，1不存在该stationid号的station
     */
    static int removeStation(QString stationName);

private:

    // 私有方法创建station
    static bool createStation_P(QString stationName, bool isValid);

    // 私有方法更新station
    static bool updateStation_P(int stationid, QString stationName, bool isValid);
    // 私有方法更新station，只更新有效
    static bool updateStation_P(int stationid, bool isValid);

    // 私有方法删除station
    static bool deleteStation_P(int stationid);
};

#endif // DAOSTATION_H
