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

#if !defined(AFX_QUEUECTRL_H__29A6A721_3E9D_495F_8456_9290AA87CF9C__INCLUDED_)
#define AFX_QUEUECTRL_H__29A6A721_3E9D_495F_8456_9290AA87CF9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueueCtrl.h : Header-Datei
//

#include "RetryServerList.h"

class CTextProgressCtrl;
class CQueueData
{
public:
	CQueueData();
	~CQueueData();

	t_transferfile transferFile;

	int retrycount;
	int priority;
	BOOL bActive;
	int nOpen;
	int nState;
	BOOL bStop;
	int nAutoUsePrimary;
	t_ffam_transferstatus *pTransferStatus;
	CFileZillaApi *pTransferApi;
	CTextProgressCtrl *pProgressControl;
	int nProgressOffset;
	BOOL bTransferStarted;	//True if the actual transfer has been started
							//Required for "Stop selected"/"Resume selected"
	bool bTriedAutoResume;
	BOOL bPaused;
	BOOL bAbort;

	CString action;
	CString status;

	int nListItemState;
};

typedef std::deque<CQueueData> t_QueueVector;

/////////////////////////////////////////////////////////////////////////////
// Fenster CQueueCtrl 

#define QUEUESTATE_INIT 0
#define QUEUESTATE_DISCONNECT 1
#define QUEUESTATE_CONNECT 2
#define QUEUESTATE_TRANSFER 3

class CQueueCtrl : public CListCtrl
{
public:
	CQueueCtrl();
	virtual ~CQueueCtrl();

	void LoadQueue();

	BOOL DoClose();
	t_QueueVector::const_iterator FindValidIndex(int postype) const;
	t_QueueVector::iterator FindValidIndex(int postype);
	void Import();
	void Export();
	void StopProcessing();
	void TransferQueue(int nPriority);
	void UpdateStatusbar();
	int DoProcessQueue() const;
	int TransferNextFile(int &nApiIndex);
	void ProcessReply(int nReplyCode, int nApiIndex = -1);
	int AddItem(t_transferfile transferfile, BOOL stdtransfer=FALSE, int nOpen = 0);
	int AddItem(CQueueData queueData);
	void ReloadHeader();
	void SetApiCount(int nCount);
	BOOL SetProgress(CFileZillaApi *pTransferApi, t_ffam_transferstatus *pTransferStatus);
	void ToggleUseMultiple();
	BOOL DoUseMultiple() const;
	BOOL IsQueueEmpty() const;
	BOOL MayResumePrimaryTransfer();


	
	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	BOOL Validate();
	t_QueueVector m_QueueItems;

	BOOL m_bUseMultiple;
	BOOL m_bPrimaryConnectionTransferInProgress;
	UINT m_nMaxApiCount;
	struct t_TransferApi
	{
		CFileZillaApi *pTransferApi;
		CTime *pLastActiveTime;
		BOOL bActive;
	};
	struct t_ServerData
	{
		int nActiveMax;
		int nFailedConnections;
		int nCurrentActive;
	};
	typedef std::map<t_server, t_ServerData> t_ServerDataMap;
	typedef t_ServerDataMap::iterator t_ServerDataMapIter;
	t_ServerDataMap m_ServerDataMap;
	int m_bMayUsePrimaryConnection;
	UINT_PTR m_nTimerID;
	int m_nProcessQueue;
	std::vector<t_TransferApi> m_TransferApiArray;
	int m_nActiveCount;
	BOOL m_bQuit;
	int m_nQuitCount;
	CRetryServerList m_RetryServerList;

	void DrawStatusLine(HDC hdc, RECT ItemRect, int nItem);
	static DWORD WINAPI ViewEditThreadProc(LPVOID lpParameter);
	void EditFile(const CQueueData &queueData);
	void ReloadHeaderItem(int nIndex, UINT nID);
	
	//{{AFX_MSG(CQueueCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDestroy();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnQueuecontextMovedown();
	afx_msg void OnQueuecontextMovetobottom();
	afx_msg void OnQueuecontextMovetotop();
	afx_msg void OnQueuecontextMoveup();
	afx_msg void OnQueuecontextProcessqueue();
	afx_msg void OnQueuecontextRemovefromqueue();
	afx_msg void OnQueuecontextResetstatus();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnQueuecontextAbort();
	afx_msg void OnQueuecontextPause();
	afx_msg void OnQueuecontextResume();
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOdstatechanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_QUEUECTRL_H__29A6A721_3E9D_495F_8456_9290AA87CF9C__INCLUDED_
