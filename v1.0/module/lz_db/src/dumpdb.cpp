#include "dumpdb.h"
#include <QProcess>

#include "settingdb.h"
#include <QDebug>
#include <QSqlQuery>
#include <QFile>
#include <QTextStream>
#include <QSqlError>
#include <QDateTime>

/**
 * 数据库备份及加载类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2015-3-5
 */
// singleton设计模式，静态私有实例变量
DumpDatabase * DumpDatabase::dumpDBInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
DumpDatabase * DumpDatabase::getDumpDatabaseInstance()
{
    if (dumpDBInstance == NULL)
    {
        dumpDBInstance = new DumpDatabase();
    }
    return dumpDBInstance;
}

// singleton单一设计模式，构造函数私有
DumpDatabase::DumpDatabase(QObject *parent) :
    QObject(parent)
{
    dumpthread = NULL;
}

DumpDatabase::~DumpDatabase()
{
    if (dumpthread != NULL)
        delete dumpthread;
}

/**
 * 数据库备份：参见mysql中的mysqldump.exe程序参数说明
 * @param filename 保存至哪个文件，后缀.sql
 * @param backuptype 备份文件类型
 *          1 DML DDL（数据库定义语言+数据库操作语言）
 *          2 DML (只含数据操作SELECT INSERT DELETE UPDATE)
 */
void DumpDatabase::exportDatabase(QString filename, int backuptype)
{
    if (dumpthread != NULL)
        delete dumpthread;

    dumpthread = new MySQLDumpThread(this);
    connect(dumpthread, SIGNAL(finish(bool, QString, QString)), this, SLOT(receiveFinish(bool, QString, QString)));
    dumpthread->init(false, filename, backuptype);
    //dumpthread->start();
    dumpthread->run();
}

void DumpDatabase::importDatabase(QString filename, int importtype)
{
    if (dumpthread != NULL)
        delete dumpthread;

    dumpthread = new MySQLDumpThread(this);
    connect(dumpthread, SIGNAL(finish(bool, QString, QString)), this, SLOT(receiveFinish(bool, QString, QString)));
    dumpthread->init(true, filename, importtype);
    //dumpthread->start();
    dumpthread->run();
}

/**
 * 得到最新数据库更新时间
 */
QDateTime DumpDatabase::getNewestUpdateTime()
{
    QDateTime time1, time2, timenewest;

    QSqlQuery query;
    query.exec(QString("SELECT MAX(information_schema.`TABLES`.CREATE_TIME), MAX(information_schema.`TABLES`.UPDATE_TIME) FROM information_schema.`TABLES` WHERE information_schema.`TABLES`.TABLE_SCHEMA = '%1' "
               ).arg(DatabaseSetting::getDBSettingInstance()->getDBName()));

    while (query.next()) {
        QString timestr = query.value(0).toString();
        if (!timestr.isEmpty())
        {
            time1 = QDateTime::fromString(timestr, "yyyy-MM-ddThh:mm:ss");
            timenewest = time1;
        }
        QString timestr2 = query.value(1).toString();
        if (!timestr2.isEmpty())
        {
            time2 = QDateTime::fromString(timestr, "yyyy-MM-ddThh:mm:ss");
            if (time1.isValid() && time2 > time1)
                timenewest = time2;
        }
        qDebug() << "database newest time" << timestr << timestr2;
        break;
    }
    return timenewest;
}

QDateTime DumpDatabase::getNewestUpdateTime(QString filename, bool & ret)
{
    QFile file(filename);
    QDateTime ret1;
    if (file.open(QFile::ReadOnly))
    {
        QString datetmp = file.readLine();
        ret1 = QDateTime::fromString(datetmp, "yyyy-MM-dd hh:mm:ss");
        if (ret1.isValid())
        {
            ret = true;
            file.close();
            return ret1;
        }
    }
    file.close();
    ret = false;
    return ret1;
}

/**
 * 设置当前程序状态中存储的数据库更新时间
 * 在程序开始运行，和备份数据库时需更新这个时间
 */
void DumpDatabase::setDBUpdateTime_Setting(QDateTime newTime)
{
    dbUpdateTime = newTime;
}

void DumpDatabase::setDBUpdateTime_Setting(QDateTime newTime, QString filename)
{
    QFile file(filename);
    if (file.open(QFile::WriteOnly))
    {
        QTextStream stream(&file);
        stream << newTime.toString("yyyy-MM-dd hh:mm:ss");
        file.close();
        return;
    }
    file.close();
}

/**
 * 获得当前程序状态中存储的数据库更新时间
 */
QDateTime DumpDatabase::getDBUpdateTime_Setting()
{
    return dbUpdateTime;
}

/**
 * 检查是否需要备份
 */
bool DumpDatabase::checkIfNeedToExport(QDateTime newTime)
{
    if (dbUpdateTime < newTime)
        return true;
    else
        return false;
}

/**
 * 检查是否需要导入
 */
bool DumpDatabase::checkIfNeedToImport(QDateTime newTime)
{
    if (dbUpdateTime < newTime)
        return true;
    else
        return false;
}

/**
 * 转发
 */
void DumpDatabase::receiveFinish(bool isimport, QString filename, QString err)
{
    emit finish(isimport, filename, err);
}

/**
 * 初始化，是导入还是导出
 */
void MySQLDumpThread::init(bool initisimport, QString initfilename, int initbackuptype)
{
    isimport = initisimport;
    filename = initfilename;
    backuptype = initbackuptype;
}

void MySQLDumpThread::run()
{
    if (isimport) // 导入
    {
        if (backuptype == 1)
        {
            // 原函数DumpDatabase::clearAndReloadDababase
            QString cmd = QString("mysql.exe -u%1 -p%2 -h%3 -P%4 %5").arg(DatabaseSetting::getDBSettingInstance()->getUsername())
                                                     .arg(DatabaseSetting::getDBSettingInstance()->getPasswd())
                                                     .arg(DatabaseSetting::getDBSettingInstance()->getIPAddress())
                                                     .arg(DatabaseSetting::getDBSettingInstance()->getPort())
                                                     .arg(DatabaseSetting::getDBSettingInstance()->getDBName());

            reloadproc.setStandardInputFile(filename);
            qDebug() << cmd << filename;

            // @author  范翔 execute函数会将掉用放到当前线程中，start函数放到新线程中
            // TODO excute为什么不行 网上有人遇到相同问题，最终使用的是start见，http://hd.hbrc.com/news/view_3229116.html
            reloadproc.start(cmd);
            emit finish(isimport, filename, "");
            qDebug() << "start import db data";
        }
        else if (backuptype == 2)
        {
            updateDatabase(filename);
            emit finish(isimport, filename, "");
        }
    }
    else // 导出
    {
        QString cmd;
        // 含数据库表格版本
        if (backuptype == 1)
            cmd = QString("mysqldump.exe --add-drop-table -u%1 -p%2 -h%3 -P%4 %5").arg(DatabaseSetting::getDBSettingInstance()->getUsername())
                                                                .arg(DatabaseSetting::getDBSettingInstance()->getPasswd())
                                                                .arg(DatabaseSetting::getDBSettingInstance()->getIPAddress())
                                                                .arg(DatabaseSetting::getDBSettingInstance()->getPort())
                                                                .arg(DatabaseSetting::getDBSettingInstance()->getDBName());

        if (backuptype == 2)
        // 只含数据不含表格版本
            cmd = QString("mysqldump.exe --no-create-db --no-create-info --no-autocommit --disable-keys --skip-opt --skip-comments --skip-add-locks --complete-insert -u%1 -p%2 -h%3 -P%4 %5")
                                                                .arg(DatabaseSetting::getDBSettingInstance()->getUsername())
                                                                .arg(DatabaseSetting::getDBSettingInstance()->getPasswd())
                                                                .arg(DatabaseSetting::getDBSettingInstance()->getIPAddress())
                                                                .arg(DatabaseSetting::getDBSettingInstance()->getPort())
                                                                .arg(DatabaseSetting::getDBSettingInstance()->getDBName());
        QString path = QString(filename);
        backupproc.setStandardOutputFile(path);
        qDebug() << cmd << path;

        // TODO excute为什么不行？？？网上有人遇到相同问题，最终使用的是start见，http://hd.hbrc.com/news/view_3229116.html
        // @author  范翔 execute函数会将掉用放到当前线程中，start函数放到新线程中
        backupproc.start(cmd);
        emit finish(isimport, filename, "");

        qDebug() << "start backup db data";
    }
}

void MySQLDumpThread::updateDatabase(QString filename)
{

    QFile file(filename);
    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << "can not open sql file";
        emit finish(true, filename, QString("can not open sql file %1").arg(filename));
    }
    QTextStream stream( &file );
    QString line;
    QSqlQuery query;
    int n = 1;
    while ( !stream.atEnd() ) {
        line = stream.readLine(); // 不包括“\n”的一行文本
        if (line.length() == 0)
            continue;
        else if (line.startsWith("--"))
            continue;
        else if (line.startsWith("/*!"))
            continue;
        if(!query.exec(line))
        {
            QSqlError error = query.lastError();
            emit finish(true, filename, QString("when import file %1, ERROR(%2)").arg(filename).arg(error.text()));
            qDebug() << "ERROR(" << n << "): " << error.text();
        }
        //printf( "%3d: %s\n", n, line.latin1() );
        n++;
    }
    file.close();
    qDebug() << "read end of update file";
}