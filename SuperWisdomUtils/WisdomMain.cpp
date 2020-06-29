//#include "WisdomUtils.h"
//#include <Windows.h>
//#include <string>
//#include <iostream>
//
//using namespace std;
//using namespace WisdomUtils;
//
//
//void DefPrint( const char* pProcessName, const int nPid, char* pBuff )
//{
//	printf("[%d][%s]: %s\n", nPid, pProcessName, pBuff);
//}
//
//
//
//
//
//#include "../SuperWisdomUtils/SystemSettings/GetCpuUseage.h"
//
//void foo(int a,int b,int c=-1)
//{
//	std::cout<<"foo("<<a<<","<<b<<","<<c<<")"<<endl;
//}
//enum MSGBOX_TYPE {
//	// keep this in sync with QDialogButtonBox::StandardButton and QPlatformDialogHelper::StandardButton
//	MSGBOX_OK= 0x0001,
//	MSGBOX_CANCEL= 0x0002,
//	MSGBOX_YES= 0x0004,
//	MSGBOX_NO= 0x0008,
//	MSGBOX_DOWN= 0x00010,
//	MSGBOX_DOWN_parse=0x00020,
//	MSGBOX_DOWN_OTHER=0x00040,
//};
//void Init(int ntype)
//{
//	if(ntype & MSGBOX_OK)
//	{
//		std::cout<<"OK";
//	}
//	if (ntype & MSGBOX_NO )
//	{
//		std::cout<<"NO";
//	}
//	if (ntype & MSGBOX_DOWN )
//	{
//		std::cout<<"DOWN";
//	}
//	if (ntype & MSGBOX_DOWN_parse )
//	{
//		std::cout<<"MSGBOX_DOWN_parse";
//	}
//}
//BOOL FileTimeToLocalFileTime2(__in CONST FILETIME *lpFileTime, __out LPFILETIME lpLocalFileTime)
//{
//	if (NULL == lpFileTime || NULL == lpLocalFileTime)
//		return FALSE;
//
//
//	// 获取时区
//	time_t tt = 0;
//	struct tm *ptm = NULL;
//	tt = ::time(&tt);
//	ptm = ::localtime(&tt);
//	int nHourLocal = ptm->tm_hour;
//	ptm = ::gmtime(&tt);
//	int nHourUTC = ptm->tm_hour;
//	int nTimeZoom = nHourLocal - nHourUTC;
//
//
//	ULARGE_INTEGER uli = { lpFileTime->dwLowDateTime, lpFileTime->dwHighDateTime };
//	uli.QuadPart += ((ULONGLONG)nTimeZoom * 60 * 60 * 1000 * 1000 * 10);
//	lpLocalFileTime->dwLowDateTime = uli.LowPart;
//	lpLocalFileTime->dwHighDateTime = uli.HighPart;
//
//
//	return TRUE;
//}
//
//
//
//time_t FileTimeToTime_t(const FILETIME &ft)
//{
//	ULARGE_INTEGER ui;
//	ui.LowPart = ft.dwLowDateTime;
//	ui.HighPart = ft.dwHighDateTime;
//	return ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
//}
//
//int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2)
//{
//	FILETIME fTime1 = { 0, 0 };
//	FILETIME fTime2 = { 0, 0 };
//	SystemTimeToFileTime(&t1, &fTime1);
//	SystemTimeToFileTime(&t2, &fTime2);
//
//	time_t tt1 = FileTimeToTime_t(fTime1);
//	time_t tt2 = FileTimeToTime_t(fTime2);
//
//	return (int)(tt2 - tt1);
//}
//
//int GetDiffDays(const SYSTEMTIME &t1, const SYSTEMTIME &t2)
//{
//	int diffSeconds = GetDiffSeconds(t1, t2);
//	return diffSeconds / (24 * 3600);
//}
//
//
//
//BOOL FileAttributes(LPCSTR lpszFilePath/*文件路径*/)
//{
//	WIN32_FIND_DATAA FindFileData = { 0 };
//	HANDLE hFile = ::FindFirstFileA(lpszFilePath, &FindFileData);
//	if (INVALID_HANDLE_VALUE == hFile)
//	{	//handling error
//		return FALSE;
//	}
//	SYSTEMTIME CreateTime = { 0 }; //创建时间
//	SYSTEMTIME AccessTime = { 0 }; //最近访问时间
//	SYSTEMTIME WriteTime = { 0 }; //最近修改时间
//	FILETIME localft;
//	FILETIME lpFileTime;
//
//	FILETIME        ftLocalCreateTime, ftLocalWriteTime, ftLocalAccessTime;
//
//
//	 FileTimeToLocalFileTime2(&FindFileData.ftCreationTime, &ftLocalCreateTime);
//	 FileTimeToLocalFileTime2(&FindFileData.ftLastAccessTime, &ftLocalAccessTime);
//	 FileTimeToLocalFileTime2(&FindFileData.ftLastWriteTime, &ftLocalWriteTime);
//
//
//
//	if (!::FileTimeToSystemTime(&ftLocalCreateTime, &CreateTime))
//	{
//		//handling error
//	
//		return FALSE;
//	}
//	if (!::FileTimeToSystemTime(&ftLocalAccessTime, &AccessTime))
//	{
//		//handling error
//		return FALSE;
//	}
//	if (!::FileTimeToSystemTime(&ftLocalWriteTime, &WriteTime))
//	{
//		//handling error
//		return FALSE;
//	}
//	cout << "filename:" << lpszFilePath << endl;
//	cout << "创建时间:" << CreateTime.wYear << "-" << CreateTime.wMonth << "-" << CreateTime.wDay << "   " << CreateTime.wHour << ":" << CreateTime.wMinute << ":" << CreateTime.wSecond<<endl;
//	cout << "最后访问时间:" << AccessTime.wYear << "-" << AccessTime.wMonth << "-" << AccessTime.wDay << "   " << AccessTime.wHour << ":" << AccessTime.wMinute << ":" << AccessTime.wSecond << endl;
//	cout << "最近修改时间:" << WriteTime.wYear << "-" << WriteTime.wMonth << "-" << WriteTime.wDay << "   " << WriteTime.wHour << ":" << WriteTime.wMinute << ":" << WriteTime.wSecond << endl;
//	
//	SYSTEMTIME st = { 0 };
//	GetLocalTime(&st);
//	int  n = GetDiffSeconds(CreateTime, st);
//	
//	
//	//OK 获取时间了，可以使用时间了
//	::FindClose(hFile);
//	return TRUE;
//}
//
//#include "Download.h"
//#include ""
//
//int main()
//{	
//
//	http_downloadfile("http://s.funnycore.com/funnyCore_190116_2695.exe",
//		"d:\\趣核安装包\\", "", false,nullptr);
//	system("pause");
//	std::list<std::pair<std::string, bool>> filelist;
//
//	filelist= WisdomUtils::CFileManange::GetDirFiles("C:\\Users\\Administrator\\Desktop\\扫描件");
//	list<std::pair<std::string, bool>>::iterator itor;  //定义迭代器 
//	
//	for (itor = filelist.begin(); itor != filelist.end(); itor++)
//	{
//		std::pair<std::string, bool> strfile;
//		strfile=*itor;
//		std::string strname= strfile.first;
//		FileAttributes(strname.c_str());
//	 }
//
//
//
//
//
//	Init(MSGBOX_DOWN| MSGBOX_NO|MSGBOX_CANCEL);
//	
//	CGetWinInfo getCpuUseage;
//	int nCount=getCpuUseage.getWin_CoreCount();
//	bool bRes=getCpuUseage.Check_CpuAvg(5);
//	getCpuUseage.getWin_MaxCpuUseage();
//	while(true)
//	{
//		//cout<<"内存使用率"<<getCpuUseage.getWin_MemUsage()<<"%"<<endl;
//		//printf ("There are %f free  GB of physical memory.\n", getCpuUseage.getWin_FreeMemory());
//		double cpu =getCpuUseage.getWin_CpuUseage();
//	
//		cout<<"5096Cpu："<<getCpuUseage.getWin_CpuUseage(5008)<<endl;
//
//		Sleep(1000);
//	}
//
//	vector<string> vec;
//	vec.push_back("xpzs.exe");
//	WisdomUtils::CDebugView view(vec);
//	view.SetPrintFunction(DefPrint);
//	view.Start();
//	return 0;
//
//}
//
//void CmdOut(int nPid, int nExitCode, std::string& pBuff)
//{
//	printf("[pid:%d][%s]\n", nPid, pBuff.c_str());
//
//}
//
