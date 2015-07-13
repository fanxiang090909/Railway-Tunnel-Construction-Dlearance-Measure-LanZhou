#include "output_heights_list.h"

/**
 * 隧道综合时候指定高度集合配置模型类实现
 * list中
 * @author 范翔
 * @date 2014-1-14
 * @version 1.0.0
 */
// singleton设计模式，静态私有实例变量
OutputHeightsList * OutputHeightsList::outputHeightsListInstance = NULL;

OutputHeightsList::OutputHeightsList(QObject *parent) :
    QObject(parent)
{
    this->heightsVersion = 0;
    //this->heights = NULL;
}

OutputHeightsList::~OutputHeightsList()
{
}

OutputHeightsList * OutputHeightsList::getOutputHeightsListInstance()
{
    if (outputHeightsListInstance == NULL)
    {
        outputHeightsListInstance = new OutputHeightsList();
    }
    return outputHeightsListInstance;
}

/******* 综合、修正、输出高度设置 ***********/
std::list<int> OutputHeightsList::list() { return heights; }
std::list<int>::iterator OutputHeightsList::heightsBegin() {return heights.begin();}
std::list<int>::iterator OutputHeightsList::heightsEnd() {return heights.end();}
bool OutputHeightsList::heightsClear() {heights.clear(); return true;}
bool OutputHeightsList::heightsList_pushback(int val) {heights.push_back(val); return true;}

/**
 * 若果修改高度，只需在这里修改高度指定配置文件即可
 */
void OutputHeightsList::resetHeights(QString filename)
{
    heights.clear();
    heights.push_back(150);
    heights.push_back(200);
    heights.push_back(300);
    heights.push_back(350);
    heights.push_back(500);
    heights.push_back(600);
    heights.push_back(800);
    heights.push_back(1000);
    heights.push_back(1100);
    heights.push_back(1210);
    heights.push_back(1250);
    heights.push_back(1600);
    heights.push_back(2000);
    heights.push_back(2400);
    heights.push_back(2800);
    heights.push_back(3000);
    heights.push_back(3200);
    heights.push_back(3600);
    heights.push_back(3800);
    heights.push_back(4000);
    heights.push_back(4200);
    heights.push_back(4400);
    heights.push_back(4500);
    heights.push_back(4600);
    heights.push_back(4800);
    heights.push_back(5000);
    heights.push_back(5200);
    heights.push_back(5400);
    heights.push_back(5500);
    heights.push_back(5600);
    heights.push_back(5800);
    heights.push_back(6000);
    heights.push_back(6200);
    heights.push_back(6400);
    heights.push_back(6600);
    heights.push_back(7000);
    heights.push_back(7500);
    heights.push_back(8000);
}

/**
 * 获得当前的指定高度配置文件版本
 */
double OutputHeightsList::getCurrentHeightsVersion() {return heightsVersion;}
void OutputHeightsList::setHeightsVersion(float version) {heightsVersion = version;}

