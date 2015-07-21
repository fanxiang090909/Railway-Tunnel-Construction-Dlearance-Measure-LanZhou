#ifndef LZCALCULATE_EXTRACTHEIGHT_H
#define LZCALCULATE_EXTRACTHEIGHT_H

#include <datastructure.h>
#include <filemanagers.h>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
///@todo 添加标定文件路径

class LzCalculate_ExtractHeight
{

public:

    /**
     * 提高度计算初始化
     * @param initHeights 待提取指定高度
     * @param fusefile_no_RT 不加RT计算的融合结果
     * @param Qmdat_file
     * @param Rmdat_file
     * @param syn_rectify_input_file 校正文件
     * @param syn_output_file 输出的提高度文件名
     */
    bool init(list<int> initHeights, string fusefile_no_RT, string Qmdat_file, string Rmdat_file, string syn_rectify_input_file, string syn_output_file);

    int run();

    void Vector2Mat(Vector<Point3d> & input, Mat & output);

    void Mat2Vector(Mat & input, Vector<Point3d> & output);

signals:
    // @author 范翔改 继承自QObject 可以通过消息槽反馈计算进度
    /**
     * 向LzCalcuQueue转发信号
     * @param tmpfc 当前计算帧号
     * @param ifsuspend 是否暂停
     */
    void statusShow(qint64 tmpfc, bool ifsuspend);

private:

    /**
     * 对三维点向量进行RT矫正
     */
    void rectify_RT();

    /**
     * 提取高度
     */
    int extract_height(list<int> Item,LzSerialStorageSynthesis* extr_high, __int64 frame_num, double mile_count);
    
    // 初始化参数
    bool hasinit_can_calc;

    bool hasinit_has_RTfile;

    bool hasinit_syn_rectify_input_file;

    string fusefile_no_RT;

    string Qmdat_file;

    string Rmdat_file;
    
    string syn_rectify_input_file;
    
    string syn_output_file;

    // 文件操作类
    // 隧道_日期.fdat
    LzSerialStorageMat *lzMatFuse;
    
    // 隧道_日期_Q.mdat
    LzSerialStorageMat *lzMat_Q;
    // 隧道_日期_R.mdat
    LzSerialStorageMat *lzMat_R;
    
    // 隧道_日期.syn
    LzSerialStorageSynthesis* extra_high;

    // 用于记录融合后的三维点
    Vector<Point3d> fus_vector;                          

    // 指定高度
    list<int> Item;

    // @author 范翔，是否终止计算
    bool ifsuspend;
    // @author 范翔，火车行驶时（计划配置时）车厢正反
    bool carriagedirect;

    bool isinterruptfile;
};

#endif // LZCALCULATE_EXTRACTHEIGHT_H