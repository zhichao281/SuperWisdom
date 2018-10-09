
#pragma  once

#include <thread>
#include <Windows.h>
#include <tchar.h>
class FixLspHelper
{
public:
	FixLspHelper();

	int CallProc(BOOL bWaitForProcessExit, const char* pProcessPath, const char* pCmdlineFmt, ...);

	BOOL IsX64();

	char* GetApplicationPath();

protected:	

	char szApplicationPath[2048];
};

class CFixLSP 
{
public:
	CFixLSP();
	~CFixLSP();	

public :
	void OnFixLSP(BOOL bSlient);	
private:
	//¾²Ä¬ÐÞ¸´
	void SlientFix();
	//µ¯´°ÐÞ¸´
	void Restore();
	FixLspHelper fixlspHelper;
};

