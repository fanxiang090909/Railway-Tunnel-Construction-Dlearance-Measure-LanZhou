#include <QStandardItemModel>
#include <QStringListModel>
#include <QSqlQuery>
#include <QDebug>

#include "daotunnel.h"
#include "daoline.h"

/**
 * 隧道数据访问对象类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-8
 */

// singleton设计模式，静态私有实例变量
TunnelDAO * TunnelDAO::tunnelDAOInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
TunnelDAO * TunnelDAO::getTunnelDAOInstance()
{
    if (tunnelDAOInstance == NULL)
    {
        tunnelDAOInstance = new TunnelDAO();
    }
    return tunnelDAOInstance;
}

// singleton单一设计模式，构造函数私有
TunnelDAO::TunnelDAO(QObject *parent) : QObject(parent)
{
    tunnelsModel = new QSqlRelationalTableModel(this);
    tunnelsModel_2 = new QSqlRelationalTableModel(this);
    tunnelNamesModel = new QStringListModel(this);
    exactTunnelModel = new QSqlQueryModel(this);
}

TunnelDAO::~TunnelDAO()
{
    delete tunnelsModel;
    delete tunnelsModel_2;
    delete tunnelNamesModel;
    delete exactTunnelModel;
}

/**
 * 根据隧道ID得到隧道名
 */
QString TunnelDAO::getOneTunnelName(int tunnelid)
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT DISTINCT t.tunnel_name_std "
               "FROM tunnel t WHERE t.tunnel_id = %1 ").arg(tunnelid));

    QStringList strings;
    while (query.next()) {
        return query.value(0).toString();
    }
    return "";
}

/**
 * 某一线路上的隧道模型，排序
 */
QSqlRelationalTableModel * TunnelDAO::getTunnels(int lineid)
{
    tunnelsModel->setTable("tunnel");

    tunnelsModel->setRelation(Tunnel_line_ID,
            QSqlRelation("railway_line", "line_id", "line_name_std"));
    // 【注意】!!!
    // 当使用了SqlRelationalTableModel的setRelation()后，再使用setFilter时就要注意了，
    // 因为SqlRelationalTableModel类中已经包含了多张表，所以在写过滤规则时，必须加上表名
    tunnelsModel->setFilter(QString("tunnel.line_id = %1 AND tunnel.is_valid = TRUE").arg(lineid));

    tunnelsModel->setSort(Tunnel_start_point_std, Qt::AscendingOrder);

    tunnelsModel->setHeaderData(Tunnel_ID, Qt::Horizontal, tr("隧道ID"));
    tunnelsModel->setHeaderData(Tunnel_name_std, Qt::Horizontal, tr("隧道名称"));
    tunnelsModel->setHeaderData(Tunnel_is_valid, Qt::Horizontal, tr("是否被删除作废"));
    tunnelsModel->setHeaderData(Tunnel_ID_std, Qt::Horizontal, tr("隧道编号"));
    tunnelsModel->setHeaderData(Tunnel_start_point_std, Qt::Horizontal, tr("起始里程"));
    tunnelsModel->setHeaderData(Tunnel_end_point_std, Qt::Horizontal, tr("终止里程"));
    tunnelsModel->setHeaderData(Tunnel_is_new_std, Qt::Horizontal, tr("是否为新线"));
    tunnelsModel->setHeaderData(Tunnel_line_type_std, Qt::Horizontal, tr("线路类型"));
    tunnelsModel->setHeaderData(Tunnel_is_double_line, Qt::Horizontal, tr("是否为双线"));
    tunnelsModel->setHeaderData(Tunnel_is_downlink, Qt::Horizontal, tr("是否为下行(双线)"));
    tunnelsModel->setHeaderData(Tunnel_line_ID, Qt::Horizontal, tr("所在线路"));
    tunnelsModel->setHeaderData(Tunnel_is_bridge, Qt::Horizontal, tr("是否是桥梁"));

    tunnelsModel->select();

    return tunnelsModel;
}

/**
 * 【只为上层需要提供】与上面函数指向两个不同的地址空间
 * 某一线路上的隧道模型，排序
 */
QSqlRelationalTableModel * TunnelDAO::getTunnels_CannotModify(int lineid) const
{
    tunnelsModel_2->setTable("tunnel");

    tunnelsModel_2->setRelation(Tunnel_line_ID,
            QSqlRelation("railway_line", "line_id", "line_name_std"));
    // 【注意】!!!
    // 当使用了SqlRelationalTableModel的setRelation()后，再使用setFilter时就要注意了，
    // 因为SqlRelationalTableModel类中已经包含了多张表，所以在写过滤规则时，必须加上表名
    tunnelsModel_2->setFilter(QString("tunnel.line_id = %1 AND tunnel.is_valid = TRUE").arg(lineid));

    tunnelsModel_2->setSort(Tunnel_start_point_std, Qt::AscendingOrder);

    tunnelsModel_2->setHeaderData(Tunnel_ID, Qt::Horizontal, tr("隧道ID"));
    tunnelsModel_2->setHeaderData(Tunnel_name_std, Qt::Horizontal, tr("隧道名称"));
    tunnelsModel_2->setHeaderData(Tunnel_is_valid, Qt::Horizontal, tr("是否被删除作废"));
    tunnelsModel_2->setHeaderData(Tunnel_ID_std, Qt::Horizontal, tr("隧道编号"));
    tunnelsModel_2->setHeaderData(Tunnel_start_point_std, Qt::Horizontal, tr("起始里程"));
    tunnelsModel_2->setHeaderData(Tunnel_end_point_std, Qt::Horizontal, tr("终止里程"));
    tunnelsModel_2->setHeaderData(Tunnel_is_new_std, Qt::Horizontal, tr("是否为新线"));
    tunnelsModel_2->setHeaderData(Tunnel_line_type_std, Qt::Horizontal, tr("线路类型"));
    tunnelsModel_2->setHeaderData(Tunnel_is_double_line, Qt::Horizontal, tr("是否为双线"));
    tunnelsModel_2->setHeaderData(Tunnel_is_downlink, Qt::Horizontal, tr("是否为下行(双线)"));
    tunnelsModel_2->setHeaderData(Tunnel_line_ID, Qt::Horizontal, tr("所在线路"));
    tunnelsModel_2->setHeaderData(Tunnel_is_bridge, Qt::Horizontal, tr("是否是桥梁"));

    tunnelsModel_2->select();

    return tunnelsModel_2;
}

/**
 * 某一线路上的隧道名称，排序
 */
QStringListModel * TunnelDAO::getTunnelNames(int lineid)
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT DISTINCT t.tunnel_name_std "
               "FROM tunnel t WHERE t.line_id = %1 "
               "WHERE t.is_valid = TRUE "
               "ORDER BY t.start_point_std ASC").arg(lineid));

    QStringList strings;
    while (query.next()) {
        QString line = query.value(0).toString();
        strings << line;
    }
    tunnelNamesModel->setStringList(strings);
    return tunnelNamesModel;
}

/**
 * 判断是否为桥梁
 */
bool TunnelDAO::getTunnelIsBriage(int tunnelid)
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT t.is_bridge "
               "FROM tunnel t WHERE t.tunnel_id = %1").arg(tunnelid));
    while (query.next()) {
        bool isbridge = query.value(0).toBool();
        return isbridge;
    }
    return false;
}

/**
 * 某条隧道
 * @param tunnelid 隧道ID
 */
QSqlQueryModel * TunnelDAO::getExactTunnel(int tunnelid)
{
    QSqlQuery query;
    bool ret = query.exec(QObject::tr("SELECT t.*, l.line_name_std, ss.station_name_std AS start_station, se.station_name_std AS end_station FROM tunnel t "
                           "JOIN railway_line l ON t.line_id = l.line_id "
                           "JOIN railway_station ss ON l.start_station_id = ss.station_id "
                           "JOIN railway_station se ON l.end_station_id = se.station_id "
                           "WHERE t.tunnel_id = %1 AND t.is_valid = true AND l.is_valid = true").arg(tunnelid));
    exactTunnelModel->setQuery(query);
    qDebug() << "getExactTunnel" << ret;
    return exactTunnelModel;
}

/**
 * 是否有该lineid对应的隧道数据库记录
 * @return true有，false没有
 */
bool TunnelDAO::hasTunnelID(int tunnelid)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM tunnel WHERE tunnel_id = %1 "
               ).arg(tunnelid));

    while (query.next()) {
        return true;
    }

    qDebug() << tunnelid << "can not find in table tunnel!";
    return false;
}

/**
 * 创建铁道隧道，没有tunnel参数，因为加入数据库时采取主键自增策略
 * @return 0 创建成功
 *         1 创建不成功 已经存在该隧道
 *         2 创建不成功 lineid不存在
 */
int TunnelDAO::addTunnel(QString tunnelName, bool isValid, int lineID,
              QString tunnelID_std, _int64 startPoint, _int64 endPoint,
              int isNew, int lineType, bool isDoubleLine, bool isDownlink, bool isBridge)
{
    int ret = 0;
    if (getTunnelID_P(tunnelName, tunnelID_std, startPoint, endPoint,
                  isNew, lineType, isDoubleLine, isDownlink) > 0)
    {
        ret = 1;
        return ret;
    }
    if (LineDAO::getLineDAOInstance()->hasLineID(lineID))
    {
        createTunnel_P(tunnelName, isValid, lineID, tunnelID_std, startPoint, endPoint,
                       isNew, lineType, isDoubleLine, isDownlink, isBridge);
        return ret;
    }
    ret = 2;
    qDebug() << "this line has not existed!";
    return ret;
}

/**
 * 更新铁道隧道
 * @return 0 更新成功
 *         1 更新不成功，当前tunnelid不存在
 *         2 更新不成功，lineid不存在
 */
int TunnelDAO::updateTunnel(int tunnelid, QString tunnelName, bool isValid, int lineID,
                 QString tunnelID_std, _int64 startPoint, _int64 endPoint,
                 int isNew, int lineType, bool isDoubleLine, bool isDownlink)
{
    if (!hasTunnelID(tunnelid)) {
        qDebug() << tunnelid << "do not has the tunnel in table tunnel!";
        return 1;
    } else if (LineDAO::getLineDAOInstance()->hasLineID(lineID)) {
        updateTunnel_P(tunnelid, tunnelName, isValid, lineID, tunnelID_std, startPoint, endPoint,
                       isNew, lineType, isDoubleLine, isDownlink);
        return 0;
    } else {
        qDebug() << lineID << "do not has the related line!";
        return 2;
    }
}

/**
 * 删除铁道隧道，并非真正删除，即is_valid变为false
 * @return 0 成功，1不存在该tunnel
 */
int TunnelDAO::removeTunnel(int tunnelid)
{
    if (!hasTunnelID(tunnelid))
    {
        qDebug() << tunnelid << "do not has the tunnel in table tunnel!";
        return 1;
    }
    updateTunnel_P(tunnelid, false);
    return 0;
}

// 是否有这条线路，return >0 tunnelid，-1没有
int TunnelDAO::getTunnelID_P(QString tunnelName, QString tunnelID_std, _int64 startPoint, _int64 endPoint,
                       int isNew, int lineType, bool isDoubleLine, bool isDownlink)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM tunnel WHERE tunnel_name_std = '%1', "
                       "tunnel_id_std = '%2', start_point = %3, end_point = %4, is_new_std = %5, "
                       "line_type_std = %6, is_double_line = %7, is_downlink = %9"
                  ).arg(tunnelName).arg(tunnelID_std).arg(startPoint).arg(endPoint).
                    arg(isNew).arg(lineType).arg(isDoubleLine).arg(isDownlink));

    while (query.next()) {
        int tunnelid = query.value(0).toInt();
        return tunnelid;
    }

    qDebug() << "can not find in table tunnel!";
    return -1;
}

bool TunnelDAO::createTunnel_P(QString tunnelName, bool isValid, int lineID,
                               QString tunnelID_std, _int64 startPoint, _int64 endPoint,
                               int isNew, int lineType, bool isDoubleLine, bool isDownlink, bool isBridge)
{
    QSqlQuery query;
    bool ret = query.exec(QString("INSERT INTO tunnel (tunnel_name_std, is_valid, line_id, "
                       "tunnel_id_std, start_point_std, end_point_std, is_new_std, line_type_std, "
                       "is_double_line, is_downlink, is_bridge) "
                       "VALUES ('%1', %2, %3, '%4', %5, %6, %7, %8, %9, %10, %11) "
               ).arg(tunnelName).arg(isValid).arg(lineID).arg(tunnelID_std)
               .arg(startPoint).arg(endPoint).arg(isNew).arg(lineType).arg(isDoubleLine).arg(isDownlink).arg(isBridge));
    qDebug() << "createTunnel_P" << tunnelName << ret;
    return ret;
}

bool TunnelDAO::updateTunnel_P(int tunnelid, QString tunnelName, bool isValid, int lineID,
                               QString tunnelID_std, _int64 startPoint, _int64 endPoint,
                               int isNew, int lineType, bool isDoubleLine, bool isDownlink)
{
    QSqlQuery query;
    bool ret = query.exec(QString("UPDATE tunnel SET tunnel_name_std = '%1', "
                       "is_valid = %2, line_id = %3, tunnel_id_std = '%4', "
                       "start_point_std = %5, end_point_std = %6, is_new_std = %7, "
                       "line_type_std = %8, is_double_line = %9, is_downlink = %10 "
                        "WHERE tunnel_id = %11"
                       ).arg(tunnelName).arg(isValid).arg(lineID).
                        arg(tunnelID_std).arg(startPoint).arg(endPoint).
                        arg(isNew).arg(lineType).arg(isDoubleLine).arg(isDownlink).arg(tunnelid));
    qDebug() << "updateTunnel_P" << tunnelid << tunnelName << ret;
    return ret;
}

bool TunnelDAO::updateTunnel_P(int tunnelid, bool isValid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("UPDATE tunnel SET is_valid = %1 "
               "WHERE tunnel_id = %2").arg(isValid).arg(tunnelid));
    qDebug() << "updateTunnel_P" << tunnelid << ret;
    return ret;
}

bool TunnelDAO::deleteTunnel_P(int tunnelid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("DELETE FROM tunnel WHERE tunnel_id = %1").arg(tunnelid));
    qDebug() << "deleteTunnel_P" << tunnelid << ret;
    return ret;
}
