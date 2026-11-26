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

// SFtpIpc.h: Schnittstelle für die Klasse CSFtpIpc.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SFTPIPC_H__71B20B86_C3D8_4197_90C2_349A18548D58__INCLUDED_)
#define AFX_SFTPIPC_H__71B20B86_C3D8_4197_90C2_349A18548D58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSFtpControlSocket;
class CSFtpIpcHelperWindow;
class CSFtpIpc  
{
	friend CSFtpIpcHelperWindow;
public:
	BOOL Send(const DWORD &nID, LPCTSTR str);
	BOOL Send(const DWORD &nID, const DWORD &nDataLength, const LPVOID pData);
	BOOL AttachProcess(HANDLE hSFtpProcess);
	CString GetHandleString();
	CSFtpIpc(CSFtpControlSocket *pOwner);
	BOOL Init();
	LPVOID GetBuffer();
	BOOL ReleaseBuffer(DWORD nID, DWORD nDataLength);
	virtual ~CSFtpIpc();

protected:
	CSFtpControlSocket *m_pOwner;

	CSFtpIpcHelperWindow *m_pHelperWindow;
	HANDLE m_hClientToServerFileMapping;
	HANDLE m_hClientToServerEvent;
	HANDLE m_hClientToServerEventDone;
	
	HANDLE m_hServerToClientFileMapping;
	HANDLE m_hServerToClientEvent;
	HANDLE m_hServerToClientEventDone;
	
	LPVOID m_pSharedServerToClientMem;
	LPVOID m_pSharedClientToServerMem;

	HANDLE m_hSFtpProcess;
	HANDLE m_hOwnProccess;

	HANDLE m_hReceiveThread;
	static DWORD WINAPI ReceiveThread( LPVOID pParam );
	HANDLE m_hExitEvent;
};

#endif // !defined(AFX_SFTPIPC_H__71B20B86_C3D8_4197_90C2_349A18548D58__INCLUDED_)
