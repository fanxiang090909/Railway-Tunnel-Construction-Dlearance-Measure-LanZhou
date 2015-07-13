//#define MAIN_TEST

#include "ReadFilenames.h"

void readFilenames( const string& searchdir, vector<string>& output, FILETYPE filetype, const string& suffix, FILENAMETYPE filenametype )
{
	_finddata_t file; 
    long lf; 
	string ss(searchdir);
	ss.append("/*");
	ss.append(suffix.begin(), suffix.end());

	output.clear();

//修改这里选择路径和要查找的文件类型
	if((lf = _findfirst(ss.c_str(), &file))!=-1l )//_findfirst返回的是long型;long __cdecl _findfirst(const char *, struct _finddata_t *) 
    { 
        do{
			if ( file.attrib&_A_SUBDIR )
			{//如果该文件是目录文件
				if (filetype == ONLYFILE)
					continue;
			}
			else
			{//如果该目录不是目录文件
				if ( filetype == ONLYDIRECTORY )
					continue;
			}
			
			if (filenametype == ONLYNAME)
			{
				output.push_back(file.name);
			}
			else if (filenametype == INCLUDEPATH)
			{
				string completename;
				completename.append(searchdir);
				completename.append("//");
				completename.append(file.name);
				output.push_back(completename);
			}
        } while( _findnext( lf, &file ) == 0 );//int __cdecl _findnext(long, struct _finddata_t *);如果找到下个文件的名字成功的话就返回0,否则返回-1 
   
    } 
    _findclose(lf); 

	return;
}

#ifdef MAIN_TEST 

int _tmain(int argc, _TCHAR* argv[])
{
	vector<string> svec;
	
	readFilenames("C:\\Users\\dingzhiyu\\Desktop\\20130820\\",".bmp",svec);

	return 0;
}

#endif