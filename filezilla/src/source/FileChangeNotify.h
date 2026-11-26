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

// FileChangeNotify.h: interface for the CFileChangeNotify class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILECHANGENOTIFY_H__FD339221_CFF2_4793_B530_CD3A9AB804C5__INCLUDED_)
#define AFX_FILECHANGENOTIFY_H__FD339221_CFF2_4793_B530_CD3A9AB804C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_REQUEST_REFRESH	WM_USER + 23

/*
	CFileChangeNotify - by Chen-hsiu Huang

	class wrapper of WIN32 API
	FindFirstChangeNotification(), FindNextChangeNotification()
	see MSDN if you want to know more detail
*/

class CFileChangeNotify  
{
public:
	CFileChangeNotify();
	virtual ~CFileChangeNotify();

	HWND m_notifyWnd;

	int StartNotify();
	int AddWatch(LPCTSTR lpPathName, BOOL bWatchSubtree, DWORD dwNotifyFilter);
	void StopNotify();

private:

	HANDLE *m_pHH;
	static DWORD WINAPI WatchingThread(LPVOID param);
	std::vector<HANDLE> m_chHandle;

	// identidy if we're in the watching mode
	bool m_isWaiting;
	HANDLE m_hThread;
	HANDLE m_hEvent;
};

#endif // !defined(AFX_FILECHANGENOTIFY_H__FD339221_CFF2_4793_B530_CD3A9AB804C5__INCLUDED_)
