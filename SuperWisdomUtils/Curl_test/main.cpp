


#include <windows.h>
#include <iostream>
#include "include/curl/curl.h"
#include <string.h>
#include <codecvt>
#include <locale>
#include <vector>
using namespace std;
std::string Utf2Ansi(const std::string & str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}
std::string gb2312_to_utf8(std::string const &strGb2312)
{
	std::vector<wchar_t> buff(strGb2312.size());
#ifdef _MSC_VER
	std::locale loc("zh-CN");
#else
	std::locale loc("zh_CN.GB18030");
#endif
	wchar_t* pwszNext = nullptr;
	const char* pszNext = nullptr;
	mbstate_t state = {};
	int res = std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >
		(loc).in(state,
			strGb2312.data(), strGb2312.data() + strGb2312.size(), pszNext,
			buff.data(), buff.data() + buff.size(), pwszNext);

	if (std::codecvt_base::ok == res)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
		return cutf8.to_bytes(std::wstring(buff.data(), pwszNext));
	}

	return "";

}
std::string utf8_to_gb2312(std::string const &strUtf8)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
	std::wstring wTemp = cutf8.from_bytes(strUtf8);
#ifdef _MSC_VER
	std::locale loc("zh-CN");
#else
	std::locale loc("zh_CN.GB18030");
#endif
	const wchar_t* pwszNext = nullptr;
	char* pszNext = nullptr;
	mbstate_t state = {};

	std::vector<char> buff(wTemp.size() * 2);
	int res = std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >
		(loc).out(state,
			wTemp.data(), wTemp.data() + wTemp.size(), pwszNext,
			buff.data(), buff.data() + buff.size(), pszNext);

	if (std::codecvt_base::ok == res)
	{
		return std::string(buff.data(), pszNext);
	}
	return "";
}
//回调函数  得到响应内容
int write_data(void* buffer, int size, int nmemb, void* userp) {
	std::string * str = dynamic_cast<std::string *>((std::string *)userp);
	str->append((char *)buffer, size * nmemb);
	return nmemb;
}
int upload(string url, string &body, string* response);

int main(int argc, char** argv) {

	std::string body;
	std::string response;

	int status_code = upload("http://120.78.180.184:10086/api?apiname=bit.api.dianzhenzhi.uploadteacherpdf&params=%7B++%22addressJson%22+%3A+%221536.675.59.95%2C1536.675.59.99%22%2C++%22fileName%22+%3A+%22E%3A%5C%5Cstudy%5C%5Clove-study%5C%5CXClassTool-demo%5C%5CXclass%5C%5C2019-08-15%5C%5CTemp%5C%5C%E9%94%99%E9%98%BF%E6%96%AF%E8%BE%BE_140606961.pdf%22%2C++%22liencesId%22+%3A+%225d2fd9a356f8012d0235856c%22%2C++%22pageCount%22+%3A+4%2C++%22pdfId%22+%3A+%22347a4c1166e1469b813f8537f9982d19%22+%7D+&token=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJwYXlsb2FkIjoie1wiaWRcIjpcIjBkNzNkMjllZTViYzRiOTE5NmNlNGMzODYzN2U5YzIwXCIsXCJ1c2VySWRcIjpcIjEwMlwiLFwiYWNjZXNzVG9rZW5cIjpudWxsLFwiY3JlYXRlZFRpbWVcIjoxNTY1ODQ5MzA0MDE3LFwiZXhwaXJlc1RpbWVcIjoxNTY1OTM1NzA0MDE3LFwiY2xpZW50SXBcIjpudWxsLFwiY2xpZW50VHlwZVwiOm51bGwsXCJlQ29kZVwiOm51bGwsXCJ1Q29kZVwiOm51bGx9In0.xdGiHKvDix06pXPhNN73RFLpieobePmRlTR0-ijfHoE", 
		body, &response);
	if (status_code != CURLcode::CURLE_OK) {
		return -1;
	}
	body = Utf2Ansi(body);
	response = Utf2Ansi(response);
	return 0;
}
#include <tchar.h>
int upload(string url, string &body, string* response)
{

	ShellExecute(NULL, _T("open"), _T("E:/"), _T(""), _T(""), SW_SHOW);


	 url = gb2312_to_utf8(url);
	CURL *curl;
	CURLcode ret;
	curl = curl_easy_init();
	struct curl_httppost* post = NULL;
	struct curl_httppost* last = NULL;
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, (char *)url.c_str());           //指定url
		//curl_formadd(&post, &last, CURLFORM_PTRNAME, "path", CURLFORM_PTRCONTENTS, "device_cover", CURLFORM_END);//form-data key(path) 和 value(device_cover)
		//curl_formadd(&post, &last, 
		//	CURLFORM_PTRNAME, "file",
		//	CURLFORM_FILE, "E:\\study\\love-study\\XClassTool-demo\\Xclass\\2019-08-15\\Temp\\错阿斯达_141530577.pdf", 

		//	 CURLFORM_END);// form-data key(file) "./test.jpg"为文件路径  "hello.jpg" 为文件上传时文件名
		curl_formadd(&post, &last,
			CURLFORM_PTRNAME, "错阿斯达_141530577.pdf",
			CURLFORM_FILE, "E:\\study\\love-study\\XClassTool-demo\\Xclass\\2019-08-15\\Temp\\错阿斯达_141530577.pdf",
			CURLFORM_END);

		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);                     //构造post参数    
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);          //绑定相应
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);        //绑定响应内容的地址

		ret = curl_easy_perform(curl);                          //执行请求
		if (ret == 0) {
			curl_easy_cleanup(curl);
			return 0;
		}
		else {
			return ret;
		}
	}
	else {
		return -1;
	}
}


//int upload(string url, string &body, string* response)
//{
//	CURL *curl;
//	CURLcode ret;
//	curl = curl_easy_init();
//	struct curl_httppost* post = NULL;
//	struct curl_httppost* last = NULL;
//	if (curl)
//	{
//
//		struct curl_slist *headers = NULL;
//		//headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
//	
//		//set headers
//		//curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//
//		curl_easy_setopt(curl, CURLOPT_URL, (char *)url.c_str());           //指定url
//
//
//		//curl_formadd(&post, &last,
//		//	CURLFORM_COPYNAME, "uploadfile",
//		//	CURLFORM_FILE, "D:/1.pdf",//imagepath
//		//	CURLFORM_CONTENTTYPE, "application/pdf",
//		//	CURLFORM_END);
//
//		//curl_formadd(&post, &last,
//		//	CURLFORM_COPYNAME, "filename",
//		//	CURLFORM_COPYCONTENTS, "1.pdf",
//		//	CURLFORM_END);
//	/*	HANDLE hFile;
//		hFile = CreateFileA("d:/1.pdf", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//
//		if (hFile == INVALID_HANDLE_VALUE)
//		{
//			return false;
//		}
//
//		DWORD dFileSize = GetFileSize(hFile, NULL);
//		char * pBuffer = new char[dFileSize + 1];
//
//		if (pBuffer == NULL)
//			return false;
//
//		memset(pBuffer, 0, dFileSize);
//
//		DWORD dReadSize(0);
//		if (!ReadFile(hFile, pBuffer, dFileSize, &dReadSize, NULL))
//		{
//			delete[]pBuffer;
//			CloseHandle(hFile);
//			return false;
//		}
//
//
//
//		curl_formadd(&post, &last,
//			CURLFORM_COPYNAME, "file",
//			CURLFORM_BUFFER, "1.pdf",
//			CURLFORM_BUFFERPTR, pBuffer,
//			CURLFORM_BUFFERLENGTH, dReadSize,
//			CURLFORM_CONTENTTYPE, "application/pdf",
//			CURLFORM_END);
//*/
//		curl_formadd(&post, &last, 
//			CURLFORM_PTRNAME, "file",
//			CURLFORM_FILE, "D:/1.pdf", 
//			CURLFORM_FILENAME, "1.pdf",
//			CURLFORM_CONTENTTYPE, "application/pdf",
//			CURLFORM_END);// form-data key(file) "./test.jpg"为文件路径  "hello.jpg" 为文件上传时文件名
//		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);                     //构造post参数    
//		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);          //绑定相应
//		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);        //绑定响应内容的地址
//
//		ret = curl_easy_perform(curl);                          //执行请求
//		if (ret == 0) {
//			curl_easy_cleanup(curl);
//			return 0;
//		}
//		else {
//			return ret;
//		}
//	}
//	else {
//		return -1;
//	}
//}
