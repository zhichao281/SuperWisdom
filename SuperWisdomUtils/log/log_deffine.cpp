#define _CRT_SECURE_NO_WARNINGS
#include "log_deffine.h"
#include "Logger.h"
#include "LogManager.h"


extern std::unique_ptr<Logger> g_logPtr;

void  _log_write(EnumLogLevel nLevel, const char *pFmt, ...)
{


	char pTemp[2048 * 100] = { 0 };
	va_list arg_ptr;
	va_start(arg_ptr, pFmt);
	vsprintf(pTemp + strlen(pTemp), pFmt, arg_ptr);
	va_end(arg_ptr);
	
	Logger::GetInstance()->Write2Caching(nLevel, pTemp);
	
}
void  _log_level(int nLevel)
{
	Logger::GetInstance()->SetLogLevel(nLevel);

}

void  _log_filename(const char *pfilename)
{
	Logger::GetInstance()->SetLogFileName(pfilename);
}