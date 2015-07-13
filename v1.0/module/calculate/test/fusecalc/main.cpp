/*********************************************************************************
**********************************************************************************
                          代码说明：兰州项目计算融合模块
                          版本信息：beta 1.0
                          提交时间：2014/8/10
                          作者：    唐书航
***********************************************************************************
*********************************************************************************/
#include "Fusion.h"
#include "xmlsynthesisheightsfileloader.h"
#include "output_heights_list.h"
void main()
{
	XMLSynthesisHeightsFileLoader * ff3 = new XMLSynthesisHeightsFileLoader("output_heights.xml");
    bool ret = ff3->loadFile();
    delete ff3;
	//std::cout<<ret<<std::endl;
	Fusion *f = new Fusion();
	string out_file = "C:\\Users\\Administrator\\Desktop\\标定数据\\Extract_height_test\\out_file.dat";
	string out_extra_high = "C:\\Users\\Administrator\\Desktop\\标定数据\\Extract_height_test\\out_extra_high.dat";
	vector<string> input_file;
	list<int> Item = OutputHeightsList::getOutputHeightsListInstance()->list();;
	f->init_test();               //供测试时使用
	//f->init(input_file);
	f->fuse(Item,out_file,out_extra_high);
	delete f;
}