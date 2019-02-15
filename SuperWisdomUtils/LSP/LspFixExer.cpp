#include "StdAfx.h"
#include "LspFixExer.h"
#include "LspFix_Tool.h"
#include "lsp_helper.h"


CLspFixExer::CLspFixExer(void)
{
}


CLspFixExer::~CLspFixExer(void)
{
}

BOOL CLspFixExer::EnumLsp()
{
	DeleteAllLspData();
	// 开始枚举LSP
	if ( !lspfix_tool::EnumLspData( EnumLspCallBack, this ) )
	{
		//MessageBox(_T("枚举LSP失败..."), _T("Error"), MB_OK | MB_ICONERROR );
		//PostQuitMessage(-1);
		return FALSE;
	}
	return TRUE;
}

void CLspFixExer::DeleteAllLspData()
{
	while ( m_vLspInfo.size() )
	{
		PLSP_INFO p = m_vLspInfo.back();
		delete p;
		m_vLspInfo.pop_back();
	}
}

BOOL WINAPI CLspFixExer::EnumLspCallBack( void* param, const LPCTSTR pszLspName, const LPCTSTR pszFilePath, const LPCTSTR pszClsid )
{
	if ( param == NULL )
		return FALSE;

	CLspFixExer* fixer = (CLspFixExer*)param;

	// 是否已知的LSP
	BOOL bIsValidLsp = lspfix_tool::IsValidLsp(pszFilePath, pszClsid);

	// 获取文件相关说明信息
	CString file_descript;
	CString company_name;
	lsp_helper::GetFileDescAndCompany( pszFilePath, file_descript, company_name );

	PLSP_INFO pInfo = new LSP_INFO;
	pInfo->clsid = pszClsid;
	pInfo->dll_path = pszFilePath;
	pInfo->bIsValidLsp = bIsValidLsp;
	pInfo->lsp_name = pszLspName;
	fixer->m_vLspInfo.push_back(pInfo);

	return TRUE;
}

void CLspFixExer::OnFix()
{
	lspfix_tool::FixLSP( m_vLspInfo );
}

void CLspFixExer::InitLspList()
{
	EnumLsp();
}

void CLspFixExer::OnRestoreLsp()
{
	lspfix_tool::RestoreLSP();
	EnumLsp();
}
