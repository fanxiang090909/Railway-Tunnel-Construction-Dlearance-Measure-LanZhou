#include <QObject>
#include <QDebug>
#include "daocurve.h"
#include "daotunnel.h"

/**
 * 隧道-曲线数据访问对象类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-9
 */
// singleton设计模式，静态私有实例变量
CurveDAO * CurveDAO::curveDAOInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
CurveDAO * CurveDAO::getCurveDAOInstance()
{
    if (curveDAOInstance == NULL)
    {
        curveDAOInstance = new CurveDAO();
    }
    return curveDAOInstance;
}

// singleton单一设计模式，构造函数私有
CurveDAO::CurveDAO(QObject *parent) :
    QObject(parent)
{
    curvesModel = new QSqlRelationalTableModel(this);
    exactTunnelCurves = new QSqlQueryModel(this);
}

CurveDAO::~CurveDAO()
{
    delete curvesModel;
    delete exactTunnelCurves;
}

/**
 * 某一隧道上的隧道曲线模型，排序
 */
QSqlRelationalTableModel * CurveDAO::getCurves(int tunnelid)
{
    curvesModel->setTable("tunnel_curve");

    curvesModel->setRelation(Curve_tunnel_ID,
            QSqlRelation("tunnel", "tunnel_id", "tunnel_name_std"));
    curvesModel->setFilter(QObject::tr("tunnel_curve.tunnel_id = %1 AND tunnel_curve.is_valid = true").arg(tunnelid));

    curvesModel->setSort(Curve_point_zhi_huan_std, Qt::AscendingOrder);

    curvesModel->setHeaderData(Curve_ID, Qt::Horizontal, tr("曲线ID"));
    curvesModel->setHeaderData(Curve_radius_std, Qt::Horizontal, tr("曲线半径"));
    curvesModel->setHeaderData(Curve_is_valid, Qt::Horizontal, tr("是否被删除作废"));
    curvesModel->setHeaderData(Curve_is_straight_std, Qt::Horizontal, tr("是否为直线隧道"));
    curvesModel->setHeaderData(Curve_is_left_std, Qt::Horizontal, tr("是否左转曲线"));
    curvesModel->setHeaderData(Curve_point_zhi_huan_std, Qt::Horizontal, tr("直缓点里程"));
    curvesModel->setHeaderData(Curve_point_huan_zhi_std, Qt::Horizontal, tr("缓直点里程"));
    curvesModel->setHeaderData(Curve_tunnel_ID, Qt::Horizontal, tr("所在隧道"));

    curvesModel->select();

    return curvesModel;
}

/**
 * 为了TunnelDataModel类中调用的load方法时使用
 * 该方法与getCurves(int tunnelid)没有本质区别，只是返回的指针指向的地址不同
 * 某一隧道上的隧道曲线模型，排序
 */
QSqlQueryModel * CurveDAO::getExactTunnelCurves(int tunnelid)
{
    QSqlQuery query;
    bool ret = query.exec(QObject::tr("SELECT c.curve_id, c.radius_std, c.is_left_std, c.is_straight_std, c.point_zhi_huan_std, c.point_huan_zhi_std "
                           "FROM tunnel_curve c JOIN tunnel t ON t.tunnel_id = c.tunnel_id "
                           "WHERE c.tunnel_id = %1 AND c.is_valid = true AND t.is_valid = true").arg(tunnelid));
    exactTunnelCurves->setQuery(query);
    qDebug() << "getExactTunnelCurves" << ret;
    return exactTunnelCurves;
}

/**
 * 是否有该curveid对应的隧道曲线数据库记录
 * @return true有，false没有
 */
bool CurveDAO::hasCurveID(_int64 curveid)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM tunnel_curve WHERE curve_id = %1 "
               ).arg(curveid));

    while (query.next()) {
        return true;
    }

    qDebug() << curveid << "can not find in table tunnel_curve!";
    return false;
}

/**
 * 创建铁道隧道曲线，没有tunnel参数，因为加入数据库时采取主键自增策略
 * @return 0 创建成功
 *         1 创建不成功 已经存在该隧道曲线
 *         2 创建不成功 lineid不存在
 */
int CurveDAO::addCurve(int tunnelID, int radius, _int64 pointZhiHuan, _int64 pointHuanZhi,
             bool isStraight, bool isLeft, bool isValid)
{
    int ret = 0;
    if (getCurveID_P(tunnelID, radius, pointZhiHuan, pointHuanZhi, isStraight, isLeft) >= 0)
    {
        ret = 1;
        return ret;
    }
    if (TunnelDAO::getTunnelDAOInstance()->hasTunnelID(tunnelID))
    {
        createCurve_P(tunnelID, radius, pointZhiHuan, pointHuanZhi, isStraight, isLeft, isValid);
        return ret;
    }
    ret = 2;
    qDebug() << "this tunnel has not existed!";
    return ret;
}

/**
 * 更新铁道隧道曲线
 * @return 0 更新成功
 *         1 更新不成功，当前curveid不存在
 *         2 更新不成功，tunnelid不存在
 */
int CurveDAO::updateCurve(_int64 curveid, int tunnelID, int radius, _int64 pointZhiHuan,
                _int64 pointHuanZhi, bool isStraight, bool isLeft, bool isValid)
{
    if (!hasCurveID(curveid))
    {
        qDebug() << curveid << "do not has the tunnel in table tunnel!";
        return 1;
    }
    if (TunnelDAO::getTunnelDAOInstance()->hasTunnelID(tunnelID))
    {
        updateCurve_P(curveid, tunnelID, radius, pointZhiHuan,
                      pointHuanZhi, isStraight, isLeft, isValid);
        return 0;
    }
    else
    {
        qDebug() << curveid << "do not has the related line!";
        return 2;
    }
}

/**
 * 删除铁道隧道曲线，并非真正删除，即is_valid变为false
 * @return 0 成功，1不存在该curve
 */
int CurveDAO::removeCurve(_int64 curveid)
{
    if (!hasCurveID(curveid))
    {
        qDebug() << curveid << "do not has the curve in table curve!";
        return 1;
    }
    updateCurve_P(curveid, false);
    return 0;
}

// 是否有这条隧道曲线，return >0 curveid，-1没有
_int64 CurveDAO::getCurveID_P(int tunnelID, int radius, _int64 pointZhiHuan, _int64 pointHuanZhi,
                            bool isStraight, bool isLeft)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM tunnel_curve WHERE tunnel_id = %1 AND "
                       "radius_std = %2 AND point_zhi_huan_std = %3 AND point_huan_zhi_std = %4 AND "
                       "is_straight_std = %5 AND is_left_std = %6"
                  ).arg(tunnelID).arg(radius).arg(pointZhiHuan).arg(pointHuanZhi).
                    arg(isStraight).arg(isLeft));

    while (query.next()) {
        _int64 tunnelid = query.value(0).toLongLong();
        return tunnelid;
    }

    qDebug() << "can not find in table curve! tunnelid=" << tunnelID << ", radius=" << radius
             << ", pointZhiHuan=" << pointZhiHuan << ", pointHuanZhi=" << pointHuanZhi;
    return -1;
}

// 数据库中创建新隧道曲线，没有curveid参数，因为加入数据库时采取主键自增策略
bool CurveDAO::createCurve_P(int tunnelID, int radius, _int64 pointZhiHuan, _int64 pointHuanZhi,
                   bool isStraight, bool isLeft, bool isValid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("INSERT INTO tunnel_curve (tunnel_id, radius_std, point_zhi_huan_std, point_huan_zhi_std, "
                       "is_straight_std, is_left_std, is_valid) VALUES (%1, %2, %3, %4, %5, %6, %7) "
               ).arg(tunnelID).arg(radius).arg(pointZhiHuan).arg(pointHuanZhi).
               arg(isStraight).arg(isLeft).arg(isValid));
    qDebug() << "createCurve_P curve of tunnelid " << tunnelID << ret;
    return ret;

}

// 数据库中修改Curve
bool CurveDAO::updateCurve_P(_int64 curveid, int tunnelID, int radius, _int64 pointZhiHuan,
                   _int64 pointHuanZhi, bool isStraight, bool isLeft, bool isValid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("UPDATE tunnel_curve SET tunnel_id = %1, "
                       "radius_std = %2, point_zhi_huan_std = %3, point_huan_zhi_std = %4, "
                       "is_straight_std = %5, is_left_std = %6, is_valid = %7 "
                       "WHERE curve_id = %8"
                   ).arg(tunnelID).arg(radius).arg(pointZhiHuan).arg(pointHuanZhi).
                    arg(isStraight).arg(isLeft).arg(isValid).arg(curveid));
    qDebug() << "updateTunnel_P" << curveid << ret; // << "isleft" << isLeft << "isStraight" << isStraight;
    return ret;
}

// 数据库中修改Curve
bool CurveDAO::updateCurve_P(_int64 curveid, bool isValid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("UPDATE tunnel_curve SET is_valid = %1 "
               "WHERE curve_id = %2").arg(isValid).arg(curveid));
    qDebug() << "updateTunnel_P" << curveid << ret;
    return ret;
}

// 数据库中删除Curve
bool CurveDAO::deleteCurve_P(_int64 curveid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("DELETE FROM tunnel_curve WHERE curve_id = %1").arg(curveid));
    qDebug() << "deleteTunnel_P" << curveid << ret;
    return ret;
}
