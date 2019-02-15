#include "stdafx.h"

#include <strsafe.h>

#include "lsp_helper.h"


namespace lsp_helper{

	/*
	 * 多字节转宽字节
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	int MByteToWChar( LPCSTR lpMultiByteStr,      
		LPWSTR lpWideCharStr,  
		int cchWideChar )
	{
		return MultiByteToWideChar(CP_ACP, 
			0, 
			lpMultiByteStr,
			-1,
			lpWideCharStr,
			cchWideChar );
	}

	/*
	 * 展开环境变量
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	CString ExpandEnvironment(LPCTSTR item_data)
	{
		const TCHAR *env_begin_pos;
		const TCHAR *env_end_pos;
		TCHAR env_name[MAX_PATH];
		TCHAR buffer[MAX_PATH];

		if ( item_data == NULL )
			return _T("");

		if ( *item_data != '%' )
			return item_data;

		env_begin_pos = CharNext(item_data);
		env_end_pos = _tcschr(env_begin_pos, '%');
		if ( !env_end_pos )
			return _T("");

		memset(env_name, 0, sizeof(env_name));
		_tcsncpy_s(env_name, MAX_PATH, env_begin_pos, env_end_pos - env_begin_pos);
		if ( !GetEnvironmentVariable(env_name, buffer, _countof(buffer)) )
			return _T("");

		env_end_pos = CharNext(env_end_pos);
		CString full_path(buffer);
		full_path += env_end_pos;
		return full_path;
	}


	/*
	 * 从指定的文件获取说明和公司名称
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	BOOL GetFileDescAndCompany( LPCTSTR file_path, CString& file_descript, CString& company_name )
	{
		file_descript.Empty();
		company_name.Empty();

		DWORD version_size = GetFileVersionInfoSize( file_path, 0 );

		if ( version_size == 0 )
			return FALSE;

		CTempBuffer<BYTE> version_data(version_size);
		GetFileVersionInfo( file_path, 0, version_size, version_data );
		LANGANDCODEPAGE* codepage;
		UINT len;

		if ( !VerQueryValue(version_data, 
			_T("\\VarFileInfo\\Translation"), 
			(LPVOID*)&codepage, 
			&len) )
		{
			return FALSE;
		}

		TCHAR subblock[MAX_PATH] = {0};
		StringCchPrintf( subblock, 
			_countof(subblock), 
			_T("\\StringFileInfo\\%04x%04x\\FileDescription"),
			codepage->wLanguage,
			codepage->wCodePage );

		LPCWSTR value = NULL;
		if ( VerQueryValue(version_data, subblock, (LPVOID*)&value, &len ) )
		{
			file_descript = value;
		}

		StringCchPrintf( subblock, 
			_countof(subblock),
			_T("\\StringFileInfo\\%04x%04x\\CompanyName"),
			codepage->wLanguage,
			codepage->wCodePage );

		if ( VerQueryValue(version_data, subblock, (LPVOID*)&value, &len) )
		{
			company_name = value;
		}

		return TRUE;
	}


	/*
	 * 导出指定注册表路径到指定的文件路径
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	BOOL ExportRegPath( LPCTSTR reg_path, LPCTSTR export_path )
	{
		// 进行导出操作
		SHELLEXECUTEINFO ExecInfo = {0};
		ExecInfo.cbSize = sizeof(ExecInfo);
		ExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ExecInfo.lpVerb = _T("open");
		ExecInfo.lpFile = _T("regedit.exe");
		ExecInfo.nShow = SW_SHOW;
		CString strParams(_T("/e \""));
		strParams += export_path;
		strParams += _T("\" ");
		strParams += _T("\"");
		strParams += reg_path;
		strParams += _T("\"");
		ExecInfo.lpParameters = strParams;
		if ( !ShellExecuteEx(&ExecInfo) )
			return FALSE;

		WaitForSingleObject(ExecInfo.hProcess, INFINITE);
		CloseHandle(ExecInfo.hProcess);
		return TRUE;
	}


	/*
	 * 读取保存文件的列表信息
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	BOOL ReadRestoreData( TCHAR* buf, DWORD buf_size, LPCTSTR file_path )
	{
		HANDLE hFile = CreateFile( file_path,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL );
		if ( hFile == INVALID_HANDLE_VALUE )
			return FALSE;

		DWORD dwFileSize = GetFileSize(hFile, NULL);
		if ( dwFileSize > buf_size )
			return FALSE;

		DWORD dwSize;
		++dwFileSize;
		CHAR* data = new CHAR[dwFileSize];
		ZeroMemory(data, dwFileSize);
		ReadFile(hFile, data, dwFileSize, &dwSize, NULL);

		int len = MByteToWChar(data, NULL, 0);
		if ( buf_size < (DWORD)len )
		{
			delete [] data;
			return FALSE;
		}

		MByteToWChar( data, buf, buf_size );

		delete [] data;
		return TRUE;
	}

}

