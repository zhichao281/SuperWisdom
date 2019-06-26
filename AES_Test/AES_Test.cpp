// AES_Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "AesHelper.h"
#include <windows.h>


#include "CCodeVerify.h"


using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{



	CCodeVerify  codeverify;
	std::string strMachineCode =  codeverify.GetSerialNumber();

	std::string strSignOther = "zhichao281@163.com";
	std::string strAcitveNumber = codeverify.CreateActiveNumber(strMachineCode, strSignOther);
	std::string strSignDate;
	codeverify.VerifyActiveNumber(strAcitveNumber, strSignDate, strSignOther);



	//CAesHelper tool;
	//std::string strMachineCode = CHardInfo::GetMachineCode();
	//std::string  strPass = "1234567890123zzc";
	//std::string strOrgNumber = tool.Encrypt(strMachineCode.c_str(), strPass.c_str());




	//printf("Result is \n%s\n", strOrgNumber.c_str());
	//std::string strSource = tool.Decrypt(strOrgNumber.c_str(), strPass.c_str());
	//printf("Result is \n%s\n", strSource.c_str());
	//

	system("pause");
	return 0;
}

