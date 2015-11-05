/*********************************************************************************
**********************************************************************************
                          代码说明：兰州项目计算融合模块
                          版本信息：beta 1.0
                          提交时间：2014/8/10
			              作者：    唐书航
***********************************************************************************
*********************************************************************************/

#include "Fusion.h"
#include "LzFilter.h"
#include <iostream>
#include "headdefine.h"
#include "LzException.h"
#include "clearance_item.h"
#include "datastructure.h"
#include "output_heights_list.h"
#include "LzSerialStorageSynthesis.h"
#include "LzSerialStorageMat.h"
#include <opencv2\core\core.hpp>
#include <sstream>

#include "LzCalc_ExtractHeight.h"

// @author 范翔改 继承自QObject 可以通过消息槽反馈计算进度
Fusion::Fusion(QObject* obj) : QObject(obj)
{
    lzMat_A = new LzSerialStorageMat();            //从机A流式存储类
    lzMat_A->setting(250, 1024*1024*50, true);
    lzMat_B = new LzSerialStorageMat();
    lzMat_B->setting(250, 1024*1024*50, true);
    lzMat_C = new LzSerialStorageMat();
    lzMat_C->setting(250, 1024*1024*50, true);
    lzMat_D = new LzSerialStorageMat();
    lzMat_D->setting(250, 1024*1024*50, true);
    lzMat_E = new LzSerialStorageMat();
    lzMat_E->setting(250, 1024*1024*50, true);
    lzMat_F = new LzSerialStorageMat(); 
    lzMat_F->setting(250, 1024*1024*50, true);
    lzMat_G = new LzSerialStorageMat(); 
    lzMat_G->setting(250, 1024*1024*50, true);
    lzMat_H = new LzSerialStorageMat();
    lzMat_H->setting(250, 1024*1024*50, true);
    lzMat_I = new LzSerialStorageMat();
    lzMat_I->setting(250, 1024*1024*50, true);
    lzMat_J = new LzSerialStorageMat();
    lzMat_J->setting(250, 1024*1024*50, true);
    lzMat_K = new LzSerialStorageMat();
    lzMat_K->setting(250, 1024*1024*50, true);
    lzMat_L = new LzSerialStorageMat();
    lzMat_L->setting(250, 1024*1024*50, true);
    lzMat_M = new LzSerialStorageMat();
    lzMat_M->setting(250, 1024*1024*50, true);
    lzMat_N = new LzSerialStorageMat();
    lzMat_N->setting(250, 1024*1024*50, true);
    lzMat_O = new LzSerialStorageMat();
    lzMat_O->setting(250, 1024*1024*50, true);
    lzMat_P = new LzSerialStorageMat();
    lzMat_P->setting(250, 1024*1024*50, true);
    lzMat_Q = new LzSerialStorageRT();
    lzMat_Q->setting(250, 1024*1024*50, true);
    lzMat_R = new LzSerialStorageRT();
    lzMat_R->setting(250, 1024*1024*50, true);

    lzMat_RT = new LzSerialStorageMat();           //RT矩阵流式存储类   RT矩阵前两为平移向量，第三位旋转角度！
    lzMat_RT->setting(250, 1024*1024*50, true);
    lzMat_Out = new LzSerialStorageMat();          //Out输出流式存储类Mat类型
    lzMat_Out->setting(250, 1024*1024*100, true);
    extra_high = new LzSerialStorageSynthesis(); 
    extra_high->setting(250, 1024*1024*50, true);
    ifsuspend = false;

    logger = NULL;
}

Fusion::~Fusion()
{
    delete lzMat_A;
    delete lzMat_B;
    delete lzMat_C;
    delete lzMat_D;
    delete lzMat_E;
    delete lzMat_F;
    delete lzMat_G;
    delete lzMat_H;
    delete lzMat_I;
    delete lzMat_J;
    delete lzMat_K;
    delete lzMat_L;
    delete lzMat_M;
    delete lzMat_N;
    delete lzMat_O;
    delete lzMat_P;
    delete lzMat_Q;
    delete lzMat_R;
    delete lzMat_Out;
    delete extra_high;
}
void Fusion::init(vector<string> load_file)
{
	A_file = load_file.at(0);
	B_file = load_file.at(1);
	C_file = load_file.at(2);
	D_file = load_file.at(3);
	E_file = load_file.at(4);
	F_file = load_file.at(5);
	G_file = load_file.at(6);
	H_file = load_file.at(7);
	I_file = load_file.at(8);
	J_file = load_file.at(9);
	K_file = load_file.at(10);
	L_file = load_file.at(11);
	M_file = load_file.at(12);
	N_file = load_file.at(13);
	O_file = load_file.at(14);
	P_file = load_file.at(15);
    Q_file = load_file.at(16);
    R_file = load_file.at(17);
}

LzSerialStorageMat* Fusion::get_mat(int index)
{
     switch(index)
     {
         case 1: 
             return lzMat_A;
             break;
         case 2:
             return lzMat_B;
             break;
         case 3:
             return lzMat_C;
             break;
         case 4:
             return lzMat_D;
             break;
         case 5:
             return lzMat_E;
             break;
         case 6:
             return lzMat_F;
             break;
         case 7:
             return lzMat_G;
             break;
         case 8:
             return lzMat_H;
             break;
         case 9:
             return lzMat_I;
             break;
         case 10:
             return lzMat_J;
             break;
         case 11:
             return lzMat_K;
             break;
         case 12:
             return lzMat_L;
             break;
         case 13:
             return lzMat_M;
             break;
         case 14:
             return lzMat_N;
             break;
         case 15:
             return lzMat_O;
             break;
         case 16:
             return lzMat_P;
             break;
         case 17:
             return lzMat_Q;
             break;
         case 18:
             return lzMat_R;
             break;
         default:
             break;
     }

}

string Fusion::open_file(int index)
{
     switch(index)
     {
     case 1:
         return A_file;
         break;
     case 2:
         return B_file;
         break;
     case 3:
         return C_file;
         break;
     case 4:
         return D_file;
         break;
     case 5:
         return E_file;
         break;
     case 6:
         return F_file;
         break;
     case 7:
         return G_file;
         break;
     case 8:
         return H_file;
         break;
     case 9:
         return I_file;
         break;
     case 10:
         return J_file;
         break;
     case 11:
         return K_file;
         break;
     case 12:
         return L_file;
         break;
     case 13:
         return M_file;
         break;
     case 14:
         return N_file;
         break;
     case 15:
         return O_file;
         break;
     case 16:
         return P_file;
         break;
     case 17:
         return Q_file;
         break;
     case 18:
         return R_file;
         break;
     default:
         break;
     }
}

Mat& Fusion::get_point(int index)
{ 

    switch (index)
    {
    case 1:
        return point_A;
        break;
    case 2:
        return point_B;
        break;
    case 3:
        return point_C;
        break;
    case 4:
        return point_D;
        break;
    case 5:
        return point_E;
        break;
    case 6:
        return point_F;
        break;
    case 7:
        return point_G;
        break;
    case 8:
        return point_H;
        break;
    case 9:
        return point_I;
        break;
    case 10:
        return point_J;
        break;
    case 11:
        return point_K;
        break;
    case 12:
        return point_L;
        break;
    case 13:
        return point_M;
        break;
    case 14:
        return point_N;
        break;
    case 15:
        return point_O;
        break;
    case 16:
        return point_P;
        break;
    case 17:
        return point_Q;
        break;
    case 18:
        return point_R;
        break;
    default:
        break;
    }
}

void Fusion::get_vector(Mat& mat_point, int cameragroupid, bool carriagedirection)
{
    Point3d point;
    // @author 范翔 车厢正反对x值的影响，使得结果浏览时符合火车真实前行时人的直观左右认知
    int a = 1;
    if (!carriagedirection)
        a = -1;
    if(0 < mat_point.cols)
        for(int i=0; i<mat_point.cols; i++)                          
        {
            point.x = (*mat_point.ptr<float>(0,i));
            //////////TODO TO DELETE////////
            // (-1) 是因为外标定的坐标系与车厢正厢正向坐标系相反，建议下次外标定点测量时采用所规定的车厢正向坐标系
            point.y = a*(-1)*(*mat_point.ptr<float>(1,i));
            point.z = cameragroupid;//*mat_point.ptr<float>(2,i);
            fus_vector.push_back(point);
        }
}

/*void Fusion::get_vector()
{
	Point3d point;
	for(int i=0; i<point_A.cols; i++)                          
	{
		point.x = *point_A.ptr<double>(0,i);
		point.y = *point_A.ptr<double>(1,i);
		point.z = *point_A.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
	for(int i=0; i<point_B.cols; i++)
	{
		point.x = *point_B.ptr<double>(0,i);
		point.y = *point_B.ptr<double>(1,i);
		point.z = *point_B.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
    for(int i=0; i<point_C.cols; i++)
	{
		point.x = *point_C.ptr<double>(0,i);
		point.y = *point_C.ptr<double>(1,i);
		point.z = *point_C.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
    for(int i=0; i<point_D.cols; i++)
	{
		point.x = *point_D.ptr<double>(0,i);
		point.y = *point_D.ptr<double>(1,i);
		point.z = *point_D.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
    for(int i=0; i<point_E.cols; i++)
	{
		point.x = *point_E.ptr<double>(0,i);
		point.y = *point_E.ptr<double>(1,i);
		point.z = *point_E.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
    for(int i=0; i<point_F.cols; i++)
	{
		point.x = *point_F.ptr<double>(0,i);
		point.y = *point_F.ptr<double>(1,i);
		point.z = *point_F.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
    for(int i=0; i<point_G.cols; i++)
	{
		point.x = *point_G.ptr<double>(0,i);
		point.y = *point_G.ptr<double>(1,i);
		point.z = *point_G.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
    for(int i=0; i<point_H.cols; i++)
	{
		point.x = *point_H.ptr<double>(0,i);
		point.y = *point_H.ptr<double>(1,i);
		point.z = *point_H.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
    for(int i=0; i<point_I.cols; i++)
	{
		point.x = *point_I.ptr<double>(0,i);
		point.y = *point_I.ptr<double>(1,i);
		point.z = *point_I.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
    for(int i=0; i<point_J.cols; i++)
	{
		point.x = *point_J.ptr<double>(0,i);
		point.y = *point_J.ptr<double>(1,i);
		point.z = *point_J.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
    for(int i=0; i<point_K.cols; i++)
	{
		point.x = *point_K.ptr<double>(0,i);
		point.y = *point_K.ptr<double>(1,i);
		point.z = *point_K.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
    for(int i=0; i<point_L.cols; i++)
	{
		point.x = *point_L.ptr<double>(0,i);
		point.y = *point_L.ptr<double>(1,i);
		point.z = *point_L.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
    for(int i=0; i<point_M.cols; i++)
	{
		point.x = *point_M.ptr<double>(0,i);
		point.y = *point_M.ptr<double>(1,i);
		point.z = *point_M.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
    for(int i=0; i<point_N.cols; i++)
	{
		point.x = *point_N.ptr<double>(0,i);
		point.y = *point_N.ptr<double>(1,i);
		point.z = *point_N.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
    for(int i=0; i<point_O.cols; i++)
	{
		point.x = *point_O.ptr<double>(0,i);
		point.y = *point_O.ptr<double>(1,i);
		point.z = *point_O.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
	for(int i=0; i<point_P.cols; i++)
	{
		point.x = *point_P.ptr<double>(0,i);
		point.y = *point_P.ptr<double>(1,i);
		point.z = *point_P.ptr<double>(2,i);
		fus_vector.push_back(point);
	}
}*/

bool& Fusion::get_valid(int index)
{
     switch (index)
     {
     case 1:
         return valid_A;
     	 break;
     case 2:
         return valid_B;
         break;
     case 3:
         return valid_C;
         break;
     case 4:
         return valid_D;
         break;
     case 5:
         return valid_E;
         break;
     case 6:
         return valid_F;
         break;
     case 7:
         return valid_G;
         break;
     case 8:
         return valid_H;
         break;
     case 9:
         return valid_I;
         break;
     case 10:
         return valid_J;
         break;
     case 11:
         return valid_K;
         break;
     case 12:
         return valid_L;
         break;
     case 13:
         return valid_M;
         break;
     case 14:
         return valid_N;
         break;
     case 15:
         return valid_O;
         break;
     case 16:
         return valid_P;
         break;
     case 17:
         return valid_Q;
         break;
     case 18:
         return valid_R;
         break;
     }
}

bool & Fusion::get_point_valid(int index)
{
    switch (index)
    {
        case 1:
            return valid_point_A;
            break;
        case 2:
            return valid_point_B;
            break;
        case 3:
            return valid_point_C;
            break;
        case 4:
            return valid_point_D;
            break;
        case 5:
            return valid_point_E;
            break;
        case 6:
            return valid_point_F;
            break;
        case 7:
            return valid_point_G;
            break;
        case 8:
            return valid_point_H;
            break;
        case 9:
            return valid_point_I;
            break;
        case 10:
            return valid_point_J;
            break;
        case 11:
            return valid_point_K;
            break;
        case 12:
            return valid_point_L;
            break;
        case 13:
            return valid_point_M;
            break;
        case 14:
            return valid_point_N;
            break;
        case 15:
            return valid_point_O;
            break;
        case 16:
            return valid_point_P;
            break;
        case 17:
            return valid_point_Q;
            break;
        case 18:
            return valid_point_R;
            break;
    }
}

void Fusion::rectify_RT()                                                 //对三维点向量进行RT矫正
{
	 out_pnts = Mat(3,fus_vector.size(),CV_64FC4);
	 int i = 0;
/*	 for(int j=0; j<3 ;j++)
		 std::cout<<*RT_point.ptr<double>(0,j)<<endl;*/
	 for(Vector<Point3d>::iterator it= fus_vector.begin(); it!=fus_vector.end(); it++)
	 {
		 std::cout<<"原点： " <<"  x: "<<(*it).x<< "  y: " << (*it).y <<endl;
		 *out_pnts.ptr<float>(0,i) = (*it).x*cos(*RT_point.ptr<float>(0,2)/180*3.14159265)-(*it).y*sin(*RT_point.ptr<float>(0,2)/180*3.14159265)+(*RT_point.ptr<float>(0,1));
		 *out_pnts.ptr<float>(1,i) = (*it).y*cos(*RT_point.ptr<float>(0,2)/180*3.14159265)+(*it).x*sin(*RT_point.ptr<float>(0,2)/180*3.14159265)+(*RT_point.ptr<float>(0,0));
		 *out_pnts.ptr<float>(2,i) = (*it).z;
		 (*it).x = *out_pnts.ptr<float>(0,i);
		 (*it).y = *out_pnts.ptr<float>(1,i);
		 (*it).z = *out_pnts.ptr<float>(2,i);
		 std::cout<<"矫正点： " <<"x: "<<(*it).x<< "  y: " << (*it).y <<endl;
		 std::cout<<endl;
		 i++;
	 }
}

void Fusion::loadcalib(string path)
{


    bool ret = fs.open(path, FileStorage::READ);

    if ( !fs.isOpened() )
    {
        // throw exception;
        std::cout<<"Can't open the Calib_file!";
        return;
    }

    // exception~
    // try catch
    fs["stand_pnt_QX"] >> std_Q.x;
    fs["stand_pnt_QY"] >> std_Q.y;
    fs["stand_pnt_RX"] >> std_R.x;
    fs["stand_pnt_RY"] >> std_R.y;
    fs["rail_dist"]   >> rail_dist;                       //定义钢轨间距
    fs.release();
}

void Fusion::Comput_RT(double Q_a, double Q_b, double R_a, double R_b)
{

    double cur_angle,std_angle;                    //定义当前角度和标准角度
    double theta = 0;
    cur_angle = atan2((Q_a-R_a),rail_dist)*180/3.14159265;
    std_angle = atan2((std_Q.x-std_R.x),rail_dist)*180/3.14159265;
    theta = cur_angle - std_angle; 
    *RT_point.ptr<float>(0,0) = -1*(std_R.y - R_b);         //以左轨R作为旋转平移标准
    *RT_point.ptr<float>(0,1) = -1*((R_b/rail_dist)*(Q_a-R_a) - (std_Q.x - std_R.x)/2);
    *RT_point.ptr<float>(0,2) = -1*theta;
    std::cout<<theta<<endl;
    log(QString("Comput_RT: std_Q.x=%1, std_Q.y=%2, std_R.x=%3, std_R.y=%4, Q_x=%5, Q_y=%6, R_x=%7, R_y=%8, theta=%9")
                         .arg(std_Q.x).arg(std_Q.y).arg(std_R.x).arg(std_R.y).arg(Q_a).arg(Q_b).arg(R_a).arg(R_b).arg(theta));
}

void Fusion::init_test()
{
	Point3d Img_pnt;
	int num = 10000;
	double iter = 0.05;
    vector<string> load_file;
	ostringstream file;
	for(int i=1; i<17; i++)
	{
		file << "C:\\Users\\Administrator\\Desktop\\lz数据\\隧道模型1200_20141024_" <<i<<".mdat";
		load_file.push_back(file.str());
		file.str("");
	}
	string RT = "C:\\Users\\Administrator\\Desktop\\Lz数据\\RT_out_file.dat";
	load_file.push_back(RT);
	init(load_file);
}

int Fusion::fuse(list<int> Item, string out_file, string out_extra_high, bool isinterruptfile, __int64 start_num, __int64 frame_cunt, string QRrailcalibfile, string rectifyfile, bool userectifyfactor, bool usesaftyfactor)
{

    initLogger(out_file + ".log", "NPU LanZhou LzFusionThread");
    log("outputfile:" + QString::fromLocal8Bit(out_file.c_str()) + ", " + out_extra_high.c_str());
    log("QRrailcalibfile:" + QString::fromLocal8Bit(QRrailcalibfile.c_str()));
    log("rectifyfile:" + QString::fromLocal8Bit(rectifyfile.c_str()) + ", " + QString("userectifyfactor=%1, usesaftyfactor=%2").arg(userectifyfactor).arg(usesaftyfactor));

    LzCalculate_ExtractHeight * extract = new LzCalculate_ExtractHeight();
    extract->init(Item, "", "", "", rectifyfile, out_extra_high, userectifyfactor, usesaftyfactor);

    ifsuspend = false;
    loadcalib(QRrailcalibfile);
    int ret;
    if (!isinterruptfile)
    {
        bool is_create = lzMat_Out->createFile(out_file.c_str(),sizeof(DataHead));
        bool flag1 = extra_high->createFile(out_extra_high.c_str());
    }
    bool is_open = lzMat_Out->openFile(out_file.c_str());
    bool flag2 = extra_high->openFile(out_extra_high.c_str());
    if (!is_open || !flag2)
    {
        delete extract;
        return 5;
    }

    BlockInfo Info;
    Info.key = start_num;

    // @author 范翔改
    // 记录18组文件是否有数据
    bool hasdata = false;
    DataHead towritedatahead;
    // @author 范翔添加
    carriagedirect = true;
    for(int i=1;i<=18;i++)
    {
        if (i <= 18)
        {
            get_valid(i) = get_mat(i)->openFile(open_file(i).c_str()); 
            log("openfile:" + QString::fromLocal8Bit(open_file(i).c_str()) + ":" + QString("%1").arg(get_valid(i)));
        }
        /*else if (i == 17)
            get_valid(i) = ((LzSerialStorageRT*)get_mat(i))->openFile(open_file(i).c_str());    
        else if (i == 18)
            get_valid(i) = ((LzSerialStorageRT*)get_mat(i))->openFile(open_file(i).c_str());    */

        if (get_valid(i) && !hasdata)
        {
            get_mat(i)->readHead((char*)&towritedatahead);
            carriagedirect = Filter_Fuse_LeftRightCoordinate(towritedatahead.carriage_direction);
            hasdata = true;
        }
        get_point_valid(i) = false;
    }

    // 如果首次创建文件（非暂停后继续），则写入文件头
    if (!isinterruptfile && hasdata) // TODO +RT
    {
        strcpy(towritedatahead.camera_index, "SYN\0");
        towritedatahead.seqno = 0;
        lzMat_Out->writeHead((char*)&towritedatahead);
        extra_high->writeHead(&towritedatahead);
    }
    else
    {
        delete extract;
        return 6; // 没有任何数据
    }
    /*	lzMat_A->openFile(A_file.c_str());                                  //打开文件
	lzMat_B->openFile(B_file.c_str());
	lzMat_C->openFile(C_file.c_str());
	lzMat_D->openFile(D_file.c_str());
	lzMat_E->openFile(E_file.c_str());
	lzMat_F->openFile(F_file.c_str());
	lzMat_G->openFile(G_file.c_str());
	lzMat_H->openFile(H_file.c_str());
	lzMat_I->openFile(I_file.c_str());
	lzMat_J->openFile(J_file.c_str());
	lzMat_K->openFile(K_file.c_str());
	lzMat_L->openFile(L_file.c_str());
	lzMat_M->openFile(M_file.c_str());
	lzMat_N->openFile(N_file.c_str());
	lzMat_O->openFile(O_file.c_str());
	lzMat_P->openFile(P_file.c_str());
	lzMat_R->openFile(R_file.c_str());
	lzMat_T->openFile(T_file.c_str());*/
    for(int i=1;i<=18;i++)
    {
        if(get_valid(i))
        {
            get_valid(i) = get_mat(i)->retrieveBlock(start_num);
        }
    }
/*	lzMat_A->retrieveBlock(start_num);                                 //获得当前文件块
	lzMat_B->retrieveBlock(start_num);
	lzMat_C->retrieveBlock(start_num);
	lzMat_D->retrieveBlock(start_num);
	lzMat_E->retrieveBlock(start_num);
	lzMat_F->retrieveBlock(start_num);
	lzMat_G->retrieveBlock(start_num);
	lzMat_H->retrieveBlock(start_num);
	lzMat_I->retrieveBlock(start_num);
	lzMat_J->retrieveBlock(start_num);
	lzMat_K->retrieveBlock(start_num);
	lzMat_L->retrieveBlock(start_num);
	lzMat_M->retrieveBlock(start_num);
	lzMat_N->retrieveBlock(start_num);
	lzMat_O->retrieveBlock(start_num);
	lzMat_P->retrieveBlock(start_num);
	lzMat_R->retrieveBlock(start_num);                                //找到RT矩阵的起始位置
	lzMat_T->retrieveBlock(start_num);*/
    // TODO TODELETE
    double mile_count = 0;
    RT_point = Mat::zeros(1 ,3 , CV_64FC4);
    
    double Q_a, Q_b, R_a, R_b;

	for(int frame_num = start_num; frame_num < start_num+frame_cunt; frame_num++, mile_count+=1)  // mile_count++ TODO TODELETE
    {
        for(int j = 1; j <= 18; j++)
        {
            if (get_valid(j))
            {
                if (j <= 16)
                    get_point_valid(j) = get_mat(j)->readMat(get_point(j));
                else if (j == 17)
                    get_point_valid(j) = ((LzSerialStorageRT*)get_mat(j))->readMatV2(Q_a, Q_b, get_point(j));
                else if (j == 18)
                    get_point_valid(j) = ((LzSerialStorageRT*)get_mat(j))->readMatV2(R_a, R_b, get_point(j));

                /*				if(j<17)
                for(int i = 0;i<get_point(j).cols;i++)
                {
					 std::cout<<"高度： "<<*get_point(j).ptr<float>(0,i)<<endl;
		             std::cout<<"水平： "<<*get_point(j).ptr<float>(1,i)<<endl;
		             std::cout<<"Z坐标："<<*get_point(j).ptr<float>(2,i)<<endl;
                }*/
            }
        }
/*		lzMat_A->readMat(point_A);                                  //读取17组从机3维点及RT矩阵
		lzMat_B->readMat(point_B);
		lzMat_C->readMat(point_C);
		lzMat_D->readMat(point_D);
		lzMat_E->readMat(point_E);
		lzMat_F->readMat(point_F);
		lzMat_G->readMat(point_G);
		lzMat_H->readMat(point_H);
		lzMat_I->readMat(point_I);
		lzMat_J->readMat(point_J);
		lzMat_K->readMat(point_K);
		lzMat_L->readMat(point_L);
		lzMat_M->readMat(point_M);
		lzMat_N->readMat(point_N);
		lzMat_O->readMat(point_O);
		lzMat_P->readMat(point_P);
		lzMat_R->readMat(R_point);
		lzMat_T->readMat(T_point);*/
        for(int k=1;k<=16;k++)
        {
            if (get_valid(k))
            {
                if (get_mat(k)->getCurrentBlockInfo().isvalid)
                    get_vector(get_point(k), k, carriagedirect);
            }
        }
//		get_vector();                                                  //将17组从机vector放入一个vector中
        if(get_valid(17) && get_valid(18))
        {
			if(lzMat_Q->getCurrentBlockInfo().isvalid&&lzMat_R->getCurrentBlockInfo().isvalid)              //当前帧左右轨点有效
            {

                if(1420<abs(R_b-Q_b)&&abs(R_b-Q_b)<1440)                  //用轨距进行判断是否存在误检测点
                    Comput_RT(Q_a, Q_b, R_a, R_b);
                rectify_RT();                                                  //对vector进行rt矫正
            }
            log(QString("FrameNo=%1, get_valid(17)=%2 && get_valid(18)=%3, Q_a=%4, Q_b=%5, R_a=%6, R_b=%7, CONDITION(1420<abs(R_b-Q_b)&&abs(R_b-Q_b)<1440)=%8")
                            .arg(Info.key).arg(get_valid(17)).arg(get_valid(18)).arg(Q_a).arg(Q_b).arg(R_a).arg(R_b).arg(1420<abs(R_b-Q_b)&&abs(R_b-Q_b)<1440));
        }
        else
        {
            log(QString("FrameNo=%1, get_valid(17)=%2 && get_valid(18)=%3").arg(Info.key).arg(get_valid(17)).arg(get_valid(18)));
        }

        out_pnts = Mat(3,fus_vector.size(),CV_64FC4);                     //需改正
        for(int i=0;i<fus_vector.size();i++)
        {
            out_pnts.at<double>(0,i) = fus_vector[i].x;
            out_pnts.at<double>(1,i) = fus_vector[i].y;
            out_pnts.at<double>(2,i) = fus_vector[i].z;
        }
        // 显示
   /*     for(int i=0; i<out_pnts.cols; i++)
	    {
		    qDebug()<<"第"<<i<<"点坐标："<<endl;
		    qDebug()<<"高度： "<<*out_pnts.ptr<double>(0,i)<<endl;
		    qDebug()<<"水平： "<<*out_pnts.ptr<double>(1,i)<<endl;
		    qDebug()<<"Z坐标："<<*out_pnts.ptr<double>(2,i)<<endl;
		    qDebug()<<endl;

	    }*/

        lzMat_Out->writeMat(out_pnts,Info);                               //将out_pointMat写入

       	BlockInfo tmpblockinfo;
        tmpblockinfo.isvalid = true;
        tmpblockinfo.key = frame_num;

        SectionData Data;
	    bool retb = Data.initMaps();
        float center_height = -1;

        // 【Step3】提取高度
		ret = extract->extract_height(fus_vector, Data, center_height);
        
        // 【Step4】系数校正
        if (userectifyfactor)
        {
            bool retrectify = extract->rectifyHeight(Data, usesaftyfactor);  
            if (!retrectify)
                log(QString("FrameNo=%1, rectifyHeight false").arg(Info.key));
        }

		extra_high->writeMap(frame_num, mile_count, center_height, Data.getMaps(), tmpblockinfo, true);    //参数需要加！！！！
        fus_vector.clear();
        ret = 0;
        if (ret != 0)
        {
            // 终止计算
            lzMat_Out->closeFile();
            extra_high->closeFile();
            delete extract;
            log(QString("FrameNo=%1, extract_height false returnVal=%2").arg(Info.key).arg(ret));
            return ret;
        }
        qDebug() << "fuse " << Info.key;
        // @author 范翔改 继承自QObject 可以通过消息槽反馈计算进度
        emit statusShow(Info.key, false);

        // 是否终止计算，传递当前计算帧号
        if (ifsuspend)
        {
            emit statusShow(Info.key, true);
            // 关闭文件
            for(int i=1;i<=18;i++)
            {
                if(get_valid(i))
                {
                    get_mat(i)->closeFile();
                }
            }
            lzMat_Out->closeFile();
            extra_high->closeFile();
            delete extract;
            log(QString("suspend FrameNo=%1").arg(Info.key));
            return -1; // 暂停计算返回
        }

        Info.key++;
        fus_vector.clear();
	}

    for(int i=1;i<=18;i++)
    {
        if(get_valid(i))
        {
            get_mat(i)->closeFile();
        }
    }
    lzMat_Out->closeFile();
    extra_high->closeFile();
	//system("pause");
    delete extract;
    log(QString("finish all"));
    return ret;
}

/** 
 * 日志类初始化
 */
bool Fusion::initLogger(string filename, string username)
{
    if (logger != NULL)
    {
        if (logger->isLogging())
        {
            logger->log("***************************************************************************");
            logger->log(string("【关闭融合计算日志类】") + filename + string("用户【") + username + string("】"));
            logger->close();
        }
        delete logger;
    }

    logger = new LzLogger(LzLogger::MasterOpt);
    logger->setFilename(filename);
    if (logger->open() == 0)
    {
        hasinitlog = true;
        logger->log("***************************************************************************");
        logger->log(string("【开启融合计算日志类") + filename + string("用户【") + username + string("】"));
        return true;
    }
    else
    {
        hasinitlog = false;
        return false;
    }
}

void Fusion::log(QString msg)
{
    if (hasinitlog)
        logger->log(msg.toLocal8Bit().constData());
}