#include <sys/types.h>  
#include <sys/stat.h>  
#include <stdio.h> 
#include <algorithm> 
#include "HttpServer.h"

//http = header + body
//header = request line + header
//header and body is cuted by "\r\n"
//each line of http header ending by "\r\n",
//so,header and body is cuted by "\r\n\r\n"


//GET /search?hl=zh-CN&source=hp&q=domety&aq=f&oq= HTTP/1.1 

//HTTP 请求消息头部实例:
//GET /a.html HTTP/1.1
//Host：rss.sina.com.cn
//User - Agent：Mozilla / 5、0(Windows; U; Windows NT 5、1; zh - CN; rv:1、8、1、14) Gecko / 20080404 Firefox / 2、0、0、14
//Accept：text / xml, application / xml, application / xhtml + xml, text / html; q = 0、9, text / plain; q = 0、8, image / png, */*;q=0、5
//Accept-Language：zh-cn,zh;q=0、5
//Accept-Encoding：gzip,deflate
//Accept-Charset：gb2312,utf-8;q=0、7,*;q=0、7
//Keep-Alive：300
//Connection：keep-alive
//Cookie：userId=C5bYpXrimdmsiQmsBPnE1Vn8ZQmdWSm3WRlEB3vRwTnRtW &lt;-- Cookie
//If-Modified-Since：Sun, 01 Jun 2008 12:05:30 GMT
//Cache-Control：max-age=0
//
//body
//


//HTTP 响应消息头部实例：
//Status：OK - 200 &lt;-- 响应状态码，表示 web 服务器处理的结果。
//Date：Sun, 01 Jun 2008 12:35:47 GMT
//Server：Apache/2、0、61 (Unix)
//Last-Modified：Sun, 01 Jun 2008 12:35:30 GMT
//Accept-Ranges：bytes
//Content-Length：18616
//Cache-Control：max-age=120
//Expires：Sun, 01 Jun 2008 12:37:47 GMT
//Content-Type：application/xml
//Age：2
//X-Cache：HIT from 236-41、D07071951、sina、com、cn &lt;-- 反向代理服务器使用的 HTTP 头部
//Via：1.0 236-41.D07071951.sina.com.cn:80 (squid/2.6.STABLE13)
//Connection：close
//is a file

#ifndef S_ISREG
    #define S_ISREG(m) (((m) & 0170000) == (0100000))
#endif

//is a directory
#ifndef S_ISDIR
    #define S_ISDIR(m) (((m) & 0170000) == (0040000))
#endif

//is can read the file
#ifndef S_IRUSR
    #define S_IRUSR 0400
#endif

#define ONEDATA_LEN  256
#define HEADER_LEN   64
#define URI_PATH_LEN 512

#ifdef _MSC_VER
#define W_HTTP_SNPRINTF_FORMAT(buffer, nPlanWriteLen, nLeftWriteLen, ...) \
    nPlanWriteLen = _snprintf_s(buffer, nLeftWriteLen, _TRUNCATE, __VA_ARGS__)
#else
    #define W_HTTP_SNPRINTF_FORMAT(m_szResponseHeader, nPlanWriteLen, nLeftWriteLen, ...) \
    nPlanWriteLen = snprintf(m_szResponseHeader, nLeftWriteLen, __VA_ARGS__)
#endif

#define RESPONSE_HEADER_CHECK(nPlanWriteLen, nLeftWriteLen, nSumWriteLen) \
	if(nPlanWriteLen < 0 || nPlanWriteLen > nLeftWriteLen) { \
	return W_HTTP_RESPONSE_ERROR; \
	} \
	nSumWriteLen += nPlanWriteLen; \
	nLeftWriteLen -= nPlanWriteLen; 

#define HTTP_RESPONSE_ERRORSTR "<html> \
<head> \
</head> \
<meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\"> \
<title>error</title> \
<body align=\"center\"> \
</body> \
%d %s \
</html>"

typedef struct tagHttpMimeType {
	const char *type;
	const char *value;
} HttpMimeType;

/* http://tool.oschina.net/commons/ */
HttpMimeType w_http_mime[] =
{
	{ ".html", "text/html" },
	{ ".xml", "text/xml" },
	{ ".xhtml", "application/xhtml+xml" },
	{ ".txt", "text/plain" },
	{ ".rtf", "application/rtf" },
	{ ".pdf", "application/pdf" },
	{ ".word", "application/msword" },
	{ ".png", "image/png" },
	{ ".gif", "image/gif" },
	{ ".jpg", "image/jpeg" },
	{ ".jpeg", "image/jpeg" },
	{ ".au", "audio/basic" },
	{ ".mpeg", "video/mpeg" },
	{ ".mpg", "video/mpeg" },
	{ ".avi", "video/x-msvideo" },
	{ ".gz", "application/x-gzip" },
	{ ".tar", "application/x-tar" },
	{ ".css", "text/css" },
	{ ".ico", "application/x-ico" },
	{ NULL, "text/plain" }
};

CHttpServer::CHttpServer()
{
	m_szResponseHeader = new char[HTTP_RESPONSE_LIMITLEN];
	m_szResponseBody = new char[HTTP_RESPONSE_BODYLEN];
	m_szUriPath = new char[URI_PATH_LEN];
	m_requestBodyCallBackFunc = NULL;
	InitHttpData();
	InitErrorInfo();
}

CHttpServer::~CHttpServer()
{
	delete[]m_szResponseHeader;
	delete[]m_szUriPath;
	m_mapErrorInfo.clear();
}

void CHttpServer::InitHttpData()
{
	m_eMethod = HTTP_GET;
	m_eHttpParserStatus = HTTP_PARSER_START;
	m_pMethodEnd = NULL;
	m_pUriStart = NULL;
	m_pUriEnd = NULL;
	m_pPathStart = NULL;
	m_pPathEnd = NULL;
	m_pVersionStart = NULL;
	m_pVersionEnd = NULL;
	m_pRequestLineStart = NULL;
	m_pRequestLineEnd = NULL;
	m_pHeaderStart = NULL;
	m_pHeaderEnd = NULL;
	m_pFileMapAddr = NULL;
	m_nReponseBodyLen = 0;
	m_nPathFileSize = 0;
	m_bIsChunked = false;
	m_bIsDynamicPage = false;
	m_szResponseHeader[0] = 0;
	m_szResponseBody[0] = 0;
	m_nBodyLen = 0;
	m_nStatusCode = W_HTTP_OK;
	m_mapHeader.clear();
}

void CHttpServer::InitErrorInfo()
{
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_PARSER_UNKNOWNERR, "Unknown Error"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_CONTINUE, "Continue"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_SWITCHING_PROTOCOLS, "Switching Protocols"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_OK, "OK"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_CREATED, "Created"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_ACCEPTED, "Accepted"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_NOAUTHORITATIVE_INFO, "Non-Authoritative Information"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_NO_CONTENT, "No Content"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_RESET_CONTENT, "Reset Content"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_PARTIAL_CONTENT, "Partial Content"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_MULTIPLE_CHOICES, "Multiple Choices"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_MOVED_PERMANENTLY, "Moved Permanently"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_FOUND, "Found"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_SEE_OTHER, "See Other"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_NOT_MODIFIED, "Not Modified"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_USE_PROXY, "Use Proxy"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_TEMPORARY_REDIRECT, "Temporary Redirect"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_BAD_REQUEST, "Bad Request"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_UNAUTHORIZED, "Unauthorized"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_FORBIDDEN, "Forbidden"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_NOT_FOUND, "Not Found"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_NOT_ALLOWED, "Method Not Allowed"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_NOT_ACCEPTABLE, "Not Acceptable"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_AUTHENTICATION_REQUIRED, "Proxy Authentication Required"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_REQUEST_TIMEOUT, "Request Time-out"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_CONFLICT, "Conflict"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_GONE, "Gone"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_LENGTH_REQUIRED, "Length Required"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_PRECONDITION_FAILED, "Precondition Failed"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_REQUESTENTITY_TOOLARGE, "Request Entity Too Large"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_REQUEST_URI_TOOLARGE, "Request-URI Too Large"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_UNSUPPORTED_MEDIATYPE, "Unsupported Media Type"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_REQUESTEDRANGE_NOTSATISFIABLE, "Requested range not satisfiable"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_EXPECTATION_FAILED, "Expectation Failed"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_INTERNAL_SERVERERROR, "Internal Server Error"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_NOT_IMPLEMENTED, "Not Implemented"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_BAD_GATEWAY, "Bad Gateway"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_SERVICE_UNAVAILABLE, "Service Unavailable"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_GATEWAY_TIMEOUT, "Gateway Time-out"));
	m_mapErrorInfo.insert(std::pair<int, const char*>(W_HTTP_VERSION_NOT_SUPPORTED, "HTTP Version not supported"));
}

const char* CHttpServer::GetStatusCodeName(int nStatusCode)
{
	std::unordered_map<int, const char*>::iterator it = m_mapErrorInfo.find(nStatusCode);
	if(it != m_mapErrorInfo.end())
		return it->second;
	return "Unknown Error";
}

const char* CHttpServer::GetHeaderValueForKey(char* pKey)
{
	std::unordered_map<string, string>::iterator it = m_mapHeader.find(pKey);
	if(it != m_mapHeader.end())
		return it->second.c_str();
	return NULL;
}

bool CHttpServer::CheckHeaderValueForKey(char* pKey, char* pValue)
{
	const char* pValueTmp = GetHeaderValueForKey(pKey);
	if(pValueTmp)
	{
#ifdef _MSC_VER
		if(_stricmp(pValueTmp, pValue) == 0)
#else
		if(strcasecmp(pValueTmp, pValue) == 0)
#endif
		{
			return true;
		}
	}
	return false;
}

int CHttpServer::GetResponseContentLength()
{
	/* make error webpage */
	if(m_nStatusCode != W_HTTP_OK)
	{
		const char* pFillStr = NULL;
		std::unordered_map<int, const char*>::iterator it = m_mapErrorInfo.find(m_nStatusCode);
		if(it != m_mapErrorInfo.end())
			pFillStr = it->second;
		else
			pFillStr = "unknown error";

		int nPlanWriteLen;
		W_HTTP_SNPRINTF_FORMAT(m_szResponseBody, nPlanWriteLen, HTTP_RESPONSE_BODYLEN, HTTP_RESPONSE_ERRORSTR, m_nStatusCode, pFillStr);
		//解决中文乱码
		//string utf8str(szErrorPage);
		//wstring s2ws(const std::string& s);
		//wstring wutf8str = s2ws(utf8str);
		//return wutf8str.c_str();
		m_nReponseBodyLen = strlen(m_szResponseBody);
		return m_nReponseBodyLen;
	}
	// 动态请求 先不做任何处理
	else if (m_bIsDynamicPage)
	{
		m_szResponseBody = "这个是测试程序";
		m_nReponseBodyLen = strlen(m_szResponseBody);
		return m_nReponseBodyLen;

//		if (HTTP_GET == m_eMethod || HTTP_POST == m_eMethod)
//		{
//			char *emptyArg[] = { NULL };
//			char *empthEnv[] = { NULL };
//			int nArgLen = m_pUriEnd - (m_pPathEnd + 1);
//			if (nArgLen <= 0) return 0;
//			string str(m_pPathEnd + 1, nArgLen);
//
//			// 处理动态请求参数
//			char * a = strtok((char *)str.c_str(), "&");
//			char * b = strtok(NULL, "&");
//			int x = atoi(a);
//			int y = atoi(b);
//			int result;
//			if (strstr(m_szUriPath, "Add"))
//			{
//				 result = x + y;
//			}
//			else if (strstr(m_szUriPath, "Sub"))
//			{
//				result = x - y;
//			}
//			sprintf(m_szResponseBody, "%d", result);
//			m_nReponseBodyLen = strlen(m_szResponseBody);
//			return m_nReponseBodyLen;
//
//
//#ifdef _MSC_VER
//			if (HTTP_GET == m_eMethod)
//			{
//				string strHead("QUERY_STRING=");
//				_putenv((strHead + str).c_str());//更改环境变量
//			}
//			else if (HTTP_POST == m_eMethod)
//			{
//				string strHead("CONTENT-LENGTH=");
//				char buff[64] = { 0 };
//				string str(itoa(m_nBodyLen, buff, 10));
//				putenv((strHead + str).c_str());
//			}
//
//			SECURITY_ATTRIBUTES sa;
//			memset(&sa, 0, sizeof(sa));
//			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
//			sa.lpSecurityDescriptor = NULL;  //默认的安全描述符     
//			sa.bInheritHandle = TRUE;        //这个必须要设定TRUE,参考资料：《windows核心编程》第三章     
//			HANDLE hRead, hWrite;
//
//			if (!CreatePipe(&hRead, &hWrite, &sa, 0))
//				return 0;
//
//			STARTUPINFOA siStartInfo;
//			PROCESS_INFORMATION ProcInfo;
//			ZeroMemory(&siStartInfo,sizeof(STARTUPINFO));
//			siStartInfo.cb =sizeof(STARTUPINFO);  
//			siStartInfo.dwFlags  |= STARTF_USESHOWWINDOW;  
//			siStartInfo.dwFlags  |= STARTF_USESTDHANDLES;  
//			siStartInfo.hStdOutput = hWrite;  
//			siStartInfo.hStdError  = hWrite;  
//
//			if (!CreateProcessA(NULL, W_HTTP_CGIBIN_WINDOWS, NULL, NULL, TRUE, NULL, NULL, NULL, &siStartInfo, &ProcInfo))
//				return 0;    
//
//			WaitForSingleObject(ProcInfo.hProcess, INFINITE);
//			CloseHandle(ProcInfo.hProcess);
//			CloseHandle(ProcInfo.hThread);
//
//			DWORD dwReadBytes;
//			if (!ReadFile(hRead, m_szResponseBody, HTTP_RESPONSE_BODYLEN - 1, &dwReadBytes, NULL))
//				return 0;
//
//			if(dwReadBytes <= 0) return 0;
//			m_nReponseBodyLen = dwReadBytes;
//			m_szResponseBody[dwReadBytes] = 0;
//#else
//			int p[2];
//			if(pipe(p)==-1) return 0;
//			if(fork() == 0)
//			{   /* child */
//				close(p[0]); /* Close unused read end */
//				if (HTTP_GET == m_eMethod)
//				    setenv("QUERY_STRING", string(str.c_str(), 1);
//				else if(HTTP_POST == m_eMethod)
//					setenv("CONTENT-LENGTH", m_nBodyLen, 1);
//				dup2(clientFD, STDOUT_FILENO);
//				execve(W_HTTP_CGIBIN_LINUX, emptyArg, empthEnv);
//			}
//			wait(NULL); /* Wait for child */
//			close(p[1]); /* Close unused write end */
//			int nReadBytes = read(p[0], m_szResponseBody, HTTP_RESPONSE_BODYLEN);
//			if (nReadBytes) <= 0) return 0;
//			m_nReponseBodyLen = nReadBytes;
//			m_szResponseBody[nReadBytes] = 0;
//#endif
//			return m_nReponseBodyLen;
//		}
		return 0;
	}

	return m_nPathFileSize;
}

const char* CHttpServer::GetMimeType(char* pExtendName/* = NULL*/)
{
	if (!pExtendName)
	{
		pExtendName = strrchr(m_szUriPath, '.');
		if (!pExtendName) return "text/plain";
	}

	int i = 0;
	for (; w_http_mime[i].type != NULL; i++)
	{
#ifdef _MSC_VER
		if (_stricmp(pExtendName, w_http_mime[i].type) == 0)
#else
		if (strcasecmp(pExtendName, w_http_mime[i].type) == 0)
#endif
		{
			return w_http_mime[i].value;
		}
	}

	return w_http_mime[i].value;
}

int CHttpServer::HttpSplit(const char* pHttpData, int nDataLen)
{
	int nDataLenSource = nDataLen;
	while(nDataLen > 0)
	{
		nDataLen--;
		if('\r' == *pHttpData++)
		{
			if(--nDataLen >= 0) 
			{
				if('\n' == *pHttpData++)
				{
			        if(NULL == m_pRequestLineEnd) 
					{
						m_pRequestLineEnd = pHttpData - 2;
						m_pHeaderStart = pHttpData;
					}
					if(--nDataLen >= 0)
					{
						if('\r' == *pHttpData++)
						{
							if(--nDataLen >= 0)
							{
								if('\n' == *pHttpData++)
								{
									m_pHeaderEnd = pHttpData - 4;
									m_nBodyLen = nDataLen;
									return W_HTTP_PARSER_SUCCESS;
								}
								else
								{
							        return W_HTTP_BAD_REQUEST;
								}
							}
							else
							{
						        return W_HTTP_PARSER_AGAIN;
							}
						}
						else
						{
					        continue;
						}
					}
					else
					{
						return W_HTTP_PARSER_AGAIN;
					}
				}
				else
				{
			        return W_HTTP_BAD_REQUEST;
				}
			}
			else
			{
		        return W_HTTP_PARSER_AGAIN;
			}
		}
	}
	return W_HTTP_PARSER_AGAIN;
}

int CHttpServer::HttpRequestLineParser()
{
	int nDataLen = (char*)m_pRequestLineEnd - (char*)m_pRequestLineStart;
	int nStep = 0;
	const char* pHttpData = m_pRequestLineStart;
	while(nDataLen > 0)
	{
		switch(nStep)
		{
		case 0:
			{
				if(' ' == *pHttpData)
				{
					m_pMethodEnd = pHttpData;
					m_pUriStart = pHttpData + 1;
					nStep = 1;
				}
			}break;
		case 1:
			{
				  //  获取请求的命令类型
				if(' ' == *pHttpData)
				{
					m_pUriEnd = pHttpData;
					nStep = 2;
				
					int nMethodLen = m_pMethodEnd - m_pRequestLineStart;
					if(_strnicmp(m_pRequestLineStart, "GET", nMethodLen) == 0)
						m_eMethod = HTTP_GET;
					else if(_strnicmp(m_pRequestLineStart, "POST", nMethodLen) == 0)
						m_eMethod = HTTP_POST;
					else
						return W_HTTP_NOT_ALLOWED;
				}
			}break;
		case 2:
			{
				if('/' == *pHttpData)
				{
					m_pVersionStart = pHttpData + 1;
					nStep = 3;
				}
			}break;
		case 3:
			{
				char byteValue = *pHttpData;
				if((byteValue < '0' || byteValue > '9') && byteValue != '.')
				{
					return W_HTTP_BAD_REQUEST;
				}
			}break;
		}
		nDataLen--;
		pHttpData++;
	}

	if(0 == nDataLen)
	{
		if('\r' == *pHttpData)
		{
			m_pVersionEnd = pHttpData;
			int nVersionLen = m_pVersionEnd - m_pVersionStart;
			int nMaxLen = max(nVersionLen, 3); /* "1.0" == 3 */
			if (strncmp(m_pVersionStart, "1.0", nMaxLen) != 0 && strncmp(m_pVersionStart, "1.1", nMaxLen) != 0)
				return W_HTTP_VERSION_NOT_SUPPORTED;
			goto RequestLineEnd;
		}
		else
		{
			return W_HTTP_BAD_REQUEST;
		}
	}

RequestLineEnd:
	return W_HTTP_PARSER_SUCCESS;
}
//head头信息写入m_mapHeader
int CHttpServer::HttpHeaderParser()
{
	int nDataLen = m_pHeaderEnd - m_pHeaderStart;
	const char* pHttpData = m_pHeaderStart;
	const char* pKeyStart = m_pHeaderStart;
	const char* pKeyEnd = NULL;
	const char* pValueStart = NULL;
	const char* pValueEnd = NULL;
	while(nDataLen > 0)
	{
		/* key */
		do
		{
			nDataLen--;
	        if(':' == *pHttpData++) break;
		}while(nDataLen > 0);

		pKeyEnd = pHttpData - 1;
		if(0 == nDataLen || pKeyEnd - pKeyStart == 0) 
			return W_HTTP_BAD_REQUEST;

		/* ' ' */
		while(nDataLen > 0 && ' ' == *pHttpData)
		{
	        nDataLen--;
			pHttpData++;
		}
		if(0 == nDataLen)
			return W_HTTP_BAD_REQUEST;
		pValueStart = pHttpData;

		/* value */
		do
		{
			nDataLen--;
			if('\r' == *pHttpData++) break;  /* "\r\n",func "HttpSplit" had check the '\n' */
		}while(nDataLen > 0);

		pValueEnd = pHttpData - 1;
		if (0 == nDataLen)
			pValueEnd = pHttpData;
		if(pValueEnd - pValueStart == 0)
			return W_HTTP_BAD_REQUEST;

		string keyStr(pKeyStart, pKeyEnd - pKeyStart);
		//std::transform(keyStr.begin(), keyStr.end(), keyStr.begin(), ::tolower); /* trans to lower byte. */
		m_mapHeader.insert(std::pair<string, string>(keyStr, string(pValueStart, pValueEnd - pValueStart)));
		pKeyStart = pHttpData + 1;
	}
	return W_HTTP_PARSER_SUCCESS;
}

int CHttpServer::HttpContentLengthParser()
{
	const char* pValue = GetHeaderValueForKey("content-length");
	int nContentLen = 0;
	if (pValue)
	{
		const char* pValueTmp = pValue;
		while (*pValueTmp)
		{
			if (*pValueTmp < '0' || *pValueTmp > '9')
				return W_HTTP_BAD_REQUEST;
			pValueTmp++;
		}

		nContentLen = atoi(pValue);
		if (nContentLen > HTTP_LIMIT_LENTH)
		{
			m_nStatusCode = W_HTTP_REQUESTENTITY_TOOLARGE;
			return m_nStatusCode;
		}

		if (nContentLen > m_nBodyLen)
		{
			m_nStatusCode = W_HTTP_PARSER_AGAIN;
			m_nBodyLen = nContentLen;
			return m_nStatusCode;
		}
		else if (nContentLen < m_nBodyLen)
		{
			m_nBodyLen = nContentLen;
		}
	}
	else
	{
		m_nBodyLen = 0;
	}

	return W_HTTP_PARSER_SUCCESS;
}
//获取请求的uri
int CHttpServer::HttpUriParser()
{
	int nDataLen = m_pUriEnd - m_pUriStart;
	const char* pHttpData = m_pUriStart;
	m_pPathStart = m_pUriStart;
	m_pPathEnd = m_pPathStart;
	while(nDataLen > 0)
	{
		nDataLen--;
		if('?' == *pHttpData++)
		{
			m_pPathEnd = pHttpData - 1;
	        break;
		}
	}
	if(0 == nDataLen) m_pPathEnd = pHttpData;

    int nPathLen = m_pPathEnd - m_pPathStart;
	//error:Get ?aldtype=85&keyfrom=alading HTTP/1.1
	if(0 == nPathLen) return W_HTTP_BAD_REQUEST;

	int nAddedLen = 0;
	char* pAddedStr = NULL;
	if('/' == *m_pPathStart) 
	{
		nAddedLen = strlen(W_HTTP_ROOT); /*  "/" -> "./"  */
		pAddedStr = W_HTTP_ROOT;
	}
	else if('.' != *m_pPathStart)
	{
		nAddedLen = strlen(W_HTTP_ROOT_PATH); /*  "" -> "./"  */
		pAddedStr = W_HTTP_ROOT_PATH;
	}
	if(nPathLen + nAddedLen >= URI_PATH_LEN) return W_HTTP_REQUEST_URI_TOOLARGE;

	if(pAddedStr) memcpy(m_szUriPath, pAddedStr, nAddedLen);
	memcpy(m_szUriPath + nAddedLen, m_pPathStart, nPathLen);
	nPathLen += nAddedLen;
	m_szUriPath[nPathLen] = 0;

	if (strstr(m_szUriPath, W_HTTP_DYNAMICPAGE_DIR))
		m_bIsDynamicPage = true;

	if (!m_bIsDynamicPage)
	{
		const char* pDot = strstr(m_szUriPath + nAddedLen, ".");
		if (!pDot)
		{
			if ('/' != m_szUriPath[nPathLen - 1])
			{
				nPathLen += 1;
				if (nPathLen >= URI_PATH_LEN) return W_HTTP_REQUEST_URI_TOOLARGE;
				m_szUriPath[nPathLen - 1] = '/';
			}
			int nPathLenTmp = nPathLen;
			int nDefaultLen = strlen(W_HTTP_PATH_DEFAULT);
			nPathLen += nDefaultLen;
			if (nPathLen >= URI_PATH_LEN) return W_HTTP_REQUEST_URI_TOOLARGE;
			memcpy(m_szUriPath + nPathLenTmp, W_HTTP_PATH_DEFAULT, nDefaultLen);
			m_szUriPath[nPathLen] = 0;
		}
	}
		
	return W_HTTP_PARSER_SUCCESS;
}

int CHttpServer::HttpChunkedParser()
{
	const char* pBodyData = m_pHeaderEnd + 4;
	bool bIsOver = false;
	int nBodyLenSave = m_nBodyLen;
	int nBodyLen = m_nBodyLen;
	char szLenBuff[64];

	while(nBodyLen > 0)
	{
		/* len */
		int nLenNums = 0;
		do
		{
			nBodyLen--;
			char byteValue = *pBodyData++;
			if('\r' == byteValue)
			{
				if(nBodyLen-- > 0)
				{
					if('\n' == *pBodyData++)
						break;
					else
						 return W_HTTP_BAD_REQUEST;
				}
				else
				{
			        return W_HTTP_PARSER_AGAIN;
				}
			}
			else
			{
		        if(byteValue >= '0' && byteValue <= '9')
				{
					szLenBuff[nLenNums] = byteValue;
					nLenNums++;
				}
				else 
				{
					return W_HTTP_BAD_REQUEST;
				}
			}
		}while(nBodyLen > 0);

		if (0 == nBodyLen || 0 == nLenNums)
			return W_HTTP_BAD_REQUEST;

		szLenBuff[nLenNums] = 0;
		int nCurChunkedBodyLen = atoi(szLenBuff);

		/* "\0\r\n\r\n", "\0\r\n had be readed,so here is "\r\n" !" */
		if(0 == nCurChunkedBodyLen)
		{
			if(nBodyLen >= 2)
			{
				nBodyLen -= 2;
				if('\r' == *pBodyData && '\n' == *(pBodyData+1))
				{
			        bIsOver = true;
					pBodyData += 2;
				    break;
				}
				else
				{
			        return W_HTTP_BAD_REQUEST;
				}
			}
			else
			{
		        return W_HTTP_PARSER_AGAIN;
			}
		}

		/* chunked body */
		nLenNums = 0;
		do
		{
			nBodyLen--;
			char byteValue = *pBodyData++;
			if('\r' == byteValue)
			{
				if(nBodyLen-- > 0)
				{
					if('\n' == *pBodyData++)
						break;
					else
						return W_HTTP_BAD_REQUEST;
				}
				else
				{
					return W_HTTP_PARSER_AGAIN;
				}
			}
			else
			{
		        nLenNums++;
			}
		}while(nBodyLen > 0);

		if(nLenNums != nCurChunkedBodyLen)
			return W_HTTP_BAD_REQUEST;
	}

	if (!bIsOver) 
	{
		return W_HTTP_PARSER_AGAIN;
	}

	m_nBodyLen = nBodyLenSave - nBodyLen;
	return W_HTTP_PARSER_SUCCESS;
}

//解析http信息 判断是否接受完成 
int CHttpServer::HttpParser(const char* pHttpData, int nDataLen)
{
	if (m_nStatusCode != W_HTTP_PARSER_AGAIN)
	    InitHttpData();
	else
		m_nBodyLen += nDataLen;

	m_pRequestLineStart = pHttpData;
	int nDataLenTmp = nDataLen;

	if (m_eHttpParserStatus < HTTP_PARSER_SPLIT)
	{
		//判断是否完全接收完， 没接收完整就继续等待下次的循环
		m_nStatusCode = HttpSplit(pHttpData, nDataLenTmp);
		if (m_nStatusCode != W_HTTP_PARSER_SUCCESS) return m_nStatusCode;
		m_eHttpParserStatus = HTTP_PARSER_SPLIT;
	}

	if (m_eHttpParserStatus < HTTP_PARSER_REQUESTLINE)//  获取请求的命令类型及获取请求的uri
	{
		m_nStatusCode = HttpRequestLineParser();
		if(m_nStatusCode != W_HTTP_PARSER_SUCCESS) return m_nStatusCode;
		m_eHttpParserStatus = HTTP_PARSER_REQUESTLINE;
	}

	if (m_eHttpParserStatus < HTTP_PARSER_HEADER)
	{
		m_nStatusCode = HttpHeaderParser();
		if (m_nStatusCode != W_HTTP_PARSER_SUCCESS) return m_nStatusCode;
		m_eHttpParserStatus = HTTP_PARSER_HEADER;
	}

	m_bIsChunked = false;
	const char* pValue = GetHeaderValueForKey("transfer-encoding");
	if (pValue)
	{
		const char *pMode = pValue;
		if (strcmp(pMode, "chunked") == 0) m_bIsChunked = true;
		if (strcmp(pMode, "identity") != 0 && !m_bIsChunked)
		{
			m_nStatusCode = W_HTTP_NOT_IMPLEMENTED;
			return m_nStatusCode;
		}
	}

	if (m_bIsChunked)
	{
		if (m_eHttpParserStatus < HTTP_PARSER_CONTENTLENGTH)
		{
			int nContentLen = 0;
			m_nStatusCode = HttpChunkedParser();
			if (m_nStatusCode != W_HTTP_PARSER_SUCCESS) return m_nStatusCode;
			m_eHttpParserStatus = HTTP_PARSER_CHUNKED;
			if (nContentLen > HTTP_LIMIT_LENTH)
			{
				m_nStatusCode = W_HTTP_REQUESTENTITY_TOOLARGE;
				return m_nStatusCode;
			}
		}
	}
	else
	{
		if (m_eHttpParserStatus < HTTP_PARSER_CONTENTLENGTH)
		{
			m_nStatusCode = HttpContentLengthParser();
			if (m_nStatusCode != W_HTTP_PARSER_SUCCESS) return m_nStatusCode;
			m_eHttpParserStatus = HTTP_PARSER_CONTENTLENGTH;
		}
	}

	if (m_eHttpParserStatus < HTTP_PARSER_URI)
	{
		m_nStatusCode = HttpUriParser();
		if (m_nStatusCode != W_HTTP_PARSER_SUCCESS) return m_nStatusCode;
		m_eHttpParserStatus = HTTP_PARSER_URI;
	}

	/* body callback */
	if (m_nBodyLen > 0 && m_requestBodyCallBackFunc)
	{
		m_requestBodyCallBackFunc(m_pHeaderEnd + 4, m_nBodyLen);
	}

	if (!m_bIsDynamicPage)
	{
		struct stat fileInfo;
		if (stat(m_szUriPath, &fileInfo) < 0) { m_nStatusCode = W_HTTP_NOT_FOUND; return m_nStatusCode; }
		if (!(S_ISREG(fileInfo.st_mode)) || !(S_IRUSR & fileInfo.st_mode)) { m_nStatusCode = W_HTTP_FORBIDDEN; return m_nStatusCode; }
		m_nPathFileSize = fileInfo.st_size;
	}

	//success
	m_nStatusCode = W_HTTP_OK;
	return m_nStatusCode;
	//return W_HTTP_PARSER_SUCCESS;
}

int CHttpServer::HttpResponseHeader(int nStatusCode, struct tagHttpResponseData* pHttpResponseData)
{
	int nSumWriteLen = 0;
	int nLeftWriteLen = HTTP_RESPONSE_LIMITLEN;
	int nPlanWriteLen = 0;

	W_HTTP_SNPRINTF_FORMAT(m_szResponseHeader + nSumWriteLen, nPlanWriteLen, nLeftWriteLen, "HTTP/1.1 %d %s\r\n", nStatusCode, GetStatusCodeName(nStatusCode));
	RESPONSE_HEADER_CHECK(nPlanWriteLen, nLeftWriteLen, nSumWriteLen);

	bool bIsKeepAlive = CheckHeaderValueForKey("Connection", "keep-alive");
	if (bIsKeepAlive)
	{
		pHttpResponseData->u.bIsKeepAlive = true;

		W_HTTP_SNPRINTF_FORMAT(m_szResponseHeader + nSumWriteLen, nPlanWriteLen, nLeftWriteLen, "Connection: keep-alive\r\n");
		RESPONSE_HEADER_CHECK(nPlanWriteLen, nLeftWriteLen, nSumWriteLen);

		W_HTTP_SNPRINTF_FORMAT(m_szResponseHeader + nSumWriteLen, nPlanWriteLen, nLeftWriteLen, "Keep-Alive: timeout=%d\r\n", W_HTTP_TIMEOUT_DEFAULT);
		RESPONSE_HEADER_CHECK(nPlanWriteLen, nLeftWriteLen, nSumWriteLen);
	}
	else
	{
		pHttpResponseData->u.bIsKeepAlive = false;

		W_HTTP_SNPRINTF_FORMAT(m_szResponseHeader + nSumWriteLen, nPlanWriteLen, nLeftWriteLen, "Connection: close\r\n");
		RESPONSE_HEADER_CHECK(nPlanWriteLen, nLeftWriteLen, nSumWriteLen);
	}

	if (W_HTTP_OK == m_nStatusCode)
	{
		// 检测请求文件的类型
		W_HTTP_SNPRINTF_FORMAT(m_szResponseHeader + nSumWriteLen, nPlanWriteLen, nLeftWriteLen, "Content-type: %s\r\n", GetMimeType());
		RESPONSE_HEADER_CHECK(nPlanWriteLen, nLeftWriteLen, nSumWriteLen);
	}
	else
	{
		W_HTTP_SNPRINTF_FORMAT(m_szResponseHeader + nSumWriteLen, nPlanWriteLen, nLeftWriteLen, "Content-type: text/html\r\n");
		RESPONSE_HEADER_CHECK(nPlanWriteLen, nLeftWriteLen, nSumWriteLen);
	}

	int nContentLen = GetResponseContentLength();
	if (nContentLen > 0)
	{
		W_HTTP_SNPRINTF_FORMAT(m_szResponseHeader + nSumWriteLen, nPlanWriteLen, nLeftWriteLen, "Content-length: %d\r\n", nContentLen);
		RESPONSE_HEADER_CHECK(nPlanWriteLen, nLeftWriteLen, nSumWriteLen);
	}

	W_HTTP_SNPRINTF_FORMAT(m_szResponseHeader + nSumWriteLen, nPlanWriteLen, nLeftWriteLen, "\r\n\0");
	RESPONSE_HEADER_CHECK(nPlanWriteLen, nLeftWriteLen, nSumWriteLen);

	pHttpResponseData->pBuff = m_szResponseHeader;
	pHttpResponseData->nBuffLen = strlen(m_szResponseHeader);
	return W_HTTP_RESPONSE_OK;
}

int CHttpServer::HttpResponseBody(struct tagHttpResponseData* pHttpResponseData)
{
	/* make error webpage or cgi webpage*/
	if (m_nReponseBodyLen > 0)
	{
		pHttpResponseData->pBuff = m_szResponseBody;
		pHttpResponseData->nBuffLen = m_nReponseBodyLen;
		return W_HTTP_RESPONSE_OK;
	}

	if(m_nPathFileSize <= 0)
		return W_HTTP_RESPONSE_ERROR;

#ifdef _MSC_VER
	// 在文件目录中找响应的资源
	HANDLE hFD = CreateFileA(m_szUriPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == hFD)
		return W_HTTP_RESPONSE_ERROR;

	HANDLE hFileMapping = CreateFileMapping(hFD, NULL, PAGE_READONLY, 0, 0, NULL);
	if(NULL == hFileMapping)
	{
		CloseHandle(hFD);
		return W_HTTP_RESPONSE_ERROR;
	}

	m_pFileMapAddr = (char*)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
	if(NULL == m_pFileMapAddr)
	{
		CloseHandle(hFD);
		CloseHandle(hFileMapping);
		return W_HTTP_RESPONSE_ERROR;
	}
	CloseHandle(hFD);
	CloseHandle(hFileMapping);
#else
	int nFD = ::open(m_szUriPath, O_RDONLY, 0);
	if(-1 == nFD)
		return W_HTTP_RESPONSE_ERROR;

	m_pFileMapAddr = mmap(NULL, m_nPathFileSize, PROT_READ, MAP_PRIVATE, nFD, 0);
	if(void*(-1) == m_pFileMapAddr)
	{
		m_pFileMapAddr = NULL;
		::close(nFD);
		return W_HTTP_RESPONSE_ERROR;
	}
	::close(nFD);
#endif

	pHttpResponseData->pBuff = m_pFileMapAddr;
	pHttpResponseData->nBuffLen = m_nPathFileSize;
	return W_HTTP_RESPONSE_OK;
}

void CHttpServer::HttpResponseBodyOver()
{
	if(m_pFileMapAddr)
	{
#ifdef _MSC_VER
		UnmapViewOfFile(m_pFileMapAddr);
#else
		munmap(m_pFileMapAddr, m_nPathFileSize);
#endif
		m_pFileMapAddr = NULL;
	}
}

void CHttpServer::SetHttpRequestBodyCallBack(HttpRequestBodyCallBack callBackFunc)
{
	m_requestBodyCallBackFunc = callBackFunc;
}