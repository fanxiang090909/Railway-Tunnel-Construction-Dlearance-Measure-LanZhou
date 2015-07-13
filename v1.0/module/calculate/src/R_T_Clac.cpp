/*********************************************************************************
**********************************************************************************
                          ����˵����������Ŀ����RT�������ģ��
                          �汾��Ϣ��beta 1.0
                          �ύʱ�䣺2014/8/10
                          ���ߣ�    ���麽
                          �������⣺����ڲ����Ա�֤��³����
***********************************************************************************
*********************************************************************************/
#include "R_T_Clac.h"
#include <iostream>
using namespace std;

void LzCalculationRT::calc_single(Mat &img, vector< vector<double> > &sppfs)
{
	vector<Point2f> pfs;
	int color_threshold = 0;
	color_threshold = ostu_threshold(img);
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
    printf("�ָֵ��%d\n",color_threshold);
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
	fs["rdistcoeffs"] >> rdistcoeffs;
	fs["R"] >> R;
	fs["T"] >> T;
	fs["recsizewid"] >> recsize.width;
	fs["recsizehei"] >> recsize.height;
	
/*	std::cout<<T<<std::endl;
	std::cout<<R<<std::endl;
	std::cout<<P1<<std::endl;
	std::cout<<P2<<std::endl;*/
	fs.release();
	return ;
/*	initUndistortRectifyMap(lcameramat, ldistcoeffs, R1, P1,
	recsize, CV_32FC1, lmap1, lmap2);
	initUndistortRectifyMap(rcameramat, rdistcoeffs, R2, P2,
	recsize, CV_32FC1, rmap1, rmap2);*/


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
	img_L = L_mat;                     //................��ͼ�Ĺ����ȡ
	img_R = R_Mat;                     //................��ͼ�Ĺ����ȡ
	//is_valid = true;
	//valid_thres = 20;
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

void LzCalculationRT::calc()
{
	vector<Point2f> lft_pts, rgt_pts;
    vector<vector<double>> sppfs_L,sppfs_R;
	calc_single(img_L, sppfs_L);
	calc_single(img_R, sppfs_R);	
	match2dpts(img_L,sppfs_L,sppfs_R,lft_pts,rgt_pts);
    pnts3d = Mat(4,lft_pts.size(),CV_64FC4);
	triangulatePoints(P1,P2, lft_pts, lft_pts, pnts3d);   //pnt3D�ǵ���*4���������pnt3D(2,i)��Z������
}




