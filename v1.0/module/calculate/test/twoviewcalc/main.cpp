/*/////////////////////////////////////////////////////////////////////////////////
                   代码说明：兰州项目双目计算模块
	               版本信息：beta 1.0   
	               提交时间：2014/8/6
	               作者：    唐书航
				   存在问题：（1）双目计算结果的存储尚有问题。
				             （2）对三维点的Rt矫正须实测调试以验证其精度
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

	std::cout << "单隧道双目计算" <<std::endl;
	int start_num_L = 0;         //指明左相机起始帧号
	int start_num_R = 0;         //指明右相机起始帧号
	int cunt_num = 100;          //指明一共多少帧
	int start_num_store = 0;     //指明存储的起始帧号
   	LzSerialStorageAcqui * lzacqui_L = new LzSerialStorageAcqui();                               //左相机流式存储类
	LzSerialStorageAcqui * lzacqui_R = new LzSerialStorageAcqui();                               //右相机流式存储类
	LzSerialStorageMat   * lzstore_mat = new LzSerialStorageMat();
	LzCalculation Lz_c;
	string L_img_file = "C:\\Users\\Administrator\\Desktop\\LZ_project\\serialfile_1.dat";       //左相机文件
	string R_img_file = "C:\\Users\\Administrator\\Desktop\\LZ_project\\serialfile_2.dat";       //右相机文件
	string output_file = "C:\\Users\\Administrator\\Desktop\\LZ_project\\output_file.dat";
    try {

        lzacqui_L->openFile(L_img_file.c_str());
		lzacqui_R->openFile(R_img_file.c_str());  
		Lz_c.loadcalib("C:\\Users\\Administrator\\Desktop\\标定数据\\result.xml");               //载入标定文件
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
			lzstore_mat->writeMat(Lz_c.rect_pnts3d,Info,true);                                    //写入Mat还有问题，须改！
			Info.key++;
		}
        lzacqui_L->closeFile();
		lzacqui_R->closeFile();
		lzstore_mat->closeFile();
    }
    catch (LzException & ex)
    {
		//抛出异常
	}
    delete lzacqui_L;
	delete lzacqui_R;
	delete lzstore_mat;
	system("pause");
    return 0;
}
