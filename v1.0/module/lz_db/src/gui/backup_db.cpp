#include "backup_db.h"
#include "ui_backup_db.h"

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

#include "dumpdb.h"
#include "setting_client.h"

/**
 * 数据库备份界面类实现
 *
 * @author 熊雪 范翔
 * @version 1.0.0
 * @date 20150305
 */
BackupDatabaseWidget::BackupDatabaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BackupDatabaseWidget)
{
    ui->setupUi(this);
    
    //@TODO 提示导入或导出
    ui->newestLabel->setText("");
    timestampfilename = ClientSetting::getSettingInstance()->getClientTmpLocalParentPath() + "/db_backup/db_timestamp.txt";
    /*//qDebug() << timestampfilename;
    bool ret;
    DumpDatabase::getDumpDatabaseInstance()->setDBUpdateTime_Setting(DumpDatabase::getDumpDatabaseInstance()->getNewestUpdateTime(timestampfilename, ret));
    ui->newestLabel->setText(QObject::tr("当前备份时间:%1").arg(DumpDatabase::getDumpDatabaseInstance()->getDBUpdateTime_Setting().toString("yyyy-MM-dd hh:mm:ss")));
    QDateTime newtime = getNewestDBDumpFileTime();
    if (newtime.isValid() && DumpDatabase::getDumpDatabaseInstance()->checkIfNeedToImport(newtime))
    {
        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::red);
        ui->newestLabel->setPalette(pe);
        ui->newestLabel->setText(QObject::tr("检测到有新文件，时间：%1，请先导入！").arg(newtime.toString("yyyy-MM-dd hh:mm:ss")));
    }*/

    // 导入和导出信号槽
    connect(ui->leading_in_pushButton, SIGNAL(clicked()), this, SLOT(importSQLFileToDB()));
    connect(ui->leading_out_pushButton, SIGNAL(clicked()), this, SLOT(exportDBToSQLFile()));
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(checkIfImport()));

    checkIfImport();

    connect(DumpDatabase::getDumpDatabaseInstance(), SIGNAL(finish(bool, QString, QString)), this, SLOT(receiveFinish(bool, QString, QString)));
}

BackupDatabaseWidget::~BackupDatabaseWidget()
{
    delete ui;
}

/**
 * 获得最新数据库dump文件时间
 */
QDateTime BackupDatabaseWidget::getNewestDBDumpFileTime()
{
    QDir dir;
    dir.setPath(ClientSetting::getSettingInstance()->getParentPath() + "/db_backup/");
    dir.setFilter(QDir::Files | QDir::Hidden);
    dir.setSorting(QDir::Time);
    QStringList names = dir.entryList();
    QDateTime rettime;

    if (!names.isEmpty())
    {
        newestDBDumpFileName = names.at(names.size() - 1);
        rettime = QDateTime::fromString(newestDBDumpFileName.mid(newestDBDumpFileName.size() - 19).left(15), "yyyyMMdd hhmmss");
        qDebug() << "newestDBDumpFileName:" << newestDBDumpFileName << rettime;
    }
    else
        newestDBDumpFileName = "";
    return rettime;
}

/**
 * 检查程序打开过程中数据库是否有更新
 */
void BackupDatabaseWidget::checkIfExport()
{
    bool ret;
    QDateTime newtime = DumpDatabase::getDumpDatabaseInstance()->getNewestUpdateTime(timestampfilename, ret);
    if (ret)
        if (newtime.isValid() && DumpDatabase::getDumpDatabaseInstance()->checkIfNeedToExport(newtime))
        {
            QPalette pe;
            pe.setColor(QPalette::WindowText, Qt::red);
            ui->newestLabel->setPalette(pe);
            ui->newestLabel->setText(QObject::tr("检测到数据库有更新，请及时导出！"));
            return;
        }
   ui->newestLabel->setText(QObject::tr("数据库最新更新时间未知"));
}

/**
 * 检查程序打开过程中数据库需要导入
 */
void BackupDatabaseWidget::checkIfImport()
{
    bool ret;
    QDateTime stamptime = DumpDatabase::getDumpDatabaseInstance()->getNewestUpdateTime(timestampfilename, ret);
    QDir dir;
    dir.setPath(ClientSetting::getSettingInstance()->getParentPath() + "db_backup");
    dir.setFilter(QDir::Files | QDir::Hidden);
    dir.setSorting(QDir::DirsFirst);
    QStringList names = dir.entryList();

    // 除去该目录下非工程目录名，如“..”“plan_tasks”“tmp_img”“system”“output”等等，以便显示给用户
    QStringList resultlist;
    QDateTime neweasttime;
    QString filename;
    for (int i = 0; i < names.length(); i++)
    {
        QStringList tmp(names.at(i).split("_"));
        if (tmp.length() >= 2)
        {
            if (neweasttime.isValid())
            {
                if (neweasttime < QDateTime::fromString(tmp.at(1).left(9), "yyyyMMdd hhmmss"))
                {
                    neweasttime = QDateTime::fromString(tmp.at(1).left(9), "yyyyMMdd hhmmss");
                    filename = names.at(i);
                }
            }
            neweasttime = QDateTime::fromString(tmp.at(1).left(9), "yyyyMMdd hhmmss");
            filename = names.at(i);
        }
    }
    
    if (neweasttime.isValid())
    {
        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::red);
        ui->newestLabel->setPalette(pe);
        ui->newestLabel->setText(QObject::tr("请先导入数据库文件%1").arg(filename));
    }
    else
        ui->newestLabel->setText(QObject::tr("没有需要新导入的数据库文件"));
}

void BackupDatabaseWidget::importSQLFileToDB()
{
    QString opendir = ClientSetting::getSettingInstance()->getParentPath() + "/db_backup/" + newestDBDumpFileName;
    QString sqlfilename = QFileDialog::getOpenFileName(this, QObject::tr("选择(.SQL)数据文件"), opendir, "SQL Files (*.sql)");
    QFileInfo info = QFileInfo(sqlfilename);
    QString importfilename = info.fileName();
    if (sqlfilename.isEmpty())//如果文件名不为空
        return;
    
    QMessageBox::StandardButton rb=QMessageBox::question(NULL,tr("警告"),tr("该操作会清空数据库数据，并将%1文件中的数据导入数据库。确认这样吗？（需谨慎）").arg(importfilename),QMessageBox::Yes|QMessageBox::No);
    if (rb == QMessageBox::Yes)
    {
        ui->status_textEdit->setText(tr("%1文件正在向数据库导入。。。").arg(importfilename));
        // 正在导入，不能点击
        ui->leading_in_pushButton->setEnabled(false);
        ui->leading_out_pushButton->setEnabled(false);

        //mysql.exe更新数据库
        DumpDatabase::getDumpDatabaseInstance()->importDatabase(QString(tr("%1").arg(sqlfilename)), 1);
        //        //或者用另一种方法更新数据库
        //        DumpDatabase::getDumpDatabaseInstance()->updateDatabase("backup_1_20140316.sql");

        ui->leading_in_pushButton->setEnabled(false);

        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::black);
        ui->newestLabel->setPalette(pe);
        QDateTime tmpstamp = QDateTime::fromString(sqlfilename.right(21).left(19), "yyyyMMdd hhmmss");
        DumpDatabase::getDumpDatabaseInstance()->setDBUpdateTime_Setting(tmpstamp);
        ui->newestLabel->setText(QObject::tr("当前备份时间:%1").arg(tmpstamp.toString("yyyy-MM-dd hh:mm:ss")));
    }
}

void BackupDatabaseWidget::exportDBToSQLFile()
{
    QString openFilePath = ClientSetting::getSettingInstance()->getParentPath() + "/db_backup/";

    QString sqlfilepath = QFileDialog::getExistingDirectory(this,tr("选择存储路径"),openFilePath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (sqlfilepath.isEmpty())//如果文件路径为空
        return;
    
    // @TODO 当前时间
    //QString datetimestr = DumpDatabase::getDumpDatabaseInstance()->getNewestUpdateTime().toString("yyyyMMdd hhmmss");
    QString datetimestr = QDateTime::currentDateTime().toString("yyyyMMdd hhmmss");
    QString exportfilename = QString(tr("%1/backup_1_%2.sql")).arg(sqlfilepath).arg(datetimestr);

    QMessageBox::StandardButton rb = QMessageBox::question(NULL,tr("警告"),tr("确定要将数据库导出文件%1中吗？").arg(exportfilename),QMessageBox::Yes|QMessageBox::No);

    if (rb == QMessageBox::Yes)
    {
        if (QFile(exportfilename).exists())
        {
            QMessageBox::StandardButton rb2 = QMessageBox::question(NULL,tr("警告"),tr("文件%1已存在，确认覆盖？").arg(exportfilename),QMessageBox::Yes|QMessageBox::No);
            if (rb2 != QMessageBox::Yes)
                return;
        }

        ui->status_textEdit->append(tr("数据库正在导出到文件%1。。。").arg(exportfilename));
        // 正在导出，不能点击
        ui->leading_in_pushButton->setEnabled(false);
        ui->leading_out_pushButton->setEnabled(false);

        //用mysqldump.exe程序导出数据库，即备份数据库
        DumpDatabase::getDumpDatabaseInstance()->exportDatabase(exportfilename, 1);
        // DumpDatabase::getDumpDatabaseInstance()->backupDatabase(QString(tr("%1//backup_1_%2.sql")).arg(sqlfilepath).arg(Date_Time), 1);

        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::red);
        ui->newestLabel->setPalette(pe);
        bool ret;
        DumpDatabase::getDumpDatabaseInstance()->setDBUpdateTime_Setting(QDateTime::fromString(datetimestr, "yyyyMMdd hhmmss"), timestampfilename);
        ui->newestLabel->setText(QObject::tr("当前备份时间:%1").arg(QDateTime::fromString(datetimestr, "yyyyMMdd hhmmss").toString("yyyy-MM-dd hh:mm:ss")));
    }
}

void BackupDatabaseWidget::receiveFinish(bool isimport, QString filename, QString err)
{
    if (isimport)
        ui->status_textEdit->append(tr("导入数据库成功！"));
    else
        ui->status_textEdit->append(tr("导出到文件%1成功！").arg(filename));
    
    // 可重新点击
    ui->leading_in_pushButton->setEnabled(true);
    ui->leading_out_pushButton->setEnabled(true);
}
