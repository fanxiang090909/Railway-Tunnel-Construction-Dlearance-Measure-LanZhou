#include <QtGui/QApplication>
#include <QTextCodec>
#include <QTimer>
#include <QDebug>

#include "lz_db.h"
#include "xmlsynthesisheightsfileloader.h"
#include "LzSynth_n.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*******************************/
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK")); //当QString变量中含有中文时，需要增加这两行代码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

    qDebug() << "helloworld";

    QString filename = "./database/stu_analysis.s3db";

    if (!CREATE_MYSQL_CONNECTION(filename))
        return 1;

    // 测试用mysqldump.exe程序备份数据库
    //DumpDatabase::getDumpDatabaseInstance()->backupDatabase(QString("D://backup_2_20140305.sql"), 2);
    //DumpDatabase::getDumpDatabaseInstance()->backupDatabase(QString("D://backup_1_20140305.sql"), 1);

    // 测试用mysql.exe更新数据库
    DumpDatabase::getDumpDatabaseInstance()->clearAndReloadDababase("D://backup_2_20140305.sql");

    // 测试自己写更新数据库
    //DumpDatabase::getDumpDatabaseInstance()->updateDatabase("D://backup_2_20140305.sql");

    // 测试基础隧道信息用的
    TunnelDataModel * m = new TunnelDataModel(656);
    m->loadTunnelData();
    qDebug() << "id" << m->getId() << "name" << QString::fromLocal8Bit(m->getName().c_str()) << "idstd" << m->getIdStd().c_str()
             << "isdouble" << m->getIsDoubleLine() << "isdownlink" << m->getIsDownlink() << "isnew" << m->getIsNew()
             << "linetype" << m->getLineType() << "endpoint" << m->getEndPoint() << "startpoint" << m->getStartPoint()
             << "lineid" << m->getLineid() << "linename" << QString::fromLocal8Bit(m->getLinename().c_str());
    qDebug() << "list size:" << m->getCurveList().size() << m->getNumberOfLeftCurves() << m->getNumberOfRightCurves() << m->getNumberOfStrights();

    for (list<CurveDataModel>::iterator it = m->begin(); it != m->end(); it++)
    {
        qDebug() << "hahaha" << it->radius << it->isLeft;
    }

    XMLSynthesisHeightsFileLoader * ff3 = new XMLSynthesisHeightsFileLoader("output_heights.xml");
    ff3->loadFile();
    delete ff3;

    // 测试隧道限界数据存入数据库
    ClearanceData data;
    data.initMaps();
    data.updateToMapVals(150, 20, 5000, -1, -1, true);
    data.updateToMapVals(150, 30, 5000, -1, -1, false);
    data.updateToMapVals(200, 22, 5000, -1, -1, false);
    data.updateToMapVals(300, 543, 5000, -1, -1, false);
    data.updateToMapVals(350, 4353, 5000, -1, -1, false);
    data.updateToMapVals(500, 123, 5000, -1, -1, false);
    data.updateToMapVals(600, 34, 5000, -1, -1, false);
    data.updateToMapVals(800, 455, 5000, -1, -1, false);
    data.updateToMapVals(1000, 234, 5000, -1, -1, false);
    data.updateToMapVals(1100, 888, 5000, -1, -1, false);
    data.updateToMapVals(1210, 666, 5000, -1, -1, false);
    data.updateToMapVals(5000, 3456, 5000, -1, -1, false);
    data.updateToMapVals(5200, 230, 5000, -1, -1, true);
    data.updateToMapVals(5200, 356, 5000, -1, -1, false);
    data.updateToMapVals(6400, 230, 5000, -1, -1, true);
    data.updateToMapVals(7000, 356, 5000, -1, -1, false);
    data.updateToMapVals(2222, 123, 5000, -1, -1, true);
    ClearanceType type = Straight_Smallest;
    int ret1 = ClearanceOutputDAO::getClearanceOutputDAOInstance()->clearanceDataToDBData(data, 6, type);
    qDebug() << "clearanceDataToDBData" << ret1;
    // 测试隧道限界数据从数据库中读出
    ClearanceData dataread;
    dataread.initMaps();
    bool ret = ClearanceOutputDAO::getClearanceOutputDAOInstance()->DBDataToClearanceData(dataread, 6, type);
    qDebug() << "DBDataToClearanceData" << ret;
    dataread.showMaps();

    // 多隧道综合测试
    std::list<long> abc;
    abc.push_back(1);
    abc.push_back(2);
    abc.push_back(3);
    abc.push_back(4);
    abc.push_back(5);
    int numl,numr,nums;
    LzNTunnelsSynthesis lznsyn;
    lznsyn.initSynthesis(abc);
    lznsyn.synthesis(dataread, dataread, dataread, nums, numl, numr);

    return a.exec();
}
