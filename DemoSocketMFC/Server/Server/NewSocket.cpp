#include "stdafx.h"
#include "NewSocket.h"
#include "Server.h"
#include "ServerDlg.h"

CNewSocket::CNewSocket()
{
      
}


CNewSocket::~CNewSocket()
{

}


void CNewSocket::OnSend(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnSend(nErrorCode);
}


void CNewSocket::OnReceive(int nErrorCode)     //
{
	// TODO: 在此添加专用代码和/或调用基类
	CServerApp *pApp = (CServerApp *)AfxGetApp();        //
	CServerDlg *pDlg = (CServerDlg*)pApp->m_pMainWnd;    //??

	CString    NewSocketAddrLocal;
	UINT       NewSocketPortLocal;
	CNewSocket *pLocalSocket = nullptr;

	CString str_handle;
	SOCKET Localhandle = Detach();                                          //get the handle of sender
	//pApp->ClientPool.Lookup(Localhandle, pLocalSocket);

	m_nLength = Receive(m_szBuffer, sizeof(m_szBuffer), 0);
	

	str_handle.Format(_T("%d"), Localhandle);
	//AfxMessageBox(str_handle);
	
	#ifdef _DEBUG
		{  //for debuging
			GetSockName(NewSocketAddrLocal, NewSocketPortLocal);
			AfxMessageBox(NewSocketAddrLocal);

			CString str_NewSocketPortLocal;
			str_NewSocketPortLocal.Format(_T("%d"), NewSocketPortLocal);
			AfxMessageBox(str_NewSocketPortLocal);
		}
	#endif
	
	
	CString str_PORT;
	str_PORT.Format(_T("%d"), pLocalSocket->m_NewSocketPortClient);

	pDlg->m_ListTracing.InsertString(0, CString("IP: ") + pLocalSocket->m_NewSocketAddrClient + 
		                             CString(" Port: ") + str_PORT + CString(" handle: ") + str_handle + 
		                             CString(" is sending: "));

	pDlg->m_ListData.InsertString(0, CString("IP: ") + pLocalSocket->m_NewSocketAddrClient + 
		                         CString(" Port: ") + str_PORT + CString(" handle: ") + str_handle 
		                         + CString(" is sending: ") + CString(m_szBuffer));

	memset(m_szBuffer, 0, sizeof(m_szBuffer));

	AsyncSelect(FD_READ | FD_CLOSE);
	                             
	CAsyncSocket::OnReceive(nErrorCode);
	pLocalSocket = nullptr;
}


void CNewSocket::OnClose(int nErrorCode)    //
{
	// TODO: 在此添加专用代码和/或调用基类
	CServerApp *pApp = (CServerApp *)AfxGetApp();
	CServerDlg *pDlg = (CServerDlg*)pApp->m_pMainWnd;    //

	CString  NewSocketAddrLocal;
	UINT     NewSocketPortLocal;

	CString strClient("Client: ");
	CNewSocket *pLocalSocket = nullptr;

	SOCKET closed_client = Detach();                               //get the handle of sender
	//pApp->ClientPool.Lookup(closed_client, pLocalSocket);          //look up the socket in the Cmap

	//pLocalSocket->GetSockName(NewSocketAddrLocal, NewSocketPortLocal);     //not reliable

	CString str_port;
	str_port.Format(_T("%d"), pLocalSocket->m_NewSocketPortClient);

	CString str_handle;
	str_handle.Format(_T("%d"), closed_client);

	pDlg->m_ListTracing.InsertString(0, strClient + CString("IP: ") + 
		                             pLocalSocket->m_NewSocketAddrClient + CString(" Port: ") + str_port
		                            +CString(" handle: ") + str_handle + CString(" is closed!"));

	
	delete pLocalSocket;
	pLocalSocket = nullptr;

	//delete the handle and socket of client in server's map
	pApp->ClientPool.RemoveKey(closed_client);

	CAsyncSocket::OnClose(nErrorCode);
}
