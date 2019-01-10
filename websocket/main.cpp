//#include <iostream>
//#include "serverlistener.h"
//
//int main() {
//    ServerListener().run();
//    return 0;
//}
//


//////////////////////////////////////////////////
//
//      创建一个很小的WebServer，基于HTTPS协议
//
/////////////////////////////////////////////////////
//////////////////
//
//#include "helpFunction.h"
//
//
//// 主函数
//int main()
//{
//	// 初始化windows socket
//	WSADATA wd;
//	SOCKET s;
//	if (WSAStartup(MAKEWORD(2, 2), &wd) < 0)
//	{
//		fprintf(stderr, "winsock startup failed\n");
//		exit(-1);
//	}
//
//	// 创建服务器套接字
//	s = socket(AF_INET, SOCK_STREAM, 0);
//
//	// 绑定并监听
//	SOCKADDR_IN addr;
//	memset(&addr, 0, sizeof(addr));
//	addr.sin_family = AF_INET;
//	addr.sin_port = htons(827);
//	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//	int ret = bind(s, (struct sockaddr*)&addr, sizeof(addr));
//	if (ret < 0)
//	{
//		fprintf(stderr, "bind failed\n");
//		closesocket(s);
//		exit(-1);
//	}
//
//	// 进行监听
//	ret = listen(s, 1024);
//	if (ret < 0)
//	{
//		fprintf(stderr, "listen failed\n");
//		closesocket(s);
//		exit(-1);
//	}
//
//	char request[8000] = { 0 };  // 请求
//	while (true)
//	{
//		// 连接到客户端的套接字
//		SOCKET clientSocket = accept(s, 0, 0);
//
//		// 接受客户端的信息
//		recv(clientSocket, request, sizeof(request), 0);
//
//		// 将客户端的信息输出到终端
//		printf(request);
//
//		// 处理请求，主要功能函数
//		HandleRequestLine(clientSocket, request);
//	}
//
//	return 0;
//}



// webserver.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"



/*
测试方式(依次在浏览器输入一下内容进行不同情形的测试) :
127.0.0.1 : 9988
127.0.0.1 : 9988 / hehe.html
127.0.0.1 : 9988 / login.html
127.0.0.1 : 9988 / haha ? x = 10 & y = 20

也可以下载fiddler工具进行更全面的模拟测试!


1、支持GET / POST方法
2、支持CGI
3、支持错误返回
4、支持chunked数据的解析

缺点 :
1、支持的方法不够全面
2、CGI实现不够科学，可以尝试使用FASTCGI
3、不支持https
4、不支持回应chunked数据

*/


#include <tchar.h>

#include "ServerListener.h"



void handleRequestBody(const char* pBodyData, int nBodyLen)
{
	printf("------handleRequestBody----\n");
	char buff[65535] = { 0 };
	memcpy(buff, pBodyData, min(nBodyLen, 65535));
	printf_s("%s\n", buff);
}


int _tmain(int argc, _TCHAR* argv[])
{
	HTTP_SERVER->SetHttpRequestBodyCallBack(handleRequestBody);
	HTTP_SERVER->run();
	return 0;
}


