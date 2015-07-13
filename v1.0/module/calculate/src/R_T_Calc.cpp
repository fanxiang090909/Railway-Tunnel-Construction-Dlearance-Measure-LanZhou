/*********************************************************************************
**********************************************************************************
                          ����˵����������Ŀ����RT�������ģ��
                          �汾��Ϣ��beta 1.0
                          �ύʱ�䣺2014/8/10
			              ���ߣ�    ���麽
						  �������⣺����ڲ����Ա�֤��³����
***********************************************************************************
*********************************************************************************/
#include "R_T_Calc.h"
#include <iostream>
#include <fstream>
using namespace std;

void LzCalculationRT::get_rail_ps(Mat &img,vector<vector<double>> &rail_pfs,vector<vector<double>> &sppfs)
{
	int min_dis = 5;                                                          //��Сֵ�ֹ��жϽ���
	int max_dis = 25;                                                         //���ֵ
	int rail_point = 0;                                                       //��¼�ֹ������
	int rail_p_cunt = 0;                                                      //���ڼ�¼�ֹ��ĸ���
	bool inital = true;
	bool find_p = false;
	bool Is_end = true;
	sppfs.resize(img.rows,vector<double>(0));
	for(int i=450; i<1050; i++)                                                //���λ��ͼ��߶�400-700֮��
	{
	    //ǰ���ѻ������������ģ�ѭ������ȡ����ƽ��
		if(!rail_pfs[i].empty())
		 {
		     for(vector<double>::iterator it = rail_pfs[i].begin(); it!= rail_pfs[i].end(); it++)
			 {
			    // img.at<uchar>(i,cvRound(*it)) = 0;
				 if(inital)                                                   //�Ըֹ���ʼ������жϣ���ʼ����
				 {
					 rail_point = cvRound(*it);
					 inital = false;
				 }
				 else if(!find_p)
				 {
					 if(abs(cvRound(*it)-rail_point)<10)
					 {
						 rail_point = cvRound(*it);
					 }
					 else if(abs(cvRound(*it)-rail_point)<90)
					 {
//						 std::cout<<"Max:"<<abs(cvRound(*it)-rail_point)<<std::endl;
						 rail_point = cvRound(*it);
						 find_p = true;
					 }
				 }
				 if(find_p&&rail_p_cunt<150&&abs(cvRound(*it)-rail_point)<3)           //�������ڸֹ��ڲ�������һ������3��������Ȼ�ǹ��
		         {
					 sppfs[i].push_back(*it);
					 img.at<uchar>(i,cvRound(*it)) = 0;
//					 std::cout<<i<<endl;
					 rail_point = cvRound(*it);
					 rail_p_cunt++;
		         }
    			 else if(find_p&&Is_end&&abs(cvRound(*it)-rail_point)<25)
				 {
//					 std::cout<<"Min:"<<abs(cvRound(*it)-rail_point)<<std::endl;
					 Is_end = false;
				 }		
			 }
		}
	  
	}

}

void LzCalculationRT::calc_single(Mat &img, vector< vector<double> > &sppfs)      //��ȡ����ƽ��
{
	vector<Point2f> pfs;
	vector<vector<double>> rail_pfs;                                          //��ƽ��㼯
	int color_threshold = 10;

	color_threshold = ostu_threshold(img)*0.8;
	coarseCenter(img,pfs,color_threshold); 
	subpixCenter(img,pfs,rail_pfs);
    if(pfs.size() < 500)
    {
        is_valid = false;
    }
    get_rail_ps(img,rail_pfs,sppfs);
   
//	namedWindow("threshold image");
//	imshow("threshold image",img);
//    printf("�ָֵ��%d\n",color_threshold);
//	waitKey(0);
//	imwrite("C:\\Users\\Administrator\\Desktop\\out_img.jpg",img);
 // namedWindow("Image"); 
 // imshow("Image",img);
 // cvWaitKey(0);
}

int LzCalculationRT::ostu_threshold(Mat& img)
{

	CV_Assert(img.depth() != sizeof(uchar)); 
	double T1,S1;
	double S1_max = -10000;
	int h[256]={0};
	int C_threshold = 0;//Ostu��ֵ
	int nrow = img.rows;
	int ncol = img.cols*img.channels();
	long N = img.rows*img.cols;	
	double p[256]={0}, u[256]={0}, w[256]={0};
	for(int i=0; i<nrow; i++)
	{
		uchar *data = img.ptr<uchar>(i); //���һ�е�ͼ��Ԫ��
		for(int j = 0; j < ncol; j++)
		{
		   h[int(data[j])]++;//ͳ��ÿ���Ҷȼ�����Ŀ
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
}

void LzCalculationRT::loadcalib(string filename)
{
	Mat standard_P;          //���ڽ��ܱ�׼��                          
	bool ret = fs.open(filename, FileStorage::READ);
	if ( !fs.isOpened() )
	{
		// throw exception;
		return;
	}

	fs["P1"] >> P1;
	fs["P2"] >> P2;
	fs["R1"] >> R1;
	fs["R2"] >> R2;
	fs["lcameramat"] >> lcameramat;
	fs["rcameramat"] >> rcameramat;
	fs["ldistcoeffs"] >> ldistcoeffs;
    fs["standard_point"] >> standard_P;
	fs["R"] >> Trans_R;
    fs["T"] >> Trans_T;
    fs["recsizewid"] >> recsize.width;
    fs["recsizehei"] >> recsize.height;
    recsize = Size(RT_REC_IMG_SIZE_WID, RT_REC_IMG_SIZE_HEI);
    std_P.x = standard_P.at<double>(0,0);
	std_P.y = standard_P.at<double>(0,1);
    initUndistortRectifyMap(lcameramat, ldistcoeffs, R1, P1,
        recsize, CV_32FC1, lmap1, lmap2);
    initUndistortRectifyMap(rcameramat, rdistcoeffs, R2, P2,
        recsize, CV_32FC1, rmap1, rmap2);
	
/*	std::cout<<T<<std::endl;
	std::cout<<R<<std::endl;
	std::cout<<P1<<std::endl;
	std::cout<<P2<<std::endl;*/
	fs.release();
	return ;
}

void LzCalculationRT::loadImgMat(string L_mat,string R_Mat)
{
	Ver_val = 0;
    Rt_val  = 0;
    Lt_val  = 0;
    Hor_val = 0;
    Img_Direct = 0;
    Light_width = 3; //���������Ŀ��
	M_Cen_L = -1;    //�Ե�ǰ���ĳ�ʼ��
	M_Cen_R = -1;    //
	img_L = imread(L_mat,CV_LOAD_IMAGE_GRAYSCALE);//................��ͼ�Ĺ����ȡ
	img_R = imread(R_Mat,CV_LOAD_IMAGE_GRAYSCALE);//................��ͼ�Ĺ����ȡ
	remap(img_L, img_L, lmap1, lmap2, INTER_LINEAR);
	remap(img_R, img_R, rmap1, rmap2, INTER_LINEAR);
	kernel_vertic = (Mat_<int>(5,7)<<0,0,1,3,1,0,0, 0,0,1,3,1,0,0, 0,0,1,3,1,0,0, 0,0,1,3,1,0,0, 0,0,1,3,1,0,0); //��ֱ����ģ��
    kernel_Rtilt  = (Mat_<int>(5,7)<<1,3,1,0,0,0,0, 0,1,3,1,0,0,0, 0,0,1,3,1,0,0, 0,0,0,1,3,1,0, 0,0,0,0,1,3,1); //���㷽��ģ��
    kernel_Ltilt  = (Mat_<int>(5,7)<<0,0,0,0,1,3,1, 0,0,0,1,3,1,0, 0,0,1,3,1,0,0, 0,1,3,1,0,0,0, 1,3,1,0,0,0,0); //���㷽��ģ��
    kernel_Hor    = (Mat_<int>(5,7)<<0,0,0,0,0,0,0, 0,1,1,1,1,1,0, 0,3,3,3,3,3,0, 0,1,1,1,1,1,0, 0,0,0,0,0,0,0); //ˮƽ����ģ��
}

void LzCalculationRT::loadImgMat(Mat L_mat,Mat R_Mat)
{
	Ver_val = 0;
    Rt_val  = 0;
    Lt_val  = 0;
    Hor_val = 0;
    Img_Direct = 0;
    Light_width = 3; //���������Ŀ��
	M_Cen_L = -1;    //�Ե�ǰ���ĳ�ʼ��
	M_Cen_R = -1;   
	remap(L_mat, img_L, lmap1, lmap2, INTER_LINEAR);                 //................��ͼ�Ĺ����ȡ
	remap(R_Mat, img_R, rmap1, rmap2, INTER_LINEAR);
	kernel_vertic = (Mat_<int>(5,7)<<0,0,1,3,1,0,0, 0,0,1,3,1,0,0, 0,0,1,3,1,0,0, 0,0,1,3,1,0,0, 0,0,1,3,1,0,0); //��ֱ����ģ��
    kernel_Rtilt  = (Mat_<int>(5,7)<<1,3,1,0,0,0,0, 0,1,3,1,0,0,0, 0,0,1,3,1,0,0, 0,0,0,1,3,1,0, 0,0,0,0,1,3,1); //���㷽��ģ��
    kernel_Ltilt  = (Mat_<int>(5,7)<<0,0,0,0,1,3,1, 0,0,0,1,3,1,0, 0,0,1,3,1,0,0, 0,1,3,1,0,0,0, 1,3,1,0,0,0,0); //���㷽��ģ��
    kernel_Hor    = (Mat_<int>(5,7)<<0,0,0,0,0,0,0, 0,1,1,1,1,1,0, 0,3,3,3,3,3,0, 0,1,1,1,1,1,0, 0,0,0,0,0,0,0); //ˮƽ����ģ��
}
int LzCalculationRT::conv_Center(Mat &img, Point2f center, Mat &kernel)//����ÿһ������ȡ��ľ����Ӧ
{
	int i,j,m,n;
	int nrow = img.rows;
	int ncol = img.cols;
	int value = 0;
	uchar* data_img = NULL;
	int* data_mat = NULL
		;
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

void LzCalculationRT::coarseCenter(Mat &img, vector<Point2f> &pfs, int color_threshold )
{
		/*pfs.resize(10);*/
		//1. ������Ч��
		//2. ����Ԥ����
		//3. ����
		//4. ���ؽ��
	int flag = -1,i,j;//���ڱ�ǵ�ǰָ���Ƿ�λ�ڹ����
	int nrow = img.rows;
	int ncol = img.cols*img.channels();
	Point2f P_center;
	for(i=0;i<nrow;i++)
	{
		uchar *data = img.ptr<uchar>(i); //���һ�е�ͼ��Ԫ��
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

bool LzCalculationRT::is_match(double point_L,double point_R)
{
    
	double threshold = 0.08748;                   //5�ȵ�tanֵ
	double f_L  = *lcameramat.ptr<double>(1,1);   //���������
	double f_R  = *rcameramat.ptr<double>(1,1);   //���������
	double cy_L = *lcameramat.ptr<double>(1,2);   //�����Y��������
	double cy_R = *rcameramat.ptr<double>(1,2);   //�����Y��������
	double k1,b1,k2,b2;                           //k1��,k2��б��  b1,b2���ҽؾ�
	double base_line = 50;                        //���߿�50cm
	double pix_wid   = 0.00053;                   //���ؿ�5.3΢��
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

void LzCalculationRT::subpix_Direct(Mat &img,vector<Point2f> &pfs, vector< vector<double> > &sppfs, int Img_Direct)//�����ĸ������������
{
    double sub_center = 0;
	double weight_val = 0;
	int nrow = img.rows;
	int ncol = img.cols;
    for(vector<Point2f>::iterator it = pfs.begin(); it!= pfs.end(); ++it)
	{
		 switch(Img_Direct)
	     {
	         case 1:  //��ֱ�������������ȡ
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

		     case 2:  //���㷽�����������ȡ
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

		     case 3:  //���㷽�����������ȡ
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

		     case 4:  //ˮƽ�������������ȡ
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

void LzCalculationRT::subpixCenter(Mat &img, vector<Point2f> &pfs , vector< vector<double> > &sppfs)
{
		//1. ������Ч��
		//2. ����Ԥ����
		//3. ����
		//4. ���ؽ��
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
	// printf("�������������Ӧ��\n");
    // printf("��ֱ: %d  ���㣺%d  ���㣺%d  ˮƽ��%d\n",Ver_val,Rt_val,Lt_val,Hor_val);
	// printf("Img_direct: %d\n",Img_Direct);
	 subpix_Direct(img,pfs,sppfs,Img_Direct);

}
	
void LzCalculationRT::match_pts(vector<double> &Vec_L, vector<double> &Vec_R)
{

    vector<double> V_L(Vec_L.begin(),Vec_L.end());
	vector<double> V_R(Vec_R.begin(),Vec_R.end());
	double Near_L = 1000,Near_R = 1000;
	if(M_Cen_L<0 || M_Cen_R<0)
	{
	   for(vector<double>::iterator point_L = V_L.begin(); point_L!= V_L.end(); ++point_L)
		 for(vector<double>::iterator point_R = V_R.begin(); point_R!= V_R.end(); ++point_R)
		 {
			if(is_match(*point_L,*point_R))                         //���ݻ���׼���������ƥ���
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
		 //������ƥ����Ĭ�Ͻ������е�һ������Ϊ����ƥ���
	}
	else
	{
	    for(vector<double>::iterator point_L = V_L.begin(); point_L!= V_L.end(); ++point_L)
	    {
	        if(abs(*point_L-M_Cen_L)<abs(Near_L))
		    {
		        Near_L = *point_L-M_Cen_L;            //��ȡ����������һ�ڵ�����ĵ�
		    }
	    }
	    for(vector<double>::iterator point_R = V_R.begin(); point_R!= V_R.end(); ++point_R)
	    {
	        if(abs(*point_R-M_Cen_R)<abs(Near_R))
		    {
		       Near_R = *point_R-M_Cen_R;            //��ȡ����������һ�ڵ�����ĵ�
		    }
		}
		Vec_L.clear();
	    Vec_R.clear();
	    Vec_L.push_back(Near_L+M_Cen_L);
	    Vec_R.push_back(Near_R+M_Cen_R);
	}
	
}

void LzCalculationRT::match2dpts(Mat img, vector< vector<double> > &sppfs_L, vector< vector<double> > &sppfs_R, 
	            vector<Point2f> &lft_pts, vector<Point2f> &rgt_pts)
{
	Point2f pts_L,pts_R;
	for(int i=0; i<img.rows; i++)
	{
	     if(!sppfs_L[i].empty()&&!sppfs_R[i].empty())
		 {
		     if(sppfs_L[i].size()!=1 || sppfs_R[i].size()!=1)   
			 { 
			    match_pts(sppfs_L[i],sppfs_R[i]);       //������ͼ�в�ֹһ����ʱ����ƥ�亯��
		     }
			 pts_L.x = float(i);
		     pts_R.x = float(i);
			 pts_L.y = float(sppfs_L[i][0]);
			 pts_R.y = float(sppfs_R[i][0]);
			 M_Cen_L = sppfs_L[i][0];                   //���ڼ�¼��һ�е�������Ϊ�ж���һ�е����ݣ������������
	         M_Cen_R = sppfs_R[i][0];
			 lft_pts.push_back(pts_L);
			 rgt_pts.push_back(pts_R);
		 }
		 else
		 {
			 sppfs_L[i].clear();                         //����������һ����Ϊ��ʱ�������
			 sppfs_R[i].clear();
		 }
	}
}

void LzCalculationRT::rectify_fenzhong(Mat &P,Mat &rect_P)
{
	rect_P = Trans_R * (P.rowRange(0,3));
	for(int i=0; i<pnts3d.cols; i++)
	{
		rect_P.at<float>(0,i) += Trans_T.at<float>(0,0); 
		rect_P.at<float>(1,i) += Trans_T.at<float>(1,0); 
		rect_P.at<float>(2,i) += Trans_T.at<float>(2,0); 
	}
}

void LzCalculationRT::get_rail(Mat& rect_pnts3d)
{
	double r_h = 0;
	double r_v = 0;
	if(20 <= rect_pnts3d.cols)
	{
	   for(int i=0; i<20;i++)
		   r_h += rect_pnts3d.at<float>(0,i);
	   r_h /= 20;
	   for(int i = (rect_pnts3d.cols-10); i<rect_pnts3d.cols ; i++)
		   r_v += rect_pnts3d.at<float>(1,i);
	   r_v /= 10;
	   rail_P.x = r_h;
	   rail_P.y = r_v;
	}
}

void LzCalculationRT::calc()
{
	float replace = 0;
    is_valid = true;
	vector<Point2f> lft_pts, rgt_pts;
	vector<Point2f> lrail_pts,rrial_pts;
    vector<vector<double>> sppfs_L,sppfs_R;
	calc_single(img_L, sppfs_L);
	calc_single(img_R, sppfs_R);	
	if(is_valid)
	{
	  match2dpts(img_L,sppfs_L,sppfs_R,lft_pts,rgt_pts);
	  if(!lft_pts.empty())
	   {
		    for(vector<Point2f>::iterator it = lft_pts.begin(); it!= lft_pts.end(); ++it)
			{
				 img_L.at<uchar>(cvRound((*it).x),cvRound((*it).y)) = 0;
				 replace = (*it).x;
				 (*it).x = (*it).y;
				 (*it).y = replace;
			}
	   }
	   if(!rgt_pts.empty())
	   {
		    for(vector<Point2f>::iterator it = rgt_pts.begin(); it!= rgt_pts.end(); ++it)
			{
				 img_R.at<uchar>(cvRound((*it).x),cvRound((*it).y)) = 0;
				 replace = (*it).x;
				 (*it).x = (*it).y;
				 (*it).y = replace;
			}
	   }
	   //imwrite("C:\\Users\\Administrator\\Desktop\\out_img_L.jpg",img_L);
       //imwrite("C:\\Users\\Administrator\\Desktop\\out_img_R.jpg",img_R);
       pnts3d = Mat(4,lft_pts.size(),CV_64FC4);
       rect_pnts3d = Mat(4,lft_pts.size(),CV_64FC4);
	   if(lft_pts.size()&&rgt_pts.size())
	   {
	   triangulatePoints(P1,P2, lft_pts, rgt_pts, pnts3d);  //������ά�㺯��
       pnts3d.row(0) /= pnts3d.row(3);
       pnts3d.row(1) /= pnts3d.row(3);
       pnts3d.row(2) /= pnts3d.row(3);
	   pnts3d.row(3) /= pnts3d.row(3);
	   /*for(int i=0; i<pnts3d.cols; i++)
	    {
			cout<<pnts3d.at<float>(0,i)<<endl;
			cout<<pnts3d.at<float>(1,i)<<endl;
			cout<<pnts3d.at<float>(2,i)<<endl;
			cout<<endl;

	    }*/
	   rectify_fenzhong(pnts3d,rect_pnts3d);                 //�������ת��Ϊ��������
	   get_rail(rect_pnts3d);                                //��������ĸߺ�ˮƽλ��
	   ofstream SaveFile("C:\\Users\\Administrator\\Desktop\\R_Output.txt");
	   SaveFile << "��ߣ� "<<rail_P.x<<" ���ؿ�ȣ�"<<rail_P.y<<endl;
		for(int i=0; i<pnts3d.cols; i++)
	    {
			SaveFile<<"��"<<i<<"�����꣺"<<endl;
			SaveFile<<"�߶ȣ� "<<*rect_pnts3d.ptr<float>(0,i)<<" ԭ��:"<<*pnts3d.ptr<float>(0,i)<<endl;
			SaveFile<<"ˮƽ�� "<<*rect_pnts3d.ptr<float>(1,i)<<" ԭ��:"<<*pnts3d.ptr<float>(1,i)<<endl;
			SaveFile<<"Z���꣺"<<*rect_pnts3d.ptr<float>(2,i)<<" ԭ��:"<<*pnts3d.ptr<float>(2,i)<<endl;
			SaveFile<<endl;

		}
		SaveFile.close();
	   }
	}
}




