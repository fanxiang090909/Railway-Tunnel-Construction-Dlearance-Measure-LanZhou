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

//�޸�����ѡ��·����Ҫ���ҵ��ļ�����
	if((lf = _findfirst(ss.c_str(), &file))!=-1l )//_findfirst���ص���long��;long __cdecl _findfirst(const char *, struct _finddata_t *) 
    { 
        do{
			if ( file.attrib&_A_SUBDIR )
			{//������ļ���Ŀ¼�ļ�
				if (filetype == ONLYFILE)
					continue;
			}
			else
			{//�����Ŀ¼����Ŀ¼�ļ�
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
        } while( _findnext( lf, &file ) == 0 );//int __cdecl _findnext(long, struct _finddata_t *);����ҵ��¸��ļ������ֳɹ��Ļ��ͷ���0,���򷵻�-1 
   
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