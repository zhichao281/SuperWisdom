
// iFengUIDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CiFengUIDlg 对话框
class CiFengUIDlg : public CDialogEx
{
// 构造
public:
	CiFengUIDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_IFENGUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	static void OnConnectStaticFunc(HANDLE hDevice,void* pUserData);
	static void OnDisConnectStaticFunc(HANDLE hDevice,void* pUserData);

	void OnConnect(HANDLE hDevice);
	void OnDisconnect(HANDLE hDevice);
	CStatic m_lbInfo;
};
