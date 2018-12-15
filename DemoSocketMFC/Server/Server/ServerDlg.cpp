
// ServerDlg.cpp: 实现文件
//the time to update the combox: onAccept and onClose

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned int ThreadTimedSend(LPVOID lpParam);

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
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CServerDlg 对话框


IMPLEMENT_DYNAMIC(CServerDlg, CDialogEx);

CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = nullptr;
	m_pChosenSocketServer = nullptr;
	m_EnableTimedSend = FALSE;
	Flag_Timer2 = FALSE;
}

CServerDlg::~CServerDlg()
{
	// 如果该对话框有自动化代理，则
	//  此对话框的返回指针为 null，所以它知道
	//  此代理知道该对话框已被删除。
	if (m_pAutoProxy != nullptr)
		m_pAutoProxy->m_pDialog = nullptr;
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ButtonSend, m_SendData);
	DDX_Control(pDX, IDC_ButtonListen, m_ListenForConnecting);
	DDX_Control(pDX, IDC_EDIT_Editdata, m_EditData);
	DDX_Control(pDX, IDC_ListReData, m_ListData);
	//DDX_Control(pDX, IDC_ListClients, m_ListClients);
	DDX_Control(pDX, IDC_ListTracing, m_ListTracing);
	DDX_Control(pDX, IDC_LISTIP, m_ListIP);
	DDX_Control(pDX, IDC_LISTPORT, m_ListPort);
	DDX_Control(pDX, IDC_EDIT_ServerPort, m_SetServerPort);
	//DDX_Control(pDX, IDC_IPADDRESS1, m_SetIPAddre);
	DDX_Control(pDX, IDC_COMBO_ChoosingClient, m_Choose_Client);
	DDX_Control(pDX, IDC_EDIT_ServerIP, m_Edit_ServerIP);
	DDX_Control(pDX, IDC_CHECKEnableAll, m_EnableAllClients);
	DDX_Control(pDX, IDC_LIST_HostName, m_HostName);
	DDX_Control(pDX, IDC_LIST_HostIP, m_HostIP);
	DDX_Control(pDX, IDC_LIST_NumClients, m_NumClients);
	DDX_Control(pDX, IDC_LIST_ClientSocketHandle, m_ListClientSocketHandle);
	//  DDX_Check(pDX, IDC_CHECKEnableAll, m_EnableBroadCast);
	DDX_Control(pDX, IDC_LIST_ServerHandle, m_ServerHandle);
	DDX_Control(pDX, IDC_ButtonCloseConnection, m_ButtonCloseClientConnection);
	DDX_Control(pDX, IDC_EDIT1, m_EditTimer);
	DDX_Control(pDX, IDC_CHECK_Timer, m_SwithTimer);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ButtonSend, &CServerDlg::OnBnClickedButtonsend)
	ON_BN_CLICKED(IDC_ButtonListen, &CServerDlg::OnBnClickedButtonlisten)
	ON_BN_CLICKED(IDC_ButtonExit, &CServerDlg::OnBnClickedButtonexit)

    ON_BN_CLICKED(IDC_ButtonCloseConnection, &CServerDlg::OnBnClickedButtoncloseconnection)
    ON_BN_CLICKED(IDC_BUTTONCloseServer, &CServerDlg::OnBnClickedButtoncloseserver)

    ON_CBN_SELENDOK(IDC_COMBO_ChoosingClient, &CServerDlg::OnSelendokComboChoosingclient)
    ON_WM_TIMER()

	ON_BN_CLICKED(IDC_CHECKEnableAll, &CServerDlg::OnClickedCheckenableall)
	ON_BN_CLICKED(IDC_CHECK_Timer, &CServerDlg::OnClickedCheckTimer)
END_MESSAGE_MAP()


// CServerDlg 消息处理程序

BOOL CServerDlg::OnInitDialog()
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
	//m_EnableAllClients.SetCheck(0);
	m_SendData.EnableWindow(FALSE);
	m_ButtonCloseClientConnection.EnableWindow(FALSE);

	SetTimer(1,100,0);
	//SetTimer(2,50,0);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CServerDlg::OnPaint()
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 当用户关闭 UI 时，如果控制器仍保持着它的某个
//  对象，则自动化服务器不应退出。  这些
//  消息处理程序确保如下情形: 如果代理仍在使用，
//  则将隐藏 UI；但是在关闭对话框时，
//  对话框仍然会保留在那里。

void CServerDlg::OnClose()
{
	if (CanExit())
		CDialogEx::OnClose();
}

void CServerDlg::OnOK()
{
	if (CanExit())
		CDialogEx::OnOK();
}

void CServerDlg::OnCancel()
{
	if (CanExit())
		CDialogEx::OnCancel();
}

BOOL CServerDlg::CanExit()
{
	// 如果代理对象仍保留在那里，则自动化
	//  控制器仍会保持此应用程序。
	//  使对话框保留在那里，但将其 UI 隐藏起来。
	if (m_pAutoProxy != nullptr)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}


/*
Sending data to the chosen clients 
*/
void CServerDlg::OnBnClickedButtonsend()    
{
	// TODO: 在此添加控件通知处理程序代码
	CServerApp *pApp = (CServerApp *)AfxGetApp();

	CString buffer;
	m_EditData.GetWindowText(buffer);

	if (buffer.IsEmpty())
	{
		MessageBox(CString("The editer is empty!"));
		return;
	}
	//making sure there is a client at least, and choosing one of them
	if (!pApp->ClientPool.IsEmpty())
	{
		if (m_EnableAllClients.GetCheck())                              //broadcasting
		{
			CSocketServer *pSocketClient = nullptr;

			int index = m_Choose_Client.GetCurSel();                   // get the index of the selected item
			int nCount = m_Choose_Client.GetCount();
			if ((index == CB_ERR) && (nCount > 1))
			{
				m_Choose_Client.SetCurSel(0);
				index = m_Choose_Client.GetCurSel();
			}

			SOCKET Client_chosen = m_Choose_Client.GetItemData(index);
			theApp.ClientPool.Lookup(Client_chosen, pSocketClient);

			//AfxMessageBox(str_index + CString(" OK "));

			m_ListIP.ResetContent();
			m_ListIP.InsertString(0, pSocketClient->m_NewSocketAddrClient);

			m_ListPort.ResetContent();
			CString str_port;
			str_port.Format(_T("%d"), pSocketClient->m_NewSocketPortClient);
			m_ListPort.InsertString(0, str_port);

			m_ListClientSocketHandle.ResetContent();
			CString str_ClientSocketHandle;
			str_ClientSocketHandle.Format(_T("%d"), Client_chosen);
			m_ListClientSocketHandle.InsertString(0, str_ClientSocketHandle);

			m_pChosenSocketServer = pSocketClient;
			m_ChosenSocketServerHandle = Client_chosen;

			m_SendData.EnableWindow(TRUE);
			m_ButtonCloseClientConnection.EnableWindow(FALSE);
			//m_EnableAllClients.EnableWindow(FALSE);

			pSocketClient = nullptr;

			m_Choose_Client.EnableWindow(FALSE);
			
			m_pChosenSocketServer->AsyncSelect(FD_WRITE);
			return;
		}
		else if(!m_EnableAllClients.GetCheck() && m_pChosenSocketServer != nullptr)  //sending data 
		{
			//CString str_Buffer;
			m_EditData.GetWindowText(m_pChosenSocketServer->m_StrBufferSend);

			m_pChosenSocketServer->m_nLength = m_pChosenSocketServer->m_StrBufferSend.GetLength(); //the length of characters, not bytes

			if (m_pChosenSocketServer->m_nLength != 0)
			{
				char pchar[1024 * 4] = { 0 };
				WideCharToMultiByte(CP_ACP, 0,
					m_pChosenSocketServer->m_StrBufferSend.GetBuffer(m_pChosenSocketServer->m_nLength),
					-1, pchar, m_pChosenSocketServer->m_nLength * 2, NULL, 0);

				//copy the data in pLPCWSTR to m_szBuffer
				memcpy(m_pChosenSocketServer->m_szBuffer, pchar, m_pChosenSocketServer->m_nLength * 2);

				CString str("Client would Send: ");
				m_ListTracing.InsertString(0, str + m_pChosenSocketServer->m_StrBufferSend);  //

				CString str1;
				str1.Format(_T("%d"), m_pChosenSocketServer->m_nLength);
				m_ListTracing.InsertString(0, str1);

				m_pChosenSocketServer->AsyncSelect(FD_WRITE);
			}
		}
		else if (!m_EnableAllClients.GetCheck() && m_pChosenSocketServer == nullptr)
		{
			CSocketServer *pSocketClient = nullptr;

			int index = m_Choose_Client.GetCurSel();                   // get the index of the selected item
			int nCount = m_Choose_Client.GetCount();
			if ((index == CB_ERR) && (nCount > 1))
			{
			   m_Choose_Client.SetCurSel(0);
			   index = m_Choose_Client.GetCurSel();
			}

			SOCKET Client_chosen = m_Choose_Client.GetItemData(index);
			theApp.ClientPool.Lookup(Client_chosen, pSocketClient);

			//AfxMessageBox(str_index + CString(" OK "));

			m_ListIP.ResetContent();
			m_ListIP.InsertString(0, pSocketClient->m_NewSocketAddrClient);

			m_ListPort.ResetContent();
			CString str_port;
			str_port.Format(_T("%d"), pSocketClient->m_NewSocketPortClient);
			m_ListPort.InsertString(0, str_port);

			m_ListClientSocketHandle.ResetContent();
			CString str_ClientSocketHandle;
			str_ClientSocketHandle.Format(_T("%d"), Client_chosen);
			m_ListClientSocketHandle.InsertString(0, str_ClientSocketHandle);

			m_pChosenSocketServer = pSocketClient;
			m_ChosenSocketServerHandle = Client_chosen;

			m_SendData.EnableWindow(TRUE);
			m_ButtonCloseClientConnection.EnableWindow(TRUE);
			//m_EnableAllClients.EnableWindow(FALSE);

			pSocketClient = nullptr;
			m_pChosenSocketServer->AsyncSelect(FD_WRITE);
		}
	}
	else if( !m_SocketServer.m_bConnected )   //If the server is connected    
	{
		MessageBox(CString("Please make a server by editing the IP and port and pressing the listern button!"));
	}
	else 
	{
		MessageBox(CString("There is no client connected to server, please wait for some clients connection!"));

	}
}


void CServerDlg::OnBnClickedButtonlisten()
{
	// TODO: 在此添加控件通知处理程序代码
	CString GetPortdata;

	m_Edit_ServerIP.GetWindowText(m_SocketServer.m_StrAddressServerLocal);
	m_SetServerPort.GetWindowText(GetPortdata);

	if(m_SocketServer.m_StrAddressServerLocal.IsEmpty() || GetPortdata.IsEmpty())    //Empty EDit  
	{
		MessageBox(CString("Please set Server IP address and Port first!"));
		return;
	}

	m_SocketServer.m_ServerLocalPort = _ttoi(GetPortdata);

	if (m_SocketServer.m_hSocket == INVALID_SOCKET)
	{
		BOOL bFlag = m_SocketServer.Create(m_SocketServer.m_ServerLocalPort, 
			                              SOCK_STREAM, 
			                              FD_ACCEPT, 
			                              m_SocketServer.m_StrAddressServerLocal);
		if(!bFlag)
		{
			MessageBox(CString(" Listening ")+ CString(" IP: ") + m_SocketServer.m_StrAddressServerLocal 
				      + CString(" Port:") + GetPortdata +CString(" failed! "));
			m_SocketServer.Close();
			PostQuitMessage(0);
			return;
		}
	}
	if (!m_SocketServer.Listen())
	{
		int nErrorCode = m_SocketServer.GetLastError();

	    if (nErrorCode != WSAEWOULDBLOCK)
		{
			MessageBox(CString(" Listening ") + CString(" IP: ") + m_SocketServer.m_StrAddressServerLocal
				       + CString(" Port:") + GetPortdata + CString(" failed! "));
		   m_SocketServer.Close();
		   PostQuitMessage(0);
		   return;
		}
	}
	else
	{
		m_ListTracing.InsertString(0, CString(" Listening ") + CString(" IP: ") + 
			                       m_SocketServer.m_StrAddressServerLocal
			                       + CString(" Port:") + GetPortdata + CString(" OK! "));

		m_SocketServer.m_bConnected = TRUE;
		m_ListenForConnecting.EnableWindow(FALSE);
		m_SocketServer.GetSockName(m_SocketServer.m_StrAddressServerLocal, m_SocketServer.m_ServerLocalPort);

		CString  str_SocketServerHandle;
		str_SocketServerHandle.Format(_T("%d"), m_SocketServer.m_hSocket);
		m_ServerHandle.ResetContent();
		m_ServerHandle.AddString(str_SocketServerHandle);
			
	}

	m_SocketServer.AsyncSelect(FD_ACCEPT | FD_CLOSE | FD_READ );        //no writing
}


void CServerDlg::OnBnClickedButtonexit()
{
	
	//TODO: clear the data stored in ClientPool
	CServerApp *pApp = (CServerApp *)AfxGetApp();
	CServerDlg *pDlg = (CServerDlg*)pApp->m_pMainWnd;    //

	unsigned int CountClients = pApp->ClientPool.GetCount();
	POSITION Start_ClientPool = pApp->ClientPool.GetStartPosition();
	CString str1;

	if (CountClients != 0)      //there are connected clients
	{
		m_SocketServer.Close();
		m_ListTracing.InsertString(0, CString("Server is closing! "));

		SOCKET iKey_handle;
		CSocketServer *ptVal;
		while (Start_ClientPool != NULL)         // m_ListTracing.InsertString    
		{
			pApp->ClientPool.GetNextAssoc(Start_ClientPool, iKey_handle, ptVal);

			str1.Format(_T("%d"), ptVal->m_NewSocketPortClient);

			m_ListTracing.InsertString(0, CString("Deleting client: IP:") + 
				                      ptVal->m_NewSocketAddrClient + CString(" Port: ")
				                      + str1);  //tracing the deleted client

			delete ptVal;                        //release the heap memeory
			ptVal = nullptr;
		}

		theApp.ClientPool.RemoveAll();

		str1.Format(_T("%d"), pApp->ClientPool.GetCount());          //setting  pDlg->m_NumClients
		m_NumClients.DeleteString(0);
		m_NumClients.AddString(str1);
	}

	EndDialog(0);    //closing the dialog
}


void CServerDlg::OnBnClickedButtoncloseconnection()     
{
	// TODO: closing the chosen client then updating the related data structure and visual controls 
	CServerApp *pApp = (CServerApp *)AfxGetApp();
	CString strClient("Client: ");

	if (pApp->ClientPool.IsEmpty())
	{
		AfxMessageBox(CString("Empty server, and no client."));
		m_pChosenSocketServer = nullptr;
		return;
	}

	if (m_pChosenSocketServer==nullptr)
	{
		AfxMessageBox(CString("Please choose a client."));
		CSocketServer *pSocketClient = nullptr;

		int index = m_Choose_Client.GetCurSel();                   // get the index of the selected item

		SOCKET Client_chosen = m_Choose_Client.GetItemData(index);
		theApp.ClientPool.Lookup(Client_chosen, pSocketClient);

		//AfxMessageBox(str_index + CString(" OK "));

		m_ListIP.ResetContent();
		m_ListIP.InsertString(0, pSocketClient->m_NewSocketAddrClient);

		m_ListPort.ResetContent();
		CString str_port;
		str_port.Format(_T("%d"), pSocketClient->m_NewSocketPortClient);
		m_ListPort.InsertString(0, str_port);

		m_ListClientSocketHandle.ResetContent();
		CString str_ClientSocketHandle;
		str_ClientSocketHandle.Format(_T("%d"), Client_chosen);
		m_ListClientSocketHandle.InsertString(0, str_ClientSocketHandle);

		m_pChosenSocketServer = pSocketClient;
		m_ChosenSocketServerHandle = Client_chosen;

		m_SendData.EnableWindow(TRUE);
		m_ButtonCloseClientConnection.EnableWindow(TRUE);
		//m_EnableAllClients.EnableWindow(FALSE);

		pSocketClient = nullptr;
	}
	
	m_pChosenSocketServer->Close();   //closing the connection first


	CString str_port;
	str_port.Format(_T("%d"), m_pChosenSocketServer->m_NewSocketPortClient);

	CString str_handle;
	str_handle.Format(_T("%d"), m_ChosenSocketServerHandle);       //??

	m_ListTracing.InsertString(0, strClient + CString("IP: ") +
		                       m_pChosenSocketServer->m_NewSocketAddrClient 
		                       + CString(" Port: ") + str_port
		                       + CString(" handle: ") + str_handle + 
		                       CString(" is closed!"));

	//delete the handle and socket of client in server's map
	pApp->ClientPool.RemoveKey(m_ChosenSocketServerHandle);

	   //code block for updating the combox 
	   CString str1, str2;
	   SOCKET iKey_handle;
	   CSocketServer *ptVal;

	    m_Choose_Client.ResetContent();

		//clear the data stored in ClientPool
		unsigned int CountClients = pApp->ClientPool.GetCount();
		POSITION Start_ClientPool = pApp->ClientPool.GetStartPosition();

		for (int index = 0; Start_ClientPool != NULL; index++)                 // m_ListTracing.InsertString    
		{
			pApp->ClientPool.GetNextAssoc(Start_ClientPool, iKey_handle, ptVal);

			str1.Format(_T("%d"), ptVal->m_NewSocketPortClient);
			str2.Format(_T("%d"), iKey_handle);
			m_Choose_Client.InsertString(index, CString("IP: ") + ptVal->m_NewSocketAddrClient
				                       + CString(" Port: ") + str1 + CString(" handle: ") + str2);

			m_Choose_Client.SetItemData(index, iKey_handle);
		}

	m_ListIP.ResetContent();
	m_ListPort.ResetContent();
	m_ListClientSocketHandle.ResetContent();

	CString str_numclients;
	str_numclients.Format(_T("%d"), pApp->ClientPool.GetCount());
	m_NumClients.DeleteString(0);
	m_NumClients.AddString(str_numclients);

	delete m_pChosenSocketServer;
	m_pChosenSocketServer = nullptr;

}


void CServerDlg::OnBnClickedButtoncloseserver()
{
	// TODO: clearing the information of clients and close the server's Socket
	 CServerApp *pApp = (CServerApp *)AfxGetApp();
	 CString str1;

	 KillTimer(2);     //closing the timer for sending
	 m_SwithTimer.SetCheck(0);
	 m_EditTimer.Clear();

	if (!pApp->ClientPool.IsEmpty())            //more than one client is connected 
	{
		//clear the data stored in ClientPool
		unsigned int CountClients = pApp->ClientPool.GetCount();
		POSITION Start_ClientPool = pApp->ClientPool.GetStartPosition();

		SOCKET iKey_handle;
		CSocketServer *ptVal;

		while (Start_ClientPool != NULL)         // m_ListTracing.InsertString    
		{
			pApp->ClientPool.GetNextAssoc(Start_ClientPool, iKey_handle, ptVal);

			str1.Format(_T("%d"), ptVal->m_NewSocketPortClient);
			m_ListTracing.InsertString(0, CString("Deleting client: IP:") + 
				                       ptVal->m_NewSocketAddrClient + CString(" Port: ")
			                           + str1);  //tracing the deleted client
			ptVal->SendTo("Server is closed...", strlen("Server is closed..."),
				         ptVal->m_NewSocketPortClient, ptVal->m_NewSocketAddrClient);
			
			delete ptVal;                        //release the heap memeory
			ptVal = nullptr;
		}

		pApp->ClientPool.RemoveAll();

		if (!pApp->ClientPool.GetHashTableSize())
		{
			pApp->ClientPool.InitHashTable(100);
		}

		
		str1.Format(_T("%d"), pApp->ClientPool.GetCount());      //setting  pDlg->m_NumClients
		m_NumClients.DeleteString(0);
		m_NumClients.AddString(str1);

		m_SocketServer.m_bConnected = FALSE;

		m_SocketServer.Close();
		
		//block for updating the cmobox
		CString str1, str2;
		m_Choose_Client.ResetContent();

		//clear the data stored in ClientPool
	
		//updating
		m_ListIP.ResetContent();
		m_ListPort.ResetContent();
		m_ListClientSocketHandle.ResetContent();

		m_ListenForConnecting.EnableWindow(TRUE);
		m_ListTracing.ResetContent();
		m_ListData.ResetContent();

		
	}
	else
	{
		m_SocketServer.Close();
		MessageBox(CString("Empty server, and no connected client."));
		m_ListIP.ResetContent();
		m_ListPort.ResetContent();
		m_ListClientSocketHandle.ResetContent();

		m_ListenForConnecting.EnableWindow(TRUE);
		m_ListTracing.ResetContent();
		m_ListData.ResetContent();
	}
	
	//m_SocketServer.AsyncSelect(FD_CLOSE);
}





void CServerDlg::OnSelendokComboChoosingclient()              //choosing the client
{
	// TODO: 

	CSocketServer *pSocketClient = nullptr;

	int index = m_Choose_Client.GetCurSel();                   // get the index of the selected item

	SOCKET Client_chosen = m_Choose_Client.GetItemData(index);
	theApp.ClientPool.Lookup(Client_chosen, pSocketClient);

	//AfxMessageBox(str_index + CString(" OK "));

	m_ListIP.ResetContent();
	m_ListIP.InsertString(0, pSocketClient->m_NewSocketAddrClient);

	m_ListPort.ResetContent();
	CString str_port;
	str_port.Format(_T("%d"), pSocketClient->m_NewSocketPortClient);
	m_ListPort.InsertString(0, str_port);

	m_ListClientSocketHandle.ResetContent();
	CString str_ClientSocketHandle;
	str_ClientSocketHandle.Format(_T("%d"), Client_chosen);
	m_ListClientSocketHandle.InsertString(0, str_ClientSocketHandle);

	m_pChosenSocketServer = pSocketClient;
	m_ChosenSocketServerHandle = Client_chosen;

	m_SendData.EnableWindow(TRUE);
	m_ButtonCloseClientConnection.EnableWindow(TRUE);
	//m_EnableAllClients.EnableWindow(FALSE);

	pSocketClient = nullptr;

}

//2018/12/11: adding a user-thread to process the periodic sending
void CServerDlg::OnTimer(UINT_PTR nIDEvent)        //2018/12/03
{
	// TODO: 
	switch (nIDEvent)
	{
	   case 1:
	   {
			KillTimer(1);
			CString strIPAddr;
			char HostName[100];
			gethostname(HostName, sizeof(HostName));   // 获得本机主机名.

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

			m_HostName.InsertString(0, CString(HostName));
			m_HostIP.InsertString(0, strIPAddr);

			m_Edit_ServerIP.SetWindowTextW(strIPAddr);

			AfxMessageBox(CString("Please set the port of server!"));
		   break;
	   }
	   case 2:     //Timer 2 for sending data pieriodlly
	   {
		   m_EnableTimedSend = TRUE;
		   break;
	   }
	   case 3:
	   {
		   break;
	   }
	   default:
		   break;
	}
	CDialogEx::OnTimer(nIDEvent);
}



void CServerDlg::OnClickedCheckenableall()
{
	// TODO:   
	if (m_EnableAllClients.GetCheck())   //
	{
		m_ButtonCloseClientConnection.EnableWindow(FALSE);
		m_Choose_Client.EnableWindow(FALSE);
		m_SendData.EnableWindow(TRUE);
		//m_pChosenSocketServer = nullptr;
    }
	else
	{
		m_ButtonCloseClientConnection.EnableWindow(	TRUE);
		m_Choose_Client.EnableWindow(TRUE);
	}
}

//Setting up te timer and starting the worker thread to share the resource
void CServerDlg::OnClickedCheckTimer()
{
	// TODO:starting the timed task
	
	if (m_SwithTimer.GetCheck()) 
	{
		CString buffer_Edit;
		m_EditTimer.GetWindowText(buffer_Edit);

		if (buffer_Edit.IsEmpty())
		{
			MessageBox(CString("The timer editer is empty!"));
			m_SwithTimer.SetCheck(0);
			return;
		}

		int time = _ttoi(buffer_Edit);

		SetTimer(2, time, 0);
		Flag_Timer2 = TRUE;
		m_EnableTimedSend = TRUE;

		pThread = AfxBeginThread(ThreadTimedSend, &m_EnableTimedSend);
	}
	else
	{
		m_EnableTimedSend = FALSE;
		KillTimer(2);
		Flag_Timer2 = FALSE;
		pThread = nullptr;
	}
}


