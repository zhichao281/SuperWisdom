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

#include "helpFunction.h"


// 主函数
int main()
{
	// 初始化windows socket
	WSADATA wd;
	SOCKET s;
	if (WSAStartup(MAKEWORD(2, 2), &wd) < 0)
	{
		fprintf(stderr, "winsock startup failed\n");
		exit(-1);
	}

	// 创建服务器套接字
	s = socket(AF_INET, SOCK_STREAM, 0);

	// 绑定并监听
	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(827);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int ret = bind(s, (struct sockaddr*)&addr, sizeof(addr));
	if (ret < 0)
	{
		fprintf(stderr, "bind failed\n");
		closesocket(s);
		exit(-1);
	}

	// 进行监听
	ret = listen(s, 1024);
	if (ret < 0)
	{
		fprintf(stderr, "listen failed\n");
		closesocket(s);
		exit(-1);
	}

	char request[8000] = { 0 };  // 请求
	while (true)
	{
		// 连接到客户端的套接字
		SOCKET clientSocket = accept(s, 0, 0);

		// 接受客户端的信息
		recv(clientSocket, request, sizeof(request), 0);

		// 将客户端的信息输出到终端
		printf(request);

		// 处理请求，主要功能函数
		HandleRequestLine(clientSocket, request);
	}

	return 0;
}
