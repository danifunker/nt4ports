// IdentServerControl.h: interface for the CIdentServerControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDENTSERVERCONTROL_H__B81DAA40_CB37_11D6_86C4_0050BABBFAD2__INCLUDED_)
#define AFX_IDENTSERVERCONTROL_H__B81DAA40_CB37_11D6_86C4_0050BABBFAD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AsyncSocketEx.h"
class CControlSocket;
class CIdentServerDataSocket;
class CMainThread;
class CIdentServerControl : public CAsyncSocketEx, public CApiLog
{
public:
	int Stop();
	int Run(void);
	CIdentServerControl(CControlSocket *pControlSocket);
	CIdentServerControl(CMainThread *pMainThread);
	virtual ~CIdentServerControl();
	void Remove(CIdentServerDataSocket *pDataSocket);

protected:
	CIdentServerControl();
	void virtual OnAccept(int nErrorCode);
private:
	static CCriticalSection m_Sync;
	static std::list<CIdentServerControl *> m_InstanceList;
	static std::list<CIdentServerDataSocket *> m_DataSocketList;
	static CIdentServerControl *m_pIdentControlSocket;
	static CIdentServerControl *m_pIdentControlSocket6;

	CString m_IP;
	UINT m_nLocalPort;
	UINT m_nRemotePort;
};

#endif // !defined(AFX_IDENTSERVERCONTROL_H__B81DAA40_CB37_11D6_86C4_0050BABBFAD2__INCLUDED_)
