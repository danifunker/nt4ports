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

// FzSFtpIpcClient.h: Schnittstelle für die Klasse CFzSFtpIpcClient.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FZSFTPIPCCLIENT_H__7DD671A4_6F4F_4B44_9EDC_EB5A7392375A__INCLUDED_)
#define AFX_FZSFTPIPCCLIENT_H__7DD671A4_6F4F_4B44_9EDC_EB5A7392375A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFzSFtpIpcClient  
{
public:
	CFzSFtpIpcClient();
	virtual ~CFzSFtpIpcClient();
	BOOL Init(_int64 handles[]);
	BOOL SendRequest(const DWORD &nID, const DWORD &nDataLength, const LPVOID pData);
	BOOL ReceiveRequest(DWORD &nID, DWORD &nDataLength, LPVOID pData);
protected:
	HANDLE m_hOwner;
	HANDLE m_hClientToServerMapping;
	HANDLE m_hClientToServerEvent;
	HANDLE m_hClientToServerEventDone;
	
	HANDLE m_hServerToClientMapping;
	HANDLE m_hServerToClientEvent;
	HANDLE m_hServerToClientEventDone;

	LPVOID m_pSharedServerToClientMem;
	LPVOID m_pSharedClientToServerMem;
};

#endif // !defined(AFX_FZSFTPIPCCLIENT_H__7DD671A4_6F4F_4B44_9EDC_EB5A7392375A__INCLUDED_)
