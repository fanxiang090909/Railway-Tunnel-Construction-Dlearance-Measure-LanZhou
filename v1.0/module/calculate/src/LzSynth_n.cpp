#include "LzSynth_n.h"
#include "output_heights_list.h"
#include "daoclearanceoutput.h"
#include "daotasktunnel.h"
#include "daotunnel.h"
#include <QDebug>
#include <QFileDialog>

/**
 * 单隧道隧道综合计算类实现
 * @author 范翔
 * @date 2014-3-25
 * @version 1.0.0
 */
LzNTunnelsSynthesis::LzNTunnelsSynthesis()
{
    hasinit = false;
    hasinitBaseClearanceData = false;
}

LzNTunnelsSynthesis::~LzNTunnelsSynthesis()
{
}

bool LzNTunnelsSynthesis::initSynthesis(std::list<_int64> init_task_clearance_ids, std::string init_templatepath, OutputClearanceImageType init_type)
{
    this->task_clearance_ids = init_task_clearance_ids;
    this->templatepath = init_templatepath;
    this->outputtype = init_type;

    if (OutputHeightsList::getOutputHeightsListInstance()->getCurrentHeightsVersion() == 0)
    {
        hasinit = false;
        std::cout << "heights init false";
        return false;
    }

    hasinit = true;
    hasinitBaseClearanceData = false;
    return true;
}

// 综合函数
void LzNTunnelsSynthesis::synthesis(ClearanceData& straightdata, ClearanceData& leftdata,
                    ClearanceData& rightdata,
                    int & straightnum, int & leftnum, int & rightnum, int & numofOutOfClearanceTunnels, int & numofOutOfClearanceBridges)
{
    if (!hasinit)
        return;

    loadBaseClearanceTemplateData(this->outputtype);

    straightnum = 0;
    leftnum = 0;
    rightnum = 0;

    numofOutOfClearanceTunnels = 0;
    numofOutOfClearanceBridges = 0;

    // 初始化时检查需要初始化几个表格
    // 【需求相关】 不管左（右）曲线段数量有多少，始终输出一个左（右）曲线限界表格
    
    // 如果没有左转右转曲线，则一定包含直线段
    straightdata.initMaps();	// 初始化高度，不可缺少
    leftdata.initMaps();	// 初始化高度，不可缺少
    rightdata.initMaps();	// 初始化高度，不可缺少

    qDebug() << "size:" << task_clearance_ids.size();

    // 临时存储当前帧的高度数据
    std::list<_int64>::iterator it = task_clearance_ids.begin();
    while (it != task_clearance_ids.end())
    {
        // 是否超限
        bool hasOutOfBaseClearance = false;

        for (int j = 0; j < 3; j++) // 3个OutputType均试一下
        {
            ClearanceType tmptype;

            switch(j)
            {
                case 0: tmptype = Straight_Smallest; break;
                case 1: tmptype = LeftCurve_Smallest; break;
                case 2: tmptype = RightCurve_Smallest; break;
                default: break;
            }

            ClearanceData tempdata;
            tempdata.initMaps(); // 初始化高度，不可缺少

            /**********读数据库限界值**********/
            bool ret = ClearanceOutputDAO::getClearanceOutputDAOInstance()->DBDataToClearanceData(tempdata, (*it), tmptype);
            qDebug() << "DBDataToClearanceData" << ret << ", clearance_outputid=" << (*it);

            // 临时变量findInterMile()函数引用
            tmptype = tempdata.getClearanceType();
            //tempdata.setMinCenterHeightTunnelID(-1)
            int tmptunnelid = -1;
            QString tmptunnelname;
            QString tmpdate;
            QString tmplinename;
            // 根据采集隧道ID得到隧道基本ID
            TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelInfo((*it), tmptunnelid, tmptunnelname, tmpdate, tmplinename);
            tempdata.setTunnelID(tmptunnelid);

            if (ret)
            {
                switch (tmptype)
                {
                    case Straight_Smallest:	    tempdata.updateToClearanceData(straightdata); straightnum++; break;
                    case LeftCurve_Smallest:    tempdata.updateToClearanceData(leftdata); leftnum++; break;
                    case RightCurve_Smallest:   tempdata.updateToClearanceData(rightdata); rightnum++; break;
                    default:qDebug() << "can not find TYPE in task_tunnel_id" << (*it); break;
                }

                // 检查是否超限
                if (!hasOutOfBaseClearance && hasinitBaseClearanceData)
                {
                    hasOutOfBaseClearance = tempdata.checkLessThanStdSectionData(baseClearandeData);
                }

                //straightdata.showMaps();
            }
            /*********************************/
        }

        if (hasOutOfBaseClearance)
        {
            QSqlQueryModel * tunnelinfo = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnels_2((*it));
            QModelIndex index;
            if (tunnelinfo->rowCount() > 0)
            {
                index = tunnelinfo->index(0, Tunnel_is_bridge);
                bool isbridge = tunnelinfo->data(index).toBool();
                if (isbridge)
                    numofOutOfClearanceBridges++;
                else
                    numofOutOfClearanceTunnels++;
            }
        }

        it++;
    }
}

bool LzNTunnelsSynthesis::loadBaseClearanceTemplateData(OutputClearanceImageType type)
{
    hasinitBaseClearanceData = false;
    baseClearandeData.initMaps();

    QFile file;
    QString in;

    QString typefilename = "";
    switch (type)
    {
        case OutputClearanceImageType::OutType_B_NeiRan: typefilename = "BaseClearance_B_NeiRan.sec"; break;
        case OutputClearanceImageType::OutType_B_DianLi: typefilename = "BaseClearance_B_DianLi.sec"; break;
        case OutputClearanceImageType::OutType_D_NeiRan: typefilename = "BaseClearance_D_NeiRan.sec"; break;
        case OutputClearanceImageType::OutType_D_DianLi: typefilename = "BaseClearance_D_DianLi.sec"; break;
        default: typefilename = "BaseClearance_B_DianLi.sec"; break;
    }

    QString filename = QString::fromLocal8Bit(templatepath.c_str()) + "/" + typefilename;
    qDebug() << "filename " << filename;
    file.setFileName(filename);

    // Currently here for debugging purposes
    qDebug() << "Reading file: " << filename.toLocal8Bit().data() << endl;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream inStream(&file);
        
        // readfile
        
        in = inStream.readLine();

        int height;
        int left;
        int right;

        while(!(in.isNull()))
        {
            height = in.section("\t",0,0).toInt();
            left = in.section("\t",1,1).toInt();
            // Reads in fifth column: signal-to-noise ratio
            right = in.section("\t",2,2).toInt();
            
            baseClearandeData.updateToMapVals(height, (float)left, (float)right);

            in = inStream.readLine();
        }

        file.close();

        qDebug() << "read_end" << endl;
        hasinitBaseClearanceData = true;
        return true;
    }
    // Do some proper debugging here at some point...
    else 
    {
        std::cout << "Problem reading file: " << filename.toLocal8Bit().data() << std::endl;
        hasinitBaseClearanceData = false;
        return false;
    }
}