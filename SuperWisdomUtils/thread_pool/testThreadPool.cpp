// Mini FtpServer.cpp : Defines the entry point for the console application.



#pragma execution_character_set("utf-8")
#include <iostream>
#include <tchar.h>
#include <string>
using namespace std;
void fun(std::string s)
{
	std::cout << "thread:" << s << endl;
	for (int i = 0; i < 20; ++i)
	{
		std::cout << "s:" << i << endl;
	}

}

void fun1(int  s, int s2)
{
	std::cout << "thread:" << s << s2 << endl;


}
void test()
{
	std::string s = "abcd";

	try {
		s[100] = 'b';
	}
	catch (std::exception& e)
	{
		std::cout << "with exception:[" << e.what() << "]" << std::endl;
	}
	catch (...)
	{
		std::cout << "with unknown exception" << std::endl;
	}
}


#include "../Json/json/json.h"
#include <iostream>
#include <fstream>
// 主函数，控制台程序开始的地方
//char* UnicodeToUtf8(const wchar_t* unicode)
//{
//	int len;
//	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
//	char *szUtf8 = (char*)malloc(len + 1);
//	memset(szUtf8, 0, len + 1);
//	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szUtf8, len, NULL, NULL);
//	return szUtf8;
//}
#include <codecvt>
#include <locale> 
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
#include "../log/Logger.h"
#include "../log/LogManager.h"
#include "thread_pool.h"

void threadmain(void)
{
	CLogManager::GetInstance();
	g_logPtr.reset(new Logger);
	LOG_INFO("main");
	GSThreadPool_AddFun(std::bind
	(
		[=](int x, int y)
	{
		x = x + 1;
		std::cout << "中文dsf :" << x << y << endl;
	}, 5, 7));
	system("pause");



	return ;
	Json::Value jsonRoot; //定义根节点
	Json::Value jsonItem; //定义一个子对象
	jsonItem["item1"] = "中文"; //添加数据
	jsonItem["item2"] = 2;
	jsonRoot.append(jsonItem);
	//jsonItem.clear(); //清除jsonItem
	//jsonItem["item1.0"] = 1.0;
	//jsonItem["item2.0"] = 2.0;
	//jsonRoot["item"] = jsonItem;
	cout << jsonItem.toStyledString() << endl; //输出到控制台
	ofstream ofs; //标准输出流
	ofs.open("sample.json"); //创建文件
	ofs << jsonItem.toStyledString(); //输出
	ofs.close();

	ifstream ifs; //标准输入流
	ifs.open("sample.json");
	jsonRoot.clear();

	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, ifs, &jsonRoot, &errs)) //从ifs中读取数据到jsonRoot
	{
		return;
	}
	string str1 = jsonRoot["item1"].asString();









	int x2 = 342;

	GSThreadPool_AddFun(std::bind
	(
	[=](int x, int y)
	{
		x=x+x2;
		std::cout << "中文dsf :" << x << y << endl;
	}, 5, 7));

//	GSThreadPool_Run();

	system("pause");
	return;

	netlib::ThreadPool pool(10);
	pool.start();
	int num = 0;
	//while (1)
	//{
	//	char pTest[256];
	//	sprintf(pTest, "hello-%d", num);
	//	std::string str1 = pTest;
	//	pool.append(std::bind(fun, str1));
	//	num++;
	//	if (num>10)
	//	{
	//		break;
	//	}
	//}


	pool.append(std::bind
	(
		[=](int x, int y)
	{
		x++;
		std::cout << "thread:" << x << y << endl;
	}, 5, 7));


	//GSThreadPool_AddFun(
	//{
	//	int x=0;
	//	int y=0;
	//	std::cout << "test";	
	//
	//}, NULL);

	//GSThreadPool_AddFun::GetInstance()->Start();


	pool.stop();
	//CFtpAppSingleton->AsynStart();
	system("pause");

}



