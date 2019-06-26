#include "CCodeVerify.h"

#include <time.h>
#include <windows.h>


#include "CHardInfo.h"
#include "AesHelper.h"

CCodeVerify::CCodeVerify()
{
	m_strKey = "123456789zhichao";

}

CCodeVerify::~CCodeVerify()
{
}



std::string  CCodeVerify::GetSerialNumber()
{
	std::string strSerialNumber = "";
	std::string strMachineCode = CHardInfo::GetMachineCode();
	CAesHelper tool;
	strSerialNumber = tool.Encrypt(strMachineCode.c_str(), m_strKey.c_str());
	return strSerialNumber;
}

std::string  CCodeVerify::CreateActiveNumber(std::string strSerialNumber, std::string otherInfo)
{
	std::string strAcitveNumber="";
	//解析获取机器码
	CAesHelper tool;
	std::string strMachineCode = tool.Decrypt(strSerialNumber.c_str(), m_strKey.c_str());

	std::string strSignDate = DatetimeToString();

	strMachineCode = strSignDate + ";"+ strMachineCode + ";" +otherInfo;
	strAcitveNumber= tool.Encrypt(strMachineCode.c_str(), m_strKey.c_str());
	return strAcitveNumber;
}


bool CCodeVerify::VerifyActiveNumber(std::string strAcitveNumber, std::string& strDate, std::string& otherInfo)
{
	//解析获取机器码
	CAesHelper tool;
	std::string strOrgActiveNumber = tool.Decrypt(strAcitveNumber.c_str(), m_strKey.c_str());

	std::vector<std::string> vecReturn= Split11(strOrgActiveNumber,";");
	strDate = vecReturn[0];
	otherInfo = vecReturn[vecReturn.size()-1];
	std::string strOrgMachineCode = vecReturn[1] +";" +vecReturn[2];
	std::string strMachineCode = CHardInfo::GetMachineCode();
	if (strOrgMachineCode == strMachineCode)
	{
		return true;
	}
	else
	{
		return false;
	}


	//std::string strMachineCode = DecodeRSA_PubKeyFile(strPemFile, strOrgActiveNumber);
	//if (CHardInfo::VerifyMachineCode(strMachineCode))
	//{
	//	auto valArray = CHardInfo::MachineCodeToArray(strMachineCode);
	//	strDate = valArray[7].substr(0, 8);
	//	otherInfo = valArray[7].substr(8, 8);
	//	return true;
	//}
	//else
	//{
		return false;
	//}
}


std::string CCodeVerify::DatetimeToString()
{
	char szBuff[128];
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	sprintf(szBuff, "%04d-%02d-%02d %02d:%02d:%02d.%04d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
	return szBuff;                   // 返回转换日期时间后的string变量。
}

std::vector<std::string> CCodeVerify::Split11(const std::string& strInput, const std::string& delim)
{
	std::vector<std::string> vecReturn;
	try
	{
		std::regex re{ delim };
		vecReturn = std::vector<std::string>
		{
			std::sregex_token_iterator(strInput.begin(), strInput.end(), re, -1),
				std::sregex_token_iterator()
		};
		return vecReturn;
	}
	catch (const std::exception& e)
	{
	
	}
	return vecReturn;
}

