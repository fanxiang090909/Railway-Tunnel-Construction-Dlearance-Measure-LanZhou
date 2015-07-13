#ifndef DUMPDB_H
#define DUMPDB_H

#include <QObject>
#include <QProcess>
#include <QThread>
#include <QDateTime>
#include <QSqlQuery>

// 预生明，否则编译错
class MySQLDumpThread;

/**
 * 数据库备份及加载类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2015-3-5
 */
class DumpDatabase : public QObject
{
    Q_OBJECT
private:
    // singleton设计模式，静态私有实例变量
    static DumpDatabase * dumpDBInstance;

public:

    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static DumpDatabase * getDumpDatabaseInstance();

    /**
     * 导出（数据库备份）：参见mysql中的mysqldump.exe程序参数说明
     * @param filename 保存至哪个文件，后缀.sql
     * @param backuptype 备份文件类型
     *          1 DML DDL（数据库定义语言+数据库操作语言）
     *          2 DML (只含数据操作SELECT INSERT DELETE UPDATE)
     */
    void exportDatabase(QString filename, int backuptype);

    /**
     * 导入
     * @param importtype 1 清除所有原始数据，重新加入数据
     *                   2 只对数据进行更新操作 updateDatabase
     */
    void importDatabase(QString filename, int importtype);

    /**
     * 得到最新数据库更新时间
     */
    QDateTime getNewestUpdateTime();
    QDateTime getNewestUpdateTime(QString filename, bool & ret);

    /**
     * 设置当前程序状态中存储的数据库更新时间
     * 在程序开始运行，和备份数据库时需更新这个时间
     */
    void setDBUpdateTime_Setting(QDateTime newTime);
    void setDBUpdateTime_Setting(QDateTime newTime, QString filename);

    /**
     * 获得当前程序状态中存储的数据库更新时间
     */
    QDateTime getDBUpdateTime_Setting();

    /**
     * 检查是否需要备份
     */
    bool checkIfNeedToExport(QDateTime newTime);

    /**
     * 检查是否需要导入
     */
    bool checkIfNeedToImport(QDateTime newTime);

    ~DumpDatabase();

private:
    explicit DumpDatabase(QObject *parent = 0);

    MySQLDumpThread * dumpthread;

    /**
     * 程序运行时认为的数据库
     */
    QDateTime dbUpdateTime;

private slots:
    /**
     * 转发
     */
    void receiveFinish(bool isimport, QString filename, QString err);

signals:
    void finish(bool isimport, QString filename, QString err);
};

/**
 * 数据库备份线程类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2015-3-5
 */
class MySQLDumpThread : public QThread //public QObject
{
    Q_OBJECT
public:
    MySQLDumpThread(QObject * object = 0) : QThread() {}//QObject(object) {}

    /**
     * 初始化，是导入还是导出
     */
    void init(bool isimport, QString filename, int backuptype);

    void run();

private:

    void updateDatabase(QString filename);

private:

    // 设置为类成员防止内存泄露
    QProcess backupproc;
    QProcess reloadproc;

    bool isimport;
    QString filename;
    int backuptype;

signals:
    void finish(bool isimport, QString filename, QString err);

};


#endif // DUMPDB_H
