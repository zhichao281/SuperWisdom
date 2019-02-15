#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#include <atlmisc.h>


/*
 * 文件版本号的代码页结构体
 * code by Root
 * E-mail: cppcoffee@gmail.com
*/
struct LANGANDCODEPAGE
{
	WORD wLanguage;  
	WORD wCodePage;  
};


/*
 * LSP相关信息的结构体
 * code by Root
 * E-mail: cppcoffee@gmail.com
*/
typedef struct tagLSP_INFO
{
	CString dll_path;
	CString clsid;
	CString lsp_name;
	BOOL bIsValidLsp;
} LSP_INFO, *PLSP_INFO;

#endif