#include "Lz_RT_Calc.h"

void Lz_RT_Clac::init( const char* path )
{	
	is_valid = false;
	_calibparas.loadcali(path);
	_calibparas.calcRemap();
	std_P.x = _calibparas.standard_P.at<double>(0,0);
	std_P.y = _calibparas.standard_P.at<double>(0,1);
	_Lem.init( _calibparas.lsize(), 12, 48 ); 
	_Rem.init( _calibparas.rsize(), 12, 48 );
}

void Lz_RT_Clac::calc(Mat &_LEFT, Mat &_RIGHT, Mat &_Ledge, Mat &_Redge,int flag)        //判断相机（R相机标定反了）
{
       	is_valid = false;

	    Mat img_L,img_R;
		rctfndroi(_LEFT, _RIGHT, img_L ,img_R);

		Mat _Lamp, _Ramp;
		argument(_LEFT, _Lamp);
		argument(_RIGHT, _Ramp); 

		GaussianBlur(_Lamp, _Lamp, Size(5,5), 1.5, 1.5);
		GaussianBlur(_Ramp, _Ramp, Size(5,5), 1.5, 1.5);

		_Lem.markedge( _Lamp, _Ledge );
		_Rem.markedge( _Ramp, _Redge );

		Mat _LP,_RP,_LsubP,_RsubP;
		L_points.resize(_Ledge.rows,vector<double>(0));
		R_points.resize(_Redge.rows,vector<double>(0));
		record_L.resize(_Ledge.rows,0);
		record_R.resize(_Redge.rows,0);
		L_opt_ps.resize(_Ledge.rows,-1);
        R_opt_ps.resize(_Redge.rows,-1);
		calccenters(_Ledge, _LEFT, _LP, _LsubP,1);
		calccenters(_Redge, _RIGHT, _RP, _RsubP,2);
     	center_match(L_points,R_points,L_opt_ps,R_opt_ps);             //匹配左右轨点，种子填充算法，L_points为检测出的点，L_opt_ps为配准中心后的点
		//get_rail_ps(_LEFT,L_opt_ps,L_rail_ps);                         //从配准中心点中获得左轨点
		//get_rail_ps(_RIGHT,R_opt_ps,R_rail_ps);                        //从配准中心点中获得右轨点
		calc3Dpts();                                                   //如果当前左右轨点个数不为0，计算轨点三维坐标。
		coordProject(flag);                                                //相机坐标到世界坐标的转换
		rail_pnts_detect(flag);
        //get_rail(_rect_Pnts3d);                                        //获得当前相机的铁轨轨高和轨距。
	    ofstream SaveFile("C:\\Users\\Administrator\\Desktop\\Output.txt");
		for(int i=0; i<_rect_Pnts3d.cols; i++)
	    {
//			SaveFile<<"第"<<i<<"点坐标："<<endl;
			SaveFile<<*_Pnts3d.ptr<float>(0,i)<<","<<*_Pnts3d.ptr<float>(1,i)<<","<<*_Pnts3d.ptr<float>(2,i)<<endl;
//			SaveFile<<"水平： "<<*_rect_Pnts3d.ptr<float>(1,i)<<" 原点:"<<*_Pnts3d.ptr<float>(1,i)<<endl;
//			SaveFile<<"Z坐标："<<*_rect_Pnts3d.ptr<float>(2,i)<<" 原点:"<<*_Pnts3d.ptr<float>(2,i)<<endl;
//			SaveFile<<endl;

		}
		SaveFile.close();
		for(int i=0;i<lft_pts.size();i++)
		{
			if(lft_pts[i].x!=-1)
			   img_L.at<Vec3b>(int(lft_pts[i].y),int(lft_pts[i].x)) = Vec3b(0,255,0);
			//cout<<i<<endl;
		}
		for(int i=0;i<rgt_pts.size();i++)
		{
			if(rgt_pts[i].x!=-1)
			   img_R.at<Vec3b>(int(rgt_pts[i].y),int(rgt_pts[i].x)) = Vec3b(0,255,0);
			//cout<<i<<endl;
		}
		/*imwrite("C:\\Users\\Administrator\\Desktop\\标定数据_1127\\代码_TSH_1125\\兰州代码11_13\\Lz_Calc\\L1.bmp",img_L);
		imwrite("C:\\Users\\Administrator\\Desktop\\标定数据_1127\\代码_TSH_1125\\兰州代码11_13\\Lz_Calc\\R1.bmp",img_R);
        imwrite("L1.bmp",_LP);
		imwrite("R1.bmp",_RP);*/
 		L_opt_ps.clear();
		R_opt_ps.clear();
		L_points.clear();
		R_points.clear();
		single_seed.clear();
		record_L.clear();
		record_R.clear();
	    lft_pts.clear();
	    rgt_pts.clear();
		R_rail_ps.clear();
		L_rail_ps.clear();

}

void Lz_RT_Clac::get_rail(Mat& rect_pnts3d)                           //获得轨面高度和水平距离
{
	double r_h = 0;
	double r_v = 0;
	if(60 <= rect_pnts3d.cols)
	{
	   for(int i=0; i<25;i++)
		   r_h += rect_pnts3d.at<float>(0,i);
	   r_h /= 25;
	   for(int i = (rect_pnts3d.cols-15); i<rect_pnts3d.cols ; i++)
		   r_v += rect_pnts3d.at<float>(1,i);
	   r_v /= 15;
	   rail_P.x = r_h;
	   rail_P.y = r_v;
	}
}

void Lz_RT_Clac::calccenters(Mat& _edge, Mat& _RctfImg, Mat& _P, Mat& _subP,int flag)
{
	// create _P, _subP
	_P.create(_edge.size(), CV_8U);
	_subP.create(_edge.size(), CV_32F);
	
//#define _LEFTEDGE 192
//#define _RIGHTEDGE 248
	const uchar _LEFTEDGE = 192;
	const uchar _RIGHTEDGE = 248;
	const int _thresh = 50;

	// init access pointer
	uchar *_ed, *_rc, *_pp;
	float *_sbp;
	const int step = _edge.cols;
	_ed = _edge.ptr<uchar>();
	_rc = _RctfImg.ptr<uchar>();
	_pp = _P.ptr<uchar>();
	_sbp = _subP.ptr<float>();

	// int lower;
	int lower = -1024;

	int rows = _edge.rows;
	int cols = _edge.cols;
	for (int i=0; i< rows; ++i, _ed+=step, _rc+=step, _pp+=step, _sbp+=step)
	{
		lower = -1024;
		for (int j=0; j<cols; ++j)
		{
			if ( _ed[j] == _LEFTEDGE )
				lower = j;
			else if ( _ed[j] == _RIGHTEDGE )
			{
				if ( j - lower < _thresh )
				{
					int c = assigncenters( lower, j ,_rc, _pp, _sbp);
					if(c!=-1)
					{
					if(flag==1)
						L_points[i].push_back(_sbp[c]);
					else
						R_points[i].push_back(_sbp[c]);
					}
				}
				lower = -1024;
			}
		}
	}

}

void Lz_RT_Clac::fill_img_front(int _begin,int _end)
{
	float _L_center,_R_center;
	_L_center = L_opt_ps[_end+6];
	_R_center = R_opt_ps[_end+6];
	float L_min=1000,R_min = 1000;
	int L_index,R_index;
	bool L_flag=true,R_flag=true;
	for(int i=_end; i>_begin; i--)
	{
		if(0==record_L[i]){
	    if(1==L_points[i].size()&&1==R_points[i].size())
		{
			if(abs(_L_center-L_points[i][0])<3)
			{
			L_opt_ps[i] = L_points[i][0];
			_L_center = L_points[i][0];
			record_L[i] = 1;
			}

		}
		else if(1<L_points[i].size())
		{
			for(int j=0;j<L_points[i].size();j++)
			{
				if(abs(_L_center-L_points[i][j])<L_min)
				{
					L_index = j;
					L_min = abs(_L_center-L_points[i][j]);
				}
			}
			if(L_min<3&&L_flag)
			{
			L_opt_ps[i] = L_points[i][L_index];
			_L_center = L_opt_ps[i];
			record_L[i] = 1;
			}
			else
			{
				L_flag = false;
			}

		}
		}
		if(0==record_R[i]){
	    if(1==L_points[i].size()&&1==R_points[i].size())
		{
			if(abs(_R_center-R_points[i][0])<3)
			{
			R_opt_ps[i] = R_points[i][0];
  		    _R_center = R_points[i][0];
			record_R[i] = 1;
			}

		}
		else if(1<R_points[i].size())
		{
			for(int j=0;j<R_points[i].size();j++)
			{
				if(abs(_R_center-R_points[i][j])<R_min)
				{
					R_index = j;
					R_min = abs(_R_center-R_points[i][j]);
				}
			}
   	  	    if(R_min<3&&R_flag)
			{
			R_opt_ps[i] = R_points[i][R_index];
			_R_center = R_opt_ps[i];
			record_R[i] = 1;
			}
			else
			{
				R_flag = false;
			}
		}
		L_min=1000;
		R_min = 1000;
		}

	}
}

void Lz_RT_Clac::fill_img_back(int _begin,int _end)
{
	float _L_center,_R_center;
	_L_center = L_opt_ps[_begin-1];
	_R_center = R_opt_ps[_begin-1];
	float L_min = 1000,R_min = 1000;
	int L_index,R_index;
	bool L_flag=true,R_flag=true;
	for(int i=_begin; i<_end; i++)
	{
		if(record_L[i]==0)
		{	
			if(i==1000)
			{
				std::cout<<""<<endl;
			}
	    if(1==L_points[i].size()&&1==R_points[i].size())
		{
			if(abs(_L_center-L_points[i][0])<3)
			{
			  L_opt_ps[i] = L_points[i][0];
			  _L_center = L_points[i][0];
			  record_L[i] = 1;
			}
		}
		else if(1<=L_points[i].size())
		{
			for(int j=0;j<L_points[i].size();j++)
			{
				if(abs(_L_center-L_points[i][j])<L_min)
				{
					L_index = j;
					L_min = abs(_L_center-L_points[i][j]);
				}
			}
			if(L_min<3&&L_flag)
			{
			L_opt_ps[i] = L_points[i][L_index];
			_L_center = L_opt_ps[i];
			record_L[i] = 1;
			}
			else
			{
				L_flag = false;
			}

		}
		}
		if(record_R[i]==0)
		{
	    if(1==L_points[i].size()&&1==R_points[i].size())
		{
			if(abs(_R_center-R_points[i][0])<3)
			{
			R_opt_ps[i] = R_points[i][0];
			_R_center = R_points[i][0];
			record_R[i] = 1;
			}

		}
		else if(1<=R_points[i].size()&&L_flag)
		{
			for(int j=0;j<R_points[i].size();j++)
			{
				if(abs(_R_center-R_points[i][j])<R_min)
				{
					R_index = j;
					R_min = abs(_R_center-R_points[i][j]);
				}
			}
			if(R_min<3&&R_flag)
			{
			R_opt_ps[i] = R_points[i][R_index];
			_R_center = R_opt_ps[i];
			record_R[i] = 1;
			}
			else
			{
				R_flag = false;
			}
		}
		L_min = 1000;
		R_min = 1000;
		}
	}
}

void Lz_RT_Clac::seed_fill(vector<Point3i>& seed)
{
	for(int i=0; i<seed.size()-2 ;i++)
	{
		fill_img_front(seed[i].y,seed[i+1].x);
		fill_img_back(seed[i+1].y,seed[i+2].x);
	}
}

void Lz_RT_Clac::pnts_add()
{
	for(int i=0;i<L_opt_ps.size();i++)
	{
		if(0==record_L[i]||0==record_R[i])
		{
			if(L_points[i].size()==2&&R_points[i].size()==2)
			{
				if(abs(L_points[i][0]-L_points[i][1])<15&&abs(R_points[i][0]-R_points[i][1])<15)
				{
					L_opt_ps[i] = (L_points[i][0]+L_points[i][1])/2;
					R_opt_ps[i] = (R_points[i][0]+R_points[i][1])/2;
				}
			}
		}
	}
}

void Lz_RT_Clac::center_match(vector<vector<double>> &L_points,vector<vector<double>> &R_points,vector<double>& L_opt_ps,vector<double>& R_opt_ps)
{
	int _L_index[2000]={0};     //0表示该行没有点，1表示该行唯一点，-1表示该行多点！
	int _R_index[2000]={0};
	int seed_begin = 0;         //用于计数填充种子的起始和结束行号。//
	int seed_end   = 0; 
	float center_L,center_R;
	bool is_calc = false;
	int seed_cunt = 0;
	Point3i seed;
	single_seed.push_back(Point3i(0,0,0));
	for(int i=0;i<L_opt_ps.size();i++)
	{
/*	  if(R_points[i].size()==1&&L_points[i].size()==1)
		{
			_L_index[i] = 1;
			L_opt_ps[i] = L_points[i][0];
	//		record[i] = 1;
		}
		else if(L_points[i].size()==0)
		{
			_L_index[i] = 0;
		}
		else
		{
			_L_index[i] = -1;
		}
	  if(R_points[i].size()==1&&L_points[i].size()==1)
		{
			_R_index[i] = 1;
			R_opt_ps[i] = R_points[i][0];
		}
		else if(R_points[i].size()==0)
		{
			_R_index[i] = 0;
		}
		else
		{
			_R_index[i] = -1;
		}*/
		if(L_points[i].size()==1&&R_points[i].size()==1&&is_calc==false)
		{
			if(is_calc==false)
			{
				is_calc = true;
				seed_begin = i;
			}
			seed_cunt++;
		}
		else if(is_calc==true&&L_points[i].size()==1&&R_points[i].size()==1&&abs(L_points[i][0]-L_points[i-1][0])<1&&abs(R_points[i][0]-R_points[i-1][0])<1)
		{
			seed_cunt++;
//			std::cout<<i<<endl;
		}
		else
		{
			if(is_calc==true)
			{
				if(20<seed_cunt)
				{
					seed_end = i;
					seed.x = seed_begin;
					seed.y = seed_end;
					seed.z = seed_cunt;
					single_seed.push_back(seed);
				}
	//			std::cout<<seed_cunt<<endl;
				seed_cunt  = 0;
				is_calc = false;

			}
			
		}
	}
	single_seed.push_back(Point3i(L_points.size(),-1,0));
/*	for(int i=0;i<L_opt_ps.size();i++)
	{
		int min = 10000;
		int min_index = 0;
		if((_R_index[i]==1&&_L_index[i]==1)||_R_index[i]==0||_L_index[i]==0)
			continue;
		else
		{
			if((0<i)&&(_L_index[i]==-1&&(_L_index[i-1])!=0))
			{
				for(int j=0;j<L_points[i].size();j++)
				{
					if(L_points[i][j]-L_points[i-1][0]<min)
					{
						min = L_points[i][j]-L_points[i-1][0];
						min_index = j;
					}
				}
				L_opt_ps[i] = L_points[i][min_index];
			}
			min = 1000;
			if((0<i)&&(_R_index[i]==-1)&&(_R_index[i-1]!=0))
			{
				for(int j=0;j<R_points[i].size();j++)
				{
					if(R_points[i][j]-R_points[i-1][0]<min)
					{
						min = R_points[i][j]-R_points[i-1][0];
						min_index = j;
					}
				}
				R_opt_ps[i] = R_points[i][min_index];
			}
		}
	}*/
	if(2<single_seed.size())
	{
		for(int i=1;i<single_seed.size()-1;i++)
		{
			for(int j=single_seed[i].x;j<single_seed[i].y;j++)
			{
				L_opt_ps[j] = L_points[j][0];
				R_opt_ps[j] = R_points[j][0];
			}
		}
        seed_fill(single_seed);
	}
	pnts_add();                     //用于加入不确定点对。
}

void Lz_RT_Clac::get_rail_ps(Mat &img,vector<double> &pnts,vector<double> &rail_pfs)
{
	int min_dis = 5;                                                          //最小值钢轨判断界限
	int max_dis = 25;                                                         //最大值
	double rail_point = 0;                                                    //记录钢轨横坐标
	int rail_p_cunt = 0;                                                      //用于记录钢轨点的个数
	bool inital = true;
	bool find_p = false;
	bool Is_end = true;
	rail_pfs.resize(img.rows,-1);
	for(int i=450; i<1050; i++)                                                //光带位于图像高度400-700之间
	{
	    //前面已获得条带光的中心，循环内求取铁轨平面
		if(pnts[i]!=-1)
		 {
			    // img.at<uchar>(i,cvRound(*it)) = 0;
				 if(inital)                                                   //对钢轨起始点进行判断，初始化。
				 {
					 rail_point = pnts[i];
					 inital = false;
				 }
				 else if(!find_p)
				 {
					 if(abs(pnts[i]-rail_point)<7)
					 {
						 rail_point = pnts[i];
					 }
					 else if(abs(pnts[i]-rail_point)<100)
					 {
//						 std::cout<<"Max:"<<abs(cvRound(*it)-rail_point)<<std::endl;
						 rail_point = pnts[i];
						 find_p = true;
					 }
				 }
				 if(find_p&&rail_p_cunt<100&&abs(pnts[i]-rail_point)<3)           //假如属于钢轨内部且与上一轨点相差3像素则依然是轨点
		         {
					 rail_pfs[i] = pnts[i];
//					 img.at<uchar>(i,pnts[i]) = 0;
//					 std::cout<<i<<endl;
					 rail_point = pnts[i];
					 rail_p_cunt++;
		         }
    			 else if(find_p&&Is_end)
				 {
//					 std::cout<<"Min:"<<abs(cvRound(*it)-rail_point)<<std::endl;
					 Is_end = false;
					 return;
				 }		
		}
	  
	}
}

void Lz_RT_Clac::rail_pnts_detect(int flag)
{
    int rail_start;             //用于返回钢轨起始位置
	int rail_end;               
	float height_max = -1;           //定义范围内最高点
	bool inital_start = false;
	bool inital_end   = false;
	bool init_rail = false;          //定义确定钢轨的起始点
	float rail_H = 0;
	float rail_W = 0;
	Point2f temp;
	for(int i=0;i<_rect_Pnts3d.cols;i++)
	{
		if(1==flag)
		{
            if(!inital_start&&*_rect_Pnts3d.ptr<float>(1,i)<=RAIL_PNTS_Max&&*_rect_Pnts3d.ptr<float>(0,i)<RAIL_H_Max)
		    {
			   rail_start = i;
			   inital_start = true;
		    }
		    if(!inital_end&&*_rect_Pnts3d.ptr<float>(1,i)<=RAIL_PNTS_Min&&*_rect_Pnts3d.ptr<float>(0,i)<RAIL_H_Max)
		    {
			   rail_end = i;
			   inital_end = true;
		    }
		    if(RAIL_PNTS_Min<=*_rect_Pnts3d.ptr<float>(1,i)&&*_rect_Pnts3d.ptr<float>(1,i)<=RAIL_PNTS_Max&&*_rect_Pnts3d.ptr<float>(0,i)<RAIL_H_Max)
		    {
			    if(*_rect_Pnts3d.ptr<float>(0,i)>height_max)
				   height_max = *_rect_Pnts3d.ptr<float>(0,i);
		    }
	   }
	   else
	   {
		   	if(!inital_start&&*_rect_Pnts3d.ptr<float>(1,i)>=-RAIL_PNTS_Max&&*_rect_Pnts3d.ptr<float>(0,i)<RAIL_H_Max)
		    {
			   rail_start = i;
			   inital_start = true;
		    }
		    if(!inital_end&&*_rect_Pnts3d.ptr<float>(1,i)>=-RAIL_PNTS_Min&&*_rect_Pnts3d.ptr<float>(0,i)<RAIL_H_Max)
		    {
			   rail_end = i;
			   inital_end = true;
		    }
		    if(-RAIL_PNTS_Min>=*_rect_Pnts3d.ptr<float>(1,i)&&*_rect_Pnts3d.ptr<float>(1,i)>=-RAIL_PNTS_Max&&*_rect_Pnts3d.ptr<float>(0,i)<RAIL_H_Max)
		    {
			    if(*_rect_Pnts3d.ptr<float>(0,i)>height_max)
				   height_max = *_rect_Pnts3d.ptr<float>(0,i);
			}
	   }
	}
	for(int j=rail_start;j<rail_end;j++)
	{
		if((height_max-*_rect_Pnts3d.ptr<float>(0,j))<2||init_rail&&(height_max-*_rect_Pnts3d.ptr<float>(0,j))<50)
		{
			temp.x = *_rect_Pnts3d.ptr<float>(0,j);
			temp.y = *_rect_Pnts3d.ptr<float>(1,j);
			*_rect_Pnts3d.ptr<float>(2,j) = -1;
			record.push_back(temp);
			init_rail = true;
		}
	}
	if(record.size()<=60)
	{
		cout<<"no rail"<<endl;
	}
	else
	{
		for(int i=0;i<30;i++)
		{
			rail_H += record[i].x;
		}
		rail_H /= 30;
		for(int i=record.size()-1;i>record.size()-1-20;i--)
		{
			rail_W += record[i].y;
		}
		rail_W /= 20;
		rail_P.x = rail_H;
		rail_P.y = rail_W;
	}
	record.clear();

}