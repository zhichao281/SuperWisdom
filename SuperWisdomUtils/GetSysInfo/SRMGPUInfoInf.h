#pragma once

/************************************************************************/
/* 显卡数据读取接口定义                                                                     */
/************************************************************************/

#include <string>
#include <windows.h>
using namespace std;

class SRMGPUInfoInf
{
public:
	//获取显卡的频率
	virtual long getGPUEngineClock() = 0;

	//获取显存频率
	virtual long  getGPUMemoryClock() = 0;

	//获取显存
	virtual float   GetGPUMemorySize() = 0;

	//获取显卡名称
	virtual  std::string getGPUName() = 0;


};

