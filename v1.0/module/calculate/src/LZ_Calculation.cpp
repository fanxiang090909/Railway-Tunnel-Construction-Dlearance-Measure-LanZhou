/*/////////////////////////////////////////////////////////////////////////////////
                   代码说明：兰州项目双目计算模块
	               版本信息：beta 1.0   
	               提交时间：2014/8/6
	               作者：    唐书航
				   存在问题：（1）双目计算结果的存储尚有问题。
				             （2）对三维点的Rt矫正须实测调试以验证其精度
///////////////////////////////////////////////////////////////////////////////*/


#include "LZ_Calculation.h"
#include <iostream>
#include <fstream>

using namespace std;
void LzCalculation::calc_single(Mat &img, Mat& Full_img,vector< vector<double> > &sppfs, Mat &map1, Mat &map2, int flag) //flag 为1表示左图，2表示右图用于填充左右图像
{
	vector<Point2f> pfs;
	int color_threshold = 0;
    int color_threshold_s = 0;
	color_threshold = ostu_threshold(img)*1.5;
    color_threshold_s = color_threshold_s*2;
   // if((color_threshold_s < 20 && color_threshold_s >= 3)||color_threshold >= 2)
	//	color_threshold = 9.5;
	cout<<"threshold:"<<color_threshold<<endl;
	if(color_threshold <= valid_thres)
	{
		is_valid = false;
	}	
	Full_img = Mat::zeros(1280,1024,img.type());
	if(1==flag)
	{
	   img.copyTo(Full_img.colRange(512,1024));
	}
	else
	{
	   img.copyTo(Full_img.colRange(0,512));
	}
	remap(Full_img, img, map1, map2, INTER_LINEAR);
	coarseCenter(img,pfs,color_threshold); 
	subpixCenter(img,pfs,sppfs);
	for(int i=0; i<img.rows; i++)
	{
	     if(!sppfs[i].empty())
		 {
		     for(vector<double>::iterator it = sppfs[i].begin(); it!= sppfs[i].end(); ++it)
			 {
			     img.at<uchar>(i,cvRound(*it)) = 0;
			 }
		 }
	}

	printf("分割阀值：%d\n",color_threshold);
//	namedWindow("Image",CV_WINDOW_AUTOSIZE); 
//   imshow("Image",img);
//	cvWaitKey(0);
	                   //用于标准显示光带图像
}  

int LzCalculation::ostu_threshold(Mat& img)  //???
{

	CV_Assert(img.depth() != sizeof(uchar)); 
	double T1,S1;
	double S1_max = -10000;
	int h[256]={0};
	int C_threshold = 0;//Ostu阀值
	int nrow = img.rows;
	int ncol = img.cols*img.channels();
	long N = img.rows*img.cols;	
	double p[256]={0}, u[256]={0}, w[256]={0};
	for(int i=0; i<nrow; i++)
	{
		uchar *data = img.ptr<uchar>(i); //获得一行的图像元素
		for(int j = 0; j < ncol; j++)
		{
		   h[int(data[j])]++;//统计每个灰度级的数目
		}
	}
	for(int i = 0; i < 256; i++)
	{
		p[i] = h[i] / double(N);
	}	
	for(int k = 0; k < 256; k++)
	{
		T1 = 0;
		for(int i = 0; i <= k; i++)
		{
			u[k] += i*p[i];
			w[k] += p[i];
		}
		for(int i = 0; i < 256; i++)
		{
			T1 += i*p[i];
		}
		S1 = (T1*w[k] - u[k])*(T1*w[k] - u[k]) / (w[k]*(1-w[k]));
		if(S1 > S1_max)
		{
			S1_max = S1;
			 C_threshold = k;
		}
	}
	return C_threshold;
}  //??

void LzCalculation::loadcalib(string filename,int flag)
{
    // @author 范翔 20141112，如果不加函数局部变量的string空间，会出现缓冲区溢出错误。待解决 TODO，已改成全局变量FileStorage
   // string filename1 = "";
    //FileStorage fs;
    bool ret = fs.open(filename, FileStorage::READ);

    if ( !fs.isOpened() )
    {
        // throw exception;
        std::cout<<"Can't open the Calib_file!";
        return;
    }

    fs["P1"] >> P1;
    fs["P2"] >> P2;
    fs["R1"] >> R1;
    fs["R2"] >> R2;
    fs["lcameramat"] >> lcameramat;
    fs["rcameramat"] >> rcameramat;
    fs["ldistcoeffs"] >> ldistcoeffs;
    fs["rdistcoeffs"] >> rdistcoeffs;
    fs["R"] >> Trans_R;
    fs["T"] >> Trans_T;
    fs["recsizewid"] >> recsize.width;
    fs["recsizehei"] >> recsize.height;
    recsize = Size(REC_IMG_SIZE_WID, REC_IMG_SIZE_HEI);

    fs.release();

    initUndistortRectifyMap(lcameramat, ldistcoeffs, R1, P1,
        recsize, CV_32FC1, lmap1, lmap2);
    initUndistortRectifyMap(rcameramat, rdistcoeffs, R2, P2,
        recsize, CV_32FC1, rmap1, rmap2);
        
}

void LzCalculation::loadImgMat(string L_mat,string R_Mat)
{
	Ver_val = 0;
    Rt_val  = 0;
    Lt_val  = 0;
    Hor_val = 0;
    Img_Direct = 0;
    Light_width = 3; //定义光带中心宽度
	M_Cen_L = -1;    //对当前中心初始化
	M_Cen_R = -1;    //
	img_L = imread(L_mat,CV_LOAD_IMAGE_GRAYSCALE);//................左图的光带提取
	img_R = imread(R_Mat,CV_LOAD_IMAGE_GRAYSCALE);//................右图的光带提取
	remap(img_L, img_L, lmap1, lmap2, INTER_LINEAR);
	remap(img_R, img_R, rmap1, rmap2, INTER_LINEAR);
	is_valid = true;
	valid_thres = 3;
	kernel_vertic = (Mat_<int>(5,7)<<0,0,1,3,1,0,0, 0,0,1,3,1,0,0, 0,0,1,3,1,0,0, 0,0,1,3,1,0,0, 0,0,1,3,1,0,0); //竖直方向模板
    kernel_Rtilt  = (Mat_<int>(5,7)<<1,3,1,0,0,0,0, 0,1,3,1,0,0,0, 0,0,1,3,1,0,0, 0,0,0,1,3,1,0, 0,0,0,0,1,3,1); //右倾方向模板
    kernel_Ltilt  = (Mat_<int>(5,7)<<0,0,0,0,1,3,1, 0,0,0,1,3,1,0, 0,0,1,3,1,0,0, 0,1,3,1,0,0,0, 1,3,1,0,0,0,0); //左倾方向模板
    kernel_Hor    = (Mat_<int>(5,7)<<0,0,0,0,0,0,0, 0,1,1,1,1,1,0, 0,3,3,3,3,3,0, 0,1,1,1,1,1,0, 0,0,0,0,0,0,0); //水平方向模板
}

void LzCalculation::loadImgMat(Mat L_mat,Mat R_mat)       
{
	Ver_val = 0;
    Rt_val  = 0;
    Lt_val  = 0;
    Hor_val = 0;
    Img_Direct = 0;
    Light_width = 3;                   //定义光带中心宽度
	M_Cen_L = -1;                      //对当前中心初始化
	M_Cen_R = -1;                       
	is_valid = true;
	valid_thres = 3;                  //定义有效分割伐值低于20判断为无光带图像
	//medianBlur(L_mat,L_mat,3);
	//blur(_mat,L_mat,Size(3,3));
//	imwrite("C:\\L_img.jpg",L_mat);
//	imwrite("C:\\R_img.jpg",R_mat);
	img_L = L_mat;
	img_R = R_mat; 
	//remap(L_mat, img_L, lmap1, lmap2, INTER_LINEAR);            //................左图的光带提取
	//remap(R_mat, img_R, rmap1, rmap2, INTER_LINEAR);            //................右图的光带提取
	kernel_vertic = (Mat_<int>(5,7)<<0,0,1,3,1,0,0, 0,0,1,3,1,0,0, 0,0,1,3,1,0,0, 0,0,1,3,1,0,0, 0,0,1,3,1,0,0); //竖直方向模板
    kernel_Rtilt  = (Mat_<int>(5,7)<<1,3,1,0,0,0,0, 0,1,3,1,0,0,0, 0,0,1,3,1,0,0, 0,0,0,1,3,1,0, 0,0,0,0,1,3,1); //右倾方向模板
    kernel_Ltilt  = (Mat_<int>(5,7)<<0,0,0,0,1,3,1, 0,0,0,1,3,1,0, 0,0,1,3,1,0,0, 0,1,3,1,0,0,0, 1,3,1,0,0,0,0); //左倾方向模板
    kernel_Hor    = (Mat_<int>(5,7)<<0,0,0,0,0,0,0, 0,1,1,1,1,1,0, 0,3,3,3,3,3,0, 0,1,1,1,1,1,0, 0,0,0,0,0,0,0); //水平方向模板
}

int LzCalculation::conv_Center(Mat &img, Point2f center, Mat &kernel)//计算每一个粗提取点的卷积响应
{
	int i,j,m,n;
	int nrow = img.rows;
	int ncol = img.cols;
	int value = 0;
	uchar* data_img = NULL;
	int* data_mat = NULL;
	for(i=int(center.x)-2,m=0; i<=int(center.x)+2; i++,m++)
	{	
		if(0<=i&&i<nrow)
		{
			data_img = img.ptr<uchar>(i); 
		    data_mat = kernel.ptr<int>(m);
		}
		for(j=int(center.y)-3,n=0; j<=int(center.y)+3; j++,n++)
		{
			if(0<=i&&i<nrow&&0<=j&&j<ncol)
			{	
				value += int(data_img[j])*data_mat[n]; 
			}
		}
	}
	return value;
}

void LzCalculation::coarseCenter(Mat &img, vector<Point2f> &pfs, int color_threshold )
{
	/*pfs.resize(10);*/
	//1. 参数有效性
	//2. 参数预处理
	//3. 计算
	//4. 返回结果
	int flag = -1,i,j;                  //用于标记当前指针是否位于光带内
	int nrow = img.rows;
	int ncol = img.cols*img.channels();
	Point2f P_center;
	for(i=0;i<nrow;i++)
	{
		uchar *data = img.ptr<uchar>(i); //获得一行的图像元素
		for(j=0;j<ncol;j++)
		{
		   if (color_threshold<=int(data[j])&&-1==flag)
			{
				flag = 1;
				P_center.x = float(i);
				P_center.y = float(j);
			}
			else if (int(data[j])<color_threshold&&1==flag)
			{
				flag = -1;
				P_center.x = (P_center.x+i)/2;
				P_center.y = (P_center.y+j-1)/2;	
				pfs.push_back(P_center);
			}			
		}
		flag=-1;
	  }

}

bool LzCalculation::is_match(double point_L,double point_R)
{
    
	double threshold = 0.08748;                   //5度的tan值
	double f_L  = *lcameramat.ptr<double>(1,1);   //左相机焦距
	double f_R  = *rcameramat.ptr<double>(1,1);   //右相机焦距
	double cy_L = *lcameramat.ptr<double>(1,2);   //左相机Y方向主点
	double cy_R = *rcameramat.ptr<double>(1,2);   //右相机Y方向主点
	double k1,b1,k2,b2;                           //k1左,k2右斜率  b1,b2左右截距
	double base_line = 50;                        //基线宽50cm
	double pix_wid   = 0.00053;                   //像素宽5.3微米
	double x2_L,x2_R;
	double med,res_x,res_y,tan;
	f_L  *= pix_wid;
	f_R  *= pix_wid;
 	cy_L *= pix_wid;
	cy_R *= pix_wid;
	x2_L = point_R*pix_wid;
	x2_R = point_L*pix_wid;
	k1 = f_L/(x2_L-cy_L);
	b1 = -(k1*cy_L);      
	k2 = f_R/(x2_R-cy_R);
	b2 = -(k2*(cy_R+base_line));
	res_x = (b1-b2)/(k2-k1);
	res_y = k1*res_x+b1;
	med = (cy_R+base_line-cy_L)/2+cy_L;
	tan = abs(res_x-med)/res_y;
	if(tan<threshold)
		return true;
	else
		return false;
}

void LzCalculation::subpix_Direct(Mat &img,vector<Point2f> &pfs, vector< vector<double> > &sppfs, int Img_Direct)//计算四个方向的亚像素
{
    double sub_center = 0;
	double weight_val = 0;
	int nrow = img.rows;
	int ncol = img.cols;
    for(vector<Point2f>::iterator it = pfs.begin(); it!= pfs.end(); ++it)
	{
		 switch(Img_Direct)
	     {
	         case 1:  //竖直方向的亚像素提取
		     {
			      uchar* data_img = img.ptr<uchar>(int((*it).x));
		          for(int i=cvRound((*it).y)-Light_width; i<=cvRound((*it).y)+Light_width; i++)
		          {
		                if(0<=i&&i<ncol)
			            {
			                sub_center += double(data_img[i])*i;
				            weight_val += double(data_img[i]);
			            }
		          }
		     }
		     break;

		     case 2:  //右倾方向的亚像素提取
		     {    
			      int x,y;
				  x = cvRound((*it).x)+Light_width;
				  y = cvRound((*it).y)-Light_width;
			      for(int i=0; i<=Light_width*2; i++)
				  {  
					   if(0<=x&&x<nrow&&0<=y&&y<ncol)
					   {
						   sub_center += double(img.at<uchar>(x,y))*y;
						   weight_val += double(img.at<uchar>(x,y));
					   }
					   x--;
					   y++;				 
				  }
		     }
		     break;

		     case 3:  //左倾方向的亚像素提取
		     {
			      int x,y;
				  x = cvRound((*it).x)-Light_width;
				  y = cvRound((*it).y)-Light_width;
			      for(int i=0; i<=Light_width*2; i++)
				  {  
					   if(0<=x&&x<nrow&&0<=y&&y<ncol)
					   {
						   sub_center += double(img.at<uchar>(x,y))*y;
						   weight_val += double(img.at<uchar>(x,y));
					   }
					   x++;
					   y++;				 
				  }
		     }
		     break;

		     case 4:  //水平方向的亚像素提取
		     {    
			      for(int i=cvRound((*it).x)-Light_width; i<=cvRound((*it).x)+Light_width; i++)
				  {
				      if(0<=i&&i<nrow)
					  {
					       sub_center += double(img.at<uchar>(i,int((*it).y)))*i;
						   weight_val += double(img.at<uchar>(i,int((*it).y)));
					  }
				  }
			      
	
		     }
		     break;
	     }
		 sub_center /= weight_val;
		 if(4==Img_Direct)
		     sppfs[cvRound(sub_center)].push_back((*it).y);
	     else
             sppfs[int((*it).x)].push_back(sub_center);
		 sub_center = 0;
	     weight_val = 0;
	}
}

void LzCalculation::subpixCenter(Mat &img, vector<Point2f> &pfs , vector< vector<double> > &sppfs)
{
		//1. 参数有效性
		//2. 参数预处理
		//3. 计算
		//4. 返回结果
	 int Max[4] = {0};
	 int Max_num = 0;
	 int nrow = img.rows;
	 int ncol = img.cols;
	 Ver_val = 0;
     Rt_val  = 0;
     Lt_val  = 0;
     Hor_val = 0;
	 sppfs.resize(nrow,vector<double>(0));
	 for(vector<Point2f>::iterator it = pfs.begin(); it!= pfs.end(); ++it)
	 {
		 Ver_val += conv_Center(img,*it,kernel_vertic);
         Rt_val  += conv_Center(img,*it,kernel_Rtilt);
         Lt_val  += conv_Center(img,*it,kernel_Ltilt);
         Hor_val += conv_Center(img,*it,kernel_Hor);	
	 }
	 Max_num = Ver_val;
	 Max[0] = Ver_val;
	 Max[1] = Rt_val;
	 Max[2] = Lt_val;
	 Max[3] = Hor_val;
	 for(int i=0;i<4;i++)
	 {
	     if(Max_num<=Max[i])
		 {
		     Max_num = Max[i];
		     Img_Direct = i+1;
		 }
	 }
//	 printf("计算个方向卷积响应：\n");
//     printf("竖直: %d  右倾：%d  左倾：%d  水平：%d\n",Ver_val,Rt_val,Lt_val,Hor_val);
//	 printf("Img_direct: %d\n",Img_Direct);
	 subpix_Direct(img,pfs,sppfs,Img_Direct);

}
	
void LzCalculation::match_pts(vector<double> &Vec_L, vector<double> &Vec_R)
{

    vector<double> V_L(Vec_L.begin(),Vec_L.end());
	vector<double> V_R(Vec_R.begin(),Vec_R.end());
	double Near_L = 1000,Near_R = 1000;
	if(M_Cen_L<0 || M_Cen_R<0)
	{
	   for(vector<double>::iterator point_L = V_L.begin(); point_L!= V_L.end(); ++point_L)
		 for(vector<double>::iterator point_R = V_R.begin(); point_R!= V_R.end(); ++point_R)
		 {
			if(is_match(*point_L,*point_R))                         //根据基线准能算出左右匹配点
			{
				Vec_L.clear();
				Vec_R.clear();
				Vec_L.push_back(*point_L);
				Vec_R.push_back(*point_R);
				M_Cen_L = *point_L;
				M_Cen_R = *point_R;
				return ;
			}		 
		 }
		 //若不能匹配则默认将向量中第一个点作为左右匹配点
	}
	else
	{
	    for(vector<double>::iterator point_L = V_L.begin(); point_L!= V_L.end(); ++point_L)
	    {
	        if(abs(*point_L-M_Cen_L)<abs(Near_L))
		    {
		        Near_L = *point_L-M_Cen_L;            //获取容器中与上一节点最近的点
		    }
	    }
	    for(vector<double>::iterator point_R = V_R.begin(); point_R!= V_R.end(); ++point_R)
	    {
	        if(abs(*point_R-M_Cen_R)<abs(Near_R))
		    {
		       Near_R = *point_R-M_Cen_R;            //获取容器中与上一节点最近的点
		    }
		}
		Vec_L.clear();
	    Vec_R.clear();
	    Vec_L.push_back(Near_L+M_Cen_L);
	    Vec_R.push_back(Near_R+M_Cen_R);
	}
	
}

void LzCalculation::match2dpts(Mat img, vector< vector<double> > &sppfs_L, vector< vector<double> > &sppfs_R, 
	            vector<Point2f> &lft_pts, vector<Point2f> &rgt_pts)
{
	Point2f pts_L,pts_R;
	for(int i=0; i<img.rows; i++)
	{
	     if(!sppfs_L[i].empty()&&!sppfs_R[i].empty())
		 {
		     if(sppfs_L[i].size()!=1 || sppfs_R[i].size()!=1)   
			 { 
			    match_pts(sppfs_L[i],sppfs_R[i]);       //当左右图有不止一个点时调用匹配函数
		     }
			 pts_L.x = float(i);
		     pts_R.x = float(i);
			 pts_L.y = float(sppfs_L[i][0]);            //始终将向量第一个点放入最终匹配向量中
			 pts_R.y = float(sppfs_R[i][0]);
			 M_Cen_L = sppfs_L[i][0];                   //用于记录上一行的中心作为判断下一行的依据（离中心最近）
	         M_Cen_R = sppfs_R[i][0];
			 lft_pts.push_back(pts_L);
			 rgt_pts.push_back(pts_R);
		 }
		 else
		 {
			 sppfs_L[i].clear();                         //当左右任意一容器为空时清空容器
			 sppfs_R[i].clear();
		 }
	}
}

void LzCalculation::rectify_fenzhong(Mat &P,Mat &rect_P)
{
	rect_P = Trans_R * (P.rowRange(0,3));
	for(int i=0; i<pnts3d.cols; i++)
	{
		rect_P.at<float>(0,i) += Trans_T.at<float>(0,0); 
		rect_P.at<float>(1,i) += Trans_T.at<float>(1,0); 
		rect_P.at<float>(2,i) += Trans_T.at<float>(2,0); 
		///std::cout
//		*rect_P.ptr<float>(0,i) = P.at<float>(0,i)*(*Trans_R.ptr<double>(0,0))+P.at<float>(1,i)*(*Trans_R.ptr<double>(1,0))+P.at<float>(2,i)*(*Trans_R.ptr<double>(2,0))+*Trans_T.ptr<float>(0);
//		*rect_P.ptr<float>(1,i) = P.at<float>(0,i)*(*Trans_R.ptr<double>(0,1))+P.at<float>(1,i)*(*Trans_R.ptr<double>(1,1))+P.at<float>(2,i)*(*Trans_R.ptr<double>(2,1))+*Trans_T.ptr<float>(1);
//		*rect_P.ptr<float>(2,i) = P.at<float>(0,i)*(*Trans_R.ptr<double>(0,2))+P.at<float>(1,i)*(*Trans_R.ptr<double>(1,2))+P.at<float>(2,i)*(*Trans_R.ptr<double>(2,2))+*Trans_T.ptr<float>(2);
//		rect_P.at<float>(0,i) = (*Trans_R.ptr<double>(0,0))*P.at<float>(0,i)+(*Trans_R.ptr<double>(0,1))*P.at<float>(1,i)+(*Trans_R.ptr<double>(0,2))*P.at<float>(2,i)+*Trans_T.ptr<double>(0);
//		rect_P.at<float>(1,i) = (*Trans_R.ptr<double>(1,0))*P.at<float>(0,i)+(*Trans_R.ptr<double>(1,1))*P.at<float>(1,i)+(*Trans_R.ptr<double>(1,2))*P.at<float>(2,i)+*Trans_T.ptr<double>(1);
//		rect_P.at<float>(2,i) = (*Trans_R.ptr<double>(2,0))*P.at<float>(0,i)+(*Trans_R.ptr<double>(2,1))*P.at<float>(1,i)+(*Trans_R.ptr<double>(2,2))*P.at<float>(2,i)+*Trans_T.ptr<double>(2);
	}
}

void LzCalculation::calc()
{
	vector<Point2f> lft_pts, rgt_pts;
    vector<vector<double>> sppfs_L,sppfs_R;
	calc_single(img_L, Full_L_img, sppfs_L, lmap1, lmap2, 1);                            //计算左图的光带中心，并放入向量中
 	calc_single(img_R, Full_R_img, sppfs_R, rmap1, rmap2, 2);	                        //计算右图的光带中心，并放入向量中
	float replace = 0;
	if(is_valid)//当光带图像有效时计算
	{
	    match2dpts(img_L,sppfs_L,sppfs_R,lft_pts,rgt_pts);   //匹配两组光带中心
	    if(!lft_pts.empty())
	    {
		     for(vector<Point2f>::iterator it = lft_pts.begin(); it!= lft_pts.end(); ++it)
			 {
				 img_L.at<uchar>(cvRound((*it).x),cvRound((*it).y)) = 255;
				 replace = (*it).x;
				 (*it).x = (*it).y;
				 (*it).y = replace;
			 }
	    }
//	    namedWindow("Image_L",CV_WINDOW_AUTOSIZE); 
//      imshow("Image_L",img_L);
//		imwrite("C:\\img_L.jpg",img_L);
//	    cvWaitKey(0);
		 if(!rgt_pts.empty())
	    {
		     for(vector<Point2f>::iterator it = rgt_pts.begin(); it!= rgt_pts.end(); ++it)
			 {
				 img_R.at<uchar>(cvRound((*it).x),cvRound((*it).y)) = 255;
				 replace = (*it).x;
				 (*it).x = (*it).y;
				 (*it).y = replace;
			 }
	    }
//	    namedWindow("Image_R",CV_WINDOW_AUTOSIZE); 
//        imshow("Image_R",img_R);
//		imwrite("C:\\img_R.jpg",img_R);
//	    cvWaitKey(0);
		//imwrite("C:\\Users\\Administrator\\Desktop\\img_out\\B2_断面.jpg",img_L);
		//imwrite("C:\\Users\\Administrator\\Desktop\\img_out\\B1_断面.jpg",img_R);
        pnts3d = Mat(4,lft_pts.size(),CV_64FC4);
		rect_pnts3d = Mat(4,lft_pts.size(),CV_64FC4);
	    triangulatePoints(P1,P2, lft_pts, rgt_pts, pnts3d);  //计算三维点函数
		pnts3d.row(0) /= pnts3d.row(3);
		pnts3d.row(1) /= pnts3d.row(3);
		pnts3d.row(2) /= pnts3d.row(3);
		pnts3d.row(3) /= pnts3d.row(3);
	/*	for(int i=0; i<pnts3d.cols; i++)
	    {
			cout<<pnts3d.at<float>(0,i)<<endl;
			cout<<pnts3d.at<float>(1,i)<<endl;
			cout<<pnts3d.at<float>(2,i)<<endl;
			cout<<endl;

	    }*/
     	rectify_fenzhong(pnts3d,rect_pnts3d);
		ofstream SaveFile("C:\\Output.txt");
		for(int i=0; i<pnts3d.cols; i++)
	    {
			SaveFile<<"第"<<i<<"点坐标："<<endl;
			SaveFile<<"高度： "<<*rect_pnts3d.ptr<float>(0,i)<<" 原点:"<<*pnts3d.ptr<float>(0,i)<<endl;
			SaveFile<<"水平： "<<*rect_pnts3d.ptr<float>(1,i)<<" 原点:"<<*pnts3d.ptr<float>(1,i)<<endl;
			SaveFile<<"Z坐标："<<*rect_pnts3d.ptr<float>(2,i)<<" 原点:"<<*pnts3d.ptr<float>(2,i)<<endl;
			SaveFile<<endl;

		}
		SaveFile.close();
	}
}

