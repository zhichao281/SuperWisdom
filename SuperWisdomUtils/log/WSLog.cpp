// +----------------------------------------------------------------------
// | Green C++ Basis Components
// +----------------------------------------------------------------------
// | WSLOG		日志
// +----------------------------------------------------------------------
// | Author:	Maga
// +----------------------------------------------------------------------
// | Modify:	Maga
// +----------------------------------------------------------------------


#include "WSLOG.h"
#include "log_deffine.h"
#include "Logger.h"
#include "LogManager.h"
extern std::unique_ptr<Logger> g_logPtr;
/*
 *	Function:		_log_write
 *	Explanation:	写日志
 *	Input:			nLevel			日志等级
					pFmt			格式化字符串
 *	Return:			实际写长度
 */
void _log_write(WSLOG_LEVEL nLevel, const char *pFmt, ...)
{
	
	if (nLevel == WSLOG_LEVEL::LEVEL_INFO)
	{
		LOG_INFO(pFmt,...);
	}

	return ;
}


