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

// QueueCtrl.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "QueueCtrl.h"
#include "queueview.h"
#include "mainfrm.h"
#include "ftplistctrl.h"
#include "commandqueue.h"
#include "FileExistsDlg.h"
#include "StatusView.h"
#include "FtpTreeView.h"
#include "LocalView2.h"
#include "misc\MarkupSTL.h"
#include "AsyncRequestQueue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CQueueData::CQueueData()
{
	bActive = FALSE;
	nOpen = 0;
	nState = 0;
	priority = 0;
	pTransferStatus = NULL;
	retrycount = 0;
	pTransferApi = NULL;
	bStop = FALSE;
	nAutoUsePrimary = 0;
	bTransferStarted = FALSE;
	bTriedAutoResume = false;
	bPaused = FALSE;
	bAbort = FALSE;
	nProgressOffset = 260;
	nListItemState = 0;
	pProgressControl = NULL;
}

CQueueData::~CQueueData()
{
}

/////////////////////////////////////////////////////////////////////////////
// CQueueCtrl

CQueueCtrl::CQueueCtrl()
{
	m_nProcessQueue = 0;
	m_bMayUsePrimaryConnection = FALSE;
	m_nActiveCount = 0;
	m_nMaxApiCount = COptions::GetOptionVal(OPTION_TRANSFERAPICOUNT);
	m_bPrimaryConnectionTransferInProgress = FALSE;
	m_bUseMultiple = COptions::GetOptionVal(OPTION_TRANSFERUSEMULTIPLE);
	m_bQuit = FALSE;
	m_nQuitCount = 0;
}

CQueueCtrl::~CQueueCtrl()
{
	if (!m_bQuit)
	{
		for (UINT i=0; i<m_TransferApiArray.size(); i++)
		{
			m_TransferApiArray[i].pTransferApi->Destroy();
			delete m_TransferApiArray[i].pTransferApi;
			delete m_TransferApiArray[i].pLastActiveTime;
		}
	}
	m_TransferApiArray.clear();

	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		delete iter->pTransferStatus;
		delete iter->pProgressControl;
	}
	m_QueueItems.clear();
}

BEGIN_MESSAGE_MAP(CQueueCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CQueueCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_QUEUECONTEXT_MOVEDOWN, OnQueuecontextMovedown)
	ON_COMMAND(ID_QUEUECONTEXT_MOVETOBOTTOM, OnQueuecontextMovetobottom)
	ON_COMMAND(ID_QUEUECONTEXT_MOVETOTOP, OnQueuecontextMovetotop)
	ON_COMMAND(ID_QUEUECONTEXT_MOVEUP, OnQueuecontextMoveup)
	ON_COMMAND(ID_QUEUECONTEXT_PROCESSQUEUE, OnQueuecontextProcessqueue)
	ON_COMMAND(ID_QUEUECONTEXT_REMOVEFROMQUEUE, OnQueuecontextRemovefromqueue)
	ON_COMMAND(ID_QUEUECONTEXT_RESETSTATUS, OnQueuecontextResetstatus)
	ON_WM_KEYDOWN()
	ON_WM_DROPFILES()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_QUEUECONTEXT_ABORT, OnQueuecontextAbort)
	ON_COMMAND(ID_QUEUECONTEXT_PAUSE, OnQueuecontextPause)
	ON_COMMAND(ID_QUEUECONTEXT_RESUME, OnQueuecontextResume)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_WM_CHAR()
	ON_NOTIFY_REFLECT(LVN_ODSTATECHANGED, OnOdstatechanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CQueueCtrl 

int CQueueCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetCallbackMask(LVIS_SELECTED);
	
	CString str;
	str.LoadString(IDS_HEADER_LOCALNAME);
	InsertColumn(0, str, LVCFMT_LEFT, 200);
	str.LoadString(IDS_HEADER_SIZE);
	InsertColumn(1, str, LVCFMT_RIGHT, 70);
	str.LoadString(IDS_HEADER_DIRECTION);
	InsertColumn(2, str, LVCFMT_CENTER, 65);
	str.LoadString(IDS_HEADER_REMOTENAME);
	InsertColumn(3, str, LVCFMT_LEFT, 200);
	str.LoadString(IDS_HEADER_HOST);
	InsertColumn(4, str, LVCFMT_LEFT, 100);
	str.LoadString(IDS_HEADER_STATUS);
	InsertColumn(5, str, LVCFMT_LEFT, 200);

	DragAcceptFiles(TRUE);

	SetExtendedStyle(LVS_EX_INFOTIP);

	t_TransferApi api={0};
	m_TransferApiArray.resize(m_nMaxApiCount);
	for (UINT i = 0;i < m_nMaxApiCount; i++)
	{
		api.pTransferApi = new CFileZillaApi();
		if (!api.pTransferApi)
			return -1;
		if (api.pTransferApi->Init(GetSafeHwnd(), WM_APP + 10 + i)!=FZ_REPLY_OK)
			return -1;
		api.pTransferApi->SetDebugLevel(COptions::GetOptionVal(OPTION_DEBUGTRACE)?4:0);
		m_TransferApiArray[i] = api;
	}

	m_nTimerID = SetTimer(1234, 1000, 0);
	
	return 0;
}

int CQueueCtrl::AddItem(t_transferfile transferfile, BOOL stdtransfer /*=FALSE*/, int nOpen /*=0*/)
{
	CQueueData queueData;

	queueData.priority = stdtransfer?2:1;
	queueData.nOpen = nOpen;
	
	m_bMayUsePrimaryConnection = IsQueueEmpty();

	queueData.transferFile = transferfile;
	queueData.transferFile.nUserData = nOpen;

	m_QueueItems.insert(FindValidIndex(queueData.priority), queueData);
	SetItemCount(m_QueueItems.size() + m_nActiveCount);
	UpdateStatusbar();
	return 1;
}

int CQueueCtrl::AddItem(CQueueData queueData)
{
	queueData.pProgressControl = 0;

	queueData.transferFile.nUserData = queueData.nOpen;

	if (!queueData.priority)
		queueData.priority = 1;
	
	m_bMayUsePrimaryConnection = IsQueueEmpty();

	m_QueueItems.insert(FindValidIndex(queueData.priority), queueData);
	SetItemCount(m_QueueItems.size() + m_nActiveCount);
	UpdateStatusbar();
	
	return 1;
}

void CQueueCtrl::OnDestroy() 
{
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		delete iter->pProgressControl;
		iter->pProgressControl = 0;
	}

	if (m_bQuit)
	{
		CListCtrl::OnDestroy();
		return;
	}

	for (UINT i=0; i<m_TransferApiArray.size(); i++)
	{
		m_TransferApiArray[i].pTransferApi->Destroy();
		delete m_TransferApiArray[i].pTransferApi;
		delete m_TransferApiArray[i].pLastActiveTime;
	}
	m_TransferApiArray.clear();

	KillTimer(m_nTimerID);

	CMutex mutex(FALSE, _T("FileZilla Queue Mutex"), 0);
	CSingleLock mLock(&mutex, TRUE);
	
	CMarkupSTL *pMarkup;

	if (COptions::LockXML(&pMarkup))
	{
		pMarkup->ResetPos();
		if (!pMarkup->FindChildElem( _T("TransferQueue") ))
			pMarkup->AddChildElem( _T("TransferQueue") );

		pMarkup->IntoElem();
		
		for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
		{
			const CQueueData &queueData = *iter;
			if (!queueData.priority && !queueData.bPaused)
				continue;

			pMarkup->AddChildElem( _T("QueueItem") );
			pMarkup->IntoElem();
			pMarkup->AddChildElem( _T("File") );
			pMarkup->AddChildAttrib( _T("Localfile"), queueData.transferFile.localfile);
			pMarkup->AddChildAttrib( _T("Size"), queueData.transferFile.size);
			pMarkup->AddChildAttrib( _T("Remotefile"), queueData.transferFile.remotefile);
			pMarkup->AddChildAttrib( _T("Remotepath"), queueData.transferFile.remotepath.GetSafePath());
			
			pMarkup->AddChildElem( _T("Data") );
			pMarkup->AddChildAttrib( _T("Transfermode"), queueData.transferFile.nType);
			pMarkup->AddChildAttrib( _T("Get"), queueData.transferFile.get);
			pMarkup->AddChildAttrib( _T("RetryCount"), queueData.retrycount);
			pMarkup->AddChildAttrib( _T("Open"), queueData.nOpen);

			COptions::SaveServer(pMarkup, queueData.transferFile.server);

			pMarkup->OutOfElem();
		}
		COptions::UnlockXML();
	}
	else
	{
		
		int j=0;
		TCHAR buffer[1000];
		HKEY key;
		if (RegOpenKey(HKEY_CURRENT_USER, _T("Software\\FileZilla\\Queue\\") ,&key)==ERROR_SUCCESS)
		{
			while (RegEnumKey(key,j,buffer,1000)==ERROR_SUCCESS)
			{
				j++;
			}
			RegCloseKey(key);
		}
		
		for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
		{
			const CQueueData &queueData = *iter;
			if (!queueData.priority && !queueData.bPaused)
				continue;
			
			CString str;
			str.Format(_T("%d"), j);
			HKEY key2;
			if (RegCreateKeyEx(HKEY_CURRENT_USER,"Software\\FileZilla\\Queue\\"+str, 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &key2, NULL)==ERROR_SUCCESS)
			{
				COptions::SetKey(key2, "Localfile", queueData.transferFile.localfile);
				COptions::SetKey(key2, "Size", queueData.transferFile.size);
				COptions::SetKey(key2, "Remotefile", queueData.transferFile.remotefile);
				COptions::SetKey(key2, "Remotepath", queueData.transferFile.remotepath.GetSafePath());
				COptions::SetKey(key2, "Transfer mode", queueData.transferFile.nType);
				COptions::SetKey(key2, "Get", queueData.transferFile.get);
				COptions::SetKey(key2, "Retry Count", queueData.retrycount);
				COptions::SetKey(key2, "Open", queueData.nOpen);
				
				COptions::SaveServer(key2, queueData.transferFile.server);

				RegCloseKey(key2);
			}
			j++;
		}
	}

	m_nActiveCount = 0;
	m_QueueItems.clear();
	DeleteAllItems();
	
	CListCtrl::OnDestroy();	
}

void CQueueCtrl::ProcessReply(int nReplyCode, int nApiIndex /*=-1*/)
{
	ASSERT(nApiIndex >= -1 && nApiIndex < static_cast<int>(m_TransferApiArray.size()));
	if (nApiIndex < -1 || nApiIndex >= static_cast<int>(m_TransferApiArray.size()))
		return;

	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
	m_bMayUsePrimaryConnection = FALSE;

	while (TRUE)
	{
		ASSERT(Validate());
		
		//Make sure pFileZillaApi is either NULL or in the active list. If not, stop processing
		t_TransferApi api={0};
		if (nApiIndex > -1)
			api = m_TransferApiArray[nApiIndex];
		
		//Find the item
		unsigned int nIndex;
		CQueueData queueData;
		for (nIndex = 0; nIndex < m_QueueItems.size(); nIndex++)
		{
			queueData = m_QueueItems[nIndex];
			if (!queueData.bActive)
				return;
			if (queueData.pTransferApi == api.pTransferApi)
				break;
		}
		if (nIndex == m_QueueItems.size())
			return;

		//Initialize all required data variables
		t_transferfile &transferFile = queueData.transferFile;
		
		t_ServerDataMapIter ServerDataMapIter = m_ServerDataMap.find(transferFile.server);
		ASSERT(ServerDataMapIter != m_ServerDataMap.end());
		if (ServerDataMapIter == m_ServerDataMap.end())
			return;
			
		BOOL bConnectError = FALSE;
		
		int nState = queueData.nState;
		
		if (nReplyCode&FZ_REPLY_DISCONNECTED && nState==QUEUESTATE_DISCONNECT)
		{
			ASSERT(api.pTransferApi);
			if (!api.pTransferApi)
				return;
			queueData.nState = QUEUESTATE_CONNECT;
			if (api.pTransferApi->Connect(transferFile.server)==FZ_REPLY_WOULDBLOCK)
			{
				queueData.status.Format(IDS_STATUSMSG_CONNECTING, (transferFile.server.name!="")?transferFile.server.name:transferFile.server.host);
				m_QueueItems[nIndex] = queueData;
				return;
			}
			else
				nReplyCode |= FZ_REPLY_ERROR | FZ_REPLY_CRITICALERROR;
		}
		else if (nState==QUEUESTATE_CONNECT)
		{
			ASSERT(api.pTransferApi);
			if (!api.pTransferApi)
				return;
			if (nReplyCode == FZ_REPLY_OK)
			{
				ServerDataMapIter->second.nCurrentActive++;
				if (ServerDataMapIter->second.nCurrentActive > ServerDataMapIter->second.nActiveMax)
					ServerDataMapIter->second.nActiveMax++;
				queueData.nState = QUEUESTATE_TRANSFER;
				int res = api.pTransferApi->FileTransfer(transferFile);
				if (res==FZ_REPLY_WOULDBLOCK)
				{
					queueData.status.Format(IDS_QUEUESTATUS_TRANSFERRING);
					m_QueueItems[nIndex] = queueData;
					return;
				}
				else
					nReplyCode|=FZ_REPLY_ERROR;
			}
			else
			{
				if (ServerDataMapIter->second.nFailedConnections < 5)
					ServerDataMapIter->second.nFailedConnections++;
				if (ServerDataMapIter->second.nFailedConnections==5 && ServerDataMapIter->second.nActiveMax)
				{
					if (ServerDataMapIter->second.nCurrentActive<ServerDataMapIter->second.nActiveMax)
						ServerDataMapIter->second.nActiveMax--;
					ServerDataMapIter->second.nFailedConnections = 3;
				}
				if (ServerDataMapIter->second.nCurrentActive)
					bConnectError = TRUE;
				
				nReplyCode |= FZ_REPLY_ERROR;
				
				//Check if primary connection is connected to the same server
				//and increase nAutoUsePrimary if necessary

				CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
				if (pMainFrame->IsConnected())
				{
					t_server server;
					CServerPath path;
					if (pMainFrame->m_pCommandQueue->GetLastServer(server, path) && 
						server==transferFile.server)
					{
						if (!ServerDataMapIter->second.nActiveMax)
						{
							queueData.nAutoUsePrimary = 3;
							//Increase nAutoUsePrimary on all other items which use the same server
							for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
							{
								if (!iter->bActive &&
									iter->transferFile.server == transferFile.server && iter->nAutoUsePrimary<3)
									iter->nAutoUsePrimary ++;
							}
						}
					}
				}

				m_RetryServerList.AddServer(transferFile.server);
			}
		}

		if (nApiIndex == -1)
			m_bPrimaryConnectionTransferInProgress = FALSE;

		ASSERT(nIndex < static_cast<unsigned int>(m_nActiveCount));
		if (nIndex < static_cast<unsigned int>(m_nActiveCount))
			m_nActiveCount--;

		if (nApiIndex != -1 && nState == QUEUESTATE_TRANSFER)
			ServerDataMapIter->second.nCurrentActive--;

		queueData.pTransferApi = 0;
		delete queueData.pTransferStatus;
		queueData.pTransferStatus = 0;
		delete queueData.pProgressControl;
		queueData.pProgressControl = 0;
		
		if (api.pTransferApi)
		{
			api.bActive = FALSE;
			api.pLastActiveTime = new CTime;
			*api.pLastActiveTime = CTime::GetCurrentTime();
			m_TransferApiArray[nApiIndex] = api;
		}

		BOOL bStop = queueData.bStop;
		if (queueData.bStop)
		{
			queueData.bTransferStarted = FALSE;
			queueData.bTriedAutoResume = false;
		}
		queueData.bStop = FALSE;
		queueData.nState = 0;
		
		if (nReplyCode==FZ_REPLY_OK)
		{
			m_QueueItems.erase(m_QueueItems.begin() + nIndex);
			SetItemCount(m_QueueItems.size() + m_nActiveCount);

			if (queueData.nOpen)
				EditFile(queueData);
					
			if (FindValidIndex(1) == m_QueueItems.begin())
				StopProcessing();
			else if (m_nProcessQueue==2 && FindValidIndex(2) == m_QueueItems.begin())
				StopProcessing();
		}
		else if ( ((nReplyCode&FZ_REPLY_ABORTED || nReplyCode&FZ_REPLY_CANCEL) && bStop) || queueData.nAutoUsePrimary==3 || bConnectError)
		{
			queueData.bActive = FALSE;
			
			m_QueueItems.erase(m_QueueItems.begin() + nIndex);
			
			if (!queueData.nAutoUsePrimary && !bConnectError)
				queueData.priority=1;
			
			if (queueData.bPaused)
			{
				queueData.priority = 0;
				queueData.action.LoadString(IDS_QUEUESTATUS_PAUSED);
			}
			else
				queueData.action = _T("");
			
			if (queueData.priority == 2)
				m_QueueItems.insert(m_QueueItems.begin() + m_nActiveCount, queueData);
			else if (queueData.bPaused)
				m_QueueItems.insert(FindValidIndex(1), queueData);
			else
				m_QueueItems.insert(FindValidIndex(2), queueData);
		}
		else if (nReplyCode&FZ_REPLY_CRITICALERROR || nReplyCode&FZ_REPLY_ABORTED || nReplyCode&FZ_REPLY_CANCEL)
		{
			queueData.bActive = FALSE;

			m_QueueItems.erase(m_QueueItems.begin() + nIndex);
			SetItemCount(m_QueueItems.size() + m_nActiveCount);
			
			queueData.priority = 0;
			if (!queueData.bPaused)
				queueData.retrycount = -1;
			
			if (FindValidIndex(1) == m_QueueItems.begin())
				StopProcessing();
			else if (m_nProcessQueue==2 && FindValidIndex(2) == m_QueueItems.begin())
				StopProcessing();
			
			if (!queueData.bAbort)
			{
				if (queueData.bPaused)
					queueData.action.LoadString(IDS_QUEUESTATUS_PAUSED);
				else
					queueData.action.LoadString((nReplyCode&FZ_REPLY_CRITICALERROR)?IDS_QUEUESTATUS_CRITICALERROR:IDS_QUEUESTATUS_ABORTED);

				m_QueueItems.insert(FindValidIndex(queueData.bPaused?1:0), queueData);
			}
		}
		else if (nReplyCode&FZ_REPLY_ERROR)
		{
			queueData.bActive = FALSE;

			queueData.retrycount++;
			m_QueueItems.erase(m_QueueItems.begin() + nIndex);
			if (queueData.retrycount>=COptions::GetOptionVal(OPTION_NUMRETRIES))
			{
				SetItemCount(m_QueueItems.size() + m_nActiveCount);
				
				queueData.retrycount = -1;
				queueData.priority = 0;
				
				if (FindValidIndex(1) == m_QueueItems.begin())
					StopProcessing();
				else if (m_nProcessQueue==2 && FindValidIndex(2) == m_QueueItems.begin())
					StopProcessing();
				
				queueData.action.LoadString(IDS_QUEUESTATUS_TOOMANYERTREIS);

				m_QueueItems.insert(FindValidIndex(0), queueData);
			}
			else
			{
				queueData.action.Format(IDS_QUEUESTATUS_ERROR, queueData.retrycount);

				if (queueData.priority==2)
					m_QueueItems.insert(m_QueueItems.begin() + m_nActiveCount, queueData);
				else
					m_QueueItems.insert(FindValidIndex(2), queueData);
			}
		}
		else
		{
			m_QueueItems.erase(m_QueueItems.begin() + nIndex);

			AfxMessageBox(_T("Internal Error in queue. Please restart FileZilla to avoid potential data loss."));
			ASSERT(FALSE);
		}
	
		SetItemCount(m_QueueItems.size() + m_nActiveCount);
		
		UpdateStatusbar();
			
		if (transferFile.get)
			pMainFrame->GetLocalPane2()->RefreshFile(transferFile.localfile);

		int res = 0;
		if (DoProcessQueue())
		{
			do
			{
				res = TransferNextFile(nApiIndex);
				if (res==-1)
				{
					nReplyCode = FZ_REPLY_ERROR;
					continue;
				}
				else if (res==-2)
				{
					nReplyCode = FZ_REPLY_CRITICALERROR;
					continue;
				}
			}
			while (res==1);
		}
		else if (m_QueueItems.empty())
			m_nActiveCount = 0;
		if (res >= 0)
			break;
	}
	SetItemCount(m_QueueItems.size() + m_nActiveCount);
}

int CQueueCtrl::TransferNextFile(int &nApiIndex)
{
	ASSERT(Validate());
	
	if (!m_nProcessQueue)
		return 0;
	if (FindValidIndex(1) == m_QueueItems.begin())
	{
		StopProcessing();
		return 0;
	}

	if (!m_bUseMultiple && m_nActiveCount)
		return 0;

	if (static_cast<UINT>(m_nActiveCount) >= m_nMaxApiCount)
		return 0;

	//Search for the first idle item
	t_QueueVector::iterator iter;	
	for (iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (iter->bActive)
			continue;
		if (iter->nAutoUsePrimary>=3 && m_bPrimaryConnectionTransferInProgress)
			continue;
		if (iter->bPaused)
			continue;
		if (!iter->priority)
			return 0;
		if (iter->priority < m_nProcessQueue)
			return 0;

		t_ServerDataMapIter ServerDataMapIter = m_ServerDataMap.find(iter->transferFile.server);
		if (ServerDataMapIter == m_ServerDataMap.end())
		{
			t_ServerData ServerData;
			ServerData.nActiveMax = ServerData.nCurrentActive = ServerData.nFailedConnections = 0;
			m_ServerDataMap[iter->transferFile.server] = ServerData;
			ServerDataMapIter = m_ServerDataMap.find(iter->transferFile.server);
			ASSERT(ServerDataMapIter != m_ServerDataMap.end());
		}
			
		if (ServerDataMapIter->second.nFailedConnections>=3 && ServerDataMapIter->second.nActiveMax && ServerDataMapIter->second.nCurrentActive>=ServerDataMapIter->second.nActiveMax)
			continue;

		if (m_nProcessQueue!=1)
			m_nProcessQueue = iter->priority;
		
		int maxsize = COptions::GetOptionVal(OPTION_TRANSFERPRIMARYMAXSIZE);
		if (iter->nAutoUsePrimary==3 || 
			(m_bMayUsePrimaryConnection && iter->transferFile.size>=0 && iter->transferFile.size < maxsize) ||
			!m_bUseMultiple )
		{
			m_bMayUsePrimaryConnection = FALSE;
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
			if (pMainFrame->IsConnected())
			{
				t_server server;
				CServerPath path;
				if (pMainFrame->m_pCommandQueue->GetLastServer(server, path) && 
					server==iter->transferFile.server)
				{
					if (!pMainFrame->m_pCommandQueue->IsLocked() && !pMainFrame->m_pCommandQueue->IsBusy())
					{
						iter->nAutoUsePrimary=4;
						m_bPrimaryConnectionTransferInProgress=TRUE;
						iter->action.Format(IDS_QUEUESTATUS_TRANSFERRING);
						
						iter->bActive = TRUE;
						iter->nState = QUEUESTATE_TRANSFER;
						
						//Create the progress control
						iter->pProgressControl = new CTextProgressCtrl;
						if (iter->pProgressControl->Create(WS_VISIBLE|WS_CHILD|WS_BORDER, CRect(1, 1, 1, 1), this, 0))
						{
							iter->pProgressControl->SetRange(0,100); //Set the range to between 0 and 100
							iter->pProgressControl->SetStep(1); // Set the step amount
							iter->pProgressControl->ShowWindow(SW_HIDE);
						}
						else
						{
							delete iter->pProgressControl;
							iter->pProgressControl = NULL;
						}
						
						CQueueData queueData = *iter;
						m_QueueItems.erase(iter);
						iter = m_QueueItems.insert(m_QueueItems.begin() + m_nActiveCount, queueData);
						
						m_nActiveCount++;
						SetItemCount(m_QueueItems.size() + m_nActiveCount);
						
						nApiIndex=-1;
						iter->pTransferApi=0;
						if (!pMainFrame->m_pCommandQueue->FileTransfer(iter->transferFile))
							return -1;
						else
							iter->status.Format(IDS_QUEUESTATUS_TRANSFERRING);

						return 1;
					}
					else if (iter->nAutoUsePrimary)
					{
						continue;
					}
				}
				else
					iter->nAutoUsePrimary=0;
			}
			else
				iter->nAutoUsePrimary=0;
	
		}
		
		iter->nAutoUsePrimary=0;
		
		//Chose one api instance
		t_TransferApi api;
		
		int nMatchingApiIndex=-1, nUnconnectedApiIndex=-1, nConnectedApiIndex=-1;
		
		for (std::vector<t_TransferApi>::const_iterator apiiter = m_TransferApiArray.begin(); apiiter != m_TransferApiArray.end(); apiiter++)
			//Find a matching api instance. The best would be an instance already
			//connected to the right server. If no one can be found, use an unconnected
			//instance and if this fails, too, use an instance connected to another server
		{
			if (apiiter->bActive)
				continue; //Skip active instances
			if (apiiter->pTransferApi->IsConnected() == FZ_REPLY_OK) 
			{
				t_server server;
				if (apiiter->pTransferApi->GetCurrentServer(server)==FZ_REPLY_OK)
					if (server == iter->transferFile.server)
					{
						nMatchingApiIndex = apiiter - m_TransferApiArray.begin();
						break;
					}
					else
						nConnectedApiIndex = apiiter - m_TransferApiArray.begin();
				else
					nUnconnectedApiIndex = apiiter - m_TransferApiArray.begin();
			}
			else
				nUnconnectedApiIndex = apiiter - m_TransferApiArray.begin();
		}

		nApiIndex = -1;
		if (nMatchingApiIndex != -1)
			nApiIndex = nMatchingApiIndex;
		else if (!m_RetryServerList.StillWait(iter->transferFile.server))
			if (nUnconnectedApiIndex != -1)
				nApiIndex = nUnconnectedApiIndex;
			else
				nApiIndex = nConnectedApiIndex;
		
		if (nApiIndex == -1)
			continue;

		api = m_TransferApiArray[nApiIndex];
		
		if (api.pLastActiveTime)
		{
			delete api.pLastActiveTime;
			api.pLastActiveTime = 0;
		}
		api.bActive = TRUE;
		m_TransferApiArray[nApiIndex] = api;


		iter->status.Format(IDS_QUEUESTATUS_TRANSFERRING);
		
		iter->bActive = TRUE;
		
		//Create the progress control
		iter->pProgressControl = new CTextProgressCtrl;
		if (iter->pProgressControl->Create(WS_VISIBLE|WS_CHILD|WS_BORDER, CRect(1, 1, 1, 1), this, 0))
		{
			iter->pProgressControl->SetRange(0,100); //Set the range to between 0 and 100
			iter->pProgressControl->SetStep(1); // Set the step amount
			iter->pProgressControl->ShowWindow(SW_HIDE);
		}
		else
		{
			delete iter->pProgressControl;
			iter->pProgressControl = NULL;
		}
		
		const CQueueData queueData = *iter;
		m_QueueItems.erase(iter);
		iter = m_QueueItems.insert(m_QueueItems.begin() + m_nActiveCount, queueData);
		
		m_nActiveCount++;
		SetItemCount(m_QueueItems.size() + m_nActiveCount);
		
		iter->pTransferApi = api.pTransferApi;
		if (api.pTransferApi->IsConnected() == FZ_REPLY_OK)
		{
			t_server server;
			if (api.pTransferApi->GetCurrentServer(server) == FZ_REPLY_OK)
			{
				if (server== iter->transferFile.server)
				{
					iter->nState = QUEUESTATE_TRANSFER;
					int res = api.pTransferApi->FileTransfer(iter->transferFile);
					if (res == FZ_REPLY_WOULDBLOCK)
					{
						iter->status.Format(IDS_QUEUESTATUS_TRANSFERRING);
						ServerDataMapIter->second.nCurrentActive++;
						if (ServerDataMapIter->second.nCurrentActive>ServerDataMapIter->second.nActiveMax)
							ServerDataMapIter->second.nActiveMax++;
						return 1;
					}
					else if (res!=FZ_REPLY_NOTCONNECTED)
						return -2;
				}
				else
				{
					iter->nState = QUEUESTATE_DISCONNECT;
					int res = api.pTransferApi->Disconnect();
					if (res == FZ_REPLY_WOULDBLOCK)
					{
						iter->status.Format(IDS_STATUSMSG_CONNECTING, (iter->transferFile.server.name!="")?iter->transferFile.server.name:iter->transferFile.server.host);
						return 1;
					}
					else if (res!=FZ_REPLY_NOTCONNECTED)
						return -2;
				}
			}
		}
		
		iter->nState = QUEUESTATE_CONNECT;
		if (api.pTransferApi->Connect(iter->transferFile.server) != FZ_REPLY_WOULDBLOCK)
			return -2;
		iter->status.Format(IDS_STATUSMSG_CONNECTING, (iter->transferFile.server.name!="")?iter->transferFile.server.name:iter->transferFile.server.host);
		return 1;
	}
	return 0;
}

void CQueueCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;
	menu.LoadMenu(IDR_QUEUECONTEXTMENU);

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

	BOOL selected = FALSE;
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!(iter->nListItemState & LVIS_SELECTED))
			continue;
		if (!selected)
			selected = TRUE;
		
		int nIndex = iter - m_QueueItems.begin();
		
		if (nIndex < m_nActiveCount)
		{
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVETOTOP, MF_GRAYED);
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVETOBOTTOM, MF_GRAYED);
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVEUP, MF_GRAYED);
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVEDOWN, MF_GRAYED);
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_REMOVEFROMQUEUE, MF_GRAYED);
		}
		if (nIndex >= FindValidIndex(1)-m_QueueItems.begin())
		{
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVETOTOP, MF_GRAYED);
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVETOBOTTOM, MF_GRAYED);
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVEUP, MF_GRAYED);
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVEDOWN, MF_GRAYED);
		}
		if (nIndex==(FindValidIndex(1)-m_QueueItems.begin()-1))
		{
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVEDOWN, MF_GRAYED);
		}
		if (nIndex == m_nActiveCount)
		{
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVEUP, MF_GRAYED);
		}
			
		if (!iter->priority)
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_PAUSE, MF_GRAYED);
		
		if (!iter->bPaused)
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_RESUME, MF_GRAYED);
		
		if (iter->priority || iter->bPaused)
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_RESETSTATUS, MF_GRAYED);
		
		if (iter->priority==1 && nIndex == FindValidIndex(2)-m_QueueItems.begin())
		{
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVEUP, MF_GRAYED);
		}
		if (iter->priority==2 && nIndex == (FindValidIndex(2)-m_QueueItems.begin()-1))
		{
			pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVEDOWN, MF_GRAYED);
		}
	}

	if (!selected)
	{
		pPopup->EnableMenuItem(ID_QUEUECONTEXT_REMOVEFROMQUEUE, MF_GRAYED);
		pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVETOTOP, MF_GRAYED);
		pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVETOBOTTOM, MF_GRAYED);
		pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVEUP, MF_GRAYED);
		pPopup->EnableMenuItem(ID_QUEUECONTEXT_MOVEDOWN, MF_GRAYED);
		pPopup->EnableMenuItem(ID_QUEUECONTEXT_RESETSTATUS, MF_GRAYED);
		pPopup->EnableMenuItem(ID_QUEUECONTEXT_PAUSE, MF_GRAYED);
		pPopup->EnableMenuItem(ID_QUEUECONTEXT_RESUME, MF_GRAYED);
		pPopup->EnableMenuItem(ID_QUEUECONTEXT_ABORT, MF_GRAYED);
	}
	if (FindValidIndex(1) == m_QueueItems.begin())
		pPopup->EnableMenuItem(ID_QUEUECONTEXT_PROCESSQUEUE, MF_GRAYED);

	if (DoProcessQueue()==1 || m_QueueItems.empty())
		pPopup->EnableMenuItem(ID_QUEUECONTEXT_PROCESSQUEUE, MF_GRAYED);
	
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
}

void CQueueCtrl::OnQueuecontextMovedown() 
{
	std::list<int> list;
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!(iter->nListItemState & LVIS_SELECTED))
			continue;

		int nIndex = iter - m_QueueItems.begin();

		if (iter->bActive)
			continue;
		if (iter->priority==1 && nIndex>=(FindValidIndex(1) - m_QueueItems.begin() - 1))
			continue;
		if (iter->priority==2 && nIndex>=(FindValidIndex(2) - m_QueueItems.begin() - 1))
			continue;
		if (nIndex >= FindValidIndex(1) - m_QueueItems.begin())
			continue;
		list.push_front(nIndex);
	}
	for (std::list<int>::iterator listiter = list.begin(); listiter != list.end(); listiter++)
	{
		CQueueData data = m_QueueItems[*listiter];
		m_QueueItems[*listiter] = m_QueueItems[*listiter + 1];
		m_QueueItems[*listiter + 1] = data;
	}
	RedrawItems(0, GetItemCount() - 1);
}

void CQueueCtrl::OnQueuecontextMovetobottom() 
{
	std::list<int> list;
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!(iter->nListItemState & LVIS_SELECTED))
			continue;

		int nIndex = iter - m_QueueItems.begin();
	
		if (iter->bActive)
			continue;
		if (nIndex >= FindValidIndex(1) - m_QueueItems.begin())
			continue;
		list.push_front(nIndex);
	}
	int offset2=0;
	int offset=0;
	for (std::list<int>::iterator listiter=list.begin(); listiter!=list.end(); listiter++)
	{
		CQueueData data = m_QueueItems[*listiter];
		m_QueueItems.erase(m_QueueItems.begin() + *listiter);

		if (data.priority==1)
			m_QueueItems.insert(FindValidIndex(1) - offset++, data);
		else
			m_QueueItems.insert(FindValidIndex(2) - offset2++, data);
	}
	SetItemCount(m_QueueItems.size() + m_nActiveCount);
}

void CQueueCtrl::OnQueuecontextMovetotop() 
{
	std::list<int> list;
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!(iter->nListItemState & LVIS_SELECTED))
			continue;

		int nIndex = iter - m_QueueItems.begin();

		if (iter->bActive)
			continue;
		if (nIndex >= FindValidIndex(1) - m_QueueItems.begin())
			continue;
		list.push_back(nIndex);
	}

	int offset = FindValidIndex(2) - m_QueueItems.begin();
	int offset2 = m_nActiveCount;
	for (std::list<int>::iterator listiter=list.begin(); listiter!=list.end(); listiter++)
	{
		CQueueData data = m_QueueItems[*listiter];
		m_QueueItems.erase(m_QueueItems.begin() + *listiter);

		if (data.priority==1)
			m_QueueItems.insert(m_QueueItems.begin() + offset++, data);
		else
			m_QueueItems.insert(m_QueueItems.begin() + offset2++, data);
	}
	SetItemCount(m_QueueItems.size() + m_nActiveCount);
}

void CQueueCtrl::OnQueuecontextMoveup() 
{
	std::list<int> list;
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!(iter->nListItemState & LVIS_SELECTED))
			continue;

		int nIndex = iter - m_QueueItems.begin();
	
		if (iter->bActive)
			continue;
		if (nIndex <= m_nActiveCount)
			continue;
		if (iter->priority == 1 && nIndex == FindValidIndex(2) - m_QueueItems.begin())
			continue;
		if (nIndex >= FindValidIndex(1) - m_QueueItems.begin())
			continue;
		if (nIndex == 1)
			m_bMayUsePrimaryConnection = FALSE;
		list.push_back(nIndex);		
	}
	for (std::list<int>::iterator listiter = list.begin(); listiter != list.end(); listiter++)
	{
		CQueueData data = m_QueueItems[*listiter];
		m_QueueItems[*listiter] = m_QueueItems[*listiter - 1];
		m_QueueItems[*listiter - 1] = data;
	}
	SetItemCount(m_QueueItems.size() + m_nActiveCount);
}

void CQueueCtrl::OnQueuecontextProcessqueue() 
{
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
	pMainFrame->TransferQueue(1);
}

void CQueueCtrl::OnQueuecontextRemovefromqueue() 
{
	std::list<int> list;
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!(iter->nListItemState & LVIS_SELECTED))
			continue;
		
		if (iter == m_QueueItems.begin())
			m_bMayUsePrimaryConnection = FALSE;

		if (iter->bActive)
			continue;
		
		list.push_front(iter - m_QueueItems.begin());
	}
	for (std::list<int>::iterator listiter=list.begin(); listiter!=list.end(); listiter++)
		m_QueueItems.erase(m_QueueItems.begin() + *listiter);

	SetItemCount(m_QueueItems.size() + m_nActiveCount);

	UpdateStatusbar();
}

t_QueueVector::iterator CQueueCtrl::FindValidIndex(int type)
{
	t_QueueVector::iterator iter = m_QueueItems.end();
	
	if (type == 1)
	{
		for (iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
			if (iter->priority == 0)
				break;
	}
	else if (type == 2)
		for (iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
			if (!iter->bActive && iter->priority != 2)
				break;
	return iter;
}

t_QueueVector::const_iterator CQueueCtrl::FindValidIndex(int type) const
{
	t_QueueVector::const_iterator iter = m_QueueItems.end();
	
	if (type == 1)
		for (iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
			if (iter->priority == 0)
				break;
	else if (type == 2)
		for (iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
			if (!iter->bActive)
				break;
	return iter;
}

void CQueueCtrl::OnQueuecontextResetstatus() 
{
	std::list<int> list;
	std::list<CQueueData> list2;
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!(iter->nListItemState & LVIS_SELECTED))
			continue;
		
		int nIndex = iter - m_QueueItems.begin();
	
		if (iter->priority || iter->bPaused)
			continue;
		list.push_front(nIndex);
		list2.push_front(m_QueueItems[nIndex]);
	}
	for (std::list<int>::iterator listiter=list.begin(); listiter!=list.end(); listiter++)
		m_QueueItems.erase(m_QueueItems.begin() + *listiter);
	for (std::list<CQueueData>::iterator iter2=list2.begin(); iter2!=list2.end(); iter2++)
	{
		iter2->action = "";
		iter2->retrycount = 0;
		iter2->bTransferStarted = FALSE;
		iter2->bTriedAutoResume = false;
		AddItem(*iter2);
	}
	UpdateStatusbar();
}

#define VK_A 65
void CQueueCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar==46)
		OnQueuecontextRemovefromqueue();
	else if(GetKeyState( VK_CONTROL )&128 && nChar == VK_A )
	{
		for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
			iter->nListItemState = LVIS_SELECTED;
		RedrawItems(0, GetItemCount() - 1);
		return;
	}
			
	
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CQueueCtrl::ReloadHeader()
{
	ReloadHeaderItem(0, IDS_HEADER_LOCALNAME);
	ReloadHeaderItem(1, IDS_HEADER_SIZE);
	ReloadHeaderItem(2, IDS_HEADER_DIRECTION);
	ReloadHeaderItem(3, IDS_HEADER_REMOTENAME);
	ReloadHeaderItem(4, IDS_HEADER_HOST);
	ReloadHeaderItem(5, IDS_HEADER_STATUS);
}

void CQueueCtrl::ReloadHeaderItem(int nIndex, UINT nID)
{
	CHeaderCtrl *header = GetHeaderCtrl();
	TCHAR text[100];
	HDITEM item;
	memset(&item, 0, sizeof(HDITEM));
	item.cchTextMax = 100;
	item.mask = HDI_TEXT;
	item.pszText = text;
	header->GetItem(nIndex, &item);
	CString str;
	str.LoadString(nID);
	_tcscpy(text, str);
	header->SetItem(nIndex, &item);
}

void CQueueCtrl::OnDropFiles(HDROP hDropInfo) 
{
	//End of a file drag&drop operation
	m_bMayUsePrimaryConnection=FALSE;
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (!((CFtpListCtrl *)pMainFrame->GetFtpPane()->GetListCtrl())->GetItemCount())
	{
		DragFinish(hDropInfo);
		return;
	}
	int dropcount = DragQueryFile(hDropInfo, 0xFFFFFFFF, 0, 0);
	for (int i = 0; i < dropcount; i++)
	{
		int len = DragQueryFile(hDropInfo, i, 0, 0) + 1;
		LPTSTR name = new TCHAR[len + 1];
		DragQueryFile(hDropInfo, i, name, len);
		CFileStatus64 status;
		GetStatus64(name, status);
		CString name2 = name;
		if (status.m_attribute&0x10)
		{
			CString filename = name;
			if (filename.ReverseFind('\\')!=-1)
				filename=filename.Right(filename.GetLength()-filename.ReverseFind('\\')-1);
			pMainFrame->UploadDir(name2 + "\\*.*", filename + "\\", FALSE);
		}
		else
		{
			ASSERT(name2.ReverseFind('\\') != -1);
			CString filename=name2.Mid(name2.ReverseFind('\\')+1);
			CString path=name2.Left(name2.ReverseFind('\\'));
			pMainFrame->AddQueueItem(FALSE,filename,"",path,CServerPath(),FALSE);
		}
		delete [] name;
	}
	DragFinish(hDropInfo);
}

int CQueueCtrl::DoProcessQueue() const
{	
	return m_nProcessQueue;
}

void CQueueCtrl::UpdateStatusbar()
{
	BOOL bUnknown=FALSE;
	__int64 size=0;
	for (t_QueueVector::const_iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (iter->priority || iter->bPaused)
			if (iter->transferFile.size >= 0)
				size += iter->transferFile.size;
			else
				bUnknown = TRUE;
	}

	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	CString str;
	CString tmp;
	CString sizestr;
	if (size<(1024*10))
	{
		sizestr.LoadString(IDS_SIZE_BYTES);
		str.Format(ID_INDICATOR_QUEUESIZE,size,sizestr+(bUnknown?"+":""));
	}
	else if (size<(1024*1024*10))
	{
		sizestr.LoadString(IDS_SIZE_KBS);
		str.Format(ID_INDICATOR_QUEUESIZE,size/1024,sizestr+(bUnknown?"+":""));
	}
	else
	{
		sizestr.LoadString(IDS_SIZE_MBS);
		str.Format(ID_INDICATOR_QUEUESIZE,size/1024/1024,sizestr+(bUnknown?"+":""));
	}
	if (!pMainFrame || !pMainFrame->m_wndStatusBar.GetSafeHwnd())
		return;
	int res=pMainFrame->m_wndStatusBar.CommandToIndex(ID_INDICATOR_QUEUESIZE);
	pMainFrame->m_wndStatusBar.GetStatusBarCtrl().SetText(str,pMainFrame->m_wndStatusBar.CommandToIndex(ID_INDICATOR_QUEUESIZE),0);

	HFONT hFont = (HFONT)pMainFrame->m_wndStatusBar.SendMessage(WM_GETFONT);
	CClientDC dcScreen(NULL);
	HGDIOBJ hOldFont = NULL;
	if (hFont != NULL)
		hOldFont = dcScreen.SelectObject(hFont);
	int cx=dcScreen.GetTextExtent(str).cx;
	pMainFrame->m_wndStatusBar.SetPaneInfo(pMainFrame->m_wndStatusBar.CommandToIndex(ID_INDICATOR_QUEUESIZE),ID_INDICATOR_QUEUESIZE,SBPS_NORMAL,cx);
	if (hOldFont != NULL)
		dcScreen.SelectObject(hOldFont);
}

void CQueueCtrl::TransferQueue(int nPriority)
{
	if (nPriority<1 || nPriority>2)
		return;
	if (FindValidIndex(1) == m_QueueItems.begin())
		return;
	int nOldProcessQueue = m_nProcessQueue;
	if (nPriority<m_nProcessQueue || !m_nProcessQueue)
		m_nProcessQueue = nPriority;
	//if (!nOldProcessQueue)
	{
		int nApiIndex=-1;
		int res;
		do
		{
			res = TransferNextFile(nApiIndex);
			if (res==-1)
				ProcessReply(FZ_REPLY_ERROR, nApiIndex);
			else if (res==-2)
				ProcessReply(FZ_REPLY_ERROR|FZ_REPLY_CRITICALERROR, nApiIndex);
		} while (res==1);
	}
	ASSERT(Validate());
}

void CQueueCtrl::StopProcessing()
{
	m_bMayUsePrimaryConnection=FALSE;
	
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());

	BOOL bCancelMainTransfer = FALSE;

	t_QueueVector::iterator iter;
	for (iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!iter->bActive)
			break;
		
		if (!iter->pTransferApi)
			bCancelMainTransfer = TRUE;
		else
			iter->pTransferApi->Cancel();
		iter->bStop = TRUE;
	}
	for (iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (iter->priority==2 && !iter->bActive)
			iter->priority = 1;
	}
	if (m_nProcessQueue && m_QueueItems.size())
	{
		for (iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
		{
			if (iter->bActive)
				iter->priority = 2;
			else
				break;
		}
		if (!m_nActiveCount)
			if (((CFtpListCtrl *)pMainFrame->GetFtpPane()->GetListCtrl())->HasUnsureEntries())
				pMainFrame->m_pCommandQueue->List(FZ_LIST_USECACHE|FZ_LIST_EXACT);
	}
	else
	{
		if (((CFtpListCtrl *)pMainFrame->GetFtpPane()->GetListCtrl())->HasUnsureEntries())
			pMainFrame->m_pCommandQueue->List(FZ_LIST_USECACHE|FZ_LIST_EXACT);
	}
	RedrawItems(0, m_QueueItems.size() + m_nActiveCount);

	m_nProcessQueue = 0;

	if (bCancelMainTransfer)
		pMainFrame->m_pCommandQueue->Cancel();
}

void CQueueCtrl::Export()
{
	CFileDialog dlg(FALSE, _T("xml"), _T("Transfer Queue"), OFN_OVERWRITEPROMPT, _T("XML files (*.xml)|*.xml||"), this);
	if (dlg.DoModal()!=IDOK)
		return;
	
	CMarkupSTL markup;
	markup.AddElem( _T("FileZilla") );
	markup.AddChildElem( _T("TransferQueue") );
	markup.IntoElem();
	
	for (t_QueueVector::const_iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!iter->priority && !iter->bPaused)
			break;
		
		markup.AddChildElem( _T("QueueItem") );
		markup.IntoElem();

		markup.AddChildElem( _T("File") );
		markup.AddChildAttrib( _T("Localfile"), iter->transferFile.localfile);
		markup.AddChildAttrib( _T("Size"), iter->transferFile.size);
		markup.AddChildAttrib( _T("Remotefile"), iter->transferFile.remotefile);
		markup.AddChildAttrib( _T("Remotepath"), iter->transferFile.remotepath.GetSafePath());
		
		markup.AddChildElem( _T("Data") );
		markup.AddChildAttrib( _T("Transfermode"), iter->transferFile.nType);
		markup.AddChildAttrib( _T("Get"), iter->transferFile.get?1:0);
		markup.AddChildAttrib( _T("RetryCount"), iter->retrycount);
		markup.AddChildAttrib( _T("Open"), iter->nOpen);
		
		COptions::SaveServer(&markup, iter->transferFile.server);
		
		markup.OutOfElem();
	}

	if (!markup.Save(dlg.GetPathName()))
	{
		CString str;
		str.Format(IDS_ERRORMSG_FILEOPENFAILED,dlg.GetPathName());
		AfxMessageBox(str,MB_ICONEXCLAMATION);
		return;
	}
}

void CQueueCtrl::Import()
{
	CFileDialog dlg(TRUE, _T("xml"), _T("Transfer Queue"), OFN_FILEMUSTEXIST, _T("XML files (*.xml)|*.xml|Text files (*.txt)|*.txt||"), this);
	if (dlg.DoModal()!=IDOK)
		return;

	//First try to load the file as XML file
	CMarkupSTL markup;
	if (markup.Load(dlg.GetPathName()))
	{
		if (markup.FindChildElem( _T("TransferQueue") ))
		{
			markup.IntoElem();
			while (markup.FindChildElem( _T("QueueItem") ))
			{
				markup.IntoElem();
				t_transferfile transferfile;
				
				if (!markup.FindChildElem( _T("File") ))
				{
					markup.OutOfElem();
					continue;
				}
				
				transferfile.localfile=markup.GetChildAttrib( _T("Localfile") );
				transferfile.remotefile=markup.GetChildAttrib( _T("Remotefile") );
				transferfile.remotepath.SetSafePath(markup.GetChildAttrib( _T("Remotepath") ));
				transferfile.size=_ttoi64(markup.GetChildAttrib( _T("Size") ));
				
				markup.ResetChildPos();
				if (!markup.FindChildElem( _T("Data") ))
				{
					markup.OutOfElem();
					continue;
				}
				
				CQueueData queueData;
				
				queueData.retrycount=_ttoi(markup.GetChildAttrib( _T("RetryCount") ));
				transferfile.nType=_ttoi(markup.GetChildAttrib( _T("Transfermode") ));
				transferfile.get=_ttoi(markup.GetChildAttrib( _T("Get") ));
				queueData.nOpen=_ttoi(markup.GetChildAttrib( _T("Open") ));
				
				markup.ResetChildPos();
				if (!COptions::LoadServer(&markup, transferfile.server))
				{
					markup.OutOfElem();
					continue;
				}

				markup.OutOfElem();
				
				if (transferfile.localfile=="" || transferfile.remotefile=="" || transferfile.remotepath.IsEmpty())
				{
					AfxMessageBox( _T("Error, can't reload queue item!") );
				}
				else
				{
					queueData.transferFile = transferfile;
					int index2=AddItem(queueData);
				}
				
			}
			markup.OutOfElem();
			markup.RemoveChildElem();
		}
		else
		{
			CString str;
			str.Format(IDS_ERRORMSG_FILEOPENFAILED,dlg.GetPathName());
			AfxMessageBox(str,MB_ICONEXCLAMATION);
			return;
		}
		return;
	}

	//Loading file as XML file failed, now try to load it as text file
	CFile file;
	if (!file.Open(dlg.GetPathName(), CFile::modeRead|CFile::shareDenyWrite))
	{
		CString str;
		str.Format(IDS_ERRORMSG_FILEOPENFAILED,dlg.GetPathName());
		AfxMessageBox(str,MB_ICONEXCLAMATION);
		return;
	}
	CArchive ar(&file,CArchive::load);
	CString str;

	while (ar.ReadString(str))
	{
		if (str=="")
			continue;
		CString name, folder="\\";
		t_transferfile transferfile;
		CQueueData queueData;
		transferfile.get = TRUE;
		do
		{
			if (str=="")
				break;
			if (str.Left(15)=="Transfer type: ")
			{
				str=str.Mid(15);
				if (str=="Upload")
					transferfile.get=FALSE;
				else if (str=="Download")
					transferfile.get=TRUE;
				else
				{
					AfxMessageBox(IDS_ERRORMSG_CANTIMPORTFILE, MB_ICONSTOP);
					file.Close();
					return;
				}
			}
			else if (str.Left(12)=="Local file: ")
			{
				str=str.Mid(12);
				transferfile.localfile=str;
			}
			else if (str.Left(6)=="Size: ")
			{
				str=str.Mid(6);
				transferfile.size=_ttoi64(str);
			}
			else if (str.Left(13)=="Remote file: ")
			{
				str=str.Mid(13);
				transferfile.remotefile=str;
			}
			else if (str.Left(13)=="Remote path: ")
			{
				str=str.Mid(13);
				if (!transferfile.remotepath.SetSafePath(str))
				{
					AfxMessageBox(IDS_ERRORMSG_CANTIMPORTFILE,MB_ICONSTOP);
					file.Close();
					return;
				}
			}
			else if (str.Left(15)=="Transfer mode: ")
			{
				str=str.Mid(15);
				transferfile.nType=_ttoi(str);
			}
			else if (str.Left(13)=="Server Type: ")
			{
				str=str.Mid(13);
				transferfile.server.nServerType=_ttoi(str);
			}
			else if (str.Left(6)=="Host: ")
			{
				str=str.Mid(6);
				transferfile.server.host=str;
			}
			else if (str.Left(6)=="Port: ")
			{
				str=str.Mid(6);
				transferfile.server.port=_ttoi(str);
			}
			else if (str.Left(6)=="User: ")
			{
				str=str.Mid(6);
				transferfile.server.user=str;
			}
			else if (str.Left(6)=="Pass: ")
			{
				str=str.Mid(6);
				transferfile.server.pass=CCrypt::decrypt(str);
			}
			else if (str.Left(17)=="Firewall Bypass: ")
			{
				str=str.Mid(17);
				transferfile.server.fwbypass=_ttoi(str);
			}
			else if (str.Left(21)=="Open after transfer: ")
			{
				str=str.Mid(21);
				queueData.nOpen=_ttoi(str);
			}
		} while (ar.ReadString(str));
		if (transferfile.localfile=="" || transferfile.remotefile=="" || transferfile.remotepath.IsEmpty() ||
			transferfile.server.host=="" || transferfile.server.port<1 || transferfile.server.port>65535 || transferfile.server.user=="")
		{
			file.Close();
			AfxMessageBox(IDS_ERRORMSG_CANTIMPORTFILE,MB_ICONSTOP);
			return;
		}
		queueData.transferFile = transferfile;
		AddItem(queueData);
	}
	CString msg;
	msg.Format(IDS_STATUSMSG_FILEIMPORTED,dlg.GetPathName());
	AfxMessageBox(msg,MB_ICONINFORMATION);
}

void CQueueCtrl::EditFile(const CQueueData &queueData)
{
	if (queueData.nOpen==3)
	{
		//Delete file
		DeleteFile(queueData.transferFile.localfile);
	}
	else if (queueData.nOpen==1 || queueData.nOpen==2)
	{
		DWORD ThreadID;
		CQueueData *pQueueData = new CQueueData;
		*pQueueData = queueData;
		pQueueData->pTransferStatus = 0;
		pQueueData->pProgressControl = 0;
		HANDLE hThreadHandle;
		DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &hThreadHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
		pQueueData->retrycount = (int)hThreadHandle;
		CreateThread(0, 0, ViewEditThreadProc, pQueueData, 0, &ThreadID);
	}
}

DWORD CQueueCtrl::ViewEditThreadProc(LPVOID lpParameter)
{
	ASSERT(lpParameter);
	CQueueData *pQueueData = reinterpret_cast<CQueueData *>(lpParameter);
	CString file = pQueueData->transferFile.localfile;

	CFileStatus64 status;
	if (!GetStatus64(pQueueData->transferFile.localfile, status))
	{
		CloseHandle((HANDLE)pQueueData->retrycount);
		delete pQueueData;
		AfxMessageBox(_T("Internal error, unable to find downloaded file"), MB_ICONEXCLAMATION);
		return 0;
	}

	LPTSTR str;
	if (pQueueData->nOpen==1)
	{
	/*	CString cmdLine;
		if (file.Find( _T(" ") )!=-1)
			file=_T("\"") + file + _T("\"");
		PROCESS_INFORMATION ProcessInformation;  
		STARTUPINFO startupinfo={0};
		startupinfo.cb=sizeof(startupinfo);
		str=new TCHAR[file.GetLength()+1];
		_tcscpy(str, file);

		if (CreateProcess(0, str, 0, 0, 0, 0, 0, 0, &startupinfo, &ProcessInformation))
		{
			CloseHandle(ProcessInformation.hThread);
			HANDLE handles[2];
			handles[0]=(HANDLE)reinterpret_cast<CQueueData *>(pTransferFile->lpParam)->retrycount;
			handles[1]=ProcessInformation.hProcess;
			int res=WaitForMultipleObjects(2, handles, FALSE, INFINITE);
			if (res==(WAIT_OBJECT_0+1) || res==(WAIT_ABANDONED_0+1))
			{
				CFileStatus64 statusnew;
				if (GetStatus64(pTransferFile->localfile, statusnew))
				{
					if (statusnew.m_mtime!=status.m_mtime || statusnew.m_size!=status.m_size || statusnew.m_attribute!=status.m_attribute)
					{
						delete [] str;
						CloseHandle((HANDLE)reinterpret_cast<CQueueData *>(pTransferFile->lpParam)->retrycount);
						if (!AfxGetMainWnd()->PostMessage(WM_APP + 10, reinterpret_cast<WPARAM>(pTransferFile), 0))
						{
							delete reinterpret_cast<CQueueData*>(pTransferFile->lpParam);
							delete pTransferFile;
							DeleteFile(pTransferFile->localfile);
						}
						return 0;
					}
					else
						DeleteFile(pTransferFile->localfile);
				}
				else
					DeleteFile(pTransferFile->localfile);
			}
		}
		else
		{
			CString str;
			str.Format(IDS_ERRORMSG_VIEWEDIT_CANTOPENFILE, pTransferFile->localfile);
			AfxMessageBox(str, MB_ICONEXCLAMATION, 0);
			DeleteFile(pTransferFile->localfile);
		}*/
	}
	else if (pQueueData->nOpen == 2)
	{
		int pos=file.ReverseFind('.');
		if (pos!=-1)
		{
			CString fext=file.Mid(pos+1);
			fext.MakeLower();
			//Parse the file associations
			CString CustomAssociations = COptions::GetOption(OPTION_VIEWEDITCUSTOM);
			
			CString ext;
			CString prog;
			BOOL bDoExt=TRUE;
			while (CustomAssociations!="")
			{
				int pos=CustomAssociations.Find( _T(";") );
				if (bDoExt)
				{
					if (!pos || pos==-1 || pos==CustomAssociations.GetLength()-1)
						break;
					ext+=CustomAssociations.Left(pos);
					CustomAssociations=CustomAssociations.Mid(pos+1);
					if (CustomAssociations.Left(1)== _T(" "))
					{
						ext+=_T(";");
						CustomAssociations=CustomAssociations.Mid(1);
					}
					else
						bDoExt=FALSE;
				}
				else
				{
					if (!pos || pos==CustomAssociations.GetLength()-1)
						break;
					if (pos!=-1)
					{
						prog += CustomAssociations.Left(pos);
						CustomAssociations=CustomAssociations.Mid(pos+1);
					}
					else
					{
						prog = CustomAssociations;
						CustomAssociations="";
					}
					if (CustomAssociations.Left(1)== _T(" "))
					{
						prog+=_T(";");
						CustomAssociations=CustomAssociations.Mid(1);
						if (CustomAssociations!="")
							continue;
					}
					
					ext.MakeLower();
					if (fext==ext)
					{ //We've found a file aassociation for this extension
						CString cmdLine;
						if (file.Find( _T(" ") )!=-1)
							file=_T("\"") + file + _T("\"");
						cmdLine=prog + _T(" ") + file;
						PROCESS_INFORMATION ProcessInformation;  
						STARTUPINFO startupinfo={0};
						startupinfo.cb=sizeof(startupinfo);
						LPTSTR str=new TCHAR[cmdLine.GetLength()+1];
						_tcscpy(str, cmdLine);
						if (CreateProcess(0, str, 0, 0, 0, 0, 0, 0, &startupinfo, &ProcessInformation))
						{
							CloseHandle(ProcessInformation.hThread);
							HANDLE handles[2];
							handles[0] = (HANDLE)pQueueData->retrycount;
							handles[1] = ProcessInformation.hProcess;
							int res = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
							if (res==(WAIT_OBJECT_0+1) || res==(WAIT_ABANDONED_0+1))
							{
								CFileStatus64 statusnew;
								if (GetStatus64(pQueueData->transferFile.localfile, statusnew))
								{
									if ((statusnew.m_has_mtime && status.m_has_mtime && statusnew.m_mtime != status.m_mtime) || statusnew.m_size!=status.m_size || statusnew.m_attribute!=status.m_attribute)
									{
										delete [] str;
										CloseHandle((HANDLE)pQueueData->retrycount);
										CWnd *pWnd = AfxGetMainWnd();
										if (!pWnd || !pWnd->PostMessage(WM_APP + 10, (WPARAM)pQueueData, 0))
										{
											DeleteFile(pQueueData->transferFile.localfile);
											delete pQueueData;
										}
										CloseHandle(ProcessInformation.hProcess);
										return 0;
									}
									else
										DeleteFile(pQueueData->transferFile.localfile);
								}
								else
									DeleteFile(pQueueData->transferFile.localfile);
							}
							CloseHandle(ProcessInformation.hProcess);
						}
						else
						{
							CString str;
							str.Format(IDS_ERRORMSG_VIEWEDIT_CANTSTARTPROGRAM, prog, pQueueData->transferFile.localfile);
							AfxMessageBox(str, MB_ICONEXCLAMATION, 0);
							DeleteFile(pQueueData->transferFile.localfile);
						}
						delete [] str;
						CloseHandle((HANDLE)pQueueData->retrycount);
						delete pQueueData;
						return 0;
					}
					ext = "";
					prog = "";
					bDoExt = TRUE;
				}
			}
		}
		//File has no extension or custom file association could not be found
		CString defprog=COptions::GetOption(OPTION_VIEWEDITDEFAULT);
		if (defprog=="")
		{
			AfxMessageBox(IDS_ERRORMSG_VIEWEDIT_NODEFPROG, MB_ICONEXCLAMATION);
			CloseHandle((HANDLE)pQueueData->retrycount);
			delete pQueueData;
			return 0;
		}
		CString cmdLine;
		if (file.Find( _T(" ") )!=-1)
			file=_T("\"") + file + _T("\"");
		cmdLine=defprog + _T(" ") + file;
		PROCESS_INFORMATION ProcessInformation;  
		STARTUPINFO startupinfo={0};
		startupinfo.cb = sizeof(startupinfo);
		str = new TCHAR[cmdLine.GetLength()+1];
		_tcscpy(str, cmdLine);
		if (CreateProcess(0, str, 0, 0, 0, 0, 0, 0, &startupinfo, &ProcessInformation))
		{
			delete [] str;
			str = NULL;
			CloseHandle(ProcessInformation.hThread);
			HANDLE handles[2];
			handles[0] = (HANDLE)pQueueData->retrycount;
			handles[1] = ProcessInformation.hProcess;
			int res=WaitForMultipleObjects(2, handles, FALSE, INFINITE);
			if (res==(WAIT_OBJECT_0+1) || res==(WAIT_ABANDONED_0+1))
			{
				CloseHandle(ProcessInformation.hProcess);
				Sleep(500);

				// Check if the file is still open
				while (1)
				{
					HANDLE hFileHandle = CreateFile(pQueueData->transferFile.localfile, GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
					if (hFileHandle == INVALID_HANDLE_VALUE)
					{
						int error = GetLastError();
						if (error != ERROR_SHARING_VIOLATION)
							break;
					}
					else
					{
						CloseHandle(hFileHandle);
						break;
					}

					res = WaitForSingleObject((HANDLE)pQueueData->retrycount, 1000);
					if (res != WAIT_TIMEOUT)
					{
						CloseHandle((HANDLE)pQueueData->retrycount);
						DeleteFile(pQueueData->transferFile.localfile);
						delete pQueueData;
						return 0;
					}
				}
				CFileStatus64 statusnew;
				if (GetStatus64(pQueueData->transferFile.localfile, statusnew))
				{
					if ((statusnew.m_has_mtime && status.m_has_mtime && statusnew.m_mtime != status.m_mtime) || statusnew.m_attribute!=status.m_attribute)
					{
						CloseHandle((HANDLE)pQueueData->retrycount);
						
						if (!AfxGetApp()->m_pMainWnd->PostMessage(WM_APP, reinterpret_cast<WPARAM>(pQueueData), 0))
						{
							DeleteFile(pQueueData->transferFile.localfile);
							delete pQueueData;
						}
						
						return 0;
					}
					else
						DeleteFile(pQueueData->transferFile.localfile);
				}
				else
					DeleteFile(pQueueData->transferFile.localfile);
			}
			else
			{
				DeleteFile(pQueueData->transferFile.localfile);
				CloseHandle(ProcessInformation.hProcess);
			}
		}
		else
		{
			CString str;
			str.Format(IDS_ERRORMSG_VIEWEDIT_CANTSTARTPROGRAM, defprog, pQueueData->transferFile.localfile);
			AfxMessageBox(str, MB_ICONEXCLAMATION, 0);
			DeleteFile(pQueueData->transferFile.localfile);
		}
	}
	CloseHandle((HANDLE)pQueueData->retrycount);
	delete pQueueData;
	delete [] str;	
	return 0;
}

LRESULT CQueueCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (message>=(WM_APP + 10) && message<(WM_APP + 10 + m_TransferApiArray.size()))
	{
		ASSERT(Validate());
		int nApiIndex = message - WM_APP - 10;
		t_TransferApi api = m_TransferApiArray[nApiIndex];
		switch(FZ_MSG_ID(wParam))
		{
		case FZ_MSG_SOCKETSTATUS:
			switch(FZ_MSG_PARAM(wParam))
			{
			case FZ_SOCKETSTATUS_RECV:
				pMainFrame->m_RecvLed.Ping(100);
				break;
			case FZ_SOCKETSTATUS_SEND:
				pMainFrame->m_SendLed.Ping(100);
				break;
			}
			break;
		case FZ_MSG_LISTDATA:
			{
				t_directory *pDir=(t_directory *)lParam;
				if (pDir)
				{
					CServerPath path=((CFtpListCtrl *)pMainFrame->GetFtpPane()->GetListCtrl())->GetCurrentDirectory();
					if (path==pDir->path)
					{
						pMainFrame->GetFtpPane()->List(pDir);
						pMainFrame->GetFtpTreePane()->List(pDir);
					}
					else
						delete pDir;
				}
			}
			break;
		case FZ_MSG_REPLY:
			//Find api which has sent this message
			if (api.bActive && !m_bQuit)
				ProcessReply(lParam, nApiIndex);
			break;
		case FZ_MSG_SECURESERVER:
			//if (m_pSecureIconCtrl)
			//	m_pSecureIconCtrl->ShowWindow(FZ_MSG_PARAM(wParam)?SW_SHOW:SW_HIDE);
			break;
		case FZ_MSG_ASYNCREQUEST:
			if (m_bQuit)
				break;

			if (FZ_MSG_PARAM(wParam) == FZ_ASYNCREQUEST_OVERWRITE)
			{
				COverwriteRequestData *pData=(COverwriteRequestData *)lParam;
				for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
				{
					if (!iter->bActive)
						break;
					if (iter->pTransferApi != api.pTransferApi)
						continue;

					if (iter->bTransferStarted && !iter->bTriedAutoResume && iter->transferFile.nType != 1)
						if (api.pTransferApi->IsValid()) //Make sure data.pFileZillaApi still exists
						{
							iter->bTriedAutoResume = TRUE;
							api.pTransferApi->SetAsyncRequestResult(FILEEXISTS_RESUME_ASKONFAIL, pData);
							return TRUE;
						}
					break;
				}
			}
			CAsyncRequestQueue::ProcessRequest(reinterpret_cast<CAsyncRequestData *>(lParam), api.pTransferApi, TRUE);
			break;
		case FZ_MSG_STATUS:
			{
				t_ffam_statusmessage *pStatus=(t_ffam_statusmessage *)lParam;
				if (pStatus)
				{
					if (pMainFrame)
						pMainFrame->GetStatusPane()->ShowStatus(pStatus->status, pStatus->type);
					if (pStatus->post)
						delete pStatus;
					else
					{
						ASSERT(FALSE);
					}
				}
			}
			break;
		case FZ_MSG_TRANSFERSTATUS:
			{
				t_ffam_transferstatus *pStatus=(t_ffam_transferstatus *)lParam;
			
				if (api.bActive && !m_bQuit)
				{
					if (!SetProgress(api.pTransferApi, pStatus))
						delete pStatus;
				}
				else
					delete pStatus;
			}
			break;
		case FZ_MSG_QUITCOMPLETE:
			if (m_bQuit)
			{
				m_nQuitCount--;
				if (!m_nQuitCount)
					VERIFY(AfxGetMainWnd()->PostMessage(WM_CLOSE));
			}
			break;
		}
		return TRUE;
	}
	return CListCtrl::DefWindowProc(message, wParam, lParam);
}

void CQueueCtrl::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent==m_nTimerID)
	{
		for (UINT i=0; i<m_TransferApiArray.size(); i++)
		{
			t_TransferApi api=m_TransferApiArray[i];
			if (!api.bActive && api.pLastActiveTime)
			{
				CTimeSpan span = CTime::GetCurrentTime() - *api.pLastActiveTime;
				if (span.GetTotalSeconds()>30)
				{
					api.pTransferApi->Disconnect();
					delete api.pLastActiveTime;
					api.pLastActiveTime=0;
					m_TransferApiArray[i]=api;
				}
			}
		}

		int nApiIndex=-1;
		int res;
		do
		{
			res = TransferNextFile(nApiIndex);
			if (res==-1)
				ProcessReply(FZ_REPLY_ERROR, nApiIndex);
			else if (res==-2)
				ProcessReply(FZ_REPLY_ERROR|FZ_REPLY_CRITICALERROR, nApiIndex);
		} while (res==1);

		return;
	}
	
	CListCtrl::OnTimer(nIDEvent);
}

void WINAPI DrawItemColumn (HDC hdc, LPCTSTR szText, LPRECT prcClip, int nAlign = 0, BOOL bDontMeasure = FALSE);

/////////////////////////////////////////////////////////////////////////////// 
// DrawListViewItem() 
// 
// Parameters 
// 
// Purpose 
// 
// Return Value 
// 
void CQueueCtrl::DrawItem (LPDRAWITEMSTRUCT pDIS) 
{ 
	TCHAR szBuffer[512]; 
	LV_ITEM lvi; 
	int cxImage = 0, cyImage = 0; 
	UINT uFirstColWidth; 
	RECT rcClip; 
	int iColumn = 1; 
	UINT uiFlags = ILD_TRANSPARENT; 

	// Get the item image to be displayed 
	lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_TEXT; 
	lvi.iItem = pDIS->itemID; 
	lvi.iSubItem = 0; 
	lvi.pszText = szBuffer; 
	lvi.cchTextMax = 512; 
	ListView_GetItem (pDIS->hwndItem, &lvi);
	
	int oldTextColor = -1;
	int oldBkColor = -1;
	// Check to see if this item is selected 
	if (pDIS->itemState & ODS_SELECTED) 
	{ 
		// Set the text background and foreground colors 
		if (GetFocus() == this)
		{
			oldTextColor = ::SetTextColor(pDIS->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT)); 
			oldBkColor = ::SetBkColor(pDIS->hDC, GetSysColor(COLOR_HIGHLIGHT)); 
		}
		else
		{
			oldTextColor = ::SetTextColor(pDIS->hDC, GetSysColor(COLOR_WINDOWTEXT));
			oldBkColor = ::SetBkColor(pDIS->hDC, GetSysColor(COLOR_INACTIVEBORDER));
		}
		
		// Also add the ILD_BLEND50 so the images come out selected 
		uiFlags |= ILD_BLEND50; 
	} 
	else 
	{ 
		// Set the text background and foreground colors to the standard window colors 
		oldTextColor = ::SetTextColor (pDIS->hDC, GetSysColor(COLOR_WINDOWTEXT)); 
		oldBkColor = ::SetBkColor (pDIS->hDC, GetSysColor(COLOR_WINDOW)); 
	} 

	// Get the image list and draw the image 
/*	ImageList_Draw (g_hImages, 
		lvi.iImage, 
		pDIS->hDC, 
		pDIS->rcItem.left, 
		pDIS->rcItem.top, 
		uiFlags); 
	// Find out how big the image we just drew was 
	ImageList_GetIconSize (g_hImages, &cxImage, &cyImage); 
*/	
	// Calculate the width of the first column after the image width. If 
	// There was no image, then cxImage will be zero. 

	int nIndex;
	BOOL bStatusLine = FALSE;
	if (pDIS->itemID < static_cast<unsigned int>(m_nActiveCount * 2))
	{
		nIndex = pDIS->itemID / 2;
		bStatusLine = pDIS->itemID % 2;
	}
	else
		nIndex = pDIS->itemID - m_nActiveCount;
	
	if (bStatusLine)
	{
		CDC *pDC=GetDC();
		HDC hDC=pDC->GetSafeHdc();
		HGDIOBJ hOldBrush=SelectObject(hDC, GetCurrentObject(pDIS->hDC, OBJ_BRUSH));
		HGDIOBJ hOldFont=SelectObject(hDC, GetCurrentObject(pDIS->hDC, OBJ_FONT));
		DrawStatusLine(hDC, pDIS->rcItem, pDIS->itemID);
		SelectObject(hDC, hOldBrush);
		SelectObject(hDC, hOldFont);
		ReleaseDC(pDC);
	}
	else
	{
		uFirstColWidth = GetColumnWidth(0) - cxImage; 
		
		// Set up the new clipping rect for the first column text and draw it 
		rcClip.left = pDIS->rcItem.left + cxImage; 
		rcClip.right = pDIS->rcItem.left + GetColumnWidth(0); 
		rcClip.top = pDIS->rcItem.top; 
		rcClip.bottom = pDIS->rcItem.bottom; 
		
		DrawItemColumn (pDIS->hDC, szBuffer, &rcClip); 
		szBuffer[0] = 0; 
		
		SetTextAlign(pDIS->hDC, TA_RIGHT);
		lvi.mask = LVIF_TEXT; 
		lvi.iSubItem++; 
		ListView_GetItem (pDIS->hwndItem, &lvi); 
		rcClip.left = rcClip.right; 
		rcClip.right = rcClip.left + GetColumnWidth(1); 
		DrawItemColumn(pDIS->hDC, szBuffer, &rcClip, 2); 
		szBuffer[0] = 0; 
		
		SetTextAlign(pDIS->hDC, TA_LEFT);
		rcClip.left = rcClip.right; 
		rcClip.right = rcClip.left + GetColumnWidth(2); 
		lvi.iSubItem++; 
		ListView_GetItem (pDIS->hwndItem, &lvi); 
		DrawItemColumn(pDIS->hDC, szBuffer, &rcClip, 1); 
		szBuffer[0] = 0; 
		
		rcClip.left = rcClip.right; 
		rcClip.right = rcClip.left + GetColumnWidth(3); 
		lvi.iSubItem++; 
		ListView_GetItem (pDIS->hwndItem, &lvi); 
		DrawItemColumn(pDIS->hDC, szBuffer, &rcClip); 
		szBuffer[0] = 0; 
		
		rcClip.left = rcClip.right; 
		rcClip.right = rcClip.left + GetColumnWidth(4); 
		lvi.iSubItem++; 
		ListView_GetItem (pDIS->hwndItem, &lvi); 
		DrawItemColumn(pDIS->hDC, szBuffer, &rcClip); 
		szBuffer[0] = 0; 
		
		rcClip.left = rcClip.right; 
		rcClip.right = rcClip.left + GetColumnWidth(5); 
		lvi.iSubItem++; 
		ListView_GetItem (pDIS->hwndItem, &lvi); 
		DrawItemColumn(pDIS->hDC, szBuffer, &rcClip); 
		szBuffer[0] = 0; 
	}

	// If we changed the colors for the selected item, undo it 
	
	// Set the text background and foreground colors 
	if (oldTextColor != -1)
		::SetTextColor (pDIS->hDC, oldTextColor);
	if (oldBkColor != -1)
		::SetBkColor (pDIS->hDC, oldBkColor);
	
	// If the item is focused, now draw a focus rect around the entire row 
	if (pDIS->itemState & ODS_FOCUS && GetFocus() == this) 
	{ 
		// Adjust the left edge to exclude the image 
		rcClip = pDIS->rcItem; 
		rcClip.left += cxImage; 
		
		// Draw the focus rect 
		DrawFocusRect (pDIS->hDC, &rcClip); 
	} 
}

BOOL WINAPI CalcStringEllipsis (HDC hdc, 
								LPTSTR szString, 
								int cchMax, 
								UINT uColWidth);

/////////////////////////////////////////////////////////////////////////////// 
// DrawItemColumn() 
// 
// Parameters 
// 
// Purpose 
// 
// Return Value 
// 
void WINAPI DrawItemColumn (HDC hdc, LPCTSTR szText, LPRECT prcClip, int nAlign /*=0*/, BOOL bDontMeasure /*=FALSE*/)
{
	if (!prcClip)
		return;

	// Check to see if the string fits in the clip rect. If not, truncate
	// the string and add "...".
	LPTSTR szString = new TCHAR[_tcslen(szText)+10]; //Make it a little bit larger as required, CalcStringEllipsis max add a few chars to the string
	if (!szString)
		return;

	lstrcpy(szString, szText);
	if (!bDontMeasure)
		CalcStringEllipsis(hdc, szString, _tcslen(szText)+9, prcClip->right - prcClip->left);
	int nOldTextAlign=::GetTextAlign(hdc);

	int x;
	switch (nAlign)
	{
	case 1:
		::SetTextAlign(hdc, TA_CENTER);
		x=prcClip->left + (prcClip->right-prcClip->left)/2;
		break;
	case 2:
		SetTextAlign(hdc, TA_RIGHT);
		x=prcClip->right - 4;
		break;
	default:
		SetTextAlign(hdc, TA_LEFT);
		x = prcClip->left + 4;
		break;
	}
	if (nAlign==1)
	{
	}
	ExtTextOut(hdc,
		x,
		prcClip->top, 
		ETO_CLIPPED | ETO_OPAQUE, 
		prcClip, 
		szString, 
		lstrlen(szString), 
		NULL); 

	::SetTextAlign(hdc, nOldTextAlign);
	delete [] szString;
} 

/////////////////////////////////////////////////////////////////////////////// 
// CalcStringEllipsis() 
// 
// Parameters 
// 
// Purpose 
// 
// Return Value 
// 
BOOL WINAPI CalcStringEllipsis (HDC hdc, 
								LPTSTR szString, 
								int cchMax, 
								UINT uColWidth) 
{ 
	const TCHAR szEllipsis[] = TEXT("..."); 
	SIZE sizeString; 
	SIZE sizeEllipsis; 
	int cbString; 
	LPTSTR lpszTemp; 
	BOOL fSuccess = FALSE; 
	
	// Adjust the column width to take into account the edges 
	uColWidth -= 4; 
	__try 
	{ 
		// Allocate a string for us to work with. This way we can mangle the 
		// string and still preserve the return value 
		lpszTemp = (LPTSTR)HeapAlloc (GetProcessHeap(), HEAP_ZERO_MEMORY, cchMax*sizeof(TCHAR)); 
		if (!lpszTemp) 
		{ 
			__leave; 
		} 
		lstrcpy (lpszTemp, szString); 
		
		// Get the width of the string in pixels 
		cbString = lstrlen(lpszTemp); 
		if (!GetTextExtentPoint32 (hdc, lpszTemp, cbString, &sizeString)) 
		{ 
			__leave; 
		} 
		
		// If the width of the string is greater than the column width shave 
		// the string and add the ellipsis 
		if ((ULONG)sizeString.cx > uColWidth) 
		{ 
			if (!GetTextExtentPoint32 (hdc, 
				szEllipsis, 
				lstrlen(szEllipsis), 
				&sizeEllipsis)) 
			{ 
				__leave; 
			} 
			
			while (cbString > 0) 
			{ 
				lpszTemp[--cbString] = 0; 
				if (!GetTextExtentPoint32 (hdc, lpszTemp, cbString, &sizeString)) 
				{ 
					__leave; 
				} 
				
				if ((ULONG)(sizeString.cx + sizeEllipsis.cx) <= uColWidth) 
				{ 
					// The string with the ellipsis finally fits, now make sure 
					// there is enough room in the string for the ellipsis 
					if (cchMax >= (cbString + lstrlen(szEllipsis))) 
					{ 
						// Concatenate the two strings and break out of the loop 
						lstrcat (lpszTemp, szEllipsis); 
						lstrcpy (szString, lpszTemp); 
						fSuccess = TRUE; 
						__leave; 
					} 
				} 
			} 
		} 
		else 
		{ 
			// No need to do anything, everything fits great. 
			fSuccess = TRUE; 
		} 
	} 
	__finally 
	{ 
		// Free the memory 
		HeapFree (GetProcessHeap(), 0, (LPVOID)lpszTemp); 
	} 
	return fSuccess; 
}

void CQueueCtrl::DrawStatusLine(HDC hdc, RECT ItemRect, int nItem)
{
	ASSERT(hdc);
	ASSERT(nItem>0);
	if (!hdc || nItem < 1)
		return;
	
	int nIndex;
	BOOL bStatusLine = FALSE;
	ASSERT(nItem < (m_nActiveCount * 2));
	if (nItem >= (m_nActiveCount * 2))
		return;
	
	nIndex = nItem / 2;
	bStatusLine = nItem % 2;
	
	ASSERT(bStatusLine);
	if (!bStatusLine)
		return;

	int oldTextColor = ::SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT)); 
	int oldBkColor = ::SetBkColor(hdc, GetSysColor(COLOR_WINDOW)); 

	if (nIndex < 0 || nIndex >= (int)m_QueueItems.size())
		return;

	CQueueData &queueData = m_QueueItems[nIndex];
	if (queueData.pTransferStatus)
	{
		t_ffam_transferstatus *pTransferStatus=queueData.pTransferStatus;
		
		CString str,str2;
		CSize sizeString;
		
		CRect clipRect=ItemRect;

		str = GetItemText(nItem, 1);		
		int offset = 150;
		GetTextExtentPoint32(hdc, str, str.GetLength(), &sizeString);
		if (offset < sizeString.cx)
			offset = sizeString.cx + 20;
		clipRect.right = clipRect.left + offset;

		DrawItemColumn(hdc, str, &clipRect, 2, TRUE);

		clipRect.left=clipRect.right;
		str = GetItemText(nItem, 2);
		offset = 100;
		GetTextExtentPoint32(hdc, str, str.GetLength(), &sizeString);
		if (offset < sizeString.cx)
			offset = sizeString.cx + 20;
		clipRect.right = clipRect.left + offset;
		DrawItemColumn(hdc, str, &clipRect, 2, TRUE);

		clipRect.left=clipRect.right;
		clipRect.right = clipRect.left + 10;

		DrawItemColumn(hdc, _T(""), &clipRect, 2, TRUE);

		clipRect.left=clipRect.right;
		clipRect.right = clipRect.left + 100;

		clipRect.top--;
		clipRect.bottom++;

		if (queueData.pProgressControl)
		{
			CRect oldRect;
			queueData.pProgressControl->GetWindowRect(&oldRect);
			ScreenToClient(&oldRect);
			oldRect.top--;
			oldRect.bottom++;
			if (clipRect!=oldRect)
			{
				queueData.nProgressOffset = clipRect.left;
				queueData.pProgressControl->MoveWindow(clipRect, TRUE);
				queueData.pProgressControl->Invalidate();
			}
		}
		clipRect.top++;
		clipRect.bottom--;
  		
		clipRect.left=clipRect.right;
		str = GetItemText(nItem, 3);
		offset = 175;
		GetTextExtentPoint32(hdc, str, str.GetLength(), &sizeString);
		if (offset < sizeString.cx)
			offset = sizeString.cx + 20;
		clipRect.right = clipRect.left + offset;
		DrawItemColumn(hdc, str, &clipRect, 2, TRUE);

		clipRect.left=clipRect.right;
		clipRect.right=ItemRect.right;
		DrawItemColumn(hdc, _T(""), &clipRect, 0, TRUE);
	
		if (queueData.pProgressControl)
		{
			if (pTransferStatus->percent!=-1)
			{
				if (!queueData.pProgressControl->IsWindowVisible())
					queueData.pProgressControl->ShowWindow(SW_SHOW);
				queueData.pProgressControl->SetPos(pTransferStatus->percent);
			}
			else if (queueData.pProgressControl->IsWindowVisible())
				queueData.pProgressControl->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		if (queueData.pProgressControl)
			queueData.pProgressControl->ShowWindow(SW_HIDE);
		ItemRect.left+=50;

		RECT clipRect=ItemRect;
		clipRect.right=50;
		FillRect(hdc, &clipRect, (HBRUSH)GetCurrentObject(hdc, OBJ_BRUSH));
		clipRect.left=clipRect.right;
		clipRect.right=ItemRect.right;
		DrawItemColumn(hdc, GetItemText(nItem, 0), &clipRect);
	}

	// Set the text background and foreground colors 
	if (oldTextColor != -1)
		::SetTextColor (hdc, oldTextColor);
	if (oldBkColor != -1)
		::SetBkColor (hdc, oldBkColor);
}

BOOL CQueueCtrl::SetProgress(CFileZillaApi *pTransferApi, t_ffam_transferstatus *pTransferStatus)
{
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!iter->bActive)
			return FALSE;

		if (iter->pTransferApi == pTransferApi)
		{
			if (iter->pTransferStatus)
				delete iter->pTransferStatus;
			iter->pTransferStatus = pTransferStatus;

			if (pTransferStatus)
				if (pTransferStatus->bFileTransfer)
				{
					iter->bTransferStarted = TRUE;
					iter->bTriedAutoResume = false;
				}

			RedrawItems((iter - m_QueueItems.begin()) * 2 + 1, (iter - m_QueueItems.begin()) * 2 + 1);
			return TRUE;
		}
	}
	
	return FALSE;
}

void CQueueCtrl::OnPaint() 
{
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!iter->bActive)
			break;

		if (iter->pProgressControl && iter->pProgressControl->IsWindowVisible())
		{
			CRect itemRect;
	
			GetItemRect((iter - m_QueueItems.begin()) * 2 + 1, &itemRect, LVIR_BOUNDS);
		
			if (GetTopIndex() > ((iter - m_QueueItems.begin()) * 2 + 1))
			{
				iter->pProgressControl->ShowWindow(SW_HIDE);
				continue;
			}
		
			itemRect.left += iter->nProgressOffset;
			itemRect.right = itemRect.left + 100;
			CRect oldRect;
			iter->pProgressControl->GetWindowRect(&oldRect);
			ScreenToClient(&oldRect);
			oldRect.top++;
			oldRect.bottom--;
			if (oldRect!=itemRect)
			{
				iter->pProgressControl->MoveWindow(itemRect, TRUE);
		        iter->pProgressControl->Invalidate();
			}
		}
	}
	
	CRect clientRect;
	GetClientRect(clientRect);

	CHeaderCtrl *ctrl = GetHeaderCtrl();
	CRect headerRect;
	ctrl->GetWindowRect(headerRect);

	CRect itemRect;
	if (GetItemCount())
	{
		GetItemRect(GetItemCount()-1, &itemRect, LVIR_BOUNDS);

		if (itemRect.right < clientRect.right)
		{
			CRect rect = clientRect;
			rect.left = itemRect.right;
			rect.top = headerRect.bottom-headerRect.top;
			CDC *pDC=GetDC();
			pDC->FillSolidRect(rect, GetSysColor(COLOR_WINDOW));
			ReleaseDC(pDC);
		}
	}
	else
	{
		itemRect.bottom=headerRect.bottom-headerRect.top-1;
	}
	if (itemRect.bottom<clientRect.bottom)
	{
		CDC *pDC=GetDC();
		clientRect.top=itemRect.bottom;
		pDC->FillSolidRect(clientRect, GetSysColor(COLOR_WINDOW));
		ReleaseDC(pDC);
	}
		
			
	CListCtrl::OnPaint();
}

BOOL CQueueCtrl::OnEraseBkgnd(CDC* pDC) 
{
	static unsigned int nOldCount=0;
	if (m_QueueItems.empty() || (m_QueueItems.size()+m_nActiveCount) != nOldCount)
	{
		nOldCount = m_QueueItems.size()+m_nActiveCount;
		return CListCtrl::OnEraseBkgnd(pDC);
	}
	return TRUE;
}

void CQueueCtrl::SetApiCount(int nCount)
{
	if (static_cast<UINT>(nCount) > m_TransferApiArray.size())
	{
		int nOldCount = m_TransferApiArray.size();
		m_TransferApiArray.resize(nCount);
		t_TransferApi api={0};
		for (int nApiIndex = nOldCount; nApiIndex<nCount; nApiIndex++)
		{
			api.pTransferApi = new CFileZillaApi();
			VERIFY(api.pTransferApi->Init(GetSafeHwnd(), WM_APP + 10 + nApiIndex)==FZ_REPLY_OK);
			api.pTransferApi->SetDebugLevel(COptions::GetOptionVal(OPTION_DEBUGTRACE)?4:0);
			m_TransferApiArray[nApiIndex] = api;
		}
	}
	m_nMaxApiCount = nCount;
}

void CQueueCtrl::ToggleUseMultiple()
{
	m_bUseMultiple = !m_bUseMultiple;
}

BOOL CQueueCtrl::DoUseMultiple() const
{
	return m_bUseMultiple;
}


void CQueueCtrl::OnQueuecontextAbort() 
{
	std::list<int> list;
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!(iter->nListItemState & LVIS_SELECTED))
			continue;
		
		int nIndex = iter - m_QueueItems.begin();

		if (!iter->priority)
			continue;
		
		list.push_front(nIndex);
	}
	for (std::list<int>::iterator listiter = list.begin(); listiter != list.end(); listiter++)
	{
		m_QueueItems[*listiter].bAbort = TRUE;
		
		if (m_QueueItems[*listiter].bActive)
		{
			if (!m_QueueItems[*listiter].pTransferApi)
			{
				CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
				pMainFrame->m_pCommandQueue->Cancel();
			}
			else
				m_QueueItems[*listiter].pTransferApi->Cancel();
		}
		else
		{
			m_QueueItems.erase(m_QueueItems.begin() + *listiter);
			
			if (FindValidIndex(1) == m_QueueItems.begin())
				StopProcessing();
			else if (m_nProcessQueue==2 && FindValidIndex(2) == m_QueueItems.begin())
				StopProcessing();
		}
	}
	SetItemCount(m_QueueItems.size() + m_nActiveCount);
	UpdateStatusbar();
}

void CQueueCtrl::OnQueuecontextPause() 
{
	std::list<int> list;
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!(iter->nListItemState & LVIS_SELECTED))
			continue;
		
		int nIndex = iter - m_QueueItems.begin();

		if (!iter->priority || iter->bPaused)
			continue;
		list.push_front(nIndex);
	}
	for (std::list<int>::iterator listiter=list.begin(); listiter!=list.end(); listiter++)
	{
		m_QueueItems[*listiter].bPaused = TRUE;
		
		if (m_QueueItems[*listiter].bActive)
		{
			if (!m_QueueItems[*listiter].pTransferApi)
			{
				CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
				pMainFrame->m_pCommandQueue->Cancel();
			}
			else
				m_QueueItems[*listiter].pTransferApi->Cancel();
		}
		else
		{
			CQueueData data = m_QueueItems[*listiter];

			m_QueueItems.erase(m_QueueItems.begin() + *listiter);
			
			data.priority = 0;
			data.retrycount = -1;
			if (FindValidIndex(1) == m_QueueItems.begin())
				StopProcessing();
			else if (m_nProcessQueue==2 && FindValidIndex(2) == m_QueueItems.begin())
				StopProcessing();

			data.action.LoadString(IDS_QUEUESTATUS_PAUSED);
			m_QueueItems.insert(FindValidIndex(0), data);
		}
	}
	SetItemCount(m_QueueItems.size() + m_nActiveCount);
}

void CQueueCtrl::OnQueuecontextResume() 
{
	std::list<int> list;
	std::list<CQueueData> list2;
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!(iter->nListItemState & LVIS_SELECTED))
			continue;
		
		int nIndex = iter - m_QueueItems.begin();
		if (iter->priority || !iter->bPaused)
			continue;
		list.push_front(nIndex);
		list2.push_front(m_QueueItems[nIndex]);
	}
	for (std::list<int>::iterator listiter = list.begin(); listiter != list.end(); listiter++)
		m_QueueItems.erase(m_QueueItems.begin() + *listiter);
	for (std::list<CQueueData>::iterator iter2=list2.begin(); iter2!=list2.end(); iter2++)
	{
		iter2->action = "";
		iter2->priority = 2;
		iter2->bPaused = FALSE;
		AddItem(*iter2);
	}
	SetItemCount(m_QueueItems.size() + m_nActiveCount);
	UpdateStatusbar();
	TransferQueue(2);
}

BOOL CQueueCtrl::DoClose()
{
	if (m_nQuitCount)
		return FALSE;
	else if (m_bQuit)
		return TRUE;
	
	m_bQuit = TRUE;
	m_nQuitCount = m_TransferApiArray.size();

	for (UINT i=0; i<m_TransferApiArray.size(); i++)
	{
		m_TransferApiArray[i].pTransferApi->Destroy();
		delete m_TransferApiArray[i].pTransferApi;
		delete m_TransferApiArray[i].pLastActiveTime;
	}
	
	KillTimer(m_nTimerID);

	CMutex mutex(FALSE, _T("FileZilla Queue Mutex"), 0);
	CSingleLock mLock(&mutex, TRUE);
	
	CMarkupSTL *pMarkup;

	if (COptions::LockXML(&pMarkup))
	{
		pMarkup->ResetPos();
		if (!pMarkup->FindChildElem( _T("TransferQueue") ))
			pMarkup->AddChildElem( _T("TransferQueue") );

		pMarkup->IntoElem();
		
		for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
		{
			const CQueueData &queueData = *iter;
			if (!queueData.priority && !queueData.bPaused)
				continue;

			pMarkup->AddChildElem( _T("QueueItem") );
			pMarkup->IntoElem();
			pMarkup->AddChildElem( _T("File") );
			pMarkup->AddChildAttrib( _T("Localfile"), queueData.transferFile.localfile);
			pMarkup->AddChildAttrib( _T("Size"), queueData.transferFile.size);
			pMarkup->AddChildAttrib( _T("Remotefile"), queueData.transferFile.remotefile);
			pMarkup->AddChildAttrib( _T("Remotepath"), queueData.transferFile.remotepath.GetSafePath());
			
			pMarkup->AddChildElem( _T("Data") );
			pMarkup->AddChildAttrib( _T("Transfermode"), queueData.transferFile.nType);
			pMarkup->AddChildAttrib( _T("Get"), queueData.transferFile.get);
			pMarkup->AddChildAttrib( _T("RetryCount"), queueData.retrycount);
			pMarkup->AddChildAttrib( _T("Open"), queueData.nOpen);

			COptions::SaveServer(pMarkup, queueData.transferFile.server);

			pMarkup->OutOfElem();
		}
		COptions::UnlockXML();
	}
	else
	{
		
		int j=0;
		TCHAR buffer[1000];
		HKEY key;
		if (RegOpenKey(HKEY_CURRENT_USER, _T("Software\\FileZilla\\Queue\\") ,&key)==ERROR_SUCCESS)
		{
			while (RegEnumKey(key,j,buffer,1000)==ERROR_SUCCESS)
			{
				j++;
			}
			RegCloseKey(key);
		}
		
		for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
		{
			const CQueueData &queueData = *iter;
			if (!queueData.priority && !queueData.bPaused)
				continue;

			CString str;
			str.Format(_T("%d"), j);
			HKEY key2;
			if (RegCreateKeyEx(HKEY_CURRENT_USER,"Software\\FileZilla\\Queue\\"+str, 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &key2, NULL)==ERROR_SUCCESS)
			{
				COptions::SetKey(key2, "Localfile", queueData.transferFile.localfile);
				COptions::SetKey(key2, "Size", queueData.transferFile.size);
				COptions::SetKey(key2, "Remotefile", queueData.transferFile.remotefile);
				COptions::SetKey(key2, "Remotepath", queueData.transferFile.remotepath.GetSafePath());
				COptions::SetKey(key2, "Transfer mode", queueData.transferFile.nType);
				COptions::SetKey(key2, "Get", queueData.transferFile.get);
				COptions::SetKey(key2, "Retry Count", queueData.retrycount);
				COptions::SetKey(key2, "Open", queueData.nOpen);

				COptions::SaveServer(key2, queueData.transferFile.server);
				
				RegCloseKey(key2);
			}
			j++;
		}
	}

	m_nActiveCount = 0;
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		delete iter->pTransferStatus;
		delete iter->pProgressControl;
	}
	m_QueueItems.clear();
	DeleteAllItems();

	return FALSE;
}

BOOL CQueueCtrl::IsQueueEmpty() const
{
	return FindValidIndex(1) == m_QueueItems.begin();
}

void CQueueCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem= &(pDispInfo)->item;

	if (static_cast<int>(m_QueueItems.size() + m_nActiveCount) <= pItem->iItem)
		return;

	int nIndex;
	BOOL bStatusLine = FALSE;
	if (pItem->iItem < (m_nActiveCount * 2))
	{
		nIndex = pItem->iItem / 2;
		bStatusLine = pItem->iItem % 2;
	}
	else
		nIndex = pItem->iItem - m_nActiveCount;

	if (bStatusLine)
	{
		if (pItem->mask & LVIF_TEXT)
		{
			if (!pItem->pszText)
				return;
			CString str, str2;
			switch(pItem->iSubItem)
			{
			case 0:
				_tcsncpy(pItem->pszText, m_QueueItems[nIndex].status, pItem->cchTextMax);
				break;
			case 1:
				if (m_QueueItems[nIndex].pTransferStatus)
				{
					str2.Format(_T("%d:%02d:%02d"), m_QueueItems[nIndex].pTransferStatus->timeelapsed/3600, (m_QueueItems[nIndex].pTransferStatus->timeelapsed/60)%60, m_QueueItems[nIndex].pTransferStatus->timeelapsed%60);
					str.Format(IDS_STATUSBAR_ELAPSED, str2);
				}
				_tcsncpy(pItem->pszText, str, pItem->cchTextMax);
				break;
			case 2:
				if (m_QueueItems[nIndex].pTransferStatus && m_QueueItems[nIndex].pTransferStatus->timeleft!=-1)
				{
					str2.Format(_T("%d:%02d:%02d"), m_QueueItems[nIndex].pTransferStatus->timeleft/3600, (m_QueueItems[nIndex].pTransferStatus->timeleft/60)%60, m_QueueItems[nIndex].pTransferStatus->timeleft%60);
					str.Format(IDS_STATUSBAR_LEFT, str2);
				}
				_tcsncpy(pItem->pszText, str, pItem->cchTextMax);
				break;
			case 3:
				if (m_QueueItems[nIndex].pTransferStatus)
				{
					CString tmp=_T("? ");
					if (m_QueueItems[nIndex].pTransferStatus->transferrate)
					{
						if (m_QueueItems[nIndex].pTransferStatus->transferrate>(1024*1024))
						{
							tmp.Format(_T("%d.%d M"), m_QueueItems[nIndex].pTransferStatus->transferrate/1024/1024,(m_QueueItems[nIndex].pTransferStatus->transferrate/1024/102)%10);
						}
						else if (m_QueueItems[nIndex].pTransferStatus->transferrate>1024)
						{
							tmp.Format(_T("%d.%d K"), m_QueueItems[nIndex].pTransferStatus->transferrate/1024,(m_QueueItems[nIndex].pTransferStatus->transferrate/102)%10);
						}
						else
						{
							tmp.Format(_T("%d "), m_QueueItems[nIndex].pTransferStatus->transferrate);
						}
					}
					_sntprintf(pItem->pszText, pItem->cchTextMax, _T("%I64d bytes (%sB/s)"), m_QueueItems[nIndex].pTransferStatus->bytes, tmp);
				}
				break;
			}
			if (pItem->cchTextMax > 0) // nt4: never before the buffer, even with no room
				pItem->pszText[pItem->cchTextMax - 1] = 0;
		}
		return;
	}

	if (pItem->mask & LVIF_TEXT)
	{
		if (!pItem->pszText)
			return;
		switch(pItem->iSubItem)
		{
		case 0:
			_tcsncpy(pItem->pszText, m_QueueItems[nIndex].transferFile.localfile, pItem->cchTextMax);
			break;
		case 1:
			if (m_QueueItems[nIndex].transferFile.size>=0)
				_sntprintf(pItem->pszText, pItem->cchTextMax, _T("%I64d"), m_QueueItems[nIndex].transferFile.size);
			else
				_tcsncpy(pItem->pszText, _T("???"), pItem->cchTextMax);
			break;
		case 2:
			if (m_QueueItems[nIndex].priority==2)
				_tcsncpy(pItem->pszText, m_QueueItems[nIndex].transferFile.get?_T("<<---"):_T("--->>"), pItem->cchTextMax);
			else
				_tcsncpy(pItem->pszText, m_QueueItems[nIndex].transferFile.get?_T("<--"):_T("-->"), pItem->cchTextMax);
			break;
		case 3:
			_tcsncpy(pItem->pszText, m_QueueItems[nIndex].transferFile.remotepath.FormatFilename(m_QueueItems[nIndex].transferFile.remotefile), pItem->cchTextMax);
			break;
		case 4:
			_sntprintf(pItem->pszText, pItem->cchTextMax, _T("%s:%d"), m_QueueItems[nIndex].transferFile.server.host, m_QueueItems[nIndex].transferFile.server.port);
			break;
		case 5:
			_tcsncpy(pItem->pszText, m_QueueItems[nIndex].action, pItem->cchTextMax);
			break;
		default:
			_tcsncpy(pItem->pszText, _T(""), pItem->cchTextMax);
		}
		if (pItem->cchTextMax > 0) // nt4: never before the buffer, even with no room
			pItem->pszText[pItem->cchTextMax - 1] = 0;
	}
	if (pItem->mask & LVIF_STATE)
	{
		pItem->state = m_QueueItems[nIndex].nListItemState;
		pItem->stateMask = LVIS_SELECTED;
	}
}

BOOL CQueueCtrl::Validate()
{
	int bActive = TRUE;
	int nPriority = 2;
	int bPaused = TRUE;

	if (static_cast<unsigned int>(GetItemCount()) != (m_QueueItems.size() + m_nActiveCount))
	{
		TRACE3("List size (%d) doesn't match item count (%d) + m_nActiveCount (%d)\n", GetItemCount(), m_QueueItems.size(), m_nActiveCount);
		return FALSE;
	}
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (iter->bActive && !iter->priority)
		{
			TRACE1("Item %d active and with 0 priority\n", iter - m_QueueItems.begin());
			return FALSE;
		}
		else if (iter->bActive && !bActive)
		{
			TRACE1("Item %d active but inactive items above\n", iter - m_QueueItems.begin());
			return FALSE;
		}
		else if ((iter - m_QueueItems.begin()) < m_nActiveCount && !iter->bActive)
		{
			TRACE2("m_nActiveCount == %d and Item %d inactive\n", m_nActiveCount, iter - m_QueueItems.begin());
			return FALSE;
		}
		else if ((iter - m_QueueItems.begin()) >= m_nActiveCount && iter->bActive)
		{
			TRACE2("m_nActiveCount == %d and Item %d active\n", m_nActiveCount, iter - m_QueueItems.begin());
			return FALSE;
		}
		else if (iter->priority > nPriority)
		{
			TRACE3("priority == %d of item %d larger than nPriority == %d and item not active\n", iter->priority, iter - m_QueueItems.begin(), nPriority);
			return FALSE;
		}
		else if (iter->bPaused && !bPaused)
		{
			TRACE1("item %d paused but already processing unpaused items\n", iter - m_QueueItems.begin());
			return FALSE;
		}

		if (!iter->bActive)
			bActive = FALSE;

		if (!iter->priority && !iter->bPaused)
			bPaused = FALSE;

		if (iter->priority < nPriority && !iter->bActive)
		{
			nPriority = iter->priority;
		}
	}
	return TRUE;
}

void CQueueCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	if (static_cast<int>(m_QueueItems.size() + m_nActiveCount) <= pNMListView->iItem)
		return;

	if (pNMListView->iItem == -1)
	{
		if ((pNMListView->uNewState|pNMListView->uOldState) & LVIS_SELECTED)
			for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
				iter->nListItemState = pNMListView->uNewState;
		return;
	}

	int nIndex;
	BOOL bStatusLine = FALSE;
	if (pNMListView->iItem < (m_nActiveCount * 2))
	{
		nIndex = pNMListView->iItem / 2;
		bStatusLine = pNMListView->iItem % 2;
	}
	else
		nIndex = pNMListView->iItem - m_nActiveCount;

	if ((pNMListView->uNewState|pNMListView->uOldState) & LVIS_SELECTED)
		m_QueueItems[nIndex].nListItemState = pNMListView->uNewState|LVIS_SELECTED;
	RedrawItems(0, GetItemCount() -1);
}

void CQueueCtrl::OnOdstatechanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVODSTATECHANGE* pStateChanged = (NMLVODSTATECHANGE*)pNMHDR;

	if (static_cast<int>(m_QueueItems.size() + m_nActiveCount) <= pStateChanged->iTo)
		return;

	if (static_cast<int>(m_QueueItems.size() + m_nActiveCount) <= pStateChanged->iFrom)
		return;

	int iItem;
	for (iItem = pStateChanged->iFrom; iItem <= pStateChanged->iTo; iItem++)
	{
		int nIndex;
		BOOL bStatusLine = FALSE;
		if (iItem < (m_nActiveCount * 2))
		{
			nIndex = iItem / 2;
			bStatusLine = iItem % 2;
		}
		else
			nIndex = iItem - m_nActiveCount;

		if ((pStateChanged->uNewState | pStateChanged->uOldState) & LVIS_SELECTED)
			m_QueueItems[nIndex].nListItemState = pStateChanged->uNewState;
	}
	RedrawItems(pStateChanged->iFrom, pStateChanged->iTo);
	*pResult = 0;
}

BOOL CQueueCtrl::MayResumePrimaryTransfer()
{
	for (t_QueueVector::iterator iter = m_QueueItems.begin(); iter != m_QueueItems.end(); iter++)
	{
		if (!iter->bActive)
			break;
		if (!iter->pTransferApi)
		{
			if (iter->bTransferStarted && !iter->bTriedAutoResume && iter->transferFile.nType != 1)
			{
				iter->bTriedAutoResume = true;
				return TRUE;
			}

			break;
		}
	}

	return FALSE;
}

void CQueueCtrl::LoadQueue()
{
	CMutex mutex(FALSE, _T("FileZilla Queue Mutex"), 0);
	CSingleLock mLock(&mutex, TRUE);
	
	int failure = 0;

	CMarkupSTL *pMarkup;
	//First try to read queue items from filezilla.xml
	if (COptions::LockXML(&pMarkup))
	{
		pMarkup->ResetPos();
		if (pMarkup->FindChildElem( _T("TransferQueue") ))
		{
			pMarkup->IntoElem();
			while (pMarkup->FindChildElem( _T("QueueItem") ))
			{
				pMarkup->IntoElem();
				t_transferfile transferfile;

				if (!pMarkup->FindChildElem( _T("File") ))
				{
					pMarkup->OutOfElem();
					continue;
				}

				transferfile.localfile = pMarkup->GetChildAttrib( _T("Localfile") );
				transferfile.remotefile = pMarkup->GetChildAttrib( _T("Remotefile") );
				transferfile.remotepath.SetSafePath(pMarkup->GetChildAttrib( _T("Remotepath") ));
				transferfile.size = _ttoi64(pMarkup->GetChildAttrib( _T("Size") ));

				if (!pMarkup->FindChildElem( _T("Data") ))
				{
					pMarkup->OutOfElem();
					continue;
				}

				CQueueData queueData;

				queueData.retrycount=_ttoi(pMarkup->GetChildAttrib( _T("RetryCount") ));
				transferfile.nType=_ttoi(pMarkup->GetChildAttrib( _T("Transfermode") ));
				transferfile.get=_ttoi(pMarkup->GetChildAttrib( _T("Get") ));
				queueData.nOpen=_ttoi(pMarkup->GetChildAttrib( _T("Open") ));

				if (!COptions::LoadServer(pMarkup, transferfile.server))
				{
					pMarkup->OutOfElem();
					continue;
				}

				pMarkup->OutOfElem();

				queueData.transferFile = transferfile;

				if (transferfile.localfile=="" || transferfile.remotefile=="" || transferfile.remotepath.IsEmpty())
					failure++;
				else
					AddItem(transferfile);
			}
			pMarkup->OutOfElem();
			pMarkup->RemoveChildElem();
		}
		COptions::UnlockXML();
	}

	if (failure)
	{
		CString str;
		str.Format(_T("Error: %d queue items could not be loaded"), failure);
		AfxMessageBox(str);
	}

	HKEY key;
	if (RegOpenKey(HKEY_CURRENT_USER, _T("Software\\FileZilla\\Queue\\"), &key)==ERROR_SUCCESS)
	{
		TCHAR buffer[MAX_PATH + 1];
		

		int index=0;
		//First try to read the queue items sequential
		while (TRUE)
		{
			CString subkey;
			subkey.Format(_T("%d"), index);
			HKEY key2;
			if (RegOpenKey(HKEY_CURRENT_USER, _T("Software\\FileZilla\\Queue\\")+subkey,&key2)!=ERROR_SUCCESS)
				break;

			t_transferfile transferfile;
			COptions::GetKey(key2, "Localfile", transferfile.localfile);
			COptions::GetKey(key2, "Remotefile", transferfile.remotefile);
			CString value;
			COptions::GetKey(key2, "Remotepath",value);
			transferfile.remotepath.SetSafePath(value);
			COptions::GetKey(key2, "Transfer mode", transferfile.nType);
			COptions::GetKey(key2, "Get", transferfile.get);
			COptions::GetKey(key2, "Size", transferfile.size);
			CQueueData queueData;
			COptions::GetKey(key2, "Retry Count", queueData.retrycount);
			COptions::GetKey(key2, "Open", queueData.nOpen);
			
			if (!COptions::LoadServer(key2, transferfile.server))
			{
				RegCloseKey(key2);
				RegDeleteKey(HKEY_CURRENT_USER, _T("Software\\FileZilla\\QUEUE\\")+subkey);
				break;
			}
			
			queueData.transferFile = transferfile;
			if (transferfile.localfile=="" || transferfile.remotefile=="" || transferfile.remotepath.IsEmpty())
			{
				AfxMessageBox( _T("Error, can't reload queue item!") );
			}
			else
			{
				int index2=AddItem(transferfile);
			}
			RegCloseKey(key2);
			RegDeleteKey(HKEY_CURRENT_USER, _T("Software\\FileZilla\\QUEUE\\")+subkey);
			index++;
		}

		//Now try to find out-of-order queue items
		index=0;
		while (RegEnumKey(key, index, buffer, MAX_PATH+1)==ERROR_SUCCESS)
		{
			t_transferfile transferfile;
			CString str=buffer;
			HKEY key2;
			if (RegOpenKey(HKEY_CURRENT_USER,"Software\\FileZilla\\Queue\\"+str,&key2)==ERROR_SUCCESS)
			{
				COptions::GetKey(key2, "Localfile", transferfile.localfile);
				COptions::GetKey(key2, "Remotefile", transferfile.remotefile);
				CString value;
				COptions::GetKey(key2, "Remotepath",value);
				transferfile.remotepath.SetSafePath(value);
				COptions::GetKey(key2, "Transfer mode", transferfile.nType);
				COptions::GetKey(key2, "Get", transferfile.get);
				COptions::GetKey(key2, "Size", transferfile.size);
				CQueueData queueData;
				COptions::GetKey(key2, "Retry Count", queueData.retrycount);
				COptions::GetKey(key2, "Open", queueData.nOpen);
				
				queueData.transferFile = transferfile;
				if (!COptions::LoadServer(key2, transferfile.server))
				{
					RegCloseKey(key2);
					RegDeleteKey(HKEY_CURRENT_USER, _T("Software\\FileZilla\\QUEUE\\")+str);
					break;
				}
				
				if (transferfile.localfile=="" || transferfile.remotefile=="" || transferfile.remotepath.IsEmpty())
					AfxMessageBox( _T("Error, can't reload queue item!") );
				else
					AddItem(transferfile);
				RegCloseKey(key2);
			}
			if (RegDeleteKey(HKEY_CURRENT_USER, _T("Software\\FileZilla\\QUEUE\\")+str)!=ERROR_SUCCESS) 
				index++;
		}
		RegCloseKey(key);
	}
	mLock.Unlock();
}
