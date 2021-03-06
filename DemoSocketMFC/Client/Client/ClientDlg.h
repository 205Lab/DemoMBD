
// ClientDlg.h: 头文件
//

#pragma once
#include "NewSocket.h"

// CClientDlg 对话框
class CClientDlg : public CDialogEx
{
// 构造
public:
	CClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_DIALOG };
#endif

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

	int TryCount;
	
	UINT m_szPort;
public:

	NewSocket m_clientSocket;

	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedExit();

	CListBox m_MSGS;
	CEdit m_MSG;
	CButton m_Send;
	CButton m_Connect;
	CButton m_Exit;
	CListBox m_Log;

	char m_szServerAdr[256];
	CString m_strServerAdr;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtenclose();

	CEdit m_ServerPort;
	// CIPAddressCtrl m_Server_Address;	// CIPAddressCtrl m_Server_Address;
	CEdit m_EditServerIp;
	CListBox m_List_ClientIP;
	CListBox m_ListClientPort;

	CListBox m_ListName;          //container of the Host name
	CListBox m_ListHostIp;        //container of the Host IP
	CButton m_SetTimer;
	CEdit m_EditTimer;
	afx_msg void OnBnClickedCheckSettimertask();
};
