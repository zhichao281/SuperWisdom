// +----------------------------------------------------------------------
// | C++ Basis Components
// +----------------------------------------------------------------------
// | GSLog		日志
// +----------------------------------------------------------------------
// | Author:	
// +----------------------------------------------------------------------
// | Modify:	
// +----------------------------------------------------------------------

#pragma  once

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <stdarg.h>


// 日志等级
typedef enum _NSLOG_LEVEL_
{
	LEVEL_ERROR		= 0,
	LEVEL_WARN		= 1,
	LEVEL_DEBUG		= 2,
	LEVEL_INFO		= 3,
	LEVEL_ALL		= 4,
	LEVEL_CLOSE		= 5
}NSLOG_LEVEL;

class NSLog
{

public:

	// 构造函数
	NSLog();

	// 析构函数
	virtual ~NSLog();

protected:
	//发布版本只记录ERROR等级
#ifdef _DEBUG
	// 初始化
	void _Init(int log_level_ = LEVEL_INFO);
#else
	// 初始化
	void _Init(int log_level_ = LEVEL_ERROR);
#endif // DEBUG

	

	// 解析路径
	void _ParsePath(char *pPath, char* pDir, char *pName);

	// 获取当前日期
	void _GetCurTime(char *pDate = NULL, char *pTime = NULL);

	// 检测日志文件时间
	bool _CheckFileTime();

	// 重新创建文件
	void _Rotate();

	// 创建文件夹
	int CreateDir(const char* pDir);
	// 配置文件
	void Config(const char* pConfigFile);
public:

	// 打开日志文件
	bool Open(char *pFilePath, NSLOG_LEVEL nLevel = LEVEL_DEBUG);

	// 写日志
	int Logv(NSLOG_LEVEL nLevel, const char *pFmt, va_list vaList);

	// 写错误日志
	int LogError(const char *pFmt, ...);

	// 写警告日志
	int LogWarn(const char *pFmt, ...);

	// 写调试日志
	int LogDebug(const char *pFmt, ...);

	// 写信息日志
	int LogInfo(const char *pFmt, ...);

	// 关闭
	void Close();

	static NSLog* GetLog();
private:

	NSLOG_LEVEL			m_nLevel;
	FILE*				m_pFile;
	CRITICAL_SECTION	m_Mutex;
	char				m_szLogPath[1024];
	char				m_szName[1024];
	char				m_szDir[1024];
	char				m_szCurDate[128];
};

void _log_init(const char *pFilePath, NSLOG_LEVEL nLevel = LEVEL_DEBUG);
int _log_write(NSLOG_LEVEL nLevel, const char *pFmt, ...);

#define NSLOG_INIT(pFilePath, nLevel) \
	_log_init(pFilePath, nLevel)


#define NSLOG_INFO(fmt, ...) _log_write(LEVEL_INFO, "%s(%d): " fmt, __FILE__, __LINE__, __VA_ARGS__)

#define NSLOG_ERROR(fmt, ...) _log_write(LEVEL_ERROR, "%s(%d): " fmt, __FILE__, __LINE__, __VA_ARGS__)

#define NSLOG_WARN(fmt, ...) _log_write(LEVEL_WARN, "%s(%d): " fmt, __FILE__, __LINE__, __VA_ARGS__)

#define NSLOG_DEBUG(fmt, ...) _log_write(LEVEL_DEBUG, "%s(%d): " fmt, __FILE__, __LINE__, __VA_ARGS__)





