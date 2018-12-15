#pragma once
#include <afxsock.h>


class NewSocket :
	public CAsyncSocket
{
public:
	BOOL m_bConnected;
	UINT m_nLength;

	CString  NewSocketAddrServer;        //address of connected server
	UINT     NewSocketPortServer;        //port of connected server

	CString  NewSocketAddrClient;        //address of connected client
	UINT     NewSocketPortClient;        //port of connected client

	char m_szBuffer[4096];              //for receiving data
	CString m_StrBufferSend;


public:
	NewSocket();
	~NewSocket();
	virtual void OnReceive(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnSend(int nErrorCode);
//	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};

