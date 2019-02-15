#include "StdAfx.h"

#include <Ws2spi.h>
#include <strsafe.h>

#include "LspFix_Tool.h"
#include "lsp_helper.h"

using namespace std;

namespace lspfix_tool{

	/*
	 * 修复LSP
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	BOOL FixLSP(vector<PLSP_INFO>& lspinfo)
	{
		BackupLspInfo(lspinfo);

		for ( vector<PLSP_INFO>::iterator it = lspinfo.begin();
			it != lspinfo.end();
			++it )
		{
			PLSP_INFO p = *it;
			if ( p->bIsValidLsp || p->dll_path.IsEmpty() )
				continue;

			// 反安装
			DeinstRundll(p->dll_path);
			DeinstallProvider(p->clsid);

			LPCTSTR other_dlls[] = { _T("shdocvw.dll"), _T("Shell32.dll"), _T("Oleaut32.dll"),
				_T("Actxprxy.dll"), _T("Mshtml.dll"), _T("Urlmon.dll"), _T("Msjava.dll"), _T("Browseui.dll") };

			TCHAR path[MAX_PATH];
			GetSystemDirectory(path, _countof(path));
			
			int nIndex;
			for ( nIndex = 0; nIndex < _countof(other_dlls); ++nIndex )
			{
				TCHAR buffer[MAX_PATH];
				StringCchPrintf(buffer, _countof(buffer), _T("%s\\%s"), 
					path, other_dlls[nIndex]);

				if ( 0 == _tcsicmp(buffer, p->dll_path) )
					break;

				GetShortPathName(p->dll_path, buffer, _countof(buffer));

				if ( 0 == _tcsicmp(buffer, p->dll_path) )
					break;
			}

			if ( nIndex != _countof(other_dlls) )
				continue;

			// 没有一个匹配
			SetFileAttributes(p->dll_path, FILE_ATTRIBUTE_NORMAL);
			if ( TRUE == DeleteFile(p->dll_path) )
			{
				DeinstallProvider(p->clsid);
				continue;
			}

			OSVERSIONINFO osinfo;
			osinfo.dwOSVersionInfoSize = sizeof(osinfo);
			GetVersionEx(&osinfo);
			if ( VER_PLATFORM_WIN32_NT == osinfo.dwPlatformId )
			{
				MoveFileEx(p->dll_path, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
			}
			else		// 针对98系统
			{
				if ( !GetShortPathName(p->dll_path, path, _countof(path)) )
					break;

				TCHAR sysdir[MAX_PATH];
				if ( !GetSystemDirectory(sysdir, _countof(sysdir)) )
					break;

				if ( sysdir[_tcslen(sysdir) - 1] != _T('\\') )
				{
					_tcscat_s(sysdir, _countof(sysdir), _T("\\"));
				}

				TCHAR buffer[MAX_PATH * 2];
				StringCchPrintf(buffer, _countof(buffer), _T("%s%s"), sysdir, _T("wininit.ini"));

				FILE* file = NULL;
				_tfopen_s(&file, buffer, _T("a+"));
				if ( file == NULL )
					break;

				rewind(file);
				ZeroMemory(buffer, sizeof(buffer));

				BOOL bIsFind = FALSE;
				while ( _fgetts(buffer, _countof(buffer), file) != NULL )
				{
					if ( !_tcsnicmp(_T("[rename]"), buffer, _tcslen(_T("[rename]"))) )
					{
						bIsFind = TRUE;
						break;
					}
				}

				fseek(file, 0, SEEK_END);
				if ( bIsFind == FALSE )
					_ftprintf_s(file, _T("%s\n"), _T("[rename]"));
				_ftprintf_s(file, _T("NUL=%s\n"), path);
				fclose(file);
			}

			DeinstallProvider(p->clsid);
		}

		return TRUE;
	}


	/*
	 * 恢复LSP
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	BOOL RestoreLSP()
	{
		TCHAR file_path[MAX_PATH];
		GetModuleFileName(NULL, file_path, _countof(file_path));	
		TCHAR* p = PathFindFileName(file_path);
		ZeroMemory(p, sizeof(TCHAR));

		TCHAR buffer[MAX_PATH];
		StringCchPrintf(buffer, _countof(buffer), _T("%s%s"), file_path, _T("WinSockLSP.reg"));

		if ( !PathFileExists(buffer) )
			return FALSE;

		// 导入WinSockLSP.reg文件
		ImportRegFile(buffer);

		// 还原DLL
		RestoreDllFile();

		// 导入WinSockLSPIFSL.reg文件
		StringCchPrintf(buffer, _countof(buffer), _T("%s%s"), file_path, _T("WinSockLSPIFSL.reg"));
		if ( !PathFileExists(buffer) )
			return FALSE;

		ImportRegFile(buffer);

		return TRUE;
	}


	/*
	 * 还原保存的LSP DLL
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	BOOL RestoreDllFile()
	{
		TCHAR current_path[MAX_PATH];
		GetModuleFileName(NULL, current_path, _countof(current_path));
		TCHAR* p = PathFindFileName(current_path);
		ZeroMemory(p, sizeof(TCHAR));

		TCHAR buffer[MAX_PATH];
		StringCchPrintf(buffer,
			_countof(buffer),
			_T("%sLSP\\LSP.ini"),
			current_path);

		TCHAR data[4096];
		ZeroMemory(data, sizeof(data));
		if ( !lsp_helper::ReadRestoreData(data, _countof(data), buffer) )
			return FALSE;

		TCHAR* pFind = data;
		while ( (pFind = _tcschr(pFind, '=')) != NULL )
		{
			++pFind;

			TCHAR file_path[MAX_PATH];
			ZeroMemory(file_path, sizeof(file_path));

			TCHAR* pTmp = _tcsstr(pFind, _T("\r\n"));
			_tcsncpy_s(file_path, _countof(file_path), pFind, pTmp - pFind);

			// 文件如果存在则不进行拷贝
			if ( PathFileExists(file_path) )
				continue;

			TCHAR* pFileName = PathFindFileName(file_path);
			if ( pFileName == NULL )
				continue;

			StringCchPrintf(buffer, 
				_countof(buffer), 
				_T("%sLSP\\%s"),
				current_path,
				pFileName);

			// 保存的文件是否存在
			if ( !PathFileExists(buffer) )
				continue;

			CopyLspDllFile(buffer, file_path);
		}

		return TRUE;
	}


	/*
	 * 导入指定的REG文件
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	BOOL ImportRegFile(LPCTSTR regpath)
	{
		TCHAR command[MAX_PATH * 2];
		ZeroMemory( command, sizeof(command) );
		StringCchPrintf( command, 
			_countof(command),
			_T("/s \"%s\""),
			regpath );

		SHELLEXECUTEINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cbSize = sizeof(si);
		si.fMask = SEE_MASK_NOCLOSEPROCESS;
		si.lpVerb = _T("open");
		si.lpFile = _T("regedit.exe");
		si.nShow = SW_SHOW;
		si.lpParameters = command;
		if ( !ShellExecuteEx(&si) )
			return FALSE;

		WaitForSingleObject(si.hProcess, INFINITE);
		CloseHandle(si.hProcess);
		return TRUE;
	}


	/*
	 * 删除指定CLSID相关的LSP信息
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	void DeinstallProvider(LPCTSTR clsid)
	{
		int nError = 0;
		CLSID id;
		LPOLESTR ole_clsid = (LPOLESTR)T2COLE(clsid);
		CLSIDFromString(ole_clsid, &id);
#ifdef _WIN64
		WSCDeinstallProvider(&id, &nError);
		WSCDeinstallProvider32(&id, &nError);
#else
		WSCDeinstallProvider(&id, &nError);
#endif
	}


	/*
	 * 获取ProtocolCatalog完整注册表路径
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	CString GetCurrentProtocolCatalogPath()
	{
		DWORD type = REG_SZ;
		TCHAR buffer[MAX_PATH] = {0};
		DWORD chData = sizeof(buffer);

		LONG lret = SHGetValue(HKEY_LOCAL_MACHINE, 
			_T("System\\CurrentControlSet\\Services\\Winsock2\\Parameters"),
			_T("Current_Protocol_Catalog"),
			&type,
			(void*)buffer,
			&chData);

		if ( lret != ERROR_SUCCESS )
			return _T("");

		CString catalog_path;

		catalog_path.Format(_T("%s\\%s\\"), 
			_T("System\\CurrentControlSet\\Services\\Winsock2\\Parameters"), 
			buffer );

		return catalog_path;
	}


	/*
	 * 枚举LSP(参数是回调函数)
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	BOOL EnumLspData(PEnumLspCallBack CallBackFuncPtr, void* param)
	{
		if ( CallBackFuncPtr == NULL )
			return FALSE;

		CString catalog_path = GetCurrentProtocolCatalogPath();
		if ( catalog_path.IsEmpty() )
			return FALSE;

		int num_catalog = GetNumCatalogEntries(catalog_path);

		// 进行编录的枚举
		CRegKey reg;
		CString catalog_entries(catalog_path);
		catalog_entries += _T("Catalog_Entries");

		if ( ERROR_SUCCESS != reg.Open(HKEY_LOCAL_MACHINE, catalog_entries, KEY_READ ) )
			return FALSE;

		CString lsp_dll_path;
		CString lsp_clsid;
		for ( int num_index = 0; num_index < num_catalog; num_index++ )
		{
			TCHAR catalog_name[MAX_PATH] = {0};
			DWORD name_len = sizeof(catalog_name);
			if ( ERROR_SUCCESS != reg.EnumKey( num_index, catalog_name, &name_len ) )
				continue;

			// PackedCatalogItem
			PACKED_CATALOG_ITEM item_data;
			ZeroMemory(&item_data, sizeof(item_data));

			if ( !GetPackedCatalogItem( catalog_entries, 
				catalog_name,
				&item_data) )
			{
				continue;
			}

			// 获取LSP DLL的全路径
			lsp_dll_path.Empty();
			lsp_dll_path = GetPathForPackedItem(item_data.spi_path);
			if ( lsp_dll_path.IsEmpty() )
				continue;

			lsp_clsid.Empty();
			lsp_clsid = GetClsidFromItem(&item_data);

			// 调用回调函数
			CallBackFuncPtr( param,
				item_data.protocol_info.szProtocol,
				lsp_dll_path,
				lsp_clsid );
		}

		return TRUE;
	}


	/*
	 * 转换CLSID到字符串
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	CString GetClsidFromItem( PPACKED_CATALOG_ITEM item_data )
	{
		LPOLESTR ole_clsid = NULL;
		if ( S_OK != StringFromCLSID( item_data->protocol_info.ProviderId, &ole_clsid ) )
			return _T("");

		CString result(ole_clsid);
		CoTaskMemFree( ole_clsid );
		return result;
	}


	/*
	 * 获取LSP的dll完整路径
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	CString GetPathForPackedItem(char* spi_path)
	{
		CString full_path(_T(""));

#ifdef UNICODE
		int len = lsp_helper::MByteToWChar((char*)spi_path, NULL, 0);
		len += sizeof(WCHAR);
		WCHAR* pBuf = (WCHAR*)malloc(sizeof(WCHAR) * len);
		lsp_helper::MByteToWChar((char*)spi_path, pBuf, len);
		full_path = pBuf;
		free(pBuf);
#else
		full_path = (WCHAR*)spi_path;
#endif
		// 路径展开
		return lsp_helper::ExpandEnvironment(full_path);
	}


	/*
	 * 保存在注册表中的LSP结构体信息
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/ 
	BOOL GetPackedCatalogItem( LPCTSTR catalog_path, 
		LPCTSTR item_name, 
		PPACKED_CATALOG_ITEM buffer )
	{
		CString item_path(catalog_path);
		item_path += _T("\\");
		item_path += item_name;

		DWORD type = REG_BINARY;
		DWORD buflen = sizeof(PACKED_CATALOG_ITEM);

		LONG lret = SHGetValue(HKEY_LOCAL_MACHINE, 
			item_path,
			_T("PackedCatalogItem"),
			&type,
			(void*)buffer,
			&buflen );

		return lret == ERROR_SUCCESS;		
	}


	/*
	 * 获取编录项的总数
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	int GetNumCatalogEntries(LPCTSTR catalog_path)
	{
		if ( catalog_path == NULL )
			return -1;

		int num_catalog = -1;
		DWORD buf_len = sizeof(num_catalog);
		DWORD type = REG_DWORD;

		LONG lret = SHGetValue( HKEY_LOCAL_MACHINE,
			catalog_path,
			_T("Num_Catalog_Entries"),
			&type,
			(void*)&num_catalog,
			&buf_len );

		if ( lret != ERROR_SUCCESS )
			return -1;

		return num_catalog;
	}


	/*
	 * 是否已经正确的LSP
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	BOOL IsValidLsp( LPCTSTR file_path, LPCTSTR clsid )
	{
		if ( clsid == NULL )
			return FALSE;

		LPCTSTR valid_clsid[] = {
			_T("{E70F1AA0-AB8B-11CF-8CA3-00805F48A192}"),
			_T("{9D60A9E0-337A-11D0-BD88-0000C082E69A}"),
			_T("{8D5F1830-C273-11CF-95C8-00805F48A192}"),
			_T("{3972523D-2AF1-11D1-B655-00805F3642CC}")
		};

		// 匹配CLSID
		int clsid_count = _countof(valid_clsid);
		int index = 0;
		for ( index = 0; index < clsid_count; ++index )
		{
			if ( _tcsicmp(clsid, valid_clsid[index]) == 0 )
				break;
		}

		if ( index == clsid_count )
			return FALSE;

		// 匹配目录
		TCHAR sys_directory[MAX_PATH] = {0};
		GetSystemDirectory( sys_directory, _countof(sys_directory) );
		
		LPCTSTR valid_dllname[] = {
			_T("mswsock.dll"),
			_T("rsvpsp.dll"),
			_T("msafd.dll"),
			_T("ws2_64.dll")
		};

		TCHAR path_buf[MAX_PATH];
		for ( index = 0; index < _countof(valid_dllname); ++index )
		{
			StringCchPrintf( path_buf, 
				_countof(path_buf),
				_T("%s\\%s"),
				sys_directory, 
				valid_dllname[index] );

			if ( _tcsicmp(file_path, path_buf) == 0 )
				return TRUE;
		}

		return FALSE;
	}


	/*
	 * 备份LSP信息
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	void BackupLspInfo(vector<PLSP_INFO>& lspinfo)
	{
		TCHAR current_path[MAX_PATH];
		GetModuleFileName(NULL, current_path, _countof(current_path));
		TCHAR* p = PathFindFileName(current_path);
		ZeroMemory(p, sizeof(TCHAR));

		TCHAR temp_path[MAX_PATH];
		GetTempPath(MAX_PATH, temp_path);

		TCHAR new_path[MAX_PATH * 2];
		StringCchPrintf( new_path, 
			_countof(new_path), 
			_T("%s%s"), 
			current_path,
			_T("WinSockLSP.reg") );

		// 文件已经备份
		if ( PathFileExists(new_path) )
			return;

		TCHAR old_path[MAX_PATH * 2];
		StringCchPrintf( old_path,
			_countof(old_path),
			_T("%s%s"),
			temp_path,
			_T("WinSockLSP.reg") );

		// 导出成功后,将文件复制程序当前路径下
		LPCTSTR reg_lsp_path = _T("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\WinSock2\\Parameters");
		if ( lsp_helper::ExportRegPath(reg_lsp_path, old_path) )
		{
			CopyFile(old_path, new_path, FALSE);
		}

		StringCchPrintf( old_path, 
			_countof(old_path),
			_T("%s%s"),
			current_path,
			_T("WinSockLSPIFSL.reg") );

		LPCTSTR reg_ifsl_path = _T("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\WS2IFSL");
		if ( !lsp_helper::ExportRegPath(reg_ifsl_path, old_path) )
			return;

		StringCchPrintf(new_path, 
			_countof(new_path), 
			_T("%s%s"), 
			current_path, 
			_T("WinSockLSPIFSL.reg") );

		CopyFile(old_path, new_path, FALSE);

		// 对未知的LSP文件进行备份
		BackupDllFile(lspinfo);
	}


	/*
	 * 备份LSP DLL文件
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	void BackupDllFile(vector<PLSP_INFO>& lspinfo)
	{
		//only return here
		return;

		TCHAR current_path[MAX_PATH];
		GetModuleFileName(NULL, current_path, _countof(current_path));
		TCHAR* file_name = PathFindFileName(current_path);
		ZeroMemory(file_name, sizeof(TCHAR));

		TCHAR buffer[MAX_PATH];
		StringCchPrintf(buffer, _countof(buffer), _T("%sLSP"), current_path);

		CreateDirectory(buffer, NULL);

		for ( vector<PLSP_INFO>::iterator it = lspinfo.begin();
			it != lspinfo.end();
			++it )
		{
			PLSP_INFO info = *it;
			if ( info->bIsValidLsp )
				continue;

			LPCTSTR dll_path = info->dll_path;
			file_name = PathFindFileName(dll_path);
			if ( file_name == NULL )
				continue;

			StringCchPrintf( buffer, 
				_countof(buffer), 
				_T("%sLSP\\%s"),
				current_path,
				file_name );

			// 文件是否已经存在
			if ( PathFileExists(buffer) )
				continue;

			if ( !CopyLspDllFile(dll_path, buffer) )
				continue;

			StringCchPrintf(buffer, _countof(buffer), _T("%sLSP\\LSP.ini"), current_path);
			WritePrivateProfileString(_T("LSP"), info->clsid, dll_path, buffer);
		}
	}

	/*
	 * 拷贝LSP的DLL文件到指定的路径
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	BOOL CopyLspDllFile(LPCTSTR src, LPCTSTR dst)
	{
		HANDLE hDllHandle = CreateFile( src, 
			GENERIC_READ, 
			FILE_SHARE_READ, 
			NULL, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL );
		if ( hDllHandle == INVALID_HANDLE_VALUE )
			return FALSE;

		LPBYTE buffer = (LPBYTE)malloc(0x8000);
		HANDLE hNewFile = CreateFile( dst, 
			GENERIC_WRITE, 
			FILE_SHARE_READ, 
			NULL, 
			CREATE_NEW, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL );
		
		DWORD file_size = GetFileSize(hDllHandle, NULL);
		while ( file_size )
		{
			DWORD read_size;
			ReadFile(hDllHandle, buffer, 0x8000, &read_size, NULL);
			WriteFile(hNewFile, buffer, read_size, &read_size, NULL);
			file_size -= read_size;
		}

		CloseHandle(hNewFile);
		CloseHandle(hDllHandle);
		free(buffer);
		return TRUE;
	}

	/*
	 * 反安装Rundll32
	 * code by Root
	 * E-mail: cppcoffee@gmail.com
	*/
	BOOL DeinstRundll(LPCTSTR dll_path)
	{
		if ( dll_path == NULL )
			return FALSE;

		if ( _tcslen(dll_path) <= 0 )
			return FALSE;

		TCHAR ch = '.';
		LPCTSTR p = _tcsrchr(dll_path, ch);
		if ( p == NULL )
			return FALSE;

		if ( 0 != _tcsicmp(p, _T(".dll")) )
			return FALSE;

		TCHAR short_path[MAX_PATH];
		GetShortPathName(dll_path, short_path, _countof(short_path));

		TCHAR command[512];
		StringCchPrintf( command, 
			_countof(command),
			_T("rundll32.exe /u /s %s"),
			short_path );

		STARTUPINFO startup_info;
		ZeroMemory(&startup_info, sizeof(startup_info));
		startup_info.cb = sizeof(startup_info);
		startup_info.wShowWindow = SW_HIDE;
		startup_info.dwFlags |= STARTF_USESHOWWINDOW;

		PROCESS_INFORMATION ProcessInfo;
		ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

		if ( !CreateProcess( NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, 
			&startup_info, &ProcessInfo) )
		{
			return FALSE;
		}

		WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
		CloseHandle(ProcessInfo.hProcess);
		CloseHandle(ProcessInfo.hThread);

		return TRUE;
	}

}
