#pragma once

#include <windows.h>

#include "WMIInfo.h"

//
//
//GetSysInfo info;
//int  nversion = info.GetWinOSVersion();
//cout << "CPU 主 频: " << info.GetCpuFreq() << " GHZ" << endl;
//cout << "CPU 型 号: " << info.GetCpuType() << endl;
//cout << "系统名: " << info.GetWinOSName() << endl;
//cout << "显卡名称: " << info.GetDisplayCard() << endl;
//cout << "显卡内存: " << info.GetDisplayRAM() << " G" << endl;
//cout << "显卡频率: " << info.GetDisplayFreq() << " G" << endl;
//cout << "硬盘大小 : " << info.GetDiskTotal() << " G" << endl;
//cout << "内存大小 : " << info.GetMemory() << " G" << endl;
//cout << "内存主频 : " << info.GetMemoryFreq() << "Mhz " << endl;

//操作系统
enum SYSTEMTYPE
{

	SYSTEM_UNKNOWN = 0,       //未知版本
	SYSTEM_XP = 1,       //xp系统
	SYSTEM_XP_64 = 2,       //xp  64系统
	SYSTEM_VISTA_32 = 3,      //vista 32位系统
	SYSTEM_VISTA_64 = 4,    //vista 64位系统
	SYSTEM_WIN7_32 = 5,		//win7 32位系统
	SYSTEM_WIN7_64 = 6,			//win7 64位系统
	SYSTEM_WIN8_32 = 7,		//win8 32位系统
	SYSTEM_WIN8_64 = 8,		//win8 64位系统
	SYSTEM_WIN8_1_32 = 9,		//win8.1 32位系统
	SYSTEM_WIN8_1_64 = 10,		//win8.1 64位系统
	SYSTEM_WIN10_32 = 11,		//win10 32位系统
	SYSTEM_WIN10_64 = 12,		//win10 64位系统
	SYSTEM_SERVER_2003 = 13,	    //winServer2003
	SYSTEM_SERVER_2008 = 14	    //winServer2008

};

class GetSysInfo
{
public:
	GetSysInfo();
	~GetSysInfo();

	BOOL IsWow64();


	//返回系统版本 具体看SYSTEMTYPE的枚举
	int GetWinOSVersion();

	//返回系统名称
	std::string  GetWinOSName();

	// 获取CPU频率,单位: GHZ
	float	GetCpuFreq();

	//CPU 型 号
	std::string GetCpuType();

	// 获取CPU 处理器数目
	int	GetCpuCoresCount();

	// 获取CPU线程数
	float	GetCpuThreadCount();

	//获取内存大小（单位:GB）
	float GetMemory();

	//获取内存主频
	float GetMemoryFreq();

	//硬盘总大小单位：G
	float GetDiskTotal();

	// 获取显卡名称
	std::string	GetDisplayCard();

	//获取显存
	float  GetDisplayRAM();

	//获取显存主频  (未完成)
	float  GetDisplayFreq();


	void GetGPUInfo(std::string &strName, float &fMemorySize , float &fEngineClock, float &fMemoryClock);
public:

	CWmiInfo wmi;
};