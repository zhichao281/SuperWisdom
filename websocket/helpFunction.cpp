#include "helpFunction.h"


// 获取文件类型
void GetFileType(char * filename, char * fileType)
{
	memset(fileType, 0, 20);
	if (strstr(filename, ".html") || 0 == strcmp(filename, "/"))
	{
		strcpy(fileType, "text/html");
	}
	else if (strstr(filename, ".gif"))
	{
		strcpy(fileType, "image/gif");
	}
	else if (strstr(filename, ".jpg"))
	{
		strcpy(fileType, "imgae/jpeg");
	}
	else
	{
		strcpy(fileType, "text/plain");
	}
}

// 处理静态请求
void HandleStatic(SOCKET clientSocket, char * filename)
{
	// 直接返回服务器主页
	if (0 == strcmp(filename, "/"))
	{
		strcpy(filename, "/index.html");

	}

	FILE *fp = fopen(filename, "r");
	if (fp == 0)
	{
		// 如果找不到响应的数据就返回404状态码，并将其发送给客户端
		char response[] = "HTTP/1.1 404 NOT FOUND\r\n\r\n";
		send(clientSocket, response, strlen(response), 0);
	}
	else
	{
		// 在文件目录中找响应的资源
		int file_size;
		char *content;
		char response[1024];
		fseek(fp, 0, SEEK_END);
		file_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		content = (char*)malloc(file_size + 1);
		fread(content, file_size, 1, fp);
		content[file_size] = 0;

		// 检测请求文件的类型
		char fileType[20];
		GetFileType(filename, fileType);

		// 发送请求headers到客户端
		sprintf(response, "HTTP/1.1 200 OK\r\n");
		sprintf(response, "%sContent-Type: %s\r\n", response, fileType);
		sprintf(response, "%sContent-Length: %d\r\n", response, file_size);
		sprintf(response, "%sServer: Bobo Server\r\n\r\n", response);
		send(clientSocket, response, strlen(response), 0);

		// 发送请求body到客户端
		sprintf(response, "%s", content);
		send(clientSocket, response, strlen(response), NULL);

		free(content);
	}
}

// 处理动态请求的函数
void HandleDynamicFunc(char * filename, char * args, int & result)
{
	// 处理动态请求参数
	char * a = strtok(args, "&");
	char * b = strtok(NULL, "&");
	int x = atoi(a);
	int y = atoi(b);

	if (strstr(filename, "Add"))
	{
		result = x + y;
	}
	else if (strstr(filename, "Sub"))
	{
		result = x - y;
	}
}

// 处理动态请求
void HandleDynamic(SOCKET clientSocket, char *filename, char * args)
{
	int result = 0;
	char response[8000];  // 响应内容
	// 执行动态请求
	HandleDynamicFunc(filename, args, result);

	// 发送响应头到客户端
	sprintf(response, "HTTP/1.1 200 OK\r\n");
	sprintf(response, "%sServer: Bobo Server\r\n\r\n", response);
	send(clientSocket, response, strlen(response), NULL);

	//发送响应Body到客户端
	sprintf(response, "<head><head><title>Welcome to Bobo Server!</title></head>");
	sprintf(response, "%s<body>The result is %d</body>", response, result);
	send(clientSocket, response, strlen(response), NULL);
}


//解析URI
bool ParseURI(SOCKET clientSocket, char * uri, char * filename, char *args)
{
	sprintf(filename, ".%s", uri);
	if (!strstr(uri, "dynamic"))  // 静态请求
	{
		strcpy(args, "");
		strcpy(filename, ".");
		strcat(filename, uri);
		if (uri[strlen(uri) - 1] == '/')
		{
			strcat(filename, "index.html");
		}
		return true;
	}
	else  // 动态请求
	{
		char * ptr = strstr(uri, "?");
		if (ptr)
		{
			strcpy(args, ptr + 1);
		}
		else
		{
			strcpy(args, "");
		}
		ptr = strstr(uri, "?");
		*ptr = '\0';

		strcpy(filename, uri);
		strcat(uri, "?");
		strcat(uri, args);
		return false;
	}
}

// 解析请求(Parse Request )，如果为静态返回true,如果为动态，返回false
void ParseRequest(SOCKET clientSocket, char * request)
{
	// 判断URI请求的是否为静态请求
	bool is_static;

	//  获取请求的命令类型
	char *method = strtok(request, " ");
	//获取请求的uri
	char *uri = strtok(0, " ");

	// 解析URI
	char args[20] = { 0 };    // URI中的动态请求参数
	char filename[100] = { 0 };  // URI中静态请求中的文件名

	is_static = ParseURI(clientSocket, uri, filename, args);

	if (is_static)  // 请求为静态
	{
		HandleStatic(clientSocket, filename);
	}
	else  // 请求为动态
	{
		HandleDynamic(clientSocket, filename, args);
	}
}


// 处理请求行：读取请客户端的请求，给出响应
void HandleRequestLine(SOCKET clientSocket, char * request)
{
	// 解析URI
	ParseRequest(clientSocket, request);
}