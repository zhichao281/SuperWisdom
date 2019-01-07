
#include <iostream>
#include <list>
#include <string>
using namespace std;
//双端队列
class Deque
{
public:
	void push_back(int x) { cout<<"Deque push_back"<<endl; }
	void push_front(int x) { cout<<"Deque push_front"<<endl; }
	void pop_back() { cout<<"Deque pop_back"<<endl; }
	void pop_front() { cout<<"Deque pop_front"<<endl; }
};
//顺序容器
class Sequence
{
public:
	virtual void push(int x) = 0;
	virtual void pop() = 0;
};
//栈
class Stack: public Sequence
{
public:
	void push(int x) { deque.push_back(x); }
	void pop() { deque.pop_back(); }
private:
	Deque deque; //双端队列
};
//队列
class Queue: public Sequence
{
public:
	void push(int x) { deque.push_front(x); }
	void pop() { deque.pop_front(); }
private:
	Deque deque; //双端队列
};

#include <tchar.h>
#include "CHttpClient.h"
#include "ZipCompress.h"
void DownloadTest();
void PostTest();
void GetTest();
int main()
{
	time_t start, stop;
	start = time(NULL);
	CZipCompress zipObj;
	zipObj.Zip_UnPackFiles(L"E:\\1.zip",L"E:\\2");
	stop = time(NULL);
	printf("Use Time:%ld\n", (stop - start));


	
	system("pause");
	GetTest();
 	Sequence *s1 = new Stack();
  	Sequence *s2 = new Queue();
	s1->push(1); s1->pop();
	s2->push(1); s2->pop();
	delete s1; delete s2;
	return 0;
}


// LibcurlLoad.cpp : 定义控制台应用程序的入口点。
//



class CLibcurlCallbackEx
	: public CLibcurlCallback
{
public:
	virtual void Progress(void* lpParam, double dTotal, double dLoaded)
	{
		if (dTotal == 0.0)
			return;
		double bPercent = (dLoaded / dTotal) * 100;
		printf("下载进度：%0.2lf%%\n", bPercent);
	}
	virtual void	WriteCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
	{
		m_strBody.append((char *)pBuffer, nSize * nMemByte);
		m_nSize = nSize * nMemByte;
	}
public:
	// 接收数据
	std::string		m_strBody;
	int  m_nSize;

};




void DownloadTest()
{
	const char* pUrl = "http://doc.cloudv.haplat.net/160612/ovptest/201804/f1ae15a601621000d6b2ed9000000000/f1ae15a601621000d6b2ed9000000000_1.png";
	CLibcurl libcurl;
	CLibcurlCallbackEx cb;
	//libcurl.SetUserAgent("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.86 Safari/537.36");
	libcurl.SetConnectTimeout(2);
	libcurl.SetResumeFrom(0);
	libcurl.SetCallback(&cb, NULL);
	libcurl.DownloadToFile(pUrl, "d:\\1.png");
	std::string	strBody=cb.m_strBody;



	FILE* wfile;
	wfile = _wfopen(L"D:\\2.png", L"wb");

	if (strBody.size() > 0)
	{
		int nsize = strBody.size();
		int nsize1 = cb.m_nSize;
		//写人数据到文件中  
		fwrite(strBody.data(), strBody.size(), 1, wfile);
		strBody.clear();
	}
	fclose(wfile);
}

void PostTest()
{
	CLibcurl libcurl;
	libcurl.SetUserAgent("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.86 Safari/537.36");
	libcurl.SetPort(80);
	libcurl.SetConnectTimeout(2);
	libcurl.AddHeader("name", "Jelin");
	libcurl.AddHeader("sex", "man");
	libcurl.SetCookieFile("c:\\cookie");
	char* pData = "maintype=10001&subtype=100&appver=2.5.19.3753&sysver=Microsoft Windows 7&applist=100:15,200:2&sign=2553d888bc922275eca2fc539a5f0c1b";
	libcurl.Post("http://interface.***********.com/v2/stat/index/jingpin", pData);
	string strRet = libcurl.GetRespons();

}

void GetTest()
{
	CLibcurl libcurl;
	//libcurl.SetUserAgent("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.86 Safari/537.36");
	libcurl.Get("http://doc.cloudv.haplat.net/160612/ovptest/201804/f1ae15a601621000d6b2ed9000000000/f1ae15a601621000d6b2ed9000000000_1.png");


	std::string	strBody = libcurl.GetRespons();
	FILE* wfile;
	wfile = _wfopen(L"D:\\2.png", L"wb");

	if (strBody.size() > 0)
	{
		int nsize = strBody.size();
		//写人数据到文件中  
		fwrite(strBody.data(), strBody.size(), 1, wfile);
		strBody.clear();
	}
	fclose(wfile);
	
	const char* pError = libcurl.GetError();
}









