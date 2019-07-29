#include "GetSysInfo.h"

#include "SRMGPUInfoInf.h"
#include "SRMATIGPUInfo.h"
#include "SRMNvidiaGPUInfo.h"

#ifdef __cplusplus  

#define VERSIONHELPERAPI inline bool  

#else  // __cplusplus  

#define VERSIONHELPERAPI FORCEINLINE BOOL  

#endif // __cplusplus  

#define _WIN32_WINNT_NT4                    0x0400  
#define _WIN32_WINNT_WIN2K                  0x0500  
#define _WIN32_WINNT_WINXP                  0x0501  
#define _WIN32_WINNT_WS03                   0x0502  
#define _WIN32_WINNT_WIN6                   0x0600  
#define _WIN32_WINNT_VISTA                  0x0600  
#define _WIN32_WINNT_WS08                   0x0600  
#define _WIN32_WINNT_LONGHORN               0x0600  
#define _WIN32_WINNT_WIN7                   0x0601  
#define _WIN32_WINNT_WIN8                   0x0602  
#define _WIN32_WINNT_WINBLUE                0x0603  
#define _WIN32_WINNT_WIN10                  0x0A00  

using namespace std;

VERSIONHELPERAPI
IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
	RTL_OSVERSIONINFOEXW verInfo = { 0 };
	verInfo.dwOSVersionInfoSize = sizeof(verInfo);
	typedef LONG(__stdcall *fnRtlGetVersion)(PRTL_OSVERSIONINFOW);
	//typedef NTSTATUS(*fnRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);  
	static auto RtlGetVersion = (fnRtlGetVersion)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");
	//static auto RtlGetVersion = (fnRtlGetVersion)GetProcAddress( GetModuleHandleW( L"ntdll.dll" ), "RtlGetVersion" );  

	if (RtlGetVersion != 0 && RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo) == 0)
	{
		if (verInfo.dwMajorVersion > wMajorVersion)
			return true;
		else if (verInfo.dwMajorVersion < wMajorVersion)
			return false;

		if (verInfo.dwMinorVersion > wMinorVersion)
			return true;
		else if (verInfo.dwMinorVersion < wMinorVersion)
			return false;

		if (verInfo.wServicePackMajor >= wServicePackMajor)
			return true;
	}

	return false;
}

VERSIONHELPERAPI
IsWindowsXPOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 0);
}

VERSIONHELPERAPI
IsWindowsXPSP1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 1);
}

VERSIONHELPERAPI
IsWindowsXPSP2OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 2);
}

VERSIONHELPERAPI
IsWindowsXPSP3OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 3);
}

VERSIONHELPERAPI
IsWindowsVistaOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
}

VERSIONHELPERAPI
IsWindowsVistaSP1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 1);
}

VERSIONHELPERAPI
IsWindowsVistaSP2OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 2);
}

VERSIONHELPERAPI
IsWindows7OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0);
}

VERSIONHELPERAPI
IsWindows7SP1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 1);
}

VERSIONHELPERAPI
IsWindows8OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0);
}

VERSIONHELPERAPI
IsWindows8Point1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINBLUE), LOBYTE(_WIN32_WINNT_WINBLUE), 0);
}

VERSIONHELPERAPI
IsWindows10OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN10), LOBYTE(_WIN32_WINNT_WIN10), 0);
}


VERSIONHELPERAPI
IsWindowsServer()
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, { 0 }, 0, 0, 0, VER_NT_WORKSTATION };
	DWORDLONG        const dwlConditionMask = VerSetConditionMask(0, VER_PRODUCT_TYPE, VER_EQUAL);

	return !VerifyVersionInfoW(&osvi, VER_PRODUCT_TYPE, dwlConditionMask);
}




GetSysInfo::GetSysInfo()
{
	wmi.InitWmi();
}

GetSysInfo::~GetSysInfo()
{
	wmi.ReleaseWmi();

}

BOOL GetSysInfo::IsWow64()
{
	BOOL bIsWow64 = FALSE;

	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			bIsWow64 = FALSE;
		}
	}
	return bIsWow64;
}

//返回系统版本
int GetSysInfo::GetWinOSVersion()
{

	bool bIs64 = IsWow64();
	do {
		if (IsWindowsServer()) {
			return SYSTEM_SERVER_2008;//winserver
		}
		if (IsWindows10OrGreater()) {
			return bIs64 ? SYSTEM_WIN10_64 : SYSTEM_WIN10_32;//win1064 : win1032
		}
		if (IsWindows8Point1OrGreater()) {
			return bIs64 ? SYSTEM_WIN8_1_64 : SYSTEM_WIN8_1_32;//win8164 : win8132
		}
		if (IsWindows8OrGreater()) {
			return bIs64 ? SYSTEM_WIN8_64 : SYSTEM_WIN8_32;//win864 : win832
		}
		if (IsWindows7SP1OrGreater() || IsWindows7OrGreater()) {
			return bIs64 ? SYSTEM_WIN7_64 : SYSTEM_WIN7_32;//win764 : win732
		}
		if (IsWindowsVistaSP2OrGreater() || IsWindowsVistaSP1OrGreater() || IsWindowsVistaOrGreater()) {
			return bIs64 ? SYSTEM_VISTA_64 : SYSTEM_VISTA_32;//vista64 : vista32
		}
		if (IsWindowsXPSP3OrGreater()) {
			return SYSTEM_XP;//xp sp3 32
		}
		if (IsWindowsXPSP2OrGreater() || IsWindowsXPSP1OrGreater() || IsWindowsXPOrGreater()) {
			return bIs64 ? SYSTEM_XP_64 : SYSTEM_XP;//xp64 : xp
		}
	} while (false);

	return SYSTEM_UNKNOWN;//unknown
}

//返回系统版本
std::string  GetSysInfo::GetWinOSName()
{
	float nRam = 0;
	std::string szProductName = "";
	bool bres = wmi.GetSingleItemInfo("Win32_OperatingSystem", "Caption", szProductName);
	return szProductName;

}

//CPU 型 号
string GetSysInfo::GetCpuType()
{
	std::string szProductName = "";
	bool bres = wmi.GetSingleItemInfo("Win32_Processor", "Name", szProductName);
	return szProductName;
}


// 获取CPU频率,单位: GHZ
float GetSysInfo::GetCpuFreq()
{
	float nSpeed = 0;
	std::string chRetValue;
	bool bres = wmi.GetSingleItemInfo("Win32_Processor", "CurrentClockSpeed", chRetValue);
	if (bres)
	{
		nSpeed = atof(chRetValue.c_str()) / 1000;
	}
	return nSpeed;
}

// 获取CPU 处理器数目
int GetSysInfo::GetCpuCoresCount()
{
	int nCount = 0;
	std::string chRetValue;
	bool bres = wmi.GetSingleItemInfo("Win32_Processor", "NumberOfCores", chRetValue);
	if (bres)
	{
		nCount = atoi(chRetValue.c_str());
	}
	return nCount;
}

// 获取CPU线程数
float GetSysInfo::GetCpuThreadCount()
{
	float nCount = 0;
	std::string chRetValue;
	bool bres = wmi.GetSingleItemInfo("Win32_Processor", "ThreadCount", chRetValue);
	if (bres)
	{
		nCount = atof(chRetValue.c_str());
	}
	return nCount;
}







//硬盘总大小单位：G
float GetSysInfo::GetDiskTotal()
{

	float all_Total = 0;
	std::string chRetValue;

	bool bres = wmi.GetSingleItemInfo("Win32_DiskDrive", "Size", chRetValue);
	if (bres)
	{
		all_Total = atof(chRetValue.c_str()) / 1024 / 1024 / 1024.0;
	}
	return all_Total;
}

//获取内存大小（单位:GB）
float GetSysInfo::GetMemory()
{
	float nTotal = 0;
	std::vector<std::string> vecRetValue;
	bool bres = wmi.GetSingleItemInfo("Win32_PhysicalMemory", "Capacity", vecRetValue);
	if (bres)
	{
		for (std::string chRetValue : vecRetValue)
		{
			nTotal += atof(chRetValue.c_str()) / 1024 / 1024 / 1024.0;
		}

	}
	return nTotal;

}

//获取内存主频
float GetSysInfo::GetMemoryFreq()
{
	float nFreq = 0;
	std::string chRetValue;
	bool bres = wmi.GetSingleItemInfo("Win32_PhysicalMemory", "Speed", chRetValue);
	if (bres)
	{
		nFreq = atof(chRetValue.c_str()) / 1.0;
	}
	return nFreq;

}



// 获取显卡名称
std::string GetSysInfo::GetDisplayCard()
{

	float nRam = 0;
	std::string chRetValue = "";

	bool bres = wmi.GetSingleItemInfo("win32_displayconfiguration", "DeviceName", chRetValue);
	if (chRetValue.length() < 1)
	{
		bres = wmi.GetSingleItemInfo("Win32_VideoController", "Description", chRetValue);
	}


	return chRetValue;
}

//获取显存
float GetSysInfo::GetDisplayRAM()
{
	float nRam = 0;

	std::string chRetValue = "";

	bool bres = wmi.GetSingleItemInfo("win32_displayconfiguration", "DeviceName", chRetValue);
	if (chRetValue.length() < 1)
	{
		bres = wmi.GetSingleItemInfo("Win32_VideoController", "Description", chRetValue);
	}


	std::string strSql = "Win32_VideoController where caption='" + chRetValue + "'";
	chRetValue = "";

	bres = wmi.GetSingleItemInfo("Win32_VideoController ", "AdapterRAM", chRetValue);
	if (bres)
	{
		nRam = atof(chRetValue.c_str()) / 1024 / 1024 / 1024.0;
	}
	return nRam;

}

//获取频率
float GetSysInfo::GetDisplayFreq()
{
	float nRam = 0;
	return nRam;

}


void GetSysInfo::GetGPUInfo(std::string &strName, float &fMemorySize, float &fEngineClock, float &fMemoryClock)
{
	SRMGPUInfoInf* pGPUInfoInf;
	pGPUInfoInf = SRMNvidiaGPUInfo::createGPUInfoInf();
	if (!pGPUInfoInf) // N卡找不到就换A卡
		pGPUInfoInf = SRMATIGPUInfo::createGPUInfoInf();

	//获取独立显卡成功
	if (pGPUInfoInf != nullptr)
	{
		strName = pGPUInfoInf->getGPUName();
		fMemorySize = pGPUInfoInf->GetGPUMemorySize();
		fEngineClock = pGPUInfoInf->getGPUEngineClock();
		fMemoryClock = pGPUInfoInf->getGPUMemoryClock();	
	}
	else
	{
		//走WMI
		float nRam = 0;

		std::string chRetValue = "";

		bool bres = wmi.GetSingleItemInfo("win32_displayconfiguration", "DeviceName", chRetValue);
		if (chRetValue.length() < 1)
		{
			bres = wmi.GetSingleItemInfo("Win32_VideoController", "Description", chRetValue);
		}
		strName = chRetValue;


		std::string strSql = "Win32_VideoController where caption='" + chRetValue + "'";
		chRetValue = "";

		bres = wmi.GetSingleItemInfo("Win32_VideoController ", "AdapterRAM", chRetValue);
		if (bres)
		{
			nRam = atof(chRetValue.c_str()) / 1024 / 1024 / 1024.0;
		}
		fMemorySize = nRam;
		fEngineClock = 0;
		fMemoryClock = 0;
		return ;
	}
	

}