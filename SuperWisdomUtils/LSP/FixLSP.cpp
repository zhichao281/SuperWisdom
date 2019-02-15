#include "stdafx.h"


#include "FixLSP.h"

#include "LspFixExer.h"





CFixLSP::CFixLSP()
{

}

CFixLSP::~CFixLSP()
{

}

void CFixLSP::SlientFix()
{
	CLspFixExer fixer;
	fixer.InitLspList();
	fixer.OnFix();

}

void CFixLSP::Restore()
{
	CLspFixExer fixer;
	fixer.InitLspList();
	fixer.OnRestoreLsp();

}



void CFixLSP::OnFixLSP(BOOL bSlient)
{
	std::thread fixlspThread([&]()
	{
		if (bSlient)
		{
			SlientFix();
		}
		else
		{
			Restore();
		}
	});
	fixlspThread.join();
}

/*
*	Function:		IsExist
*	Explanation:	判断文件或目录是否文件
*	Input:			strPath		路径
*	Return:			true 存在 false 不存在
*/
bool IsExist(std::string strPath)
{
	unsigned long		ulAttr;

	if (strPath.empty())
	{
		return false;
	}

	ulAttr = GetFileAttributesA((char *)strPath.c_str());

	if (INVALID_FILE_ATTRIBUTES == ulAttr)
	{
		return false;
	}

	return true;
}

