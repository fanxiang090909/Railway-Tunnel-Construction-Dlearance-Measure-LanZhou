#include "LzSynth.h"
#include "LzSerialStorageSynthesis.h"

#include "LzFilter.h"
#include "output_heights_list.h"
#include <QDebug>

/**
 * 单隧道隧道综合计算类实现
 * @author 范翔
 * 调用步骤
 * （1）调用initSynthesis
 * （2）调用synthesis
 * @date 2014-5-30
 * @version 1.0.0
 */
LzSynthesis::LzSynthesis(QObject *parent) : QObject(parent)
{
    currentTunnelData = NULL;
    currentCheckedTunnelModel = NULL;
    hasinit = false;
}

LzSynthesis::~LzSynthesis()
{
    // currentTunnelData与不在currentCheckedTunnelModel不在这里delete，因为不在这里new
}

/**
 * 隧道综合初始化，设置两个结构，检查输出高度配置文件（output_heights.xml）是否加载
 * @param initialTunnelheight_syn_name 输入隧道提高度结果文件名（含路径）
 * @param initialTunnelData 输入数据库中查出的该隧道模型, 包含有无左曲线、有曲线、直线及其曲线里程起止信息、 @see TunnelDataModel
 * @param initialCheckedTunnelModel 输入的校对后的隧道信息模型，包含对应的计划隧道信息，实际采集的“隧道”文件信息，可能不只一条（误触发多采集） @see CheckedTunnelTaskModel
 * @return true 检查输出高度配置文件（output_heights.xml）是否加载
 */
bool LzSynthesis::initSynthesis(string initialTunnelheight_syn_name, TunnelDataModel * initialTunnelData, 
								CheckedTunnelTaskModel * initialCheckedTunnelModel, float framedistance)
{
    tunnelheight_syn_name = initialTunnelheight_syn_name;
    currentTunnelData = initialTunnelData;
    currentCheckedTunnelModel = initialCheckedTunnelModel;
    currentFrameDistance = framedistance;

    if (OutputHeightsList::getOutputHeightsListInstance()->getCurrentHeightsVersion() == 0)
    {
        hasinit = false;
        std::cout << "heights init false";
        return false;
    }

    hasinit = true;
    return true;
}

/**
 * 综合函数
 * @param straightdata 返回值：直线段断面数据
 * @param leftdata 返回值：左曲线线段断面数据
 * @param rightdata 返回值：右曲线线段断面数据
 * @param hasstraight 返回值：是否含有直线断面
 * @param hasleft 返回值：是否含有左曲线线段断面
 * @param hasright 返回值：是否含有右曲线线段断面
 */
void LzSynthesis::synthesis(ClearanceData& straightdata, ClearanceData& leftdata,
                    ClearanceData& rightdata,
                    bool & hasstraight, bool & hasleft, bool & hasright)
{
    if (!hasinit)
        return;

    // 初始化时检查需要初始化几个表格
    hasstraight = false, hasleft = false, hasright = false;
    // 【需求相关】 不管左（右）曲线段数量有多少，始终输出一个左（右）曲线限界表格
    
    // TODO
    // 如果没有左转右转曲线，则一定包含直线段
    //if (currentTunnelData->getNumberOfStrights() > 0)
    //{
    hasstraight = true;
    straightdata.initMaps();	// 初始化高度，不可缺少
    //}
    if (currentTunnelData->getNumberOfLeftCurves() > 0)
    {
        hasleft = true;
        leftdata.initMaps();	// 初始化高度，不可缺少
    }
    if (currentTunnelData->getNumberOfRightCurves() > 0)
    {
        hasright = true;
        rightdata.initMaps();	// 初始化高度，不可缺少
    }

    // 临时存储当前帧的高度数据
    SectionData tempdata;
    tempdata.initMaps(); // 初始化高度，不可缺少

    // 调用读取指定高度流式文件类
    LzSerialStorageSynthesis * syn = new LzSerialStorageSynthesis();

    // 当前CheckedTunnelModel包含的文件位置
    /*********************************/
    string filename = tunnelheight_syn_name;

    // 打开文件 @see CheckedTunnelTaskModel结构，一条计划隧道可能对应实际多条采集“隧道”
    // 但是在融合时已经生成了一条隧道提取高度文件，文件名为“(projectpath)/syn_data/隧道名_采集日期.syn”
    // 【注意！】设置缓存大小1M，默认1G太大
    /**********读文件**********/
    syn->setting(100, 1024*1024, true);
    if (!syn->openFile(filename.c_str()))
    {
        qDebug() << "can not open file:" << QString::fromLocal8Bit(filename.c_str());
        delete syn;
        return;
    }
    qDebug() << "open file:" << QString::fromLocal8Bit(filename.c_str());

    DataHead dataread;
    // 读文件头
    syn->readHead(&dataread);
    qDebug() << "carriage_direction:" << dataread.carriage_direction << "is_normal:" << dataread.is_normal << "tunnel_id:" << dataread.tunnel_id <<
              "tunnel_name:" << QString::fromLocal8Bit(dataread.tunnel_name) << "datetime" << dataread.datetime;

    //if (currentTunnelData->compare(dataread) == false)
    //{
    //	qDebug() << "synthesis file header different with tunnels data in database";
        ////TODO 若不一致，警告人工处理
    //}

    // 两个隧道综合判断里程的必要变量
    bool carriage_direction = dataread.carriage_direction;  // 此次采集的车厢正反
    bool is_normal = dataread.is_normal;                   // 此次拍摄的双线正常/非正常信息，（单线正序/逆序信息）
    bool is_doubleline = dataread.is_double_line;
    bool is_down_link = dataread.is_downlink;
    _int64 startPoint = (_int64) dataread.start_mileage;
    _int64 endPoint = (_int64) dataread.end_mileage;

    std::vector<BLOCK_KEY> keys = syn->readKeys();
    std::cout << "size:" << keys.size();
    //for (int i = 0; i < keys.size(); i++)
    //    qDebug() << keys.at(i);

    __int64 framecounter;
    double mile;
    double tempmile;
    float centerheight;

    // 文件取出帧数
    bool ret = syn->retrieveMap(keys.at(0));
    if (ret == false)
    {
        syn->closeFile();
        qDebug() << "close file:" << QString::fromLocal8Bit(filename.c_str());
        delete syn;
        return;
    }

    // 反馈计算起始、终止帧号
    emit initfc(keys.at(0), keys.at(keys.size()-1));
    __int64 startfc = keys.at(0);
    __int64 endfc = keys.at(keys.size()-1);

    __int64 framecount = endfc - startfc;

    // 临时变量findInterMile()函数引用
    CurveType curvetype;
    int curveid;
    int curveradius;
    bool intermileret;

    std::cout << "retrive result:" << ret;
    for (int i = 0; i < keys.size(); i++)
    {
        // 在readMap中调用的blocktomap中已调用data.resetMaps();
        ret = syn->readMap(framecounter, mile, centerheight, tempdata);
        if (ret == false)
             continue;
        qDebug() << "read " << i << "times. return:" << ret;
        qDebug() << "mile:" << mile << ", framecounter:" << framecounter << ", centerheight:" << centerheight;
        //tempdata.showMaps();

        emit currentfc(framecounter);

        // 里程折算
        //@author范翔20150423注释
        tempmile = (framecounter - startfc) * currentFrameDistance;
;
        if (!Filter_TunnelMileage_InterToActual(is_normal, is_doubleline, is_down_link, startPoint, endPoint, mile))
        {
            mile = tempmile;
            intermileret = false;
        }
        mile = tempmile;
        //else
        {
            // 判断更新直，左，右的限界
            intermileret = currentTunnelData->findInterMile(mile, curvetype, curveid, curveradius);
        }
        
		//straightdata.showMaps();

        //@author范翔20150423注释
        //if (intermileret)
            switch (curvetype)
            {
                case 0:	tempdata.setMile(mile);
                        tempdata.setCenterHeight(centerheight);
                        tempdata.setType(CurveType::Curve_Straight);
                        tempdata.updateToClearanceData(straightdata);
                        hasstraight = true;
                        break;
                case -1:tempdata.setMile(mile);
                        tempdata.setCenterHeight(centerheight);
                        tempdata.setRadius(curveradius);
                        tempdata.setType(CurveType::Curve_Left);
                        //  曲线折减
                        clearanceReduction(tempdata, carriage_direction);
                        hasleft = true;
                        tempdata.updateToClearanceData(leftdata);
                        break;
                case 1: tempdata.setMile(mile);
                        tempdata.setCenterHeight(centerheight);
                        tempdata.setRadius(curveradius);
                        tempdata.setType(CurveType::Curve_Right);
                        //  曲线折减
                        clearanceReduction(tempdata, carriage_direction);
                        hasright = true;
                        tempdata.updateToClearanceData(rightdata);
                        break;
                default:break;
            }
        /*else
        {
            // 表示未找到该里程
            tempdata.setMile((-1)*mile);
            tempdata.setCenterHeight(centerheight);
            tempdata.setType(CurveType::Curve_Straight);
            tempdata.updateToClearanceData(straightdata);
            hasstraight = true;
            qDebug() << "can not find mile in tunnel" << QString::fromLocal8Bit(currentTunnelData->getName().c_str());
        }*/
        //straightdata.showMaps();
    }
    // 关闭文件
    syn->closeFile();
    qDebug() << "close file:" << QString::fromLocal8Bit(filename.c_str());
    /*********************************/

    delete syn;
}
