#ifndef LZ_LOGGER_H
#define LZ_LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/**
 * 兰州项目日志类声明，记录各种日志信息
 * @author 范翔
 * @date 2014-04-02
 * @version 1.0.0
 */
class LzLogger
{
public:
	enum LogType{
		CollectCheck = 0, 		// 采集验证，即记录对CheckedFile的各种操作
		FileTransport = 1, 		// 文件传输，主要负责记录Backup的情况
		SynthesisCorrect = 2, 	// 综合修正日志
		MasterOpt = 3,			// 主控操作记录
        SlaveCalcu = 4          // 从控计算日志
	};

	LzLogger(LogType type);
	
	~LzLogger();
	
    /**
     * 外部接口，是否正在写入文件，文件正打开
     */
    bool isLogging();

	/**
     * 文件名（含路径)
     */
	void setFilename(string newfilename);
	
	/**
	 * @return 0 open已有文件成功
	 * 	1 没有已有文件，但创建成功
	 *	2 没有已有文件，且创建文件失败
	 */
	int open();

	void close();

	/**
	 * 日志记录
	 * @param msg a string to log
	 */
	void log(string msg);

private:

	// 文件名（含路径)
	string filename;
	
	// 文件类型
	LogType fileType;
	
	// 文件是否打开
	bool isopen;

    /**
     * 输出流
     */
    ofstream out;
};

#endif // LZ_LOGGER_H