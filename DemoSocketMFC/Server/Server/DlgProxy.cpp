
// DlgProxy.cpp: 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "DlgProxy.h"
#include "ServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerDlgAutoProxy

IMPLEMENT_DYNCREATE(CServerDlgAutoProxy, CCmdTarget)

CServerDlgAutoProxy::CServerDlgAutoProxy()
{
	EnableAutomation();

	// 为使应用程序在自动化对象处于活动状态时一直保持
	//	运行，构造函数调用 AfxOleLockApp。
	AfxOleLockApp();

	// 通过应用程序的主窗口指针
	//  来访问对话框。  设置代理的内部指针
	//  指向对话框，并设置对话框的后向指针指向
	//  该代理。
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CServerDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CServerDlg)))
		{
			m_pDialog = reinterpret_cast<CServerDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CServerDlgAutoProxy::~CServerDlgAutoProxy()
{
	// 为了在用 OLE 自动化创建所有对象后终止应用程序，
	//	析构函数调用 AfxOleUnlockApp。
	//  除了做其他事情外，这还将销毁主对话框
	if (m_pDialog != nullptr)
		m_pDialog->m_pAutoProxy = nullptr;
	AfxOleUnlockApp();
}

void CServerDlgAutoProxy::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。  基类将自动
	// 删除该对象。  在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CServerDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CServerDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// 注意: 我们添加了对 IID_IServer 的支持来支持类型安全绑定
//  以支持来自 VBA 的类型安全绑定。  此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {e56a7148-7a36-435e-8eda-17b69df8d784}
static const IID IID_IServer =
{0xe56a7148,0x7a36,0x435e,{0x8e,0xda,0x17,0xb6,0x9d,0xf8,0xd7,0x84}};

BEGIN_INTERFACE_MAP(CServerDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CServerDlgAutoProxy, IID_IServer, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 宏在此项目的 StdAfx.h 中定义
// {7d3a666a-8462-4126-829b-2939e7cd207c}
IMPLEMENT_OLECREATE2(CServerDlgAutoProxy, "Server.Application", 0x7d3a666a,0x8462,0x4126,0x82,0x9b,0x29,0x39,0xe7,0xcd,0x20,0x7c)


// CServerDlgAutoProxy 消息处理程序
