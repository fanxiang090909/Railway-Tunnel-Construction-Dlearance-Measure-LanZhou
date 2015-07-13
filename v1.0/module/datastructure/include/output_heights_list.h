#ifndef OUTPUT_HEIGHTS_LIST_H
#define OUTPUT_HEIGHTS_LIST_H

#include <QObject>

/**
 * 隧道综合时候指定高度集合配置模型类声明
 * list中
 * @author 范翔
 * @date 2014-1-14
 * @version 1.0.0
 */
class OutputHeightsList : public QObject
{
    Q_OBJECT
public:

    /**
     * singleton单例设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static OutputHeightsList * getOutputHeightsListInstance();

    ~OutputHeightsList();

    /******* 综合、修正、输出高度设置 ***********/
    std::list<int> list();
    std::list<int>::iterator heightsBegin();
    std::list<int>::iterator heightsEnd();
    bool heightsClear();
    bool heightsList_pushback(int val);
    /**
     * 若果修改高度，只需在这里修改高度指定配置文件即可
     */
    void resetHeights(QString filename);
    /**
     * @return 获得当前的指定高度配置文件版本
     */
    double getCurrentHeightsVersion();
    /**
     * 设置当前的指定高度配置文件版本
     * @param version 新高度版本
     */
    void setHeightsVersion(float version);

private:
    // singleton单一设计模式，构造函数私有
    explicit OutputHeightsList(QObject *parent = 0);

    // singleton设计模式，静态私有实例变量
    static OutputHeightsList * outputHeightsListInstance;


    /******* 综合、修正、输出高度设置 ***********/
    /**
     * 待综合、修正、输出的高度
     */
    std::list<int> heights;
    /**
     * 综合高度值集合  版本（预防以后更改）
     */
    float heightsVersion;
};

#endif // OUTPUT_HEIGHTS_LIST_H
