//#include "StdAfx.h"
#include "LiteLog.h"
#include <algorithm>
#include <io.h>
#include <direct.h>

int CreateDir( const char* pDir )
{
	int nLen = strlen(pDir);
	std::string strPath;

	for (int i = 0; i < nLen; i++)
	{
		if (pDir[i] == '\\' || pDir[i] == '/')
		{
			strPath = std::string(pDir, &pDir[i] - pDir);			
			if (_access(strPath.c_str(), 0 != 0))
			{
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
//初始化 读取配置文件
WisdomUtils::CLiteLog::CLiteLog() : m_nLogLevel(LOG_INFO), pLogFile(0)
{
	::InitializeCriticalSection(&cs);
	char szConfig[1024];
	GetModuleFileNameA(NULL, szConfig, 1024);
	char* p = strrchr(szConfig, '\\');
	*p = 0;
	strcat(szConfig, "\\config.cfg");
	Config(szConfig);
}

WisdomUtils::CLiteLog::~CLiteLog()
{
	::DeleteCriticalSection(&cs);
	if (pLogFile){
		fclose(pLogFile);
		pLogFile = 0;
	}
}

WisdomUtils::CLiteLog* WisdomUtils::CLiteLog::GetLog()
{
	static CLiteLog self;
	return &self;
}

void WisdomUtils::CLiteLog::Config( const char* pConfigFile )
{
	/*
	[set]
	#ERROR 2
	#DEBUG 1
	#INFO 0	
	log_level=0
	dir_path=d:\x\y
	file_name=xxx.log
	*/

	m_nLogLevel = GetPrivateProfileIntA("set", "log_level", 0, pConfigFile);

	char szDirPath[1024];
	GetModuleFileNameA(NULL, szDirPath, 1024);
	char *p = strrchr(szDirPath, '\\');
	*p = 0;
	GetPrivateProfileStringA("set", "dir_path", szDirPath, szDirPath, 1024, pConfigFile);
	CreateDir(szDirPath);

	char szFileName[128];
	sprintf(szFileName, "%s.log", GetTime().c_str());
	std::string strFileName(szFileName);
	std::replace(strFileName.begin(), strFileName.end(), ':', '_');
	GetPrivateProfileStringA("set", "file_name", strFileName.c_str(), szFileName, 128, pConfigFile);
	if (szDirPath[strlen(szDirPath) - 1] != '\\'){
		strcat(szDirPath, "\\");
	}
	strcat(szDirPath, szFileName);
	if (pLogFile){
		fclose(pLogFile);
	}		
	pLogFile = fopen(szDirPath, "a");	
	fflush(pLogFile);
}

void WisdomUtils::CLiteLog::Log( int nLogLevel, char* pModule, char* pCppFile, int nLine, char* pFmt, ... )
{
	if (nLogLevel < m_nLogLevel){
		return;
	}
	Lock();
	fprintf(pLogFile, "\n[%s][%s][%s_%s(%d)]", GetTime().c_str(), GetType(nLogLevel), pCppFile,pModule, nLine);
	va_list ap;
	va_start(ap, pFmt);
	vfprintf(pLogFile, pFmt, ap);
	va_end(ap);
	fflush(pLogFile);
	UnLock();
}

void WisdomUtils::CLiteLog::Debug( char* pModule, char* pCppFile, int nLine, char* pFmt, ... )
{
	if (m_nLogLevel > LOG_DEBUG){
		return;
	}
	Lock();
	fprintf(pLogFile, "\n[%s][%s][%s_%s(%d)]", GetTime().c_str(), GetType(LOG_DEBUG), pCppFile,pModule, nLine);
	va_list ap;
	va_start(ap, pFmt);
	vfprintf(pLogFile, pFmt, ap);
	va_end(ap);
	fflush(pLogFile);
	UnLock();
}

void WisdomUtils::CLiteLog::Info( char* pModule, char* pCppFile, int nLine, char* pFmt, ... )
{
	if (m_nLogLevel > LOG_INFO){
		return;
	}
	Lock();
	fprintf(pLogFile, "\n[%s][%s][%s_%s(%d)]", GetTime().c_str(), GetType(LOG_INFO), pCppFile,pModule, nLine);
	va_list ap;
	va_start(ap, pFmt);
	vfprintf(pLogFile, pFmt, ap);
	va_end(ap);
	fflush(pLogFile);
	UnLock();
}

void WisdomUtils::CLiteLog::Error( char* pModule, char* pCppFile, int nLine, char* pFmt, ... )
{
	if (m_nLogLevel > LOG_ERROR){
		return;
	}
	Lock();
	fprintf(pLogFile, "\n[%s][%s][%s_%s(%d)]", GetTime().c_str(), GetType(LOG_ERROR), pCppFile,pModule, nLine);
	va_list ap;
	va_start(ap, pFmt);
	vfprintf(pLogFile, pFmt, ap);
	va_end(ap);
	fflush(pLogFile);
	UnLock();
}

void WisdomUtils::CLiteLog::SetLogLevel( int nLogLevel )
{
	m_nLogLevel = nLogLevel;
}


