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

// FzSFtpIpcClient.cpp: Implementierung der Klasse CFzSFtpIpcClient.
//
//////////////////////////////////////////////////////////////////////

#include "windows.h"
#include "FzSFtpIpcClient.h"
#include "assert.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CFzSFtpIpcClient::CFzSFtpIpcClient()
{
	m_hClientToServerMapping=0;
	m_hClientToServerEvent=0;
	m_hClientToServerEventDone=0;

	m_hServerToClientMapping=0;
	m_hServerToClientEvent=0;
	m_hServerToClientEventDone=0;

	m_pSharedClientToServerMem=0;
	m_pSharedServerToClientMem=0;

	m_hOwner=0;	
}

CFzSFtpIpcClient::~CFzSFtpIpcClient()
{
	if (m_pSharedClientToServerMem)
		UnmapViewOfFile(m_pSharedClientToServerMem);	
	if (m_pSharedServerToClientMem)
		UnmapViewOfFile(m_pSharedServerToClientMem);	
}

BOOL CFzSFtpIpcClient::Init(__int64 handles[])
{
	//Set the global handle objects
	m_hClientToServerMapping=(HANDLE)handles[0];
	m_hClientToServerEvent=(HANDLE)handles[1];
	m_hClientToServerEventDone=(HANDLE)handles[2];
	
	m_hServerToClientMapping=(HANDLE)handles[3];
	m_hServerToClientEvent=(HANDLE)handles[4];
	m_hServerToClientEventDone=(HANDLE)handles[5];

	m_hOwner=(HANDLE)handles[6];
	
	m_pSharedClientToServerMem=MapViewOfFile(m_hClientToServerMapping, FILE_MAP_ALL_ACCESS, 0, 0, 4+4+20480);
	if (!m_pSharedClientToServerMem)
		return FALSE;
	m_pSharedServerToClientMem=MapViewOfFile(m_hServerToClientMapping, FILE_MAP_ALL_ACCESS, 0, 0, 4+4+20480);
	if (!m_pSharedServerToClientMem)
		return FALSE;

	//Set event so that the server knows that the program is valid
	if (!SetEvent(m_hServerToClientEventDone))
		return FALSE;

	return TRUE;
}

BOOL CFzSFtpIpcClient::SendRequest(const DWORD &nID, const DWORD &nDataLength, const LPVOID pData)
{
	assert(nID);
	assert(nDataLength>=0 && nDataLength<=20480);
	assert(!nDataLength || pData);

	HANDLE handles[]={m_hClientToServerEventDone, m_hOwner};
	
	DWORD res = WaitForMultipleObjects(2, handles, FALSE, 5000);
	if (res == WAIT_TIMEOUT || res == WAIT_FAILED || 
		res == WAIT_ABANDONED_0 ||
		res == (WAIT_OBJECT_0+1) || res == (WAIT_ABANDONED_0+1))
		return FALSE;
	
	if (!ResetEvent(m_hClientToServerEventDone))
		return FALSE;

	*((DWORD *)m_pSharedClientToServerMem)=nID;
	*((DWORD *)m_pSharedClientToServerMem+1)=nDataLength;
	if (nDataLength)
		memcpy((BYTE *)m_pSharedClientToServerMem+8, pData, nDataLength);

	if (!SetEvent(m_hClientToServerEvent))
		return FALSE;
	
	res = WaitForMultipleObjects(2, handles, FALSE, 5000);
	if (res == WAIT_TIMEOUT || res == WAIT_FAILED || 
		res == WAIT_ABANDONED_0 ||
		res == (WAIT_OBJECT_0+1) || res == (WAIT_ABANDONED_0+1))
		return FALSE;
	
	return TRUE;
}

BOOL CFzSFtpIpcClient::ReceiveRequest(DWORD &nID, DWORD &nDataLength, LPVOID pData)
{
	assert(pData);

	HANDLE handles[]={m_hServerToClientEvent, m_hOwner};
	
	DWORD res = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
	if (res == WAIT_TIMEOUT || res == WAIT_FAILED || 
		res == WAIT_ABANDONED_0 ||
		res == (WAIT_OBJECT_0+1) || res == (WAIT_ABANDONED_0+1))
		return FALSE;

	if (!ResetEvent(m_hServerToClientEvent))
		return FALSE;
	nID=*(DWORD *)m_pSharedServerToClientMem;
	nDataLength=*((DWORD *)m_pSharedServerToClientMem+1);
	if (nDataLength>20480)
		return FALSE;
	if (nDataLength)
		memcpy(pData, (BYTE *)m_pSharedServerToClientMem+8, nDataLength);

	if (!SetEvent(m_hServerToClientEventDone))
		return FALSE;

	return TRUE;
}
