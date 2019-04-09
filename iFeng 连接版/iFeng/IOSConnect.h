#pragma once
#include "Structures.h"
#include "iTunesApi.h"

class IOSConnect{

public:
	IOSConnect();
	~IOSConnect();
	static pConnecttion OnConnect;
	static pConnecttion OnDisConnect;
	static void* OnConnectUserData;
	static void* OnDisconnectUserData;
	static void DeviceOnConnection(DeviceConnectInfo* info);
};



