#include "CHttpClient.h"
#include "include/curl/curl.h"
#pragma comment(lib, "lib/libcurl.lib")


#include <string>
#include <assert.h>


#pragma comment(lib, "ws2_32")
#pragma comment(lib, "Iphlpapi")
#pragma comment(lib, "Wldap32")


CLibcurl::CLibcurl(void)
	: m_pCurl(NULL)
	, m_nPort(80)
	, m_hFile(INVALID_HANDLE_VALUE)
	, m_pCallback(NULL)
	, m_pCallbackParam(NULL)
	, m_curlCode(CURLE_OK)
	, m_lfFlag(Lf_None)
	, m_curlList(NULL)
{
	m_pCurl = curl_easy_init();

	// 设置写数据的回调函数
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, this);

	/**
	* 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
	* 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
	*/
	curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1);

	// 设置默认的超时时间
	curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, 3);


}


CLibcurl::~CLibcurl(void)
{
	ClearHeaderList();
	curl_easy_cleanup(m_pCurl);

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

bool CLibcurl::SetPort(LONG port)
{
	if (port == m_nPort)
		return true;
	m_nPort = port;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_PORT, m_nPort);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::SetTimeout(int nSecond)
{
	if (nSecond < 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, nSecond);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::SetConnectTimeout(int nSecond)
{
	if (nSecond < 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, nSecond);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::SetUserAgent(LPCSTR lpAgent)
{
	if (NULL == lpAgent)
		return false;
	int nLen = strlen(lpAgent);
	if (nLen == 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_USERAGENT, lpAgent);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::SetResumeFrom(LONG lPos)
{
	if (lPos < 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_RESUME_FROM, lPos);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::SetResumeFromLarge(LONGLONG llPos)
{
	if (llPos < 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_RESUME_FROM_LARGE, llPos);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::AddHeader(LPCSTR lpKey, LPCSTR lpValue)
{
	assert(lpKey != NULL && lpValue != NULL);
	int nLen1 = strlen(lpKey), nLen2 = strlen(lpValue);
	assert(nLen1 > 0 && nLen2 > 0);
	string strHeader(lpKey);
	strHeader.append(": ");
	strHeader.append(lpValue);
	m_curlList = curl_slist_append(m_curlList, strHeader.c_str());
	if (m_curlList)
	{
		m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, m_curlList);
	}
	else
	{
		m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, 0L);
	}
	return CURLE_OK == m_curlCode;
}

void CLibcurl::ClearHeaderList()
{
	if (m_curlList)
	{
		curl_slist_free_all(m_curlList);
		m_curlList = NULL;
	}
}

bool CLibcurl::SetCookie(LPCSTR lpCookie)
{
	assert(lpCookie != NULL);
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_COOKIE, lpCookie);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::SetCookieFile(LPCSTR lpFilePath)
{
	assert(lpFilePath != NULL);
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_COOKIEFILE, lpFilePath);
	return CURLE_OK == m_curlCode;
}

const char* CLibcurl::GetError() const
{
	return curl_easy_strerror(m_curlCode);
}

void CLibcurl::SetCallback(CLibcurlCallback* pCallback, void* lpParam)
{
	m_pCallback = pCallback;
	m_pCallbackParam = lpParam;
}


bool CLibcurl::AsynDownloadToFile(LPCSTR lpUrl, LPCSTR lpFile)
{
	// 设置访问网址
	CURLcode code = curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	DeleteFileA(lpFile);
	m_hFile = CreateFileA(lpFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		return FALSE;
	}
	curl_easy_setopt(m_pCurl, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSDATA, this);
	m_lfFlag = Lf_Download;
	//开始执行请求
	m_curlCode = curl_easy_perform(m_pCurl);
	if (CURLE_OK != m_curlCode)
	{
		return false;
	}

	return CURLE_OK == m_curlCode;
}

bool CLibcurl::DownloadToFile(LPCSTR lpUrl, LPCSTR lpFile)
{
	bool bres = AsynDownloadToFile(lpUrl, lpFile);
	if (CURLE_OK != bres)
	{
		return false;
	}

	// 判断返回值
	curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &m_nResponseCode);

	if (200 != m_nResponseCode)
	{
		return false;
	}

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	return CURLE_OK == m_curlCode;
}







bool CLibcurl::Post(LPCSTR lpUrl, LPCSTR lpData)
{
	assert(lpData != NULL);


	curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, lpData);
	//curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, lpData);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);


	m_lfFlag = Lf_Post;
	m_strRespons.clear();
	m_curlCode = curl_easy_perform(m_pCurl);
	return CURLE_OK == m_curlCode;
}

std::string CLibcurl::Post2(std::string strUrl, std::string strData)
{
	assert(strData.size()>0);

	std::string		strReturn = "";
	curl_easy_setopt(m_pCurl, CURLOPT_URL, strUrl.c_str());

	// 设置为Post方式
	curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
	// post数据
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, strData.c_str());
	//curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, strData.size());

	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, false);

	m_lfFlag = Lf_Post;
	m_strRespons.clear();
	// 执行请求
	m_curlCode = curl_easy_perform(m_pCurl);
	if (CURLE_OK != m_curlCode)
	{
		return strReturn;
	}

	// 判断返回值
	curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &m_nResponseCode);

	strReturn = GetRespons();

	if (200 != m_nResponseCode)
	{
		return strReturn;
	}
	return strReturn;

}

bool CLibcurl::Post(LPCSTR lpUrl, unsigned char* lpData, unsigned int nSize)
{
	assert(lpData != NULL && nSize > 0);
	curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, lpData);
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, nSize);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	m_lfFlag = Lf_Post;
	m_strRespons.clear();
	m_curlCode = curl_easy_perform(m_pCurl);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::AsynGet(LPCSTR lpUrl)
{
	assert(lpUrl != NULL);
	curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1);//支持重定向
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(m_pCurl, CURLOPT_NOPROGRESS, false);//设为false 下面才能设置进度响应函数
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);//进度响应函数
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSDATA, this);//数据传输的对象
	m_lfFlag = Lf_Get;
	m_strRespons.clear();
	m_curlCode = curl_easy_perform(m_pCurl);
	return CURLE_OK == m_curlCode;
}

string CLibcurl::Get(LPCSTR lpUrl)
{
	assert(lpUrl != NULL);
	string		strReturn = "";
	curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1);//支持重定向
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L);

	m_lfFlag = Lf_Get;
	m_strRespons.clear();
	// 执行请求
	m_curlCode = curl_easy_perform(m_pCurl);

	if (CURLE_OK != m_curlCode)
	{
		return strReturn;
	}

	// 判断返回值
	curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &m_nResponseCode);
	if (200 != m_nResponseCode)
	{
		strReturn = GetRespons();
		return strReturn;
	}

	strReturn = GetRespons();

	return strReturn;
}
const string& CLibcurl::GetRespons() const
{
	return m_strRespons;
}

const char* CLibcurl::GetResponsPtr() const
{
	return m_strRespons.c_str();
}

int CLibcurl::GetResponseCode()
{
	return m_nResponseCode;
}

size_t CLibcurl::WriteCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	//把下载到的数据以追加的方式写入文件(一定要有a，否则前面写入的内容就会被覆盖了)
	CLibcurl* pThis = (CLibcurl*)pParam;
	DWORD dwWritten = 0;
	switch (pThis->m_lfFlag)
	{
	case Lf_Download://下载
	{
		if (pThis->m_pCallback)
		{
			pThis->m_pCallback->WriteCallback(pBuffer, nSize, nMemByte, pParam);
		}

		pThis->m_strRespons.append((const char*)pBuffer, nSize*nMemByte);

		if (pThis->m_hFile == INVALID_HANDLE_VALUE)
			return 0;
		if (!WriteFile(pThis->m_hFile, pBuffer, nSize*nMemByte, &dwWritten, NULL))
			return 0;

	}
	break;
	case Lf_Post://Post数据
	case Lf_Get://Get数据
	{
		pThis->m_strRespons.append((const char*)pBuffer, nSize*nMemByte);
		dwWritten = nSize * nMemByte;
	}
	break;
	case Lf_None://未定义
		break;
	}
	return dwWritten;
}

size_t CLibcurl::ResponseHeadCallBack(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	CLibcurl* pThis = (CLibcurl*)pParam;
	return 0;
}

int CLibcurl::ProgressCallback(void *pParam, double dltotal, double dlnow, double ultotal, double ulnow)
{
	CLibcurl* pThis = (CLibcurl*)pParam;
	if (pThis->m_pCallback)
	{
		pThis->m_pCallback->Progress(pThis->m_pCallbackParam, dltotal, dlnow);
	}
	return 0;
}
