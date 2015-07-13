#include "tunneldatamodel.h"

#include "daotunnel.h"
#include "daocurve.h"
#include <QDebug>
#include <QTextCodec>

/**
 * 对应数据库中的tunnel模型类实现，可调用的数据访问接口
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-11-16
 */

/**
 * 构造函数，指定tunnelid
 */
TunnelDataModel::TunnelDataModel(int tunnelid)
{
    this->id = tunnelid;
    this->curves = list<CurveDataModel>();
    this->numLeftCurves = 0;
    this->numRightCurves = 0;
    this->numStrights = 0;
}

/**
 * 析构函数
 */
TunnelDataModel::~TunnelDataModel() {}

/**
 * !!!必要步骤
 * 已知tunnelid的情况下从数据库中加载相关数据
 */
bool TunnelDataModel::loadTunnelData()
{
    // 【注意！】QString转到stdstring中文必须
    // 如果code为0，表示在运行的机器上没有装gb18030字符集。不过一般的中文windows系统中都安装了这一字符集
    // 【
    //   QTextCodec *code = QTextCodec::codecForName("gb2312");
    //   if (code) idstd = code->fromUnicode(temp).data();
    // 】
    //QTextCodec *code = QTextCodec::codecForName("gb2312");
    QTextCodec *code = QTextCodec::codecForName("GBK");
    // QTextCodec *code=QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    /*******************基本的隧道数据************************/
    QSqlQueryModel * model = NULL;
    model = TunnelDAO::getTunnelDAOInstance()->getExactTunnel(this->id);

    if (model->rowCount() == 0)
        return false;

    QModelIndex index;
    QString temp;
    for (int i = 0; i < model->columnCount(); i++)
    {
        index = model->index(0, i);
        temp = model->data(index).toString();
        switch(i)
        {
            case Tunnel_ID: break;
            case Tunnel_name_std:
                {
                    if (code) name = code->fromUnicode(temp).data();
                    break;
                }
            case Tunnel_ID_std:
                {
                    if (code) idstd = code->fromUnicode(temp).data();
                    break;
                }
            case Tunnel_start_point_std:startPoint = temp.toLongLong(); break;
            case Tunnel_end_point_std: endPoint = temp.toLongLong();break;
            case Tunnel_is_new_std: isNew = temp.toInt();break;
            case Tunnel_line_type_std: lineType = temp.toInt();break;
            case Tunnel_is_double_line:
                {
                    if (temp.toInt() == 0)
                        isDoubleLine = false;
                    else
                        isDoubleLine = true;
                    break;
                }
            case Tunnel_is_downlink:
                {
                    if (temp.toInt() == 0)
                        isDoubleLine = false;
                    else
                        isDoubleLine = true;
                    break;
                }
            case Tunnel_line_ID: lineid = temp.toInt();break;
            case Tunnel_is_valid + 1:
                {
                    if (code) linename = code->fromUnicode(temp).data();
                    break;
                }
            case Tunnel_is_valid + 2:
                {
                    if (code) linestartstation = code->fromUnicode(temp).data();
                    break;
                }
            case Tunnel_is_valid + 3:
                {
                    if (code) lineendstation = code->fromUnicode(temp).data();
                    break;
                }
            default: break;
        }
    }

    /*******************隧道对应的弯道数据************************/
    QSqlQueryModel * rmodel = NULL;
    rmodel = CurveDAO::getCurveDAOInstance()->getExactTunnelCurves(this->id);
    for (int i = 0; i < rmodel->rowCount(); i++)
    {
        CurveDataModel c;
        for (int j = 0; j < rmodel->columnCount(); j++)
        {
            index = rmodel->index(i, j);
            temp = rmodel->data(index).toString();
            switch (j)
            {
                case 0: c.curveid = temp.toLongLong();break;
                case 1: c.radius = temp.toInt();break;
                case 2:
                    {
                        if (temp.toInt() == 0)
                            c.isLeft = false;
                        else
                            c.isLeft = true;
                        break;
                    }
                case 3:
                    {
                        if (temp.toInt() == 0)
                            c.isStraight = false;
                        else
                            c.isStraight = true;
                        break;
                    }
                case 4: c.pointZhiHuan = temp.toLongLong();break;
                case 5: c.pointHuanZhi = temp.toLongLong();break;
                default: break;
            }
        }
        if (c.isLeft == true)
            numLeftCurves++;
        else
            numRightCurves++;
        curves.push_back(c);
    }
    return true;
}

/**
 * 比较该数据域DataHead中的数据是否有不同
 * 即比较采集时的隧道基本信息与数据库中的信息是否不同
 * @param head 文件头引用
 * @return true 相同 else 不同，需要警告！！
 */
bool TunnelDataModel::compare(DataHead & head)
{
    if (this->id == head.tunnel_id && this->name.compare(head.tunnel_name) == 0)
        return true;
    else
        return false;
}

/**
 * 左转曲线段
 */
int TunnelDataModel::getNumberOfLeftCurves()
{
    return numLeftCurves;
}

/**
 * 右转曲线段
 */
int TunnelDataModel::getNumberOfRightCurves()
{
    return numRightCurves;
}

/**
 * 直线段
 */
int TunnelDataModel::getNumberOfStrights()
{
    return numStrights;
}

/**
 * 得到该里程在此隧道的曲线，及曲线curveid，及曲线半径
 * @param mileage 【折算后，非采集时】注明内部里程，隧道口为0
 * @param curvetype （ref）找到了该里程，在曲线类型上，曲线类型，直线，左转曲线，右转曲线
 * @param curveid （ref）如果是曲线，给出数据库中的该曲线段id（暂时版本直线段没有）
 * @param curveradius （ref）如果是曲线，给出数据库中的该曲线段半径（暂时版本直线段没有）
 * @return true 如果找到里程，false没找到
 */
bool TunnelDataModel::findInterMile(int filteredmileage, CurveType & curvetype, int & curveid, int &curveradius)
{
    list<CurveDataModel>::iterator it = begin();
    while (it != end())
    {
        if ((*it).pointZhiHuan < filteredmileage && filteredmileage < (*it).pointHuanZhi)
        {
            if ((*it).isLeft)
            {
                curvetype = CurveType::Curve_Left;	// 左转曲线，返回curvetype和curveid
                curveid = (*it).curveid;
                curveradius = (*it).radius;
                return true;
			}
            else
            {
                curvetype = CurveType::Curve_Right;	// 右转曲线，返回curvetype和curveid
                curveid = (*it).curveid;
                curveradius = (*it).radius;
                return true;
            }
        }
        it++;
    }
    if (this->startPoint <= filteredmileage && filteredmileage <= this->endPoint)
    {
        curvetype = CurveType::Curve_Straight;	// 直线段，返回curvetype和curveid
        curveid = -1;
        curveradius = -1;
        return true;
    }
    else
        return false;
}

/****************get 方法********************/
// 隧道名称、编号及其他
int TunnelDataModel::getId() { return id; }
string TunnelDataModel::getName() { return name; }
string TunnelDataModel::getIdStd() { return idstd; }
_int64 TunnelDataModel::getStartPoint() { return startPoint; }
_int64 TunnelDataModel::getEndPoint() { return endPoint; }
int TunnelDataModel::getIsNew() { return isNew;}
int TunnelDataModel::getLineType() { return lineType; }
bool TunnelDataModel::getIsDoubleLine() { return isDoubleLine; }
bool TunnelDataModel::getIsDownlink() { return isDownlink; }

// 线路名称和编号
string TunnelDataModel::getLinename() { return linename; }
int TunnelDataModel::getLineid() { return lineid; }
string TunnelDataModel::getLineStartStation() { return linestartstation; }
string TunnelDataModel::getLineEndStation() { return lineendstation; }

// 对应该隧道的弯道
list<CurveDataModel> TunnelDataModel::getCurveList() { return curves; }
list<CurveDataModel>::iterator TunnelDataModel::begin() { return curves.begin(); }
list<CurveDataModel>::iterator TunnelDataModel::end() { return curves.end(); }

