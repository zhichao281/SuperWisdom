#include "StdAfx.h"
#include <stdlib.h> 
#include <stdio.h> 
#include <tchar.h>
#include "iTunesApi.h"


iTunesApi::iTunesApi(void)
{
}


iTunesApi::~iTunesApi(void)
{
}

HMODULE iTunesApi::__hiTunesDeviceDll = NULL;
HMODULE iTunesApi::__hCoreFunctionDll = NULL;

/************************************************************************/
/* Return value                                                         */
/* 1:success                                                            */
/* 2:the itunes not found                                               */
/* 3:some function load error                                           */
/************************************************************************/
UINT iTunesApi::InitApi()
{
	char *libvar;
	char newlibvar[1024];
	char dllpath_iTunesMobileDevice[MAX_PATH];
	char dllpath_CoreFoundation[MAX_PATH];

	HKEY regKey1,regKey2;
	WCHAR value1[MAX_PATH],value2[MAX_PATH];
	long ret = 0;
	unsigned long vsize = MAX_PATH;
	DWORD dwType=REG_NONE;
	ret = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,(LPCWSTR)L"SOFTWARE\\Apple Inc.\\Apple Mobile Device Support\\Shared",0, KEY_READ,&regKey1);
	if (ret) return 2;
	ret = ::RegQueryValueEx(regKey1,(LPCWSTR)L"iTunesMobileDeviceDLL",0,&dwType,(LPBYTE)value1,&vsize);
	if (ret) return 2;
	ret = ::RegCloseKey(regKey1);
	ret = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,(LPCWSTR)L"SOFTWARE\\Apple Inc.\\Apple Application Support",0, KEY_READ,&regKey2);
	if (ret) return 2;
	ret = ::RegQueryValueEx(regKey2,(LPCWSTR)L"InstallDir",0,&dwType,(LPBYTE)value2,&vsize);
	if (ret) return 2;
	ret = ::RegCloseKey(regKey2);
	
	UINT len1 = wcslen(value1) - strlen("\\iTunesMobileDevice.dll");
	UINT len2 = wcslen(value2) - 1;
	wcstombs(dllpath_iTunesMobileDevice,value1,wcslen(value1) - strlen("\\iTunesMobileDevice.dll"));
	wcstombs(dllpath_CoreFoundation,value2,wcslen(value2) - 1);
	dllpath_iTunesMobileDevice[len1] = '\0';
	dllpath_CoreFoundation[len2] = '\0';

	WCHAR ____iTunesMobileDevice_path[MAX_PATH];
	WCHAR ____CoreFoundation_path[MAX_PATH];
	__hCoreFunctionDll = NULL;
	__hiTunesDeviceDll = NULL;

	libvar = getenv("path");
	sprintf(newlibvar,"path=%s",libvar);
	strcat(newlibvar,";");
	strcat(newlibvar,dllpath_iTunesMobileDevice);
	strcat(newlibvar,";");
	strcat(newlibvar,dllpath_CoreFoundation);
	_putenv(newlibvar);
	wcscpy(____iTunesMobileDevice_path,value1);
	wcscpy(____CoreFoundation_path,value2);
	wcscat(____CoreFoundation_path,L"CoreFoundation.dll");
	__hiTunesDeviceDll = ::LoadLibrary(____iTunesMobileDevice_path);
	__hCoreFunctionDll = ::LoadLibrary(____CoreFoundation_path);

	//am
	LoadLibraryModule(__hiTunesDeviceDll,AMRestoreRegisterForDeviceNotifications,func_AMRestoreRegisterForDeviceNotifications,"AMRestoreRegisterForDeviceNotifications");
	LoadLibraryModule(__hiTunesDeviceDll,AMDServiceConnectionSend,func_AMDServiceConnectionSend,"AMDServiceConnectionSend");
	LoadLibraryModule(__hiTunesDeviceDll,AMDServiceConnectionReceive,func_AMDServiceConnectionReceive,"AMDServiceConnectionReceive");
	LoadLibraryModule(__hiTunesDeviceDll,AMDeviceGetInterfaceType,func_AMDeviceGetInterfaceType,"AMDeviceGetInterfaceType");
	LoadLibraryModule(__hiTunesDeviceDll,AMDServiceConnectionInvalidate,func_AMDServiceConnectionInvalidate,"AMDServiceConnectionInvalidate");
	LoadLibraryModule(__hiTunesDeviceDll,AMDeviceRetain,func_AMDeviceRetain,"AMDeviceRetain");
	LoadLibraryModule(__hiTunesDeviceDll,AMDeviceNotificationSubscribe,func_AMDeviceNotificationSubscribe,"AMDeviceNotificationSubscribe");
	LoadLibraryModule(__hiTunesDeviceDll,AMDeviceConnect,func_AMDeviceConnect,"AMDeviceConnect");
	LoadLibraryModule(__hiTunesDeviceDll,AMDeviceCopyDeviceIdentifier,func_AMDeviceCopyDeviceIdentifier,"AMDeviceCopyDeviceIdentifier");
	LoadLibraryModule(__hiTunesDeviceDll,AMDeviceDisconnect,func_AMDeviceDisconnect,"AMDeviceDisconnect");
	LoadLibraryModule(__hiTunesDeviceDll,AMDeviceIsPaired,func_AMDeviceIsPaired,"AMDeviceIsPaired");
	LoadLibraryModule(__hiTunesDeviceDll,AMDeviceValidatePairing,func_AMDeviceValidatePairing,"AMDeviceValidatePairing");
	LoadLibraryModule(__hiTunesDeviceDll,AMDeviceStartSession,func_AMDeviceStartSession,"AMDeviceStartSession");
	LoadLibraryModule(__hiTunesDeviceDll,AMDeviceStopSession,func_AMDeviceStopSession,"AMDeviceStopSession");
	LoadLibraryModule(__hiTunesDeviceDll,AMDeviceSetValue,func_AMDeviceSetValue,"AMDeviceSetValue");
	LoadLibraryModule(__hiTunesDeviceDll,AMDeviceCopyValue,func_AMDeviceCopyValue,"AMDeviceCopyValue");
	LoadLibraryModule(__hiTunesDeviceDll,AMDeviceGetInterfaceType,func_AMDeviceGetInterfaceType,"AMDeviceGetInterfaceType");
	
	//cfstring
	LoadLibraryModule(__hCoreFunctionDll,CFStringMakeConstantString,func_CFStringMakeConstantString,"__CFStringMakeConstantString");
	
	//cf
	LoadLibraryModule(__hCoreFunctionDll,CFStringGetCString,func_CFStringGetCString,"CFStringGetCString");
	LoadLibraryModule(__hCoreFunctionDll,CFGetTypeID,func_CFGetTypeID,"CFGetTypeID");
	LoadLibraryModule(__hCoreFunctionDll,CFStringGetTypeID,func_CFStringGetTypeID,"CFStringGetTypeID");
	LoadLibraryModule(__hCoreFunctionDll,CFStringGetLength,func_CFStringGetLength,"CFStringGetLength");
	return 1;
}
BOOL iTunesApi::ReleaseApi()
{
	if(__hiTunesDeviceDll)
		::FreeLibrary(__hiTunesDeviceDll);
	if(__hCoreFunctionDll)
		::FreeLibrary(__hCoreFunctionDll);
	return TRUE;
}


//function values
//am
func_AMRestoreRegisterForDeviceNotifications iTunesApi::AMRestoreRegisterForDeviceNotifications;
func_AMDServiceConnectionSend iTunesApi::AMDServiceConnectionSend;
func_AMDServiceConnectionReceive iTunesApi::AMDServiceConnectionReceive;
func_AMDeviceGetInterfaceType iTunesApi::AMDeviceGetInterfaceType;
func_AMDServiceConnectionInvalidate iTunesApi::AMDServiceConnectionInvalidate;
func_AMDeviceRetain iTunesApi::AMDeviceRetain;
func_AMDeviceNotificationSubscribe iTunesApi::AMDeviceNotificationSubscribe;
func_AMDeviceConnect iTunesApi::AMDeviceConnect;
func_AMDeviceCopyDeviceIdentifier iTunesApi::AMDeviceCopyDeviceIdentifier;
func_AMDeviceDisconnect iTunesApi::AMDeviceDisconnect;
func_AMDeviceIsPaired iTunesApi::AMDeviceIsPaired;
func_AMDeviceValidatePairing iTunesApi::AMDeviceValidatePairing;
func_AMDeviceStartSession iTunesApi::AMDeviceStartSession;
func_AMDeviceStopSession iTunesApi::AMDeviceStopSession;
func_AMDeviceSetValue iTunesApi::AMDeviceSetValue;
func_AMDeviceCopyValue iTunesApi::AMDeviceCopyValue;
//cfstring
func_CFStringMakeConstantString iTunesApi::CFStringMakeConstantString;
//cf
func_CFStringGetCString iTunesApi::CFStringGetCString;
func_CFGetTypeID iTunesApi::CFGetTypeID;
func_CFStringGetTypeID iTunesApi::CFStringGetTypeID;
func_CFStringGetLength iTunesApi::CFStringGetLength;