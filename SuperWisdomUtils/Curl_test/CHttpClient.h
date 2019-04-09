/*****************************************
*封装Libcurl下载库
*/
#pragma once
//libcurl采用静态库引用
#define  CURL_STATICLIB

#include "include/curl/curl.h"
#include <string>

using std::string;


class CLibcurlCallback
{
public:
	// 虚函数，进度显示
	virtual void Progress(void* lpParam, double dTotal, double bLoaded) = 0;

	// 写数据回调函数
	virtual void	WriteCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam) = 0;

};

enum LibcurlFlag
{
	Lf_None = 0,
	Lf_Download,
	Lf_Post,
	Lf_Get,
};

class CLibcurl
{
public:
	CLibcurl(void);
	~CLibcurl(void);
	/******************封装类的外部调用接口************/

	//************************************
	// Method:    设置连接端口号
	// Returns:   bool
	// Parameter: LONG port
	//************************************
	bool SetPort(LONG port);
	bool SetTimeout(int nSecond);										//设置执行超时（秒）
	bool SetConnectTimeout(int nSecond);								//设置连接超时（秒）
	bool SetUserAgent(LPCSTR lpAgent);									//设置用户代理
	bool SetResumeFrom(LONG lPos);										//设置断点续传起始位置
	bool SetResumeFromLarge(LONGLONG llPos);							//设置断点续传起始位置，针对大文件
	bool AddHeader(LPCSTR lpKey, LPCSTR lpValue);						//添加自定义头
	void ClearHeaderList();												//清理HTTP列表头
	bool SetCookie(LPCSTR lpCookie);									//设置HTTP请求cookie
	bool SetCookieFile(LPCSTR lpFilePath);								//设置HTTP请求cookie文件
	const char* GetError()const;
	//获取错误详细信息
	void SetCallback(CLibcurlCallback* pCallback, void* lpParam);		//设置下载进度回调




																		/**
																		* @brief HTTP 异步下载文件到磁盘
																		* @param lpUrl 输入参数,请求的Url地址,如:http://www.baidu.com
																		* @param lpFile 输出参数,返回的内容
																		* @return 返回是否Post成功
																		*/
	bool AsynDownloadToFile(LPCSTR lpUrl, LPCSTR lpFile);

	/**
	* @brief HTTP 同步下载文件到磁盘
	* @param lpUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param lpFile 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	bool DownloadToFile(LPCSTR lpUrl, LPCSTR lpFile);

	//Post 字符串数据
	bool Post(LPCSTR lpUrl, LPCSTR lpData);

	// 发送Post请求，返回字符串
	std::string Post2(std::string strUrl, std::string strData);

	// 发送Post请求，返回字符串
	std::string Posts2(std::string strUrl, std::string strData);


	//Post 字符串或者二进制数据
	bool Post(LPCSTR lpUrl, unsigned char* lpData, unsigned int nSize);

	//异步执行 Get 请求
	bool AsynGet(LPCSTR lpUrl);

	// 同步执行发送Get请求，等待返回字符串
	string Get(LPCSTR strUrl);

	//获取Post/Get请求返回数据
	const string& GetRespons()const;

	//获取Post/Get请求返回数据
	const char*	GetResponsPtr()const;

	// 获取响应代码
	int		GetResponseCode();

protected:
	// 写数据回调函数
	static size_t	WriteCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);

	// 返回头部信息回调
	static size_t	ResponseHeadCallBack(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);

	// 虚函数，进度显示
	static int		ProgressCallback(void *pParam, double dltotal, double dlnow, double ultotal, double ulnow);

private:
	CURL * m_pCurl;
	LONG		m_nPort;
	HANDLE		m_hFile;
	CURLcode	m_curlCode;
	string		m_strRespons;
	LibcurlFlag m_lfFlag;
	curl_slist *m_curlList;
	void		*m_pCallbackParam;
	CLibcurlCallback	*m_pCallback;

	// 响应代码
	int				m_nResponseCode;
};
