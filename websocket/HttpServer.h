#ifndef __WEBSERVER__H__
#define __WEBSERVER__H__
#include <unordered_map>
#include <string>
#ifdef _MSC_VER
#include <Windows.h>
#endif
using namespace std;

#define W_HTTP_ROOT_PATH "./"
#define W_HTTP_ROOT "."
#define W_HTTP_PATH_DEFAULT       "login.html"
#define W_HTTP_DYNAMICPAGE_DIR    "FunnyCore"
#define W_HTTP_CGIBIN_LINUX       "./cgi-bin/webcgi.exe"
#define W_HTTP_CGIBIN_WINDOWS      ".\\cgi-bin\\webcgi"

#define W_HTTP_TIMEOUT_DEFAULT 500     /* ms */

#define W_HTTP_PARSER_SUCCESS      0
#define W_HTTP_PARSER_UNKNOWNERR   -1
#define W_HTTP_PARSER_AGAIN        -2

#define W_HTTP_CONTINUE  100
#define W_HTTP_SWITCHING_PROTOCOLS 101
#define W_HTTP_OK 200
#define W_HTTP_CREATED 201
#define W_HTTP_ACCEPTED 202
#define W_HTTP_NOAUTHORITATIVE_INFO 203
#define W_HTTP_NO_CONTENT 204
#define W_HTTP_RESET_CONTENT 205
#define W_HTTP_PARTIAL_CONTENT 206
#define W_HTTP_MULTIPLE_CHOICES 300
#define W_HTTP_MOVED_PERMANENTLY 301
#define W_HTTP_FOUND 302
#define W_HTTP_SEE_OTHER 303
#define W_HTTP_NOT_MODIFIED 304
#define W_HTTP_USE_PROXY 305
#define W_HTTP_TEMPORARY_REDIRECT 307
#define W_HTTP_BAD_REQUEST 400
#define W_HTTP_UNAUTHORIZED 401
#define W_HTTP_FORBIDDEN 403
#define W_HTTP_NOT_FOUND 404
#define W_HTTP_NOT_ALLOWED 405
#define W_HTTP_NOT_ACCEPTABLE 406
#define W_HTTP_AUTHENTICATION_REQUIRED 407
#define W_HTTP_REQUEST_TIMEOUT 408
#define W_HTTP_CONFLICT 409
#define W_HTTP_GONE 410
#define W_HTTP_LENGTH_REQUIRED 411
#define W_HTTP_PRECONDITION_FAILED 412
#define W_HTTP_REQUESTENTITY_TOOLARGE 413
#define W_HTTP_REQUEST_URI_TOOLARGE 414
#define W_HTTP_UNSUPPORTED_MEDIATYPE 415
#define W_HTTP_REQUESTEDRANGE_NOTSATISFIABLE 416
#define W_HTTP_EXPECTATION_FAILED 417
#define W_HTTP_INTERNAL_SERVERERROR 500
#define W_HTTP_NOT_IMPLEMENTED 501
#define W_HTTP_BAD_GATEWAY 502
#define W_HTTP_SERVICE_UNAVAILABLE 503
#define W_HTTP_GATEWAY_TIMEOUT 504
#define W_HTTP_VERSION_NOT_SUPPORTED 505


#define W_HTTP_RESPONSE_ERROR      0
#define W_HTTP_RESPONSE_OK         1
#define W_HTTP_RESPONSE_TOOLARGE   2

struct tagUnorderMapHash
{
	size_t operator() (const char *str) const
	{
		std::hash<char*> hashStr;
		return hashStr((char*)str);
	}
};

struct tagMapCmp
{
	bool operator() (char const *a, char const *b) const
	{
		return std::strcmp(a, b) < 0;
	}
};

struct tagHttpResponseData
{
	const char* pBuff;
	int nBuffLen;
	union
	{
		bool bIsKeepAlive;
	}u;
};

typedef void (*HttpRequestBodyCallBack)(const char* pBodyData, int nBodyLen);

class CHttpServer
{
#define HTTP_HEADER_LIMIT  1024
#define HTTP_BODY_LIMIT    7168
#define HTTP_LIMIT_LENTH   8192
#define HTTP_RESPONSE_LIMITLEN   8192
#define HTTP_RESPONSE_BODYLEN    4096

enum HTTP_METHOD
{
	HTTP_GET = 1,
	HTTP_POST
}E_HTTP_METHOD;

enum HTTP_PARSER_STATUS
{
	HTTP_PARSER_START = 1,
	HTTP_PARSER_SPLIT,
	HTTP_PARSER_REQUESTLINE,
	HTTP_PARSER_HEADER,
	HTTP_PARSER_CHUNKED,
	HTTP_PARSER_CONTENTLENGTH,
	HTTP_PARSER_URI,
} E_HTTP_PARSER_STATUS;

public:
	CHttpServer();
	~CHttpServer();
private:
	//  获取请求的命令类型
	HTTP_METHOD m_eMethod; /* GET/POST and so on */

	HTTP_PARSER_STATUS m_eHttpParserStatus;

	const char* m_pMethodEnd;
	const char* m_pUriStart;
	const char* m_pUriEnd;
	const char* m_pPathStart;
	const char* m_pPathEnd;
	const char* m_pVersionStart;
	const char* m_pVersionEnd;
	const char* m_pRequestLineStart;
	const char* m_pRequestLineEnd;
	const char* m_pHeaderStart;
	const char* m_pHeaderEnd;
	//const char* m_pUriFileTypeDotStart; /*for example: "/index.html" the dot is at index 7.*/

	int  m_nStatusCode;
	bool m_bIsChunked;
	bool m_bIsDynamicPage; 	// 判断URI请求的是否为静态请求
	int  m_nBodyLen;
	char *m_szUriPath;   	//获取请求的uri
	char *m_szResponseHeader;
	char *m_szResponseBody;
	char *m_pFileMapAddr;   //uri对应的文件写入内存
	int  m_nReponseBodyLen;
	int  m_nPathFileSize;//uri对应的文件的长度
	std::unordered_map<string, string> m_mapHeader;//head头对应的map值
	std::unordered_map<int, const char*> m_mapErrorInfo;
	HttpRequestBodyCallBack m_requestBodyCallBackFunc;
private:
	void  InitHttpData();
	void  InitErrorInfo();
private:
	//根据错误码 返回具体的错误信息
	const char* GetStatusCodeName(int nStatusCode);

	/**
	* 根据key获取对应http  header的value
	*
	* @write Headers in form of std::map (name -> value)
	*/
	const char* GetHeaderValueForKey(char* pKey);

	/**
	* 检测http的对应key值是否存在
	*
	*/
	bool  CheckHeaderValueForKey(char* pKey, char* pValue);

	int   GetResponseContentLength();

	// 获取文件类型
	const char* GetMimeType(char* pExtendName = NULL);

	int   HttpSplit(const char* pHttpData, int nDataLen);
	
	int   HttpRequestLineParser();

	/**
	* Get extracted headers.
	*
	* @write Headers in form of std::map (name -> value)
	*/
	int   HttpHeaderParser();

	int   HttpContentLengthParser();
	int   HttpUriParser();
	int   HttpChunkedParser();
public:
	// 解析请求(Parse Request )
	int   HttpParser(const char* pHttpData, int nDataLen);

	int   HttpResponseHeader(int nStatusCode, struct tagHttpResponseData* pHttpResponseData);

	int   HttpResponseBody(struct tagHttpResponseData* pHttpResponseData);

	void  HttpResponseBodyOver();

	void  SetHttpRequestBodyCallBack(HttpRequestBodyCallBack callBackFunc);

};

#endif