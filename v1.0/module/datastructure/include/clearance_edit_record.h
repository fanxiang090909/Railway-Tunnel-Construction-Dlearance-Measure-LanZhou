#ifndef CLEARANCE_EDIT_RECORD_H
#define CLEARANCE_EDIT_RECORD_H

#include <iostream>
#include <stack>
#include "clearance_item.h"
#include "lz_logger.h"

using namespace std;
/**
 * 修改操作记录
 */
struct EditRecord
{
    // 用户名
    int height;
    // 类型 -1（左尺寸值）,1（右尺寸值）
    // 类型 -2（左位置）,2（右位置）
    // 类型 -3（左半径）,3（右半径）
    //int type;
    bool isleft;
    float oldval;
    float newval;
    //_int64 oldpos;
    //_int64 newpos;
    //int oldradius;
    //int newradius;
};

/**
 * 某个限界的编辑操作记录list类声明
 * @author fanxiang
 * @date 2014-6-15
 * @version 1.0.0
 */
class EditRecordStack
{
public:

    EditRecordStack();

    ~EditRecordStack();

    /**
     * 初始化日志输出类
     */
    bool initLogger(string filename, string username);

    /**
     * 是否还有剩余操作未保存
     */
    bool isEmpty();
	
    /**
     * 加入新的修改操作
     * （1）插入到队列尾部
     */
    bool addOneStep(EditRecord item);

    /**
     * 全部保存已经修改的内容
     * （1）写入修正记录日志
     * （2）清空list
     * @param currentframecounter 这些修改所对应的帧
     * @param currentmile 这些修改所对应的里程
     * @param carriagedirection 车厢正反
     */
    bool saveAll(__int64 currentframecounter, float currentmile, bool carriagedirection);

    /**
     * 全部撤销原来操作
     * （1）清空list
     */
    void cancelAll(SectionData &data);

    /**
     * 撤销前一操作，重新把旧值赋给data中
     * @param data 同时作为输入和输出：输入已经修改好的限界
     *                                 输出撤销后的限界
     * @return true 撤销一步成功
     *         false 前面没有更多，不能再撤销了
     */
    bool cancelOneStep(SectionData &data);
	
    LzLogger * getLogger();

private:

    stack<EditRecord> editingStack;

    LzLogger * logger;

    bool hasinitlog;
};

#endif // CLEARANCE_EDIT_RECORD_H
