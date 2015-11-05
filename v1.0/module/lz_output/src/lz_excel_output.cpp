#include "lz_excel_output.h"

#include <QFileInfo>
#include <QTextCodec>
#include <QDateTime>

/**
 * 兰州项目限界数据图表输出
 * EXCEL格式输出限界图表
 * @author 范翔
 * @date 2014-06-12
 * @version 1.0.0
 */
LzExcelOutput::LzExcelOutput () 
{
    excel1 = NULL;
    hasinit = false;
}
	
LzExcelOutput::~LzExcelOutput () 
{
    if (hasinit)
        delete excel1;
}
	
int LzExcelOutput::outputSingleSection(TunnelDataModel * inputbasicdata, QString collectdate, SectionData & inputdata, QString templatepath, QString outputfile, QString insertimgpath)
{
    QString templatename = "SingleSectionTemplate.xls";
    int ret = copy(templatepath, templatename, outputfile);
    qDebug() << templatepath << outputfile << insertimgpath;

    if (ret != 0)
        return ret;

    // 插入图片路径不存在
    if (!QFile(insertimgpath).exists())
        return 3;

    //操作已经存在的excel，生成限界尺寸表
    ExcelEngine excel;

    excel.Open(outputfile, 1, false);

    // 限界表名称
    excel.SetCellData(1,1,QObject::tr("%1隧道单幅尺寸表").arg(QObject::tr(inputbasicdata->getName().c_str())));
    excel.SetCellData(52,1,QObject::tr("%1隧道断面示意图").arg(QObject::tr(inputbasicdata->getName().c_str())));

    // 线路名称
    excel.SetCellData(3,2,QObject::tr(inputbasicdata->getLinename().c_str()));
    excel.SetCellData(54,2,QObject::tr(inputbasicdata->getLinename().c_str()));

    // 区段///TODO
    excel.SetCellData(3,7,QObject::tr(inputbasicdata->getLinename().c_str()));
    // 面向站名
    excel.SetCellData(3,10,QObject::tr(inputbasicdata->getLineEndStation().c_str()));
    excel.SetCellData(54,8,QObject::tr(inputbasicdata->getLineEndStation().c_str()));

    // 桥隧名称
    excel.SetCellData(4,3,QObject::tr(inputbasicdata->getName().c_str()));
    excel.SetCellData(54,4,QObject::tr(inputbasicdata->getName().c_str()));

    // 时间
    QByteArray ba = collectdate.toLocal8Bit();
    excel.SetCellData(48,10,ba.constData());
    excel.SetCellData(54,10,ba.constData());

    // 计算隧道长度
    _int64 tunnellength = inputbasicdata->getEndPoint() - inputbasicdata->getStartPoint();
    if (tunnellength < 0)
        tunnellength = 0 - tunnellength;
    //excel.SetCellData(102,3,tunnellength); 

    // 计算中心里程 隧道中心点的里程数
    float centerlength = 1.0 * (inputbasicdata->getEndPoint() + inputbasicdata->getStartPoint()) / 2; 
    excel.SetCellData(4,7,centerlength);
    excel.SetCellData(101,3,centerlength); 

    SectionData & data = inputdata;
    int rowcount = data.getMaps().size();
    
    // Excel模板从第9行开始为高度限界数据，前面几行为表头或空格
    int startrownum = 9;
    int startcolumnnumdef = 1;
    
    // 临时变量
    int startcolumnnum = startcolumnnumdef;
    float tmpval;
    __int64 tmppos;
    int tmpradius;
    
    {
        startcolumnnum = startcolumnnumdef;
        
        CurveType type = inputdata.getType();
        long long pos = inputdata.getMile();
        int radius = inputdata.getRadius();
        float minHeight = inputdata.getCenterHeight();

        switch(type)
        {
            case Curve_Straight:
                startcolumnnum += LzOutputTableColumn::Straight_Left_Val;
                break;
            case Curve_Left:
                startcolumnnum += 3;
                break;
            case Curve_Right:
                startcolumnnum += 7;
                break;
            default:
                break;
        }

        // map反向遍历，正向为高度从小到大，插入图表是需要高度从大到小
        std::map<int, item>::reverse_iterator it = data.getMaps().rbegin();
        int k = 0; // 相对行数
        while (it != data.getMaps().rend())
        {
            std::pair<int, item> pair = (*it);

            if (pair.first > minHeight)
            {
                it++;
                k++;
                continue;
            }

            // 此处写要插入的内容
            if (pair.second.left >= 0)
                excel.SetCellData(k + startrownum, startcolumnnum, (int)pair.second.left); 
            if (pair.second.right >= 0)
                excel.SetCellData(k + startrownum, startcolumnnum + 1, (int)pair.second.right);
            if (type != Curve_Straight) // 不是直线，折减后尺寸
            {
                if (pair.second.left >= 0)
                    excel.SetCellData(k + startrownum, startcolumnnum + 2, (long long)pair.second.left); 
                if (pair.second.right >= 0)
                    excel.SetCellData(k + startrownum, startcolumnnum + 3, (long long)pair.second.left);
            }
            it++;
            k++;
        }
    }

    // <150高度手动输入？ ///TODO

    // 线路中心线上方最低净高mm///TODO
    excel.SetCellData(rowcount+startrownum+1,6,(int)inputdata.getCenterHeight());
    
    // 半径
    int radius = inputdata.getRadius();
    if (radius <= 0)
        radius = 0;
    excel.SetCellData(4,10,radius); 
    excel.SetCellData(54,6,radius); 
    excel.SetCellData(102,3,radius); 

    excel.setProperty("DisplayAlerts", 0);
    
    // 保存
    excel.Save(); 
    excel.Close();    
    
    // 生成限界图
    initExcelEngine();

    QAxObject *workBooks1 = excel1->querySubObject("Workbooks");
    workBooks1->dynamicCall("Open(const QString&)", outputfile);
    QAxObject *workBook1 = excel1->querySubObject("ActiveWorkBook");
    QAxObject *sheets1 = workBook1->querySubObject("WorkSheets");
    QAxObject *sheet1 = sheets1->querySubObject("Item(int)", 1);//获得第一个工作表

    // QAxObject *range=sheet1->querySubObject("Range(const QVariant&)",QVariant(QString("D52:AA92")));
    // range->dynamicCall("Clear()");//指定清除某个区域

    /////////////AddPicture函数详解////////////////
    // AddPicture(FileName, LinkToFile, SaveWithDocument, Left, Top, Width, Height)中7个参数的意思。
    // (1)FileName 必选 String 图片的路径和文件名。
    // (2)LinkToFile 可选 Variant 如果该参数值为 True，则将图片链接到创建它的文件。如果该参数值为 False，则将图片作为该文件的独立副本。默认值为 False。
    // (3)SaveWithDocument 可选 Variant 如果该参数值为 True，则将链接的图片与文档一起保存。默认值为 False。
    // (4)Left 可选 Variant 新图片的左边缘相对于绘图画布的位置，以磅为单位。
    // (5)Top 可选 Variant 新图片的上边缘相对于绘图画布的位置，以磅为单位。
    // (6)Width 可选 Variant 图片的宽度，以磅为单位。
    // (7)Height 可选 Variant 图片的高度，以磅为单位。
    ///////////////////////////////////////////////

    // @author 熊雪、范翔。路径变成反斜杠，否则AddPicture，SaveAs函数出错
    outputfile = toBackSlashStr(outputfile);
    insertimgpath = toBackSlashStr(insertimgpath);

    QAxObject *shapes1 = sheet1->querySubObject("Shapes");
    shapes1->dynamicCall("AddPicture(QString&,bool,bool,double,double,double,double",QString("%1").arg(insertimgpath),true,true,8,770,510,595);//8,750,510,627  QString("%1image.jpg").arg(outputfile)
    excel1->setProperty("DisplayAlerts", 0);
    //excel1->dynamicCall("Save()");
    workBook1->dynamicCall("SaveAs(const QString&)", outputfile);
    // QMessageBox::information(NULL, tr("OK"), tr("成功生成Excel文件！"),QMessageBox::Yes | QMessageBox::No);
    excel1->dynamicCall("Quit()");

    return 0;
}

int LzExcelOutput::outputSingleTunnel(TunnelDataModel * inputbasicdata, ClearanceSingleTunnel* inputdata, QString templatepath, QString outputfile, QString insertimgpath) 
{
    QString templatename = "SingleTunnelTemplate.xls";
    int ret = copy(templatepath, templatename, outputfile);
    if (ret != 0)
        return ret;

    // 插入图片路径不存在
    if (!QFile(insertimgpath).exists())
        return 3;

    //操作已经存在的excel，生成限界尺寸表
    ExcelEngine excel;

    excel.Open(outputfile, 1, false);

    // 限界表名称
    excel.SetCellData(1,1,QObject::tr("%1桥隧综合最小建筑限界尺寸表").arg(QObject::tr(inputbasicdata->getName().c_str())));
    excel.SetCellData(52,1,QObject::tr("%1桥隧综合最小建筑限界尺寸图").arg(QObject::tr(inputbasicdata->getName().c_str())));

    // 线路名称
    excel.SetCellData(3,3,QObject::tr(inputbasicdata->getLinename().c_str()));
    excel.SetCellData(54,2,QObject::tr(inputbasicdata->getLinename().c_str()));

    // 起讫站名
    excel.SetCellData(3,9,inputdata->getQiQiZhanMing());
    // 面向站名
    excel.SetCellData(3,15,QObject::tr(inputbasicdata->getLineEndStation().c_str()));
    excel.SetCellData(54,13,QObject::tr(inputbasicdata->getLineEndStation().c_str()));

    // 桥隧名称
    excel.SetCellData(4,4,QObject::tr(inputbasicdata->getName().c_str()));
    excel.SetCellData(54,5,QObject::tr(inputbasicdata->getName().c_str()));

    // 隧道编号
    excel.SetCellData(54, 8, QObject::tr(inputbasicdata->getIdStd().c_str()));


    // 时间
    QStringList strList = inputdata->getTaskTunnelInfo().split("_");
    QByteArray ba = strList.at(1).toLocal8Bit();
    QDateTime tmpdate = QDateTime::fromString(ba.constData(), "yyyyMMdd");
    QString date = tmpdate.toString("yyyy-MM-dd");
    excel.SetCellData(49,15, date.toLocal8Bit().constData());
    excel.SetCellData(54,16, date.toLocal8Bit().constData());
    // TODO表上再插入一个时间

    // 起讫里程
    _int64 startdistance = inputbasicdata->getStartPoint();
    QString startenddistance = QString("%1-%2").arg(inputbasicdata->getStartPoint()).arg(inputbasicdata->getEndPoint());
    excel.SetCellData(4,16,startenddistance); 

    // 计算隧道长度
    _int64 tunnellength = inputbasicdata->getEndPoint() - inputbasicdata->getStartPoint();
    if (tunnellength < 0)
        tunnellength = 0 - tunnellength;
    excel.SetCellData(102,3,tunnellength); 

    // 计算中心里程 隧道中心点的里程数
    float centerlength = (inputbasicdata->getEndPoint() + inputbasicdata->getStartPoint()) / 2; 
    excel.SetCellData(4,10,centerlength);
    excel.SetCellData(101,3,centerlength); 

    // 最小曲线半径
    excel.SetCellData(5, 4, inputdata->getMinRadius()); 
    excel.SetCellData(54, 10, inputdata->getMinRadius()); 

    // 最大外轨超高
    excel.SetCellData(5,10,inputdata->getWaiGuiChaoGao()); 
    // 最低接触网高度
    excel.SetCellData(5,16,inputdata->getJieChuWangGaoDu()); 


    ClearanceData * data = &inputdata->getClearanceStraightData();
    int rowcount = data->getMaps().size();
    
    // Excel模板从第10行开始为高度限界数据，前面几行为表头或空格
    int startrownum = 10;
    int startcolumnnumdef = 1;
    
    // 临时变量
    int startcolumnnum = startcolumnnumdef;
    float tmpval;
    _int64 tmppos;
    int tmpradius;
    float minHeight;
    
    for (int i = 0; i < 3; i++)
    {
        startcolumnnum = startcolumnnumdef;
        
        switch(i)
        {
            case Curve_Straight:
                if (inputdata->getHasStraight())
                {
                    data = &inputdata->getClearanceStraightData();
                    startcolumnnum += LzOutputTableColumn::Straight_Left_Val;
                }
                else
                    continue;
                break;
            case Curve_Left:
                if (inputdata->getHasLeft())
                {
                    data = &inputdata->getClearanceLeftData();
                    startcolumnnum += LzOutputTableColumn::LeftCurve_Left_Val;
                }
                else
                    continue;
                break;
            case Curve_Right:
                if (inputdata->getHasRight())
                {
                    data = &inputdata->getClearanceRightData();
                    startcolumnnum += LzOutputTableColumn::RightCurve_Left_Val;
                }
                else
                    continue;
                break;
            default:
                break;
        }

        minHeight = data->getMinCenterHeight();

        // map反向遍历，正向为高度从小到大，插入图表是需要高度从大到小
        std::map<int, ClearanceItem>::reverse_iterator it = data->getMaps().rbegin();
        int k = 0; // 相对行数
        while (it != data->getMaps().rend())
        {
            std::pair<int, ClearanceItem> pair = (*it);

            if (pair.first > minHeight)
            {
                it++;
                k++;
                continue;
            }

            // 此处写要插入的内容
            if (pair.second.leftval >= 0)
                excel.SetCellData(k + startrownum, startcolumnnum, pair.second.leftval); 
            if (pair.second.rightval >= 0)
                excel.SetCellData(k + startrownum, startcolumnnum + 1, pair.second.rightval);
            
            if (i == Curve_Straight) // 直线
            {
                if (pair.second.leftval >= 0)
                    excel.SetCellData(k + startrownum, startcolumnnum + 2, pair.second.leftpos); 
                if (pair.second.rightval >= 0)
                    excel.SetCellData(k + startrownum, startcolumnnum + 3, pair.second.rightpos);
            }
            else
            {
                if (pair.second.leftval >= 0)
                {
                    excel.SetCellData(k + startrownum, startcolumnnum + 2, pair.second.leftradius); 
                    excel.SetCellData(k + startrownum, startcolumnnum + 4, pair.second.leftpos); 
                }
                if (pair.second.rightval >= 0)
                {
                    excel.SetCellData(k + startrownum, startcolumnnum + 3, pair.second.rightradius);
                    excel.SetCellData(k + startrownum, startcolumnnum + 5, pair.second.rightpos);
                }
            }
            it++;
            k++;
        }
    }

    // <150高度手动输入？ ///TODO

    // 线路中心线上方最低净高mm
    excel.SetCellData(rowcount+startrownum+1,9, inputdata->getMinHeight());
    
    excel.setProperty("DisplayAlerts", 0);
    
    // 保存
    excel.Save(); 
    excel.Close();    

    // 生成限界图
    initExcelEngine();
    QAxObject *workBooks1 = excel1->querySubObject("Workbooks");
    workBooks1->dynamicCall("Open(const QString&)", outputfile);
    QAxObject *workBook1 = excel1->querySubObject("ActiveWorkBook");
    QAxObject *sheets1 = workBook1->querySubObject("WorkSheets");
    QAxObject *sheet1 = sheets1->querySubObject("Item(int)", 1);//获得第一个工作表

    // QAxObject *range=sheet1->querySubObject("Range(const QVariant&)",QVariant(QString("D52:AA92")));
    // range->dynamicCall("Clear()");//指定清除某个区域

    /////////////AddPicture函数详解////////////////
    // AddPicture(FileName, LinkToFile, SaveWithDocument, Left, Top, Width, Height)中7个参数的意思。
    // (1)FileName 必选 String 图片的路径和文件名。
    // (2)LinkToFile 可选 Variant 如果该参数值为 True，则将图片链接到创建它的文件。如果该参数值为 False，则将图片作为该文件的独立副本。默认值为 False。
    // (3)SaveWithDocument 可选 Variant 如果该参数值为 True，则将链接的图片与文档一起保存。默认值为 False。
    // (4)Left 可选 Variant 新图片的左边缘相对于绘图画布的位置，以磅为单位。
    // (5)Top 可选 Variant 新图片的上边缘相对于绘图画布的位置，以磅为单位。
    // (6)Width 可选 Variant 图片的宽度，以磅为单位。
    // (7)Height 可选 Variant 图片的高度，以磅为单位。
    ///////////////////////////////////////////////
    
    // @author 熊雪、范翔。路径变成反斜杠，否则AddPicture，SaveAs函数出错
    outputfile = toBackSlashStr(outputfile);
    insertimgpath = toBackSlashStr(insertimgpath);

    QAxObject *shapes1 = sheet1->querySubObject("Shapes");
    shapes1->dynamicCall("AddPicture(QString&,bool,bool,double,double,double,double",insertimgpath,true,true,8,770,510,595);//8,750,510,627  QString("%1image.jpg").arg(outputfile)
    excel1->setProperty("DisplayAlerts", 0);
    //excel1->dynamicCall("Save()");
    workBook1->dynamicCall("SaveAs(const QString&)", outputfile);
    // QMessageBox::information(NULL, tr("OK"), tr("成功生成Excel文件！"),QMessageBox::Yes | QMessageBox::No);
    excel1->dynamicCall("Quit()");

    return 0;
}

int LzExcelOutput::outputMultiTunnels(ClearanceMultiTunnels * inputdata, QString templatepath, QString outputfile, QString insertimgpath) 
{
    QString templatename = "MultiTunnelsTemplate.xls";
    int ret = copy(templatepath, templatename, outputfile);
    if (ret != 0)
        return ret;

    // 插入图片路径不存在
    if (!QFile(insertimgpath).exists())
        return 3;

    //操作已经存在的excel，生成限界尺寸表
    ExcelEngine excel;

    excel.Open(outputfile, 1, false);
    
    QString linename = inputdata->getLineName(); 
    // 限界表名称
    excel.SetCellData(1,1,QObject::tr("%1-区段桥隧综合最小建筑限界尺寸表").arg(linename));
    excel.SetCellData(52,1,QObject::tr("%1-区段桥隧综合最小建筑限界尺寸图").arg(linename));

    // 线路名称
    excel.SetCellData(3,3,linename);
    excel.SetCellData(54,2,linename);
    excel.SetCellData(54,5,linename);

    // 区段
    excel.SetCellData(3,9,linename);

    QString endStationName = inputdata->getEndStationName();
    // 面向站名
    excel.SetCellData(3,15,endStationName);
    excel.SetCellData(54,13,endStationName);

    QStringListModel * tunnelsmodel = inputdata->getTunnelsNames();

    // 桥隧总数
    excel.SetCellData(4,4,tunnelsmodel->rowCount());
    // 桥梁数
    excel.SetCellData(4,11,inputdata->getNumOfOutOfClearanceBridges());
    // 隧道数
    excel.SetCellData(4,14,inputdata->getNumOfOutOfClearanceTunnels());

    // 时间
    // 采集日期
    QModelIndex index = tunnelsmodel->index(0,0);
    QString tunnelinfo = index.data().toString();//这两行代码获取QStringListModel中第一行的值
    QStringList strlist = tunnelinfo.split("-");
    if (strlist.length() < 3)
        return -1;
    QString collectdate = strlist.at(2);
    excel.SetCellData(49,15,collectdate);
    excel.SetCellData(54,16,collectdate);

    // 最小曲线半径

    int minRadius = inputdata->getMinRadius();
    excel.SetCellData(5,5,minRadius);
    // 最大外轨超高
    excel.SetCellData(5,10,inputdata->getWaiGuiChaoGao()); 
    // 最低接触网高度
    excel.SetCellData(5,16,inputdata->getJieChuWangGaoDu()); 

    ClearanceData * data = &inputdata->getClearanceStraightData();
    int rowcount = data->getMaps().size();
    
    // Excel模板从第10行开始为高度限界数据，前面几行为表头或空格
    int startrownum = 10;
    int startcolumnnumdef = 1;
    
    // 临时变量
    int startcolumnnum = startcolumnnumdef;
    float tmpval;
    _int64 tmppos;
    int tmpradius;
    float minHeight;

    for (int i = 0; i < 3; i++)
    {
        startcolumnnum = startcolumnnumdef;
        
        switch(i)
        {
            case Curve_Straight:
                if (inputdata->getNumOfStraight())
                {
                    data = &inputdata->getClearanceStraightData();
                    startcolumnnum += LzOutputTableColumn::Straight_Left_Val;
                }
                else
                    continue;
                break;
            case Curve_Left:
                if (inputdata->getNumOfLeft())
                {
                    data = &inputdata->getClearanceLeftData();
                    startcolumnnum += LzOutputTableColumn::LeftCurve_Left_Val;
                }
                else
                    continue;
                break;
            case Curve_Right:
                if (inputdata->getNumOfRight())
                {
                    data = &inputdata->getClearanceRightData();
                    startcolumnnum += LzOutputTableColumn::RightCurve_Left_Val;
                }
                else
                    continue;
                break;
            default:
                break;
        }

        minHeight = data->getMinCenterHeight();

        // map反向遍历，正向为高度从小到大，插入图表是需要高度从大到小
        std::map<int, ClearanceItem>::reverse_iterator it = data->getMaps().rbegin();
        int k = 0; // 相对行数
        while (it != data->getMaps().rend())
        {
            std::pair<int, ClearanceItem> pair = (*it);

            if (pair.first > minHeight)
            {
                it++;
                k++;
                continue;
            }

            // 此处写要插入的内容
            if (pair.second.leftval >= 0)
                excel.SetCellData(k + startrownum, startcolumnnum, pair.second.leftval); 
            if (pair.second.rightval >= 0)
                excel.SetCellData(k + startrownum, startcolumnnum + 1, pair.second.rightval);
            if (i == Curve_Straight) // 直线
            {
                if (pair.second.leftval >= 0)
                {
                    excel.SetCellData(k + startrownum, startcolumnnum + 2, toTunnelPosString2(pair.second.leftpos, pair.second.lefttunnelid, "0"));
                    qDebug() << toTunnelPosString2(pair.second.leftpos, pair.second.lefttunnelid, "0");
                }
                if (pair.second.rightval >= 0)
                {
                    excel.SetCellData(k + startrownum, startcolumnnum + 3, toTunnelPosString2(pair.second.rightpos, pair.second.righttunnelid, "0"));
                    qDebug() << toTunnelPosString2(pair.second.rightpos, pair.second.righttunnelid, "0");
                }
            }
            else
            {
                if (pair.second.leftval >= 0)
                {
                    excel.SetCellData(k + startrownum, startcolumnnum + 2, pair.second.leftradius); 
                    excel.SetCellData(k + startrownum, startcolumnnum + 4, toTunnelPosString2(pair.second.leftpos, pair.second.lefttunnelid, "0"));
                    qDebug() << toTunnelPosString2(pair.second.leftpos, pair.second.lefttunnelid, "0");
                }
                if (pair.second.rightval >= 0)
                {
                    excel.SetCellData(k + startrownum, startcolumnnum + 3, pair.second.rightradius);
                    excel.SetCellData(k + startrownum, startcolumnnum + 5, toTunnelPosString2(pair.second.rightpos, pair.second.righttunnelid, "0"));
                    qDebug() << toTunnelPosString2(pair.second.rightpos, pair.second.righttunnelid, "0");
                }
            }
            it++;
            k++;
        }
    }

    // <150高度手动输入？ ///TODO

    // 线路中心线上方最低净高mm
    excel.SetCellData(rowcount+startrownum+1,9, inputdata->getMinHeight());
    
    excel.setProperty("DisplayAlerts", 0);
    
    // 保存
    excel.Save(); 
    excel.Close();    
    
    // 生成限界图
    initExcelEngine();
    QAxObject *workBooks1 = excel1->querySubObject("Workbooks");
    workBooks1->dynamicCall("Open(const QString&)", outputfile);
    QAxObject *workBook1 = excel1->querySubObject("ActiveWorkBook");
    QAxObject *sheets1 = workBook1->querySubObject("WorkSheets");
    QAxObject *sheet1 = sheets1->querySubObject("Item(int)", 1);//获得第一个工作表

    // QAxObject *range=sheet1->querySubObject("Range(const QVariant&)",QVariant(QString("D52:AA92")));
    // range->dynamicCall("Clear()");//指定清除某个区域

    /////////////AddPicture函数详解////////////////
    // AddPicture(FileName, LinkToFile, SaveWithDocument, Left, Top, Width, Height)中7个参数的意思。
    // (1)FileName 必选 String 图片的路径和文件名。
    // (2)LinkToFile 可选 Variant 如果该参数值为 True，则将图片链接到创建它的文件。如果该参数值为 False，则将图片作为该文件的独立副本。默认值为 False。
    // (3)SaveWithDocument 可选 Variant 如果该参数值为 True，则将链接的图片与文档一起保存。默认值为 False。
    // (4)Left 可选 Variant 新图片的左边缘相对于绘图画布的位置，以磅为单位。
    // (5)Top 可选 Variant 新图片的上边缘相对于绘图画布的位置，以磅为单位。
    // (6)Width 可选 Variant 图片的宽度，以磅为单位。
    // (7)Height 可选 Variant 图片的高度，以磅为单位。
    ///////////////////////////////////////////////

    // @author 熊雪、范翔。路径变成反斜杠，否则AddPicture，SaveAs函数出错
    outputfile = toBackSlashStr(outputfile);
    insertimgpath = toBackSlashStr(insertimgpath);

    QAxObject *shapes1 = sheet1->querySubObject("Shapes");
    shapes1->dynamicCall("AddPicture(QString&,bool,bool,double,double,double,double",QString("%1").arg(insertimgpath),true,true,8,770,510,595);//8,750,510,627  QString("%1image.jpg").arg(outputfile)
    excel1->setProperty("DisplayAlerts", 0);
    //excel1->dynamicCall("Save()");
    workBook1->dynamicCall("SaveAs(const QString&)", outputfile);
    // QMessageBox::information(NULL, tr("OK"), tr("成功生成Excel文件！"),QMessageBox::Yes | QMessageBox::No);
    excel1->dynamicCall("Quit()");

    return 0;
}

/**
 * 初始化EXCEL生成接口
 */
void LzExcelOutput::initExcelEngine()
{
    if (hasinit)
        return;
    else
    {
        excel1 = new QAxObject("Excel.Application");
        hasinit = true;
    }
}