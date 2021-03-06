
// ClientDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientDlg 对话框



CClientDlg::CClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTMSG, m_MSGS);
	DDX_Control(pDX, IDC_EDITMSG, m_MSG);
	DDX_Control(pDX, IDC_Send, m_Send);
	DDX_Control(pDX, IDC_Connect, m_Connect);
	DDX_Control(pDX, IDC_Exit, m_Exit);
	DDX_Control(pDX, IDC_LISTLog, m_Log);
	DDX_Control(pDX, IDC_EDIT_ServerPort, m_ServerPort);
	//DDX_Control(pDX, IDC_IPADDRESS1, m_Server_Address);
	DDX_Control(pDX, IDC_EDIT_ServerIP, m_EditServerIp);
	DDX_Control(pDX, IDC_LIST_ClientIP, m_List_ClientIP);
	DDX_Control(pDX, IDC_LIST_ClientPort, m_ListClientPort);
	DDX_Control(pDX, IDC_LIST_HostName, m_ListName);
	DDX_Control(pDX, IDC_LIST_Host_IP, m_ListHostIp);
	DDX_Control(pDX, IDC_CHECK_SetTimerTask, m_SetTimer);
	DDX_Control(pDX, IDC_EDIT_Timer, m_EditTimer);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Send, &CClientDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_Connect, &CClientDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_Exit, &CClientDlg::OnBnClickedExit)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ButtenClose, &CClientDlg::OnBnClickedButtenclose)
	ON_BN_CLICKED(IDC_CHECK_SetTimerTask, &CClientDlg::OnBnClickedCheckSettimertask)
END_MESSAGE_MAP()


// CClientDlg 消息处理程序

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//初始化hostname and hostIP
	SetTimer(2, 100, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);

	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CClientDlg::OnBnClickedSend()
{
	// TODO: 在此添加控件通知处理程序代码

	if (m_clientSocket.m_bConnected)
	{   
		//CString str_Buffer;
		m_MSG.GetWindowText(m_clientSocket.m_StrBufferSend);
		m_clientSocket.m_nLength = m_clientSocket.m_StrBufferSend.GetLength();

		if (m_clientSocket.m_nLength==0)
		{
			MessageBox(CString("Please enter some data first!"));
			return;
		}
		char pchar[1024*4] = { 0 };
		WideCharToMultiByte(CP_ACP, 0, m_clientSocket.m_StrBufferSend.GetBuffer(m_clientSocket.m_nLength), -1, 
			                pchar, m_clientSocket.m_nLength*2, NULL, 0);

		memcpy(m_clientSocket.m_szBuffer, pchar, m_clientSocket.m_nLength*2);  //copy the data in pLPCWSTR to m_szBuffer

		CString str("Client would Send: ");
		m_Log.InsertString(0, str + m_clientSocket.m_StrBufferSend);  //
		
		CString str1;
		str1.Format(_T("%d"), m_clientSocket.m_nLength);
        m_Log.InsertString(0, str1);

		m_MSG.SetWindowText(CString(""));

		m_clientSocket.AsyncSelect(FD_WRITE);         //enable the send event
	}
	else     //If there is no connection    
	{
		MessageBox(CString("Please press the connect button and connect to the server first!"));
	}
}


void CClientDlg::OnBnClickedConnect()
{
	// TODO: 在此添加控件通知处理程序代码

	CString str_port;

	m_EditServerIp.GetWindowText(m_strServerAdr);
	m_ServerPort.GetWindowText(str_port);

	if ( m_strServerAdr.IsEmpty() || str_port.IsEmpty())
	{
		MessageBox(CString("Please enter Server IP address and Port first!"));
		return;
	}

	m_szPort = _ttoi(str_port);
	
	m_clientSocket.NewSocketAddrServer = m_strServerAdr;           //address of connected client
	m_clientSocket.NewSocketPortServer = m_szPort;                 //port of connected client

	CString str("Client would connect to: ");
	m_Log.InsertString(0, str + CString("IP: ") + m_clientSocket.NewSocketAddrServer +
		              CString("Port: ") + str_port);

	m_clientSocket.Close();                 //close first

	m_clientSocket.m_hSocket = INVALID_SOCKET;
	m_clientSocket.m_bConnected = FALSE;


	SetTimer(1, 1000, NULL);
	TryCount = 0;
}


void CClientDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
    //关闭Socket
	
	//关闭对话框
	EndDialog(0);
}


void CClientDlg::OnTimer(UINT_PTR nIDEvent)               //creating the socket and connecting 
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	switch (nIDEvent)
	{
	  case 1:     //Timer1
	  {
		  if (m_clientSocket.m_bConnected)
		  {
			  KillTimer(1);
			  return;
		  }

		  if (m_clientSocket.m_hSocket == INVALID_SOCKET)
		  {
			  BOOL bFlag = m_clientSocket.Create(0, SOCK_STREAM, FD_CONNECT, m_clientSocket.NewSocketAddrClient);
			  if (!bFlag)
			  {
				  MessageBox(CString("Socket Error!"));
				  m_clientSocket.Close();
				  PostQuitMessage(0);
				  return;
			  }
		  }

		  m_clientSocket.GetSockName(m_clientSocket.NewSocketAddrClient, m_clientSocket.NewSocketPortClient);

		  m_clientSocket.Connect(m_strServerAdr, m_szPort);      // first time for handshaking

		  TryCount++;

		  {
			  CString str("connecting to ");
			  //MessageBox(str);
			  CString str_port;
			  str_port.Format(_T("%d"), m_szPort);

			  m_Log.InsertString(0, str + CString("IP: ") + m_clientSocket.NewSocketAddrServer +
				  CString(" Port: ") + str_port);
		  }
		  if (TryCount >= 20 || m_clientSocket.m_bConnected)
		  {
			  KillTimer(1);
			  if (TryCount >= 20)
				  MessageBox(CString("Connect Failed!"));
			  return;
		  }
		  break;
	  }
	  case 2:     //Timer2
	  {
		  KillTimer(2);

		  CString strIPAddr;
		  char HostName[100];
		  gethostname(HostName, sizeof(HostName));// 获得本机主机名.

		  struct addrinfo *res;
		  struct addrinfo hints;
		  int ret;
		  struct sockaddr_in *addr;
		  memset(&hints, 0, sizeof(struct addrinfo));
		  hints.ai_family = AF_INET;
		  ret = getaddrinfo(HostName, NULL, &hints, &res);

		  char ipbuf[16];
		  addr = (struct sockaddr_in *)res->ai_addr;
		  strIPAddr += inet_ntop(AF_INET, &addr->sin_addr, ipbuf, 16);//将IP地址的二进制转换为十进制
		  freeaddrinfo(res);

		  m_ListName.InsertString(0, CString(HostName));
		  m_ListHostIp.InsertString(0, strIPAddr);

		  m_clientSocket.NewSocketAddrClient = strIPAddr;

		  AfxMessageBox(CString("Please set the port and IP address of connected server!"));

		  break;
	  }
	  case 3:
	  {
		  if (m_clientSocket.m_bConnected)
		  {
			  //CString str_Buffer;
			  m_MSG.GetWindowText(m_clientSocket.m_StrBufferSend);
			  m_clientSocket.m_nLength = m_clientSocket.m_StrBufferSend.GetLength();

			  if (m_clientSocket.m_nLength == 0)
			  {
				  KillTimer(3);
				  m_SetTimer.SetCheck(0);
				  m_EditTimer.Clear();
				  MessageBox(CString("Please enter some data first!"));
				  return;
			  }
			  char pchar[1024 * 4] = { 0 };
			  WideCharToMultiByte(CP_ACP, 0, m_clientSocket.m_StrBufferSend.GetBuffer(m_clientSocket.m_nLength), -1,
				  pchar, m_clientSocket.m_nLength * 2, NULL, 0);

			  memcpy(m_clientSocket.m_szBuffer, pchar, m_clientSocket.m_nLength * 2);  //copy the data in pLPCWSTR to m_szBuffer

			  CString str("Client would Send: ");
			  m_Log.InsertString(0, str + m_clientSocket.m_StrBufferSend);  //

			  CString str1;
			  str1.Format(_T("%d"), m_clientSocket.m_nLength);
			  m_Log.InsertString(0, str1);

			  m_clientSocket.AsyncSelect(FD_WRITE);         //enable the send event
		  }
		  else     //If there is no connection    
		  {
			  KillTimer(3);
			  m_SetTimer.SetCheck(0);
			  m_EditTimer.Clear();
			  MessageBox(CString("Please press the connect button and connect to the server first!"));
		  }
		  break;
	  }
	  default:
		   break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CClientDlg::OnBnClickedButtenclose()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(3);
	m_SetTimer.SetCheck(0);
	m_EditTimer.Clear();

	m_clientSocket.Close();
	m_List_ClientIP.DeleteString(0);
	m_ListClientPort.DeleteString(0);
	this->m_Connect.EnableWindow(TRUE);

	m_clientSocket.m_bConnected = FALSE;
}


void CClientDlg::OnBnClickedCheckSettimertask()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_SetTimer.GetCheck())
	{
		CString buffer_Edit;
		m_EditTimer.GetWindowText(buffer_Edit);

		if (buffer_Edit.IsEmpty())
		{
			MessageBox(CString("The timer editer is empty!"));
			m_SetTimer.SetCheck(0);
			return;
		}

		int time = _ttoi(buffer_Edit);

		SetTimer(3, time, 0);
	}
	else
	{
		KillTimer(3);
	}
}
