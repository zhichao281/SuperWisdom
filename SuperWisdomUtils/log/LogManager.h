#pragma once
#include "Logger.h"


#define _WINSOCKAPI_

#include <windows.h>
#include <atomic>
#include <thread>
#define _CRT_SECURE_NO_WARNINGS
class CLoggerManager
{
private:
	CLoggerManager();

	~CLoggerManager();


public:
	static CLoggerManager* GetInstance();

	void AddLog(NLogger* pLogger);

	void DoWriteLog();

	void StartWriteLogThread();

	void Stop();

private:
	static CLoggerManager* m_pLogManager;

	std::vector<NLogger*> m_loggerList;

	std::unique_ptr<std::thread> m_thread;

	std::atomic< bool> m_bRunning; //线程是否在运行

};

