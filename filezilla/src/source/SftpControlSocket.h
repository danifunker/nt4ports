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

// SftpControlSocket.h: Schnittstelle für die Klasse CSFtpControlSocket.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SFTPCONTROLSOCKET_H__173B4310_5E72_4F4B_A4F0_4E3D4CFAF4A5__INCLUDED_)
#define AFX_SFTPCONTROLSOCKET_H__173B4310_5E72_4F4B_A4F0_4E3D4CFAF4A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AsyncSocketEx.h"
#include "ControlSocket.h"
#include "structures.h"	// Hinzugefügt von der Klassenansicht

class CMainThread;
class CAsyncProxySocketLayer;
class CSFtpIpc;
class CSFtpControlSocket : public CControlSocket
{
public:
	BOOL m_bError;
	void OnClientError(int nErrorCode);
	int OnClientRequest(int nId, int &nDataLength, LPVOID pData);
	CSFtpControlSocket(CMainThread *pMainThread);
	virtual ~CSFtpControlSocket();

	virtual void Connect(t_server &server);
	virtual void OnTimer();
	virtual BOOL IsReady();
	virtual void List(BOOL bFinish, int nError = 0, CServerPath path = CServerPath(), CString subdir = "", int nListMode = 0);
	virtual void FtpCommand(LPCTSTR pCommand);
	virtual void Disconnect();
	virtual void FileTransfer(t_transferfile *transferfile = 0, BOOL bFinish = FALSE, int nError = 0);
	virtual void Delete(CString filename, const CServerPath &path);
	virtual void Rename(CString oldName, CString newName, const CServerPath &path, const CServerPath &newPath);
	virtual void MakeDir(const CServerPath &path);
	virtual void RemoveDir(CString dirname, const CServerPath &path);
	virtual void Chmod(CString filename, const CServerPath &path, int nValue);
	
	virtual void ProcessReply();
	virtual void TransferEnd(int nMode);
	virtual void Cancel(BOOL bQuit=FALSE);
	virtual void SetAsyncRequestResult(int nAction, CAsyncRequestData *pData);
	
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);

protected:
	
	void SetFileExistsAction(int nAction, COverwriteRequestData *pData);
	void SetVerifyCertResult( int nResult, t_SslCertData *pData );
	
	void UpdateTransferstatus(int len);
	int CheckOverwriteFile();

	BOOL m_bQuit;
	CStringA m_Reply;
	BOOL m_bCheckForTimeout;
	virtual void DoClose(int nError = 0);
	void ResetOperation(int nSuccessful = -1);
	HANDLE m_hSFtpProcess;
	CSFtpIpc *m_pDataChannel;
	CMainThread *m_pOwner;
	
	t_server m_CurrentServer;
	CString m_ServerName;
	CTime m_LastSendTime;
	CTime m_LastRecvTime;

	class CFileTransferData;
	class CListData;
};

#endif // !defined(AFX_SFTPCONTROLSOCKET_H__173B4310_5E72_4F4B_A4F0_4E3D4CFAF4A5__INCLUDED_)
