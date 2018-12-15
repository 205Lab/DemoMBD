
// Server.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "NewSocket.h"      //for servering the connection client
#include "CSocketServer.h"
#include <afxtempl.h>
#include "afxmt.h"


// CServerApp:
// 有关此类的实现，请参阅 Server.cpp
//
extern unsigned int ThreadTimedSend(LPVOID lpParam);

class CServerApp : public CWinApp
{
public:
	CServerApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	DECLARE_MESSAGE_MAP()

public:
	CList<SOCKET, SOCKET> m_listSocket;       //list of client sockets

	UINT CountClientSocket;

	CMap<SOCKET, SOCKET, CSocketServer *, CSocketServer *> ClientPool;
};

extern CServerApp theApp;
