#pragma once
#include "Structures.h"
#include "iTunesApi.h"

class IOSDevice
{
public:
	IOSDevice(void);
	~IOSDevice(void);

	static BOOL IsValid(HANDLE hDevice);

	static BOOL GetCharProperty(HANDLE hDevice,char* cProperty,char* propertyName);
	static BOOL GetDeviceName(HANDLE hDevice,WCHAR* wcName);
	static BOOL GetDeviceID(HANDLE hDevice,WCHAR* wcDeviceID);
	static BOOL GetDeviceProductType(HANDLE hDevice,WCHAR* wcProductType);
	static BOOL GetDeviceSerialNumber(HANDLE hDevice,WCHAR* wcDeviceSerialNumber);
};

