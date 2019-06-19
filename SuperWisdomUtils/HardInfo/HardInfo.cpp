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


int main()
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



#include "../Crypto.h"

int main()
{
 
	std::string  str = "12345";
	std::wstring wstr=CryptoMD5::MD5String32(L"admin");

	std::wstring wstr2 = CryptoMD5::MD5String16(L"admin");
	unsigned char encrypt[] = "7A57A5A743894A0E";
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
