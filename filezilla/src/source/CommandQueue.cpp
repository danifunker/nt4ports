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

// CommandQueue.cpp: Implementierung der Klasse CCommandQueue.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommandQueue.h"
#include "mainfrm.h"
#include "queueview.h"
#include "queuectrl.h"
#include "statusview.h"
#include "ftplistctrl.h"
#include "entersomething.h"
#include "version.h"
#include "localview2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCommandQueue::CCommandQueue(CFileZillaApi *pFileZillaApi)
{
	ASSERT(pFileZillaApi);
	m_pFileZillaApi=pFileZillaApi;
	m_nRetryCount=0;
	
	m_pLastServer=0;
	m_pLastServerPath=0;

	m_bLocked=0;
}

CCommandQueue::~CCommandQueue()
{
	delete m_pLastServer;
	delete m_pLastServerPath;
}

BOOL CCommandQueue::Connect(const t_server &server)
{
	m_nRetryCount=0; //Reset retry count

	//Remember the last server
	COptions::SetOption(OPTION_LASTSERVERHOST,server.host);
	COptions::SetOption(OPTION_LASTSERVERPORT,server.port);
	COptions::SetOption(OPTION_LASTSERVERUSER,server.user);
	COptions::SetOption(OPTION_LASTSERVERACCOUNT,server.account);
	COptions::SetOption(OPTION_LASTSERVERDONTREMEMBERPASS,server.bDontRememberPass?1:0);
	COptions::SetOption(OPTION_LASTSERVERUTF8,server.nUTF8);
	COptions::SetOption(OPTION_LASTSERVERNAME,server.name);
	if (!server.bDontRememberPass && !_ttoi(COptions::GetOption(OPTION_RUNINSECUREMODE)))
		COptions::SetOption(OPTION_LASTSERVERPASS,CCrypt::encrypt(server.pass));
	else
		COptions::SetOption(OPTION_LASTSERVERPASS,"");
	COptions::SetOption(OPTION_LASTSERVERFWBYPASS,server.fwbypass);			
	COptions::SetOption(OPTION_LASTSERVERPATH,"");
	if (m_pLastServerPath)
	{
		delete m_pLastServerPath;
		m_pLastServerPath=0;
	}
	COptions::SetOption(OPTION_LASTSERVERTYPE, server.nServerType);
	COptions::SetOption(OPTION_LASTSERVERTRANSFERMODE, server.nPasv);
	COptions::SetOption(OPTION_LASTSERVERTIMEZONE, server.nTimeZoneOffset);
	if (!m_pLastServer)
		m_pLastServer=new t_server;
	*m_pLastServer=server;
	
	t_command command;
	command.id = FZ_COMMAND_CONNECT;
	command.server = server;
	if (!m_CommandList.empty())
	{
		m_CommandList.push_back(command);
		return TRUE;
	}
	else
	{
		int res=m_pFileZillaApi->Connect(server);
		if (res==FZ_REPLY_WOULDBLOCK)
		{
			m_RetryServerList.AddServer(server);
			m_CommandList.push_back(command);
		
			CString title;
			CString str;
			if (server.name!="")
				str=server.name + _T(" (") + server.host + _T(")");
			else
				str=server.host;
			title.Format(IDS_TITLE_CONNECTING, str);
			SetWindowText( _T("FileZilla - ") + title);
			return TRUE;
		}
		else if (res==FZ_REPLY_OK)
		{
			CString title;
			CString str;
			if (server.name!="")
				str=server.name + _T(" (") + server.host + _T(")");
			else
				str=server.host;
			title.Format(IDS_TITLE_CONNECTED, str);
			SetWindowText( _T("FileZilla - ") + title);
			return TRUE;
		}
		else
			return FALSE;
	}
}

void CCommandQueue::ProcessReply(int nReplyCode, LPARAM lParam)
{
	int nOldSize = m_CommandList.size() - 1;
	if (nReplyCode&FZ_REPLY_OK || nReplyCode&FZ_REPLY_ERROR || nReplyCode&FZ_REPLY_CANCEL || nReplyCode&FZ_REPLY_DISCONNECTED)
	{
		int nOldCommand=0;
		CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,AfxGetMainWnd());
		if (!lParam)
		{ //Out of order message, connection may have been closed by remote side
			if (nReplyCode & FZ_REPLY_DISCONNECTED && m_CommandList.empty())
			{
				//Failed to connect, retry if possible
				if (m_nRetryCount < 0)
					m_nRetryCount *= -1;
				m_nRetryCount++;
				if (m_nRetryCount <= COptions::GetOptionVal(OPTION_NUMRETRIES))
				{
					t_server server;
					CServerPath path;
					if (GetLastServer(server, path))
					{
						t_command command;
						command.id = FZ_COMMAND_CONNECT;
						command.server = server;
						m_CommandList.push_front(command);
						List(path, FZ_LIST_USECACHE, TRUE);
						OutputRetryMessage();
						CString title;
						CString str;
						if (server.name!="")
							str = server.name + _T(" (") + server.host + _T(")");
						else
							str = server.host;
						title.Format(IDS_TITLE_CONNECTING, str);
						SetWindowText( _T("FileZilla - ") + title);
					}
				}
				else
					SetWindowText(GetVersionString());
			}
			else if (nReplyCode & FZ_REPLY_DISCONNECTED)
				SetWindowText(GetVersionString());
			return;
		}
		else
			if (!m_CommandList.empty())
			{
				t_command command = m_CommandList.front();
				m_CommandList.pop_front();

				nOldCommand=command.id;
				if (command.id==FZ_COMMAND_FILETRANSFER)
				{
					t_transferfile transferfile=command.transferfile;
					
					//Dispatch result to the command queue
					pMainFrame->GetQueuePane()->GetListCtrl()->ProcessReply(nReplyCode);
					if (transferfile.get)
						pMainFrame->GetLocalPane2()->RefreshFile(transferfile.localfile);
					if (!m_CommandList.empty())
						return;
				}
				else if (command.id==FZ_COMMAND_CONNECT)
				{
					if (nReplyCode&FZ_REPLY_CRITICALERROR)
					{
						if (m_pLastServer)
						{
							if (m_pLastServer->bDontRememberPass)
								m_pLastServer->pass="";
						}
					}
					else if (!(nReplyCode&(FZ_REPLY_OK|FZ_REPLY_CANCEL)))
					{
						//Failed to connect, retry if possible
						if (m_nRetryCount < 0)
							m_nRetryCount *= -1;
						m_nRetryCount++;
						if (m_nRetryCount<=COptions::GetOptionVal(OPTION_NUMRETRIES))
						{
							m_CommandList.push_front(command);
							OutputRetryMessage();
							return;
						}
					}
					if (nReplyCode&FZ_REPLY_OK)
					{
						CString title;
						CString str;
						if (command.server.name!="")
							str=command.server.name + _T(" (") + command.server.host + _T(")");
						else
							str=command.server.host;
						title.Format(IDS_TITLE_CONNECTED, str);
						SetWindowText( _T("FileZilla - ") + title);
					}
				}
				else if (command.id==FZ_COMMAND_LIST)
				{
					if (nReplyCode!=FZ_REPLY_OK)
						((CFtpListCtrl *)pMainFrame->GetFtpPane()->GetListCtrl())->ListComplete(FALSE, command.path, command.param1);
					else
						((CFtpListCtrl *)pMainFrame->GetFtpPane()->GetListCtrl())->ListComplete(TRUE, command.path, command.param1);
				}
			}
		if (nOldSize)
			while (!m_CommandList.empty())
			{
				t_command command = m_CommandList.front();
				int res = m_pFileZillaApi->Command(&command);
				if (res==FZ_REPLY_WOULDBLOCK)
				{
					if (command.id==FZ_COMMAND_CONNECT)
					{
						m_RetryServerList.AddServer(command.server);
						CString title;
						CString str;
						if (command.server.name!="")
							str=command.server.name + _T(" (") + command.server.host + _T(")");
						else
							str=command.server.host;
						title.Format(IDS_TITLE_CONNECTING, str);
						SetWindowText( _T("FileZilla - ") + title);
					}
					return;
				}
				else
				{
					m_CommandList.pop_front();
					if (command.id == FZ_COMMAND_LIST)
					{
						((CFtpListCtrl *)pMainFrame->GetFtpPane()->GetListCtrl())->ListComplete((res == FZ_REPLY_OK)?TRUE:FALSE, command.path, command.param1);
					}
					else if (command.id == FZ_COMMAND_FILETRANSFER)
					{
						pMainFrame->GetQueuePane()->GetListCtrl()->ProcessReply(FZ_REPLY_ERROR);
						break;
					}
				}
			}
		if (m_CommandList.empty())
		{
			pMainFrame->GetQueuePane()->GetListCtrl()->TransferQueue(
				pMainFrame->GetQueuePane()->GetListCtrl()->DoProcessQueue()
				);
		}

		if (m_CommandList.empty() && nOldCommand!=FZ_COMMAND_LIST)
			if (((CFtpListCtrl *)pMainFrame->GetFtpPane()->GetListCtrl())->HasUnsureEntries())
				List(FZ_LIST_USECACHE|FZ_LIST_EXACT);	

		if (!IsConnected() && !IsBusy())
			SetWindowText(GetVersionString());
	}
	else
	{
		CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		CString str;
		str.Format(_T("ProcessReply(%d) unknown reply code"), 1);
		pMainFrame->GetStatusPane()->ShowStatus(str, FZ_LOG_INFO);
	}
}


BOOL CCommandQueue::List(int nListMode /*=FZ_LIST_USECACHE*/, BOOL bAddToQueue /*=FALSE*/) //Lists current folder
{
	t_command command;
	command.id = FZ_COMMAND_LIST;
	command.param4 = nListMode;
	if (nListMode&FZ_LIST_USECACHE && !bAddToQueue)
	{
		int res=m_pFileZillaApi->List(nListMode|(m_CommandList.empty()?0:FZ_LIST_FORCECACHE));
		if (res==FZ_REPLY_OK)
			return TRUE;
		else if (res==FZ_REPLY_WOULDBLOCK)
		{
			m_CommandList.push_back(command);
			return TRUE;
		}
		else
			return FALSE;
	}
	
	if (!m_CommandList.empty())
	{
		if (!bAddToQueue)
			return FALSE;
		m_CommandList.push_back(command);
		return TRUE;
	}
	else
	{
		int res = m_pFileZillaApi->List(nListMode);
		if (res==FZ_REPLY_WOULDBLOCK)
		{
			m_CommandList.push_back(command);
			return TRUE;
		}
		else if (res==FZ_REPLY_OK)
			return TRUE;
		else
			return FALSE;
	}
}

BOOL CCommandQueue::List(CServerPath path, int nListMode /*=FZ_LIST_USECACHE*/, BOOL bAddToQueue /*=FALSE*/)
{
	t_command command;
	command.id = FZ_COMMAND_LIST;
	command.path = path;
	command.param4 = nListMode;
	if (nListMode&FZ_LIST_USECACHE && !bAddToQueue)
	{
		int res = m_pFileZillaApi->List(path,nListMode|(m_CommandList.empty()?0:FZ_LIST_FORCECACHE));
		if (res == FZ_REPLY_OK)
			return TRUE;
		else if (res == FZ_REPLY_WOULDBLOCK)
		{
			m_CommandList.push_back(command);
			return TRUE;
		}
		else
			return FALSE;
	}	
	if (!m_CommandList.empty())
	{
		if (!bAddToQueue)
			return FALSE;
		m_CommandList.push_back(command);
		return TRUE;
	}
	else
	{
		int res=m_pFileZillaApi->List(path,nListMode);
		if (res==FZ_REPLY_WOULDBLOCK)
		{
			m_CommandList.push_back(command);
			return TRUE;
		}
		else if (res==FZ_REPLY_OK)
			return TRUE;
		else
			return FALSE;
	}
}

int CCommandQueue::List(CServerPath parent, CString dirname, int nListMode /*=FZ_LIST_USECACHE*/, BOOL bAddToQueue /*=FALSE*/)
{
	t_command command;
	command.id = FZ_COMMAND_LIST;
	command.path = parent;
	command.param1 = dirname;
	command.param4 = nListMode;	
	if (nListMode&FZ_LIST_USECACHE && !bAddToQueue)
	{
		int res = m_pFileZillaApi->List(parent, dirname, nListMode|(m_CommandList.empty()?0:FZ_LIST_FORCECACHE));
		if (res==FZ_REPLY_OK)
			return TRUE;
		else if (res==FZ_REPLY_WOULDBLOCK)
		{
			m_CommandList.push_back(command);
			return TRUE;
		}
		else
			return FALSE;
	}
	
	if (!m_CommandList.empty())
	{
		if (!bAddToQueue)
			return FALSE;
		m_CommandList.push_back(command);
		return TRUE;
	}
	else
	{
		int res=m_pFileZillaApi->List(parent, dirname, nListMode);
		if (res==FZ_REPLY_WOULDBLOCK)
		{
			m_CommandList.push_back(command);
			return TRUE;
		}
		else if (res==FZ_REPLY_OK)
			return TRUE;
		else
			return FALSE;
	}
}

BOOL CCommandQueue::IsBusy()
{
	if (m_CommandList.empty())
		return FALSE;
	return TRUE;
}

BOOL CCommandQueue::FileTransfer(const t_transferfile TransferFile)
{
	t_command command;
	command.id=FZ_COMMAND_FILETRANSFER;
	command.transferfile = TransferFile;
	if (!m_CommandList.empty())
	{
		m_CommandList.push_back(command);
		return TRUE;
	}
	else
	{
		t_server server;
		int res=m_pFileZillaApi->GetCurrentServer(server);
		if (!IsConnected() || res!=FZ_REPLY_OK)
		{
			BOOL res=Connect(TransferFile.server);
			if (res)
			{
				m_CommandList.push_back(command);
				return TRUE;
			}
			else
				return FALSE;
		}
		else if (server!=TransferFile.server)
		{
			Disconnect();
			BOOL res=Connect(TransferFile.server);
			if (res)
			{
				m_CommandList.push_back(command);
				return TRUE;
			}
			else
				return FALSE;
		}
		else
		{
			BOOL res = m_pFileZillaApi->FileTransfer(TransferFile);
			if (res==FZ_REPLY_WOULDBLOCK)
			{
				m_CommandList.push_back(command);
				return TRUE;
			}
			else if (res==FZ_REPLY_OK)
				return TRUE;
			else
				return FALSE;
		}
	}
}

void CCommandQueue::Cancel()
{
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,AfxGetMainWnd());

	bool didWaitForRetry = false;
	if (m_nRetryCount > 0)
	{
		didWaitForRetry = true;
		CString str;
		if (!m_CommandList.empty() && m_CommandList.front().id == FZ_COMMAND_CONNECT)
		{
			str.LoadString(IDS_ERRORMSG_CANTCONNECT);
			pMainFrame->GetStatusPane()->ShowStatus(str, FZ_LOG_ERROR);
		}
		str.LoadString(IDS_ERRORMSG_INTERRUPTED);
		pMainFrame->GetStatusPane()->ShowStatus(str, FZ_LOG_ERROR);
		m_nRetryCount = 0;
	}
	else
	{
		m_pFileZillaApi->Cancel();
	}

	if (m_CommandList.empty())
		return;


	t_command firstCommand = m_CommandList.front();
	m_CommandList.pop_front();
	while (!m_CommandList.empty())
	{
		t_command command = m_CommandList.front();
		m_CommandList.pop_front();
		if (command.id == FZ_COMMAND_FILETRANSFER)
			pMainFrame->GetQueuePane()->GetListCtrl()->ProcessReply(FZ_REPLY_ERROR|FZ_REPLY_CANCEL);
		else if (command.id == FZ_COMMAND_LIST)
			((CFtpListCtrl *)pMainFrame->GetFtpPane()->GetListCtrl())->ListComplete(FALSE, command.path, command.param1);
	}

	if (!didWaitForRetry)
		m_CommandList.push_back(firstCommand);
}


BOOL CCommandQueue::Disconnect()
{
	t_command command;
	command.id = FZ_COMMAND_DISCONNECT;
	if (!m_CommandList.empty())
	{
		m_CommandList.push_back(command);
		return TRUE;
	}
	else
	{
		if (m_pFileZillaApi->IsConnected()==FZ_REPLY_NOTCONNECTED)
			return TRUE;
		int res=m_pFileZillaApi->Disconnect();
		if (res==FZ_REPLY_WOULDBLOCK)
		{
			m_CommandList.push_back(command);
			return TRUE;
		}
		else if (res==FZ_REPLY_NOTCONNECTED)
			return TRUE;
		else
			return FALSE;
	}
}

BOOL CCommandQueue::IsConnected()
{
	return m_pFileZillaApi->IsConnected()==FZ_REPLY_OK;
}

BOOL CCommandQueue::Command(CString commandStr, BOOL bAddToQueue /*=FALSE*/)
{
	t_command command;
	command.id = FZ_COMMAND_CUSTOMCOMMAND;
	command.param1 = commandStr;
	if (!m_CommandList.empty() && !bAddToQueue) //Don't queue direct commands
		return FALSE;
	else if (!m_CommandList.empty())
	{
		m_CommandList.push_back(command);
		return TRUE;
	}
	else
	{
		int res=m_pFileZillaApi->CustomCommand(commandStr);
		if (res==FZ_REPLY_WOULDBLOCK)
		{
			m_CommandList.push_back(command);
			return TRUE;
		}
		else if (res==FZ_REPLY_OK)
			return TRUE;
		else
			return FALSE;
	}
}

BOOL CCommandQueue::Delete(CString FileName, const CServerPath &path /*=CServerPath()*/)
{
	t_command command;
	command.id = FZ_COMMAND_DELETE;
	command.param1 = FileName;
	command.path = path;
	if (!m_CommandList.empty())
	{
		m_CommandList.push_back(command);
		return TRUE;
	}
	else
	{
		int res = m_pFileZillaApi->Delete(FileName, path);
		if (res==FZ_REPLY_WOULDBLOCK)
		{
			m_CommandList.push_back(command);
			return TRUE;
		}
		else if (res==FZ_REPLY_OK)
			return TRUE;
		else
			return FALSE;
	}
}

BOOL CCommandQueue::RemoveDir(CString DirName, const CServerPath &path /*=CServerPath()*/)
{
	t_command command;
	command.id = FZ_COMMAND_REMOVEDIR;
	command.param1 = DirName;
	command.path = path;
	if (!m_CommandList.empty())
	{
		m_CommandList.push_back(command);
		return TRUE;
	}
	else
	{
		int res=m_pFileZillaApi->RemoveDir(DirName, path);
		if (res==FZ_REPLY_WOULDBLOCK)
		{
			m_CommandList.push_back(command);
			return TRUE;
		}
		else if (res==FZ_REPLY_OK)
			return TRUE;
		else
			return FALSE;
	}
}

BOOL CCommandQueue::MakeDir(const CServerPath &path /*=CServerPath()*/)
{
	t_command command;
	command.id = FZ_COMMAND_MAKEDIR;
	command.path = path;
	if (!m_CommandList.empty())
	{
		m_CommandList.push_back(command);
		return TRUE;
	}
	else
	{
		int res = m_pFileZillaApi->MakeDir(path);
		if (res == FZ_REPLY_WOULDBLOCK)
		{
			m_CommandList.push_back(command);
			return TRUE;
		}
		else if (res == FZ_REPLY_OK)
			return TRUE;
		else
			return FALSE;
	}
}

BOOL CCommandQueue::Rename(CString oldName, CString newName, const CServerPath &path /*=CServerPath()*/, const CServerPath &newPath /*=CServerPath()*/)
{
	t_command command;
	command.id = FZ_COMMAND_RENAME;
	command.param1 = oldName;
	command.param2 = newName;
	command.path = path;
	command.newPath = newPath;
	if (!m_CommandList.empty())
	{
		m_CommandList.push_back(command);
		return TRUE;
	}
	else
	{
		int res=m_pFileZillaApi->Rename(oldName, newName, path, newPath);
		if (res==FZ_REPLY_WOULDBLOCK)
		{
			m_CommandList.push_back(command);
			return TRUE;
		}
		else if (res==FZ_REPLY_OK)
			return TRUE;
		else
			return FALSE;
	}
}

void CCommandQueue::OnRetryTimer()
{
	if (m_nRetryCount > 0 && m_nRetryCount <= COptions::GetOptionVal(OPTION_NUMRETRIES))
	{
		if (m_CommandList.empty())
		{
			m_nRetryCount = 0;
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			pMainFrame->GetStatusPane()->ShowStatus("No command to retry", FZ_LOG_INFO);
			return;
		}
		else if (!m_RetryServerList.StillWait(m_CommandList.front().server))
		{
			m_nRetryCount *= -1;
			int res = m_pFileZillaApi->Connect(m_CommandList.front().server);
			if (res==FZ_REPLY_WOULDBLOCK)
			{
				m_RetryServerList.AddServer(m_CommandList.front().server);
				return;
			}
			else if (res == FZ_REPLY_OK)
			{
				CString title;
				CString str;
				if (m_CommandList.front().server.name!="")
					str = m_CommandList.front().server.name + _T(" (") + m_CommandList.front().server.host + _T(")");
				else
					str = m_CommandList.front().server.host;
				title.Format(IDS_TITLE_CONNECTED, str);
				SetWindowText( _T("FileZilla - ") + title);
				return;
			}
			else
			{
				m_CommandList.pop_front();
				
				while (!m_CommandList.empty())
				{
					t_command command = m_CommandList.front();
					int res = m_pFileZillaApi->Command(&command);
					if (res==FZ_REPLY_WOULDBLOCK)
					{
						if (command.id==FZ_COMMAND_CONNECT)
						{
							m_RetryServerList.AddServer(command.server);
							CString title;
							CString str;
							if (command.server.name!="")
								str=command.server.name + _T(" (") + command.server.host + _T(")");
							else
								str=command.server.host;
							title.Format(IDS_TITLE_CONNECTED, str);
							SetWindowText( _T("FileZilla - ") + title);							
						}
						return;
					}
					else
					{
						m_CommandList.pop_front();
						CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
						if (command.id == FZ_COMMAND_FILETRANSFER)
						{
							pMainFrame->GetQueuePane()->GetListCtrl()->ProcessReply(FZ_REPLY_ERROR);
							break;
						}
						else if (command.id==FZ_COMMAND_LIST)
						{
							if (res != FZ_REPLY_OK)
								((CFtpListCtrl *)pMainFrame->GetFtpPane()->GetListCtrl())->ListComplete(FALSE, command.path, command.param1);
							else
								((CFtpListCtrl *)pMainFrame->GetFtpPane()->GetListCtrl())->ListComplete(TRUE, command.path, command.param1);
						}
					}
				}
				if (!IsConnected() && !IsBusy())
					SetWindowText(GetVersionString());
			}
		}
	}
}

BOOL CCommandQueue::GetLastServer(t_server &server, CServerPath &path)
{
	t_server server2;
	BOOL bAskForPassword = FALSE;
	if (m_pLastServer)
		server2 = *m_pLastServer;
	else
	{
		server2.host=COptions::GetOption(OPTION_LASTSERVERHOST);
		server2.port=COptions::GetOptionVal(OPTION_LASTSERVERPORT);
		server2.user=COptions::GetOption(OPTION_LASTSERVERUSER);
		server2.account = COptions::GetOption(OPTION_LASTSERVERACCOUNT);
		server2.nUTF8=COptions::GetOptionVal(OPTION_LASTSERVERUTF8);
		server2.bDontRememberPass=COptions::GetOptionVal(OPTION_LASTSERVERDONTREMEMBERPASS);
		if (!server2.bDontRememberPass)
		{
			if (_ttoi(COptions::GetOption(OPTION_RUNINSECUREMODE)))
				bAskForPassword=TRUE;
			else
				server2.pass=CCrypt::decrypt(COptions::GetOption(OPTION_LASTSERVERPASS));
		}
		server2.fwbypass=COptions::GetOptionVal(OPTION_LASTSERVERFWBYPASS);
		server2.nServerType=COptions::GetOptionVal(OPTION_LASTSERVERTYPE);
		server2.name=COptions::GetOption(OPTION_LASTSERVERNAME);
		server2.nTimeZoneOffset = COptions::GetOptionVal(OPTION_LASTSERVERTIMEZONE);
		
		if (server2.host=="")
			return FALSE;
		if (server2.port<1 || server2.port>65535)
			return FALSE;	
	}
	BOOL bUseGSS = FALSE;
	if (COptions::GetOptionVal(OPTION_USEGSS))
	{
		USES_CONVERSION;
		
		CString GssServers = COptions::GetOption(OPTION_GSSSERVERS);
		hostent *fullname = gethostbyname(T2CA(server2.host));
		CString host;
		if (fullname)
			host = fullname->h_name;
		else
			host = server2.host;
		host.MakeLower();
		int i;
		while ((i=GssServers.Find( _T(";") ))!=-1)
		{
			if (("."+GssServers.Left(i))==host.Right(GssServers.Left(i).GetLength()+1) || GssServers.Left(i)==host)
			{
				bUseGSS=TRUE;
				break;
			}
			GssServers=GssServers.Mid(i+1);
		}
	}
	if (server2.bDontRememberPass && server2.pass=="")
		bAskForPassword = TRUE;
	if (bAskForPassword && !bUseGSS)
	{
		CEnterSomething dlg(IDS_INPUTDIALOGTITLE_INPUTPASSWORD,IDS_INPUTDIALOGTEXT_INPUTPASSWORD,'*');
		if (dlg.DoModal()==IDOK)
			server2.pass=dlg.m_String;
		else
			return FALSE;
	}	
	server = server2;

	CServerPath path2;
	
	if (m_pLastServerPath)
		path2=*m_pLastServerPath;
	else
	{
		if (!path2.SetSafePath(COptions::GetOption(OPTION_LASTSERVERPATH)))
			path2=CServerPath();
	}

	server.nPasv = COptions::GetOptionVal(OPTION_LASTSERVERTRANSFERMODE);

	path = path2;
	return TRUE;
}

void CCommandQueue::OutputRetryMessage()
{
	if (m_CommandList.empty())
		return;
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,AfxGetMainWnd());
	CString status;
	
	if (m_nRetryCount<COptions::GetOptionVal(OPTION_NUMRETRIES))
		status.Format(IDS_STATUSMSG_WAITINGTORETRY_LEFT,COptions::GetOptionVal(OPTION_NUMRETRIES)-m_nRetryCount+1);
	else
		status.Format(IDS_STATUSMSG_WAITINGTORETRY);
	pMainFrame->GetStatusPane()->ShowStatus(status,0);
}

void CCommandQueue::SetLastServerPath(const CServerPath &path)
{
	if (!m_pLastServerPath)
		m_pLastServerPath=new CServerPath;
	*m_pLastServerPath=path;
}

BOOL CCommandQueue::Chmod(int nValue, CString filename, CServerPath path /*=CServerPath()*/, BOOL bAddToQueue /*=FALSE*/)
{
	if (!m_CommandList.empty() && !bAddToQueue) //Don't queue direct commands
		return FALSE;
	
	t_command command;
	command.id = FZ_COMMAND_CHMOD;
	command.param1 = filename;
	command.path = path;
	command.param4 = nValue;
	if (!m_CommandList.empty())
	{
		m_CommandList.push_back(command);
		return TRUE;
	}
	else
	{
		int res=m_pFileZillaApi->Chmod(nValue, filename, path);
		if (res==FZ_REPLY_WOULDBLOCK)
		{
			m_CommandList.push_back(command);
			return TRUE;
		}
		else if (res == FZ_REPLY_OK)
			return TRUE;
		else
			return FALSE;
	}
}

BOOL CCommandQueue::IsLocked() const
{
	return m_bLocked;
}

void CCommandQueue::SetLock(BOOL bLock)
{
	m_bLocked=bLock;
}

void CCommandQueue::SetWindowText(LPCTSTR lpszWindowText)
{
	CMainFrame *pMainFrame = reinterpret_cast<CMainFrame *>(AfxGetMainWnd());
	pMainFrame->SetRedraw(FALSE);
	pMainFrame->SetWindowText(lpszWindowText);
	pMainFrame->SetRedraw(TRUE);
	pMainFrame->RedrawWindow(0, 0, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
}

BOOL CCommandQueue::IsListInQueue() const
{
	for (std::list<t_command>::const_iterator iter = m_CommandList.begin(); iter != m_CommandList.end(); iter++)
		if (iter->id == FZ_COMMAND_LIST)
			return TRUE;

	return FALSE;
}