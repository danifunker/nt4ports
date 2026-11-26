// IdentServerControl.cpp: implementation of the CIdentServerControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mainthread.h"
#include "IdentServerControl.h"
#include "IdentServerDataSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCriticalSection CIdentServerControl::m_Sync;
std::list<CIdentServerControl *> CIdentServerControl::m_InstanceList;
std::list<CIdentServerDataSocket *> CIdentServerControl::m_DataSocketList;
CIdentServerControl *CIdentServerControl::m_pIdentControlSocket = 0;
CIdentServerControl *CIdentServerControl::m_pIdentControlSocket6 = 0;

CIdentServerControl::CIdentServerControl(CControlSocket *pControlSocket)
{
	InitLog(pControlSocket);
		
	m_Sync.Lock();
	pControlSocket->GetPeerName(m_IP, m_nRemotePort);
	m_InstanceList.push_back(this);

	if (!m_pIdentControlSocket)
	{
		m_pIdentControlSocket = new CIdentServerControl();
		m_pIdentControlSocket->SetFamily(AF_INET);
		m_pIdentControlSocket->InitLog(this);
		m_pIdentControlSocket->Run();
	}
	if (!m_pIdentControlSocket6 && COptions::GetOptionVal(OPTION_ENABLE_IPV6))
	{
		m_pIdentControlSocket6 = new CIdentServerControl();
		m_pIdentControlSocket6->SetFamily(AF_INET6);
		m_pIdentControlSocket6->InitLog(this);
		m_pIdentControlSocket6->Run();
	}
	m_Sync.Unlock();
}

CIdentServerControl::CIdentServerControl(CMainThread *pMainThread)
{
	InitLog(pMainThread);
		
	m_Sync.Lock();
	m_InstanceList.push_back(this);

	if (!m_pIdentControlSocket)
	{
		m_pIdentControlSocket = new CIdentServerControl();
		m_pIdentControlSocket->SetFamily(AF_INET);
		m_pIdentControlSocket->InitLog(this);
		m_pIdentControlSocket->Run();
	}
	if (!m_pIdentControlSocket6 && COptions::GetOptionVal(OPTION_ENABLE_IPV6))
	{
		m_pIdentControlSocket6 = new CIdentServerControl();
		m_pIdentControlSocket6->SetFamily(AF_INET6);
		m_pIdentControlSocket6->InitLog(this);
		m_pIdentControlSocket6->Run();
	}
	m_Sync.Unlock();
}

CIdentServerControl::CIdentServerControl()
{
}

CIdentServerControl::~CIdentServerControl()
{
	CSingleLock(&m_Sync, TRUE);

	if (this == m_pIdentControlSocket || this == m_pIdentControlSocket6)
	{
		Stop();
	}
	else
	{
		if (this == m_InstanceList.front())
			m_pIdentControlSocket->InitLog(m_InstanceList.back());
		std::list<CIdentServerControl *>::iterator self = std::find(m_InstanceList.begin(), m_InstanceList.end(), this);
		if (self != m_InstanceList.end())
			m_InstanceList.erase(self);
	
		if (m_InstanceList.empty())
		{
			delete m_pIdentControlSocket;
			m_pIdentControlSocket = 0;
			delete m_pIdentControlSocket6;
			m_pIdentControlSocket6 = 0;
		}
	}
}

int CIdentServerControl::Run()
{
	int val=Create(113);
	if(!val)
		LogMessage(FZ_LOG_ERROR, IDS_ERRORMSG_IDENT_CANTSTART);	
	else
		LogMessage(FZ_LOG_STATUS, IDS_STATUSMSG_IDENT_START);
	Listen();
	return 1;
}

int CIdentServerControl::Stop()
{
	std::list<CIdentServerDataSocket *>::iterator i;
	while (!m_DataSocketList.empty())
	{
		delete m_DataSocketList.front();
	}
	m_DataSocketList.clear();
	Close();
	LogMessage(FZ_LOG_STATUS, IDS_STATUSMSG_IDENT_STOP);
	
	return 1;
}

void CIdentServerControl::OnAccept(int nErrorCode)
{
	CIdentServerDataSocket *pDataSocket=new CIdentServerDataSocket(this);

	Accept(*pDataSocket);
	pDataSocket->AsyncSelect();

	CString peername;
	UINT nPort;
	CSingleLock(&m_Sync, TRUE);
	if (COptions::GetOptionVal(OPTION_IDENTSAMEIP))
	{
		if (pDataSocket->GetPeerName(peername, nPort))
		{
			std::list<CIdentServerControl *>::iterator i;
			for (i=m_InstanceList.begin();i!=m_InstanceList.end();i++)
				if ((*i)->m_IP==peername)
				{
					m_DataSocketList.push_back(pDataSocket);
					CString logmsg;
					logmsg.Format(_T("Ident request from : %s"), peername);
					LogMessageRaw(FZ_LOG_STATUS, logmsg);
					return;
				}
		}
		pDataSocket->Close();
		delete pDataSocket;
	}
	else
	{
		if (pDataSocket->GetPeerName(peername, nPort))
		{
			CString logmsg;
			logmsg.Format(IDS_STATUSMSG_IDENT_REQUEST, peername);
			LogMessageRaw(FZ_LOG_STATUS, logmsg);
		}
		
		m_DataSocketList.push_back(pDataSocket);
	}
}

void CIdentServerControl::Remove(CIdentServerDataSocket *pDataSocket)
{
	CSingleLock(&m_Sync, TRUE);
	std::list<CIdentServerDataSocket *>::iterator i;
	for (i=m_DataSocketList.begin();i!=m_DataSocketList.end();i++)
		if (*i==pDataSocket)
		{
			m_DataSocketList.erase(i);
			break;
		}
}