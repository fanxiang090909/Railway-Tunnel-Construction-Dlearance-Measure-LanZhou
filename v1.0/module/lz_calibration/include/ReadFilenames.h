#ifndef READFILENAMES_H
#define READFILENAMES_H

/* ReadFilenames.h
 * 
 * 用途：读取给定目录下所有文件(Windows平台)
 * 修改内容：
 *		增加 FILETYPE，允许选择读取给定目录下：目录、文件、所有文件
 *		修改函数原型的参数顺序，后缀 string& suffix 为可选参数
 *		增加函数原型参数 enum FILETYPE
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