
#pragma once 
#include <vector>
#include <regex>
#include <string>
class CCodeVerify
{
public:
	CCodeVerify();
	~CCodeVerify();
public:

	//获取机器码
	std::string  GetSerialNumber();

	//创建激活码
	std::string  CreateActiveNumber(std::string strSerialNumber, std::string otherInfo);
	
	//解析激活码
	bool  VerifyActiveNumber(std::string strAcitveNumber, std::string& strDate,std::string& otherInfo);

	//当前系统时间
	std::string DatetimeToString();

	std::vector<std::string> Split11(const std::string& strInput, const std::string& delim);
private:

	std::string m_strKey;


};
