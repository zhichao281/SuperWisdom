#pragma once
#include "Structures.h"

//LoadFunctions
#define LoadLibraryModule(lib_hmodule, fun_pointer, fun_type, fun_name)  \
	fun_pointer = (fun_type)GetProcAddress(lib_hmodule, fun_name); \
	if(!fun_pointer) {return 3;}

//Function name
//am functions
typedef		int			(*func_AMRestoreRegisterForDeviceNotifications)(
	void (*callback_dfu_connect)(DeviceConnectInfo* state),
	void (*callback_recovery_connect)(DeviceConnectInfo* state),
	void (*callback_dfu_disconnect)(DeviceConnectInfo* state),
	void (*callback_recovery_disconnect)(DeviceConnectInfo* state),
	UINT unknow1,
	UINT unknow2
	);
typedef		int			(*func_AMDServiceConnectionSend)(HANDLE conn,char* data,UINT len);
typedef		int			(*func_AMDServiceConnectionReceive)(HANDLE conn,char* data,UINT* len);
typedef		void		(*func_AMDServiceConnectionInvalidate)(HANDLE conn);
typedef		int			(*func_AMDeviceGetInterfaceType)(HANDLE handle);
typedef		HANDLE		(*func_AMDeviceRetain)(HANDLE handle);
typedef		int			(*func_AMDeviceNotificationSubscribe)(void (*callback)(DeviceConnectInfo* state),UINT uint1,UINT uint2,UINT uint3,HANDLE *notify);
typedef		int			(*func_AMDeviceConnect)(HANDLE handle);
typedef		HANDLE		(*func_AMDeviceCopyDeviceIdentifier)(HANDLE handle);
typedef		int			(*func_AMDeviceDisconnect)(HANDLE handle);
typedef		int			(*func_AMDeviceIsPaired)(HANDLE handle);
typedef		int			(*func_AMDeviceValidatePairing)(HANDLE handle);
typedef		int			(*func_AMDeviceStartSession)(HANDLE handle);
typedef		int			(*func_AMDeviceStopSession)(HANDLE handle);
typedef		int			(*func_AMDeviceSetValue)(HANDLE handle,HANDLE cfstring1,HANDLE cfstring2);
typedef		HANDLE		(*func_AMDeviceCopyValue)(HANDLE handle,HANDLE cfstring1,HANDLE cfstring2);
//cf functions
typedef		HANDLE		(*func_CFStringMakeConstantString)(char* data);
//cf functions
typedef		BOOL		(*func_CFStringGetCString)(HANDLE cfstring,char* byteString,UINT len,UINT encode);
typedef		int 		(*func_CFGetTypeID)(HANDLE type);
typedef		int 		(*func_CFStringGetTypeID)();
typedef		int 		(*func_CFStringGetLength)(HANDLE hStr);

class iTunesApi
{
public:
	iTunesApi(void);
	~iTunesApi(void);
public:
	static HMODULE __hiTunesDeviceDll;
	static HMODULE __hCoreFunctionDll;
public:
	static UINT InitApi();
	static BOOL ReleaseApi();
public:
	//am
	static func_AMRestoreRegisterForDeviceNotifications AMRestoreRegisterForDeviceNotifications;
	static func_AMDServiceConnectionSend AMDServiceConnectionSend;
	static func_AMDServiceConnectionReceive AMDServiceConnectionReceive;
	static func_AMDeviceGetInterfaceType AMDeviceGetInterfaceType;
	static func_AMDServiceConnectionInvalidate AMDServiceConnectionInvalidate;
	static func_AMDeviceRetain AMDeviceRetain;
	static func_AMDeviceNotificationSubscribe AMDeviceNotificationSubscribe;
	static func_AMDeviceConnect AMDeviceConnect;
	static func_AMDeviceCopyDeviceIdentifier AMDeviceCopyDeviceIdentifier;
	static func_AMDeviceDisconnect AMDeviceDisconnect;
	static func_AMDeviceIsPaired AMDeviceIsPaired;
	static func_AMDeviceValidatePairing AMDeviceValidatePairing;
	static func_AMDeviceStartSession AMDeviceStartSession;
	static func_AMDeviceStopSession AMDeviceStopSession;
	static func_AMDeviceSetValue AMDeviceSetValue;
	static func_AMDeviceCopyValue AMDeviceCopyValue;
	//cfstring
	static func_CFStringMakeConstantString CFStringMakeConstantString;
	//cf
	static func_CFStringGetCString CFStringGetCString;
	static func_CFGetTypeID CFGetTypeID;
	static func_CFStringGetTypeID CFStringGetTypeID;
	static func_CFStringGetLength CFStringGetLength;
};

