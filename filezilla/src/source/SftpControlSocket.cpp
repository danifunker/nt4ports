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

// SftpControlSocket.cpp: Implementierung der Klasse CSFtpControlSocket.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SftpControlSocket.h"
#include "mainthread.h"
#include "AsyncProxySocketLayer.h"
#include "SFtpIpc.h"
#include "SFtpCommandIDs.h"
#include "FtpListResult.h"
#include "FileExistsDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


class CSFtpControlSocket::CFileTransferData : public CSFtpControlSocket::t_operation::COpData
{
public:
	CFileTransferData(t_server server) 
	{
		pDirectoryListing=0;
		nWaitNextOpState=0;
		nMKDOpState=-1;
		pFileTime=0;
		transferdata.transferleft=-1;
		transferdata.transfersize=-1;
		transferdata.nTransferStart=0;
		pStartTime=0;
		bUseAbsolutePaths = FALSE;
		pParser = new CFtpListResult(server);
	};
	virtual ~CFileTransferData() 
	{
		if (pDirectoryListing)
			delete pDirectoryListing;
		pDirectoryListing=0;
		delete pFileTime;
		delete pStartTime;
		delete pParser;
	};
	CString rawpwd;
	CTime ListStartTime;
	t_transferfile transferfile;
	t_transferdata transferdata;
	t_directory *pDirectoryListing;
	int nWaitNextOpState;
	CServerPath MKDCurrent;
	std::list<CString> MKDSegments;
	int nMKDOpState;
	CTime *pFileTime; //Used when downloading and OPTION_PRESERVEDOWNLOADFILETIME is set or when LIST fails
	CTime *pStartTime;
	CFtpListResult *pParser;
	BOOL bUseAbsolutePaths;
};

class CSFtpControlSocket::CListData:public CSFtpControlSocket::t_operation::COpData
{
public:
	CListData(t_server server) 
	{
		pDirectoryListing=0;
		pParser = new CFtpListResult(server);
	}
	virtual ~CListData() 
	{
		if (pDirectoryListing)
			delete pDirectoryListing;
		delete pParser;
	}
	CString rawpwd;
	CTime ListStartTime;
	CServerPath path;
	CString subdir;
	CServerPath realpath;
	int nListMode;
	t_directory *pDirectoryListing;
	CFtpListResult *pParser;
};

#define MKD_INIT -1
#define MKD_FINDPARENT 0
#define MKD_MAKESUBDIRS 1
#define MKD_CHANGETOSUBDIR 2

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CSFtpControlSocket::CSFtpControlSocket(CMainThread *pMainThread) : CControlSocket(pMainThread)
{
	ASSERT(pMainThread);
	m_pOwner=pMainThread;

	m_Operation.nOpMode=0;
	m_Operation.nOpState=-1;
	m_Operation.pData=0;

	m_pDataChannel=0;
	m_hSFtpProcess=0;

	m_bQuit=FALSE;
	m_bCheckForTimeout=TRUE;
	m_bError=FALSE;
}

CSFtpControlSocket::~CSFtpControlSocket()
{
	if (m_pDataChannel)
		delete m_pDataChannel;
	if (m_hSFtpProcess)
	{
		TerminateProcess(m_hSFtpProcess, 1);
		CloseHandle(m_hSFtpProcess);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Member-Funktion CControlSocket 
#define CONNECT_INIT -1
#define CONNECT_GSS -3

extern CString GetProgramFile();

void CSFtpControlSocket::Connect(t_server &server)
{
	USES_CONVERSION;

	ASSERT(!m_pOwner->IsConnected());
	ASSERT(!m_hSFtpProcess);
	
	ASSERT(!m_Operation.nOpMode);
	
	ASSERT(!m_pProxyLayer);
	
	if (!server.fwbypass)
	{
		int nProxyType=COptions::GetOptionVal(OPTION_PROXYTYPE);
		if (nProxyType!=PROXYTYPE_NOPROXY)
		{
			USES_CONVERSION;

			m_pProxyLayer=new CAsyncProxySocketLayer;
			if (nProxyType==PROXYTYPE_SOCKS4)
				m_pProxyLayer->SetProxy(PROXYTYPE_SOCKS4, T2CA(COptions::GetOption(OPTION_PROXYHOST)), COptions::GetOptionVal(OPTION_PROXYPORT));
			else if (nProxyType==PROXYTYPE_SOCKS4A)
				m_pProxyLayer->SetProxy(PROXYTYPE_SOCKS4A, T2CA(COptions::GetOption(OPTION_PROXYHOST)), COptions::GetOptionVal(OPTION_PROXYPORT));
			else if (nProxyType==PROXYTYPE_SOCKS5)
				if (COptions::GetOptionVal(OPTION_PROXYUSELOGON))
					m_pProxyLayer->SetProxy(PROXYTYPE_SOCKS5, T2CA(COptions::GetOption(OPTION_PROXYHOST)),
											COptions::GetOptionVal(OPTION_PROXYPORT),
											T2CA(COptions::GetOption(OPTION_PROXYUSER)),
											T2CA(CCrypt::decrypt(COptions::GetOption(OPTION_PROXYPASS))));
				else
					m_pProxyLayer->SetProxy(PROXYTYPE_SOCKS5, T2CA(COptions::GetOption(OPTION_PROXYHOST)),
											COptions::GetOptionVal(OPTION_PROXYPORT));
			else if (nProxyType==PROXYTYPE_HTTP11)
				if (COptions::GetOptionVal(OPTION_PROXYUSELOGON))
					m_pProxyLayer->SetProxy(PROXYTYPE_HTTP11, T2CA(COptions::GetOption(OPTION_PROXYHOST)),
											COptions::GetOptionVal(OPTION_PROXYPORT),
											T2CA(COptions::GetOption(OPTION_PROXYUSER)),
											T2CA(CCrypt::decrypt(COptions::GetOption(OPTION_PROXYPASS))));
				else
					m_pProxyLayer->SetProxy(PROXYTYPE_HTTP11, T2CA(COptions::GetOption(OPTION_PROXYHOST)), COptions::GetOptionVal(OPTION_PROXYPORT));
			else
				ASSERT(FALSE);
			AddLayer(m_pProxyLayer);
		}
	}
		
	m_Operation.nOpMode=CSMODE_CONNECT;
	
	m_Operation.nOpState=CONNECT_INIT;

	//Create data channel
	ASSERT(!m_pDataChannel);
	m_pDataChannel=new CSFtpIpc(this);
	if (!m_pDataChannel->Init())
	{
		ShowStatus(IDS_ERRORMSG_SFTP_CANTCREATESFTPIPC, 1);
		DoClose();
		return;
	}

	CString path=GetProgramFile();
	int pos=path.ReverseFind('\\');
	ASSERT(pos!=-1 && pos!=(path.GetLength()-1) );
	path=path.Left(pos+1);
	STARTUPINFO StartupInfo={0};
	StartupInfo.cb=sizeof(StartupInfo);
	
	PROCESS_INFORMATION ProcessInformation={0};
	TCHAR HandleString[1000];
	_tcscpy(HandleString, _T("FzSFtp.exe ") + m_pDataChannel->GetHandleString());
	if (!CreateProcess(path + _T("FzSFtp.exe"), HandleString, 0, 0, TRUE, 0, 0, 0, &StartupInfo, &ProcessInformation))
	{
		ShowStatus(IDS_ERRORMSG_SFTP_CANTCREATESFTPPROCESS, 1);
		DoClose();
		return;
	}
	
	m_hSFtpProcess=ProcessInformation.hProcess;
	CloseHandle(ProcessInformation.hThread);
	if (!m_pDataChannel->AttachProcess(ProcessInformation.hProcess))
	{
		ShowStatus(IDS_ERRORMSG_SFTP_CANTCREATESFTPPROCESS, 1);
		DoClose();
		return;
	}
	
	if (!Create(0, SOCK_STREAM, FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT |	FD_CONNECT | FD_CLOSE, 0, COptions::GetOptionVal(OPTION_ENABLE_IPV6) ? AF_UNSPEC : AF_INET))
	{
		DoClose();
		return;
	}
	AsyncSelect();
	int port;
	CString temp;
	
	temp=server.host;
	port=server.port;
	
	m_CurrentServer=server;
	
	CString hostname=server.host;
	if(server.port!=21) 
		hostname.Format( _T("%s:%d"), hostname, server.port); // add port to hostname (only if port is not 21)
	CString str;
	str.Format(IDS_STATUSMSG_CONNECTING,hostname);
	ShowStatus(str,0);
	
	m_Operation.nOpMode=CSMODE_CONNECT;
	
	if (!CControlSocket::Connect(temp, port))
	{
		if (WSAGetLastError()!=WSAEWOULDBLOCK)
		{
			DoClose();
			return;
		}
	}

	/* Disable Nagle algorithm. Most of the time single short strings get 
	 * transferred over the control connection. Waiting for additional data
	 * where there will be most likely none affects performance.
	 */
	//BOOL value = TRUE;
	//SetSockOpt(TCP_NODELAY, &value, sizeof(value), IPPROTO_TCP);

	m_ServerName = hostname;
	m_LastRecvTime = m_LastSendTime = CTime::GetCurrentTime();
}

void CSFtpControlSocket::OnTimer()
{
    if ( m_Operation.nOpMode&(CSMODE_LIST|CSMODE_TRANSFER) )
	UpdateTransferstatus(0);
}

BOOL CSFtpControlSocket::IsReady()
{
	return TRUE;
}

void CSFtpControlSocket::List(BOOL bFinish, int nError /*=0*/, CServerPath path /*=CServerPath()*/, CString subdir /*=""*/, int nListMode /*=0*/)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("List(%s,%d,\"%s\",\"%s\",%d)"),bFinish?_T("TRUE"):_T("FALSE"),nError,path.GetPath(),subdir,nListMode);

	USES_CONVERSION;

	#define LIST_INIT	-1
	#define LIST_PWD	0
	#define LIST_CWD	1
	#define LIST_CWD2	2
	#define LIST_LIST	3

	ASSERT(!m_Operation.nOpMode || m_Operation.nOpMode&CSMODE_LIST);
	
	m_Operation.nOpMode|=CSMODE_LIST;

	if (!m_pOwner->IsConnected())
	{
		ResetOperation(FZ_REPLY_ERROR|FZ_REPLY_NOTCONNECTED);
		return;
	}

	if (bFinish || nError)
		if (m_Operation.nOpMode!=CSMODE_LIST)
			return; //Old message coming in
	
	if (nError)
	{
		if (nError&CSMODE_TRANSFERTIMEOUT)
			DoClose();
		else
			ResetOperation(FZ_REPLY_ERROR);
		return;
	}

	CListData *pData = static_cast<CListData*>(m_Operation.pData);

	if (m_Operation.nOpState!=LIST_INIT)
	{
		if (m_Operation.nOpState==LIST_PWD)
		{ //Reply to PWD command
			pData->realpath.SetServer(m_CurrentServer);
			if (m_Reply=="" || !pData->realpath.SetPath(A2CT(m_Reply)))
			{
				LogMessage(__FILE__, __LINE__, this,FZ_LOG_WARNING, _T("Can't parse path") );
				ResetOperation(FZ_REPLY_ERROR);
				return;
			}
			pData->rawpwd=m_Reply;
			m_pOwner->SetCurrentPath(pData->realpath);
			if (pData->nListMode&FZ_LIST_USECACHE)
			{
				t_directory dir;
				CDirectoryCache cache;
				BOOL res=cache.Lookup(pData->realpath,m_CurrentServer,dir);
				if (res)
				{
					BOOL bExact=TRUE;
					if (pData->nListMode & FZ_LIST_EXACT)
						for (int i=0;i<dir.num;i++)
							if (dir.direntry[i].bUnsure || (dir.direntry[i].size==-1 && !dir.direntry[i].dir))
							{
								bExact=FALSE;
								break;
							}
					if (bExact)
					{
						ShowStatus(IDS_STATUSMSG_DIRLISTSUCCESSFUL,0);
						SetDirectoryListing(&dir);
						ResetOperation(FZ_REPLY_OK);
						return;
					}
				}
			}
			m_Operation.nOpState=LIST_LIST;
		}
		else if (m_Operation.nOpState==LIST_CWD)
		{
			pData->realpath.SetServer(m_CurrentServer);
			if (m_Reply == "" || !pData->realpath.SetPath(A2CT(m_Reply)))
			{
				LogMessage(__FILE__, __LINE__, this,FZ_LOG_WARNING, _T("Can't parse path") );
				ResetOperation(FZ_REPLY_ERROR);
				return;
			}
			pData->rawpwd = m_Reply;
			m_pOwner->SetCurrentPath(pData->realpath);
			if (pData->nListMode&FZ_LIST_USECACHE && pData->subdir=="")
			{
				t_directory dir;
				CDirectoryCache cache;
				BOOL res = cache.Lookup(pData->realpath, m_CurrentServer, dir);
				if (res)
				{
					BOOL bExact=TRUE;
					if (pData->nListMode & FZ_LIST_EXACT)
						for (int i=0;i<dir.num;i++)
							if (dir.direntry[i].bUnsure || (dir.direntry[i].size==-1 && !dir.direntry[i].dir))
							{
								bExact=FALSE;
								break;
							}
					if (bExact)
					{
						ShowStatus(IDS_STATUSMSG_DIRLISTSUCCESSFUL,0);
						SetDirectoryListing(&dir);
						ResetOperation(FZ_REPLY_OK);
						return;
					}
				}
			}
			if (pData->subdir!="")
			{
				if (pData->path!=pData->realpath)
				{
					ResetOperation(FZ_REPLY_ERROR);
					return;
				}
				m_Operation.nOpState=LIST_CWD2;
			}
			else
				m_Operation.nOpState=LIST_LIST;
		}
		else if (m_Operation.nOpState==LIST_CWD2)
		{
			pData->realpath.SetServer(m_CurrentServer);
			if (m_Reply=="" || !pData->realpath.SetPath(A2CT(m_Reply)))
			{
				LogMessage(__FILE__, __LINE__, this,FZ_LOG_WARNING, _T("Can't parse path") );
				ResetOperation(FZ_REPLY_ERROR);
				return;
			}
			pData->rawpwd=m_Reply;
			m_pOwner->SetCurrentPath(pData->realpath);
			if (pData->nListMode&FZ_LIST_USECACHE)
			{
				t_directory dir;
				CDirectoryCache cache;
				cache.Lock();
				BOOL res=cache.Lookup(pData->realpath,m_CurrentServer,dir);
				if (res)
				{
					if (pData->subdir!="..")
						m_pOwner->m_pDirectoryCache->Store(dir,pData->path,pData->subdir,TRUE);
					cache.Unlock();
					BOOL bExact=TRUE;
					if (pData->nListMode & FZ_LIST_EXACT)
						for (int i=0;i<dir.num;i++)
							if (dir.direntry[i].bUnsure || (dir.direntry[i].size==-1 && !dir.direntry[i].dir))
							{
								bExact=FALSE;
								break;
							}
					if (bExact)
					{
						ShowStatus(IDS_STATUSMSG_DIRLISTSUCCESSFUL,0);
						SetDirectoryListing(&dir);
						ResetOperation(FZ_REPLY_OK);
						return;
					}
				}
				else
					cache.Unlock();
			}
			m_Operation.nOpState=LIST_LIST;
		}
		else if (m_Operation.nOpState==LIST_LIST)
		{
			if (m_Reply != "")
			{
				char *data = new char[m_Reply.GetLength()+2+1];
				strcpy(data, m_Reply + "\r\n");
				pData->pParser->AddData(data, m_Reply.GetLength()+2);
			}
			else
			{
				int num = 0;
				delete pData->pDirectoryListing;
				pData->pDirectoryListing = new t_directory;
				if (COptions::GetOptionVal(OPTION_DEBUGSHOWLISTING))
					pData->pParser->SendToMessageLog(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID);
				pData->pDirectoryListing->direntry = pData->pParser->getList(num, CTime::GetCurrentTime());
				pData->pDirectoryListing->num = num;
				if (pData->pParser->m_server.nServerType & FZ_SERVERTYPE_SUB_FTP_VMS && m_CurrentServer.nServerType&FZ_SERVERTYPE_FTP)
					m_CurrentServer.nServerType |= FZ_SERVERTYPE_SUB_FTP_VMS;
				pData->pDirectoryListing->server = m_CurrentServer;
				pData->pDirectoryListing->path.SetServer(pData->pDirectoryListing->server);
				if (pData->rawpwd!="")
				{
					if (!pData->pDirectoryListing->path.SetPath(pData->rawpwd))
					{
						delete m_pDirectoryListing;
						m_pDirectoryListing=0;
						ResetOperation(FZ_REPLY_ERROR);
						return;
					}
				}
				else
					pData->pDirectoryListing->path = pData->realpath;
				ShowStatus(IDS_STATUSMSG_DIRLISTSUCCESSFUL,0);
				
				//Merge new listing into cache
				CDirectoryCache cache;
				cache.Lock();
				t_directory dir;
				if (!pData->path.IsEmpty() && pData->subdir!="")
				{
					if (cache.Lookup(pData->pDirectoryListing->path, pData->pDirectoryListing->server, dir))
						pData->pDirectoryListing->Merge(dir, pData->ListStartTime);
					cache.Store(*pData->pDirectoryListing,pData->path,pData->subdir);
				}
				else
				{
					if (cache.Lookup(pData->pDirectoryListing->path, pData->pDirectoryListing->server, dir))
						pData->pDirectoryListing->Merge(dir, pData->ListStartTime);
					cache.Store(*pData->pDirectoryListing);
				}
				cache.Unlock();

				SetDirectoryListing(pData->pDirectoryListing);
				ResetOperation(FZ_REPLY_OK);
			}
			return;
		}
	}
	
	if (m_Operation.nOpState==LIST_INIT)
	{ //Initialize some variables
		pData = new CListData(m_CurrentServer);;
		pData->pParser->InitLog(this);
		pData->nListMode=nListMode;
		pData->path=path;
		pData->subdir=subdir;
		m_Operation.pData=pData;
		ShowStatus(IDS_STATUSMSG_RETRIEVINGDIRLIST,0);
		if (m_pDirectoryListing)
		{
			delete m_pDirectoryListing;
			m_pDirectoryListing=0;
		}
		
		CServerPath path=pData->path;
		m_pOwner->GetCurrentPath(pData->realpath);
		CServerPath realpath=pData->realpath;
		if (!pData->realpath.IsEmpty())
		{
			if (!pData->path.IsEmpty() && pData->path!=pData->realpath)
				m_Operation.nOpState=LIST_CWD;
			else if (!pData->path.IsEmpty() && pData->subdir!="")
				m_Operation.nOpState=LIST_CWD2;
			else
			{
				if (pData->nListMode&FZ_LIST_REALCHANGE)
				{
					if (pData->subdir=="")
						m_Operation.nOpState=LIST_CWD;
					else
						m_Operation.nOpState=LIST_CWD2;
				}
				else
				{
					if (pData->nListMode&FZ_LIST_USECACHE)
					{
						t_directory dir;
						CDirectoryCache cache;
						BOOL res=cache.Lookup(pData->realpath,m_CurrentServer,dir);
						if (res)
						{
							BOOL bExact=TRUE;
							if (pData->nListMode & FZ_LIST_EXACT)
								for (int i=0;i<dir.num;i++)
									if (dir.direntry[i].bUnsure || (dir.direntry[i].size==-1 && !dir.direntry[i].dir))
									{
										bExact=FALSE;
										break;
									}
							if (bExact)
							{
								ShowStatus(IDS_STATUSMSG_DIRLISTSUCCESSFUL,0);
								SetDirectoryListing(&dir);
								ResetOperation(FZ_REPLY_OK);
								return;
							}
						}
					}
					m_Operation.nOpState=LIST_LIST;
				}
			}
		}
		else
		{
			if (pData->path.IsEmpty())
				m_Operation.nOpState=LIST_PWD;
			else
				m_Operation.nOpState=LIST_CWD;
		}
	}
	BOOL bSuccessful=FALSE;
	if (m_Operation.nOpState==LIST_PWD)
		bSuccessful=m_pDataChannel->Send(SFTP_DATAID_STC_PWD, 0, 0);
	else if (m_Operation.nOpState==LIST_CWD)
		bSuccessful=m_pDataChannel->Send(SFTP_DATAID_STC_CD, pData->path.GetPath());
	else if (m_Operation.nOpState==LIST_CWD2)
	{
		if (!pData->subdir)
		{
			ResetOperation(FZ_REPLY_ERROR);
			return;
		}
		bSuccessful=m_pDataChannel->Send(SFTP_DATAID_STC_CD, pData->subdir);
	}
	else if (m_Operation.nOpState==LIST_LIST)
	{
		pData->ListStartTime=CTime::GetCurrentTime();
		bSuccessful=m_pDataChannel->Send(SFTP_DATAID_STC_LIST, 0, 0);
	}
	if (!bSuccessful)
		DoClose();
}

void CSFtpControlSocket::FtpCommand(LPCTSTR pCommand)
{
	ASSERT(FALSE);
}

void CSFtpControlSocket::Disconnect()
{
	ASSERT(!m_Operation.nOpMode);
	m_Operation.nOpMode=CSMODE_DISCONNECT;
	DoClose();
}

void CSFtpControlSocket::FileTransfer(t_transferfile *transferfile /*=0*/, BOOL bFinish /*=FALSE*/, int nError /*=0*/)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("FileTransfer(%d, %s, %d)  OpMode=%d OpState=%d"), transferfile,bFinish?_T("TRUE"):_T("FALSE"),nError,m_Operation.nOpMode,m_Operation.nOpState);

	USES_CONVERSION;
	
	#define FILETRANSFER_INIT			-1 //Opt: LIST TYPE
	#define FILETRANSFER_CWD			0 //PWD
	#define FILETRANSFER_MKD			1
	#define FILETRANSFER_CWD2			2
	#define FILETRANSFER_LIST			4
	#define FILETRANSFER_RETRSTOR		13
	
	#define FILETRANSFER_WAIT			15


	//Flowchart of FileTransfer
	//
	//            +----+
	//     /------|Init|--------\
	//     |      +----+        |
	//     |         |          |
	//     |         |          |
	//     |       +---+        |
	//     |       |CWD|--\     |
	//     |       +---+  |     |
	//     |         |    |     |
	//     |         |    |     |
	//     |         |  +---+   |
	//     |         |  |MKD|   |
	//     |         |  +---+   |
	//     |         |    |     |
	//     |         |    |     |
	//     |         |  +----+  |
	//     |         |  |CWD2|  |
	//     |         |  +----+  |
	//     |         |    |     |
	//     |         +----/     |
	//     |         |          |
	//     |         +----------/
	//     |         |
	//     |      +----+
	//     |      |LIST|
	//     |      +----+
    //     |         |
	//     \---------+
	//               |
	//          +--------+
	//          |RETRSTOR|
	//          +--------+
	
	ASSERT(!m_Operation.nOpMode || m_Operation.nOpMode&CSMODE_TRANSFER);
	if (!m_pOwner->IsConnected())
	{
		m_Operation.nOpMode=CSMODE_TRANSFER|(transferfile->get?CSMODE_DOWNLOAD:CSMODE_UPLOAD);
		ResetOperation(FZ_REPLY_ERROR|FZ_REPLY_DISCONNECTED);
	}

	CFileTransferData *pData=static_cast<CFileTransferData *>(m_Operation.pData);

	//////////////////
	//Initialization//
	//////////////////
	int nReplyError=0;
	if (m_Operation.nOpState==FILETRANSFER_INIT)
	{
		ASSERT(transferfile);
		ASSERT(!m_Operation.nOpMode);
		ASSERT(!m_Operation.pData);

		CString str;
		str.Format(transferfile->get?IDS_STATUSMSG_DOWNLOADSTART:IDS_STATUSMSG_UPLOADSTART,transferfile->get?transferfile->remotepath.GetPath()+transferfile->remotefile:transferfile->localfile);
		ShowStatus(str,0);
		
		m_Operation.nOpMode=CSMODE_TRANSFER|(transferfile->get?CSMODE_DOWNLOAD:CSMODE_UPLOAD);

		pData = new CFileTransferData(m_CurrentServer);
		pData->pParser->InitLog(this);
		m_Operation.pData=pData;

		//Replace invalid characters in the local filename
		int pos=transferfile->localfile.ReverseFind('\\');
		for (int i=(pos+1);i<transferfile->localfile.GetLength();i++)
			if (transferfile->localfile[i]==':')
				transferfile->localfile.SetAt(i, '_');
		
		pData->transferfile=*transferfile;
		pData->transferdata.bResume=FALSE;
		pData->transferdata.bType = (pData->transferfile.nType == 1) ? TRUE : FALSE;
		
		CServerPath path;
		VERIFY (m_pOwner->GetCurrentPath(path));
		if (path==pData->transferfile.remotepath)
		{
			if (m_pDirectoryListing)
			{
				m_Operation.nOpState=FILETRANSFER_RETRSTOR;
				CString remotefile=pData->transferfile.remotefile;
				int i;
				for (i=0; i<m_pDirectoryListing->num; i++)
				{
					if (m_pDirectoryListing->direntry[i].name==remotefile &&
						( m_pDirectoryListing->direntry[i].bUnsure || m_pDirectoryListing->direntry[i].size==-1 ))
					{
						delete m_pDirectoryListing;
						m_pDirectoryListing=0;
						m_Operation.nOpState=FILETRANSFER_LIST;
						break;
					}
				}				
				if (m_pDirectoryListing && i==m_pDirectoryListing->num)
				{
					nReplyError=CheckOverwriteFile();
					if (!nReplyError)
					{
						if (pData->transferfile.get)
						{
							CString path=pData->transferfile.localfile;
							if (path.ReverseFind('\\')!=-1)
							{
								path=path.Left(path.ReverseFind('\\')+1);
								CString path2;
								while (path!="")
								{
									path2+=path.Left(path.Find( _T("\\") )+1);
									path=path.Mid(path.Find( _T("\\") )+1);
									int res=CreateDirectory(path2,0);
								}
							}
						}
					}
				}
			}
			else
			{
				CDirectoryCache cache;
				t_server server;
				m_pOwner->GetCurrentServer(server);
				t_directory dir;
				BOOL res=cache.Lookup(pData->transferfile.remotepath,server,dir);
				if (res)
				{
					CString remotefile = pData->transferfile.remotefile;
					int i;
					for (i=0; i<dir.num; i++)
					{
						if (dir.direntry[i].name==remotefile &&
							( dir.direntry[i].bUnsure || dir.direntry[i].size==-1 ))
						{
							m_Operation.nOpState=FILETRANSFER_LIST;
							break;
						}
					}
					if (i==dir.num)
					{
						SetDirectoryListing(&dir);
						m_Operation.nOpState=FILETRANSFER_RETRSTOR;
						nReplyError=CheckOverwriteFile();
						if (!nReplyError)
						{
							if (pData->transferfile.get)
							{
								CString path=pData->transferfile.localfile;
								if (path.ReverseFind('\\')!=-1)
								{
									path=path.Left(path.ReverseFind('\\')+1);
									CString path2;
									while (path!="")
									{
										path2+=path.Left(path.Find( _T("\\") )+1);
										path=path.Mid(path.Find(_T( "\\") )+1);
										int res=CreateDirectory(path2,0);
									}
								}
							}
						}
					}
				}
				else
					m_Operation.nOpState=FILETRANSFER_LIST;
			}
		}
		else
			m_Operation.nOpState=FILETRANSFER_CWD;
	}
	else
	{
		///////////
		//Replies//
		///////////
		switch(m_Operation.nOpState)
		{
		case FILETRANSFER_CWD:
			if (m_bError)
			{
				if (!pData->transferfile.get)
					m_Operation.nOpState = FILETRANSFER_MKD;
				else
				{
					pData->bUseAbsolutePaths = TRUE;
					m_Operation.nOpState = FILETRANSFER_RETRSTOR;
				}
			}
			else
			{
				CServerPath path;
				path.SetServer(m_CurrentServer);
				if (!path.SetPath(A2CT(m_Reply)))
				{
					LogMessage(__FILE__, __LINE__, this,FZ_LOG_WARNING, _T("Can't parse path"));
					nReplyError=FZ_REPLY_ERROR;
				}
				else
				{
					pData->rawpwd=m_Reply;
					m_pOwner->SetCurrentPath(path);
					if (path!=pData->transferfile.remotepath)
						nReplyError=FZ_REPLY_ERROR | FZ_REPLY_CRITICALERROR;
					else
					{
						CDirectoryCache cache;
						t_server server;
						m_pOwner->GetCurrentServer(server);
						t_directory dir;
						BOOL res=cache.Lookup(pData->transferfile.remotepath, server,dir);
						if (res)
						{
							CString remotefile=pData->transferfile.remotefile;
							int i;
							for (i=0; i<dir.num; i++)
							{
								if (dir.direntry[i].name==remotefile &&
									( dir.direntry[i].bUnsure || dir.direntry[i].size==-1 ))
								{
									m_Operation.nOpState=FILETRANSFER_LIST;
									break;
								}
							}
							if (i==dir.num)
							{
								SetDirectoryListing(&dir);
								m_Operation.nOpState=FILETRANSFER_RETRSTOR;
								nReplyError=CheckOverwriteFile();
								if (!nReplyError)
								{
									if (pData->transferfile.get)
									{
										CString path=pData->transferfile.localfile;
										if (path.ReverseFind('\\')!=-1)
										{
											path=path.Left(path.ReverseFind('\\')+1);
											CString path2;
											while (path!="")
											{
												path2+=path.Left(path.Find( _T("\\") )+1);
												path=path.Mid(path.Find( _T("\\") )+1);
												int res=CreateDirectory(path2,0);
											}
										}
									}
								}
							}
						}
						else
							m_Operation.nOpState=FILETRANSFER_LIST;
					}
				}
			}
			break;
		case FILETRANSFER_MKD:
			switch(pData->nMKDOpState)
			{
			case MKD_FINDPARENT:
				if (!m_bError)
				{
					m_pOwner->SetCurrentPath(pData->MKDCurrent);

					if (!m_pDirectoryListing)
					{
						CDirectoryCache cache;
						t_directory dir;
						BOOL res=cache.Lookup(pData->MKDCurrent, m_CurrentServer, dir, TRUE);
						if (res)
							SetDirectoryListing(&dir);
					}
					
					pData->nMKDOpState=MKD_CHANGETOSUBDIR;
					pData->MKDCurrent.AddSubdir(pData->MKDSegments.front());
					CString Segment=pData->MKDSegments.front();
					pData->MKDSegments.pop_front();
					if (!m_pDataChannel->Send(SFTP_DATAID_STC_MKD, Segment))
					{
						DoClose();
						return;
					}
				}
				else
				{
					if (!pData->MKDCurrent.HasParent())
						nReplyError=FZ_REPLY_ERROR|FZ_REPLY_CRITICALERROR;
					else
					{
						pData->MKDSegments.push_front(pData->MKDCurrent.GetLastSegment());
						pData->MKDCurrent=pData->MKDCurrent.GetParent();
						if (!m_pDataChannel->Send(SFTP_DATAID_STC_CD, pData->MKDCurrent.GetPath()))
						{
							DoClose();
							return;	
						}
					}
				}
				break;
			case MKD_MAKESUBDIRS:
				if (!m_bError)
				{ //Create dir entry in parent dir
					ASSERT(!pData->MKDSegments.empty());
					pData->MKDCurrent.AddSubdir(pData->MKDSegments.front());
					CString Segment=pData->MKDSegments.front();
					pData->MKDSegments.pop_front();
					if (m_pDataChannel->Send(SFTP_DATAID_STC_MKD, Segment))
						pData->nMKDOpState=MKD_CHANGETOSUBDIR;
					else
					{
						DoClose();
						return;
					}
				}
				else
					nReplyError=FZ_REPLY_ERROR;
				break;
			case MKD_CHANGETOSUBDIR:
				if (!m_bError)
				{
					CServerPath path2=pData->MKDCurrent;
					if (path2.HasParent())
					{
						CString name=path2.GetLastSegment();
						path2=path2.GetParent();
						CDirectoryCache cache;
						cache.Lock();
						BOOL bCached=FALSE;
						t_directory dir;
						BOOL res=cache.Lookup(path2,m_CurrentServer,dir);
						if (!res)
							bCached=FALSE;
						if (!res && m_pDirectoryListing)
						{
							if (m_pDirectoryListing->path==path2)
							{
								dir=*m_pDirectoryListing;
								res=TRUE;
							}
						}
						t_directory WorkingDir;
						BOOL bFound=m_pOwner->GetWorkingDir(&WorkingDir);
						if (!res && bFound)
							if (WorkingDir.path==path2)
							{
								dir=WorkingDir;
								res=TRUE;
							}
						if (!res)
						{
							dir.path==path2;
							dir.server=m_CurrentServer;
						}
							
						int i;
						for (i=0; i<dir.num; i++)
							if (dir.direntry[i].name==name)
							{
								LogMessage(__FILE__, __LINE__, this,FZ_LOG_WARNING, _T("Dir already exists in cache!"));
									break;
							}
						if (i == dir.num)
						{
							t_directory::t_direntry *entries = new t_directory::t_direntry[dir.num+1];
							for (i=0; i<dir.num; i++)
								entries[i] = dir.direntry[i];
							entries[i].name=name;
							entries[i].lName=name;
							entries[i].lName.MakeLower();
							entries[i].dir=TRUE;
							entries[i].date.hasdate=FALSE;
							entries[i].size=-1;
							delete [] dir.direntry;
							dir.direntry=entries;
							dir.num++;
							cache.Store(dir, bCached);
							BOOL updated=FALSE;
							if (m_pDirectoryListing && m_pDirectoryListing->path==dir.path)
							{
								updated=TRUE;
								SetDirectoryListing(&dir);
							}
							if (!updated)
								if (WorkingDir.path==dir.path)
								{
									updated=TRUE;
									m_pOwner->SetWorkingDir(&dir);
								}
						}
						cache.Unlock();
					}
				}

				//Continue operation even if MKD failed, maybe another thread did create this directory for us
				if (pData->MKDSegments.empty())
					m_Operation.nOpState=FILETRANSFER_CWD2;
				else
				{
					if (m_pDataChannel->Send(SFTP_DATAID_STC_CD, pData->MKDCurrent.GetPath()))
						pData->nMKDOpState=MKD_MAKESUBDIRS;
					else
					{
						DoClose();
						return;
					}
				}		
				break;
			default: 
				ASSERT(FALSE);
			}
			
			break;
		case FILETRANSFER_CWD2:
			if (m_bError)
				nReplyError=FZ_REPLY_ERROR | FZ_REPLY_CRITICALERROR;
			else
			{
				CServerPath path;
				path.SetServer(m_CurrentServer);
				if (!path.SetPath(A2CT(m_Reply)))
				{
					LogMessage(__FILE__, __LINE__, this,FZ_LOG_WARNING, _T("Can't parse path"));
					nReplyError=FZ_REPLY_ERROR;
				}
				else
				{
					pData->rawpwd=m_Reply;
					m_pOwner->SetCurrentPath(path);
					if (path!=pData->transferfile.remotepath)
						nReplyError=FZ_REPLY_ERROR | FZ_REPLY_CRITICALERROR;
					else
					{
						CDirectoryCache cache;
						t_server server;
						m_pOwner->GetCurrentServer(server);
						t_directory dir;
						BOOL res=cache.Lookup(pData->transferfile.remotepath, server,dir);
						if (res)
						{
							CString remotefile=pData->transferfile.remotefile;
							int i;
							for (i=0; i<dir.num; i++)
							{
								if (dir.direntry[i].name==remotefile &&
									( dir.direntry[i].bUnsure || dir.direntry[i].size==-1 ))
								{
										m_Operation.nOpState=FILETRANSFER_LIST;
										break;
								}
							}
							if (i == dir.num)
							{
								SetDirectoryListing(&dir);
								m_Operation.nOpState=FILETRANSFER_RETRSTOR;
								nReplyError=CheckOverwriteFile();
								if (!nReplyError)
								{
									if (pData->transferfile.get)
									{
										CString path=pData->transferfile.localfile;
										if (path.ReverseFind('\\')!=-1)
										{
											path=path.Left(path.ReverseFind('\\')+1);
											CString path2;
											while (path!="")
											{
												path2+=path.Left(path.Find( _T("\\") )+1);
												path=path.Mid(path.Find( _T("\\") )+1);
												int res=CreateDirectory(path2,0);
											}
										}
									}
								}
							}
						}
						else
							m_Operation.nOpState=FILETRANSFER_LIST;
					}
				}
			}
			break;
		case FILETRANSFER_LIST:
			if (m_bError)
			{
				m_Operation.nOpState = FILETRANSFER_RETRSTOR;
				nReplyError = CheckOverwriteFile();
				if (!nReplyError)
				{
					if (pData->transferfile.get)
					{
						CString path = pData->transferfile.localfile;
						if (path.ReverseFind('\\')!=-1)
						{
							path=path.Left(path.ReverseFind('\\')+1);
							CString path2;
							while (path!="")
							{
								path2+=path.Left(path.Find( _T("\\") )+1);
								path=path.Mid(path.Find( _T("\\") )+1);
								int res=CreateDirectory(path2,0);
							}
						}
					}
				}
			}
			else
			{
				if (m_Reply != "")
				{
					char *data = new char[m_Reply.GetLength() + 2 + 1];
					strcpy(data, m_Reply + "\r\n");
					pData->pParser->AddData(data, m_Reply.GetLength()+2);
					return;
				}
				else
				{
					int num = 0;
					delete pData->pDirectoryListing;
					pData->pDirectoryListing = new t_directory;
					if (COptions::GetOptionVal(OPTION_DEBUGSHOWLISTING))
						pData->pParser->SendToMessageLog(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID);
					pData->pDirectoryListing->direntry=pData->pParser->getList(num, pData->ListStartTime);
					pData->pDirectoryListing->num=num;
					if (pData->pParser->m_server.nServerType&FZ_SERVERTYPE_SUB_FTP_VMS && m_CurrentServer.nServerType&FZ_SERVERTYPE_FTP)
						m_CurrentServer.nServerType|=FZ_SERVERTYPE_SUB_FTP_VMS;
					pData->pDirectoryListing->server=m_CurrentServer;
					pData->pDirectoryListing->path.SetServer(pData->pDirectoryListing->server);
					if (pData->rawpwd!="")
					{
						if (!pData->pDirectoryListing->path.SetPath(pData->rawpwd))
						{
							delete m_pDirectoryListing;
							m_pDirectoryListing=0;
							ResetOperation(FZ_REPLY_ERROR);
							return;
						}
					}
					else
						pData->pDirectoryListing->path=pData->transferfile.remotepath;
					ShowStatus(IDS_STATUSMSG_DIRLISTSUCCESSFUL,0);
					CDirectoryCache cache;
					t_directory dir;
					cache.Lock();
					if (cache.Lookup(pData->pDirectoryListing->path, pData->pDirectoryListing->server, dir, TRUE))
					{
						pData->pDirectoryListing->Merge(dir, pData->ListStartTime);
					}
					cache.Store(*pData->pDirectoryListing);
					cache.Unlock();
					SetDirectoryListing(pData->pDirectoryListing);
				}
				m_Operation.nOpState = FILETRANSFER_RETRSTOR;
				nReplyError = CheckOverwriteFile();
				if (!nReplyError)
				{
					if (pData->transferfile.get)
					{
						CString path = pData->transferfile.localfile;
						if (path.ReverseFind('\\')!=-1)
						{
							path=path.Left(path.ReverseFind('\\')+1);
							CString path2;
							while (path!="")
							{
								path2+=path.Left(path.Find( _T("\\") )+1);
								path=path.Mid(path.Find( _T("\\") )+1);
								int res=CreateDirectory(path2,0);
							}
						}
					}
				}
			}
			break;
		case FILETRANSFER_WAIT:
			if (!pData->nWaitNextOpState)
				nReplyError=FZ_REPLY_ERROR;
			else
				m_Operation.nOpState=pData->nWaitNextOpState;
			break;
		case FILETRANSFER_RETRSTOR:
			if (m_bError)
				nReplyError=FZ_REPLY_ERROR;
			else
			{
				if (COptions::GetOptionVal(OPTION_PRESERVEDOWNLOADFILETIME))
				{
					if (pData->pFileTime)
					{
						SYSTEMTIME stime;
						FILETIME ftime;
						HANDLE hFile=CreateFile(pData->transferfile.localfile, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
						if (hFile!=INVALID_HANDLE_VALUE)
						{
							if (pData->pFileTime->GetAsSystemTime(stime))
								if (SystemTimeToFileTime(&stime, &ftime))
									SetFileTime(hFile, &ftime, &ftime, &ftime);
							CloseHandle(hFile);
						}
					}
				}
				//Transfer successful
				ResetOperation(FZ_REPLY_OK);
			}
			break;
		}
		if (nReplyError)
		{ //Error transferring the file	
			ResetOperation(nReplyError);
			return;
		}
	}
	/////////////////
	//Send commands//
	/////////////////
	BOOL bError=FALSE;
	switch(m_Operation.nOpState)
	{
	case FILETRANSFER_CWD:
		if (!m_pDataChannel->Send(SFTP_DATAID_STC_CD, pData->transferfile.remotepath.GetPath()))
			bError=TRUE;
		break;
	case FILETRANSFER_MKD:
		if (pData->nMKDOpState==MKD_INIT)
		{
			if (!m_pDataChannel->Send(SFTP_DATAID_STC_CD, pData->transferfile.remotepath.GetParent().GetPath()))
				bError=TRUE;
			else
			{
				pData->MKDCurrent=pData->transferfile.remotepath.GetParent();
				pData->MKDSegments.push_front(pData->transferfile.remotepath.GetLastSegment());
				pData->nMKDOpState=MKD_FINDPARENT;
			}
		}
		break;
	case FILETRANSFER_CWD2:
		if (!m_pDataChannel->Send(SFTP_DATAID_STC_CD, pData->transferfile.remotepath.GetPath()))
			bError=TRUE;
		break;
	case FILETRANSFER_LIST:
		if (!m_pDataChannel->Send(SFTP_DATAID_STC_LIST, 0))
			bError=TRUE;
		else
			pData->ListStartTime=CTime::GetCurrentTime();
		break;
	case FILETRANSFER_RETRSTOR:
		if (m_pDirectoryListing)
			for (int i=0;i<m_pDirectoryListing->num;i++)
			{
				CString remotefile=pData->transferfile.remotefile;
				if (m_pDirectoryListing->direntry[i].name==remotefile)
				{
					if (pData->transferfile.get)
					{
						if (m_pDirectoryListing->direntry[i].date.hasdate)
						{
							ASSERT(!pData->pFileTime);
							if (pData->pFileTime)
								delete pData->pFileTime;
							TRY
							{
								pData->pFileTime = new CTime(m_pDirectoryListing->direntry[i].date.year,
								m_pDirectoryListing->direntry[i].date.month,
								m_pDirectoryListing->direntry[i].date.day,
								m_pDirectoryListing->direntry[i].date.hastime?m_pDirectoryListing->direntry[i].date.hour:0,
								m_pDirectoryListing->direntry[i].date.hastime?m_pDirectoryListing->direntry[i].date.minute:0,
								0,-1);
								TIME_ZONE_INFORMATION tzInfo={0};
								BOOL res = GetTimeZoneInformation(&tzInfo);
								CTimeSpan span(0, 0, tzInfo.Bias+((res==TIME_ZONE_ID_DAYLIGHT)?tzInfo.DaylightBias:tzInfo.StandardBias), 0);
								*pData->pFileTime += span;
							}
							CATCH_ALL(e)
							{
								TCHAR buffer[1024];
								CString str =_T("Exception creating CTime object: ");
								if (e->GetErrorMessage(buffer, 1024, NULL))
									str += buffer;
								else
								str += _T("Unknown exception");
								LogMessageRaw(__FILE__, __LINE__, this, FZ_LOG_WARNING, str);
								pData->pFileTime = 0;
							}
							END_CATCH_ALL;
						}
						pData->transferdata.transfersize=m_pDirectoryListing->direntry[i].size;
					}
					else if (pData->transferdata.bResume)
						pData->transferdata.nTransferStart=m_pDirectoryListing->direntry[i].size;
					break;
				}
			}
		if (pData->transferfile.get)
			if (pData->transferdata.bResume)
			{
				VERIFY(GetLength64(pData->transferfile.localfile, pData->transferdata.nTransferStart));
			}
			else
				pData->transferdata.nTransferStart=0;
		else
			VERIFY(GetLength64(pData->transferfile.localfile, pData->transferdata.transfersize));

		if (pData->transferdata.transfersize>=0)
			pData->transferdata.transferleft=pData->transferdata.transfersize-pData->transferdata.nTransferStart;

		CString filename;
		if (!pData->bUseAbsolutePaths)
			filename = pData->transferfile.remotefile;
		else
			filename = pData->transferfile.remotepath.GetPath() + pData->transferfile.remotefile;
		int nLen1 = strlen(T2CA(filename)) + 1;
		int nLen2 = strlen(T2CA(pData->transferfile.localfile)) + 1;
		char *pCmd=new char[nLen1+nLen2+4];
		strcpy(pCmd, T2CA(filename));
		strcpy(pCmd+nLen1, T2CA(pData->transferfile.localfile));
		memcpy(pCmd+nLen1+nLen2, &pData->transferdata.bResume, 4);
		pData->pStartTime = new CTime;
		*pData->pStartTime = CTime::GetCurrentTime();
		if (pData->transferfile.get)
		{
			if (!m_pDataChannel->Send(SFTP_DATAID_STC_GET, nLen1+nLen2+4, pCmd))
				bError=TRUE;
		}
		else
			if (!m_pDataChannel->Send(SFTP_DATAID_STC_PUT, nLen1+nLen2+4, pCmd))
				bError=TRUE;


		delete [] pCmd;
		break;
	}
	if (bError)
	{ //Error transferring the file	
		DoClose();
		return;
	}
}


void CSFtpControlSocket::Delete(CString filename, const CServerPath &path)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("Delete(\"%s\", \"%s\")"),filename,path.GetPath());
	
	class CDeleteData : public CSFtpControlSocket::t_operation::COpData
	{
	public:
		CDeleteData() {}
		virtual ~CDeleteData() {}
		CString m_FileName;
		CServerPath path;
	};	
	
	if (filename!="")
	{
		ASSERT(!path.IsEmpty());
		ASSERT(m_Operation.nOpMode==CSMODE_NONE);
		ASSERT(m_Operation.nOpState==-1);
		ASSERT(!m_Operation.pData);
		m_Operation.nOpMode=CSMODE_DELETE;
		if (!m_pDataChannel->Send(SFTP_DATAID_STC_DELE, path.GetPath()+filename))
		{
			DoClose();
			return;
		}
		CDeleteData *data=new CDeleteData;
		data->m_FileName=filename;
		data->path=path;
		m_Operation.pData=data;
	}
	else
	{
		ASSERT(path.IsEmpty());
		ASSERT(m_Operation.nOpMode==CSMODE_DELETE);
		ASSERT(m_Operation.nOpState==-1);
		ASSERT(m_Operation.pData);
		if (m_bError)
		{
			ResetOperation(FZ_REPLY_ERROR);
			return;
		}
		else
		{ //Remove file from cached dirs
			CDeleteData *pData=(CDeleteData *)m_Operation.pData;
			CDirectoryCache cache;
			cache.Lock();
			BOOL bCached=TRUE;
			t_directory dir;
			BOOL res=cache.Lookup(pData->path,m_CurrentServer,dir);
			if (!res)
				bCached=FALSE;
			if (!res && m_pDirectoryListing)
			{
				if (m_pDirectoryListing->path==pData->path)
				{
					dir=*m_pDirectoryListing;
					res=TRUE;
				}
			}
			t_directory WorkingDir;
			BOOL bFound=m_pOwner->GetWorkingDir(&WorkingDir);
			if (!res && bFound)
				if (WorkingDir.path==pData->path)
				{
					dir=WorkingDir;
					res=TRUE;
				}
			if (res)
			{
				BOOL found=FALSE;
				for (int i=0;i<dir.num;i++)
				{
					if (dir.direntry[i].name==pData->m_FileName)
					{
						ASSERT(!dir.direntry[i].dir || dir.direntry[i].bLink);
						found=TRUE;
						break;
					}
				}
				if (found)
				{
					t_directory::t_direntry *direntry=new t_directory::t_direntry[dir.num-1];
					int j=0;
					for (int i=0;i<dir.num;i++)
					{
						if (dir.direntry[i].name==pData->m_FileName)
							continue;
						direntry[j]=dir.direntry[i];
						j++;
					}
					delete [] dir.direntry;
					dir.direntry=direntry;
					dir.num--;
					cache.Store(dir, bCached);
					BOOL updated=FALSE;
					if (m_pDirectoryListing)
						if (m_pDirectoryListing->path==dir.path)
						{
							updated=TRUE;
							SetDirectoryListing(&dir);
						}
					if (!updated)
						if (WorkingDir.path==dir.path)
						{
							updated=TRUE;
							m_pOwner->SetWorkingDir(&dir);
						}
				}
			}
			cache.Unlock();
		}
		ResetOperation(FZ_REPLY_OK);
	}
}

void CSFtpControlSocket::Rename(CString oldName, CString newName, const CServerPath &path, const CServerPath &newPath)
{
	LogMessage(__FILE__, __LINE__, this, FZ_LOG_DEBUG, _T("Rename(\"%s\", \"%s\", \"%s\", \"%s\")  OpMode=%d OpState=%d"), oldName, newName, path.GetPath(),
			   newPath.GetPath(), m_Operation.nOpMode, m_Operation.nOpState);
	class CRenameData : public CFtpControlSocket::t_operation::COpData
	{
	public:
		CRenameData() {}
		virtual ~CRenameData() {}
		CString oldName, newName;
		CServerPath path;
		CServerPath newPath;
	};
	if (oldName != "")
	{
		ASSERT(newName != "");
		ASSERT(!path.IsEmpty());
		ASSERT(m_Operation.nOpMode == CSMODE_NONE);
		ASSERT(m_Operation.nOpState == -1);
		ASSERT(!m_Operation.pData);

		USES_CONVERSION;

		m_Operation.nOpMode = CSMODE_RENAME;
		int nLen;
		char *pData;
		if (newPath.IsEmpty())
		{
			nLen = _tcslen( path.GetPath() + oldName + path.GetPath() + newName) + 2;
			pData = new char[nLen];
			strcpy(pData, T2CA(path.GetPath() + oldName));
			strcpy(pData + _tcslen(path.GetPath() + oldName) + 1, T2CA(path.GetPath() + newName));
		}
		else
		{
			nLen = _tcslen( path.GetPath() + oldName + newPath.GetPath() + newName) + 2;
			pData = new char[nLen];
			strcpy(pData, T2CA(path.GetPath() + oldName));
			strcpy(pData + _tcslen(path.GetPath() + oldName) + 1, T2CA(newPath.GetPath() + newName));
		}
		if (!m_pDataChannel->Send(SFTP_DATAID_STC_RENAME, nLen, pData))
		{
			delete [] pData;
			DoClose();
			return;
		}
		delete [] pData;
		CRenameData *data = new CRenameData;
		data->oldName = oldName;
		data->newName = newName;
		data->path = path;
		data->newPath = newPath;
		m_Operation.pData = data;
	}
	else
	{
		ASSERT(oldName == "");
		ASSERT(path.IsEmpty());
		ASSERT(newPath.IsEmpty());
		ASSERT(m_Operation.nOpMode == CSMODE_RENAME);
		ASSERT(m_Operation.pData);
		if (m_bError)
		{
			ResetOperation(FZ_REPLY_ERROR);
			return;
		}

		//Rename entry in cached directory
		CRenameData *pData = reinterpret_cast<CRenameData *>(m_Operation.pData);
			
		CDirectoryCache cache;

		cache.Lock();
				
		//Rename all references to the directory in the cache
		if (pData->newPath.IsEmpty())
			cache.Rename(pData->path, pData->oldName, pData->newName, m_CurrentServer);
		
		BOOL bCached = TRUE;
		t_directory dir;
		BOOL res = cache.Lookup(pData->path, m_CurrentServer, dir);
		if (!res)
			bCached = FALSE;
		if (!res && m_pDirectoryListing)
		{
			if (m_pDirectoryListing->path == pData->path)
			{
				dir = *m_pDirectoryListing;
				res = TRUE;
			}
		}
		t_directory WorkingDir;
		BOOL bFound = m_pOwner->GetWorkingDir(&WorkingDir);
		if (!res && bFound)
			if (WorkingDir.path == pData->path)
			{
				dir = WorkingDir;
				res = TRUE;
			}
		if (res)
		{
			for (int i=0; i<dir.num; i++)
				if (dir.direntry[i].name == pData->oldName)
				{
					if (pData->newPath.IsEmpty())
					{
						dir.direntry[i].name = pData->newName;
						dir.direntry[i].lName = pData->newName;
						dir.direntry[i].lName.MakeLower();

						cache.Store(dir, bCached);
						BOOL updated = FALSE;
						if (m_pDirectoryListing && m_pDirectoryListing->path == dir.path)
						{
							updated = TRUE;
							SetDirectoryListing(&dir, bFound && WorkingDir.path == dir.path);
						}
						if (!updated)
							if (WorkingDir.path == dir.path)
							{
								updated = TRUE;
								m_pOwner->SetWorkingDir(&dir);
							}
					}
					else
					{
						t_directory::t_direntry oldentry = dir.direntry[i];

						for (int j = i+1; j < dir.num; j++)
						{
							dir.direntry[j-1] = dir.direntry[j];
						}
						dir.num--;

						cache.Store(dir, bCached);
							
						// If directory, delete old directory from cache
						t_directory olddir;
						res = cache.Lookup(pData->path, pData->oldName, m_CurrentServer, olddir);
						if (res)
						{
							cache.Purge(olddir.path, m_CurrentServer);
							t_directory newdir;
							if (cache.Lookup(dir.path, m_CurrentServer, newdir))
								dir = newdir;
						}

						BOOL updated = FALSE;
						if (m_pDirectoryListing && m_pDirectoryListing->path == dir.path)
						{
							updated = TRUE;
							SetDirectoryListing(&dir, WorkingDir.path == dir.path);
						}
						if (!updated)
							if (WorkingDir.path == dir.path)
							{
								updated = TRUE;
								m_pOwner->SetWorkingDir(&dir);
							}
																	
						BOOL bCached = TRUE;
						BOOL res = cache.Lookup(pData->newPath, m_CurrentServer, dir);
						if (!res)
							bCached = FALSE;
						if (!res && m_pDirectoryListing)
						{
							if (m_pDirectoryListing->path == pData->newPath)
							{
								dir = *m_pDirectoryListing;
								res = TRUE;
							}
						}
						t_directory WorkingDir;
						BOOL bFound = m_pOwner->GetWorkingDir(&WorkingDir);
						if (!res && bFound)
							if (WorkingDir.path == pData->newPath)
							{
								dir = WorkingDir;
								res = TRUE;
							}
						if (res)
						{
							t_directory::t_direntry *direntry = new t_directory::t_direntry[dir.num + 1];
							for (int i = 0; i < dir.num; i++)
								direntry[i] = dir.direntry[i];
							direntry[dir.num] = oldentry;
							direntry[dir.num].name = pData->newName;
							direntry[dir.num].lName = pData->newName;
							direntry[dir.num].lName.MakeLower();
							dir.num++;
							delete [] dir.direntry;
							dir.direntry = direntry;
									
							cache.Store(dir, bCached);
							BOOL updated = FALSE;
							if (m_pDirectoryListing && m_pDirectoryListing->path == dir.path)
							{
								updated = TRUE;
								SetDirectoryListing(&dir, WorkingDir.path == dir.path);
							}
							if (!updated)
								if (bFound && WorkingDir.path == dir.path)
								{
									updated = TRUE;
									m_pOwner->SetWorkingDir(&dir);
								}
						}
					}
					break;
				}
		}
		cache.Unlock();
		ResetOperation(FZ_REPLY_OK);
	}
}

void CSFtpControlSocket::MakeDir(const CServerPath &path)
{
	//Directory creation works like this:
	//Find existing parent and create subdirs one by one
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("MakeDir(\"%s\")"), path.GetPath());
	
	class CMakeDirData : public CSFtpControlSocket::t_operation::COpData
	{
	public:
		CMakeDirData() {}
		virtual ~CMakeDirData() {}
		CServerPath path;
		CServerPath Current;
		std::list<CString> Segments;
	};

	if (m_Operation.nOpState==MKD_INIT)
	{
		ASSERT(!path.IsEmpty());
		ASSERT(m_Operation.nOpMode==CSMODE_NONE);
		ASSERT(!m_Operation.pData);
		m_Operation.nOpMode=CSMODE_MKDIR;
		if (!m_pDataChannel->Send(SFTP_DATAID_STC_CD, path.GetParent().GetPath()))
		{
			DoClose();
			return;
		}
		CMakeDirData *data=new CMakeDirData;
		data->path=path;
		data->Current=path.GetParent();
		data->Segments.push_front(path.GetLastSegment());
		m_Operation.pData=data;
		m_Operation.nOpState=MKD_FINDPARENT;
	}
	else if (m_Operation.nOpState==MKD_FINDPARENT)
	{
		ASSERT(m_Operation.nOpMode==CSMODE_MKDIR);
		ASSERT(path.IsEmpty());
		ASSERT(m_Operation.pData);
		CMakeDirData *pData=(CMakeDirData *)m_Operation.pData;
		if (!m_bError)
		{
			m_pOwner->SetCurrentPath(pData->Current);
			
			m_Operation.nOpState=MKD_MAKESUBDIRS;
			pData->Current.AddSubdir(pData->Segments.front());
			CString Segment=pData->Segments.front();
			pData->Segments.pop_front();
			if (!m_pDataChannel->Send(SFTP_DATAID_STC_MKD, Segment))
			{
				DoClose();
				return;
			}
			m_Operation.nOpState=MKD_CHANGETOSUBDIR;
		}
		else
		{
			if (!pData->Current.HasParent())
				ResetOperation(FZ_REPLY_ERROR);
			else
			{
				pData->Segments.push_front(pData->Current.GetLastSegment());
				pData->Current=pData->Current.GetParent();
				if (!m_pDataChannel->Send(SFTP_DATAID_STC_CD, pData->Current.GetPath()))
				{
					DoClose();
					return;	
				}
			}
		}
	}
	else if (m_Operation.nOpState==MKD_MAKESUBDIRS)
	{
		if (!m_bError)
		{ //Create dir entry in parent dir
			CMakeDirData *pData=(CMakeDirData *)m_Operation.pData;

			ASSERT(!pData->Segments.empty());

			m_pOwner->SetCurrentPath(pData->Current);
			
			pData->Current.AddSubdir(pData->Segments.front());
			CString Segment=pData->Segments.front();
			pData->Segments.pop_front();
			if (!m_pDataChannel->Send(SFTP_DATAID_STC_MKD,Segment))
			{
				DoClose();
				return;
			}
			m_Operation.nOpState=MKD_CHANGETOSUBDIR;
		}
		else
			ResetOperation(FZ_REPLY_ERROR);
	}
	else if (m_Operation.nOpState==MKD_CHANGETOSUBDIR)
	{
		CMakeDirData *pData=(CMakeDirData *)m_Operation.pData;
		if (!m_bError)
		{ //Create dir entry in parent dir
			CServerPath path2=pData->Current;
			if (path2.HasParent())
			{
				CString name=path2.GetLastSegment();
				path2=path2.GetParent();

				CDirectoryCache cache;
				t_directory dir;
				cache.Lock();
				BOOL bCached = TRUE;
				BOOL res = cache.Lookup(path2, m_CurrentServer,dir);
				if (!res)
					bCached = FALSE;
				if (!res && m_pDirectoryListing)
				{
					if (m_pDirectoryListing->path == path2)
					{
						dir = *m_pDirectoryListing;
						res = TRUE;
					}
				}
				t_directory WorkingDir;
				BOOL bFound = m_pOwner->GetWorkingDir(&WorkingDir);
				if (!res && bFound)
					if (WorkingDir.path == path2)
					{
						dir = WorkingDir;
						res = TRUE;
					}
				if (!res)
				{
					dir.path = path2;
					dir.server = m_CurrentServer;
				}

				int i;
				for (i=0; i<dir.num; i++)
					if (dir.direntry[i].name==name)
					{
						LogMessage(__FILE__, __LINE__, this,FZ_LOG_WARNING, _T("Dir already exists in cache!"));
						break;
					}
				if (i==dir.num)
				{
					t_directory::t_direntry *entries=new t_directory::t_direntry[dir.num+1];
					for (i=0;i<dir.num;i++)
						entries[i]=dir.direntry[i];
					entries[i].name=name;
					entries[i].lName=name;
					entries[i].lName.MakeLower();
					entries[i].dir=TRUE;
					entries[i].date.hasdate=FALSE;
					entries[i].size=-1;
					entries[i].bUnsure=FALSE;
					delete [] dir.direntry;
					dir.direntry=entries;
					dir.num++;

					cache.Store(dir, bCached);
					BOOL updated = FALSE;
					if (m_pDirectoryListing && m_pDirectoryListing->path == dir.path)
					{
						updated = TRUE;
						SetDirectoryListing(&dir, bFound && WorkingDir.path == dir.path);
					}
					if (!updated)
						if (bFound && WorkingDir.path == dir.path)
						{
							updated = TRUE;
							m_pOwner->SetWorkingDir(&dir);
						}
				}
				
				cache.Unlock();
			}
		}
		
		//Continue operation even if MKD failed, maybe another thread did create this directory for us
		if (pData->Segments.empty())
			ResetOperation(FZ_REPLY_OK);
		else
		{
			if (!m_pDataChannel->Send(SFTP_DATAID_STC_CD, pData->Current.GetPath()))
			{
				DoClose();
				return;
			}
			m_Operation.nOpState=MKD_MAKESUBDIRS;
		}				
	}
	else 
		ASSERT(FALSE);
}

void CSFtpControlSocket::RemoveDir(CString dirname, const CServerPath &path)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("RemoveDir(\"%s\", \"%s\")"),dirname, path.GetPath());
	
	class CRemoveDirData : public CSFtpControlSocket::t_operation::COpData
	{
public:
		CRemoveDirData() {}
		virtual ~CRemoveDirData() {}
		CString m_DirName;
		CServerPath path;
	};
	if (dirname!="")
	{
		ASSERT(!path.IsEmpty());
		ASSERT(m_Operation.nOpMode==CSMODE_NONE);
		ASSERT(m_Operation.nOpState==-1);
		ASSERT(!m_Operation.pData);
		m_Operation.nOpMode=CSMODE_RMDIR;
		if (!m_pDataChannel->Send(SFTP_DATAID_STC_RMD, path.GetPath()+dirname))
		{
			DoClose();
			return;
		}
		CRemoveDirData *data=new CRemoveDirData;
		data->m_DirName=dirname;
		data->path=path;
		m_Operation.pData=data;
	}
	else
	{
		ASSERT(path.IsEmpty());
		ASSERT(m_Operation.nOpMode==CSMODE_RMDIR);
		ASSERT(m_Operation.nOpState==-1);
		ASSERT(m_Operation.pData);
		if (!m_bError)
		{ //Remove dir from cached dirs
			CRemoveDirData *pData=(CRemoveDirData *)m_Operation.pData;
			CDirectoryCache cache;
			cache.Lock();
			BOOL bCached=TRUE;
			t_directory dir;
			BOOL res=cache.Lookup(pData->path,m_CurrentServer,dir);
			if (!res)
				bCached=FALSE;
			if (!res && m_pDirectoryListing)
			{
				if (m_pDirectoryListing->path==pData->path)
				{
					dir=*m_pDirectoryListing;
					res=TRUE;
				}
			}
			t_directory WorkingDir;
			BOOL bFound=m_pOwner->GetWorkingDir(&WorkingDir);
			if (!res && bFound)
				if (WorkingDir.path==pData->path)
				{
					dir=WorkingDir;
					res=TRUE;
				}
			if (res)
			{
				BOOL found=FALSE;
				for (int i=0;i<dir.num;i++)
				{
					if (dir.direntry[i].name==pData->m_DirName)
					{
						ASSERT(dir.direntry[i].dir);
						found=TRUE;
						break;
					}
				}
				if (found)
				{
					t_directory::t_direntry *direntry=new t_directory::t_direntry[dir.num-1];
					int j=0;
					for (int i=0;i<dir.num;i++)
					{
						if (dir.direntry[i].name==pData->m_DirName)
							continue;
						direntry[j]=dir.direntry[i];
						j++;
					}
					delete [] dir.direntry;
					dir.direntry=direntry;
					dir.num--;
					cache.Store(dir, bCached);
					BOOL updated=FALSE;
					if (m_pDirectoryListing)
						if (m_pDirectoryListing->path==dir.path)
						{
							updated=TRUE;
							SetDirectoryListing(&dir);
						}
					if (!updated)
						if (WorkingDir.path==dir.path)
						{
							updated=TRUE;
							m_pOwner->SetWorkingDir(&dir);
						}
				}
			}
			if (cache.Lookup(pData->path,pData->m_DirName,m_CurrentServer,dir))
				cache.Purge(dir.path,dir.server);
			cache.Unlock();
			ResetOperation(FZ_REPLY_OK);
		}
		else
			ResetOperation(FZ_REPLY_ERROR);
	}
}

void CSFtpControlSocket::ProcessReply()
{
	if (m_Operation.nOpMode&CSMODE_TRANSFER)
		FileTransfer(0);
	else if (m_Operation.nOpMode&CSMODE_LIST)
		List(FALSE, m_bError);
	else if (m_Operation.nOpMode&CSMODE_DELETE)
		Delete( _T(""),CServerPath());
	else if (m_Operation.nOpMode&CSMODE_RMDIR)
		RemoveDir( _T(""),CServerPath());
	else if (m_Operation.nOpMode&CSMODE_MKDIR)
		MakeDir(CServerPath());
	else if (m_Operation.nOpMode&CSMODE_RENAME)
		Rename(_T(""), _T(""), CServerPath(), CServerPath());
	else if (m_Operation.nOpMode&CSMODE_CHMOD)
		ResetOperation(m_bError?FZ_REPLY_ERROR:FZ_REPLY_OK);
	else if (m_Operation.nOpMode&CSMODE_CONNECT)
		if (m_bError)
			DoClose();
}

void CSFtpControlSocket::TransferEnd(int nMode)
{
}

void CSFtpControlSocket::Cancel(BOOL bQuit /*=FALSE*/)
{
	if (bQuit)
		m_bQuit=TRUE;
	if (m_Operation.nOpMode!=CSMODE_NONE)
		DoClose(FZ_REPLY_CANCEL);
}

int CSFtpControlSocket::OnClientRequest(int nId, int &nDataLength, LPVOID pData)
{
	USES_CONVERSION;
	ASSERT(pData);
	ASSERT(nId);
	ASSERT(nDataLength>=0 && nDataLength<=20480);
	int i;
	switch(nId)
	{
	case SFTP_DATAID_CTS_TRACE:
		for (i=0;i<nDataLength;i++)
		{
			if (!reinterpret_cast<char *>(pData)[i])
			{
				char *str = (char *)pData;
				char *p = str;
				while (p=strstr(str, "\n"))
				{
					*p = 0;
					ShowStatus(str, 3);
					str = p+1;
				}
				LogMessage(FZ_LOG_INFO, _T("FzSFtp.exe: %s"), A2CT(str));
				break;
			}
		}
		break;
	case SFTP_DATAID_CTS_READ:
		{
			if (nDataLength!=4)
				ResetOperation(FZ_REPLY_ERROR);
			else
			{
				int len=*(int *)pData;
				if (len<0 || len>20476)
				{
					ShowStatus("SFTP_DATAID_CTS_READ called with invalid data", 1);
					DoClose();
					return 0;
				}
			
				LPVOID pBuffer = m_pDataChannel->GetBuffer();
				if (!pBuffer)
				{
					DoClose();
					return 0;
				}
				
				bool beenWaiting = FALSE;
				if (m_Operation.nOpMode & CSMODE_DOWNLOAD || m_Operation.nOpMode & CSMODE_UPLOAD)
					len = static_cast<int>(GetAbleToTransferSize(download, beenWaiting, len));

				int res = Receive((int *)pBuffer+1, len);
				if (res==SOCKET_ERROR)
				{
					int error = WSAGetLastError();
					if (error==WSAEWOULDBLOCK)
						AsyncSelect(FD_READ | FD_WRITE | FD_CLOSE);
					res = 4;
					*(int *)pBuffer=0;
					*((int *)pBuffer+1)=error;
				}
				else
				{
					if (m_Operation.nOpMode & CSMODE_DOWNLOAD || m_Operation.nOpMode & CSMODE_UPLOAD)
						SpeedLimitAddTransferredBytes(download, res);
					*(int *)pBuffer=res;
					m_LastRecvTime=CTime::GetCurrentTime();
					if (res!=0)
						PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_SOCKETSTATUS, FZ_SOCKETSTATUS_RECV), 0);
				}
				
				res+=4;
				if (!m_pDataChannel->ReleaseBuffer(SFTP_DATAID_STC_READ, res))
				{
					DoClose();
					return 0;
				}
			}
		}
		break;
	case SFTP_DATAID_CTS_WRITE:
		{
			int len=nDataLength;
			bool beenWaiting = FALSE;
			if (m_Operation.nOpMode & CSMODE_DOWNLOAD || m_Operation.nOpMode & CSMODE_UPLOAD)
				len = static_cast<int>(GetAbleToTransferSize(upload, beenWaiting, len));

			int res=Send(pData, len);
			int error=0;
			if (res==SOCKET_ERROR)
			{
				res=0;
				error=WSAGetLastError();
			}
			else
			{
				if (m_Operation.nOpMode & CSMODE_DOWNLOAD || m_Operation.nOpMode & CSMODE_UPLOAD)
					SpeedLimitAddTransferredBytes(upload, res);
				m_LastSendTime=CTime::GetCurrentTime();
				if (res!=0)
					PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_SOCKETSTATUS, FZ_SOCKETSTATUS_SEND), 0);
			}
			*((int *)pData)=res;
			*((int *)pData+1)=error;
			if (!m_pDataChannel->Send(SFTP_DATAID_STC_WRITE, 8, pData))
			{
				DoClose();
				return 0;
			}
		}
		break;
	case SFTP_DATAID_CTS_CONNECTED:
		m_pOwner->SetConnected(TRUE);
		PostMessage(m_pOwner->m_hOwnerWnd,m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_SECURESERVER, TRUE), 0);
		ShowStatus(IDS_STATUSMSG_CONNECTED,0);
		ResetOperation(FZ_REPLY_OK);
		break;
	case SFTP_DATAID_CTS_PWD:
	case SFTP_DATAID_CTS_CD:
		if (nDataLength<=1 || nDataLength!=static_cast<int>(strlen((char *)pData)+1))
			ResetOperation(FZ_REPLY_ERROR);
		else
		{
			m_Reply=(char *)pData;
			ProcessReply();
		}
		break;
	case SFTP_DATAID_CTS_LIST:
		if (nDataLength && nDataLength!=static_cast<int>(strlen((char *)pData)+1))
			ResetOperation(FZ_REPLY_ERROR);
		else
		{
			if (!nDataLength)
				m_Reply="";
			else
			{
				m_Reply=(char *)pData;
				ASSERT(m_Reply!="");
			}
			
			ProcessReply();
		}
		break;
	case SFTP_DATAID_CTS_ERROR:
		if (nDataLength && nDataLength==static_cast<int>(strlen((char *)pData)+1))
			ShowStatus((char *)pData, 3);
		m_bError=TRUE;
		ProcessReply();
		m_bError=FALSE;
		break;
	case SFTP_DATAID_CTS_STATUS:
		if (nDataLength && nDataLength==static_cast<int>(strlen((char *)pData)+1))
		{
			char *str = (char *)pData;
			char *p = str;
			while (p=strstr(str, "\n"))
			{
				*p = 0;
				ShowStatus(str, 3);
				str = p+1;
			}
			ShowStatus(str, 3);
		}
		break;
	case SFTP_DATAID_CTS_CRITICALERROR:
		if (nDataLength && nDataLength==static_cast<int>(strlen((char *)pData)+1))
		{
			char *str = (char *)pData;
			char *p = str;
			while (p=strstr(str, "\n"))
			{
				*p = 0;
				ShowStatus(str, 3);
				str = p+1;
			}
			ShowStatus(str, 3);
		}
		if (m_Operation.nOpMode&CSMODE_CONNECT)
			DoClose(FZ_REPLY_CRITICALERROR);
		else
			DoClose(FZ_REPLY_ERROR);
		break;
	case SFTP_DATAID_CTS_FATALERROR:
		if (nDataLength && nDataLength==static_cast<int>(strlen((char *)pData)+1))
		{
			char *str = (char *)pData;
			char *p = str;
			while (p=strstr(str, "\n"))
			{
				*p = 0;
				ShowStatus(str, 3);
				str = p+1;
			}
			ShowStatus(str, 3);
		}
		DoClose(FZ_REPLY_ERROR);
		break;
	case SFTP_DATAID_CTS_MKD:
	case SFTP_DATAID_CTS_DELE:
	case SFTP_DATAID_CTS_RMD:
	case SFTP_DATAID_CTS_RENAME:
	case SFTP_DATAID_CTS_CHMOD:
		if (nDataLength && nDataLength!=static_cast<int>(strlen((char *)pData)+1))
			ResetOperation(FZ_REPLY_ERROR);
		else
		{
			ShowStatus((char *)pData, 3);
			ProcessReply();
		}
		break;
	case SFTP_DATAID_CTS_GET:
	case SFTP_DATAID_CTS_PUT:
		ProcessReply();
		break;
	case SFTP_DATAID_CTS_TRANSFERSTATUS:
		if (nDataLength!=4)
			ResetOperation(FZ_REPLY_ERROR);
		else
			UpdateTransferstatus(*(int *)pData);
		break;
	case SFTP_DATAID_CTS_NEWHOSTKEY:
		{
			CNewHostKeyRequestData *pRequestData=new CNewHostKeyRequestData;
			pRequestData->nRequestID=m_pOwner->GetNextAsyncRequestID();
			pRequestData->Hostkey=(char *)pData;
			if (!PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_ASYNCREQUEST, FZ_ASYNCREQUEST_NEWHOSTKEY), (LPARAM)pRequestData))
			{
				delete pRequestData;
				DoClose();
			}				
		}
		break;
	case SFTP_DATAID_CTS_CHANGEDHOSTKEY:
		{
			CChangedHostKeyRequestData *pRequestData=new CChangedHostKeyRequestData;
			pRequestData->nRequestID=m_pOwner->GetNextAsyncRequestID();
			pRequestData->Hostkey=(char *)pData;
			if (!PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_ASYNCREQUEST, FZ_ASYNCREQUEST_CHANGEDHOSTKEY), (LPARAM)pRequestData))
			{
				delete pRequestData;
				DoClose();
			}				
		}
		break;
	case SFTP_DATAID_CTS_KEYBOARD_INTERACTIVE:
		{
			CKeyboardInteractiveRequestData *pRequestData = new CKeyboardInteractiveRequestData;
			pRequestData->nRequestID = m_pOwner->GetNextAsyncRequestID();
			pRequestData->nRequestType = FZ_ASYNCREQUEST_KEYBOARDINTERACTIVE;
			memcpy(pRequestData->data, pData, nDataLength);
			if (!PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_ASYNCREQUEST, FZ_ASYNCREQUEST_KEYBOARDINTERACTIVE), (LPARAM)pRequestData))
			{
				delete pRequestData;
				DoClose();
			}				
		}
		break;
	}
		
	return 0;
}

void CSFtpControlSocket::OnConnect(int nErrorCode)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("OnConnect(%d)"), nErrorCode);
	USES_CONVERSION;
	if (!m_Operation.nOpMode)
	{
		if (!m_pOwner->IsConnected())
			DoClose();
		return;
	}
	if (!nErrorCode)
	{
		/* Set internal socket send buffer to 64k,
		 * this should fix the speed problems some users have reported
		 */
		DWORD value;
		int len = sizeof(value);
		GetSockOpt(SO_SNDBUF, &value, &len);
		if (value < 65536)
		{
			value = 65536;
			SetSockOpt(SO_SNDBUF, &value, sizeof(value));
		}
	    
		AsyncSelect(FD_WRITE | FD_CLOSE);
		if (!m_pOwner->IsConnected())
		{
			m_pOwner->SetConnected(TRUE);
			CString str;

			str.Format(IDS_STATUSMSG_CONNECTEDWITHSFTP, m_ServerName);
			ShowStatus(str,0);
			char *data = new char[strlen(T2CA(m_CurrentServer.host)) + strlen(T2CA(m_CurrentServer.user)) + strlen(T2CA(m_CurrentServer.pass)) + 4 + 3 + 2];
			char *p=data;
			strcpy(p, T2CA(m_CurrentServer.host));
			p+=strlen(p)+1;
			*(int *)p=m_CurrentServer.port;
			p+=4;
			strcpy(p, T2CA(m_CurrentServer.user));
			p+=strlen(p)+1;
			strcpy(p, T2CA(m_CurrentServer.pass));
			p+=strlen(p)+1;
			*p++ = COptions::GetOptionVal(OPTION_SSHUSECOMPRESSION);
			*p++ = COptions::GetOptionVal(OPTION_SSHPROTOCOL);
			const BOOL bTrace=GetDebugLevel()>=3;
			if (!m_pDataChannel->Send(SFTP_DATAID_STC_TRACE, 4, (LPVOID)&bTrace))
			{
				delete [] data;
				DoClose();
				return;
			}
			if (!m_pDataChannel->Send(SFTP_DATAID_STC_CONNECT, p-data, data))
			{
				delete [] data;
				DoClose();
				return;
			}
			delete [] data;		
		}
	}
	else
	{
		if (nErrorCode == WSAHOST_NOT_FOUND)
			ShowStatus(IDS_ERRORMSG_CANTRESOLVEHOST, 1);
		DoClose();
	}
}

void CSFtpControlSocket::OnClientError(int nErrorCode)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("OnClientError(%d)"), nErrorCode);
	switch(nErrorCode)
	{
	case 1:
		ShowStatus(IDS_ERRORMSG_SFTP_CLOSED,1);
		break;
	case 2:
		ShowStatus(IDS_ERRORMSG_SFTP_NORESPONSE,1);
		break;
	case 3:
	default:
		ShowStatus(IDS_ERRORMSG_SFTP_INVALIDDATA,1);
		break;
	}
	DoClose();
	return;
}

void CSFtpControlSocket::DoClose(int nError /*=0*/)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("DoClose(%d)"),nError);
	
	m_bCheckForTimeout=TRUE;
	m_pOwner->SetConnected(FALSE);
	PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_SECURESERVER, FALSE), 0);
	
	if (nError & FZ_REPLY_CRITICALERROR)
		nError |= FZ_REPLY_ERROR;
	ResetOperation(FZ_REPLY_ERROR|FZ_REPLY_DISCONNECTED|nError);

	if (m_pDataChannel)
		delete m_pDataChannel;
	m_pDataChannel=0;
	if (m_hSFtpProcess)
	{
		int res=WaitForSingleObject(m_hSFtpProcess, 1000);
		//if (res==WAIT_FAILED || res==WAIT_TIMEOUT)
			TerminateProcess(m_hSFtpProcess, 0);
		CloseHandle(m_hSFtpProcess);
	}
		
	m_hSFtpProcess=0;

	m_bQuit=FALSE;

	CControlSocket::Close();
}

void CSFtpControlSocket::ResetOperation(int nSuccessful /*=-1*/)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("ResetOperation(%d)"),nSuccessful);

	if (nSuccessful & FZ_REPLY_CRITICALERROR)
		nSuccessful |= FZ_REPLY_ERROR;

	//There may be an active ident socket, close it
	if (m_pIdentControl)
	{
		delete m_pIdentControl;
		m_pIdentControl=0;
	}

	if (m_Operation.nOpMode)
	{	
		//Unset busy attribute so that new commands can be executed
		m_pOwner->SetBusy(FALSE);

		if (m_Operation.nOpMode&CSMODE_CONNECT && nSuccessful&FZ_REPLY_ERROR)
		{
			nSuccessful|=FZ_REPLY_DISCONNECTED;
			if (!m_bQuit)
				ShowStatus(IDS_ERRORMSG_CANTCONNECT,1);
		}
		if (m_Operation.nOpMode & (CSMODE_LIST|CSMODE_TRANSFER))
		{
			RemoveActiveTransfer();
			PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_TRANSFERSTATUS, 0), 0);
			if (nSuccessful==FZ_REPLY_OK)
				m_LastSendTime=CTime::GetCurrentTime();
		}
		
		//Update remote file entry
		if (m_Operation.pData && 
			m_Operation.nOpMode&CSMODE_TRANSFER && 
			(!((CFileTransferData *)m_Operation.pData)->transferfile.get) && 
			m_pDirectoryListing &&
			m_Operation.nOpState>=FILETRANSFER_RETRSTOR)
		{
			CString filename=((CFileTransferData *)m_Operation.pData)->transferfile.remotefile;
			CServerPath path=((CFileTransferData*)m_Operation.pData)->transferfile.remotepath;
			CDirectoryCache cache;
			cache.Lock();
			t_directory dir;
			BOOL bCached = TRUE;
			BOOL res = cache.Lookup(path, m_CurrentServer, dir);
			if (!res)
				bCached = FALSE;
			if (!res && m_pDirectoryListing)
			{
				if (m_pDirectoryListing->path==path)
				{
					dir=*m_pDirectoryListing;
					res=TRUE;
				}
			}
			t_directory WorkingDir;
			BOOL bFound = m_pOwner->GetWorkingDir(&WorkingDir);
			if (!res && bFound)
				if (WorkingDir.path == path)
				{
					dir = WorkingDir;
					res = TRUE;
				}
			if (res)
			{	
				int i;
				for (i=0; i<dir.num; i++)
					if (dir.direntry[i].name==filename)
					{
						dir.direntry[i].bUnsure = TRUE;
						if (nSuccessful & FZ_REPLY_ERROR)
						{
							dir.direntry[i].bUnsure = TRUE;
							if (!((CFileTransferData *)m_Operation.pData)->transferfile.get)
								dir.direntry[i].size = -1;
						}
						else if (nSuccessful & FZ_REPLY_ERROR)
						{
							if (!GetLength64(((CFileTransferData *)m_Operation.pData)->transferfile.localfile, dir.direntry[i].size))
								dir.direntry[i].size = -1;
						}
						dir.direntry[i].date.hasdate = false;
						break;
					}
				if (i == dir.num)
				{
					t_directory::t_direntry *entries=new t_directory::t_direntry[dir.num+1];
					for (i=0; i<dir.num; i++)
						entries[i]=dir.direntry[i];
					entries[i].name=filename;
					entries[i].lName=filename;
					entries[i].lName.MakeLower();
					entries[i].dir=FALSE;
					if (nSuccessful&FZ_REPLY_OK)
						entries[i].bUnsure=FALSE;
					else
						entries[i].bUnsure=TRUE;
					entries[i].date.hasdate=FALSE;
					entries[i].size=-1;
				
					delete [] dir.direntry;
					dir.direntry=entries;
					dir.num++;				
				}
				CDirectoryCache cache;
				cache.Store(dir, bCached);
				BOOL updated=FALSE;
				if (m_pDirectoryListing && m_pDirectoryListing->path==dir.path)
				{
					updated=TRUE;
					SetDirectoryListing(&dir, bFound && WorkingDir.path == dir.path);
				}
				if (!updated)
					if (bFound && WorkingDir.path == dir.path)
					{
						updated = TRUE;
						m_pOwner->SetWorkingDir(&dir);
					}
			}
			cache.Unlock();
		}
		if (!m_bQuit)
		{
			if (m_Operation.pData && nSuccessful&FZ_REPLY_ERROR)
			{
				if (m_Operation.nOpMode&CSMODE_TRANSFER)
					if (nSuccessful&FZ_REPLY_ABORTED)
						//Transfer aborted by user
						ShowStatus((m_Operation.nOpMode&CSMODE_DOWNLOAD)?IDS_ERRORMSG_DOWNLOADABORTED:IDS_ERRORMSG_UPLOADABORTED,1);
					else
						ShowStatus(((CFileTransferData *)m_Operation.pData)->transferfile.get?IDS_ERRORMSG_DOWNLOADFAILED:IDS_ERRORMSG_UPLOADFAILED,1);
				else if (m_Operation.nOpMode&CSMODE_LIST)
					ShowStatus(IDS_ERRORMSG_CANTGETLIST,1);
			}
			else if (m_Operation.pData && m_Operation.nOpMode&CSMODE_TRANSFER && nSuccessful==FZ_REPLY_OK)
				ShowStatus(((CFileTransferData *)m_Operation.pData)->transferfile.get?IDS_STATUSMSG_DOWNLOADSUCCESSFUL:IDS_STATUSMSG_UPLOADSUCCESSFUL,0);
	
			if (nSuccessful&FZ_REPLY_CANCEL)
				ShowStatus(IDS_ERRORMSG_INTERRUPTED, 1);
		}
	}
	else
	{
		//No operation in progress
		nSuccessful&=FZ_REPLY_DISCONNECTED|FZ_REPLY_CANCEL;
		if (!nSuccessful)
			ASSERT(FALSE);
	}
	if (nSuccessful&FZ_REPLY_DISCONNECTED && m_Operation.nOpMode!=CSMODE_CONNECT && !m_bQuit)
		if (m_Operation.nOpMode!=CSMODE_DISCONNECT)
			ShowStatus(IDS_STATUSMSG_DISCONNECTED, 1); //Send the disconnected message to the message log	
		else
			ShowStatus(IDS_STATUSMSG_DISCONNECTED, 0); //Send the disconnected message to the message log	
	
	if (nSuccessful&FZ_REPLY_DISCONNECTED)
		m_pOwner->SetWorkingDir(0); //Disconnected, reset working dir
	
	Sleep(0);
	if (m_Operation.nOpMode)
		VERIFY(PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_REPLY, m_pOwner->m_LastCommand.id), nSuccessful));
	else
		VERIFY(PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_REPLY, 0), nSuccessful));

	m_Operation.nOpMode=0;
	m_Operation.nOpState=-1;

	if (m_Operation.pData)
		delete m_Operation.pData;
	m_Operation.pData=0;
}

void CSFtpControlSocket::OnReceive(int nErrorCode)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("OnReceive(%d)"), nErrorCode);
	if (!m_pDataChannel->Send(SFTP_DATAID_STC_FDREAD, 0, 0))
		DoClose();
	AsyncSelect(FD_WRITE | FD_CLOSE);
}

void CSFtpControlSocket::OnClose(int nErrorCode)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("OnClose(%d)"), nErrorCode);
	if (m_Operation.nOpMode)
	{
		if (!m_pDataChannel->Send(SFTP_DATAID_STC_FDREAD, 0, 0))
			DoClose();
	}
	else
	{
		ShowStatus(IDS_STATUSMSG_DISCONNECTED, 1);
		DoClose();
	}
}

void CSFtpControlSocket::OnSend(int nErrorCode)
{
	if (!m_pDataChannel->Send(SFTP_DATAID_STC_FDWRITE, 0, 0))
		DoClose();
}

int CSFtpControlSocket::CheckOverwriteFile()
{
	CFileTransferData *pData = reinterpret_cast<CFileTransferData *>(m_Operation.pData);
	if (!pData)
		return FZ_REPLY_ERROR;

	int nReplyError=0;
	CFileStatus64 status;
	BOOL res=GetStatus64(pData->transferfile.localfile, status);
	if (!res)
		if (!pData->transferfile.get)
			nReplyError=FZ_REPLY_ERROR | FZ_REPLY_CRITICALERROR; //File has to exist when uploading
		else
			m_Operation.nOpState=FILETRANSFER_RETRSTOR;
	else
	{
		if (status.m_attribute&0x10)
			nReplyError=FZ_REPLY_ERROR | FZ_REPLY_CRITICALERROR; //Can't transfer to/from dirs
		else
		{			
			_int64 localsize;
			if (!GetLength64(pData->transferfile.localfile, localsize))
				if (!pData->transferfile.get)
					nReplyError=FZ_REPLY_ERROR | FZ_REPLY_CRITICALERROR;
				else
					m_Operation.nOpState=FILETRANSFER_RETRSTOR;

			CTime *localtime = NULL;
			TRY
			{
				if (status.m_has_mtime && status.m_mtime != -1)
					localtime = new CTime(status.m_mtime);
			}
			CATCH_ALL(e)
			{
				TCHAR buffer[1024];
				CString str =_T("Exception creating CTime object: ");
				if (e->GetErrorMessage(buffer, 1024, NULL))
					str += buffer;
				else
					str += _T("Unknown exception");
				LogMessage(__FILE__, __LINE__, this, FZ_LOG_WARNING, str);
				localtime = NULL;
			}
			END_CATCH_ALL;

			BOOL bRemoteFileExists=FALSE;
			__int64 remotesize=-1;
			CTime *remotetime=0;
			if (m_pDirectoryListing)
				for (int i=0; i<m_pDirectoryListing->num; i++)
				{
					CString remotefile=pData->transferfile.remotefile;
					if (m_pDirectoryListing->direntry[i].name==remotefile)
					{
						remotesize=m_pDirectoryListing->direntry[i].size;
						if (m_pDirectoryListing->direntry[i].date.hasdate)
						{
							TRY
							{
								remotetime = new CTime(m_pDirectoryListing->direntry[i].date.year,
									m_pDirectoryListing->direntry[i].date.month,
									m_pDirectoryListing->direntry[i].date.day,
									m_pDirectoryListing->direntry[i].date.hastime?m_pDirectoryListing->direntry[i].date.hour:0,
									m_pDirectoryListing->direntry[i].date.hastime?m_pDirectoryListing->direntry[i].date.minute:0,
									0,-1);
							}
							CATCH_ALL(e)
							{
								TCHAR buffer[1024];
								CString str =_T("Exception creating CTime object: ");
								if (e->GetErrorMessage(buffer, 1024, NULL))
									str += buffer;
								else
									str += _T("Unknown exception");
								LogMessageRaw(__FILE__, __LINE__, this, FZ_LOG_WARNING, str);
								remotetime = NULL;
							}
							END_CATCH_ALL;
						}
						bRemoteFileExists = TRUE;
						break;
					}
				}

			if (bRemoteFileExists || pData->transferfile.get )
			{
				COverwriteRequestData *pOverwriteData = new COverwriteRequestData;
				t_transferfile *pTransferFile = new t_transferfile;
				*pTransferFile=pData->transferfile;
				pOverwriteData->pTransferFile=pTransferFile;
				if (pData->transferfile.get)
				{
					int pos=pData->transferfile.localfile.ReverseFind('\\');
					ASSERT(pos!=-1);
					pOverwriteData->FileName1=pData->transferfile.localfile.Mid(pos+1);
					pOverwriteData->FileName2=pData->transferfile.remotefile;
					pOverwriteData->path1=pData->transferfile.localfile.Left(pos+1);
					pOverwriteData->path2=pData->transferfile.remotepath.GetPath();
					pOverwriteData->size1=localsize;
					pOverwriteData->size2=remotesize;
					pOverwriteData->time1=localtime;
					pOverwriteData->time2=remotetime;
				}
				else
				{
					int pos=pData->transferfile.localfile.ReverseFind('\\');
					ASSERT(pos!=-1);
					pOverwriteData->FileName1=pData->transferfile.remotefile;
					pOverwriteData->FileName2=pData->transferfile.localfile.Mid(pos+1);
					pOverwriteData->path1=pData->transferfile.remotepath.GetPath();
					pOverwriteData->path2=pData->transferfile.localfile.Left(pos+1);
					pOverwriteData->size1=remotesize;
					pOverwriteData->size2=localsize;
					pOverwriteData->time1=remotetime;
					pOverwriteData->time2=localtime;								
				}
				pOverwriteData->nRequestID=m_pOwner->GetNextAsyncRequestID();
				if (!PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_ASYNCREQUEST, FZ_ASYNCREQUEST_OVERWRITE), (LPARAM)pOverwriteData))
				{
					delete pData;
					nReplyError=FZ_REPLY_ERROR;
				}
				else
				{
					m_bCheckForTimeout=FALSE;
					m_Operation.nOpState=FILETRANSFER_WAIT;
				}
			}
			else
			{
				m_Operation.nOpState=FILETRANSFER_RETRSTOR;
				delete localtime;
				delete remotetime;
			}
		}
	}
	return nReplyError;
}

void CSFtpControlSocket::SetFileExistsAction(int nAction, COverwriteRequestData *pData)
{
	if (!pData)
		return;
	if (!(m_Operation.nOpMode&CSMODE_TRANSFER))
		return;
	if (m_Operation.nOpState!=FILETRANSFER_WAIT)
		return;
	m_bCheckForTimeout=TRUE;
	int nReplyError=0;
	switch (nAction)
	{
	case FILEEXISTS_SKIP:
		nReplyError=FZ_REPLY_OK;
		break;
	case FILEEXISTS_OVERWRITE:
		((CFileTransferData *)m_Operation.pData)->nWaitNextOpState=FILETRANSFER_RETRSTOR;
		break;
	case FILEEXISTS_OVERWRITEIFNEWER:
		if ( !pData->time1 || !pData->time2 )
			((CFileTransferData *)m_Operation.pData)->nWaitNextOpState=FILETRANSFER_RETRSTOR;
		else if (*pData->time2<=*pData->time1)
			nReplyError=FZ_REPLY_OK;
		else
			((CFileTransferData *)m_Operation.pData)->nWaitNextOpState=FILETRANSFER_RETRSTOR;
		break;
	case FILEEXISTS_RENAME:
		if (((CFileTransferData *)m_Operation.pData)->transferfile.get)
		{
			CFileStatus64 status;
			if (GetStatus64(pData->FileName1, status))
			{
				ShowStatus(IDS_ERRORMSG_NAMEINUSE,1);
				nReplyError=FZ_REPLY_ERROR | FZ_REPLY_CRITICALERROR;
			}
			else
			{
				((CFileTransferData *)m_Operation.pData)->transferfile.localfile=pData->path1+pData->FileName1;

				//Replace invalid characters in the local filename
				int pos=((CFileTransferData *)m_Operation.pData)->transferfile.localfile.ReverseFind('\\');
				for (int i=(pos+1);i<((CFileTransferData *)m_Operation.pData)->transferfile.localfile.GetLength();i++)
					if (((CFileTransferData *)m_Operation.pData)->transferfile.localfile[i]==':')
						((CFileTransferData *)m_Operation.pData)->transferfile.localfile.SetAt(i, '_');
					
				((CFileTransferData *)m_Operation.pData)->nWaitNextOpState=FILETRANSFER_RETRSTOR;
			}						
		}
		else
		{
			ASSERT(m_pDirectoryListing);
			int i;
			for (i=0; i<m_pDirectoryListing->num; i++)
			{
				if (m_pDirectoryListing->direntry[i].name==pData->FileName1)
				{
					ShowStatus(IDS_ERRORMSG_NAMEINUSE,1);
					nReplyError=FZ_REPLY_ERROR | FZ_REPLY_CRITICALERROR;
					break;
				}
			}
			if (i==m_pDirectoryListing->num)
			{
				((CFileTransferData *)m_Operation.pData)->transferfile.remotefile=pData->FileName1;
				((CFileTransferData *)m_Operation.pData)->nWaitNextOpState=FILETRANSFER_RETRSTOR;
			}
		}
		break;
	case FILEEXISTS_RESUME:
		if (pData->size1>=0)
		{
			((CFileTransferData *)m_Operation.pData)->transferdata.bResume=TRUE;
/*			if (pData->size2>=0)
				if (pData->size1>=pData->size2)
					nReplyError=FZ_REPLY_OK;*/
		}
		((CFileTransferData *)m_Operation.pData)->nWaitNextOpState=FILETRANSFER_RETRSTOR;
		break;
	}
	if (nReplyError==FZ_REPLY_OK)
		ResetOperation(FZ_REPLY_OK);
	else if (nReplyError)
		ResetOperation(FZ_REPLY_ERROR|nReplyError); //Error transferring the file	
	else
		FileTransfer();
}

void CSFtpControlSocket::UpdateTransferstatus(int len)
{
	if ( !( m_Operation.nOpMode&(CSMODE_LIST|CSMODE_TRANSFER) ))
		return;
	
	//Update the statusbar
	if (m_Operation.nOpMode&CSMODE_TRANSFER)
	{
		CTime *pStartTime=((CFileTransferData *)m_Operation.pData)->pStartTime;
		if (!pStartTime || !len)
			return;
		
		t_transferdata &transferdata = ((CFileTransferData *)m_Operation.pData)->transferdata;
		if (transferdata.transferleft>=0 && transferdata.transfersize>0)
		{
			transferdata.transferleft-=len;
			if (transferdata.transferleft<0)
				transferdata.transferleft=0;
		}
		
		//Don't flood the main window with messages
		//Else performance would be really low
		LARGE_INTEGER curtime;
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&curtime);
		static LARGE_INTEGER oldtime={0};
		if ( ( (curtime.QuadPart-oldtime.QuadPart) < (freq.QuadPart/50) ))
			return;
		oldtime = curtime;

		CTimeSpan timespan=CTime::GetCurrentTime()-*pStartTime;
		int elapsed=static_cast<int>(timespan.GetTotalSeconds());
		
		t_ffam_transferstatus *status=new t_ffam_transferstatus;
		status->bFileTransfer = m_Operation.nOpMode & (CSMODE_UPLOAD | CSMODE_DOWNLOAD);
		status->timeelapsed=elapsed;
		status->bytes=transferdata.transfersize-transferdata.transferleft;
		if (transferdata.transfersize>0)
		{
			double leftmodifier=static_cast<double>(transferdata.transfersize-transferdata.nTransferStart-transferdata.transferleft);
			leftmodifier*=100;
			leftmodifier/=(transferdata.transfersize-transferdata.nTransferStart);
			if (leftmodifier==0)
				leftmodifier=1;
			double leftmodifier2=100-leftmodifier;
			int left=static_cast<int>((elapsed/leftmodifier)*leftmodifier2);
			double percent=100*static_cast<double>(transferdata.transfersize-transferdata.transferleft);
			percent/=transferdata.transfersize;
			if (percent>100)
				percent=100;
			status->percent=static_cast<int>(percent);
			if (left < 0)
				left = -1;
			status->timeleft = left;		
		}
		else
		{
			status->percent=-1;
			status->timeleft=-1;
		}
		
		status->transferrate=(elapsed&&(transferdata.transfersize-transferdata.transferleft-transferdata.nTransferStart))?
			static_cast<int>( (transferdata.transfersize-transferdata.transferleft-transferdata.nTransferStart)/elapsed ):
			0;
		
		PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_TRANSFERSTATUS, 0), (LPARAM)status);
	}
}

void CSFtpControlSocket::Chmod(CString filename, const CServerPath &path, int nValue)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("Chmod(\"%s\", \"%s\", %d)"), filename, path.GetPath(), nValue);
	USES_CONVERSION;
	m_Operation.nOpMode=CSMODE_CHMOD;
	CString str;
	str.Format( _T("%d %s%s"), nValue, path.GetPath(), filename);
	int nLen = strlen(T2CA(str)) + 1;
	char *pData=new char[nLen];
	strcpy(pData, T2CA(str));
	int pos(str.Find( _T(" ") ));
	ASSERT(pos!=-1);
	pData[pos]=0;
	if (!m_pDataChannel->Send(SFTP_DATAID_STC_CHMOD, nLen, pData))
		DoClose();
	delete [] pData;
}

void CSFtpControlSocket::SetAsyncRequestResult(int nAction, CAsyncRequestData *pData)
{
	switch (pData->nRequestType)
	{
	case FZ_ASYNCREQUEST_OVERWRITE:
		SetFileExistsAction(nAction, (COverwriteRequestData *)pData);
		break;
	case FZ_ASYNCREQUEST_NEWHOSTKEY:
	case FZ_ASYNCREQUEST_CHANGEDHOSTKEY:
		if (m_Operation.nOpMode!=CSMODE_CONNECT)
			break;
		if (!m_pDataChannel->Send(SFTP_DATAID_STC_HOSTKEYREPLY, 4, &pData->nRequestResult))
		{
			DoClose();
			return;
		}
		break;
/*	case FZ_ASYNCREQUEST_VERIFYCERT:
		SetVerifyCertResult(nAction, ((CVerifyCertRequestData *)pData)->pCertData );
		break;*/
	case FZ_ASYNCREQUEST_KEYBOARDINTERACTIVE:
		if (!nAction)
		{
			DoClose(FZ_REPLY_CANCEL);
			return;
		}
		if (!m_pDataChannel->Send(SFTP_DATAID_STC_KEYBOARD_INTERACTIVE, strlen(reinterpret_cast<CKeyboardInteractiveRequestData *>(pData)->data) + 1, reinterpret_cast<CKeyboardInteractiveRequestData *>(pData)->data))
		{
			DoClose();
			return;
		}
		break;
	default:
		LogMessage(__FILE__, __LINE__, this,FZ_LOG_WARNING, _T("Unknown request reply %d"), pData->nRequestType);
		break;
	}
}
