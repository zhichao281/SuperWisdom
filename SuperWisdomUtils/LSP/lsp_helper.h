#ifndef __LSP_HELPER_H__
#define __LSP_HELPER_H__

//#include <atlmisc.h>

#include "common_def.h"

/*
 * code by Root
 * E-mail: cppcoffee@gmail.com
*/
namespace lsp_helper{

	// 多字节转宽字节
	int MByteToWChar( LPCSTR lpMultiByteStr, 
		LPWSTR lpWideCharStr,
		int cchWideChar );

	// 展开环境变量
	CString ExpandEnvironment(LPCTSTR item_data);

	// 获取指定文件的说明和公司名称
	BOOL GetFileDescAndCompany( LPCTSTR file_path, CString& file_descript, CString& company_name );

	// 导出注册表
	BOOL ExportRegPath( LPCTSTR reg_path, LPCTSTR export_path );

	// 读取保存文件的列表
	BOOL ReadRestoreData( TCHAR* buf, DWORD size, LPCTSTR file_path );

}


#endif