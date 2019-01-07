#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment ( lib , "ws2_32" )

#define MAXLINE 10000


// 声明函数

// 获取文件类型
void GetFileType(char * filename, char * fileType);

// 处理静态请求
void HandleStatic(SOCKET clientSocket, char * filename);

// 处理动态请求的函数
void HandleDynamicFunc(char * filename, char * args, int & result);

// 处理动态请求
void HandleDynamic(SOCKET clientSocket, char *filename, char * args);

//解析URI
bool ParseURI(SOCKET clientSocket, char * uri, char * filename, char *args);

// 解析请求(Parse Request )，如果为静态返回true,如果为动态，返回false
void ParseRequest(SOCKET clientSocket, char * request);

// 处理请求行：读取请客户端的请求，给出响应
void HandleRequestLine(SOCKET clientSocket, char * request);




