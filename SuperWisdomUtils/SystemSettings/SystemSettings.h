#pragma once



//#include "Tools.h"
//#include "head.h"


#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息


// Windows 头文件:
#include <windows.h>

//C++头文件
#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <Shlwapi.h>
#include <string>
#include <io.h>
#include <direct.h>

#include <nb30.h> 
using namespace std;


//存储长度
#ifdef _UNICODE
#define CountStringBuffer CountStringBufferW
#else
#define CountStringBuffer CountStringBufferA
#endif
//存储长度
#define CountStringBufferA(String) ((UINT)((lstrlenA(String)+1)*sizeof(CHAR)))
#define CountStringBufferW(String) ((UINT)((lstrlenW(String)+1)*sizeof(WCHAR)))



/**********************注册表封装类**************************/
class CRegSettings
{
public:
	CRegSettings(HKEY hRootKey, LPCTSTR pszSubKey, bool bWow64 = false, bool bCreate = true);
	virtual ~CRegSettings();

public:
	bool SetValue(LPCTSTR pszKey, DWORD dwValue);
	bool SetValue(LPCTSTR pszKey, LPCTSTR pszValue);
	bool SetValue(LPCTSTR pszKey, const void* pData, DWORD dwSize);
	bool SetMultiValue(LPCTSTR pszKey, LPCTSTR pszValue, DWORD dwValueSize);


	void GetValue(LPCTSTR pszKey, DWORD* pdwValue, DWORD dwDefValue);
	void GetValue(LPCTSTR pszKey, LPTSTR pszValue, DWORD dwMaxSize, LPCTSTR pszDefValue);
	bool GetValue(LPCTSTR pszKey, void* pData, DWORD dwDataSize);

	bool DeleteValue(LPCTSTR pszKey);
	bool DeleteKey(LPCTSTR pszKey);
	bool RecurseDeleteKey(LPCTSTR pszKey);

	bool IsKeyExist();

private:
	LSTATUS RecurseDeleteKey(HKEY hParentKey, LPCTSTR pszSubKey);

private:
	HKEY     m_hRegKey;
	bool     m_bWow64;
};


