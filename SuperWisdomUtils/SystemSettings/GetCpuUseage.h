#pragma once

#include <Windows.h>

#include <vector>
#include <iostream>
using namespace  std;


class CpuServer
{
public:
	CpuServer(int nPid);
	~CpuServer(){};

  int getWin_CpuUseage();

  int getPid(){return m_nPid;}

  unsigned getWin_CoreCount();
private:
	int m_nCpuUseage;

	int m_nPid;

	//cpu数量
	 int processor_count_ ;
	//上一次的时间
	 __int64 last_time_ ;
	 __int64 last_system_time_ ;
	
};


class Secretary
{
public:  
    void Add(CpuServer ob) { m_observers.push_back(ob); } 
 
    void Remove(int pid);

	bool find(int pid);

	void Remove();

    int Notify();
  
private:  
    vector<CpuServer> m_observers;  
};





class CGetWinInfo
{
public:
	CGetWinInfo();
	~CGetWinInfo();

public:
	/*****************/
	//Cpu使用率
	/*****************/
	double getWin_CpuUseage();


	/*****************/
	//Cpu使用率
	/*****************/
	int getWin_CpuUseage(int pid);

	/*****************/
	//获取windows 内存 使用率
	/*****************/
	DWORD getWin_MemUsage();


	/*****************/
	//获取Cpu最大的使用率
	/*****************/
	DWORD getWin_MaxCpuUseage();

	/*****************/
	//判断CPU 是否超过峰值
	/*****************/
	bool  Check_CpuAvg(int nMax=0);

	/*****************/
	//获取CPU 核数
	/*****************/
	unsigned getWin_CoreCount();

	/*****************/
	//获取剩余内存大小
	/*****************/
	DWORDLONG getWin_FreeMem();

	/*****************/
	//获取剩余内存大小
	/*****************/
	float getWin_FreeMemory();

	/*****************/
	//判断晶核是否打开
	/*****************/
	bool Check_HvmUser();


   
private:
	FILETIME m_preidleTime;
	FILETIME m_prekernelTime;
	FILETIME m_preuserTime;


};