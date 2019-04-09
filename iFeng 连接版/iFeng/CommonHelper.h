#pragma once
class CommonHelper
{

public:
	static void GetDevicePathName(char* deviceFullPath,char* pathName);
	static void GetDeviceFileName(char* deviceFullPath,char* fileName);
	static void DeleteLocalFolder(char* localPath,BOOL deleteAll = TRUE);
	static BOOL LocalFolderExits(char* localPath);
	static void GetCFString(HANDLE hValue,char* charstring,UINT charstringLen);
};

