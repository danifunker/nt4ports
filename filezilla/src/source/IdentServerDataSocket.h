// IdentServerDataSocket.h: Schnittstelle für die Klasse CIdentServerDataSocket.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDENTSERVERDATASOCKET_H__65D993E5_A63E_470E_82FC_113FC52218F0__INCLUDED_)
#define AFX_IDENTSERVERDATASOCKET_H__65D993E5_A63E_470E_82FC_113FC52218F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AsyncSocketEx.h"
#include "ApiLog.h"

class CIdentServerControl;
class CIdentServerDataSocket : public CAsyncSocketEx, public CApiLog
{
public:
	CIdentServerDataSocket(CIdentServerControl *pOwner);
	virtual ~CIdentServerDataSocket();
protected:
	void virtual OnReceive(int nErrorCode);
	void virtual OnClose(int nErrorCode);
private:
	int m_BuffSize;
	char * m_Buffer;
	CIdentServerControl *m_pOwner;
};

#endif // !defined(AFX_IDENTSERVERDATASOCKET_H__65D993E5_A63E_470E_82FC_113FC52218F0__INCLUDED_)
