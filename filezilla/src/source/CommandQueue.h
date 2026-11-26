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

// CommandQueue.h: Schnittstelle für die Klasse CCommandQueue.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDQUEUE_H__022F1125_D783_48B6_A2B8_E623856893DA__INCLUDED_)
#define AFX_COMMANDQUEUE_H__022F1125_D783_48B6_A2B8_E623856893DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileZillaApi.h"
#include "retryserverlist.h"

class CCommandQueue  
{
public:
	CCommandQueue(CFileZillaApi *pFileZillaApi);
	virtual ~CCommandQueue();

	void SetLastServerPath(const CServerPath &path);
	BOOL GetLastServer(t_server &server, CServerPath &path);
	void OnRetryTimer();
	BOOL Rename(CString oldName,CString newName, const CServerPath &path = CServerPath(), const CServerPath &newPath = CServerPath());
	BOOL MakeDir(const CServerPath &path = CServerPath());
	BOOL RemoveDir(CString DirName, const CServerPath &path = CServerPath());
	BOOL Delete(CString FileName, const CServerPath &path = CServerPath());
	BOOL Command(CString commandStr, BOOL bAddToQueue = FALSE);
	BOOL IsConnected();
	void Cancel();
	BOOL Disconnect();
	BOOL IsBusy();
	void ProcessReply(int nReplyCode,LPARAM lParam);
	BOOL Connect(const t_server &server);
	BOOL Chmod(int nValue, CString filename, CServerPath path = CServerPath(), BOOL bAddToQueue = FALSE);
	BOOL IsListInQueue() const;
	
	//Operations
	BOOL List(int nListMode = FZ_LIST_USECACHE, BOOL bAddToQueue = FALSE); //Lists current folder
	BOOL List(CServerPath path, int nListMode = FZ_LIST_USECACHE, BOOL bAddToQueue = FALSE);
	BOOL List(CServerPath parent, CString dirname, int nListMode = FZ_LIST_USECACHE, BOOL bAddToQueue = FALSE);

	BOOL FileTransfer(const t_transferfile TransferFile);

	BOOL IsLocked() const;
	void SetLock(BOOL bLock);

protected:
	void SetWindowText(LPCTSTR lpszWindowText);
	BOOL m_bLocked;
	void OutputRetryMessage();
	t_server *m_pLastServer;
	CServerPath *m_pLastServerPath;
	int m_nRetryCount;
	CRetryServerList m_RetryServerList;
	CFileZillaApi *m_pFileZillaApi;
	std::list<t_command> m_CommandList;
	BOOL bTransferQueueCommand;
};

#endif // !defined(AFX_COMMANDQUEUE_H__022F1125_D783_48B6_A2B8_E623856893DA__INCLUDED_)
