#ifndef LZCALCULATE_EXTRACTHEIGHT_H
#define LZCALCULATE_EXTRACTHEIGHT_H

#include <datastructure.h>
#include <filemanagers.h>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
///@todo 添加标定文件路径

class LzCalculate_ExtractHeight : public QObject 
{
    Q_OBJECT

public:

    LzCalculate_ExtractHeight();

    ~LzCalculate_ExtractHeight();

    /**
     * 提高度计算初始化
     * @param initHeights 待提取指定高度
     * @param fusefile_no_RT 不加RT计算的融合结果
     * @param Qmdat_file
     * @param Rmdat_file
     * @param syn_rectify_input_file 校正文件
     * @param syn_output_file 输出的提高度文件名
     */
    bool init(list<int> initHeights, string fusefile_no_RT, string Qmdat_file, string Rmdat_file, string syn_rectify_input_file, string syn_output_file, bool userectifyfactor, bool usesafetyfactor);

    int run();

    void Vector2Mat(Vector<Point3d> & input, Mat & output);

    void Mat2Vector(Mat & input, Vector<Point3d> & output);

    /**
     * fuseMat转Vector
     */
    void get_vector(Mat& mat_point, Vector<Point3d> & fus_vector, int cameragroupid, bool carriagedirection);

    /**
     * 加载校正系数配置文件.rectify配置文件
     */
    bool loadRectifyData(string rectifyname);
    
    /**
     * 加校正系数校正提高度结果
     */
    void rectifyHeight(SectionData & data, bool safetyfactor);

    /**
     * 对三维点向量进行RT矫正
     */
    void rectify_RT();

    /**
     * 提取高度
     */
    int extract_height(Vector<Point3d> & fus_vector, SectionData & Data, float & center_height);

signals:
    // @author 范翔改 继承自QObject 可以通过消息槽反馈计算进度
    /**
     * 向LzCalcuQueue转发信号
     * @param tmpfc 当前计算帧号
     * @param ifsuspend 是否暂停
     */
    void statusShow(qint64 tmpfc, bool ifsuspend);

private:

    // 初始化参数
    bool hasinit_can_calc;

    bool hasinit_has_RTfile;

    bool hasinit_syn_rectify_input_file;

    string fusefile_no_RT;

    string Qmdat_file;

    string Rmdat_file;
    
    string syn_rectify_input_file;
    
    string syn_output_file;

    bool usesafetyfactor;
    bool userectifyfactor;

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
    Vector<Point3d> fuse_vector;                          

    // 指定高度
    list<int> Item;

    // @author 范翔，是否终止计算
    bool ifsuspend;
    // @author 范翔，火车行驶时（计划配置时）车厢正反
    bool carriagedirect;

    bool isinterruptfile;

    
    struct RectifyFactor
    {
        float l_a;
        float l_b;
        float l_c;
        float l_d;
        float r_a;
        float r_b;
        float r_c;
        float r_d;
    };

    /**
     * 右边点  左边点 （包含高度信息）
     * 【注意！】左边右边都以标准输出表格的左右为准
     *          即单线下行，与双线非正常方向
     *          面向终点的左边数据点和右边数据点
     */
    std::map<int, RectifyFactor> vals;
};

#endif // LZCALCULATE_EXTRACTHEIGHT_H