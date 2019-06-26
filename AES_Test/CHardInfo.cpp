
#include "CHardInfo.h"
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>

#include <array>
#pragma comment(lib, "IPHLPAPI.lib")
//注册码一共8行，每行16个字符
//所以单行长度为16
const int SINGLE_LINE_LEN = 16;
const int MAC_ADDR_LEN = 12;

//CPUID
//MAC_ADDR1+4
//MAC_ADDR2+4
//MAC_ADDR3+4
//DISK_ID1
//DISK_ID2
//MainBoardID
//OtherInfo


std::string& trim(std::string &s)
{
	if (s.empty())
	{
		return s;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

#ifdef WIN32
std::string CHardInfo::Get_CPU_ID()
{
	char buff[32] = { 0 };
	unsigned long s1, s2;

	__asm {
		mov eax, 01h
		xor edx, edx
		cpuid
		mov s1, edx
		mov s2, eax
	}
	sprintf_s(buff, "%08X%08X", s1, s2);
	return buff;
}


#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
std::vector<std::string> CHardInfo::Get_ALL_MAC_ADDR_ID()
{
	std::vector<std::string> strVec;
	/* Declare and initialize variables */

// It is possible for an adapter to have multiple
// IPv4 addresses, gateways, and secondary WINS servers
// assigned to the adapter. 
//
// Note that this sample code only prints out the 
// first entry for the IP address/mask, and gateway, and
// the primary and secondary WINS server for each adapter. 

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;


	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		return strVec;
	}
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			return strVec;
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		char buff[8] = { 0 };
		std::string strMacAddr;
		while (pAdapter) {
			strMacAddr.clear();
			for (i = 0; i < pAdapter->AddressLength; i++) {
				memset(buff, 0, 8);
				sprintf_s(buff, 8, "%.2X", (int)pAdapter->Address[i]);
				strMacAddr += buff;

			}
			if (!strMacAddr.empty())
			{
				strVec.push_back(strMacAddr);
			}
			pAdapter = pAdapter->Next;
		}
	}
	else {
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);
	return strVec;
}


std::vector<std::string> CHardInfo::Get_ALL_DISK_ID()
{
	std::vector<std::string> strVec;
	std::array<char, 128> buffer;
	std::string result;
	std::string strCmd = "wmic path win32_physicalmedia get SerialNumber";
	std::shared_ptr<FILE> pipe(_popen(strCmd.c_str(), "r"), _pclose);
	if (!pipe) throw std::runtime_error("_popen() failed!");
	while (!feof(pipe.get())) {
		if (fgets(buffer.data(), 128, pipe.get()) != NULL)
			result += buffer.data();
	}

	std::size_t startIndex = 0;
	std::size_t findIndex = 0;

	findIndex = result.find("\r\n", startIndex);
	std::string strHDNumber;
	while (findIndex != std::string::npos)
	{
		strHDNumber.clear();
		strHDNumber = result.substr(startIndex, findIndex - startIndex);
		if (strHDNumber.find("SerialNumber") == std::string::npos && !strHDNumber.empty())
		{
			strVec.push_back(trim(strHDNumber));
		}
		startIndex = findIndex + 2;
		findIndex = result.find("\r\n", startIndex);
	}
	return strVec;
}

std::string CHardInfo::Get_MotherBoard_ID()
{
	char lpszBaseBoard[256] = { 0 };

	//ԭhttps ://blog.csdn.net/aoshilang2249/article/details/44958985 

	const long MAX_COMMAND_SIZE = 10000; //
	WCHAR szFetCmd[] = L"wmic BaseBoard get SerialNumber"; //	
	const std::string strEnSearch = "SerialNumber"; // 

	BOOL   bret = FALSE;
	HANDLE hReadPipe = NULL; //
	HANDLE hWritePipe = NULL; //
	PROCESS_INFORMATION pi;   //
	STARTUPINFO			si;	  //
	SECURITY_ATTRIBUTES sa;   //

	char			szBuffer[MAX_COMMAND_SIZE + 1] = { 0 }; // 
	std::string			strBuffer;
	unsigned long	count = 0;
	long			ipos = 0;

	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	memset(&sa, 0, sizeof(sa));

	pi.hProcess = NULL;
	pi.hThread = NULL;
	si.cb = sizeof(STARTUPINFO);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if (!bret)
	{
		goto END;
	}

	GetStartupInfo(&si);
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.wShowWindow = SW_HIDE; //
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	bret = CreateProcess(NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!bret)
	{
		goto END;
	}

	WaitForSingleObject(pi.hProcess, 500/*INFINITE*/);
	bret = ReadFile(hReadPipe, szBuffer, MAX_COMMAND_SIZE, &count, 0);
	if (!bret)
	{
		goto END;
	}

	bret = FALSE;
	strBuffer = szBuffer;
	ipos = strBuffer.find(strEnSearch);

	if (ipos < 0)
	{
		goto END;
	}
	else
	{
		strBuffer = strBuffer.substr(ipos + strEnSearch.length());
	}

	memset(szBuffer, 0x00, sizeof(szBuffer));
	strcpy_s(szBuffer, strBuffer.c_str());

	std::size_t j = 0;
	for (std::size_t i = 0; i < strlen(szBuffer); i++)
	{
		if (szBuffer[i] != ' ' && szBuffer[i] != '\n' && szBuffer[i] != '\r')
		{
			lpszBaseBoard[j] = szBuffer[i];
			j++;
		}
	}

	bret = TRUE;

END:
	CloseHandle(hWritePipe);
	CloseHandle(hReadPipe);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return std::string(lpszBaseBoard);
}

#else

#endif


std::string  CHardInfo::GetFormatCPU_ID()
{
	std::string strCpuID = Get_CPU_ID();
	if (strCpuID.length() >= 16)
	{
		return strCpuID.substr(strCpuID.length() - 16, 16);
	}
	else
	{
		return "FFFFFFFFFFFFFFFF";
	}
}

std::vector<std::string>  CHardInfo::GetFormatALL_MAC_ADDR_ID()
{
	std::vector<std::string> strFormatMacAddrVec;
	std::vector<std::string> strAllMacAddr = Get_ALL_MAC_ADDR_ID();
	const std::size_t MAX_MAC_ADDR_COUNT = 1;
	for (const auto item : strAllMacAddr)
	{
		std::string strMac = item;
		strFormatMacAddrVec.push_back(strMac);
		if (strFormatMacAddrVec.size() >= MAX_MAC_ADDR_COUNT)
		{
			break;
		}
	}
	return strFormatMacAddrVec;


	std::string strRandom = "0123456789ABCDEFGHIGKLMNOPQRSTUVWXYZ";
	srand((unsigned)time(0));// rand()产生的是伪随机数字，每次执行时是相同的;若要不同,用函数srand()初始化它。
	for (const auto item : strAllMacAddr)
	{
		std::string strMac = item;
		for (int i = 0; i < 4; ++i)
		{
			int  i1=rand() % strRandom.length();
			strMac += strRandom[rand() % strRandom.length()];
		}
		strFormatMacAddrVec.push_back(strMac);
		if (strFormatMacAddrVec.size() >= MAX_MAC_ADDR_COUNT)
		{
			break;
		}
	}

	while (strFormatMacAddrVec.size() < MAX_MAC_ADDR_COUNT)
	{
		std::string strMac;
		for (int i = 0; i < SINGLE_LINE_LEN; ++i)
		{
			strMac += strRandom[rand() % strRandom.length()];
		}
		strFormatMacAddrVec.push_back(strMac);
	}
	return strFormatMacAddrVec;
}

std::vector<std::string>  CHardInfo::GetFormatALL_DISK_ID()
{
	std::vector<std::string> strFormatVec;
	auto HDNumbers = Get_ALL_DISK_ID();
	std::string strRandom = "0123456789ABCDEFGHIGKLMNOPQRSTUVWXYZ";
	srand((unsigned)time(0));// rand()产生的是伪随机数字，每次执行时是相同的;若要不同,用函数srand()初始化它。
	for (const auto item : HDNumbers)
	{
		std::string strMac = item;

		while (strMac.length() < SINGLE_LINE_LEN)
		{
			strMac += strRandom[rand() % strRandom.length()];
		}

		if (strMac.length() > SINGLE_LINE_LEN)
		{
			strMac = strMac.substr(0, SINGLE_LINE_LEN);
		}
		strFormatVec.push_back(strMac);
	}
	return strFormatVec;
}

std::string  CHardInfo::GetFormatMotherBoard_ID()
{
	std::string strResult = Get_MotherBoard_ID();
	if (!strResult.empty())
	{
		if (strResult.length() > SINGLE_LINE_LEN)
		{
			return strResult.substr(0, SINGLE_LINE_LEN);
		}
		else
		{
			std::string strRandom = "0123456789ABCDEFGHIGKLMNOPQRSTUVWXYZ";
			srand((unsigned)time(0));// rand()产生的是伪随机数字，每次执行时是相同的;若要不同,用函数srand()初始化它。
			while (strResult.length() < SINGLE_LINE_LEN)
			{
				strResult += strRandom[rand() % strRandom.length()];
			}
		}
		return strResult;
	}
	else
	{
		return "FFFFFFFFFFFFFFFF";
	}
}


std::string CHardInfo::GetMachineCode()
{
	std::string strResult = GetFormatCPU_ID();

	//MAC ARRAY
	{
		auto allMacs = GetFormatALL_MAC_ADDR_ID();
		for (auto item : allMacs)
		{
			strResult += ";";
			strResult += item;
		}
	}
	//{
	//	auto allMacs = GetFormatALL_DISK_ID();
	//	for (auto item : allMacs)
	//	{
	//		strResult += item;
	//	}
	//}
	//{
	//	strResult += GetFormatMotherBoard_ID();
	//}
	//{
	//	strResult += ";zhichao281@163.com";
	//}
	return strResult;
}

bool CHardInfo::VerifyMachineCode(const std::string strCode)
{
	std::vector<std::string> allArray = MachineCodeToArray(strCode);

	if (allArray.size() != 8)
	{
		return false;
	}

	if (!VerifyCPUID(allArray[0]))
	{
		return false;
	}

	{
		std::vector<std::string> macAddrArray;
		macAddrArray.push_back(allArray[1]);
		macAddrArray.push_back(allArray[2]);
		macAddrArray.push_back(allArray[3]);
		if (!VerifyAllMAC(macAddrArray))
		{
			return false;
		}
	}
	{
		//std::vector<std::string> diskIdArray;
		//diskIdArray.push_back(allArray[4]);
		//diskIdArray.push_back(allArray[5]);
		//if (!VerifyDiskID(diskIdArray))
		//{
		//	return false;
		//}
	}
	{
		//if (!VerifyMotherBoardID(allArray[6]))
		//{
		//	return false;
		//}
	}
	return true;
}

std::string CHardInfo::GetPrintabeMaichineCode()
{
	std::vector<std::string> strArray = MachineCodeToArray(GetMachineCode());
	std::string strResult = "";
	for (const auto item : strArray)
	{
		strResult += item;
		strResult += "\r\n";
	}
	return strResult;
}


std::vector<std::string> CHardInfo::MachineCodeToArray(const std::string strMachineCode)
{
	std::vector<std::string> strResultVec;
	for (int i = 0; i < 8; i++)
	{
		strResultVec.push_back(strMachineCode.substr(i * SINGLE_LINE_LEN, SINGLE_LINE_LEN));
	}
	return strResultVec;
}



std::string  CHardInfo::ArrayToMachinCode(const std::vector<std::string> strArray)
{
	std::string strResult;
	for (const auto item : strArray)
	{
		strResult += item;
	}
	return strResult;
}


bool CHardInfo::VerifyCPUID(const std::string strCpuID)
{
	std::string strLocalId = Get_CPU_ID();
	int sameCount = 0;
	for (std::size_t index = 0; index < SINGLE_LINE_LEN; ++index)
	{
		if (strLocalId[index] == strCpuID[index])
		{
			sameCount++;
		}
	}
	return sameCount > 12;
}

bool CHardInfo::VerifyAllMAC(const std::vector<std::string> strMacAddrArray)
{
	std::vector<std::string> cpuId = Get_ALL_MAC_ADDR_ID();
	std::vector<std::string> strInputArray = strMacAddrArray;
	std::sort(cpuId.begin(), cpuId.end());
	std::sort(strInputArray.begin(), strInputArray.end());
	if (strMacAddrArray.size() != 3)
	{
		return false;
	}
	if (strInputArray.size() != 3)
	{
		return false;
	}

	int sameCount = 0;
	for (int index = 0; index < 3; ++index)
	{
		std::string macAddr = strInputArray[index].substr(0, MAC_ADDR_LEN);
		if (cpuId.end() != std::find(cpuId.begin(), cpuId.end(), macAddr))
		{
			sameCount++;
		}
	}
	return sameCount > 1;
}

bool CHardInfo::VerifyDiskID(const std::vector<std::string> strDiskIDArray)
{
	std::vector<std::string> diskIdArray = Get_ALL_DISK_ID();
	std::vector<std::string> strInputArray = strDiskIDArray;
	std::sort(diskIdArray.begin(), diskIdArray.end());
	std::sort(strInputArray.begin(), strInputArray.end());
	if (strInputArray.size() != 2)
	{
		return false;
	}

	int sameCount = 0;
	for (int index = 0; index < 1; ++index)
	{
		if (GetStringCommonPrefix(strInputArray[index], diskIdArray[index]) == diskIdArray[index].length())
		{
			sameCount++;
		}
	}
	return sameCount > 1;
}

bool CHardInfo::VerifyMotherBoardID(const std::string strBoardId)
{
	std::string strMotherBoardId = Get_MotherBoard_ID();
	std::size_t length = std::min<std::size_t>(strMotherBoardId.length(), 16);
	return GetStringCommonPrefix(strBoardId, strMotherBoardId) == length;
}


int CHardInfo::GetStringCommonPrefix(const std::string strFirst, const std::string strSecond)
{
	int sameCount = 0;
	std::size_t strMinLen = std::min<std::size_t>(strFirst.length(), strSecond.length());
	for (std::size_t index = 0; index < strMinLen; ++index)
	{
		if (strFirst[index] == strSecond[index])
		{
			sameCount++;
		}
	}
	return sameCount;
}