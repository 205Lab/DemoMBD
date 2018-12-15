#include "stdafx.h"
#include "NewSocket.h"
#include "Client.h"
#include "ClientDlg.h"

NewSocket::NewSocket()
{
	m_nLength = 0;
	m_bConnected = FALSE;
	memset(m_szBuffer, 0, sizeof(m_szBuffer));
}


NewSocket::~NewSocket()
{
	if (m_hSocket != INVALID_SOCKET)
		Close();
}


void NewSocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	CClientApp *pApp = (CClientApp *)AfxGetApp();
	CClientDlg *pDlg = (CClientDlg*)pApp->m_pMainWnd;
	CString str("Client is reveiving: ");
	CString str_server("Server is sending: ");

	m_nLength = Receive(m_szBuffer, sizeof(m_szBuffer), 0);

	pDlg->m_MSGS.InsertString(0, str_server+CString(m_szBuffer));
	pDlg->m_Log.InsertString(0,str+CString(m_szBuffer));
	memset(m_szBuffer, 0, sizeof(m_szBuffer));

	AsyncSelect(FD_READ | FD_CLOSE);
	CAsyncSocket::OnReceive(nErrorCode);
}


void NewSocket::OnConnect(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	CClientApp *pApp = (CClientApp *)AfxGetApp();
	CClientDlg *pDlg = (CClientDlg*)pApp->m_pMainWnd;

	if(0==nErrorCode)
	{
		m_bConnected = TRUE;

		CString str_port;
		pDlg->m_ServerPort.GetWindowText(str_port);

		pDlg->m_Log.InsertString(0, CString("Connected to ") + pDlg->m_strServerAdr + 
			                     CString(" Port: ") + str_port);

		pDlg->m_List_ClientIP.InsertString(0, NewSocketAddrClient);

		CString str_PORT;
		str_PORT.Format(_T("%d"), NewSocketPortClient);
		pDlg->m_ListClientPort.InsertString(0, str_PORT);

		pDlg->m_Connect.EnableWindow(FALSE);

		AsyncSelect(FD_READ | FD_CLOSE);
	}
	CAsyncSocket::OnConnect(nErrorCode);
}

/*
int SendTo(
	const void* lpBuf,
	int nBufLen,
	UINT nHostPort,
	LPCTSTR lpszHostAddress = NULL,
	int nFlags = 0);
*/
void NewSocket::OnSend(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	//
	CClientApp *pApp = (CClientApp *)AfxGetApp();
	CClientDlg *pDlg = (CClientDlg*)pApp->m_pMainWnd;//
	
	//log for tracing
	{
		CString str("Client is Sending: ");
		pDlg->m_Log.InsertString(0, str + m_StrBufferSend);

		CString str1;
		str1.Format(_T("%d"), m_nLength);
		pDlg->m_Log.InsertString(0, str1);
	}

	char buffer[4096] = { 0 };
	strcpy_s(buffer, m_szBuffer);
	SendTo(buffer, strlen(buffer), NewSocketPortServer, NewSocketAddrServer);//sending m_nLength*2 bytes of data

	m_nLength = 0;
	memset(m_szBuffer, 0, sizeof(m_szBuffer));
	m_StrBufferSend.Empty();

	AsyncSelect(FD_READ|FD_CLOSE);
	CAsyncSocket::OnSend(nErrorCode);
}


//void NewSocket::OnAccept(int nErrorCode)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//
//	CAsyncSocket::OnAccept(nErrorCode);
//}


void NewSocket::OnClose(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	//server can close the connection actively
	CClientApp *pApp = (CClientApp *)AfxGetApp();
	CClientDlg *pDlg = (CClientDlg*)pApp->m_pMainWnd;

	GetSockName(NewSocketAddrClient, NewSocketPortClient);

	CString str_PORT;
	str_PORT.Format(_T("%d"), NewSocketPortClient);

	Close();

	pDlg->m_Log.InsertString(0, CString("IP: ")+ NewSocketAddrClient + CString(" Port: ")  +
		                    str_PORT + CString(" is shut down! "));

	pDlg->m_List_ClientIP.DeleteString(0);
	pDlg->m_ListClientPort.DeleteString(0);
	AfxMessageBox(CString("connection is shut down!"), 1);

	pDlg->m_clientSocket.m_bConnected = FALSE;
	pDlg->m_Connect.EnableWindow(TRUE);

	CAsyncSocket::OnClose(nErrorCode);
}
