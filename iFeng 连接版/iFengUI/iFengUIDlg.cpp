#include "stdafx.h"
#include "iFengUI.h"
#include "iFengUIDlg.h"
#include "afxdialogex.h"

#include "../iFeng/iFeng.h"
#pragma comment(lib, "../Debug/iFeng.lib")





#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()




CiFengUIDlg::CiFengUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CiFengUIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CiFengUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_INFO, m_lbInfo);
}

BEGIN_MESSAGE_MAP(CiFengUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

BOOL CiFengUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	UINT uret = 0;
	BOOL bret = FALSE;
	uret = IOSInitialize();
	bret = IOSRegisterOnConnectListener(OnConnectStaticFunc,this);
	bret = IOSRegisterOnDisConnectListener(OnDisConnectStaticFunc,this);
	bret = IOSStartListen();
	return TRUE;
}

void CiFengUIDlg::OnConnectStaticFunc(HANDLE hDevice,void* pUserData)
{
	((CiFengUIDlg*)pUserData)->OnConnect(hDevice);
}
void CiFengUIDlg::OnDisConnectStaticFunc(HANDLE hDevice,void* pUserData)
{
	((CiFengUIDlg*)pUserData)->OnDisconnect(hDevice);
}

void CiFengUIDlg::OnConnect(HANDLE hDevice)
{
	IOSDeviceOpen(hDevice);
	DeviceProperty info;
	IOSGetDeviceInformation(hDevice,&info);
	WCHAR wcText[MAX_PATH];
	wsprintf(wcText,
		L"IdentifyNumber:%s\n\nName:%s\n\nProductType:%s\n\nSerialNumber:%s",
		info.IdentifyNumber,
		info.Name,
		info.ProductType,
		info.SerialNumber);
	m_lbInfo.SetWindowText(wcText);
}
void CiFengUIDlg::OnDisconnect(HANDLE hDevice)
{

}

void CiFengUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CiFengUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CiFengUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

