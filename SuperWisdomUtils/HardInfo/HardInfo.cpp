// HardInfo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <set>
#include "CHardInfo.h"
#include "CCodeVerify.h"
//LINUX
#if 0 
#include <stdio.h>
#include <string.h>
#include <libudev.h>
#include <sys/stat.h>


int hardmain()
{
	struct udev            *ud = NULL;
	struct stat             statbuf;
	struct udev_device     *device = NULL;
	struct udev_list_entry *entry = NULL;

	ud = udev_new();
	if (NULL == ud) {
		fprintf(stderr, "Failed to create udev.\n");
	}
	else {
		if (0 != stat("/dev/sda", &statbuf)) {
			fprintf(stderr, "Failed to stat /dev/sda.\n");
		}
		else {
			device = udev_device_new_from_devnum(ud, 'b', statbuf.st_rdev);
			if (NULL == device) {
				fprintf(stderr, "Failed to open /dev/sda.\n");
			}
			else {
				entry = udev_device_get_properties_list_entry(device);
				while (NULL != entry) {
					if (0 == strcmp(udev_list_entry_get_name(entry),
						"ID_SERIAL")) {
						break;
					}

					entry = udev_list_entry_get_next(entry);
				}

				printf("Serial ID: %s\n", udev_list_entry_get_value(entry));

				udev_device_unref(device);
			}
		}

		(void)udev_unref(ud);
	}

	return 0;
}
#endif


#include "../Encode/AesEncryptor.h"
#include "../Encode/Crypto.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include "../Encode/AES.h"
#include "../Encode/base64New.h"

//软件的认证部分， 如果不通过， 则不可以用
bool  checklicense()
{

	unsigned char str1[] = "这个是测试";
	wstring wstrOut;
	CryptoBase64::Encode(str1,13, wstrOut);
	string  strInput = "这个是测试";
	string  strout=Base64New::encode64(strInput);

	unsigned char szKey[] = "1234567890123456";
	AesEncryptor Aes(szKey);
	std::string  strPass = Aes.EncryptString("admin");
	std::string  strPass1 = Aes.DecryptString("7ae2d4a63d1c3e1a187a1fd89f471ef5");


	// 获取所在目录下的license.txt文件的第一行
	char szLicenseFile[MAX_PATH] = { 0 };
	char szModuleFileName[MAX_PATH] = "";

	GetModuleFileNameA(NULL, szModuleFileName, MAX_PATH);
	char* pRchr = strrchr(szModuleFileName, '\\');
	if (NULL == pRchr)
	{
		pRchr = strrchr(szModuleFileName, '/');
	}

	if (NULL != pRchr)
	{
		pRchr++;
		*pRchr = '\0';
	}
	strcat_s(szModuleFileName, "license1.txt"); 
	strcpy_s(szLicenseFile, szModuleFileName);

	// 校验license.txt文件是否存在
	ifstream filein(szLicenseFile);
	//filein.open(szLicenseFile);
	if (filein)
	{
		std::string strFile((std::istreambuf_iterator<char>(filein)),
			std::istreambuf_iterator<char>());
		filein.close();
		if (strFile.length() >1)
		{
		
		}
		cout << "license.txt文件错误" << endl;
		system("pause");
		return -1;
	}

	else
	{
		// 获取license.txt文件指纹
		
		cout << "无license.txt文件， 认证失败" << endl;
		system("pause");
		return -1;
	}
	






	return true;
}


#include "../FileManager/FileManange.h"

/*
*	Function:		GetFileSize
*	Explanation:	获取文件大小
*	Input:			strFilePath		文件路径
*  Output:			pHighSize		高阶大小
*	Return:			低阶大小
*/
double GetFileSize(std::string strFilePath)
{

	double filesize = 0;
	struct _stat64 buf;
	if (_stat64(strFilePath.c_str(), &buf) < 0)
	{
		filesize = 0;
	}
	else
	{
		filesize = (double)buf.st_size;
	}
	return filesize;
}
string wstr2str(const wstring& wstr)
{
	if (wstr.empty() == true)
	{
		return "";
	}
	const wchar_t* unicodeString = wstr.c_str();
	int nsize = wcslen(unicodeString);
	int utf8Len = WideCharToMultiByte(936, 0, unicodeString, nsize, NULL, 0, NULL, NULL);

	char* utf8String = new char[utf8Len + 1];
	memset(utf8String, 0x00, utf8Len + 1);
	WideCharToMultiByte(936, 0, unicodeString, nsize, utf8String, utf8Len, NULL, NULL);

	string dest = utf8String;
	delete[] utf8String;
	return dest;
}
int hardmain()
{
	TCHAR m_szSIFile[MAX_PATH] = L"E:\\新建文件夹\\232.dmp";

	if (GetFileSize(wstr2str(m_szSIFile)) < 1)
	{
		checklicense();
	}
	

	std::string  str = "12345";
	std::wstring wstr=CryptoMD5::MD5String32(L"admin1111111121212121");

	std::wstring wstr2 = CryptoMD5::MD5String16(L"admin");
	unsigned char encrypt[] = "6FC8FACBDF38336C3FBE054E75DBB1FD";
	std::wstring wstr3;
	CryptoBase64::Encode(encrypt, (int)strlen((char *)encrypt), wstr3);

	std::string strSerivalNumber;
	
	int x = 1;
	while (x > 0)
	{
		if (CCodeVerify::GetSerialNumber("E:\\study\\SuperWisdom\\SuperWisdomUtils\\HardInfo\\pubKey.pem", strSerivalNumber))
		{
			std::cout << " SerialNumberLen	:  " << strSerivalNumber.length() << std::endl;
			std::cout << "SerivalNumber		:  " << strSerivalNumber << std::endl;
		}

		std::string strActiveCode;
		std::string strSignDate = "20190430";
		std::string strSignOther = "ABCDEFGH";
		if (CCodeVerify::CreateActiveNumber("E:\\study\\SuperWisdom\\SuperWisdomUtils\\HardInfo\\prikey.pem", strSerivalNumber, strSignDate, strSignOther, strActiveCode))
		{
			std::cout << "ActiveCodeLen:  " << strActiveCode.length() << std::endl;
			std::cout << "ActiveCode : " << strActiveCode << std::endl;
		}
		std::string strVerifyDate;
		std::string strVerifyOther; 
		if (CCodeVerify::VerifyActiveNumber("E:\\study\\SuperWisdom\\SuperWisdomUtils\\HardInfo\\pubKey.pem", strActiveCode, strVerifyDate, strVerifyOther))
		{
			std::cout << "UntilDate " << strVerifyDate << "   OtherInfo:" << strVerifyOther << std::endl;
		}
		else
		{
			std::cout << "Verify Failed" << std::endl;
		}
		strActiveCode.clear();
		strSerivalNumber.clear();
		std::cin >> x;
	}
	

	return 0;

}



// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
