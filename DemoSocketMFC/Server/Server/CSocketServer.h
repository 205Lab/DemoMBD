#pragma once
#include <afxsock.h>
#include "NewSocket.h"     //for serving the new connection of client

class CSocketServer : public CAsyncSocket
{
public:

	BOOL            m_bConnected;
	UINT            m_nLength;
	char            m_szBuffer[4096];           //for receiving data
	CString         m_StrBufferSend;
	


	CString         m_StrAddressServerLocal;    //container of server IP address
	UINT            m_ServerLocalPort;          //container of server port 

	CString         m_NewSocketAddrClient;        //address of connected client
	UINT            m_NewSocketPortClient;        //port of connected client

public:
	CSocketServer();
	~CSocketServer();
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
//	virtual void Dump(CDumpContext& dc) const;
};

