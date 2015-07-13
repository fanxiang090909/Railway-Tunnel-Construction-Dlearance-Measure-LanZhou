#ifndef DAOCURVE_H
#define DAOCURVE_H

#include <QObject>
#include <QSqlQuery>
#include <QSqlRelationalTableModel>
#include <QStringListModel>

enum {
    Curve_ID = 0,
    Curve_tunnel_ID = 1,
    Curve_radius_std = 2,
    Curve_is_straight_std = 3,
    Curve_is_left_std = 4,
    Curve_point_zhi_huan_std = 5,
    Curve_point_huan_zhi_std = 6,
    Curve_is_valid = 7
};

/**
 * 隧道-曲线数据访问对象类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-9
 */
class CurveDAO : public QObject
{
    Q_OBJECT
private:
    // singleton设计模式，静态私有实例变量
    static CurveDAO * curveDAOInstance;

    QSqlRelationalTableModel * curvesModel;

    QSqlQueryModel * exactTunnelCurves;

public:
    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static CurveDAO * getCurveDAOInstance();

    ~CurveDAO();

    /**
     * 某一隧道上的隧道曲线模型，排序
     */
    QSqlRelationalTableModel * getCurves(int tunnelid);

    /**
     * 为了TunnelDataModel类中调用的load方法时使用
     * 该方法与getCurves(int tunnelid)没有本质区别，只是返回的指针指向的地址不同
     * 某一隧道上的隧道曲线模型，排序
     */
    QSqlQueryModel * getExactTunnelCurves(int tunnelid);

    /**
     * 是否有该curveid对应的隧道曲线数据库记录
     * @return true有，false没有
     */
    bool hasCurveID(_int64 curveid);

    /**
     * 创建铁道隧道曲线，没有tunnel参数，因为加入数据库时采取主键自增策略
     * @return 0 创建成功
     *         1 创建不成功 已经存在该隧道曲线
     *         2 创建不成功 lineid不存在
     */
    int addCurve(int tunnelID, int radius, _int64 pointZhiHuan, _int64 pointHuanZhi,
                 bool isStraight, bool isLeft, bool isValid);

    /**
     * 更新铁道隧道曲线
     * @return 0 更新成功
     *         1 更新不成功，当前curveid不存在
     *         2 更新不成功，tunnelid不存在
     */
    int updateCurve(_int64 curveid, int tunnelID, int radius, _int64 pointZhiHuan,
                    _int64 pointHuanZhi, bool isStraight, bool isLeft, bool isValid);

    /**
     * 删除铁道隧道曲线，并非真正删除，即is_valid变为false
     * @return 0 成功，1不存在该curve
     */
    int removeCurve(_int64 curveid);


private:

    // singleton单一设计模式，构造函数私有
    explicit CurveDAO(QObject *parent = 0);

    // 是否有这条隧道曲线，return >0 curveid，-1没有
    _int64 getCurveID_P(int tunnelID, int radius, _int64 pointZhiHuan, _int64 pointHuanZhi,
                                bool isStraight, bool isLeft);

    // 数据库中创建新隧道曲线，没有curveid参数，因为加入数据库时采取主键自增策略
    bool createCurve_P(int tunnelID, int radius, _int64 pointZhiHuan, _int64 pointHuanZhi,
                       bool isStraight, bool isLeft, bool isValid);

    // 数据库中修改Curve
    bool updateCurve_P(_int64 curveid, int tunnelID, int radius, _int64 pointZhiHuan,
                       _int64 pointHuanZhi, bool isStraight, bool isLeft, bool isValid);

    // 数据库中修改Curve
    bool updateCurve_P(_int64 curveid, bool isValid);

    // 数据库中删除Curve
    bool deleteCurve_P(_int64 curveid);

signals:

public slots:

};

#endif // DAOCURVE_H
