/*

 The framework calls these notification callback functions to notify you of important socket events,
 such as the receipt of data that you can begin reading, the operation of writing that you can perform.

 debug for:
 1,Synchronizing the Threads of writing data and reading data
 2,Reading and maintaining:


 */

#include "stdafx.h"
#include "ServerDlg.h"
#include "Server.h"
#include "CSocketServer.h"
#include "NewSocket.h"    //for serving the connection of client 

CSocketServer::CSocketServer()
{

}


CSocketServer::~CSocketServer()
{
   	
}

//
void CSocketServer::OnReceive(int nErrorCode)    
{
	// TODO: 在此添加专用代码和/或调用基类
	CServerApp *pApp = (CServerApp *)AfxGetApp();        //
	CServerDlg *pDlg = (CServerDlg*)pApp->m_pMainWnd;    //

	CSocketServer  *pLocalSocket = nullptr;
	char Rebuffer[4096] = {0};
	
	//get the handle of client
	SOCKET Clienthandle = Detach();
	
	//get the pointer via handle in Cmap
	if (pApp->ClientPool.Lookup(Clienthandle, pLocalSocket))
	{
		//::AfxMessageBox(CString("had find"));
	}

	pLocalSocket->Attach(Clienthandle, FD_READ);

	m_nLength = pLocalSocket->Receive(Rebuffer, sizeof(Rebuffer));
	
	switch (m_nLength)
	{
	  case 0:
	  {
		  //OnClose(0);       //server would close this connection!
		  break;
	  }
	  case SOCKET_ERROR:
	  {
		  if (GetLastError() != WSAEWOULDBLOCK)
		  {
			  AfxMessageBox(_T("Error occurred when client sent data to server   \
				              and server would close this connection!"));
			  //OnClose(0);        //server would close this connection!
		  }
		  break;
	  }
	}
	
	
	CString str_handle;
	str_handle.Format(_T("%d"), Clienthandle);

	CString str_PORT;
	str_PORT.Format(_T("%d"), pLocalSocket->m_NewSocketPortClient);

	pDlg->m_ListTracing.InsertString(0, CString("Client of ")+CString("IP: ") + 
		                             pLocalSocket->m_NewSocketAddrClient +
		                             CString(" Port: ") + str_PORT + CString(" handle: ") + 
		                             str_handle + CString(" is sending. "));

	pDlg->m_ListData.InsertString(0, CString("Client of ") + CString("IP: ") +
		                        pLocalSocket->m_NewSocketAddrClient +
		                        CString(" Port: ") + str_PORT + CString(" handle: ") +
		                       str_handle + CString(" is sending: "));
	pDlg->m_ListData.InsertString(1, CString(Rebuffer));
		
	memset(Rebuffer, 0, 4096);
	
	AsyncSelect(FD_READ|FD_CLOSE);
	pLocalSocket = nullptr;
	CAsyncSocket::OnReceive(nErrorCode);
}

//Using SendTo();
void CSocketServer::OnSend(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	CServerApp *pApp = (CServerApp *)AfxGetApp();        //
	CServerDlg *pDlg = (CServerDlg*)pApp->m_pMainWnd;    //

    
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
		WideCharToMultiByte(CP_ACP, 0,str_buffer.GetBuffer(m_nLength),-1, buffer, m_nLength * 2, NULL, 0);

	    //copy the data in pLPCWSTR to m_szBuffer
		//memcpy(m_szBuffer, pchar, m_nLength * 2);
		//strcpy_s(buffer, m_szBuffer);                                        //

		for (int index = 0; Start_ClientPool != NULL; index++)               // m_ListTracing.InsertString    
		{
			pApp->ClientPool.GetNextAssoc(Start_ClientPool, iKey_handle, ptVal);
            ptVal->SendTo(buffer,
				          strlen(buffer),
				          ptVal->m_NewSocketPortClient,
				          ptVal->m_NewSocketAddrClient);
		}

		m_nLength = 0;
		memset(pDlg->m_pChosenSocketServer->m_szBuffer, 0, sizeof(pDlg->m_pChosenSocketServer->m_szBuffer));
		m_StrBufferSend.Empty();

		AsyncSelect(FD_READ | FD_ACCEPT | FD_CLOSE);
		CAsyncSocket::OnSend(nErrorCode);
		return;
	}
	//log for tracing
	{
		CString str("Server is Sending: ");
		pDlg->m_ListTracing.InsertString(0, str + pDlg->m_pChosenSocketServer->m_StrBufferSend);

		CString str1;
		str1.Format(_T("%d"), m_nLength);
		pDlg->m_ListTracing.InsertString(0, str1);
	}
	//sending m_nLength*2 bytes of data
	if (nullptr != pDlg->m_pChosenSocketServer)
	{
		char buffer[4096] = {0};
		strcpy_s(buffer, pDlg->m_pChosenSocketServer->m_szBuffer);
		pDlg->m_pChosenSocketServer->SendTo(buffer,
			                                strlen(buffer),
			                                pDlg->m_pChosenSocketServer->m_NewSocketPortClient,
			                                pDlg->m_pChosenSocketServer->m_NewSocketAddrClient);
	}
	else
	{
		AfxMessageBox(CString("Select one client or enable the broadcasting!"));
	}

	m_nLength = 0;
	memset(pDlg->m_pChosenSocketServer->m_szBuffer, 0, sizeof(pDlg->m_pChosenSocketServer->m_szBuffer));
	m_StrBufferSend.Empty();

	AsyncSelect(FD_READ | FD_ACCEPT |FD_CLOSE);
	CAsyncSocket::OnSend(nErrorCode);
}


// The framework calls these notification callback functions to notify you of important socket events, 
// such as the receipt of data that you can begin reading.
void CSocketServer::OnAccept(int nErrorCode)        
{
	// TODO: 在此添加专用代码和/或调用基类
	CServerApp *pApp = (CServerApp *)AfxGetApp();
	CServerDlg *pDlg = (CServerDlg*)pApp->m_pMainWnd;  //

	CString  NewSocketAddrLocal;          //
	UINT     NewSocketPortLocal;
	SOCKET   HandleClient;
	char     buffer[1024*4];

	//for storing the information of client's socket on the heap
	CSocketServer *pClientSocket = new CSocketServer();       

	//new client is trying to connect the server and just accept the connection
	if(pDlg->m_SocketServer.Accept(*pClientSocket))
	{
		//AfxMessageBox(CString(" New client! "));

		pDlg->m_SocketServer.GetSockName(NewSocketAddrLocal, NewSocketPortLocal);   //???

		pClientSocket->GetPeerName(pClientSocket->m_NewSocketAddrClient, pClientSocket->m_NewSocketPortClient);
	
		HandleClient = pClientSocket->m_hSocket;
		
		//Synchronizing the Threads of writting data and reading data
		pDlg->m_CriticalSection.Lock();
		//making sure that enough space in ClientPool
			if (!pApp->ClientPool.GetHashTableSize())
			{
				pApp->ClientPool.InitHashTable(100);
			}

			//writing the new client's socket to the container for looking up 
			pApp->ClientPool.SetAt(HandleClient, pClientSocket);

			CString str1, str2;
		   //updating and displaying
			str1.Format(_T("%d"), pApp->ClientPool.GetCount());
			pDlg->m_NumClients.DeleteString(0);
			pDlg->m_NumClients.AddString(str1);

			//code block for updating the combox
			pDlg->m_Choose_Client.ResetContent();
			if (pDlg->m_SocketServer.m_bConnected)
			{
				//clear the data stored in ClientPool
				CServerApp *pApp = (CServerApp *)AfxGetApp();

				unsigned int CountClients = pApp->ClientPool.GetCount();
				POSITION Start_ClientPool = pApp->ClientPool.GetStartPosition();

				SOCKET iKey_handle;
				CSocketServer *ptVal;
				for (int index = 0; Start_ClientPool != NULL; index++)               // m_ListTracing.InsertString    
				{
					pApp->ClientPool.GetNextAssoc(Start_ClientPool, iKey_handle, ptVal);

					str1.Format(_T("%d"), ptVal->m_NewSocketPortClient);
					str2.Format(_T("%d"), iKey_handle);

					pDlg->m_Choose_Client.InsertString(index, CString("IP: ") + ptVal->m_NewSocketAddrClient
													   + CString(" Port: ") + str1 + 
													   CString(" handle: ") + str2);

					pDlg->m_Choose_Client.SetItemData(index, iKey_handle);
				}
			}

		pDlg->m_CriticalSection.Unlock();

		CString str3;
		str3.Format(_T("%d"), pClientSocket->m_NewSocketPortClient);

		CString str4;
		str4.Format(_T("%d"), pClientSocket->m_hSocket);
		pDlg->m_ListTracing.InsertString(0, CString(" New connected client: ") +
			CString("IP: ") + pClientSocket->m_NewSocketAddrClient +
			CString(" Port: ") + str3 +
			CString(" handle: ") + str4);

		//send some message to new connected client
		CString str_buffer;
		pDlg->m_ListTracing.GetText(0, str_buffer);
		str_buffer = CString("Hello,") + str_buffer;

		char pchar[1024 * 4] = { 0 };

		WideCharToMultiByte(CP_ACP, 0, str_buffer.GetBuffer(str_buffer.GetLength()), -1,
			pchar, str_buffer.GetLength() * 2, NULL, 0);

		memcpy(buffer, pchar, str_buffer.GetLength() * 2);  //copy the data in pLPCWSTR to m_szBuffer

		pClientSocket->SendTo(buffer,
			                  strlen(buffer),
			                  pClientSocket->m_NewSocketPortClient,
			                  pClientSocket->m_NewSocketAddrClient);

		pClientSocket = nullptr;
		AsyncSelect(FD_READ | FD_ACCEPT |FD_CLOSE);
		CAsyncSocket::OnAccept(nErrorCode);
		
	}
	else
	{
		delete pClientSocket;
		pClientSocket = nullptr;
		AsyncSelect(FD_READ | FD_CLOSE | FD_ACCEPT);
		CAsyncSocket::OnAccept(nErrorCode);
	}
}


void CSocketServer::OnClose(int nErrorCode)              
{
	// TODO: responding to the active close of client then updating the data structure
	CServerApp *pApp = (CServerApp *)AfxGetApp();
	CServerDlg *pDlg = (CServerDlg*)pApp->m_pMainWnd;   

	CString  NewSocketAddrLocal;
	UINT     NewSocketPortLocal;

	CString strClient("Client: ");

	CSocketServer *pLocalSocket = nullptr;

	pDlg->m_SocketServer.GetSockName(NewSocketAddrLocal, NewSocketPortLocal);   //???

	pDlg->m_CriticalSection.Lock();

		//get the handle of Client
		SOCKET closed_client = Detach();
		//look up the socket in the Cmap
		pApp->ClientPool.Lookup(closed_client, pLocalSocket);                    
		//deleting the handle and socket of client in server's map
		pApp->ClientPool.RemoveKey(closed_client);

		CString str_port;
		str_port.Format(_T("%d"), pLocalSocket->m_NewSocketPortClient);
		CString str_handle;
		str_handle.Format(_T("%d"), closed_client);

		pDlg->m_ListTracing.InsertString(0, strClient +
			CString("IP: ") + pLocalSocket->m_NewSocketAddrClient +
			CString(" Port: ") + str_port
			+ CString(" handle: ") + str_handle +
			CString(" is closed!"));

		//dedicating for new CSocketServer();
		delete pLocalSocket;
		pLocalSocket = nullptr;

		//updating the number of clients
		CString str_numclients;
		str_numclients.Format(_T("%d"), pApp->ClientPool.GetCount());
		pDlg->m_NumClients.DeleteString(0);
		pDlg->m_NumClients.AddString(str_numclients);

		//code block for updating the combox 
		CString str1, str2;
		pDlg->m_Choose_Client.ResetContent();
		if (!pApp->ClientPool.IsEmpty())
		{
			//clear the data stored in ClientPool
			CServerApp *pApp = (CServerApp *)AfxGetApp();

			unsigned int CountClients = pApp->ClientPool.GetCount();
			POSITION Start_ClientPool = pApp->ClientPool.GetStartPosition();

			SOCKET iKey_handle;
			CSocketServer *ptVal;
			for (int index = 0; Start_ClientPool != NULL; index++)    // m_ListTracing.InsertString    
			{
				pApp->ClientPool.GetNextAssoc(Start_ClientPool, iKey_handle, ptVal);

				str1.Format(_T("%d"), ptVal->m_NewSocketPortClient);
				str2.Format(_T("%d"), iKey_handle);
				pDlg->m_Choose_Client.InsertString(index, CString("IP: ") + ptVal->m_NewSocketAddrClient
					+ CString(" Port: ") + str1 + CString(" handle: ") + str2);
				pDlg->m_Choose_Client.SetItemData(index, iKey_handle);
			}
		}

	pDlg->m_CriticalSection.Unlock();

	pDlg->m_ListIP.ResetContent();
	pDlg->m_ListPort.ResetContent();
	pDlg->m_ListClientSocketHandle.ResetContent();

	CAsyncSocket::OnClose(nErrorCode);
}

