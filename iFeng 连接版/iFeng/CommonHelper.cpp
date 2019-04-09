#include "StdAfx.h"
#include "CommonHelper.h"
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <shellapi.h>
#include <direct.h>
#include "iTunesApi.h"

void CommonHelper::GetDevicePathName(char* deviceFullPath,char* pathName){
	UINT len = strlen(deviceFullPath);
	UINT pos = 0;
	while (deviceFullPath[len-1] == '/')
	{
		len--;
	}
	if (len>2)
	{
		for (pos=len-1;pos>=0;pos--)
		{
			if (deviceFullPath[pos] == '/')
			{
				break;
			}
		}
	}else{
		pathName[0] = '\0';
		return;
	}
	UINT i = 0;
	while (pos++<len-1)
	{
		pathName[i++] = deviceFullPath[pos];
	}
	pathName[i] = '\0';
}
void CommonHelper::GetDeviceFileName(char* deviceFullPath,char* fileName){
	UINT len = strlen(deviceFullPath);
	UINT pos = 0;
	if (len>2)
	{
		for (pos=len-1;pos>=0;pos--)
		{
			if (deviceFullPath[pos] == '/')
			{
				break;
			}
		}
	}else{
		fileName[0] = '\0';
		return;
	}
	UINT i = 0;
	while (pos++<len-1)
	{
		fileName[i++] = deviceFullPath[pos];
	}
	fileName[i] = '\0';
}
void CommonHelper::DeleteLocalFolder(char* localPath,BOOL deleteAll){
	if (deleteAll)
	{
		WCHAR wcToPath[MAX_PATH];
		mbstowcs(wcToPath,localPath,MAX_PATH);
		SHFILEOPSTRUCT FileOp; 
		ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));
		FileOp.fFlags = FOF_NOCONFIRMATION; 
		FileOp.hNameMappings = NULL; 
		FileOp.hwnd = NULL; 
		FileOp.lpszProgressTitle = NULL; 
		FileOp.pFrom = wcToPath; 
		FileOp.pTo = NULL; 
		FileOp.wFunc = FO_DELETE; 
		SHFileOperation(&FileOp);
	}else
		rmdir(localPath);
}
BOOL CommonHelper::LocalFolderExits(char* localPath){
	return access(localPath,0);
}

void CommonHelper::GetCFString(HANDLE hValue,char* charstring,UINT charstringLen)
{
	UINT retID = iTunesApi::CFGetTypeID(hValue);
	UINT retstring = iTunesApi::CFStringGetTypeID();
	if (retID == retstring)
	{
		UINT len = iTunesApi::CFStringGetLength(hValue);
		iTunesApi::CFStringGetCString(hValue,charstring,charstringLen,kCFStringEncodingUTF8);
	}
}


