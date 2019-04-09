#ifndef IFENG_EXPORTS_H
#define IFENG_EXPORTS_H

#ifdef IFENG_EXPORTS_H
#define IFENG_API __declspec(dllexport)
#else
#define IFENG_API __declspec(dllimport)
#endif

#if (defined WIN32 || defined Win64)
#include <windows.h>
#endif


#include "iTunesApi.h"



#ifdef __cplusplus
extern "C"
{
#endif

	/*初始化相关*/
	//初始化
	IFENG_API UINT IOSInitialize(void);
	//释放
	IFENG_API BOOL IOSDispose(void);

	/*连接相关*/
	//注册连接回调函数
	IFENG_API BOOL IOSRegisterOnConnectListener(pConnecttion callback,void* pUserData);
	//注册断开回调函数
	IFENG_API BOOL IOSRegisterOnDisConnectListener(pConnecttion callback,void* pUserData);
	//开始监听
	IFENG_API BOOL IOSStartListen();

	/*设备相关*/
	//打开设备
	IFENG_API BOOL IOSDeviceOpen(HANDLE hDevice);
	//保持设备打开状态
	IFENG_API BOOL IOSDeviceKeepConnect(HANDLE hDevice);
	//关闭设备
	IFENG_API BOOL IOSDeviceClose(HANDLE hDevice);
	//获取设备基本信息
	IFENG_API BOOL IOSGetDeviceInformation(HANDLE hDevice,DeviceProperty* info);
#ifdef __cplusplus
};
#endif

#endif