#ifndef TUNNELDATAMODEL_H
#define TUNNELDATAMODEL_H

#include <string>
#include <list>
#include "headdefine.h"
#include "clearance_item.h"

/**
 * 对应数据库中的tunnel模型类声明，可调用的数据访问接口
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-11-16
 */

using namespace std;

/**
 * 对应数据库中的curve模型
 */
struct CurveDataModel
{
    _int64 curveid;
    int radius;
    _int64 pointZhiHuan;
    _int64 pointHuanZhi;
    bool isStraight;
    bool isLeft;
};

/**
 * 对应数据库中的tunnel模型
 * 为方便访问，提供list中存储curveDataModel
 */
class TunnelDataModel
{
public:
    /**
     * 构造函数，指定tunnelid
     */
    TunnelDataModel(int tunnelid);

    /**
     * 析构函数
     */
    ~TunnelDataModel();

    /**
     * !!!必要步骤
     * 已知tunnelid的情况下从数据库中加载相关数据
     */
    bool loadTunnelData();

    /**
     * 比较该数据域DataHead中的数据是否有不同
     * 即比较采集时的隧道基本信息与数据库中的信息是否不同
     * @param head 文件头引用
     * @return true 相同 else 不同，需要警告！！
     */
    bool compare(DataHead & head); 

    /**
     * 左转曲线段
     * @return 曲线数量
     */
    int getNumberOfLeftCurves();

    /**
     * 右转曲线段
     * @return 曲线数量
     */
    int getNumberOfRightCurves();

    /**
     * 直线段
     * @return 直线数量
     */
    int getNumberOfStrights();

    /**
     * 得到该里程在此隧道的曲线，及曲线curveid，及曲线半径
     * @param mileage 【折算后，非采集时】注明内部里程，隧道口为0
     * @param carriagedirection 车厢正反
     * @param curvetype （ref）找到了该里程，在曲线类型上，曲线类型，直线，左转曲线，右转曲线
     * @param curveid （ref）如果是曲线，给出数据库中的该曲线段id（暂时版本直线段没有）
     * @param curveradius （ref）如果是曲线，给出数据库中的该曲线段半径（暂时版本直线段没有）
     * @return true 如果找到里程，false没找到
     */
    bool findInterMile(int filteredmileage, bool carriagedirection,  CurveType & curvetype, int & curveid, int &curveradius);

    /****************get 方法********************/
    // 隧道名称、编号及其他
    int getId();
    string getName();
    string getIdStd();
    _int64 getStartPoint();
    _int64 getEndPoint();
    int getIsNew();
    int getLineType();
    bool getIsDoubleLine();
    bool getIsDownlink();

	bool getIsNormalTravel();


    // 线路名称和编号
    string getLinename();
    int getLineid();
    string getLineStartStation();
    string getLineEndStation();

    /**
     * 获得该隧道对应的弯道列表，std::list类型
     * @return list，每个元素都是弯道数据
     */
    list<CurveDataModel> getCurveList();
    list<CurveDataModel>::iterator begin();
    list<CurveDataModel>::iterator end();

private:
    // 隧道名称、编号及其他
    int id;
    string name;
    string idstd;
    _int64 startPoint;
    _int64 endPoint;
    int isNew;
    int lineType;
    bool isDoubleLine;
    bool isDownlink;

	bool isNormalTravel;

    // 线路名称和编号
    string linename;
    int lineid;
    string linestartstation;
    string lineendstation;

    /**
     * 对应该隧道的弯道
     */
    list<CurveDataModel> curves;

    // 应用需求，首次load中设置，后边不用重新遍历curves
    int numLeftCurves;
    int numRightCurves;
    int numStrights;

};

#endif // TUNNELDATAMODEL_H
