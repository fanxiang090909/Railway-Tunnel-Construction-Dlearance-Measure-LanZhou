/*/////////////////////////////////////////////////////////
*
*	LzException.h
*	兰州隧道项目  异常定义
*
*	版本：	alpha v1.0
*	作者：	Ding (leeleedzy@gmail.com)
*	日期：	2014.1.18
*
*//////////////////////////////////////////////////////////

#ifndef LZ_EXCEPTION_H
#define LZ_EXCEPTION_H

#include <exception>
#include <sstream>
#include <string>


class LzException : public std::exception
{
public:
	LzException( int id, const char* des ) { 
		setErrId(id);
		setErrDescribe(des); }
	LzException( LzException& ex ) 
	{
		this->errorid = ex.errorid;
		this->str = ex.str;
	}

	virtual const char* what() const throw() {
		std::stringstream sstr;
		sstr.str("");
		sstr << errorid;
		sstr << "_";
		sstr << str;

		const char* tt = sstr.str().c_str();
		return tt;
	}

	void setErrId(int id) { errorid = id; }
	void setErrDescribe( const char* pt ) { str = pt; }

	int getErrId() { return errorid; }
	std::string getErrDescribe() { return str; }

private:
	int errorid;
	std::string str;
};

#endif