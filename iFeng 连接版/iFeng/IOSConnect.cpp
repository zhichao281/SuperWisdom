#include "StdAfx.h"
#include "IOSConnect.h"
#include "IOSDevice.h"

pConnecttion IOSConnect::OnConnect;
pConnecttion IOSConnect::OnDisConnect;
void* IOSConnect::OnConnectUserData;
void* IOSConnect::OnDisconnectUserData;

void IOSConnect::DeviceOnConnection(DeviceConnectInfo* info)
{
	if (info->state == STATE_CONNECT)
	{
		if (IOSDevice::IsValid(info->hDevice))
		{
			OnConnect(info->hDevice,OnConnectUserData);
		}
		return;
	}
	else if (info->state == STATE_DISCONNECT)
	{
		OnDisConnect(info->hDevice,OnDisconnectUserData);
	}
}
