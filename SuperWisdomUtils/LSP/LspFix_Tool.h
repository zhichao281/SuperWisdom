#ifndef __LSP_FIX_TOOL_H__
#define __LSP_FIX_TOOL_H__

#include <vector>

#include "common_def.h"


/*
 * 枚举LSP时调用的回调函数声明
 * code by Root
 * E-mail: cppcoffee@gmail.com
*/
typedef BOOL (WINAPI *PEnumLspCallBack)(void* param,
										const LPCTSTR lsp_name, 
										const LPCTSTR dll_file_path,
										const LPCTSTR clsid);


/*
 * 保存在注册表中的LSP结构体信息
 * code by Root
 * E-mail: cppcoffee@gmail.com
*/
typedef struct tagPACKED_CATALOG_ITEM
{
	char spi_path[MAX_PATH];
	WSAPROTOCOL_INFO protocol_info;
} PACKED_CATALOG_ITEM, *PPACKED_CATALOG_ITEM;


namespace lspfix_tool{

	// 修复LSP
	BOOL FixLSP(std::vector<PLSP_INFO>& lspinfo);

	// 恢复LSP到最后一次状态
	BOOL RestoreLSP();

	// 枚举LSP(参数是回调函数)
	BOOL EnumLspData(PEnumLspCallBack CallBackFuncPtr, void* param);

	// 获取ProtocolCatalog完整注册表路径
	CString GetCurrentProtocolCatalogPath();

	// 获取获取编录项的总数
	int GetNumCatalogEntries(LPCTSTR catalog_path);

	// 获取指定LSP数据
	BOOL GetPackedCatalogItem( LPCTSTR catalog_path, 
		LPCTSTR item_name, 
		PPACKED_CATALOG_ITEM buffer );

	// 获取LSP的dll完整路径
	CString GetPathForPackedItem(char* item_data);

	// 将clsid转换成CString
	CString GetClsidFromItem( PPACKED_CATALOG_ITEM item_data );

	// 是否已经正确的LSP
	BOOL IsValidLsp( LPCTSTR file_path, LPCTSTR clsid );

	// 备份LSP信息
	void BackupLspInfo(std::vector<PLSP_INFO>& lspinfo);

	// 备份LSP DLL文件
	void BackupDllFile(std::vector<PLSP_INFO>& lspinfo);

	// 拷贝LSP的DLL文件到指定的路径
	BOOL CopyLspDllFile(LPCTSTR src, LPCTSTR dst);

	// rundll32.exe /u /s
	BOOL DeinstRundll(LPCTSTR dll_path);

	// 删除LSP
	void DeinstallProvider(LPCTSTR clsid);

	// 导入注册表文件
	BOOL ImportRegFile(LPCTSTR regpath);

	// 还原保存的LSP DLL
	BOOL RestoreDllFile();

}


#endif