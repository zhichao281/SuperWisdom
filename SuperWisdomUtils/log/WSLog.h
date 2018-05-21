/**  * @copyright (C) 2016 ChinaNetCenter Inc. All rights reserved.
*
* @file paintWidget.h
* @summary  WSLOG	日志
*
* @version 1.0
* @author zhengzc@wangsu.com
* @date 2018-03-05 14:25:06
*
* @others
* @MODIFY    VERSION  YYYYMMDD    TITLE
* zhengzc      1.0    20180320     title
*
*/


#pragma  once


#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <stdarg.h>


// 日志等级
typedef enum class _WSLOG_LEVEL_
{

	LEVEL_ALL		= 0,
	LEVEL_DEBUG		= 1,
	LEVEL_INFO	    = 2,
	LEVEL_WARN		= 3,
	LEVEL_ERROR		= 4
}WSLOG_LEVEL;

//void _log_init(const char *pFilePath, WSLOG_LEVEL nLevel = _WSLOG_LEVEL_::LEVEL_DEBUG);
void  _log_write(WSLOG_LEVEL nLevel, const char *pFmt, ...);



#define WSLOG_INFO(fmt, ...) _log_write(_WSLOG_LEVEL_::LEVEL_INFO, "[%s<%d>]: " fmt, __FILE__, __LINE__, __VA_ARGS__)

#define WSLOG_ERROR(fmt, ...) _log_write(_WSLOG_LEVEL_::LEVEL_ERROR, "[%s<%d>]: " fmt, __FILE__, __LINE__, __VA_ARGS__)

#define WSLOG_WARN(fmt, ...) _log_write(_WSLOG_LEVEL_::LEVEL_WARN, "[%s<%d>]: " fmt, __FILE__, __LINE__, __VA_ARGS__)

#define WSLOG_DEBUG(fmt, ...) _log_write(_WSLOG_LEVEL_::LEVEL_DEBUG, "[%s<%d>]: " fmt, __FILE__, __LINE__, __VA_ARGS__)

