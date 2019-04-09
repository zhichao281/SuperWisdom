#pragma once
#include "StdAfx.h"
#include <stdlib.h>
/************************************************************************/
enum CFDataType{
	CFDictionary_Type = 0,
	CFArray_Type = 1,
	CFData_Type = 2,
	CFString_Type = 3,
	CFNumber_Type = 4,
	CFDate_Type = 5,
	CFBoolean_Type = 6,
	UnKnow_Type = 7
};
enum CFPropertyListMutabilityOptions {
	kCFPropertyListImmutable = 0,
	kCFPropertyListMutableContainers = 1,
	kCFPropertyListMutableContainersAndLeaves = 2
};

enum CFStringBuiltInEncodings {
	kCFStringEncodingMacRoman = 0,
	kCFStringEncodingWindowsLatin1 = 0x0500,
	kCFStringEncodingISOLatin1 = 0x0201,
	kCFStringEncodingNextStepLatin = 0x0B01,
	kCFStringEncodingASCII = 0x0600,
	kCFStringEncodingUnicode = 0x0100,
	kCFStringEncodingUTF8 = 0x08000100,
	kCFStringEncodingNonLossyASCII = 0x0BFF,

	kCFStringEncodingUTF16 = 0x0100,
	kCFStringEncodingUTF16BE = 0x10000100,
	kCFStringEncodingUTF16LE = 0x14000100,
	kCFStringEncodingUTF32 = 0x0c000100,
	kCFStringEncodingUTF32BE = 0x18000100,
	kCFStringEncodingUTF32LE = 0x1c000100
};

enum CFNumberType {
	kCFNumberSInt8Type = 1,
	kCFNumberSInt16Type = 2,
	kCFNumberSInt32Type = 3,
	kCFNumberSInt64Type = 4,
	kCFNumberFloat32Type = 5,
	kCFNumberFloat64Type = 6,
	kCFNumberCharType = 7,
	kCFNumberShortType = 8,
	kCFNumberIntType = 9,
	kCFNumberLongType = 10,
	kCFNumberLongLongType = 11,
	kCFNumberFloatType = 12,
	kCFNumberDoubleType = 13,
	kCFNumberCFIndexType = 14,
	kCFNumberNSIntegerType = 15,
	kCFNumberCGFloatType = 16,
	kCFNumberMaxType = 16
};

typedef struct DeviceProperty_Struct{
	WCHAR IdentifyNumber[MAX_PATH];
	WCHAR Name[MAX_PATH];
	WCHAR SerialNumber[MAX_PATH];
	WCHAR ProductType[MAX_PATH];
} DeviceProperty;


typedef void(*pConnecttion)(HANDLE hDevice,void* pUserData);


typedef struct{
	HANDLE hDevice;
	UINT state;
} DeviceConnectInfo;

typedef enum {
	STATE_CONNECT = 1,
	STATE_DISCONNECT = 2,
	STATE_UNKNOW = 3
} CState;

#define SECTIONSIZE (1024*128)