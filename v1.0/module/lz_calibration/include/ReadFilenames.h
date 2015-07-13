#ifndef READFILENAMES_H
#define READFILENAMES_H

/* ReadFilenames.h
 * 
 * ��;����ȡ����Ŀ¼�������ļ�(Windowsƽ̨)
 * �޸����ݣ�
 *		���� FILETYPE������ѡ���ȡ����Ŀ¼�£�Ŀ¼���ļ��������ļ�
 *		�޸ĺ���ԭ�͵Ĳ���˳�򣬺�׺ string& suffix Ϊ��ѡ����
 *		���Ӻ���ԭ�Ͳ��� enum FILETYPE
 *
 * v1.1 Author Ding(leeleedzy@gmail.com)
 *							2013.10.18
 */

#include<iostream> 
#include<io.h> 

#include <vector>
#include <string>

using namespace std; 

enum FILENAMETYPE { INCLUDEPATH, ONLYNAME };
enum FILETYPE { ONLYDIRECTORY, ONLYFILE, ALL };

void readFilenames(const string& searchdir, vector<string>& output, FILETYPE filetype = ONLYFILE, const string& suffix = "", FILENAMETYPE filenametype = INCLUDEPATH);

#endif