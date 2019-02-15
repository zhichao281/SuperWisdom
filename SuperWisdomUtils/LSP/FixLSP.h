
#pragma  once

#include <thread>
#include <Windows.h>
#include <tchar.h>

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

};

