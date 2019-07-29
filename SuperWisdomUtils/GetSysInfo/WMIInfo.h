#pragma once
/*************************************************************************************************

* Description  : [[CN]] 获取系统硬件配置信息 [[CN]]
*
**************************************************************************************************/

#include <windows.h>
#include <tchar.h>
#include <string>
#include <WbemIdl.h>  
#include <iostream>
#include <comdef.h>
#include <vector>
class CWmiInfo
{
public:
	CWmiInfo();
	~CWmiInfo();

public:
	HRESULT InitWmi();    //初始化WMI  
	HRESULT ReleaseWmi(); //释放 

	/*获取一个类成员
	*@param [in ] ClassName   Example: "Win32_Processor"
	*@param [in ] ClassMember Example: "SerialNumber"
	*@param [out] chRetValue
	*@param return TRUE success; false fail

	Example:
	std::string strRetValue;
	GetSingleItemInfo(_T("Win32_Processor"),_T("Caption"),strRetValue);
	*/
	BOOL GetSingleItemInfo(std::string ClassName, std::string ClassMember, std::string &chRetValue, std::string strWhere = "");

	BOOL GetSingleItemInfo(std::string ClassName, std::string ClassMember, std::vector<std::string> &vecRetValue, std::string strWhere = "");




	VARIANT GetSingleItemInfo(std::string ClassName, std::string ClassMember);


	/*获取一个类的多个成员
	*@param [in ] ClassName   Example: "Win32_Processor"
	*@param [in ] ClassMember Example: "SerialNumber"
	*@param [in ] n   成员个数
	*@param [out] chRetValue
	*@param return TRUE success; false fail

	Example:
	std::string strRetValue;std::string [] strClassMem = {_T("Caption"),_T("CurrentClockSpeed"),_T("DeviceID"),_T("Manufacturer"),_T("Manufacturer")};
	GetGroupItemInfo(_T("Win32_Processor"),strClassMem,5,strRetValue);
	*/
	BOOL GetGroupItemInfo(std::string ClassName, std::string ClassMember[], int n, std::string &chRetValue);

private:
	void VariantToString(const LPVARIANT, std::string &) const;//将Variant类型的变量转换为std::string

private:
	IEnumWbemClassObject* m_pEnumClsObj;
	IWbemClassObject* m_pWbemClsObj;
	IWbemServices* m_pWbemSvc;
	IWbemLocator* m_pWbemLoc;
};
