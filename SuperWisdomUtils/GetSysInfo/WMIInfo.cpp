#include "WMIInfo.h"

#pragma comment(lib,"WbemUuid.lib")

using namespace std;




CWmiInfo::CWmiInfo(void)
{
	m_pWbemSvc = NULL;
	m_pWbemLoc = NULL;
	m_pEnumClsObj = NULL;
}

CWmiInfo::~CWmiInfo(void)
{
	m_pWbemSvc = NULL;
	m_pWbemLoc = NULL;
	m_pEnumClsObj = NULL;
}

HRESULT CWmiInfo::InitWmi()
{

	HRESULT hres;

	//一、初始化COM组件  
	//初始化COM  
	hres = ::CoInitializeEx(0, COINIT_MULTITHREADED);
	if (SUCCEEDED(hres) || RPC_E_CHANGED_MODE == hres)
	{
		//设置进程的安全级别，（调用COM组件时在初始化COM之后要调用CoInitializeSecurity设置进程安全级别，否则会被系统识别为病毒）  
		hres = CoInitializeSecurity(NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_PKT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE,
			NULL);


		//二、创建一个WMI命名空间连接  
		//创建一个CLSID_WbemLocator对象  
		hres = CoCreateInstance(CLSID_WbemLocator,
			0,
			CLSCTX_INPROC_SERVER,
			IID_IWbemLocator,
			(LPVOID*)&m_pWbemLoc);


		//使用m_pWbemLoc连接到"root\cimv2"并设置m_pWbemSvc的指针  
		hres = m_pWbemLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"),
			NULL,
			NULL,
			0,
			NULL,
			0,
			0,
			&m_pWbemSvc);



		//三、设置WMI连接的安全性  
		hres = CoSetProxyBlanket(m_pWbemSvc,
			RPC_C_AUTHN_WINNT,
			RPC_C_AUTHZ_NONE,
			NULL,
			RPC_C_AUTHN_LEVEL_CALL,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE);


	}
	return(hres);
}

HRESULT CWmiInfo::ReleaseWmi()
{
	HRESULT hr;

	if (NULL != m_pWbemSvc)
	{
		hr = m_pWbemSvc->Release();
	}
	if (NULL != m_pWbemLoc)
	{
		hr = m_pWbemLoc->Release();
	}
	if (NULL != m_pEnumClsObj)
	{
		hr = m_pEnumClsObj->Release();
	}

	::CoUninitialize();

	return(hr);
}

BOOL CWmiInfo::GetSingleItemInfo(std::string ClassName, std::string ClassMember, std::string &chRetValue, std::string strWhere /* = ""*/)
{

	bstr_t query("SELECT * FROM ");
	VARIANT vtProp;
	ULONG uReturn;
	HRESULT hr;
	BOOL bRet = FALSE;

	if (NULL != m_pWbemSvc)
	{
		//查询类ClassName中的所有字段,保存到m_pEnumClsObj中  
		query += _com_util::ConvertStringToBSTR((ClassName + strWhere).c_str());
		hr = m_pWbemSvc->ExecQuery(bstr_t("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			0, &m_pEnumClsObj);
		if (SUCCEEDED(hr))
		{
			//初始化vtProp值  
			VariantInit(&vtProp);

			uReturn = 0;

			//返回从当前位置起的第一个对象到m_pWbemClsObj中  
			hr = m_pEnumClsObj->Next(WBEM_INFINITE, 1, &m_pWbemClsObj, &uReturn);
			if (SUCCEEDED(hr) && uReturn > 0)
			{
				//从m_pWbemClsObj中找出ClassMember标识的成员属性值,并保存到vtProp变量中  
				hr = m_pWbemClsObj->Get(_com_util::ConvertStringToBSTR(ClassMember.c_str()), 0, &vtProp, 0, 0);
				if (SUCCEEDED(hr))
				{
					VariantToString(&vtProp, chRetValue);
					VariantClear(&vtProp);//清空vtProp  
					bRet = TRUE;
				}
			}

		}
	}
	if (NULL != m_pEnumClsObj)
	{
		hr = m_pEnumClsObj->Release();
		m_pEnumClsObj = NULL;
	}
	if (NULL != m_pWbemClsObj)
	{
		hr = m_pWbemClsObj->Release();
		m_pWbemClsObj = NULL;
	}
	return bRet;
}

BOOL CWmiInfo::GetSingleItemInfo(std::string ClassName, std::string ClassMember, std::vector<std::string>& vecRetValue, std::string strWhere /* = ""*/)
{
	bstr_t query("SELECT * FROM ");
	VARIANT vtProp, vtPropAll;
	ULONG uReturn;
	HRESULT hr;
	BOOL bRet = FALSE;

	if (NULL != m_pWbemSvc)
	{
		//查询类ClassName中的所有字段,保存到m_pEnumClsObj中  
		query += _com_util::ConvertStringToBSTR((ClassName + strWhere).c_str());
		hr = m_pWbemSvc->ExecQuery(bstr_t("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			0, &m_pEnumClsObj);
		if (SUCCEEDED(hr))
		{
			//初始化vtProp值  
			VariantInit(&vtProp);
			std::string chRetValue = "";
			uReturn = 0;
			while (m_pEnumClsObj)
			{
				//返回从当前位置起的第一个对象到m_pWbemClsObj中  
				hr = m_pEnumClsObj->Next(WBEM_INFINITE, 1, &m_pWbemClsObj, &uReturn);
				if (0 == uReturn)
				{
					break;
				}
				if (SUCCEEDED(hr) && uReturn > 0)
				{
					//从m_pWbemClsObj中找出ClassMember标识的成员属性值,并保存到vtProp变量中  
					hr = m_pWbemClsObj->Get(_com_util::ConvertStringToBSTR(ClassMember.c_str()), 0, &vtProp, 0, 0);
					if (SUCCEEDED(hr))
					{
						VariantToString(&vtProp, chRetValue);
						vecRetValue.push_back(chRetValue);
						VariantClear(&vtProp);//清空vtProp  
						bRet = TRUE;
					}
				}
			}
		}
	}
	if (NULL != m_pEnumClsObj)
	{
		hr = m_pEnumClsObj->Release();
		m_pEnumClsObj = NULL;
	}
	if (NULL != m_pWbemClsObj)
	{
		hr = m_pWbemClsObj->Release();
		m_pWbemClsObj = NULL;
	}
	return bRet;
}

VARIANT CWmiInfo::GetSingleItemInfo(std::string ClassName, std::string ClassMember)
{

	bstr_t query("SELECT * FROM ");
	VARIANT vtProp;
	ULONG uReturn;
	HRESULT hr;
	BOOL bRet = FALSE;

	if (NULL != m_pWbemSvc)
	{
		//查询类ClassName中的所有字段,保存到m_pEnumClsObj中  
		query += _com_util::ConvertStringToBSTR(ClassName.c_str());
		hr = m_pWbemSvc->ExecQuery(bstr_t("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			0, &m_pEnumClsObj);
		if (SUCCEEDED(hr))
		{
			//初始化vtProp值  
			VariantInit(&vtProp);
			uReturn = 0;

			//返回从当前位置起的第一个对象到m_pWbemClsObj中  
			hr = m_pEnumClsObj->Next(WBEM_INFINITE, 1, &m_pWbemClsObj, &uReturn);
			if (SUCCEEDED(hr) && uReturn > 0)
			{
				//从m_pWbemClsObj中找出ClassMember标识的成员属性值,并保存到vtProp变量中  
				hr = m_pWbemClsObj->Get(_com_util::ConvertStringToBSTR(ClassMember.c_str()), 0, &vtProp, 0, 0);
				if (SUCCEEDED(hr))
				{

					//VariantClear(&vtProp);//清空vtProp  
					bRet = TRUE;
				}
			}
		}
	}
	if (NULL != m_pEnumClsObj)
	{
		hr = m_pEnumClsObj->Release();
		m_pEnumClsObj = NULL;
	}
	if (NULL != m_pWbemClsObj)
	{
		hr = m_pWbemClsObj->Release();
		m_pWbemClsObj = NULL;
	}
	return vtProp;
}

BOOL CWmiInfo::GetGroupItemInfo(std::string ClassName, std::string ClassMember[], int n, std::string &chRetValue)
{
	_bstr_t query("SELECT * FROM ");
	std::string result, info;
	VARIANT vtProp;
	ULONG uReturn;
	HRESULT hr;
	int i;
	BOOL bRet = FALSE;
	if (NULL != m_pWbemSvc)
	{
		query += _bstr_t(ClassName.c_str());
		hr = m_pWbemSvc->ExecQuery(_bstr_t("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 0, &m_pEnumClsObj);
		if (SUCCEEDED(hr))
		{
			VariantInit(&vtProp); //初始化vtProp变量  
			if (m_pEnumClsObj)
			{
				Sleep(10);
				uReturn = 0;
				hr = m_pEnumClsObj->Next(WBEM_INFINITE, 1, &m_pWbemClsObj, &uReturn);
				if (SUCCEEDED(hr) && uReturn > 0)
				{
					for (i = 0; i < n; ++i)
					{
						hr = m_pWbemClsObj->Get(_bstr_t(ClassMember[i].c_str()), 0, &vtProp, 0, 0);
						if (SUCCEEDED(hr))
						{
							VariantToString(&vtProp, info);
							chRetValue += info + "\t";
							VariantClear(&vtProp);
							bRet = TRUE;
						}
					}
					chRetValue += "\r\n";
				}
			}
		}
	}

	if (NULL != m_pEnumClsObj)
	{
		hr = m_pEnumClsObj->Release();
		m_pEnumClsObj = NULL;
	}
	if (NULL != m_pWbemClsObj)
	{
		hr = m_pWbemClsObj->Release();
		m_pWbemClsObj = NULL;
	}
	return bRet;
}

void CWmiInfo::VariantToString(const LPVARIANT pVar, std::string &chRetValue) const
{
	_bstr_t HUGEP* pBstr;
	BYTE HUGEP* pBuf;
	LONG low, high, i;
	HRESULT hr;
	char  szRetValue[256];
	switch (pVar->vt)
	{
	case VT_BSTR:
	{
		chRetValue = _com_util::ConvertBSTRToString(pVar->bstrVal);


	}
	break;
	case VT_BOOL:
	{
		if (VARIANT_TRUE == pVar->boolVal)
			chRetValue = "true";
		else
			chRetValue = "false";
	}
	break;
	case VT_I4:
	{
		sprintf_s(szRetValue, 256, "%u", pVar->uintVal);

		chRetValue = szRetValue;
	}
	break;
	case VT_UI1:
	{
		sprintf_s(szRetValue, 256, "%u", pVar->uintVal);
		chRetValue = szRetValue;
	}
	break;
	case VT_UI4:
	{
		sprintf_s(szRetValue, 256, "%u", pVar->uintVal);
		chRetValue = szRetValue;
	}
	break;

	case VT_BSTR | VT_ARRAY:
	{
		hr = SafeArrayAccessData(pVar->parray, (void HUGEP**)&pBstr);
		hr = SafeArrayUnaccessData(pVar->parray);

		sprintf_s(szRetValue, "%s", pBstr);
		chRetValue = szRetValue;
	}
	break;

	case VT_I4 | VT_ARRAY:
	{
		SafeArrayGetLBound(pVar->parray, 1, &low);
		SafeArrayGetUBound(pVar->parray, 1, &high);

		hr = SafeArrayAccessData(pVar->parray, (void HUGEP**)&pBuf);
		hr = SafeArrayUnaccessData(pVar->parray);
		std::string strTmp;
		high = min(high, MAX_PATH * 2 - 1);
		for (i = low; i <= high; ++i)
		{
			sprintf_s(szRetValue, 256, "%02X", pBuf[i]);
			chRetValue += szRetValue;
		}
	}
	break;
	default:
		break;
	}
}