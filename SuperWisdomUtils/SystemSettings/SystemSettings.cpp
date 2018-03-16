#include "SystemSettings.h"


//Êý×éÎ¬Êý
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))
/**********************×¢²á±í·â×°Àà**************************/
CRegSettings::CRegSettings(HKEY hRootKey, LPCTSTR pszSubKey, bool bWow64 /* = false */, bool bCreate /* = true */)
{
	m_bWow64 = bWow64;
	DWORD dwSam = KEY_ALL_ACCESS, dwSecurity = (KEY_ALL_ACCESS | KEY_EXECUTE);

	if(m_bWow64)
	{
		dwSam |= KEY_WOW64_64KEY;
		dwSecurity |= KEY_WOW64_64KEY;
	}

	m_hRegKey = NULL;
	LSTATUS lRet = RegOpenKeyEx(hRootKey, pszSubKey, 0, dwSam, &m_hRegKey);
	if(ERROR_SUCCESS != lRet && bCreate)
	{
		lRet = RegCreateKeyEx(hRootKey, pszSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, dwSecurity, NULL, &m_hRegKey, NULL);
	}
}

CRegSettings::~CRegSettings()
{
	if(NULL != m_hRegKey)
	{
		RegCloseKey(m_hRegKey);
		m_hRegKey = NULL;
	}
}

bool CRegSettings::IsKeyExist()
{
	return (NULL != m_hRegKey);
}

bool CRegSettings::SetValue(LPCTSTR pszKey, DWORD dwValue)
{
	if(NULL != m_hRegKey)
	{
		assert(NULL != pszKey);

		LSTATUS lRet = RegSetValueEx(m_hRegKey, pszKey, 0, REG_DWORD, (LPBYTE)(&dwValue), sizeof(DWORD));
		return (ERROR_SUCCESS == lRet);
	}

	return false;
}

bool CRegSettings::SetValue(LPCTSTR pszKey, LPCTSTR pszValue)
{
	if(NULL != m_hRegKey)
	{
		assert(NULL != pszKey && NULL != pszValue);

		DWORD dwSize = CountStringBuffer(pszValue);
		LSTATUS lRet = RegSetValueEx(m_hRegKey, pszKey, 0, REG_SZ, (LPBYTE)pszValue, dwSize);
		return (ERROR_SUCCESS == lRet);
	}

	return false;
}

bool CRegSettings::SetValue(LPCTSTR pszKey, const void* pData, DWORD dwSize)
{
	if(NULL != m_hRegKey)
	{
		assert(NULL != pszKey && NULL != pData);

		LSTATUS lRet = RegSetValueEx(m_hRegKey, pszKey, 0, REG_BINARY, (LPBYTE)pData, dwSize);
		return (ERROR_SUCCESS == lRet);
	}

	return false;
}

bool CRegSettings::SetMultiValue(LPCTSTR pszKey, LPCTSTR pszValue, DWORD dwValueSize)
{
	if(NULL != m_hRegKey)
	{
		assert(NULL != pszKey && NULL != pszValue);

		DWORD dwSize = dwValueSize * sizeof(TCHAR);
		LSTATUS lRet = RegSetValueEx(m_hRegKey, pszKey, 0, REG_MULTI_SZ, (LPBYTE)pszValue, dwSize);
		return (ERROR_SUCCESS == lRet);
	}

	return false;
}

void CRegSettings::GetValue(LPCTSTR pszKey, DWORD* pdwValue, DWORD dwDefValue)
{
	assert(NULL != pszKey && NULL != pdwValue);

	bool bRet = true;
	if(NULL != m_hRegKey)
	{
		DWORD dwSize = sizeof(DWORD), dwType = REG_DWORD;
		LSTATUS lRet = RegQueryValueEx(m_hRegKey, pszKey, 0, &dwType, (LPBYTE)pdwValue, &dwSize);
		if(ERROR_SUCCESS != lRet || REG_DWORD != dwType)
		{
			bRet = false;
		}
	}

	if(!bRet)
	{
		*pdwValue = dwDefValue;
	}
}

void CRegSettings::GetValue(LPCTSTR pszKey, LPTSTR pszValue, DWORD dwMaxSize, LPCTSTR pszDefValue)
{
	assert(NULL != pszKey && NULL != pszValue);

	bool bRet = true;
	if(NULL != m_hRegKey)
	{
		DWORD dwType = REG_SZ, dwSize = dwMaxSize * sizeof(TCHAR);
		LSTATUS lRet = RegQueryValueEx(m_hRegKey, pszKey, 0, &dwType, (LPBYTE)pszValue, &dwSize);
		if((ERROR_SUCCESS != lRet) || ((REG_SZ != dwType) && (REG_EXPAND_SZ != dwType)))
		{
			bRet = false;
		}
	}

	if(!bRet)
	{
		//if(NULL != pszDefValue)
			//StrCpy(pszValue, pszDefValue);
	}
}

bool CRegSettings::GetValue(LPCTSTR pszKey, void* pData, DWORD dwDataSize)
{
	assert(NULL != pszKey && NULL != pData);

	if(NULL != m_hRegKey)
	{
		DWORD dwType = REG_BINARY, dwSize = dwDataSize;
		LSTATUS lRet = RegQueryValueEx(m_hRegKey, pszKey, 0, &dwType, (LPBYTE)pData, &dwSize);
		return (ERROR_SUCCESS == lRet && REG_BINARY == dwType);
	}

	return false;
}

bool CRegSettings::DeleteValue(LPCTSTR pszKey)
{
	if(NULL != m_hRegKey)
	{
		assert(NULL != pszKey);

		LSTATUS lRet = RegDeleteValue(m_hRegKey, pszKey);
		return (ERROR_SUCCESS == lRet);
	}

	return false;
}

bool CRegSettings::DeleteKey(LPCTSTR pszKey)
{
	if(NULL != m_hRegKey)
	{
		assert(NULL != pszKey);

		LSTATUS lRet = RegDeleteKey(m_hRegKey, pszKey);
		return (ERROR_SUCCESS == lRet);
	}

	return false;
}

bool CRegSettings::RecurseDeleteKey(LPCTSTR pszKey)
{
	if(NULL != m_hRegKey)
	{
		assert(NULL != pszKey);

		LSTATUS lRet = RecurseDeleteKey(m_hRegKey, pszKey);
		return (ERROR_SUCCESS == lRet);
	}

	return false;
}

LSTATUS CRegSettings::RecurseDeleteKey(HKEY hParentKey, LPCTSTR pszSubKey)
{
	if(NULL != hParentKey)
	{
		DWORD dwSam = KEY_ALL_ACCESS;
		if(m_bWow64)
		{
			dwSam |= KEY_WOW64_64KEY;
		}

		HKEY hSubKey = NULL;
		LSTATUS lRet = RegOpenKeyEx(hParentKey, pszSubKey, 0, dwSam, &hSubKey);
		if(ERROR_SUCCESS != lRet)
		{
			return lRet;
		}

		//µÝ¹éÉ¾³ý
		TCHAR szKeyName[256] = _T("");
		while(ERROR_SUCCESS == RegEnumKey(hSubKey, 0, szKeyName, CountArray(szKeyName)))
		{
			//É¾³ý¼üÖµ
			lRet = RecurseDeleteKey(hSubKey, szKeyName);
			if(ERROR_SUCCESS != lRet)
			{
				RegCloseKey(hSubKey);
				return lRet;
			}
		}

		//¹Ø±Õ¼üÖµ
		RegCloseKey(hSubKey);

		//É¾³ý¼üÖµ
		lRet = RegDeleteKey(hParentKey, pszSubKey);
		return lRet;
	}

	return ERROR_INVALID_HANDLE;
}


