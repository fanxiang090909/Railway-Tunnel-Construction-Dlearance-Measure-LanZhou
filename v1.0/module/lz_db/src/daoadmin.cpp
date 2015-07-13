#include <QDebug>
#include "daoadmin.h"

/**
 * 系统高级用户数据访问对象类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-8
 */
// singleton设计模式，静态私有实例变量
AdminDAO * AdminDAO::adminDAOInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
AdminDAO * AdminDAO::getAdminDAOInstance()
{
    if (adminDAOInstance == NULL)
    {
        adminDAOInstance = new AdminDAO();
    }
    return adminDAOInstance;
}

// singleton单一设计模式，构造函数私有
AdminDAO::AdminDAO(QObject *parent) :
    QObject(parent)
{
    userNamesModel = new QStringListModel(this);
}

AdminDAO::~AdminDAO()
{
    delete userNamesModel;
}

/**
 * 登录验证
 * @return 0 用户名密码正确，1 用户不存在，2 用户名存在但密码错误
 */
int AdminDAO::setCurrentUser(QString username, QString passwd)
{
    int ret = validateUser(username, passwd);
    if (ret <= 0)
        currentUser = username;
    return ret;
}

QString AdminDAO::getCurrentUser()
{
    return currentUser;
}

QStringListModel * AdminDAO::getUsernames()
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT a.username "
               "FROM admin a"));

    QStringList strings;
    while (query.next()) {
        QString line = query.value(0).toString();
        strings << line;
        //qDebug() << major;
    }
    userNamesModel->setStringList(strings);
    return userNamesModel;
}

/**
 * 登录验证
 * @return 0 用户名密码正确，1 用户不存在，2 用户名存在但密码错误
 */
int AdminDAO::validateUser(QString username, QString password)
{
    QSqlQuery query;
	query.exec(QString("SELECT a.password FROM admin a WHERE a.username = '%1' "
               ).arg(username));
    while (query.next()) {
        QString pw = query.value(0).toString();
        if (pw == password)
            return 0;
        else
            return 2;
    }
    return 1;
}

/**
 * 得到用户级别
 * @return <0 用户不存在，>= 用户级别
 */
int AdminDAO::getUserLevel(QString username)
{
	QSqlQuery query;
	query.exec(QString("SELECT a.level FROM admin a WHERE a.username = '%1' "
               ).arg(username));
    while (query.next()) {
        int level = query.value(0).toInt();
        return level;
    }
    return -1;
}

/**
 * 创建用户
 * @return 0创建成功 1创建不成功 已经存在该用户
 */
int AdminDAO::addAdmin(QString username, QString password, int level)
{
    if (!hasUser_P(username))
        return 1;
    else
        createAdmin_P(username, password, level);
    return 0;
}

/**
 * 更新用户信息（密码或者级别）
 * @return 0成功 1更新不成功，当前username不存在
 */
int AdminDAO::updateAdmin(QString username, QString password, int level)
{
    if (!hasUser_P(username))
        return 1;
    else
        updateAdmin_P(username, password, level);
    return 0;
}

/**
 * 删除用户信息，并非真正删除，即level变为-1冻结用户
 * @return 0 成功，1更新不成功，当前username不存在
 */
int AdminDAO::removeAdmin(QString username)
{
    if (!hasUser_P(username))
        return 1;
    else
        updateAdmin_P(username, -1);
    return 0;
}

// 用户是否存在 return true存在，false不存在
bool AdminDAO::hasUser_P(QString username)
{
    QSqlQuery query;
    query.exec(QString("SELECT a.* FROM admin a WHERE a.username = '%1' "
               ).arg(username));

    while (query.next()) {
        return true;
    }

    qDebug() << username << "can not find in table admin!";
    return false;
}

bool AdminDAO::createAdmin_P(QString username, QString password, int level)
{
    QSqlQuery query;
    bool ret = query.exec(QString("INSERT INTO admin (username, password, level) "
                       "VALUES('%1', '%2', %3)"
               ).arg(username).arg(password).arg(level));
    qDebug() << "createAdmin_P" << username << ret;
    return ret;
}

bool AdminDAO::updateAdmin_P(QString username, QString password, int level)
{
    QSqlQuery query;
    bool ret = query.exec(QString("UPDATE admin SET password = '%1', level = %2 WHERE username = '%3'"
               ).arg(password).arg(level).arg(username));
    qDebug() << "updateAdmin_P" << username << ret;
    return ret;
}

bool AdminDAO::updateAdmin_P(QString username, int level)
{
    QSqlQuery query;
    bool ret = query.exec(QString("UPDATE admin SET level = %1 WHERE username = '%2'"
               ).arg(level).arg(username));
    qDebug() << "updateAdmin_P" << username << ret;
    return ret;
}

bool AdminDAO::deleteAdmin_P(QString username)
{
    QSqlQuery query;
    bool ret = query.exec(QString("DELETE admin WHERE username = '%1'"
               ).arg(username));
    qDebug() << "deleteAdmin_P" << username << ret;
    return ret;
}
