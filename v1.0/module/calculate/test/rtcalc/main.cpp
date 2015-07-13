/*********************************************************************************
**********************************************************************************
                          代码说明：兰州项目车厢RT矩阵计算模块
                          版本信息：beta 1.0
                          提交时间：2014/8/10
			              作者：    唐书航
						  存在问题：需后期测试以保证其鲁棒性
***********************************************************************************
*********************************************************************************/
#include "R_T_Calc.h"
#include "icpPointToPoint.h"
#include "headdefine.h"
#include "Lz_Exception.h"
#include "LzSerialStorageMat.h"
#include "LzSerialStorageAcqui.h"
#include <iostream>
using namespace std;

void Comput_RT(Mat pnts3D_1, Mat pnts3D_2, Vector<Point3f> calibed_pnts3d, Mat &RR, Mat &TT)
{
	int32_t dim = 3;
	int32_t num = pnts3D_1.cols+pnts3D_2.cols;


    Mat Test_Rx = (Mat_<double>(3,3)<<1,0,0,0,0.866,-0.5,0,0.5,0.866);
	Mat Test_Ry = (Mat_<double>(3,3)<<0.717,0,-0.717,0,1,0,0.717,0,0.717);
	Mat Test_Rz = (Mat_<double>(3,3)<<0.5,-0.866,0,0.866,0.5,0,0,0,1);
	Mat T = (Mat_<double>(3,1)<<1,2,3);
	Mat R = Test_Rz*Test_Ry*Test_Rx; 
	
    // allocate model and template memory
    double* Source = (double*)calloc(3*num,sizeof(double));
	double* Target = (double*)calloc(3*num,sizeof(double));
	pnts3D_1.col(0) /= pnts3D_1.col(3);
	pnts3D_1.col(1) /= pnts3D_1.col(3);
	pnts3D_1.col(2) /= pnts3D_1.col(3);
	pnts3D_1  = pnts3D_1.rowRange(0,3).t();
	pnts3D_2  = pnts3D_2.rowRange(0,3).t();
	
	// set model and template points
	for(int i=0; i<pnts3D_1.rows; i++)
	{
		Source[i*3+0] = *pnts3D_1.ptr<double>(i,0);
		Source[i*3+1] = *pnts3D_1.ptr<double>(i,1);
		Source[i*3+2] = *pnts3D_1.ptr<double>(i,2);		
	}
	for(int i=0; i<pnts3D_2.rows; i++){
		Source[(i+pnts3D_1.rows)*3+0] = *pnts3D_2.ptr<double>(i,0);
		Source[(i+pnts3D_1.rows)*3+1] = *pnts3D_2.ptr<double>(i,1);  
		Source[(i+pnts3D_1.rows)*3+2] = *pnts3D_2.ptr<double>(i,2);
	}
	
	for(int i=0; i<pnts3D_1.rows+pnts3D_2.rows; i++){
		Target[i*3+0] = Source[i*3+0]*(*R.ptr<double>(0,0))+Source[i*3+1]*(*R.ptr<double>(0,1))+Source[i*3+2]*(*R.ptr<double>(0,2))+1;
		Target[i*3+1] = Source[i*3+0]*(*R.ptr<double>(1,0))+Source[i*3+1]*(*R.ptr<double>(1,1))+Source[i*3+2]*(*R.ptr<double>(1,2))+2;
		Target[i*3+2] = Source[i*3+0]*(*R.ptr<double>(2,0))+Source[i*3+1]*(*R.ptr<double>(2,1))+Source[i*3+2]*(*R.ptr<double>(2,2))+3;
		
	}
     // start with identity as initial transformation
     // in practice you might want to use some kind of prediction here
     
	 Matrix R1 = Matrix::eye(3);
	 Matrix t1(3,1);

     // run point-to-plane ICP (-1 = no outlier threshold)

	 IcpPointToPoint icp(Target,num,dim);
	 icp.fit(Source,num,R1,t1,-1);

	 for(int i = 0; i < 3; ++i) 
	 cout<<*R.ptr<double>(i,0)<<" "<<*R.ptr<double>(i,1)<<" "<<*R.ptr<double>(i,2)<<endl; // results
     std::cout << std::endl << "Transformation results:" << std::endl;
     std::cout << "R:" << std::endl << R1 << std::endl << std::endl;
     std::cout << "t:" << std::endl << t1 << std::endl << std::endl;
	 
	 for(int i=0; i<3; i++)
	   for(int j=0; j<3 ;j++)
	   {
		   *RR.ptr<double>(i,j) = R1.val[i][j];
	   }
	 for(int i=0; i<3 ;i++)
	    *TT.ptr<double>(i) = t1.val[0][i];
     // free memory
	 free(Target);
	 free(Source);
     return;
}

void loadcalib_pnts(Vector<Point3f> &calibed_pnts3d, Mat calib_mat)
{
	 Point3f point;
	 for(int i=0; i<calib_mat.cols; i++)
	 {
		 point.x = float(*calib_mat.ptr<double>(i,0));
		 point.y = float(*calib_mat.ptr<double>(i,1));
		 point.z = float(*calib_mat.ptr<double>(i,2));
		 calibed_pnts3d.push_back(point);
	 }
}

int main()
{
	
	int snum_1L=0,snum_1R=0,snum_2L=0,snum_2R=0,cunt_num=15,out_snum=0;                    //定义左右1,2相机的起始位置
	bool isopen_1 = false;
	bool isopen_2 = false;
	string calibe_file_1,calibe_file_2;                                                     //calibe_file 表示载入的配置问文件
	LzSerialStorageAcqui * lzacqui_1L = new LzSerialStorageAcqui();                         //组1左相机流式存储类
	LzSerialStorageAcqui * lzacqui_1R = new LzSerialStorageAcqui();                         //组1右相机流式存储类
	LzSerialStorageAcqui * lzacqui_2L = new LzSerialStorageAcqui();                         //组2左相机流式存储类
	LzSerialStorageAcqui * lzacqui_2R = new LzSerialStorageAcqui();                         //组2右相机流式存储类
	LzSerialStorageMat   * lzstore_R_mat = new LzSerialStorageMat();
	LzSerialStorageMat   * lzstore_T_mat = new LzSerialStorageMat();
	LzSerialStorageMat * lzacqui_calib_pnts = new LzSerialStorageMat();                     //载入calibed_points标准三维点
	string L1_img_file = "C:\\Users\\Administrator\\Desktop\\标定数据\\serialfile_1.dat";
	string R1_img_file = "C:\\Users\\Administrator\\Desktop\\标定数据\\serialfile_2.dat";
	string L2_img_file = "C:\\Users\\Administrator\\Desktop\\标定数据\\serialfile_3.dat";
	string R2_img_file = "C:\\Users\\Administrator\\Desktop\\标定数据\\serialfile_4.dat";
	string R_out_file = "C:\\Users\\Administrator\\Desktop\\标定数据\\R_out_file.dat";           //R矩阵的输出文件
	string T_out_file = "C:\\Users\\Administrator\\Desktop\\标定数据\\T_out_file.dat";           //T矩阵的输出文件
	string calib_pnts_file = "C:\\Users\\Administrator\\Desktop\\标定数据\\calib_pnts_file.dat"; //标准三维点的输入文件
	calibe_file_1 = "C:\\Users\\Administrator\\Desktop\\LZ_project\\result.xml";            
	calibe_file_2 = "C:\\Users\\Administrator\\Desktop\\LZ_project\\result.xml";            
	Vector<Point3f> calibed_pnts3d;

	Mat calib_mat;
	LzCalculation Cam_1, Cam_2;                                                            //生成两组相机计算类 得到两组3维点
	Cam_1.loadcalib(calibe_file_1);
	Cam_2.loadcalib(calibe_file_2);
	
    lzacqui_1L->openFile(L1_img_file.c_str());
    lzacqui_1R->openFile(R1_img_file.c_str());  
	lzacqui_2L->openFile(L2_img_file.c_str());
    lzacqui_2R->openFile(R2_img_file.c_str());
	lzstore_R_mat->createFile(R_out_file.c_str(),sizeof(DataHead));
	lzstore_T_mat->createFile(T_out_file.c_str(),sizeof(DataHead));
	isopen_1 = lzstore_R_mat->openFile(R_out_file.c_str());
	isopen_2 = lzstore_T_mat->openFile(T_out_file.c_str());

	lzacqui_calib_pnts->openFile(calib_pnts_file.c_str());
	lzacqui_calib_pnts->retrieveBlock(0);
	lzacqui_calib_pnts->readMat(calib_mat);
	loadcalib_pnts(calibed_pnts3d,calib_mat);

	std::cout<<isopen_1<<" "<<isopen_2<<std::endl;
	std::vector<BLOCK_KEY> key_1L = lzacqui_1L->readKeys();
    std::vector<BLOCK_KEY> key_1R = lzacqui_1R->readKeys();
	std::vector<BLOCK_KEY> key_2L = lzacqui_2L->readKeys();
    std::vector<BLOCK_KEY> key_2R = lzacqui_2R->readKeys();
	lzacqui_1L->retrieveBlock(snum_1L);
	lzacqui_1R->retrieveBlock(snum_1R);
	lzacqui_2L->retrieveBlock(snum_2L);
	lzacqui_2R->retrieveBlock(snum_2R);
	Mat L1_mat,L2_mat,R1_mat,R2_mat;
	Mat R = Mat(3,3,CV_64FC4);
	Mat T = Mat(3,1,CV_64FC4);
	BlockInfo Info;
	Info.key = out_snum; 
	Info.seqindex = out_snum;
	Info.size = sizeof(BlockInfo);
	for(int i=0 ;i<cunt_num; i++)
	{
		lzacqui_1L->readFrame(L1_mat);
		lzacqui_1R->readFrame(R1_mat);
		lzacqui_2L->readFrame(L2_mat);
		lzacqui_2R->readFrame(R2_mat);
		Cam_1.loadImgMat(L1_mat,R1_mat);
		Cam_1.calc();
		Cam_2.loadImgMat(L2_mat,R2_mat);
		Cam_2.calc();
		Comput_RT(Cam_1.pnts3d,Cam_2.pnts3d,calibed_pnts3d, R, T);
		lzstore_R_mat->writeMat(R,Info,true);                                         
		lzstore_T_mat->writeMat(T,Info,true);                                         
		Info.key++;
	}
	lzacqui_1L->closeFile();
	lzacqui_1R->closeFile();
	lzacqui_2L->closeFile();
	lzacqui_2R->closeFile();
	lzstore_T_mat->closeFile();
	lzstore_R_mat->closeFile();
	delete lzacqui_1L;
	delete lzacqui_1R;
	delete lzacqui_2L;
	delete lzacqui_2R;
	delete lzstore_R_mat;
	delete lzstore_T_mat;
	system("pause");
}