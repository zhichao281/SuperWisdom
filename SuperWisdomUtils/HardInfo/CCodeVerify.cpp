#include "CCodeVerify.h"
#include "CHardInfo.h"
#include "openssl/rsa.h"
#include "openssl/err.h"
#include "openssl/pem.h"
#include <iostream>
extern "C"
{
	#include "openssl/applink.c"
}
#include <cassert>
bool CCodeVerify::GetSerialNumber(const std::string strPemFile, std::string& strSerialNumber)
{
	std::string strMachineCode = CHardInfo::GetMachineCode();
	std::string strOrgNumber = EncodeRSA_PubKeyFile(strPemFile, strMachineCode);
	strSerialNumber = StrToHex(strOrgNumber);
	return true;
}

bool CCodeVerify::VerifyActiveNumber(const std::string strPemFile, const std::string strAcitveNumber, std::string& strDate, std::string& otherInfo)
{
	std::string strOrgActiveNumber = HexToStr(strAcitveNumber);
	std::string strMachineCode = DecodeRSA_PubKeyFile(strPemFile, strOrgActiveNumber);
	if (CHardInfo::VerifyMachineCode(strMachineCode))
	{
		auto valArray = CHardInfo::MachineCodeToArray(strMachineCode);
		strDate = valArray[7].substr(0, 8);
		otherInfo = valArray[7].substr(8, 8);
		return true;
	}
	else
	{
		return false;
	}
}

bool CCodeVerify::CreateActiveNumber(const std::string strPemFile, const std::string strSerialNumber, const std::string strDate, const std::string otherInfo, std::string& strAcitveNumber)
{
	std::string strOrgSerivalNumber = HexToStr(strSerialNumber);
	std::string strOrgNumber = DecodeRSA_PriKeyFile(strPemFile, strOrgSerivalNumber);
	std::string strMachineCode = strOrgNumber.substr(0, 112) + strDate + otherInfo;
	std::string strOrgActiveNumber = EncodeRSA_PriKeyFile(strPemFile, strMachineCode);
	strAcitveNumber = StrToHex(strOrgActiveNumber);
	return true;
}


std::string CCodeVerify::EncodeRSA_PubKeyFile(const std::string& strPemFileName, const std::string& strData)
{
	if (strPemFileName.empty() || strData.empty())
	{
		assert(false);
		return "";
	}
	FILE* hPubKeyFile = nullptr;
	fopen_s(&hPubKeyFile, strPemFileName.c_str(), "rb");
	if (hPubKeyFile == NULL)
	{
		assert(false);
		return "";
	}
	std::string strRet;
	RSA* pRSAPublicKey = RSA_new();
	if (PEM_read_RSA_PUBKEY(hPubKeyFile, &pRSAPublicKey, 0, 0) == NULL)
	{
		assert(false);
		return "";
	}

	int nLen = RSA_size(pRSAPublicKey);
	char* pEncode = new char[nLen + 1];
	int ret = RSA_public_encrypt(strData.length(), (const unsigned char*)strData.c_str(), (unsigned char*)pEncode, pRSAPublicKey, RSA_PKCS1_PADDING);
	if (ret >= 0)
	{
		strRet = std::string(pEncode, ret);
	}
	delete[] pEncode;
	RSA_free(pRSAPublicKey);
	fclose(hPubKeyFile);
	//CRYPTO_cleanup_all_ex_data();
	return strRet;
}

std::string CCodeVerify::DecodeRSA_PriKeyFile(const std::string& strPemFileName, const std::string& strData)
{
	if (strPemFileName.empty() || strData.empty())
	{
		assert(false);
		return "";
	}
	FILE* hPriKeyFile = nullptr;
	fopen_s(&hPriKeyFile, strPemFileName.c_str(), "rb");
	if (hPriKeyFile == NULL)
	{
		assert(false);
		return "";
	}
	std::string strRet;
	RSA* pRSAPriKey = RSA_new();
	if (PEM_read_RSAPrivateKey(hPriKeyFile, &pRSAPriKey, 0, 0) == NULL)
	{
		assert(false);
		return "";
	}

	int nLen = RSA_size(pRSAPriKey);
	char* pEncode = new char[nLen + 1];
	int ret = RSA_private_decrypt(strData.length(), (const unsigned char*)strData.c_str(), (unsigned char*)pEncode, pRSAPriKey, RSA_PKCS1_PADDING);
	if (ret >= 0)
	{
		strRet = std::string(pEncode, ret);
	}
	delete[] pEncode;
	RSA_free(pRSAPriKey);
	fclose(hPriKeyFile);
	//CRYPTO_cleanup_all_ex_data();
	return strRet;
}


std::string CCodeVerify::EncodeRSA_PriKeyFile(const std::string& strPemFileName, const std::string& strData)
{
	if (strPemFileName.empty() || strData.empty())
	{
		assert(false);
		return "";
	}
	FILE* hPriKeyFile = nullptr;
	fopen_s(&hPriKeyFile, strPemFileName.c_str(), "rb");
	if (hPriKeyFile == NULL)
	{
		assert(false);
		return "";
	}
	std::string strRet;
	RSA* pRSAPriKey = RSA_new();
	if (PEM_read_RSAPrivateKey(hPriKeyFile, &pRSAPriKey, 0, 0) == NULL)
	{
		assert(false);
		return "";
	}
	int nLen = RSA_size(pRSAPriKey);
	char* pDecode = new char[nLen + 1];

	int ret = RSA_private_encrypt(strData.length(), (const unsigned char*)strData.c_str(), (unsigned char*)pDecode, pRSAPriKey, RSA_PKCS1_PADDING);
	if (ret >= 0)
	{
		strRet = std::string((char*)pDecode, ret);
	}
	delete[] pDecode;
	RSA_free(pRSAPriKey);
	fclose(hPriKeyFile);
	//CRYPTO_cleanup_all_ex_data();
	return strRet;
}


std::string CCodeVerify::DecodeRSA_PubKeyFile(const std::string& strPemFileName, const std::string& strData)
{
	if (strPemFileName.empty() || strData.empty())
	{
		assert(false);
		return "";
	}
	FILE* hPubKeyFile = nullptr;
	fopen_s(&hPubKeyFile, strPemFileName.c_str(), "rb");
	if (hPubKeyFile == NULL)
	{
		assert(false);
		return "";
	}
	std::string strRet;
	RSA* pRSAPublicKey = RSA_new();
	if (PEM_read_RSA_PUBKEY(hPubKeyFile, &pRSAPublicKey, 0, 0) == NULL)
	{
		assert(false);
		return "";
	}

	int nLen = RSA_size(pRSAPublicKey);
	char* pEncode = new char[nLen + 1];
	int ret = RSA_public_decrypt(strData.length(), (const unsigned char*)strData.c_str(), (unsigned char*)pEncode, pRSAPublicKey, RSA_PKCS1_PADDING);
	if (ret >= 0)
	{
		strRet = std::string(pEncode, ret);
	}
	delete[] pEncode;
	RSA_free(pRSAPublicKey);
	fclose(hPubKeyFile);
	//CRYPTO_cleanup_all_ex_data();
	return strRet;
}