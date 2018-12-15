//CNewSocket class for recording the new socket information of client 
//
#pragma once
#include <afxsock.h>

class CNewSocket :
	public CAsyncSocket
{
public:
	CNewSocket();
	~CNewSocket();

	virtual void OnSend(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);

public:
	CString  m_NewSocketAddrClient;        //address of connected client
	UINT     m_NewSocketPortClient;        //port of connected client

	BOOL        m_bConnected;
	UINT        m_nLength;
	char        m_szBuffer[4096];  //for receiving data
	CString     m_StrBufferSend;
};

