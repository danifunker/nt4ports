// FileZilla - a Windows ftp client

// Copyright (C) 2002-2005 - Tim Kosse <tim.kosse@gmx.de>

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

// FileChangeNotify.cpp: implementation of the CFileChangeNotify class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileChangeNotify.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileChangeNotify::CFileChangeNotify()
{
	m_isWaiting = false;
	m_notifyWnd = NULL;
	m_hThread = INVALID_HANDLE_VALUE;
	m_hEvent = INVALID_HANDLE_VALUE;
	m_pHH = NULL;
}	

CFileChangeNotify::~CFileChangeNotify()
{
	StopNotify();
}

int CFileChangeNotify::AddWatch(LPCTSTR lpPathName, BOOL bWatchSubtree, DWORD dwNotifyFilter)
{
	if (m_isWaiting)
		return -1;
	HANDLE handle = FindFirstChangeNotification(lpPathName, bWatchSubtree, dwNotifyFilter);
	if (handle == INVALID_HANDLE_VALUE)
		return -1;
	m_chHandle.push_back(handle);
	return 0;
}

DWORD WINAPI CFileChangeNotify::WatchingThread(LPVOID param)
{
	CFileChangeNotify *pNotify = (CFileChangeNotify *)param;
	
	std::vector<HANDLE> &handles = pNotify->m_chHandle;

	int objnums = handles.size() + 1;

	while (true)
	{
		// Send notifications no more than once per second
		if (WaitForSingleObject(pNotify->m_pHH[objnums - 1], 1000) != WAIT_TIMEOUT)
			return 0;

		// Wait for notification.
		DWORD status = WaitForMultipleObjects(objnums, pNotify->m_pHH, FALSE, INFINITE); 
		if (status == (WAIT_OBJECT_0 + objnums - 1) ||
			status == (WAIT_ABANDONED_0 + objnums - 1))
			return 0;

		int idx = status - WAIT_OBJECT_0;
		::SendMessage(pNotify->m_notifyWnd, WM_REQUEST_REFRESH, idx, NULL);
		if (FindNextChangeNotification(pNotify->m_pHH[idx]) == FALSE)
			break;
	}

	return 0;
}

int CFileChangeNotify::StartNotify()
{
	if (m_isWaiting) 
		return -1;

	if (!m_notifyWnd)
		return -1;


	m_pHH = new HANDLE[m_chHandle.size() + 1];
	for (unsigned int i = 0; i < m_chHandle.size(); i++)
		m_pHH[i] = m_chHandle[i];

	// At the topmost index, but the event that will be signaled when the thread should quit
	m_hEvent = CreateEvent(0, false, false, 0);
	if (m_hEvent == INVALID_HANDLE_VALUE)
		return -1;
	m_pHH[m_chHandle.size()] = m_hEvent;

	m_hThread = CreateThread(NULL, NULL, WatchingThread, (LPVOID)this, NULL, NULL);
	if (m_hThread == INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hEvent);
		m_hEvent = INVALID_HANDLE_VALUE;
		return -1;
	}

	m_isWaiting = true;		// enter the watching mode

	return 0;
}

void CFileChangeNotify::StopNotify()
{
	m_isWaiting = false;

	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		SetEvent(m_hEvent);
		if (WaitForSingleObject(m_hThread, 1000) == WAIT_TIMEOUT)
			TerminateThread(m_hThread, 0);
		CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}
	if (m_hEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hEvent);
		m_hEvent = INVALID_HANDLE_VALUE;
	}

	m_notifyWnd = NULL;
	
	for (unsigned int i = 0; i < m_chHandle.size(); i++)
		CloseHandle(m_chHandle[i]);

	m_chHandle.clear();
	delete [] m_pHH;
	m_pHH = NULL;
}

