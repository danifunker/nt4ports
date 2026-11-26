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

// FileZillaApi.cpp: Implementierung der Klasse CFileZillaApi.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileZillaApi.h"
#include "mainthread.h"
#include "directorycache.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CFileZillaApi::CFileZillaApi()
{
	m_hOwnerWnd=0;
	//Create Message IDs
	m_nReplyMessageID=RegisterWindowMessage( _T("FileZillaApiReplyMessage{8EF2E328-685E-4815-A9B9-823512F8381D}") );
	m_nInternalMessageID=0;
	m_pMainThread=0;
	m_bInitialized=FALSE;
}

CFileZillaApi::~CFileZillaApi()
{
	Destroy();
}

int CFileZillaApi::Init(HWND hOwnerWnd, int nReplyMessageID /*=0*/)
{
	//Check parameters
	//-> No check needed, if hOwnerWnd is NULL, use blocking mode and don't send status messages

	//Check if call allowed
	if (m_bInitialized)
		return FZ_REPLY_ALREADYINIZIALIZED;

	//Initialize variables
	if (nReplyMessageID)
		m_nReplyMessageID=nReplyMessageID;
	m_hOwnerWnd=hOwnerWnd;
	m_nInternalMessageID=RegisterWindowMessage( _T("FileZillaInternalApiMessage{F958620E-040C-4b33-A091-7E04E10AA660}") );
	if (!m_nInternalMessageID)
		return FZ_REPLY_NOTINITIALIZED;
	
	//Create thread object
	m_pMainThread = CMainThread::Create(THREAD_PRIORITY_BELOW_NORMAL, CREATE_SUSPENDED);

	//Initialize Thread variables
	m_pMainThread->m_nInternalMessageID=m_nInternalMessageID;
	m_pMainThread->m_nReplyMessageID=m_nReplyMessageID;
	m_pMainThread->m_hOwnerWnd=m_hOwnerWnd;

	m_pMainThread->InitLog(m_hOwnerWnd, m_nReplyMessageID);
	
	//Resume Thread
	m_pMainThread->ResumeThread();

	//Initialization OK
	m_bInitialized=TRUE;
	return FZ_REPLY_OK;
}

unsigned int CFileZillaApi::GetMessageID()
{
	return m_nReplyMessageID;
}

int CFileZillaApi::IsConnected()
{
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	return m_pMainThread->IsConnected()?FZ_REPLY_OK:FZ_REPLY_NOTCONNECTED;
}

int CFileZillaApi::IsBusy()
{
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	return m_pMainThread->IsBusy()?FZ_REPLY_BUSY:FZ_REPLY_IDLE;
}

int CFileZillaApi::Connect(const t_server &server)
{
	//Check parameters
	if (server.host=="" || server.port<1 || server.port>65535)
		return FZ_REPLY_INVALIDPARAM;
	
	BOOL bUseGSS = FALSE;
	if (COptions::GetOptionVal(OPTION_USEGSS))
	{
		USES_CONVERSION;
		
		CString GssServers = COptions::GetOption(OPTION_GSSSERVERS);
		hostent *fullname = gethostbyname(T2CA(server.host));
		CString host;
		if (fullname)
			host = fullname->h_name;
		else
			host = server.host;
		host.MakeLower();
		int i;
		while ((i=GssServers.Find( _T(";") ))!=-1)
		{
			if (("."+GssServers.Left(i))==host.Right(GssServers.Left(i).GetLength()+1) || GssServers.Left(i)==host)
			{
				bUseGSS = TRUE;
				break;
			}
			GssServers = GssServers.Mid(i+1);
		}
	}
	if (!bUseGSS && server.user == "")
		return FZ_REPLY_INVALIDPARAM;
		
	if (!(server.nServerType&FZ_SERVERTYPE_HIGHMASK))
		return FZ_REPLY_INVALIDPARAM;

	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (m_pMainThread->IsBusy())
		return FZ_REPLY_BUSY;

	t_command command;
	command.id=FZ_COMMAND_CONNECT;
	command.server=server;
	m_pMainThread->Command(command);
	if (m_hOwnerWnd)
		return FZ_REPLY_WOULDBLOCK;
	else
		return m_pMainThread->LastOperationSuccessful()?FZ_REPLY_OK:FZ_REPLY_ERROR;
}

int CFileZillaApi::List(int nListMode /*=FZ_LIST_USECACHE*/)
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (IsConnected()==FZ_REPLY_NOTCONNECTED)
		return FZ_REPLY_NOTCONNECTED;
	if (nListMode&FZ_LIST_REALCHANGE)
		return FZ_REPLY_INVALIDPARAM;
	if (nListMode&FZ_LIST_FORCECACHE)
		nListMode|=FZ_LIST_USECACHE;
	//Check if current dir is cached
	CServerPath path;
	if (nListMode&FZ_LIST_USECACHE)
	{
		if (!m_pMainThread->GetWorkingDirPath(path) || path.IsEmpty())
			m_pMainThread->GetCurrentPath(path);
		if (!path.IsEmpty())
		{
			t_server server;
			BOOL res=m_pMainThread->GetCurrentServer(server);
			if (res)
			{
				t_directory *directory=new t_directory;
				CDirectoryCache cache;
				res=cache.Lookup(path,server,*directory);
				if (res)
				{
					BOOL bExact=TRUE;
					if (nListMode & FZ_LIST_EXACT)
						for (int i=0;i<directory->num;i++)
							if (directory->direntry[i].bUnsure || (directory->direntry[i].size==-1 && !directory->direntry[i].dir))
							{
								bExact=FALSE;
								break;
							}
					if (bExact)
					{
						m_pMainThread->SetWorkingDir(directory);
						delete directory;
						return FZ_REPLY_OK;
					}
				}
				delete directory;
			}
		}
	}

	if (m_pMainThread->IsBusy())
		return FZ_REPLY_BUSY;
	if (nListMode&FZ_LIST_FORCECACHE)
		return FZ_REPLY_ERROR;

	t_command command;
	command.id=FZ_COMMAND_LIST;
	command.path = path;
	command.param4=nListMode;
	m_pMainThread->Command(command);
	if (m_hOwnerWnd)
		return FZ_REPLY_WOULDBLOCK;
	else
		return m_pMainThread->LastOperationSuccessful()?FZ_REPLY_OK:FZ_REPLY_ERROR;
}

int CFileZillaApi::Cancel()
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (IsBusy()!=FZ_REPLY_BUSY)
		return FZ_REPLY_NOTBUSY;
	m_pMainThread->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_CANCEL, 0);
	return FZ_REPLY_WOULDBLOCK;
}

void CFileZillaApi::Destroy()
{
	if (!m_bInitialized)
		return;
	ASSERT(m_pMainThread);
	HANDLE tmp=m_pMainThread->m_hThread;
	m_pMainThread->Quit();
	//Wait for the main thread to quit
	WaitForSingleObject(tmp, 10000);
	PostMessage(m_hOwnerWnd, m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_QUITCOMPLETE, 0), 0);
		
	m_pMainThread=0;
	m_bInitialized=FALSE;
}

int CFileZillaApi::Disconnect()
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (IsConnected()==FZ_REPLY_NOTCONNECTED)
		return FZ_REPLY_NOTCONNECTED;
	if (IsBusy()==FZ_REPLY_BUSY)
		return FZ_REPLY_BUSY;
	
	m_pMainThread->PostThreadMessage(m_nInternalMessageID,FZAPI_THREADMSG_DISCONNECT,0);
	return FZ_REPLY_WOULDBLOCK;
}

int CFileZillaApi::Command(t_command *pCommand)
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;

	//Dispatch command to command specific functions
	switch(pCommand->id)
	{
	case FZ_COMMAND_LIST:
		if (pCommand->param1!="")
			return List(pCommand->path,pCommand->param1,pCommand->param4);
		else if (!pCommand->path.IsEmpty())
			return List(pCommand->path,pCommand->param4);
		else
			return List(pCommand->param4);
		break;
	case FZ_COMMAND_CONNECT:
		return Connect(pCommand->server);
		break;
	case FZ_COMMAND_DISCONNECT:
		return Disconnect();
		break;
	case FZ_COMMAND_FILETRANSFER:
		return FileTransfer(pCommand->transferfile);
		break;
	case FZ_COMMAND_DELETE:
		return Delete(pCommand->param1, pCommand->path);
		break;
	case FZ_COMMAND_REMOVEDIR:
		return RemoveDir(pCommand->param1, pCommand->path);
		break;
	case FZ_COMMAND_MAKEDIR:
		return MakeDir(pCommand->path);
		break;
	case FZ_COMMAND_RENAME:
		return Rename(pCommand->param1, pCommand->param2, pCommand->path, pCommand->newPath);
		break;
	case FZ_COMMAND_CUSTOMCOMMAND:
		return CustomCommand(pCommand->param1);
		break;
	case FZ_COMMAND_CHMOD:
		return Chmod(pCommand->param4, pCommand->param1, pCommand->path);
		break;
	}
	return FZ_REPLY_INVALIDPARAM;
}

int CFileZillaApi::List(const CServerPath& path, int nListMode /*=FZ_LIST_USECACHE*/)
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (IsConnected()==FZ_REPLY_NOTCONNECTED)
		return FZ_REPLY_NOTCONNECTED;
	if ( (nListMode&(FZ_LIST_FORCECACHE|FZ_LIST_REALCHANGE))==(FZ_LIST_FORCECACHE|FZ_LIST_REALCHANGE) )
		return FZ_REPLY_INVALIDPARAM;
	if (nListMode&FZ_LIST_FORCECACHE)
		nListMode|=FZ_LIST_USECACHE;
	if (path.IsEmpty())
		return FZ_REPLY_INVALIDPARAM;
	
	//Check if current dir is cached
	if (nListMode&FZ_LIST_USECACHE && !(nListMode&FZ_LIST_REALCHANGE))
	{
		t_server server;
		BOOL res=m_pMainThread->GetCurrentServer(server);
		if (res)
		{
			t_directory *directory=new t_directory;
			CDirectoryCache cache;
			res=cache.Lookup(path,server,*directory);
			if (res)
			{
				BOOL bExact=TRUE;
				if (nListMode & FZ_LIST_EXACT)
					for (int i=0;i<directory->num;i++)
						if (directory->direntry[i].bUnsure || (directory->direntry[i].size==-1 && !directory->direntry[i].dir))
						{
							bExact=FALSE;
							break;
						}
				if (bExact)
				{
					m_pMainThread->SetWorkingDir(directory);
					delete directory;
					return FZ_REPLY_OK;
				}
			}
			delete directory;
		}
	}

	if (m_pMainThread->IsBusy())
		return FZ_REPLY_BUSY;
	if (nListMode&FZ_LIST_FORCECACHE)
		return FZ_REPLY_ERROR;

	t_command command;
	command.id=FZ_COMMAND_LIST;
	command.path=path;
	command.param4=nListMode;
	m_pMainThread->Command(command);
	if (m_hOwnerWnd)
		return FZ_REPLY_WOULDBLOCK;
	else
		return m_pMainThread->LastOperationSuccessful()?FZ_REPLY_OK:FZ_REPLY_ERROR;
}

int CFileZillaApi::List(const CServerPath& parent, CString dirname, int nListMode /*=FZ_LIST_USECACHE*/)
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (IsConnected()==FZ_REPLY_NOTCONNECTED)
		return FZ_REPLY_NOTCONNECTED;
	if ( (nListMode&(FZ_LIST_FORCECACHE|FZ_LIST_REALCHANGE))==(FZ_LIST_FORCECACHE|FZ_LIST_REALCHANGE) )
		return FZ_REPLY_INVALIDPARAM;
	if (nListMode&FZ_LIST_FORCECACHE)
		nListMode|=FZ_LIST_USECACHE;
	if (dirname=="" || parent.IsEmpty())
		return FZ_REPLY_INVALIDPARAM;

	//Check if current dir is cached
	if (nListMode&FZ_LIST_USECACHE && !(nListMode&FZ_LIST_REALCHANGE))
	{
		t_server server;
		BOOL res=m_pMainThread->GetCurrentServer(server);
		if (res)
		{
			t_directory *directory=new t_directory;
			CDirectoryCache cache;
			res=cache.Lookup(parent,dirname,server,*directory);
			if (res)
			{
				BOOL bExact=TRUE;
				if (nListMode & FZ_LIST_EXACT)
					for (int i=0;i<directory->num;i++)
						if (directory->direntry[i].bUnsure || (directory->direntry[i].size==-1 && !directory->direntry[i].dir))
						{
							bExact=FALSE;
							break;
						}
				if (bExact)
				{
					m_pMainThread->SetWorkingDir(directory);
					delete directory;
					return FZ_REPLY_OK;
				}
			}
			delete directory;
		}
	}

	if (m_pMainThread->IsBusy())
		return FZ_REPLY_BUSY;
	if (nListMode&FZ_LIST_FORCECACHE)
		return FZ_REPLY_ERROR;

	t_command command;
	command.id=FZ_COMMAND_LIST;
	command.path=parent;
	command.param1=dirname;
	command.param4=nListMode;
	m_pMainThread->Command(command);
	if (m_hOwnerWnd)
		return FZ_REPLY_WOULDBLOCK;
	else
		return m_pMainThread->LastOperationSuccessful()?FZ_REPLY_OK:FZ_REPLY_ERROR;
}

int CFileZillaApi::FileTransfer(const t_transferfile &TransferFile)
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (IsConnected()==FZ_REPLY_NOTCONNECTED)
		return FZ_REPLY_NOTCONNECTED;
	if (TransferFile.remotefile=="" || TransferFile.localfile=="" || TransferFile.remotepath.IsEmpty())
		return FZ_REPLY_INVALIDPARAM;
	if (IsBusy()==FZ_REPLY_BUSY)
		return FZ_REPLY_BUSY;
	t_command command;
	command.id=FZ_COMMAND_FILETRANSFER;
	command.transferfile=TransferFile;
	m_pMainThread->Command(command);
	if (m_hOwnerWnd)
		return FZ_REPLY_WOULDBLOCK;
	else
		return m_pMainThread->LastOperationSuccessful()?FZ_REPLY_OK:FZ_REPLY_ERROR;
}

int CFileZillaApi::GetCurrentServer(t_server &server)
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (IsConnected()==FZ_REPLY_NOTCONNECTED)
		return FZ_REPLY_NOTCONNECTED;
	if (m_pMainThread->GetCurrentServer(server))
		return FZ_REPLY_OK;
	else
		return FZ_REPLY_NOTCONNECTED;
}

int CFileZillaApi::CustomCommand(CString CustomCommand)
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (IsConnected()==FZ_REPLY_NOTCONNECTED)
		return FZ_REPLY_NOTCONNECTED;
	if (IsBusy()==FZ_REPLY_BUSY)
		return FZ_REPLY_BUSY;
	t_server server;
	int res=GetCurrentServer(server);
	if (res!=FZ_REPLY_OK)
		return res;
	if (server.nServerType&FZ_SERVERTYPE_SUB_FTP_SFTP)
		return FZ_REPLY_NOTSUPPORTED;
	if (CustomCommand=="")
		return FZ_REPLY_INVALIDPARAM;

	t_command command;
	command.id=FZ_COMMAND_CUSTOMCOMMAND;
	command.param1=CustomCommand;
	m_pMainThread->Command(command);
	if (m_hOwnerWnd)
		return FZ_REPLY_WOULDBLOCK;
	else
		return m_pMainThread->LastOperationSuccessful()?FZ_REPLY_OK:FZ_REPLY_ERROR;
}

int CFileZillaApi::Delete(CString FileName, const CServerPath &path /*=CServerPath()*/)
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (IsConnected()==FZ_REPLY_NOTCONNECTED)
		return FZ_REPLY_NOTCONNECTED;
	if (IsBusy()==FZ_REPLY_BUSY)
		return FZ_REPLY_BUSY;
	if (FileName=="")
		return FZ_REPLY_INVALIDPARAM;

	CServerPath path2=path;
	if (path2.IsEmpty())
	{
		m_pMainThread->GetCurrentPath(path2);
		if (path2.IsEmpty())
			return FZ_REPLY_INVALIDPARAM;
	}

	t_command command;
	command.id=FZ_COMMAND_DELETE;
	command.param1=FileName;
	command.path=path2;
	m_pMainThread->Command(command);
	if (m_hOwnerWnd)
		return FZ_REPLY_WOULDBLOCK;
	else
		return m_pMainThread->LastOperationSuccessful()?FZ_REPLY_OK:FZ_REPLY_ERROR;

}

int CFileZillaApi::RemoveDir(CString DirName, const CServerPath &path /*=CServerPath()*/)
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (IsConnected()==FZ_REPLY_NOTCONNECTED)
		return FZ_REPLY_NOTCONNECTED;
	if (IsBusy()==FZ_REPLY_BUSY)
		return FZ_REPLY_BUSY;
	if (DirName=="")
		return FZ_REPLY_INVALIDPARAM;

	CServerPath path2=path;
	if (path2.IsEmpty())
	{
		m_pMainThread->GetCurrentPath(path2);
		if (path2.IsEmpty())
			return FZ_REPLY_INVALIDPARAM;
	}

	t_command command;
	command.id=FZ_COMMAND_REMOVEDIR;
	command.param1=DirName;
	command.path=path2;
	m_pMainThread->Command(command);
	if (m_hOwnerWnd)
		return FZ_REPLY_WOULDBLOCK;
	else
		return m_pMainThread->LastOperationSuccessful()?FZ_REPLY_OK:FZ_REPLY_ERROR;
	
	return FZ_REPLY_ERROR;
}

int CFileZillaApi::MakeDir(const CServerPath &path)
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (IsConnected()==FZ_REPLY_NOTCONNECTED)
		return FZ_REPLY_NOTCONNECTED;
	if (IsBusy()==FZ_REPLY_BUSY)
		return FZ_REPLY_BUSY;
	if (path.IsEmpty() || !path.HasParent())
		return FZ_REPLY_INVALIDPARAM;

	t_command command;
	command.id=FZ_COMMAND_MAKEDIR;
	command.path=path;
	m_pMainThread->Command(command);
	if (m_hOwnerWnd)
		return FZ_REPLY_WOULDBLOCK;
	else
		return m_pMainThread->LastOperationSuccessful()?FZ_REPLY_OK:FZ_REPLY_ERROR;
	
	return FZ_REPLY_ERROR;
}

int CFileZillaApi::Rename(CString oldName, CString newName, const CServerPath &path /*=CServerPath()*/, const CServerPath &newPath /*=CServerPath()*/)
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (IsConnected()==FZ_REPLY_NOTCONNECTED)
		return FZ_REPLY_NOTCONNECTED;
	if (IsBusy()==FZ_REPLY_BUSY)
		return FZ_REPLY_BUSY;
	if (oldName=="" || newName=="")
		return FZ_REPLY_INVALIDPARAM;

	CServerPath path2 = path;
	if (path2.IsEmpty())
	{
		m_pMainThread->GetCurrentPath(path2);
		if (path2.IsEmpty())
			return FZ_REPLY_INVALIDPARAM;
	}

	t_command command;
	command.id = FZ_COMMAND_RENAME;
	command.param1 = oldName;
	command.param2 = newName;
	command.path = path2;
	command.newPath = newPath;
	m_pMainThread->Command(command);
	if (m_hOwnerWnd)
		return FZ_REPLY_WOULDBLOCK;
	else
		return m_pMainThread->LastOperationSuccessful()?FZ_REPLY_OK:FZ_REPLY_ERROR;
	
	return FZ_REPLY_ERROR;
}

int CFileZillaApi::SetAsyncRequestResult(int nAction, CAsyncRequestData *pData)
{
	if (!this || !pData)
		return FZ_REPLY_CRITICALERROR | FZ_REPLY_INVALIDPARAM;

	if (IsBadWritePtr(pData, sizeof(CAsyncRequestData)))
		return FZ_REPLY_CRITICALERROR;
	
	if (!m_bInitialized)
	{
		delete pData;
		return FZ_REPLY_NOTINITIALIZED;
	}
	if (IsConnected()==FZ_REPLY_NOTCONNECTED)
	{
		delete pData;
		return FZ_REPLY_NOTCONNECTED;
	}

	switch(pData->nRequestType)
	{
	case FZ_ASYNCREQUEST_OVERWRITE:
		break;
	case FZ_ASYNCREQUEST_VERIFYCERT:
		if (!((CVerifyCertRequestData *)pData)->pCertData)
		{
			delete pData;
			return FZ_REPLY_INVALIDPARAM;
		}
		break;
	case FZ_ASYNCREQUEST_GSS_AUTHFAILED:
	case FZ_ASYNCREQUEST_GSS_NEEDUSER:
	case FZ_ASYNCREQUEST_GSS_NEEDPASS:
	case FZ_ASYNCREQUEST_NEWHOSTKEY:
	case FZ_ASYNCREQUEST_CHANGEDHOSTKEY:
		break;
	default:
		delete pData;
		return FZ_REPLY_INVALIDPARAM;
	}
	pData->nRequestResult = nAction;
	if (!m_pMainThread)
	{
		delete pData;
		return FZ_REPLY_NOTINITIALIZED;
	}

	m_pMainThread->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_ASYNCREQUESTREPLY,  (LPARAM)pData);
	
	return FZ_REPLY_OK;
}

int CFileZillaApi::SetOption(int nOption, int value)
{
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	switch (nOption)
	{
		case FZAPI_OPTION_SHOWHIDDEN:
			m_pMainThread->SetOption(nOption, value);
			break;
		default:
			return FZ_REPLY_INVALIDPARAM;
	}
	return FZ_REPLY_OK;
}

int CFileZillaApi::GetOption(int nOption, int &value)
{
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	switch (nOption)
	{
		case FZAPI_OPTION_SHOWHIDDEN:
			value = m_pMainThread->GetOption(nOption);
			break;
		default:
			return FZ_REPLY_INVALIDPARAM;
	}
	return FZ_REPLY_OK;
}

int CFileZillaApi::Chmod(int nValue, CString FileName, const CServerPath &path /*=CServerPath()*/ )
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (IsConnected()==FZ_REPLY_NOTCONNECTED)
		return FZ_REPLY_NOTCONNECTED;
	if (IsBusy()==FZ_REPLY_BUSY)
		return FZ_REPLY_BUSY;
	if (FileName=="")
		return FZ_REPLY_INVALIDPARAM;

	t_command command;
	command.id=FZ_COMMAND_CHMOD;
	command.param1=FileName;
	command.param4=nValue;
	command.path=path;
	m_pMainThread->Command(command);
	if (m_hOwnerWnd)
		return FZ_REPLY_WOULDBLOCK;
	else
		return m_pMainThread->LastOperationSuccessful()?FZ_REPLY_OK:FZ_REPLY_ERROR;
}

int CFileZillaApi::SetDebugLevel(int nDebugLevel)
{
	//Check if call allowed
	if (!m_bInitialized)
		return FZ_REPLY_NOTINITIALIZED;
	if (!m_pMainThread->SetDebugLevel(nDebugLevel))
		return FZ_REPLY_ERROR;

	return FZ_REPLY_OK;
}

BOOL CFileZillaApi::DumpDirectoryCache(LPCTSTR pFileName)
{
	CDirectoryCache cache;
	return cache.Dump(pFileName);
}

//CAsyncRequestData derived classes
CAsyncRequestData::CAsyncRequestData()
{
}

CAsyncRequestData::~CAsyncRequestData()
{
}
	
COverwriteRequestData::COverwriteRequestData()
{
	nRequestType=FZ_ASYNCREQUEST_OVERWRITE;
	time1=0;
	time2=0;
	pTransferFile=0;
}

COverwriteRequestData::~COverwriteRequestData()
{
	delete pTransferFile;
	delete time1;
	delete time2;
}

CVerifyCertRequestData::CVerifyCertRequestData()
{
	nRequestType=FZ_ASYNCREQUEST_VERIFYCERT;
	pCertData=0;
}

CVerifyCertRequestData::~CVerifyCertRequestData()
{
	delete pCertData;
}

CGssNeedPassRequestData::CGssNeedPassRequestData()
{
	nRequestType=FZ_ASYNCREQUEST_GSS_NEEDPASS;
}

CGssNeedPassRequestData::~CGssNeedPassRequestData()
{
}

CGssNeedUserRequestData::CGssNeedUserRequestData()
{
	nRequestType = FZ_ASYNCREQUEST_GSS_NEEDUSER;
}

CGssNeedUserRequestData::~CGssNeedUserRequestData()
{
}

CNewHostKeyRequestData::CNewHostKeyRequestData()
{
	nRequestType=FZ_ASYNCREQUEST_NEWHOSTKEY;
}

CNewHostKeyRequestData::~CNewHostKeyRequestData()
{
}

CChangedHostKeyRequestData::CChangedHostKeyRequestData()
{
	nRequestType=FZ_ASYNCREQUEST_CHANGEDHOSTKEY;
}

CChangedHostKeyRequestData::~CChangedHostKeyRequestData()
{
}

BOOL CFileZillaApi::IsValid() const
{
	if (!this)
		return FALSE;
	if (IsBadWritePtr((VOID *)this, sizeof(CFileZillaApi)) )
		return FALSE;
	if (IsBadWritePtr((VOID *)&m_bInitialized, sizeof(BOOL)) ||
		IsBadWritePtr((VOID *)&m_hOwnerWnd, sizeof(HWND)) ||
		IsBadWritePtr((VOID *)&m_nInternalMessageID, sizeof(unsigned int)) ||
		IsBadWritePtr((VOID *)&m_nReplyMessageID, sizeof(unsigned int)) ||
		IsBadWritePtr(m_pMainThread, sizeof(CMainThread)) )
			return FALSE;

	if (!m_pMainThread->IsValid())
		return FALSE;

	return TRUE;
}