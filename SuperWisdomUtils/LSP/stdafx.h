// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
// #define _WTL_NO_CSTRING // 禁用 WTL::CString
// #define _WTL_NO_WTYPES  // 禁用 WTL::CSize、CPoint、CRect


#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include <tchar.h>

#pragma comment(lib, "version.lib")

// TODO: 在此处引用程序需要的其他头文件
