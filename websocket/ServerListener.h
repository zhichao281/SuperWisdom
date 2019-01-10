#pragma once

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
#include <atomic>
#include <mutex>
#include <map>
#include <thread>
#include <memory>
#include <WinSock2.h>
#include "HttpServer.h"
class CServerListener
{
public:

	CServerListener(int nPort = 9988);

	~CServerListener();

public:

	/**
	* 初始化windows socket
	*
	* @param port Port on which the server will be listening for connections
	* @param buffer_size Size of the buffer used to retrieve data from sockets
	*/
	int Init(int  nPort);


	/**
	* Start listening for connections.
	* @param client_acceptation_error_callback The function receiving the ClientAcceptationException object when a problem with acceptation of new connection occurs
	*/
	//void run(std::function<void(ClientAcceptationException)> client_acceptation_error_callback = [](ClientAcceptationException) {});
	void run();
	/**
	* Stop listening for connections (close listening socket).
	*
	* @warning It could be run only from the different thread, as start() is blocking
	*/
	void stop();

	void  SetHttpRequestBodyCallBack(HttpRequestBodyCallBack callBackFunc);

	//客户端操作线程
	 void clientHandler(SOCKET client_socket);

	 //服务器往客户端发送消息
	 void send2client(SOCKET socketValue, const char* pResponseBuf, int nSendLen);

public:
	// 获取http服务器实例
	static CServerListener* GetInstance();

	//退出程序时候内存释放
	static void Destory()
	{
		if (nullptr != s_pServerListener)
		{
			delete s_pServerListener;
			s_pServerListener = nullptr;
		}
	};
public:

	static CServerListener*		s_pServerListener;

	std::mutex m_mutex;

	HttpRequestBodyCallBack m_requestBodyCallBackFunc;

	SOCKET listen_socket = INVALID_SOCKET;

	std::atomic<BOOL>	   m_bRunning_Server;		 //当前状态
};


#define HTTP_SERVER CServerListener::GetInstance()