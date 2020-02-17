// AES_Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "AesHelper.h"
#include <windows.h>

#include <string>
#include "CCodeVerify.h"
#include <codecvt>
#include <iostream>
using namespace std;

//将string转换成wstring  
static inline wstring string2wstring(const std::string& s)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const char* chSrc = s.c_str();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest, chSrc, nDestSize);
	std::wstring wstrResult = wchDest;
	delete[]wchDest;
	setlocale(LC_ALL, strLocale.c_str());
	return wstrResult;
}
static inline std::string wstring2string(const std::wstring& ws)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const wchar_t* wchSrc = ws.c_str();
	size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
	char* chDest = new char[nDestSize];
	memset(chDest, 0, nDestSize);
	wcstombs(chDest, wchSrc, nDestSize);
	std::string strResult = chDest;
	delete[]chDest;
	setlocale(LC_ALL, strLocale.c_str());
	return strResult;
}

//判断是否为utf8编码
static inline bool is_str_utf8(const char* str)
{
	unsigned int nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节
	unsigned char chr = *str;
	bool bAllAscii = true;
	for (unsigned int i = 0; str[i] != '\0'; ++i) {
		chr = *(str + i);
		//判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
		if (nBytes == 0 && (chr & 0x80) != 0) {
			bAllAscii = false;
		}
		if (nBytes == 0) {
			//如果不是ASCII码,应该是多字节符,计算字节数
			if (chr >= 0x80) {
				if (chr >= 0xFC && chr <= 0xFD) {
					nBytes = 6;
				}
				else if (chr >= 0xF8) {
					nBytes = 5;
				}
				else if (chr >= 0xF0) {
					nBytes = 4;
				}
				else if (chr >= 0xE0) {
					nBytes = 3;
				}
				else if (chr >= 0xC0) {
					nBytes = 2;
				}
				else {
					return false;
				}
				nBytes--;
			}
		}
		else {
			//多字节符的非首字节,应为 10xxxxxx
			if ((chr & 0xC0) != 0x80) {
				return false;
			}
			//减到为零为止
			nBytes--;
		}
	}
	//违返UTF8编码规则
	if (nBytes != 0) {
		return false;
	}
	if (bAllAscii) { //如果全部都是ASCII, 也是UTF8
		return true;
	}
	return true;
}

static inline std::string UnicodeToUTF8(const std::wstring& wstr)
{
	std::string ret;
	try {
		std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.to_bytes(wstr);
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}

static inline std::wstring UTF8ToUnicode(const std::string& str)
{
	std::wstring ret;
	try {
		std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.from_bytes(str);
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}

static inline string toUTF8(const string& s)
{
	if (s.length() == 0) {
		return s;
	}
	return UnicodeToUTF8(string2wstring(s));
}
int _tmain(int argc, _TCHAR* argv[])
{




	//CCodeVerify  codeverify;
	//std::string strMachineCode =  codeverify.GetSerialNumber();

	//std::string strSignOther = "zhichao281@163.com";
	//std::string strAcitveNumber = codeverify.CreateActiveNumber(strMachineCode, strSignOther);
	//std::string strSignDate;
	//codeverify.VerifyActiveNumber(strAcitveNumber, strSignDate, strSignOther);



	CAesHelper tool;
	std::string strMachineCode = "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";

	std::string  strPass = "081e57bc22283299";
	std::string strOrgNumber = tool.Decrypt(strMachineCode.c_str(), strPass.c_str());

	std::cout << strOrgNumber;

	bool bres = is_str_utf8(strOrgNumber.c_str());

	std::wstring str = UTF8ToUnicode(strOrgNumber);
	//printf("Result is \n%s\n", strOrgNumber.c_str());
	//std::string strSource = tool.Decrypt(strOrgNumber.c_str(), strPass.c_str());
	//printf("Result is \n%s\n", strSource.c_str());
	//

	system("pause");
	return 0;
}

