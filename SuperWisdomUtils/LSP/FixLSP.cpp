#include "FixLSP.h"
#include <Windows.h>
#include <stdarg.h>
#include <iostream>

#define ForceWait//强行等待2秒的开关

#ifdef ForceWait
#include <Windows.h>
#include <time.h>
#endif



CFixLSP::CFixLSP()
{
}

CFixLSP::~CFixLSP()
{

}

void CFixLSP::SlientFix()
{
	int nStart = time(0);
	fixlspHelper.CallProc(true, fixlspHelper.GetApplicationPath(), " /slient");
	int nWaitTime = time(0) - nStart;
#ifdef ForceWait
	if (nWaitTime <= 2)
	{
		HANDLE hEvent = CreateEvent(NULL, false, false, NULL);
		WaitForSingleObject(hEvent, 2000);
		CloseHandle(hEvent);
	}
#endif

}

void CFixLSP::Restore()
{
	fixlspHelper.CallProc(true, fixlspHelper.GetApplicationPath(), " /restore");

}



void CFixLSP::OnFixLSP(BOOL bSlient)
{
	std::thread fixlspThread([&]()
	{
		if (bSlient)
		{
			SlientFix();
		}
		else
		{
			Restore();
		}
	});
	fixlspThread.detach();
}


int FixLspHelper::CallProc(BOOL bWaitForProcessExit, const char* pProcessPath, const char* pCmdlineFmt, ...)
{
	va_list ap;
	va_start(ap, pCmdlineFmt);
	const int nLen = 1024;
	char szCmd[nLen] = "";	
	vsprintf_s(szCmd, 1024,pCmdlineFmt, ap);
	va_end(ap);
	char szPath[nLen] = "";

	sprintf_s(szPath, 1024,"\"%s\" %s", pProcessPath, szCmd);
	STARTUPINFOA si; 
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si); 
	GetStartupInfoA(&si); 	
	si.wShowWindow = SW_HIDE;  //隐藏窗口 
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	BOOL hRes = CreateProcessA(NULL, szPath, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi);

	if (bWaitForProcessExit)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
	}

	return hRes == false ? GetLastError() : 0;
}

BOOL FixLspHelper::IsX64()
{
	// 获取操作系统位数
	SYSTEM_INFO si;
	typedef VOID(WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
#ifdef _UNICODE
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(_T("kernel32")), "GetNativeSystemInfo");
#else
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress(GetModuleHandleA(_T("kernel32")), "GetNativeSystemInfo");
#endif
	if (NULL != fnGetNativeSystemInfo)
	{
		fnGetNativeSystemInfo(&si);
	}
	else
	{
		GetSystemInfo(&si);
	}

	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
	
		return TRUE;
	}
	else
	{
		
		return FALSE;
	}
}

FixLspHelper::FixLspHelper()
{
	GetModuleFileNameA(NULL, szApplicationPath, 2048);
	char* p = strrchr(szApplicationPath, '\\');
	*p = 0;
	char szFixLSPFileName[128] = "";
	sprintf_s(szFixLSPFileName, 128,"\\fixlsp%s", FixLspHelper::IsX64() ? "x64.exe" : "x86.exe");
	strcat_s(szApplicationPath, szFixLSPFileName);
}

char* FixLspHelper::GetApplicationPath()
{
	return szApplicationPath;
}
