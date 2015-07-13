/*/////////////////////////////////////////////////////////////////////////////////
                   ����˵����������Ŀ˫Ŀ����ģ��
	               �汾��Ϣ��beta 1.0   
	               �ύʱ�䣺2014/8/6
	               ���ߣ�    ���麽
				   �������⣺��1��˫Ŀ�������Ĵ洢�������⡣
				             ��2������ά���Rt������ʵ���������֤�侫��
///////////////////////////////////////////////////////////////////////////////*/

#include <iostream>
#include "LZ_Calculation.h"
#include "headdefine.h"
#include "LzException.h"
#include "LzSerialStorageMat.h"
#include "LzSerialStorageAcqui.h"
#include <opencv2\core\core.hpp>
#include <sstream>
using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{

	std::cout << "�����˫Ŀ����" <<std::endl;
	int start_num_L = 0;         //ָ���������ʼ֡��
	int start_num_R = 0;         //ָ���������ʼ֡��
	int cunt_num = 100;          //ָ��һ������֡
	int start_num_store = 0;     //ָ���洢����ʼ֡��
   	LzSerialStorageAcqui * lzacqui_L = new LzSerialStorageAcqui();                               //�������ʽ�洢��
	LzSerialStorageAcqui * lzacqui_R = new LzSerialStorageAcqui();                               //�������ʽ�洢��
	LzSerialStorageMat   * lzstore_mat = new LzSerialStorageMat();
	LzCalculation Lz_c;
	string L_img_file = "C:\\Users\\Administrator\\Desktop\\LZ_project\\serialfile_1.dat";       //������ļ�
	string R_img_file = "C:\\Users\\Administrator\\Desktop\\LZ_project\\serialfile_2.dat";       //������ļ�
	string output_file = "C:\\Users\\Administrator\\Desktop\\LZ_project\\output_file.dat";
    try {

        lzacqui_L->openFile(L_img_file.c_str());
		lzacqui_R->openFile(R_img_file.c_str());  
		Lz_c.loadcalib("C:\\Users\\Administrator\\Desktop\\�궨����\\result.xml");               //����궨�ļ�
     	std::vector<BLOCK_KEY> key_L = lzacqui_L->readKeys();
		std::vector<BLOCK_KEY> key_R = lzacqui_R->readKeys();
		Mat L_img,R_img;
		BlockInfo Info;
		Info.key = start_num_store;
		lzacqui_L->retrieveBlock(start_num_L);
		lzacqui_R->retrieveBlock(start_num_R);
		for(int i = 0; i<cunt_num; i++)
		{
			lzacqui_L->readFrame(L_img);
			lzacqui_R->readFrame(R_img);	
			Lz_c.loadImgMat(L_img,R_img);
			Lz_c.calc();
			lzstore_mat->writeMat(Lz_c.rect_pnts3d,Info,true);                                    //д��Mat�������⣬��ģ�
			Info.key++;
		}
        lzacqui_L->closeFile();
		lzacqui_R->closeFile();
		lzstore_mat->closeFile();
    }
    catch (LzException & ex)
    {
		//�׳��쳣
	}
    delete lzacqui_L;
	delete lzacqui_R;
	delete lzstore_mat;
	system("pause");
    return 0;
}
