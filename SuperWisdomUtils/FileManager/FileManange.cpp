#include "FileManange.h"
#include <Windows.h>
#include <direct.h>
#include <io.h>
#include <string>
#include <sys/stat.h>

using namespace WisdomUtils;
using namespace std;

CFileManange::CFileManange(void)
{
}


CFileManange::~CFileManange(void)
{
}

std::string WisdomUtils::CFileManange::LoadFile( const char* pFilePath )
{
	FILE* pFile = fopen(pFilePath, "rb");
	std::string strData;
	if (pFile)
	{
		int nReadSize = 1024;
		char szBuff[1024];
		int nLen = 0;
		while(1)
		{
			nLen = fread(szBuff, 1, 1024, pFile);
			if (nLen <= 0)
			{
				break;
			}
			strData += std::string(szBuff, nLen);
		}
	}
	fclose(pFile);
	return strData;
}

int WisdomUtils::CFileManange::WriteFile( const char* pFilePath, std::string strData, bool bAppend )
{
	FILE* pFile = fopen(pFilePath, bAppend ? "a" : "w");
	if (pFile)
	{
		fwrite(strData.data(), 1, strData.length(), pFile);
		fclose(pFile);
		return 0;
	}	
	return GetLastError();
}

int WisdomUtils::CFileManange::DelFile( const char* pFilePath )
{
	return remove(pFilePath);
}

int WisdomUtils::CFileManange::CpyFile( const char* pSrcFilePath, const char* pDescFilePath, bool bFailIfExist )
{
	return CopyFileA(pSrcFilePath, pDescFilePath, bFailIfExist);
}

DWORD CALLBACK CopyProgressRoutine(
	LARGE_INTEGER TotalFileSize,              // 文件总字节数
	LARGE_INTEGER TotalBytesTransferred,      // 已复制的字节数
	LARGE_INTEGER StreamSize,                 // 当前流的字节数
	LARGE_INTEGER StreamBytesTransferred,     // 当前流已拷贝的字节数
	DWORD dwStreamNumber,                     // 当前流序号
	DWORD dwCallbackReason,                   // 回调函数的状态; 下面有补充... 
	HANDLE hSourceFile,                       // 源文件句柄
	HANDLE hDestinationFile,                  // 目标文件句柄
	LPVOID lpData                             // CopyFileEx 传递的参数指针
	)
{
		//return PROGRESS_CANCEL;
		CopyFileCallbackFunction pFun = (CopyFileCallbackFunction)lpData;
		return pFun(TotalFileSize.QuadPart, TotalBytesTransferred.QuadPart);
};

int WisdomUtils::CFileManange::CpyFileEx( const char* pSrcFilePath, const char* pDescFilePath, bool bFailIfExist, CopyFileCallbackFunction pFunc )
{
	if (pFunc == nullptr)
	{
		return -1;
	}
	return CopyFileExA(pSrcFilePath, pDescFilePath, CopyProgressRoutine, pFunc, 0, 0);
}

int WisdomUtils::CFileManange::CreateDir( const char* pDir )
{
	int nLen = strlen(pDir);
	std::string strPath;

	for (int i = 0; i < nLen; i++){
		if (pDir[i] == '\\' || pDir[i] == '/'){
			strPath = std::string(pDir, &pDir[i] - pDir);			
			if (_access(strPath.c_str(), 0 != 0)){
				_mkdir(strPath.c_str());
			}
		}
	}
	if (pDir[nLen -1] != '\\' && pDir[nLen -1] != '/'){
		strPath = pDir;
		if (_access(strPath.c_str(), 0 )!= 0){
			_mkdir(strPath.c_str());
		}	
	}		
	return 0;
}

std::list<std::pair<std::string, bool>> WisdomUtils::CFileManange::GetDirFiles( const char* pDir )
{
	list<pair<string, bool>> lstFileInfo;
	WIN32_FIND_DATAA fd;
	HANDLE lFile;
	char szDir[1024] = "";
	int nLen = strlen(pDir);
	sprintf(szDir, "%s%s*.*", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\");

	lFile = FindFirstFileA(szDir, &fd);
	char szSub[1024];
	while(lFile != INVALID_HANDLE_VALUE)
	{
		if(FindNextFileA(lFile, &fd) == false){
			break;
		}
		if ((fd.dwFileAttributes & 0x10) == 0x10){						
			sprintf(szSub, "%s%s%s\\*.*", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\", fd.cFileName);
			if (string(fd.cFileName) != "." && string(fd.cFileName) != ".."){
				char szFileName[1024];
				sprintf(szFileName, "%s%s%s", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\", fd.cFileName);
				lstFileInfo.push_back(make_pair(szFileName, true));
				list<pair<string, bool>> listTemp = GetDirFiles(szFileName);
				for (list<pair<string, bool>>::iterator iter = listTemp.begin(); iter != listTemp.end(); iter++){
					lstFileInfo.push_back(*iter);
				}
			}			
		}else{
			char szFileName[1024];
			sprintf(szFileName, "%s%s%s", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\", fd.cFileName);
			lstFileInfo.push_back(make_pair(szFileName, false));
		}		
	}
	FindClose(lFile);
	return lstFileInfo;
}

bool WisdomUtils::CFileManange::CheckFileExist( const char* pFile )
{
	if (_access(pFile, 0) != 0){
		return false;
	}
	return true;
}

int WisdomUtils::CFileManange::DeleteDir( const char* pDir )
{
	char szPath[1024] = "";
	int nLen = strlen(pDir);
	sprintf(szPath, "%s%s*.*", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\");
	WIN32_FIND_DATAA fd;
	HANDLE lFile;
	lFile = FindFirstFileA(szPath, &fd);

	while(lFile != INVALID_HANDLE_VALUE){
		if (FindNextFileA(lFile, &fd) == false){
			break;
		}

		if (strcmp(fd.cFileName, "..") == 0 || strcmp(fd.cFileName, ".") == 0){
			continue;
		}

		if ((fd.dwFileAttributes & 0x10) == 0x10){
			char szSubDir[1024];
			sprintf(szSubDir, "%s%s%s", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\", fd.cFileName);
			DeleteDir(szSubDir);
		}else{
			char szFile[1024];
			sprintf(szFile, "%s%s%s", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\", fd.cFileName);			
			DelFile(szFile);
		}
	}	
	FindClose(lFile);
	return _rmdir(pDir);
}

long WisdomUtils::CFileManange::GetFileSize( const char* pFile )
{
	struct _stat info;
	_stat((char*)pFile, &info);
	return info.st_size;
}

//描述：获取当前程序所在路径
void WisdomUtils::CFileManange::GetRootDirectoryA(LPSTR pszRootDir)
{
	if(NULL != pszRootDir)
	{
		char szModuleFileName[MAX_PATH] = "";
		GetModuleFileNameA(NULL, szModuleFileName, MAX_PATH);

		char* pRchr = strrchr(szModuleFileName, '\\');
		if(NULL == pRchr)
		{
			pRchr = strrchr(szModuleFileName, '/');
		}

		if(NULL != pRchr)
		{
			pRchr++;
			*pRchr = '\0';
		}

		strcpy(pszRootDir, szModuleFileName);
	}
}


//描述：获取当前程序所在路径
void  WisdomUtils::CFileManange::GetRootDirectoryW(LPTSTR pszRootDir)
{
	if(NULL != pszRootDir)
	{
		TCHAR szModuleFileName[MAX_PATH] = _T("");
		GetModuleFileName(NULL, szModuleFileName, MAX_PATH);

		TCHAR* pRchr = wcsrchr(szModuleFileName, '\\');
		if(NULL == pRchr)
		{
			pRchr = wcsrchr(szModuleFileName, '/');
		}

		if(NULL != pRchr)
		{
			pRchr++;
			*pRchr = '\0';
		}

		StrCpy(pszRootDir, szModuleFileName);
	}
}

string WisdomUtils::CFileManange::GetImageHead(wstring wstrPath)
{
	string strFomat="";
	char pfile[4096];
	//std::wstring wstrPath = strPath.toStdWString();

	int nwstrPathLen = ::WideCharToMultiByte(CP_ACP, 0, (wchar_t*)wstrPath.c_str(), -1, 0, 0, 0, 0);
	if (nwstrPathLen <= 0)
	{
		strFomat = "png";
	}
	WideCharToMultiByte(CP_ACP, 0, (wchar_t*)wstrPath.c_str(), -1, pfile, nwstrPathLen, 0, 0);
	FILE *file=fopen(pfile,"rb");
	if (file)
	{
		char buf[5] ={0},buf1[5] ={0},buf2[5] ={0};		
		fread(buf,5,5,file);
		fclose(file);
		memcpy(buf1, buf, 3);
		memcpy(buf2, &buf[1], 3);
		std::string str1(buf1, 3);
		const char szJpgHead[] = {0xff, 0xd8, 0xff};
		const string strJpgHead(szJpgHead, 3);
		if (strJpgHead == str1)
		{
			strFomat = "jpg";
		}
		else if (strcmp(buf1,"GIF") == 0)
		{
			strFomat="gif";
		}
		else if (strcmp(buf2,"PNG") == 0)
		{
			strFomat="png";
		}
		else		
		{
			strFomat="bmp";
		}

	}
	return strFomat;
}

std::string GetFileType(const std::string& strPath)
{
	std::string strType = "";
	static const char hex_chars[] = "0123456789ABCDEF";
	FILE* fp = fopen(strPath.c_str(), "rb");
	if (fp)
	{
		fseek(fp, 0L, SEEK_END);
		int nFileSize = ftell(fp);
		rewind(fp);

		unsigned char *pData = new unsigned char[nFileSize];

		while (!feof(fp))
		{
			int nRet = fread(pData, sizeof(char), nFileSize, fp);
		}

		fclose(fp);

		static std::map<std::string, std::string>	s_mapFileTypes;
		s_mapFileTypes.insert(std::make_pair("FFD8FF", "jpg"));
		s_mapFileTypes.insert(std::make_pair("89504E47", "png"));
		s_mapFileTypes.insert(std::make_pair("47494638", "gif"));
		s_mapFileTypes.insert(std::make_pair("49492A00", "tif"));
		s_mapFileTypes.insert(std::make_pair("424D", "bmp"));
		s_mapFileTypes.insert(std::make_pair("41433130", "dwg"));
		s_mapFileTypes.insert(std::make_pair("38425053", "psd"));
		s_mapFileTypes.insert(std::make_pair("7B5C727466", "rtf"));
		s_mapFileTypes.insert(std::make_pair("3C3F786D6C", "xml"));
		s_mapFileTypes.insert(std::make_pair("68746D6C3E", "html"));
		s_mapFileTypes.insert(std::make_pair("44656C69766572792D646174653A", "eml"));
		s_mapFileTypes.insert(std::make_pair("CFAD12FEC5FD746F", "dbx"));
		s_mapFileTypes.insert(std::make_pair("2142444E", "pst"));
		s_mapFileTypes.insert(std::make_pair("D0CF11E0", "doc"));
		s_mapFileTypes.insert(std::make_pair("5374616E64617264204A", "mdb"));
		s_mapFileTypes.insert(std::make_pair("FF575043", "wpd"));
		s_mapFileTypes.insert(std::make_pair("255044462D312E", "pdf"));
		s_mapFileTypes.insert(std::make_pair("AC9EBD8F", "qdf"));
		s_mapFileTypes.insert(std::make_pair("E3828596", "pwl"));
		s_mapFileTypes.insert(std::make_pair("504B0304", "zip"));
		s_mapFileTypes.insert(std::make_pair("52617221", "rar"));
		s_mapFileTypes.insert(std::make_pair("57415645", "wav"));
		s_mapFileTypes.insert(std::make_pair("41564920", "avi"));
		s_mapFileTypes.insert(std::make_pair("2E7261FD", "ram"));
		s_mapFileTypes.insert(std::make_pair("2E524D46", "rm"));
		s_mapFileTypes.insert(std::make_pair("000001B", "mpg"));
		s_mapFileTypes.insert(std::make_pair("6D6F6F76", "mov"));
		s_mapFileTypes.insert(std::make_pair("3026B2758E66CF11", "asf"));
		s_mapFileTypes.insert(std::make_pair("4D546864", "mid"));

		std::string strTemp;
		unsigned int c = 0;
		for (int i = 0; i < 50; i++)
		{
			unsigned int x = 0;
			x = pData[i];
			x = pData[i] >> 4;

			c = (pData[i] >> 4) & 0x0f;
			strTemp += hex_chars[c];
			strTemp += hex_chars[pData[i] & 0x0f];
		}

		std::map<std::string, std::string>::iterator iter = s_mapFileTypes.begin();
		for (; iter != s_mapFileTypes.end(); iter++)
		{
			int nLen = iter->first.size();
			std::string strHex = strTemp.substr(0, nLen);
			if (iter->first.c_str() == strHex)
				return iter->second;
		}
	}

	return strType;
}