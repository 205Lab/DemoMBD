
// Server.cpp: 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include <afxsock.h>       //service for socket 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerApp

BEGIN_MESSAGE_MAP(CServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CServerApp 构造

CServerApp::CServerApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CServerApp 对象

CServerApp theApp;

const GUID CDECL BASED_CODE _tlid = {0x2df69eb6,0xcf60,0x49d5,{0x88,0xa4,0x8f,0x27,0x8c,0x29,0x7d,0x96}};
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


// CServerApp 初始化

BOOL CServerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(1, 1), &wsaData);
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	// 分析自动化开关或注册/注销开关的命令行。
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 应用程序是用 /Embedding 或 /Automation 开关启动的。
	//使应用程序作为自动化服务器运行。
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// 通过 CoRegisterClassObject() 注册类工厂。
		COleTemplateServer::RegisterAll();
	}

	// 应用程序是用 /Unregserver 或 /Unregister 开关启动的。  移除
	// 注册表中的项。
	else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		COleObjectFactory::UpdateRegistryAll(FALSE);
		AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor);
		return FALSE;
	}

	// 应用程序是以独立方式或用其他开关(如 /Register
	// 或 /Regserver)启动的。  更新注册表项，包括类型库。
	else
	{
		COleObjectFactory::UpdateRegistryAll();
		AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid);
		if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister)
			return FALSE;
	}

	CServerDlg dlg;
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

    //添加用户初始化
	ClientPool.InitHashTable(100);
	
	CountClientSocket = 0;
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}

int CServerApp::ExitInstance()
{
	AfxOleTerm(FALSE);

	//delete [] pNewSocketClient;
	WSACleanup();
	return CWinApp::ExitInstance();
}

//This is a thread of reading and displaying the data contained in the pApp->ClientPool. 
//Mutex is needed to synchronize with other threads.
unsigned int ThreadTimedSend(LPVOID lpParam)
{
	// TODO: 
	CServerApp *pApp = (CServerApp *)AfxGetApp();
	CServerDlg *pDlg = (CServerDlg*)pApp->m_pMainWnd;

	CString buffer;

	BOOL* pBOOL = (BOOL*)lpParam;

	pDlg->m_ListTracing.InsertString(0, CString("New thread of sending is beginning!"));

	while (1)
	{
		if (*pBOOL)                 //starting the sending
		{
			*pBOOL = FALSE;

			pDlg->m_EditData.GetWindowText(buffer);

			if (buffer.IsEmpty())
			{
				pDlg->KillTimer(2);
				AfxMessageBox(CString("The editer is empty, please enter some data and try again!"));
				pDlg->m_SwithTimer.SetCheck(0);
				pDlg->m_EditTimer.Clear();
				return 0;
			}

			pDlg->m_CriticalSection.Lock();

			//making sure there is a client at least, and choosing one of them
			if (!pApp->ClientPool.IsEmpty())
			{
				if (pDlg->m_EnableAllClients.GetCheck())                              //broadcasting
				{
					CSocketServer *pSocketClient = nullptr;

					int index = pDlg->m_Choose_Client.GetCurSel();                   // get the index of the selected item
					int nCount = pDlg->m_Choose_Client.GetCount();

					if ((index == CB_ERR) && (nCount > 1))
					{
						pDlg->m_Choose_Client.SetCurSel(0);
						index = pDlg->m_Choose_Client.GetCurSel();
					}

					SOCKET Client_chosen = pDlg->m_Choose_Client.GetItemData(index);
					theApp.ClientPool.Lookup(Client_chosen, pSocketClient);

					//AfxMessageBox(str_index + CString(" OK "));

					pDlg->m_ListIP.ResetContent();
					pDlg->m_ListIP.InsertString(0, pSocketClient->m_NewSocketAddrClient);

					pDlg->m_ListPort.ResetContent();
					CString str_port;
					str_port.Format(_T("%d"), pSocketClient->m_NewSocketPortClient);
					pDlg->m_ListPort.InsertString(0, str_port);

					pDlg->m_ListClientSocketHandle.ResetContent();
					CString str_ClientSocketHandle;
					str_ClientSocketHandle.Format(_T("%d"), Client_chosen);
					pDlg->m_ListClientSocketHandle.InsertString(0, str_ClientSocketHandle);

					pDlg->m_pChosenSocketServer = pSocketClient;
					pDlg->m_ChosenSocketServerHandle = Client_chosen;

					pDlg->m_SendData.EnableWindow(TRUE);
					pDlg->m_ButtonCloseClientConnection.EnableWindow(FALSE);
					//m_EnableAllClients.EnableWindow(FALSE);

					pSocketClient = nullptr;

					pDlg->m_Choose_Client.EnableWindow(FALSE);

					if (pDlg->m_EnableAllClients.GetCheck())    //broadcasting the data to all of the clients and traversing the Cmap
					{
						//clear the data stored in ClientPool

						unsigned int CountClients = pApp->ClientPool.GetCount();
						POSITION Start_ClientPool = pApp->ClientPool.GetStartPosition();

						SOCKET iKey_handle;
						CSocketServer *ptVal;

						char buffer[4096] = { 0 };
						CString str_buffer;
						pDlg->m_EditData.GetWindowText(str_buffer);
						int m_nLength = str_buffer.GetLength(); //the length of characters, not bytes

						char pchar[1024 * 4] = { 0 };
						WideCharToMultiByte(CP_ACP, 0, str_buffer.GetBuffer(m_nLength), 
							               -1, buffer, m_nLength * 2, NULL, 0);

						for (int index = 0; Start_ClientPool != NULL; index++)               // m_ListTracing.InsertString    
						{
							pApp->ClientPool.GetNextAssoc(Start_ClientPool, iKey_handle, ptVal);
							int result_send = ptVal->SendTo(buffer, strlen(buffer),
								                            ptVal->m_NewSocketPortClient,
								                            ptVal->m_NewSocketAddrClient);

							//dealing with some error which would happen when server is invovled in the operation of SendTo
							if (result_send == WSAENOTCONN)  //The socket is not connected, maybe it had been closed by the client
							{
								pDlg->m_ListTracing.InsertString(0, CString("send error! The socket is not connected, maybe it had been closed by the client."));
								delete ptVal;
							}
						}

						m_nLength = 0;
						memset(pDlg->m_pChosenSocketServer->m_szBuffer, 0, sizeof(pDlg->m_pChosenSocketServer->m_szBuffer));
						pDlg->m_pChosenSocketServer->m_StrBufferSend.Empty();

					}
					
				}
				else if (!pDlg->m_EnableAllClients.GetCheck() && pDlg->m_pChosenSocketServer != nullptr)    //sending data to one client 
				{
					
					pDlg->m_EditData.GetWindowText(pDlg->m_pChosenSocketServer->m_StrBufferSend);

					pDlg->m_pChosenSocketServer->m_nLength = pDlg->m_pChosenSocketServer->m_StrBufferSend.GetLength(); //the length of characters, not bytes

					if (pDlg->m_pChosenSocketServer->m_nLength != 0)
					{
						char pchar[1024 * 4] = { 0 };
						WideCharToMultiByte(CP_ACP, 0,
							pDlg->m_pChosenSocketServer->m_StrBufferSend.GetBuffer(pDlg->m_pChosenSocketServer->m_nLength),
							-1, pchar, pDlg->m_pChosenSocketServer->m_nLength * 2, NULL, 0);

						//copy the data in pLPCWSTR to m_szBuffer
						memcpy(pDlg->m_pChosenSocketServer->m_szBuffer, pchar, pDlg->m_pChosenSocketServer->m_nLength * 2);

						CString str("Client would Send: ");
						pDlg->m_ListTracing.InsertString(0, str + pDlg->m_pChosenSocketServer->m_StrBufferSend);  //

						CString str1;
						str1.Format(_T("%d"), pDlg->m_pChosenSocketServer->m_nLength);
						pDlg->m_ListTracing.InsertString(0, str1);

						//pDlg->m_pChosenSocketServer->AsyncSelect(FD_WRITE);
						if (nullptr != pDlg->m_pChosenSocketServer)
						{
							char buffer[4096] = { 0 };
							strcpy_s(buffer, pDlg->m_pChosenSocketServer->m_szBuffer);
							pDlg->m_pChosenSocketServer->SendTo(buffer,
								                               strlen(buffer),
								                               pDlg->m_pChosenSocketServer->m_NewSocketPortClient,
								                               pDlg->m_pChosenSocketServer->m_NewSocketAddrClient);
						}
					}
				}
				else if (!pDlg->m_EnableAllClients.GetCheck() && pDlg->m_pChosenSocketServer == nullptr)
				{
					CSocketServer *pSocketClient = nullptr;

					int index = pDlg->m_Choose_Client.GetCurSel();                   // get the index of the selected item
					int nCount = pDlg->m_Choose_Client.GetCount();

					if ((index == CB_ERR) && (nCount > 1))
					{
						pDlg->m_Choose_Client.SetCurSel(0);
						index = pDlg->m_Choose_Client.GetCurSel();
					}

					SOCKET Client_chosen = pDlg->m_Choose_Client.GetItemData(index);
					theApp.ClientPool.Lookup(Client_chosen, pSocketClient);

					//AfxMessageBox(str_index + CString(" OK "));

					pDlg->m_ListIP.ResetContent();
					pDlg->m_ListIP.InsertString(0, pSocketClient->m_NewSocketAddrClient);

					pDlg->m_ListPort.ResetContent();
					CString str_port;
					str_port.Format(_T("%d"), pSocketClient->m_NewSocketPortClient);
					pDlg->m_ListPort.InsertString(0, str_port);

					pDlg->m_ListClientSocketHandle.ResetContent();
					CString str_ClientSocketHandle;
					str_ClientSocketHandle.Format(_T("%d"), Client_chosen);
					pDlg->m_ListClientSocketHandle.InsertString(0, str_ClientSocketHandle);

					pDlg->m_pChosenSocketServer = pSocketClient;
					pDlg->m_ChosenSocketServerHandle = Client_chosen;

					pDlg->m_SendData.EnableWindow(TRUE);
					pDlg->m_ButtonCloseClientConnection.EnableWindow(TRUE);
					//m_EnableAllClients.EnableWindow(FALSE);

					pSocketClient = nullptr;
					pDlg->m_pChosenSocketServer->AsyncSelect(FD_WRITE);
				}
			}
			else if (!pDlg->m_SocketServer.m_bConnected)   //If the server is connected    
			{
				AfxMessageBox(CString("Please make a server by editing the IP and port and pressing the listern button!"));
			}
			else
			{
				pDlg->KillTimer(2);
				pDlg->m_SwithTimer.SetCheck(0);
				pDlg->m_EditTimer.Clear();
				AfxMessageBox(CString("There is no client connected to server, please wait for some clients connection!"));
			}

			pDlg->m_CriticalSection.Unlock();
		}

		if (pDlg->Flag_Timer2 == FALSE)
		{
			return 0;
		}
	}

	return 0;
}