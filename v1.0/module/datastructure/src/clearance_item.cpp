/*
 *	clearrance_item.cpp
 *	兰州隧道限界检测项目
 *
 *	作者: FanX
 *	版本: alpha v1.0
 *	日期：2014.1.14
 */

#include "..\include\clearance_item.h"

#include <exception>

#include "output_heights_list.h"

#include <list>
#include <QDebug>

using namespace std;

SectionData::SectionData()
{
    mileCount = -1;
    frameCounter = -1;
    type = Curve_Straight;
    curveRadius = -1;
    hasinit = false;
    centerHeight = -1;
}

// 复制构造函数
SectionData::SectionData(const SectionData & real)
{
    this->frameCounter = real.frameCounter;
    this->mileCount = real.mileCount;
    this->type = real.type;
    this->curveRadius = real.curveRadius;
    this->vals = real.vals;
    this->hasinit = real.hasinit;
    this->centerHeight = real.centerHeight;
}

// 重载赋值号
SectionData & SectionData::operator=(const SectionData &real)
{
    //if (this == &real)
    //    return *this;

    this->frameCounter = real.frameCounter;
    this->mileCount = real.mileCount;
    this->type = real.type;
    this->curveRadius = real.curveRadius;
	this->vals = real.vals;
	this->hasinit = real.hasinit;
    this->centerHeight = real.centerHeight;
    return *this;
}

// 友元函数不用写“SectionData::”
/*bool operator==(const SectionData &real1, const SectionData &real2)
{
    if (real1.frameCounter == real2.frameCounter && real1.mileCount == real2.mileCount &&
        real1.hasinit == real2.hasinit && real1.vals == real2.vals)
        return true;
    else
        return false;
};*/

/**
 * 左右数据对调，方便限界图形预览（符合人眼感觉）
 */
void SectionData::swapLeftAndRight()
{
    std::map<int,item>::iterator it = vals.begin();
    int tempkey;
    float tmpval;
	while (it != vals.end())
    {
        /*
        vs2012下不行，出错“初始化”: 无法从“std::pair<_Ty1,_Ty2>”转换为“std::pair<_Ty1,_Ty2>”
        std::pair<int,item&> pair = (*it);
        pair.second.left = -1;
        pair.second.right = -1;
        */
        std::pair<int,item> pair = (*it);
        tempkey = pair.first;
        tmpval = vals[tempkey].left;
        vals[tempkey].left = vals[tempkey].right;
        vals[tempkey].right = tmpval;
        it++;
    }
}

/**
 * 插入map值
 */
bool SectionData::updateToMapVals(int height, float val, bool isleft)
{
    if (!hasinit)
        return false;
    try {
        //vals.at(height);      // map::at throws an out-of-range
        if (isleft)
            vals.at(height).left = val;
        else
            vals.at(height).right = val;
        //qDebug() << "insert" << val;
    }
    catch (const std::out_of_range& oor) {
        qDebug() << "Out of Range error: " << oor.what() << '\n';
        return false;
    }
    return true;
}

/**
 * 插入map值，并返回原值
 */
bool SectionData::updateToMapVals(int height, float val, bool isleft, float & oldval)
{
    if (!hasinit)
        return false;
    try {
        //vals.at(height);      // map::at throws an out-of-range
        if (isleft)
        {
            oldval = vals.at(height).left;
            vals.at(height).left = val;
        }
        else
        {
            oldval = vals.at(height).right;
            vals.at(height).right = val;
        }
        //qDebug() << "insert" << val;
    }
    catch (const std::out_of_range& oor) {
        qDebug() << "Out of Range error: " << oor.what() << '\n';
        return false;
    }
    return true;
}

bool SectionData::updateToMapVals(int height, float leftVal, float rightVal)
{
    if (!hasinit)
        return false;
    try {
        //vals.at(height);      // map::at throws an out-of-range
        vals.at(height).left = leftVal;
        vals.at(height).right = rightVal;
        //qDebug() << "insert,left:" << leftVal << ",right:" << rightVal;
    }
    catch (const std::out_of_range& oor) {
        qDebug() << "Out of Range error: height=" << height << oor.what() << '\n';
        return false;
    }
    return true;
}

/**
 * 初始化map（长度已知，从配置文件中读出）
 */
bool SectionData::initMaps()
{
    if (!hasinit) // 如果未被初始化，创建高度
    {
        if (OutputHeightsList::getOutputHeightsListInstance()->getCurrentHeightsVersion() == 0)
        {
            hasinit = false;
            qDebug() << "sectiondata heights init false";
            return false;
        }
        std::list<int>::iterator it = OutputHeightsList::getOutputHeightsListInstance()->heightsBegin();
        while (it != OutputHeightsList::getOutputHeightsListInstance()->heightsEnd())
        {
            item i;
            i.left = -1;
            i.right = -1;
            vals.insert(std::pair<int,item>((*it),i));
            it++;
        }
        qDebug() << "sectiondata heights init ok";
        hasinit = true;
        setCenterHeight(-1);
    }
    else // 如果已被初始化，全部重置-1
        resetMaps();
    return true;
}

/**
 * 重置maps，不删除map，只把maps中的item.left和item.right清零，记录下一帧的
 */
bool SectionData::resetMaps()
{
    std::map<int,item>::iterator it = vals.begin();
    int tempkey;
	while (it != vals.end())
    {
        /*
        vs2012下不行，出错“初始化”: 无法从“std::pair<_Ty1,_Ty2>”转换为“std::pair<_Ty1,_Ty2>”
        std::pair<int,item&> pair = (*it);
        pair.second.left = -1;
        pair.second.right = -1;
        */
        std::pair<int,item> pair = (*it);
        tempkey = pair.first;
        vals[tempkey].left = -1;
        vals[tempkey].right = -1;
        it++;
    }
    setCenterHeight(-1);
    return true;
}

/**
 * 得到maps
 */
std::map<int, item>& SectionData::getMaps()
{
    return vals;
}

/**
 * 显示输出map的值
 */
bool SectionData::showMaps()
{
    std::map<int,item>::iterator it = vals.begin();
    while (it != vals.end())
    {
        std::pair<int,item> pair = (*it);
       // qDebug() << "height:" << pair.first << ", lval:" << pair.second.left << ", rval:" << pair.second.right;
        it++;
    }
    return true;
}

/**
 * 更新限界值
 * @param clearancedata 当前限界ClearanceData数据指针
 */
bool SectionData::updateToClearanceData(ClearanceData & clearancedata)
{
    //clearancedata.showMaps();
    // 如果不添加这个判断，在下面if (clearancevals[tempkey].leftval < 0) 时就加到Map中了
    if (clearancedata.getMaps().size() == 0)
        return false;
    std::map<int,ClearanceItem> & clearancevals = clearancedata.getMaps();
    std::map<int,item>::iterator it = vals.begin();
    int tempkey;
    while (it != vals.end())
    {
        std::pair<int,item> pair = (*it);
        tempkey = pair.first;
        // qDebug() << "height:" << tempkey << ", val:" << pair.second.left << pair.second.right;
        // qDebug() << "\t clearanceval:" << clearancevals[tempkey].leftval << clearancevals[tempkey].rightval;
        // 更新左右限界值
        if (clearancevals[tempkey].leftval <= 0 || (pair.second.left - 30 > 0 && (pair.second.left - 30) < clearancevals[tempkey].leftval))
        {
            //if (pair.second.left < 0)
            //    pair.second.left = 0;
            clearancevals[tempkey].leftval = pair.second.left - 30; // 人为缩减30
            clearancevals[tempkey].leftpos = mileCount;
			clearancevals[tempkey].leftradius = curveRadius;
        }
        if (clearancevals[tempkey].rightval <= 0 || (pair.second.right - 30 > 0 && pair.second.right - 30 < clearancevals[tempkey].rightval))
        {
            //if (pair.second.right < 0)
            //    pair.second.right = 0;
            clearancevals[tempkey].rightval = pair.second.right - 30; // 人为缩减30
            clearancevals[tempkey].rightpos = mileCount;
            clearancevals[tempkey].rightradius = curveRadius;
        }

        // 最小曲线半径
        //if (clearancevals.getMinRadius() < 0 || clearancevals.getMinRadius() > curveRadius)
        //    clearancevals.setMinRadius(curveRadius);
        it++;
    }
    if (clearancedata.getMinCenterHeight() <= 0 || (centerHeight > 0 && centerHeight < clearancedata.getMinCenterHeight()))
    {
        clearancedata.updateToMinCenterHeight(centerHeight, mileCount);
    }

    return true;
}

// geter & setter
void SectionData::setMile(double newmile) { this->mileCount = newmile; }
double SectionData::getMile() { return mileCount; }
void SectionData::setRadius(float newradius) { this->curveRadius = newradius; }
float SectionData::getRadius() { return curveRadius; }
void SectionData::setType(CurveType newtype) { this->type = newtype; }
CurveType SectionData::getType() { return type; }
void SectionData::setCenterHeight(float newcenterheight) { this->centerHeight = newcenterheight; }
float SectionData::getCenterHeight() { return centerHeight; }

ClearanceData::ClearanceData()
{
    tunnelid = -1;
    hasinit = false;
    this->clearanceType = Unknown_Smallest;

    minCenterHeight = -1;
    minCenterHeightPos = -1;
    minCenterHeightTunnelID = -1;
}

// 复制构造函数
ClearanceData::ClearanceData(const ClearanceData & real)
{
    this->tunnelid = real.tunnelid;
    this->vals = real.vals;
    this->hasinit = real.hasinit;
    this->clearanceType = real.clearanceType;

    this->minRadius = real.minRadius;
    this->minCenterHeight = real.minCenterHeight;
    this->minCenterHeightPos = real.minCenterHeightPos;
    this->minCenterHeightTunnelID = real.minCenterHeightTunnelID;
}

// 重载赋值号
ClearanceData & ClearanceData::operator=(const ClearanceData &real)
{
    //if (this == &real)
    //    return *this;
    this->tunnelid = real.tunnelid;
    this->vals = real.vals;
    this->hasinit = real.hasinit;
    this->clearanceType = real.clearanceType;

    this->minRadius = real.minRadius;
    this->minCenterHeight = real.minCenterHeight;
    this->minCenterHeightPos = real.minCenterHeightPos;
    this->minCenterHeightTunnelID = real.minCenterHeightTunnelID;
    return *this;
}

/**
 * 左右数据对调，方便限界图形预览（符合人眼感觉）
 */
void ClearanceData::swapLeftAndRight()
{
    std::map<int,ClearanceItem>::iterator it = vals.begin();
    int tempkey;
    float tmpvalf;
    int tmpvali;
	while (it != vals.end())
    {
        /*
        vs2012下不行，出错“初始化”: 无法从“std::pair<_Ty1,_Ty2>”转换为“std::pair<_Ty1,_Ty2>”
        std::pair<int,item&> pair = (*it);
        pair.second.left = -1;
        pair.second.right = -1;
        */
        std::pair<int,ClearanceItem> pair = (*it);
        tempkey = pair.first;
        tmpvalf = vals[tempkey].leftval;
        vals[tempkey].leftval = vals[tempkey].rightval;
        vals[tempkey].rightval = tmpvalf;

        tmpvali = vals[tempkey].leftradius;
        vals[tempkey].leftradius = vals[tempkey].rightradius;
        vals[tempkey].rightradius = tmpvali;

        tmpvalf = vals[tempkey].leftpos;
        vals[tempkey].leftpos = vals[tempkey].rightpos;
        vals[tempkey].rightpos = tmpvalf;

        tmpvali = vals[tempkey].lefttunnelid;
        vals[tempkey].lefttunnelid = vals[tempkey].righttunnelid;
        vals[tempkey].righttunnelid = tmpvali;

        it++;
    }
}

/**
 * 插入map值
 */
bool ClearanceData::updateToMapVals(int height, float val, double pos, int ttunnelid, float radius, bool isleft)
{
    if (!hasinit)
        return false;
    try {
        //vals.at(height);      // map::at throws an out-of-range
        if (isleft)
		{
            vals.at(height).leftval = val;
            vals.at(height).leftpos = pos;
            vals.at(height).lefttunnelid = ttunnelid;
            vals.at(height).leftradius = radius;
        }
        else
        {
            vals.at(height).rightval = val;
            vals.at(height).rightpos = pos;
            vals.at(height).righttunnelid = ttunnelid;
            vals.at(height).rightradius = radius;
        }
		//qDebug() << "insert" << val;
    }
    catch (const std::out_of_range& oor) {
        qDebug() << "Out of Range error: " << oor.what() << '\n';
        return false;
    }
    return true;
}

bool ClearanceData::updateToMapVals(int height, float leftVal, float rightVal, double leftpos, double rightpos, 
						int lefttunnelid, int righttunnelid, float leftradius, float rightradius)
{
    if (!hasinit)
        return false;
    try {
        //vals.at(height);      // map::at throws an out-of-range
        vals.at(height).leftval = leftVal;
        vals.at(height).rightval = rightVal;
        vals.at(height).leftpos = leftpos;
        vals.at(height).rightpos = rightpos;
        vals.at(height).lefttunnelid = lefttunnelid;
        vals.at(height).righttunnelid = righttunnelid;
        vals.at(height).leftradius = leftradius;
        vals.at(height).rightradius = rightradius;
        //qDebug() << "insert,left:" << leftVal << ",right:" << rightVal;
    }
    catch (const std::out_of_range& oor) {
        qDebug() << "Out of Range error: height=" << height << oor.what() << '\n';
        return false;
    }
    return true;
}

/**
 * 初始化map（长度已知，从配置文件中读出）
 */
bool ClearanceData::initMaps()
{
    if (!hasinit) // 如果未被初始化过，创建高度，并初始化
    {
        if (OutputHeightsList::getOutputHeightsListInstance()->getCurrentHeightsVersion() == 0)
        {
            hasinit = false;
            qDebug() << "clearancedata heights init false";
            return false;
        }
        std::list<int>::iterator it = OutputHeightsList::getOutputHeightsListInstance()->heightsBegin();
        while (it != OutputHeightsList::getOutputHeightsListInstance()->heightsEnd())
        {
            ClearanceItem i;
            i.leftval = -1;
            i.rightval = -1;
            i.leftpos = -1;
            i.rightpos = -1;
            i.lefttunnelid = -1;
            i.righttunnelid = -1;
            i.leftradius = -1;
            i.rightradius = -1;
            vals.insert(std::pair<int,ClearanceItem>((*it),i));
            it++;
        }
        setMinRadius(-1);
        setMinCenterHeight(-1);
        setMinCenterHeightPos(-1);
        setMinCenterHeightTunnelID(-1);
        setTunnelID(-1);

        qDebug() << "clearancedata heights init ok";
        hasinit = true;
    }
    else // 如果已被初始化过，全部赋值-1
    {
        resetMaps();
        setMinCenterHeight(-1);
        setMinCenterHeightPos(-1);
        setMinCenterHeightTunnelID(-1);
        setTunnelID(-1);
    }
    return true;
}

/**
 * 重置maps，不删除map，只把maps中的item.left和item.right清零，记录下一帧的
 */
bool ClearanceData::resetMaps()
{
    std::map<int,ClearanceItem>::iterator it = vals.begin();
    int tempkey;
    while (it != vals.end())
    {
        /*
        vs2012下不行，出错“初始化”: 无法从“std::pair<_Ty1,_Ty2>”转换为“std::pair<_Ty1,_Ty2>”
        std::pair<int,ClearanceItem&> pair = (*it);
        pair.second.left = -1;
        pair.second.right = -1;
        */
        std::pair<int,ClearanceItem> pair = (*it);
        tempkey = pair.first;
        vals[tempkey].leftval = -1;
        vals[tempkey].rightval = -1;
        vals[tempkey].leftpos = -1;
        vals[tempkey].rightpos = -1;
        vals[tempkey].lefttunnelid = -1;
        vals[tempkey].righttunnelid = -1;
        vals[tempkey].leftradius = -1;
        vals[tempkey].rightradius = -1;
        it++;
    }
    setMinRadius(-1);
    setMinCenterHeight(-1);
    setMinCenterHeightPos(-1);
    setMinCenterHeightTunnelID(-1);
    setTunnelID(-1);

    return true;
}

/**
 * 得到maps
 */
std::map<int, ClearanceItem>& ClearanceData::getMaps()
{
    return vals;
}

/**
 * 显示输出map的值
 */
bool ClearanceData::showMaps()
{
    qDebug() << "******************ClearanceData Maps******************";
    qDebug() << "*******minHeight:" << minCenterHeight << ", ********minHeightPos:" << minCenterHeightPos << " ******"; 
    std::map<int,ClearanceItem>::iterator it = vals.begin();
    while (it != vals.end())
    {
        std::pair<int,ClearanceItem> pair = (*it);
            qDebug() << "height:" << pair.first << ", lval:" << pair.second.leftval << ", lpos:" << pair.second.leftpos << ", ltunnelid:" << pair.second.lefttunnelid
                 << ", rval:" << pair.second.rightval << ", rpos:" << pair.second.rightpos << ", rtunnelid:" << pair.second.righttunnelid;
        it++;
    }
    return true;
}

/**
 * 更新线路中心线上方最低净高
 */
void ClearanceData::updateToMinCenterHeight(float val, double pos)
{
    minCenterHeight = val;
    minCenterHeightPos = pos;
}

void ClearanceData::updateToMinCenterHeight(float val, double pos, int tunnelid)
{
    minCenterHeight = val;
    minCenterHeightPos = pos;
    minCenterHeightTunnelID = tunnelid;
}

/**
 * 设置最小半径
 */ 
void ClearanceData::setMinRadius(int newradius)
{
    this->minRadius = newradius;
}

/**
 *  得到最小半径
 */
int ClearanceData::getMinRadius()
{
    return this->minRadius;
}


/**
 * 线路中心线上方最低净高
 */
float ClearanceData::getMinCenterHeight()
{
    return minCenterHeight;
}

/**
 * 线路中心线上方最低净高
 */
void ClearanceData::setMinCenterHeight(float newval)
{
    minCenterHeight = newval;
}

/**
 * 线路中心线上方最低净高
 */
double ClearanceData::getMinCenterHeightPos()
{
    return minCenterHeightPos;
}

/**
 * 线路中心线上方最低净高所在位置
 */
void ClearanceData::setMinCenterHeightPos(double newpos)
{
    minCenterHeightPos = newpos;
}

/**
 * 线路中心线上方最低净高所在隧道ID
 */
int ClearanceData::getMinCenterHeightTunnelID()
{
    return minCenterHeightTunnelID;
}

/**
 * 线路中心线上方最低净高所在隧道ID
 */
void ClearanceData::setMinCenterHeightTunnelID(int newtunnelid)
{
    minCenterHeightTunnelID = newtunnelid;
}

int ClearanceData::getTunnelID()
{
    return tunnelid;
}

void ClearanceData::setTunnelID(int newtunnelid)
{
    this->tunnelid = newtunnelid;
}

/**
 * 更新限界值
 * @param clearancedata 当前限界ClearanceData数据指针
 */
bool ClearanceData::updateToClearanceData(ClearanceData & clearancedata)
{
    // clearancedata.showMaps();
    std::map<int,ClearanceItem> & clearancevals = clearancedata.getMaps();
    std::map<int,ClearanceItem>::iterator it = vals.begin();
    int tempkey;
    while (it != vals.end())
    {
        std::pair<int,ClearanceItem> pair = (*it);
        tempkey = pair.first;
        qDebug() << "height:" << tempkey << ", val:" << pair.second.leftval << pair.second.rightval;
        // qDebug() << "\t clearanceval:" << clearancevals[tempkey].left << clearancevals[tempkey].right;
        // 更新左右限界值
        if (clearancevals[tempkey].leftval < 0 || (pair.second.leftval >= 0 && pair.second.leftval < clearancevals[tempkey].leftval))
        {
            clearancevals[tempkey].leftval = pair.second.leftval;
            clearancevals[tempkey].leftpos = pair.second.leftpos;
            clearancevals[tempkey].leftradius = pair.second.leftradius;
            clearancevals[tempkey].lefttunnelid = tunnelid;
        }
        if (clearancevals[tempkey].rightval < 0 || (pair.second.rightval >= 0 && pair.second.rightval < clearancevals[tempkey].rightval))
        {
            clearancevals[tempkey].rightval = pair.second.rightval;
            clearancevals[tempkey].rightpos = pair.second.rightpos;
            clearancevals[tempkey].rightradius = pair.second.rightradius;
            clearancevals[tempkey].righttunnelid = tunnelid;
        }
        it++;
    }
    if (clearancedata.getMinCenterHeight() < 0 || (minCenterHeight >= 0 && minCenterHeight < clearancedata.getMinCenterHeight()) )
    {
        clearancedata.updateToMinCenterHeight(minCenterHeight, minCenterHeightPos, minCenterHeightTunnelID);
    }

    return true;
}

const ClearanceType ClearanceData::getClearanceType() const { return clearanceType; }
void ClearanceData::setType(ClearanceType curvetype) { this->clearanceType = curvetype; }
