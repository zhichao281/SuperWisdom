#include <windows.h>
#include <io.h>
#include <process.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include "debug_log.h"
#include <winsock2.h>

#include "network_interface.h"

#pragma comment ( lib , "ws2_32" )


Network_Interface *Network_Interface::m_network_interface = NULL;

Network_Interface::Network_Interface():
		epollfd_(0),
		listenfd_(0),
		websocket_handler_map_()
{
	if(0 != init())
		exit(1);
}

Network_Interface::~Network_Interface()
{
	::closesocket(listenfd_);
	WSACleanup();

}
// 初始化windows socket
int Network_Interface::init()
{
	WSADATA wsaData;

	int err = WSAStartup(0x202, &wsaData); 
	if (err != 0)
	{ 
		DEBUG_LOG("winsock startup failed");
		return -1;
	}
	//检测是否支持这个版本的socket 
	else if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
	{
		WSACleanup( ); 
		DEBUG_LOG("winsock version error");
		return -1;
	}


	// 创建服务器套接字
	listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd_ == -1){
		DEBUG_LOG("创建套接字失败!");
		return -1;
	}

	// 绑定并监听
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(INADDR_ANY);
	int ret = bind(listenfd_, (struct sockaddr*)(&server_addr), sizeof(server_addr));
	if (ret < 0)
	{
		DEBUG_LOG("绑定套接字失败!");
		closesocket(listenfd_);
		return -1;
	}
	// 进行监听
	ret = listen(listenfd_, 1024);
	if (ret < 0)
	{
		DEBUG_LOG("监听失败!");
		closesocket(listenfd_);
		return -1;
	}

	DEBUG_LOG("服务器启动成功!");
	return 0;
}

int Network_Interface::epoll_loop()
{


}


void Network_Interface::run()
{
	epoll_loop();
}
Network_Interface *Network_Interface::get_share_network_interface()
{
	if (m_network_interface == NULL)
		m_network_interface = new Network_Interface();
	return m_network_interface;
}