#include "StdAfx.h"
#include <stdlib.h>
#include "IOSDevice.h"
#include "CommonHelper.h"

BOOL IOSDevice::IsValid(HANDLE hDevice){
	UINT ret = 0;
	hDevice = iTunesApi::AMDeviceRetain(hDevice);
	ret = iTunesApi::AMDeviceConnect(hDevice);
	ret = iTunesApi::AMDeviceIsPaired(hDevice);
	ret = iTunesApi::AMDeviceValidatePairing(hDevice);
	ret = iTunesApi::AMDeviceStartSession(hDevice);
	WCHAR wcSerialNumber[MAX_PATH];
	IOSDevice::GetDeviceSerialNumber(hDevice,wcSerialNumber);
	ret = iTunesApi::AMDeviceStopSession(hDevice);
	ret = iTunesApi::AMDeviceDisconnect(hDevice);
	ret = iTunesApi::AMDeviceGetInterfaceType(hDevice);
	if (ret == 1)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL IOSDevice::GetCharProperty(HANDLE hDevice,char* cProperty,char* propertyName)
{
	HANDLE handleResult;
	HANDLE cfName = iTunesApi::CFStringMakeConstantString(propertyName);
	handleResult = iTunesApi::AMDeviceCopyValue(hDevice,NULL,cfName);
	if (!handleResult)
	{
		return FALSE;
	}
	CommonHelper::GetCFString(handleResult,cProperty,MAX_PATH);
	return TRUE;
}
BOOL IOSDevice::GetDeviceName(HANDLE hDevice,WCHAR* wcName)
{
	char cReturn[MAX_PATH];
	GetCharProperty(hDevice,cReturn,"DeviceName");
	mbstowcs(wcName,cReturn,MAX_PATH);
	return TRUE;
}
BOOL IOSDevice::GetDeviceID(HANDLE hDevice,WCHAR* wcDeviceID)
{
	char cReturn[MAX_PATH];
	GetCharProperty(hDevice,cReturn,"UniqueDeviceID");
	mbstowcs(wcDeviceID,cReturn,MAX_PATH);
	return TRUE;
}
BOOL IOSDevice::GetDeviceProductType(HANDLE hDevice,WCHAR* wcProductType)
{
	char cReturn[MAX_PATH];
	GetCharProperty(hDevice,cReturn,"ProductType");
	mbstowcs(wcProductType,cReturn,MAX_PATH);
	return TRUE;
}
BOOL IOSDevice::GetDeviceSerialNumber(HANDLE hDevice,WCHAR* wcDeviceSerialNumber)
{
	char cReturn[MAX_PATH];
	GetCharProperty(hDevice,cReturn,"SerialNumber");
	mbstowcs(wcDeviceSerialNumber,cReturn,MAX_PATH);
	return TRUE;
}