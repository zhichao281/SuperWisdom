// AES_Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "AesHelper.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CAesHelper tool;
	std::string strRet = tool.Encrypt("aaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaa");
	printf("Result is \n%s\n", strRet.c_str());
	std::string strSource = tool.Decrypt("UYjGR0sijL3SQukSXr4dU/+G1pnGtb9Kjmfne6LFimk=", "aaaaaaaaaaaaaaaa");
	printf("Result is \n%s\n", strSource.c_str());
	return 0;
}

