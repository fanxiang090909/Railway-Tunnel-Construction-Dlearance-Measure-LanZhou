#ifndef FUSECALCULTAION_H
#define FUSECALCULTAION_H

/*********************************************************************************
**********************************************************************************
                          ����˵����������Ŀ�����ں�ģ��
                          �汾��Ϣ��beta 1.0
                          �ύʱ�䣺2014/8/10
			              ���ߣ�    ���麽
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

// @author ����� �̳���QObject ����ͨ����Ϣ�۷����������
class Fusion : public QObject
{
    Q_OBJECT

signals:
    // @author ����� �̳���QObject ����ͨ����Ϣ�۷����������
    /**
     * ��LzCalcuQueueת���ź�
     * @param tmpfc ��ǰ����֡��
     * @param ifsuspend �Ƿ���ͣ
     */
    void statusShow(qint64 tmpfc, bool ifsuspend);

public:
    /**
     * ��ͣ����
     * @author ����
     */
    void suspend() { ifsuspend = true; }
    explicit Fusion(QObject * parent = 0);
    ~Fusion();


	void init(vector<string> load_file);                 //��ʼ���ļ������ļ�������
	void get_vector();                                   //��17��ӻ��ļ�����ͬһ����vector��
	void rectify_RT();                                   //����ά����������RT����

    /**
     * �ںϼ���������
     * @param Item ��ȡ�߶�ʱ�ĸ߶�ֵ����
     * @param out_file �ں���ά�����洢�ļ�����full file path��
     * @param out_extrac_high �ں���ά�����ȡ�߶Ƚ���洢�ļ�����full file path��
     * @param isinterruptfile �Ƿ��ԭ����ͣ���ļ�λ�ü�����ʼ����
     * @param start_num ������ʼ֡����isinterruptfile == true ���ѻ��㣩
     * @param frame_cunt ������֡������isinterruptfile == true ���ѻ��㣩
     * @return 0 �������������
     *         1 �߶�δ��ʼ����
     *         2 ��A��P���û���κ����ݣ�һ�鶼û�У���
     *         3 �������ļ��򿪲��ɹ��������ͣ������������������
     */
    int fuse(list<int> Item, string out_file, string out_extrac_high, bool isinterruptfile, __int64 start_num, __int64 frame_cunt);

    /**
     * @return 0 �������������1 �߶�δ��ʼ��
     */
    int extrac_height(list<int> Item, LzSerialStorageSynthesis* extrac_high, __int64 frame_num, double mile_count);
	void init_test();
    LzSerialStorageMat* get_mat(int);                   
    string open_file(int);  
    Mat& get_point(int);
    bool& get_valid(int);                                //�����ļ���Ч��
    void get_vector(Mat&, int cameragroupid, bool carriagedirection);
    bool & get_point_valid(int);

private:
    // @author ���裬�Ƿ���ֹ����
    bool ifsuspend;
    // @author ���裬����ʻʱ���ƻ�����ʱ����������
    bool carriagedirect;

	string A_file,B_file,C_file,D_file;                  //17��ӻ��ļ��Ŀ���
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
	Mat RT_point,out_pnts,out_pnts_H;     //out_pnts��¼��ά�����out_pnts_H��¼��ȡ�߶Ⱥ����ά�����
    bool valid_RT_point;
	Vector<Point3d> fus_vector;                          //���ڼ�¼�ںϺ����ά��
	int start_num,cunt_num;                              //���ڶ����ںϵ���ʼ֡�ż��ܹ��ںϵ�֡��

};

#endif // FUSECALCULTAION_H
