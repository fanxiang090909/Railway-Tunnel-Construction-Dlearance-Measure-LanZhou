#ifndef FUSECALCULTAION_H
#define FUSECALCULTAION_H

/*********************************************************************************
**********************************************************************************
                          代码说明：兰州项目计算融合模块
                          版本信息：beta 1.0
                          提交时间：2014/8/10
			              作者：    唐书航
***********************************************************************************
*********************************************************************************/
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>
#include <string>
#include <vector>
#include <math.h>
#include "LzSerialStorageMat.h"
#include "LzSerialStorageSynthesis.h"

#include <QObject>
#include <QDebug>

using namespace std;
using namespace cv;

// @author 范翔改 继承自QObject 可以通过消息槽反馈计算进度
class Fusion : public QObject
{
    Q_OBJECT

signals:
    // @author 范翔改 继承自QObject 可以通过消息槽反馈计算进度
    /**
     * 向LzCalcuQueue转发信号
     * @param tmpfc 当前计算帧号
     * @param ifsuspend 是否暂停
     */
    void statusShow(qint64 tmpfc, bool ifsuspend);

public:
    /**
     * 暂停计算
     * @author 范翔
     */
    void suspend() { ifsuspend = true; }
    explicit Fusion(QObject * parent = 0);
    ~Fusion();


	void init(vector<string> load_file);                 //初始化文件载入文件名向量
	void get_vector();                                   //将17组从机文件放入同一向量vector中
	void rectify_RT();                                   //对三维点向量进行RT矫正

    /**
     * 融合计算主函数
     * @param Item 提取高度时的高度值链表
     * @param out_file 融合三维点结果存储文件名（full file path）
     * @param out_extrac_high 融合三维点的提取高度结果存储文件名（full file path）
     * @param isinterruptfile 是否从原来暂停的文件位置继续开始计算
     * @param start_num 计算起始帧（若isinterruptfile == true ，已换算）
     * @param frame_cunt 计算总帧数（若isinterruptfile == true ，已换算）
     * @return 0 正常计算结束；
     *         1 高度未初始化；
     *         2 从A到P相机没有任何数据（一组都没有）。
     *         3 计算结果文件打开不成功（针对暂停计算后继续计算的情况）
     */
    int fuse(list<int> Item, string out_file, string out_extrac_high, bool isinterruptfile, __int64 start_num, __int64 frame_cunt);

    /**
     * @return 0 正常计算结束，1 高度未初始化
     */
    int extrac_height(list<int> Item, LzSerialStorageSynthesis* extrac_high, __int64 frame_num, double mile_count);
	void init_test();
    LzSerialStorageMat* get_mat(int);                   
    string open_file(int);  
    Mat& get_point(int);
    bool& get_valid(int);                                //返回文件有效性
    void get_vector(Mat&, int cameragroupid, bool carriagedirection);
    bool & get_point_valid(int);

private:
    // @author 范翔，是否终止计算
    bool ifsuspend;
    // @author 范翔，火车行驶时（计划配置时）车厢正反
    bool carriagedirect;

	string A_file,B_file,C_file,D_file;                  //17组从机文件的开启
	string E_file,F_file,G_file,H_file;
	string I_file,J_file,K_file,L_file;
	string M_file,N_file,O_file,P_file;
	string RT_file,out_file;
	Mat point_A,point_B,point_C,point_D;
	Mat point_E,point_F,point_G,point_H;
	Mat point_I,point_J,point_K,point_L;
	Mat point_M,point_N,point_O,point_P;
    bool valid_point_A, valid_point_B, valid_point_C, valid_point_D;
    bool valid_point_E, valid_point_F, valid_point_G, valid_point_H;
    bool valid_point_I, valid_point_J, valid_point_K, valid_point_L;
    bool valid_point_M, valid_point_N, valid_point_O, valid_point_P;
    LzSerialStorageMat *lzMat_A,*lzMat_B,*lzMat_C,*lzMat_D;
    LzSerialStorageMat *lzMat_E,*lzMat_F,*lzMat_G,*lzMat_H;
    LzSerialStorageMat *lzMat_I,*lzMat_J,*lzMat_K,*lzMat_L;
    LzSerialStorageMat *lzMat_M,*lzMat_N,*lzMat_O,*lzMat_P;
    LzSerialStorageMat *lzMat_RT,*lzMat_Out;
    LzSerialStorageSynthesis* extra_high;
    bool valid_A,valid_B,valid_C,valid_D,valid_E,valid_F;
    bool valid_G,valid_H,valid_I,valid_J,valid_K,valid_L;
    bool valid_M,valid_N,valid_O,valid_P,valid_RT;
	Mat RT_point,out_pnts,out_pnts_H;     //out_pnts记录三维点矩阵，out_pnts_H记录提取高度后的三维点矩阵
    bool valid_RT_point;
	Vector<Point3d> fus_vector;                          //用于记录融合后的三维点
	int start_num,cunt_num;                              //用于定义融合的起始帧号及总共融合的帧数

};

#endif // FUSECALCULTAION_H
