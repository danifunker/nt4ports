// IdentServerDataSocket.cpp: Implementierung der Klasse CIdentServerDataSocket.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IdentServerDataSocket.h"
#include "IdentServerControl.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CIdentServerDataSocket::CIdentServerDataSocket(CIdentServerControl *pOwner)
{
	InitLog(pOwner);
	m_Buffer = new char[1024];
	m_BuffSize=0;
	m_pOwner=pOwner;
}

CIdentServerDataSocket::~CIdentServerDataSocket()
{
	delete [] m_Buffer;
	m_pOwner->Remove(this);
}


void CIdentServerDataSocket::OnReceive(int nErrorCode)
{
	int received,port1,port2;
	
	bool bLineComplete=false;
	received = Receive(m_Buffer+m_BuffSize, 1000-m_BuffSize);
	while(received != SOCKET_ERROR && received && m_BuffSize<1000)
	{
		for (int i=m_BuffSize;i<(m_BuffSize+received);i++)
		{
			if (m_Buffer[i]=='\n')
			{
				m_BuffSize=i;
				if (i && m_Buffer[i-1]=='\r')
					m_BuffSize--;
				m_Buffer[m_BuffSize]=0;
				bLineComplete=true;
				break;
			}
		}
		if (bLineComplete)
			break;
		
		m_BuffSize+=received;

		received = Receive(m_Buffer+m_BuffSize, 1000-m_BuffSize);
	}
	if (bLineComplete)
	{
		CString tosend=m_Buffer;
		tosend.TrimRight();
		//checking if the request as the good format : "%d , %d"
		if(2!=sscanf(m_Buffer, "%d , %d", &port1, &port2))
			tosend+=" : ERROR : UNKNOWN-ERROR";	//if not the right format, reply as specified in RFC1413
		else
			if(port1<65535 && port1>0 && port2<65535 && port2>0)	//check if right port value
			{
				tosend += _T(" : USERID : ");
				
				tosend += COptions::GetOption(OPTION_IDENTSYSTEM);
				tosend += " : ";
				tosend += COptions::GetOption(OPTION_IDENTUSER);
			}
			else
				tosend+=" : ERROR : UNKNOWN-ERROR";
			
		LogMessageRaw(FZ_LOG_INFO, _T("Sending ident response: ") + tosend);

		tosend += _T("\r\n");

		USES_CONVERSION;
		if (Send(T2CA(tosend), tosend.GetLength())!=tosend.GetLength())
		{
			Close();
			delete this;
		}
		else
			TriggerEvent(FD_CLOSE);
		return;
	}
	else if (received==SOCKET_ERROR)
	{
		if (GetLastError()==WSAEWOULDBLOCK)
			return;
	}
	Close();
	delete this;
}

void CIdentServerDataSocket::OnClose(int nErrorCode)
{
	Close();
	delete this;
}
