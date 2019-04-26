#include "LogManager.h"
CLoggerManager* CLoggerManager::m_pLogManager = NULL;



CLoggerManager::CLoggerManager()
{
	m_bRunning.store(false);

}
CLoggerManager::~CLoggerManager()
{

	m_bRunning.store(false);
	Sleep(1000);
	if (m_thread)
	{
		m_thread->join();
	}

}


void CLoggerManager::AddLog(NLogger* pLogger)
{
	m_loggerList.push_back(pLogger);
}

CLoggerManager* CLoggerManager::GetInstance()
{
	if (m_pLogManager == NULL)
	{
		m_pLogManager = new CLoggerManager();
		m_pLogManager->StartWriteLogThread();
	}
	return m_pLogManager;
}
void CLoggerManager::StartWriteLogThread()
{
	if (m_bRunning.load() == true)
	{
		return;
	}
	if (!m_thread)
	{
		m_thread = std::make_unique<std::thread>(std::bind([=]()
		{
			m_bRunning.store(true);
			DoWriteLog();
		}		
		));
	}

}

void CLoggerManager::Stop()
{
	m_bRunning.store(false);
	m_loggerList.clear();
}

void CLoggerManager::DoWriteLog()
{
	while(m_bRunning.load())
	{ 		
		std::vector<NLogger*> vcLogList;
		vcLogList = m_loggerList;
		for (int i = 0;i<vcLogList.size();i++)
		{
			if (!m_bRunning.load())
			{
				return;
			}
			if (vcLogList[i] && m_bRunning.load())
			{
				vcLogList[i]->DoWriteLog();
			}
		}
		if (!m_bRunning.load())
		{
			return;
		}
		Sleep(10);
	}

}
