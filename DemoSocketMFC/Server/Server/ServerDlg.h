
// ServerDlg.h: 头文件
//

#pragma once
#include "CSocketServer.h"
#include "afxmt.h"


class CServerDlgAutoProxy;


// CServerDlg 对话框
class CServerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CServerDlg);
	friend class CServerDlgAutoProxy;

// 构造
public:
	CServerDlg(CWnd* pParent = nullptr);	// 标准构造函数
	virtual ~CServerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	CServerDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnBnClickedButtonsend();
	afx_msg void OnBnClickedButtonlisten();
	afx_msg void OnBnClickedButtonexit();
	afx_msg void OnBnClickedButtoncloseconnection();
	afx_msg void OnBnClickedButtoncloseserver();
	afx_msg void OnSelendokComboChoosingclient();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
    
	CButton			m_SendData;
	CButton			m_ListenForConnecting;
	CEdit			m_EditData;
	CListBox		m_ListData;
	CListBox		m_ListClients;
	CListBox		m_ListTracing;
	CListBox		m_ListIP;
	CListBox		m_ListPort;
	CSocketServer	m_SocketServer;
	CEdit			m_SetServerPort;
	CIPAddressCtrl	m_SetIPAddre;
	CComboBox		m_Choose_Client;
	CEdit			m_Edit_ServerIP;
	CButton			m_EnableAllClients;
	CListBox		m_HostName;
	CListBox		m_HostIP;
	CListBox		m_NumClients;
	CListBox		m_ListClientSocketHandle;
//	BOOL            m_EnableBroadCast;

	CSocketServer * m_pChosenSocketServer;
	SOCKET          m_ChosenSocketServerHandle;
	CWinThread*     pThread;
	
	CListBox m_ServerHandle;
	afx_msg void OnClickedCheckenableall();
	CButton m_ButtonCloseClientConnection;
	CEdit m_EditTimer;
	CButton m_SwithTimer;
	afx_msg void OnClickedCheckTimer();

	BOOL m_EnableTimedSend;
	BOOL Flag_Timer2;
	CCriticalSection m_CriticalSection;
};
