
#pragma once 

#include <string>
class CCodeVerify
{
public:


	static bool GetSerialNumber(const std::string strPemFile,std::string& strSerialNumber);


	static bool CreateActiveNumber(const std::string strPemFile, const std::string strSerialNumber, const std::string strDate, const std::string otherInfo,std::string& strAcitveNumber);
	static bool VerifyActiveNumber(const std::string strPemFile, const std::string strAcitveNumber, std::string& strDate,std::string& otherInfo);

	static std::string EncodeRSA_PubKeyFile(const std::string& strPemFileName, const std::string& strData);
	// 公钥加密 
	static std::string EncodeRSA_PubKey(const std::string &strData, std::string &pubKey);

	
	
	static std::string DecodeRSA_PubKeyFile(const std::string& strPemFileName, const std::string& strData);
	// 公钥解密
	static std::string DecodeRSA_PubKey(const std::string &strData, std::string &pubKey);


	// 私钥加密
	static std::string EncodeRSA_PriKeyFile(const std::string& strPemFileName, const std::string& strData);
	static std::string EncodeRSA_PriKey(const std::string &strData, std::string &pubKey);
	


	static std::string DecodeRSA_PriKeyFile(const std::string& strPemFileName, const std::string& strData);
//	static std::string DecodeRSA__PriKey(const std::string &strData, std::string &pubKey);


	static int MakeKey();


	// 函数方法生成密钥对    公钥
	static std::string  GenerateRSAPubKey(std::string strPemFileName="");

	// 函数方法生成密钥对    私钥
	static std::string  GenerateRSAPriKey(std::string strPemFileName="");

	

	static std::string StrToHex(const std::string strIn)
	{
		std::string strOut;
		std::size_t nLength = strIn.length();
		std::string strHex = "0123456789ABCDEF";
		unsigned char buff[256] = { 0 };
		memcpy(buff, strIn.c_str(), strIn.length());
		for (std::size_t index = 0; index < nLength; index++)
		{
			int value = buff[index];
			strOut += strHex[value / 16];
			strOut += strHex[value % 16];
		}
		return strOut;
	}

	static int CharToValue(const char ch)
	{
		if ('0' <= ch && ch <= '9')
		{
			return ch - '0';
		}
		if ('A' <= ch && ch <= 'F')
		{
			return 10 + ch - 'A';
		}
		return -1;
	}

	static std::string HexToStr(const std::string strHexIn)
	{
		std::string strOut;
		std::size_t nLength = strHexIn.length();
		unsigned char buff[256] = { 0 };
		for (std::size_t index = 0; index < nLength; index += 2)
		{
			buff[index / 2] = unsigned char(CharToValue(strHexIn[index]) * 16 + CharToValue(strHexIn[index + 1]));
		}
		return std::string((char *)(buff),strHexIn.length()/2);
	}

};
