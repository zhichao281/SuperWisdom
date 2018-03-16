// Mini FtpServer.cpp : Defines the entry point for the console application.

#include "thread_pool.h"

#include <iostream>
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


// 主函数，控制台程序开始的地方
void main(void)
{
	GSThreadPool_AddFun(std::bind
	(
	[=](int x, int y)
	{
		x++;
		std::cout << "thread:" << x << y << endl;
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



