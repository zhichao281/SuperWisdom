

#include <process.h>
#include <stdio.h>

#include "ServerListener.h"
#pragma comment(lib, "ws2_32.lib")
#include "NSLog.h"



CServerListener* CServerListener::s_pServerListener = nullptr;



CServerListener::CServerListener(int nPort)
{
	if (0 != Init(nPort))
		exit(1);

	m_bRunning_Server.store(TRUE);
}


CServerListener::~CServerListener()
{
	stop();	
}

CServerListener * CServerListener::GetInstance()
{

	if (nullptr != s_pServerListener)
	{
		return s_pServerListener;
	}

	s_pServerListener = new CServerListener();
	atexit(Destory);
	return s_pServerListener;


}

//	 初始化windows socket
int CServerListener::Init(int  nPort)
{
	WSADATA wsaData;
	int error = 0;

	error = WSAStartup(MAKEWORD(2,2), &wsaData);//希望使用的WinSock DLL的版本  
	if (error == SOCKET_ERROR)
	{
		char errorBuffer[100];

		error = WSAGetLastError();
		if (error == WSAVERNOTSUPPORTED)
		{
			//确认WinSock DLL支持版本2.2  
			sprintf_s(errorBuffer, "Startup() - WSAStartup() error.\nRequested v2.0(highest v2.2), found only v%d.%d.",
				LOBYTE(wsaData.wVersion),
				HIBYTE(wsaData.wVersion));
			WSACleanup();
		}
		else
			sprintf_s(errorBuffer, "Startup() - WSAStartup() error %d", WSAGetLastError());
		printf(errorBuffer);
		NSLOG_ERROR("%s", errorBuffer);
		return -1;
	}

	// 创建服务器套接字
	listen_socket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listen_socket == INVALID_SOCKET)
	{
		printf("wsasocket error %d\n", errno);
		NSLOG_ERROR("wsasocket error %d\n", errno);
		return -1;
	}
	// 
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(nPort);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	int nBind = ::bind(listen_socket, (sockaddr*)&server_addr, sizeof(server_addr));
	if (nBind == INVALID_SOCKET)
	{
		::closesocket(listen_socket);
		printf("bind error %s\n", errno);
		NSLOG_ERROR("bind error %s\n", errno);
		return -1;
	}

	int nListen = ::listen(listen_socket, 200);
	if (SOCKET_ERROR == nListen)
	{
		::closesocket(listen_socket);
		printf("listen error %s\n", errno);
		NSLOG_ERROR("listen error %s\n", errno);
		return -1;
	}

	
	NSLOG_INFO("服务器启动成功!");
	printf("prepare accept...\n");
	return 0;
}

void CServerListener::run()
{
	std::map<SOCKET, std::shared_ptr<std::thread>> map_threads;


	sockaddr_in addrAccept;
	int addrLen = sizeof(addrAccept);
	while (m_bRunning_Server.load())
	{
		//使用google浏览器请求(输入"127.0.0.1:9999")发现accept调用了2次.因为可能:
		//server没有及时响应http请求，有些浏览器在pipeline被阻塞情况下会再次发起新连接
		SOCKET client_socket = ::accept(listen_socket, (sockaddr*)&addrAccept, &addrLen);
		if (client_socket == INVALID_SOCKET)
		{
			::closesocket(client_socket);
			printf("accept error %s\n", errno);
			NSLOG_ERROR("accept error %s\n", errno);
			continue;
		}
		else
		{
			NSLOG_INFO("connect from addr = %s, port = %d\n", inet_ntoa(addrAccept.sin_addr), addrAccept.sin_port);
			printf("connect from addr = %s, port = %d\n", inet_ntoa(addrAccept.sin_addr), addrAccept.sin_port);
			
			
			// 处理请求，主要功能函数  改为多线程并发
			std::thread clientThread([=]()
			{
				clientHandler(client_socket);
			});
			clientThread.detach();

			//std::unique_lock<std::mutex> guard(m_mutex);
			//std::shared_ptr<std::thread> clientThread;
			//clientThread = std::make_shared<std::thread>(std::bind(&CServerListener::clientHandler, this, client_socket));
			//map_threads[client_socket] = clientThread;
		}
	}

}


void CServerListener::stop()
{
	m_bRunning_Server.store(FALSE);
	if (listen_socket != INVALID_SOCKET) 
	{
		shutdown(listen_socket, SD_BOTH);
		closesocket(listen_socket);
		WSACleanup();
	}
}





void CServerListener::send2client(SOCKET socketValue, const char* pResponseBuf, int nSendLen)
{
	int nSendedLen = 0;
	while (nSendLen > 0)
	{
		nSendedLen = send(socketValue, pResponseBuf, nSendLen, 0);
		if (nSendedLen > 0)
		{
			pResponseBuf += nSendedLen;
			nSendLen -= nSendedLen;
		}
		else if (errno == EWOULDBLOCK)
		{
			continue;
		}
		else
		{
			closesocket(socketValue);
			break;
		}
	}
}

void CServerListener::SetHttpRequestBodyCallBack(HttpRequestBodyCallBack callBackFunc)
{
	//m_webServer.SetHttpRequestBodyCallBack(callBackFunc);
	m_requestBodyCallBackFunc = callBackFunc;
}


void CServerListener::clientHandler(SOCKET client_socket)
{
	CHttpServer m_webServer;
	m_webServer.SetHttpRequestBodyCallBack(m_requestBodyCallBackFunc);
	SOCKET socketValue = client_socket;
	//the buff is just for test.
	char buff_Request[1024 * 60] = { 0 };
	int nSumBytes = 0;
	int nNowBytes = 0;
	struct tagHttpResponseData httpResponseData;
	
	while (1)
	{
		// 接受客户端的信息
		nNowBytes = recv(socketValue, buff_Request + nSumBytes, sizeof(buff_Request)-nSumBytes, 0);
		if (nNowBytes > 0)
		{
			int nOldSumBytes = nSumBytes;
			nSumBytes += nNowBytes;
			if (nSumBytes > sizeof(buff_Request)-1)
			{
				printf("buff full.\n");
				NSLOG_ERROR("buff full.\n");
				closesocket(socketValue);
				break;
			}
			buff_Request[nSumBytes] = 0;
			// 将客户端的信息输出到终端
			printf("-----recv data:\n%s\n", &buff_Request[nOldSumBytes]);
			int nStatusCode = m_webServer.HttpParser(buff_Request, nNowBytes);
			if (W_HTTP_PARSER_AGAIN == nStatusCode)
			{
				printf("-----again recv data\n");
				continue;
			}
			else
			{
				// 处理请求，主要功能函数
				nSumBytes = 0;
				bool bIsKeepAlive = false;
				int nResponseResult = m_webServer.HttpResponseHeader(nStatusCode, &httpResponseData);
				if (nResponseResult == W_HTTP_RESPONSE_OK)
				{
					//send header
					bIsKeepAlive = httpResponseData.u.bIsKeepAlive;
					send2client(socketValue, httpResponseData.pBuff, httpResponseData.nBuffLen);

					//send body
					nResponseResult = m_webServer.HttpResponseBody(&httpResponseData);
					if (W_HTTP_RESPONSE_OK == nResponseResult)
					{
						send2client(socketValue, httpResponseData.pBuff, httpResponseData.nBuffLen);
						m_webServer.HttpResponseBodyOver();
					}
				}

				if (!bIsKeepAlive || nStatusCode != W_HTTP_OK)
				{
					closesocket(socketValue);
					break;
				}
			}
		}
		else if (SOCKET_ERROR == nNowBytes)
		{
			printf("recv error:%d,fd:%d,bytes=%d\n", WSAGetLastError(), socketValue, nNowBytes);
			//Sleep(1000);
			//由原来的 等待下载 改为退出
			closesocket(socketValue);
			break;
		}
		else
		{
			printf("close...\n");
			NSLOG_ERROR("close..\n");
			closesocket(socketValue);
			break;
		}
	}
	return ;

}