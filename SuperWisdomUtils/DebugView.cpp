#include "DebugView.h"
#include <TlHelp32.h>
#include <string>

using namespace WisdomUtils;

#define _LeaveIfTrue(o) if( o == true) return -1;


std::string DV_ToChar( wchar_t* pSrc )
{
	std::string str;
	int nLen = ::WideCharToMultiByte(CP_ACP, 0, (wchar_t*)pSrc, -1, 0, 0, 0, 0);
	if (nLen <= 0){
		return "";
	}
	str.resize(nLen);
	WideCharToMultiByte(CP_ACP, 0, (wchar_t*)pSrc, -1, (char*)str.data(), nLen, 0, 0);
	return str;
}

WisdomUtils::CDebugView::CDebugView( std::vector<std::string>& vecProcessName )
{
	Init();
	for (auto iter = vecProcessName.begin(); iter != vecProcessName.end(); iter++)
	{
		AddProcess(iter->c_str());
	}
}

WisdomUtils::CDebugView::CDebugView( std::vector<int>& vecPid )
{
	Init();
	for (auto iter = vecPid.begin(); iter != vecPid.end(); iter++)
	{
		AddPid(*iter);
	}
}

void WisdomUtils::CDebugView::Start( bool bAsyc /*= false*/ )
{
	if (bAsyc)
	{
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Listen, this, 0, 0));
	}else
	{
		Listen(this);
	}
	return;
}

void WisdomUtils::CDebugView::SetPrintFunction( pPrintCallback _callback )
{
	if (_callback)
	{
		this->m_func = _callback;
	}else
	{
		this->m_func = DefaultPrint;
	}
}

WisdomUtils::CDebugView::~CDebugView( void )
{

}

void WisdomUtils::CDebugView::AddProcess( const char* pProcessName )
{
#ifdef USE_AUTO_LOCK
	_LOCK_;
#endif
	for (auto iter = m_mapProcess.begin(); iter != m_mapProcess.end(); iter++)
	{
		if (stricmp(iter->second.c_str(), pProcessName) == 0)
		{
			m_scanProcess[iter->first] = iter->second;
		}
	}
}

int WisdomUtils::CDebugView::GetProcessId( char* pProcessName )
{
#ifdef USE_AUTO_LOCK
	_LOCK_;
#endif
	for (auto iter = m_scanProcess.begin(); iter != m_scanProcess.end(); iter++){
		if (iter->second == std::string(pProcessName)){
			return iter->first;
		}
	}
	return -1;
}

char* WisdomUtils::CDebugView::GetProcessName( int nPid )
{
#ifdef USE_AUTO_LOCK
	_LOCK_;
#endif
	auto iter = m_scanProcess.find(nPid);
	if (iter != m_scanProcess.end()){
		return (char*)iter->second.c_str();
	}else{
		return nullptr;
	}
}

void WisdomUtils::CDebugView::SetMapProcess()
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE){
		return;
	}

	BOOL bMore = ::Process32First(hProcessSnap, &pe32); 
	m_mapProcess.clear();
#ifdef USE_AUTO_LOCK
	_LOCK_;
#endif
	while(bMore){
		m_mapProcess[pe32.th32ProcessID] = DV_ToChar(pe32.szExeFile);
		bMore = ::Process32Next(hProcessSnap, &pe32);
	}
}

void WisdomUtils::CDebugView::Init()
{
	SetPrintFunction(nullptr);
	SetMapProcess();
}

void WisdomUtils::CDebugView::AddPid( const int nPid )
{
#ifdef USE_AUTO_LOCK
	_LOCK_;
#endif
	auto iter = m_mapProcess.find(nPid);
	if (iter != m_mapProcess.end()){
		m_scanProcess[iter->first] = iter->second;
	}
}

void WisdomUtils::CDebugView::DefaultPrint( const char* pProcessName, const int nPid, char* pBuff )
{
	printf("[%d][%s]: %s\n", nPid, pProcessName, pBuff);
}

int WisdomUtils::CDebugView::Listen(void* pThis)
{
	CDebugView* pDebug = (CDebugView*)pThis;
	DWORD dwResults = ERROR_INVALID_HANDLE;
	HANDLE hAckEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("DBWIN_BUFFER_READY"));
	_LeaveIfTrue(hAckEvent == nullptr);
	HANDLE hReadyEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("DBWIN_DATA_READY"));
	_LeaveIfTrue(hReadyEvent == nullptr);
	HANDLE hMappingFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAX_SZ_BUFF_SIZE, TEXT("DBWIN_BUFFER"));
	_LeaveIfTrue(hMappingFile == nullptr);
	DebugViewInfo* pdbBuffer = (DebugViewInfo*) MapViewOfFile(hMappingFile, FILE_MAP_READ, 0, 0, 0);
	_LeaveIfTrue(pdbBuffer == nullptr);

	char* pProcess = nullptr;

	for (dwResults = ERROR_SIGNAL_PENDING; (dwResults == ERROR_SIGNAL_PENDING); ){
		SetEvent(hAckEvent);
		if (WaitForSingleObject(hReadyEvent, INFINITE) == WAIT_OBJECT_0){
			if (dwResults == ERROR_SIGNAL_PENDING){
				pProcess = pDebug->GetProcessName(pdbBuffer->nPid);
				if(pProcess){
					pDebug->m_func(pProcess, pdbBuffer->nPid, pdbBuffer->szBuff);
				}
			}
			else{
				dwResults = WAIT_ABANDONED;
			}
		}
	}

	return 0;
}
