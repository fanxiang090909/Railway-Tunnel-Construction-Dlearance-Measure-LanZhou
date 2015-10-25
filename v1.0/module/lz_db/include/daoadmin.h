#ifndef DAOADMIN_H
#define DAOADMIN_H

#include <QObject>
#include <QSqlQuery>
#include <QStringListModel>

/**
 * 系统高级用户数据访问对象类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-8
 */
class AdminDAO : public QObject
{
    Q_OBJECT

private:
    // singleton设计模式，静态私有实例变量
    static AdminDAO * adminDAOInstance;

    QStringListModel * userNamesModel;

public:

    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static AdminDAO * getAdminDAOInstance();

    ~AdminDAO();

    QStringListModel * getUsernames();

    /**
     * 登录验证
     * @return 0 用户名密码正确，1 用户不存在，2 用户名存在但密码错误
     */
    int setCurrentUser(QString username, QString passwd);
    QString getCurrentUser();

    /**
     * 登录验证
     * @return 0 用户名密码正确，1 用户不存在，2 用户名存在但密码错误
     */
    int validateUser(QString username, QString password);

    QString getUserNameByID(int id);

    /**
     * 得到用户级别
     * @return <0 用户不存在，>= 用户级别
     */
    int getUserLevel(QString username);

    /**
     * 创建用户
     * @return 0创建成功 1创建不成功 已经存在该用户
     */
    int addAdmin(QString username, QString password, int level);

    /**
     * 更新用户信息（密码或者级别）
     * @return 0成功 1更新不成功，当前username不存在
     */
    int updateAdmin(QString username, QString password, int level);

    /**
     * 删除用户信息，并非真正删除，即level变为-1冻结用户
     * @return 0 成功，1更新不成功，当前username不存在
     */
    int removeAdmin(QString username);

private:

    // singleton单一设计模式，构造函数私有
    explicit AdminDAO(QObject *parent = 0);

    // 用户是否存在 return true存在，false不存在
    bool hasUser_P(QString username);

    bool createAdmin_P(QString username, QString password, int level);

    bool updateAdmin_P(QString username, QString password, int level);

    bool updateAdmin_P(QString username, int level);

    bool deleteAdmin_P(QString username);


    // 当前操作用户的用户名
    QString currentUser;

signals:

public slots:

};

#endif // DAOADMIN_H
