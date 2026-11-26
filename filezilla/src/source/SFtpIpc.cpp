// FileZilla - a Windows ftp client

// Copyright (C) 2002-2004 - Tim Kosse <tim.kosse@gmx.de>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

// SFtpIpc.cpp: Implementierung der Klasse CSFtpIpc.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SFtpIpc.h"
#include "sftpcontrolsocket.h"
#include "SFtpCommandIDs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

class CSFtpIpcHelperWindow
{
public:
	CSFtpIpcHelperWindow(CSFtpIpc *pOwner)
	{
		ASSERT(pOwner);
		
		m_pOwner=pOwner;

		WNDCLASSEX wndclass; 
		wndclass.cbSize=sizeof wndclass; 
		wndclass.style=0; 
		wndclass.lpfnWndProc=WindowProc; 
		wndclass.cbClsExtra=0; 
		wndclass.cbWndExtra=0; 
		wndclass.hInstance=GetModuleHandle(0); 
		wndclass.hIcon=0; 
		wndclass.hCursor=0; 
		wndclass.hbrBackground=0; 
		wndclass.lpszMenuName=0; 
		wndclass.lpszClassName=_T("CSFtpIpc Helper Window"); 
		wndclass.hIconSm=0; 

		RegisterClassEx(&wndclass);

		m_hWnd=CreateWindow(_T("CSFtpIpc Helper Window"), _T("CSFtpIpc Helper Window"), 0, 0, 0, 0, 0, 0, 0, 0, GetModuleHandle(0));
		ASSERT(m_hWnd);
		SetWindowLongPtr(m_hWnd, GWL_USERDATA, (LONG)this);
	}

	virtual ~CSFtpIpcHelperWindow()
	{
		if (m_hWnd)
		{
			DestroyWindow(m_hWnd);
			m_hWnd=0;
		}
	}

	static LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		if (message==WM_USER)
		{
			CSFtpIpcHelperWindow *pWnd=(CSFtpIpcHelperWindow *)GetWindowLongPtr(hWnd, GWL_USERDATA);
			ASSERT(pWnd);
			if (reinterpret_cast<CSFtpIpc*>(wParam)!=pWnd->m_pOwner || !lParam)
				return 3;

			LPVOID pData=(LPVOID)lParam;

			int nID=*reinterpret_cast<int*>(pData);
			if (!nID)
			{
				pWnd->m_pOwner->m_pOwner->OnClientError(3);
				delete [] pData;
				return 3;
			}
			int nDataLength=*(reinterpret_cast<int*>(pData)+1);
			if (nDataLength>20480 || nDataLength<0)
			{
				pWnd->m_pOwner->m_pOwner->OnClientError(3);
				delete [] pData;
				return 3;
			}

			int res=nID=pWnd->m_pOwner->m_pOwner->OnClientRequest(nID, nDataLength, (int *)pData+2);
			delete [] pData;
			if (res)
			{
				pWnd->m_pOwner->m_pOwner->OnClientError(res);
				return res;
			}
			
			return res;
		}
		else if (message>WM_USER)
		{
			CSFtpIpcHelperWindow *pWnd=(CSFtpIpcHelperWindow *)GetWindowLongPtr(hWnd, GWL_USERDATA);
			ASSERT(pWnd);
			if (reinterpret_cast<CSFtpIpcHelperWindow*>(lParam)!=pWnd || reinterpret_cast<CSFtpIpc*>(wParam)!=pWnd->m_pOwner)
				return 0;
			pWnd->m_pOwner->m_pOwner->OnClientError(message-WM_USER);
			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);		
	}

	CSFtpIpc *m_pOwner;
	HWND m_hWnd;
};

CSFtpIpc::CSFtpIpc(CSFtpControlSocket *pOwner)
{
	ASSERT(pOwner);
	m_pOwner=pOwner;
	
	m_pHelperWindow=0;
	m_hClientToServerFileMapping = 0;
	m_hServerToClientFileMapping = 0;
	m_hServerToClientEvent=0;
	m_hServerToClientEventDone=0;
	m_hClientToServerEvent=0;
	m_hClientToServerEventDone=0;
	m_pSharedClientToServerMem=0;
	m_pSharedServerToClientMem=0;

	m_hSFtpProcess=0;
	m_hOwnProccess=0;

	m_hReceiveThread=0;
	m_hExitEvent=0;
}

CSFtpIpc::~CSFtpIpc()
{
	if (m_hExitEvent)
		VERIFY(SetEvent(m_hExitEvent));
	if (m_hReceiveThread)
	{
		BOOL res=WaitForSingleObject(m_hReceiveThread, 1000);
		//if (res==WAIT_FAILED || res==WAIT_TIMEOUT)
			TerminateThread(m_hReceiveThread, 0);
		VERIFY(CloseHandle(m_hReceiveThread));
	}
	if (m_hExitEvent)
		VERIFY(CloseHandle(m_hExitEvent));
	
	delete m_pHelperWindow;
	if (m_pSharedClientToServerMem)
		VERIFY( UnmapViewOfFile(m_pSharedClientToServerMem) );
	if (m_hClientToServerFileMapping)
		VERIFY( CloseHandle(m_hClientToServerFileMapping) );
	if (m_hClientToServerEvent)
		VERIFY( CloseHandle(m_hClientToServerEvent) );
	if (m_hClientToServerEventDone)
		VERIFY( CloseHandle(m_hClientToServerEventDone) );

	if (m_pSharedServerToClientMem)
		VERIFY( UnmapViewOfFile(m_pSharedServerToClientMem) );
	if (m_hServerToClientFileMapping)
		VERIFY( CloseHandle(m_hServerToClientFileMapping) );
	if (m_hServerToClientEvent)
		VERIFY( CloseHandle(m_hServerToClientEvent) );
	if (m_hServerToClientEventDone)
		VERIFY( CloseHandle(m_hServerToClientEventDone) );
	if (m_hOwnProccess)
		VERIFY( CloseHandle(m_hOwnProccess) );
}

BOOL CSFtpIpc::Init()
{
	m_pHelperWindow = new CSFtpIpcHelperWindow(this);
	if (!m_pHelperWindow)
		return FALSE;

	SECURITY_ATTRIBUTES SecAttribs;
	SecAttribs.nLength=sizeof(SECURITY_ATTRIBUTES);
	SecAttribs.bInheritHandle=TRUE;
	SecAttribs.lpSecurityDescriptor=NULL;
	
	//Total size: 4+4+20480
	m_hClientToServerFileMapping=CreateFileMapping((HANDLE)0xFFFFFFFF, &SecAttribs, PAGE_READWRITE, 0, 4+4+20480, 0);
	if (!m_hClientToServerFileMapping)
		return FALSE;
	m_pSharedClientToServerMem=MapViewOfFile( m_hClientToServerFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 4+4+20480);
	if (!m_pSharedClientToServerMem)
		return FALSE;
	m_hClientToServerEvent=CreateEvent( &SecAttribs, TRUE, FALSE, NULL);
	if (!m_hClientToServerEvent)
		return FALSE;
	m_hClientToServerEventDone=CreateEvent( &SecAttribs, TRUE, TRUE, NULL);
	if (!m_hClientToServerEventDone)
		return FALSE;
	
	m_hServerToClientFileMapping=CreateFileMapping((HANDLE)0xFFFFFFFF, &SecAttribs, PAGE_READWRITE, 0, 4+4+20480, 0);
	if (!m_hServerToClientFileMapping)
		return FALSE;
	m_pSharedServerToClientMem=MapViewOfFile( m_hServerToClientFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 4+4+20480);
	if (!m_pSharedServerToClientMem)
		return FALSE;
	m_hServerToClientEvent=CreateEvent( &SecAttribs, TRUE, FALSE, NULL);
	if (!m_hServerToClientEvent)
		return FALSE;
	m_hServerToClientEventDone=CreateEvent( &SecAttribs, TRUE, FALSE, NULL);
	if (!m_hServerToClientEventDone)
		return FALSE;

	m_hExitEvent=CreateEvent( &SecAttribs, TRUE, FALSE, NULL);
	if (!m_hExitEvent)
		return FALSE;
	DWORD tmp;
	m_hReceiveThread=CreateThread(0, 0, &ReceiveThread, this, CREATE_SUSPENDED, &tmp);
	if (!m_hReceiveThread)
		return FALSE;

	HANDLE hProcess=GetCurrentProcess();
	if (!DuplicateHandle(hProcess, hProcess, hProcess, &m_hOwnProccess, 0, TRUE, DUPLICATE_SAME_ACCESS))
	{
		m_hOwnProccess=0;
		return FALSE;
	}
	return TRUE;
}

CString CSFtpIpc::GetHandleString()
{
	CString str;
	str.Format( _T("%u %u %u %u %u %u %u"),
				m_hClientToServerFileMapping,
				m_hClientToServerEvent,
				m_hClientToServerEventDone,
				m_hServerToClientFileMapping,
				m_hServerToClientEvent,
				m_hServerToClientEventDone,
				m_hOwnProccess);
	return str;
}


BOOL CSFtpIpc::AttachProcess(HANDLE hSFtpProcess)
{
	ASSERT(hSFtpProcess);
	ASSERT(!m_hSFtpProcess);

	m_hSFtpProcess = hSFtpProcess;

	HANDLE handles[]={m_hServerToClientEventDone, m_hSFtpProcess};
	
	DWORD res = WaitForMultipleObjects(2, handles, FALSE, 5000);
	if (res == WAIT_TIMEOUT || res == WAIT_FAILED || 
		res == (WAIT_OBJECT_0+1) ||
		res == WAIT_ABANDONED_0 ||
		res == (WAIT_ABANDONED_0+1))
		return FALSE;

	VERIFY(ResumeThread(m_hReceiveThread)!=0xFFFFFFFF);

	return TRUE;
}

DWORD WINAPI CSFtpIpc::ReceiveThread(LPVOID pParam)
{
	CSFtpIpc *pOwner=reinterpret_cast<CSFtpIpc *>(pParam);
	ASSERT(pOwner);

	while(TRUE)
	{

		HANDLE handles[]={pOwner->m_hClientToServerEvent, pOwner->m_hSFtpProcess, pOwner->m_hExitEvent };

		DWORD res = WaitForMultipleObjects(3, handles, FALSE, INFINITE);
		if (res == WAIT_TIMEOUT || res == WAIT_FAILED)
		{
			//FzSFtp.exe is blocking
			VERIFY(PostMessage(pOwner->m_pHelperWindow->m_hWnd, WM_USER+2, reinterpret_cast<WPARAM>(pOwner), reinterpret_cast<LPARAM>(pOwner->m_pHelperWindow)));
			return 2;
		}
		else if (res == (WAIT_OBJECT_0+1) || res == (WAIT_ABANDONED_0+1))
		{
			//FzSFtp.exe has been closed
			VERIFY(PostMessage(pOwner->m_pHelperWindow->m_hWnd, WM_USER+1, reinterpret_cast<WPARAM>(pOwner), reinterpret_cast<LPARAM>(pOwner->m_pHelperWindow)));
			return 1;
		}
		else if (res == (WAIT_OBJECT_0+2) ||
				res == WAIT_ABANDONED_0 ||
				res == (WAIT_ABANDONED_0+2))
		{
			//Either qúit event was set or events were closed without termination this thread first
			return 0;
		}

		char *pData=new char[4+4+20480];
		
		int nDataLength=*((int *)pOwner->m_pSharedClientToServerMem+1);
		if (nDataLength>20480 || nDataLength<0)
			nDataLength=20480;
		memcpy(pData, pOwner->m_pSharedClientToServerMem, 4+4+nDataLength);
		int nID=*(int*)pData;
		VERIFY(PostMessage(pOwner->m_pHelperWindow->m_hWnd, WM_USER, reinterpret_cast<WPARAM>(pOwner), (LPARAM)pData));

		if (nID==SFTP_DATAID_CTS_FATALERROR || nID==SFTP_DATAID_CTS_CRITICALERROR)
			//Due to critical/fatal error, FzSFtp will close automatically and FileZilla will reset connection.
			//By exiting this thread, we avoid the "FzSFtp.exe closed" error
			return 1;
		
		VERIFY(ResetEvent(pOwner->m_hClientToServerEvent));
		VERIFY(SetEvent(pOwner->m_hClientToServerEventDone));		
	}
	return 0;
}

BOOL CSFtpIpc::Send(const DWORD &nID, const DWORD &nDataLength, const LPVOID pData)
{
	USES_CONVERSION;
	ASSERT(m_hSFtpProcess);
	ASSERT(nID);
	ASSERT(nDataLength>=0 && nDataLength<=20480);

	ASSERT(!nDataLength || pData);

	CString str;
	if (nID==SFTP_DATAID_STC_CONNECT)
		str.Format( _T("CONNECT %s@%s:%d"), A2CT((char *)pData+strlen((char *)pData)+1+4), A2CT((char *)pData), *(int *)((char *)pData+strlen((char *)pData)+1) );
	else if (nID==SFTP_DATAID_STC_PWD)
		str="PWD";
	else if (nID==SFTP_DATAID_STC_CD)
		str=(CString)"CD " + (char *)pData;
	else if (nID==SFTP_DATAID_STC_LIST)
		str="LIST";
	else if (nID==SFTP_DATAID_STC_MKD)
		str=(CString)"MKD " + (char *)pData;
	else if (nID==SFTP_DATAID_STC_GET)
		str.Format( _T("GET %s %s %s"), 
					A2CT((char *)pData), 
					A2CT((char *)pData+strlen((char *)pData)+1), 
					( *(int *)((char *)pData+strlen((char *)pData)+1+strlen((char *)pData+strlen((char *)pData)+1)+1)?_T("TRUE"):_T("FALSE") ));
	else if (nID==SFTP_DATAID_STC_PUT)
		str.Format( _T("PUT %s %s %s"), 
					A2CT((char *)pData), 
					A2CT((char *)pData+strlen((char *)pData)+1), 
					( *(int *)((char *)pData+strlen((char *)pData)+1+strlen((char *)pData+strlen((char *)pData)+1)+1)?_T("TRUE"):_T("FALSE") ));
	else if (nID==SFTP_DATAID_STC_DELE)
		str=(CString)"DELE " + (char *)pData;
	else if (nID==SFTP_DATAID_STC_RMD)
		str=(CString)"RMD " + (char *)pData;
	else if (nID==SFTP_DATAID_STC_RENAME)
		str.Format( _T("RENAME %s %s"), 
					A2CT((char *)pData), 
					A2CT((char *)pData+strlen((char *)pData)+1));
	else if (nID==SFTP_DATAID_STC_CHMOD)
		str.Format( _T("CHMOD %s %s"), 
					A2CT((char *)pData), 
					A2CT((char *)pData+strlen((char *)pData)+1));
	if (str!="")
		m_pOwner->ShowStatus(str, 2);
		

	HANDLE handles[]={m_hServerToClientEventDone, m_hSFtpProcess};
	
	DWORD res = WaitForMultipleObjects(2, handles, FALSE, 5000);
	if (res == WAIT_TIMEOUT || res == WAIT_FAILED)
	{
		m_pOwner->ShowStatus(IDS_ERRORMSG_SFTP_NORESPONSE, 1);
		return FALSE;
	}
	else if (res == (WAIT_OBJECT_0+1) ||
			res == WAIT_ABANDONED_0 ||
			res == (WAIT_ABANDONED_0+1))
	{
		m_pOwner->ShowStatus(IDS_ERRORMSG_SFTP_CLOSED, 1);
		return FALSE;
	}
	
	VERIFY(ResetEvent(m_hServerToClientEventDone));

	*((DWORD *)m_pSharedServerToClientMem)=nID;
	*((DWORD *)m_pSharedServerToClientMem+1)=nDataLength;
	if (nDataLength)
		memcpy((BYTE *)m_pSharedServerToClientMem+8, pData, nDataLength);

	SetEvent(m_hServerToClientEvent);
	
	res = WaitForMultipleObjects(2, handles, FALSE, 5000);
	if (res == WAIT_TIMEOUT || res == WAIT_FAILED)
	{
		m_pOwner->ShowStatus(IDS_ERRORMSG_SFTP_NORESPONSE, 1);
		return FALSE;
	}
	else if (res == (WAIT_OBJECT_0+1) ||
			res == WAIT_ABANDONED_0 ||
			res == (WAIT_ABANDONED_0+1))
	{
		m_pOwner->ShowStatus(IDS_ERRORMSG_SFTP_CLOSED, 1);
		return FALSE;
	}
	
	return TRUE;
}

BOOL CSFtpIpc::Send(const DWORD &nID, LPCTSTR str)
{
	USES_CONVERSION;
	if (str)
		return Send(nID, (const DWORD)strlen(T2CA(str))+1, (LPVOID)T2CA(str));
	else
		return Send(nID, 0, 0);
}

LPVOID CSFtpIpc::GetBuffer()
{
	ASSERT(m_hSFtpProcess);

	HANDLE handles[]={m_hServerToClientEventDone, m_hSFtpProcess};
	
	DWORD res = WaitForMultipleObjects(2, handles, FALSE, 5000);
	if (res == WAIT_TIMEOUT || res == WAIT_FAILED)
	{
		m_pOwner->ShowStatus(IDS_ERRORMSG_SFTP_NORESPONSE, 1);
		return 0;
	}
	else if (res == (WAIT_OBJECT_0+1) ||
			res == WAIT_ABANDONED_0 ||
			res == (WAIT_ABANDONED_0+1))
	{
		m_pOwner->ShowStatus(IDS_ERRORMSG_SFTP_CLOSED, 1);
		return 0;
	}
	
	VERIFY(ResetEvent(m_hServerToClientEventDone));

	return ((DWORD *)m_pSharedServerToClientMem+2);
}

BOOL CSFtpIpc::ReleaseBuffer(DWORD nID, DWORD nDataLength)
{
	ASSERT(nID);
	ASSERT(nDataLength>=0 && nDataLength<=20480);

	*((DWORD *)m_pSharedServerToClientMem)=nID;
	*((DWORD *)m_pSharedServerToClientMem+1)=nDataLength;

	SetEvent(m_hServerToClientEvent);

	HANDLE handles[]={m_hServerToClientEventDone, m_hSFtpProcess};
	DWORD res = WaitForMultipleObjects(2, handles, FALSE, 5000);
	if (res == WAIT_TIMEOUT || res == WAIT_FAILED)
	{
		m_pOwner->ShowStatus(IDS_ERRORMSG_SFTP_NORESPONSE, 1);
		return FALSE;
	}
	else if (res == (WAIT_OBJECT_0+1) ||
			res == WAIT_ABANDONED_0 ||
			res == (WAIT_ABANDONED_0+1))
	{
		m_pOwner->ShowStatus(IDS_ERRORMSG_SFTP_CLOSED, 1);
		return FALSE;
	}

	return TRUE;
}