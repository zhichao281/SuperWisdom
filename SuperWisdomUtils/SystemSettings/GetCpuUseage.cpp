#include "GetCpuUseage.h"

#include <iostream>

#include <TlHelp32.h>
#include <map>
#include <vector>
#include "../SystemSettings/SystemSettings.h"

#include <stdio.h>
#define SAFEMON_HVMUSERACATION     L"SOFTWARE\\360Safe\\safemon"
#define  CPUMAXUSE  60
using namespace std;

__int64 CompareFileTime ( FILETIME time1, FILETIME time2 )
{
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime ;
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime ;

	return   (b - a);
}


CGetWinInfo::CGetWinInfo()
{
	GetSystemTimes( &m_preidleTime, &m_prekernelTime, &m_preuserTime);
	Sleep(1000);
}


CGetWinInfo::~CGetWinInfo()
{


}

//windows 内存 使用率
DWORD CGetWinInfo::getWin_MemUsage()
{
	MEMORYSTATUS ms;
	::GlobalMemoryStatus(&ms);
	return ms.dwMemoryLoad;
}
//Cpu使用率
double CGetWinInfo::getWin_CpuUseage()
{
	float cpu,cpuidle;
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;
	GetSystemTimes( &idleTime, &kernelTime, &userTime );

	__int64 idle = CompareFileTime( m_preidleTime,idleTime);
	__int64 kernel = CompareFileTime( m_prekernelTime, kernelTime);
	__int64 user = CompareFileTime(m_preuserTime, userTime);

	if(kernel+user == 0)
		return 0.0;

	//（总的时间-空闲时间）/总的时间=占用cpu的时间就是使用率
	cpu = (kernel +user - idle) *100/(kernel+user);
	cpuidle = (idle) *100.0/(kernel+user);
	m_preidleTime = idleTime;
	m_prekernelTime = kernelTime;
	m_preuserTime = userTime ;

	cout<<"CPU："<<cpu<<"%"<<"空闲CPU："<<cpuidle<<"%"<< endl;
	return cpu;
}

unsigned CGetWinInfo::getWin_CoreCount()
{
	unsigned count = 1; // 至少一个

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	count = si.dwNumberOfProcessors;

	return count;
}

//判断CPU 是否超过峰值
bool  CGetWinInfo::Check_CpuAvg(int nMax)
{
	if (nMax == 0)
	{
		nMax = CPUMAXUSE;
	}
	int nResulut=0;
	for(int i=0;i<5;i++)
	{  
		double cpu =getWin_CpuUseage();
		if (nMax  < cpu)
		{
			nResulut++;
		}
	}
	if (nResulut>3)
	{
		return true;
	}
	return false;
}

//获取剩余内存大小
DWORDLONG CGetWinInfo::getWin_FreeMem()
{

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);

	return statex.ullAvailPhys;
}

//获取剩余内存大小
float CGetWinInfo::getWin_FreeMemory()
{

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	float nFreeMen=statex.ullAvailPhys/1024.0/1024.0/1024.0;
	return nFreeMen;
}


//判断晶核是否打开
bool CGetWinInfo::Check_HvmUser()
{
	DWORD dwHvmUserAction ;
	CRegSettings reg(HKEY_LOCAL_MACHINE, SAFEMON_HVMUSERACATION, false, false);
	reg.GetValue(_T("HvmUserAction"), &dwHvmUserAction, 0);
	if (dwHvmUserAction == 1)
	{
		return true;
	}
	else
	{
		return false;
	}

}
/// 时间转换  
static unsigned long long file_time_2_utc(const FILETIME* ftime)  
{  
	LARGE_INTEGER li;  
	li.LowPart = ftime->dwLowDateTime;  
	li.HighPart = ftime->dwHighDateTime;  
	return li.QuadPart;  
}  

int CGetWinInfo::getWin_CpuUseage(int pid)
{  
	//cpu数量
	static int processor_count_ = -1;
	//上一次的时间
	static __int64 last_time_ = 0;
	static __int64 last_system_time_ = 0;

	FILETIME now;
	FILETIME creation_time;
	FILETIME exit_time;
	FILETIME kernel_time;
	FILETIME user_time;
	__int64 system_time;
	__int64 time;
	__int64 system_time_delta;
	__int64 time_delta;

	int cpu = -1;

	if(processor_count_ == -1)
	{
		processor_count_ = (int)getWin_CoreCount();
	}

	GetSystemTimeAsFileTime(&now);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	//计算占用CPU的百分比  
	if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
	{
		return -1;
	}
	system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) 
		/ processor_count_;  
	time = file_time_2_utc(&now);
	//判断是否为首次计算  
	if ((last_system_time_ == 0) || (last_time_ == 0))
	{
		last_system_time_ = system_time;
		last_time_ = time;
		return -1;
	}

	system_time_delta = system_time - last_system_time_;
	time_delta = time - last_time_;

	if (time_delta == 0)
		return -1;

	cpu = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
	last_system_time_ = system_time;
	last_time_ = time;
	return cpu;
}



//获取Cpu最大的使用率
DWORD CGetWinInfo::getWin_MaxCpuUseage()
{
	// 创建通知者  
	Secretary* p = new Secretary();  
	map<int,vector<int>>mapVector;
	while(1)
	{
		HANDLE hProceessnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProceessnap == INVALID_HANDLE_VALUE)
		{
			printf_s("创建进行快照失败\n");
			return -1;
		}
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(pe32);
		BOOL hProcess = Process32First(hProceessnap, &pe32);
		CHAR buff[1024];
		while (hProcess)
		{				
			int nPid=pe32.th32ParentProcessID;
			if (p->find(nPid) == false)
			{
				CpuServer *pPid=new CpuServer(nPid);
				p->Add(*pPid);		
			}					
			memset(buff, 0x00, 1024);
			hProcess = Process32Next(hProceessnap, &pe32);

		}	
		CloseHandle(hProceessnap);
		Sleep(1000);
		// 通知  
		p->Notify(); 

	}
	// 动态删除  
	p->Remove(); 


	return 0;  

}



CpuServer::CpuServer(int nPid)
{
	m_nPid=nPid;

	//cpu数量
	processor_count_ = -1;
	//上一次的时间
	last_time_ = 0;
	last_system_time_ = 0;


	getWin_CpuUseage();
};

int  CpuServer::getWin_CpuUseage()
{

	FILETIME now;
	FILETIME creation_time;
	FILETIME exit_time;
	FILETIME kernel_time;
	FILETIME user_time;
	__int64 system_time;
	__int64 time;
	__int64 system_time_delta;
	__int64 time_delta;

	int cpu = -1;

	if(processor_count_ == -1)
	{
		processor_count_ = (int)getWin_CoreCount();
	}

	GetSystemTimeAsFileTime(&now);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, m_nPid);
	if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
	{
		m_nCpuUseage=-1;
		return m_nCpuUseage;
	}
	system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) 
		/ processor_count_;  
	time = file_time_2_utc(&now);

	if ((last_system_time_ == 0) || (last_time_ == 0))
	{
		last_system_time_ = system_time;
		last_time_ = time;
		m_nCpuUseage=-1;
		return m_nCpuUseage;
	}

	system_time_delta = system_time - last_system_time_;
	time_delta = time - last_time_;

	if (time_delta == 0)
	{
		m_nCpuUseage=-1;
		return m_nCpuUseage;

	}
	cpu = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
	last_system_time_ = system_time;
	last_time_ = time;
	m_nCpuUseage=cpu;
	return m_nCpuUseage;

}

unsigned CpuServer::getWin_CoreCount()
{
	unsigned count = 1; // 至少一个

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	count = si.dwNumberOfProcessors;

	return count;
}

/**************************************************/


bool Secretary::find(int pid)
{
	vector<CpuServer>::iterator it;  
	for (it=m_observers.begin(); it!=m_observers.end(); ++it)  
	{  
		if (pid  ==  it->getPid())
		{
			return true;
		}     
	}  
	return false;
}

void Secretary::Remove(int pid)  
{  
	vector<CpuServer>::iterator it;  
	for (it=m_observers.begin(); it!=m_observers.end(); ++it)  
	{  
		if (pid  ==  it->getPid())
		{
			m_observers.erase(it);
		}  
		if (it==m_observers.end())
		{
			return;
		}	
	}  

}  
void Secretary::Remove()
{
	vector<CpuServer>::iterator it;  
	for (it=m_observers.begin(); it!=m_observers.end(); ++it)  
	{  
		m_observers.erase(it);	
		if (it==m_observers.end())
		{
			return;
		}		   
	} 
}
int Secretary::Notify()  
{  
	map<int ,int> mapCpu;
	vector<int>vecCpu;
	vector<CpuServer>::iterator it;  
	int nMaxCpu=0,nPid=0;
	for (it=m_observers.begin(); it!=m_observers.end(); ++it)  
	{  			
		int  nCpu=it->getWin_CpuUseage();
		if (nCpu>0 && nCpu <100)
		{
			if (nCpu >nMaxCpu)
			{
				nMaxCpu =nCpu;
				nPid=it->getPid();
			}
			
		}			
	}  
	std::cout<< "Max pid="<<nPid <<"    cpu:"<<nMaxCpu<<endl;
	return nPid;
} 