#ifndef DAOTUNNEL_H
#define DAOTUNNEL_H

#include <QSqlQuery>
#include <QStringListModel>
#include <QSqlRelationalTableModel>
#include <QSqlQueryModel>

enum {
    Tunnel_ID = 0,
    Tunnel_name_std = 1,
    Tunnel_ID_std = 2,
    Tunnel_start_point_std = 3,
    Tunnel_end_point_std = 4,
    Tunnel_is_new_std = 5,
    Tunnel_line_type_std = 6,
    Tunnel_is_double_line = 7,
    Tunnel_is_downlink = 8,
    Tunnel_line_ID = 9,
    Tunnel_is_bridge = 10, // 是否是桥梁，@author范翔 2014-03-28加入
    Tunnel_is_valid = 11
};

/**
 * 隧道数据访问对象类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-8
 */
class TunnelDAO : public QObject
{
    Q_OBJECT

private:
    // singleton设计模式，静态私有实例变量
    static TunnelDAO * tunnelDAOInstance;

    QSqlRelationalTableModel * tunnelsModel;
    // 上层界面需要，指向于tunnelsModel不同的两个Model空间
    QSqlRelationalTableModel * tunnelsModel_2;

    QStringListModel * tunnelNamesModel;
    QSqlQueryModel * exactTunnelModel;

public:
    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static TunnelDAO * getTunnelDAOInstance();

    ~TunnelDAO();

    /**
     * 根据隧道ID得到隧道名
     */
    QString getOneTunnelName(int tunnelid);

    /**
     * 某一线路上的隧道模型，排序
     */
    QSqlRelationalTableModel * getTunnels(int lineid);

    /**
     * 【只为上层需要提供】与上面函数指向两个不同的地址空间
     * 某一线路上的隧道模型，排序
     */
    QSqlRelationalTableModel * getTunnels_CannotModify(int lineid) const;

    /**
     * 某一线路上的所有隧道名称，排序
     */
    QStringListModel * getTunnelNames(int lineid);

    /**
     * 判断是否为桥梁
     */
    bool getTunnelIsBriage(int tunnelid);

    /**
     * 某条隧道
     * @param tunnelid 隧道ID
     */
    QSqlQueryModel * getExactTunnel(int tunnelid);

    /**
     * 是否有该tunnelid对应的隧道数据库记录
     * @return true有，false没有
     */
    bool hasTunnelID(int tunnelid);

    /**
     * 创建铁道隧道，没有tunnel参数，因为加入数据库时采取主键自增策略
     * @return 0 创建成功
     *         1 创建不成功 已经存在该隧道
     *         2 创建不成功 lineid不存在
     */
    int addTunnel(QString tunnelName, bool isValid, int lineID,
                  QString tunnelID_std, _int64 startPoint, _int64 endPoint,
                  int isNew, int lineType, bool isDoubleLine, bool isDownlink, bool isBridge);

    /**
     * 更新铁道隧道
     * @return 0 更新成功
     *         1 更新不成功，当前tunnelid不存在
     *         2 更新不成功，lineid不存在
     */
    int updateTunnel(int tunnelid, QString tunnelName, bool isValid, int lineID,
                     QString tunnelID_std, _int64 startPoint, _int64 endPoint,
                     int isNew, int lineType, bool isDoubleLine, bool isDownlink);

    /**
     * 删除铁道隧道，并非真正删除，即is_valid变为false
     * @return 0 成功，1不存在该tunnel
     */
    int removeTunnel(int tunnelid);

private:

    // singleton单一设计模式，构造函数私有
    explicit TunnelDAO(QObject *parent = 0);


    // 是否有这条隧道，return >0 tunnelid，-1没有
    int getTunnelID_P(QString tunnelName, QString tunnelID_std, _int64 startPoint, _int64 endPoint,
                                int isNew, int lineType, bool isDoubleLine, bool isDownlink);

    // 数据库中创建新隧道，没有tunnelid参数，因为加入数据库时采取主键自增策略
    bool createTunnel_P(QString tunnelName, bool isValid, int lineID,
                                QString tunnelID_std, _int64 startPoint, _int64 endPoint,
                                int isNew, int lineType, bool isDoubleLine, bool isDownlink, bool isBridge);

    // 数据库中修改Tunnel
    bool updateTunnel_P(int tunnelid, QString tunnelName, bool isValid, int lineID,
                                QString tunnelID_std, _int64 startPoint, _int64 endPoint,
                                int isNew, int lineType, bool isDoubleLine, bool isDownlink);
    // 数据库中修改Tunnel
    bool updateTunnel_P(int tunnelid, bool isValid);

    // 数据库中删除Tunnel
    bool deleteTunnel_P(int tunnelid);
};

#endif // DAOTUNNEL_H
