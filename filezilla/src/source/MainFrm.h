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

// MainFrm.h : Schnittstelle der Klasse CMainFrame
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__81E5968B_9DF3_4913_911B_1018731A6051__INCLUDED_)
#define AFX_MAINFRM_H__81E5968B_9DF3_4913_911B_1018731A6051__INCLUDED_

#include "FtpView.h"	// Hinzugefügt von der Klassenansicht
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFtpView;
class CStatusView;
class CLocalView;
class CLocalView2;
class CQueueView;
class CFtpTreeView;

#include "TextProgressCtrl.h"
#include "splitex.h"
#include "misc\led.h"
#include "ServerChangePass.h"
#include "misc\systemtray.h"
#include "SiteManager.h"
#include "misc\CoolBtn.h"
#include "misc\TransparentDialogBar.h"

class CCommandQueue;
class CFileZillaApi;
class CMinTrayBtn;

class CMainFrame : public CFrameWnd
{
	friend CCommandQueue;
public:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attribute
public:

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementierung
public:
	void ParseSite(t_SiteManagerItem &site);
	UINT_PTR m_nSecTimerID;
	CFileZillaApi* m_pFileZillaApi;
	UINT m_nFileZillaApiMessageID;
	CCommandQueue *m_pCommandQueue;
	t_LocalViewHeader *m_pLocalViewHeader;
	t_RemoteViewHeader *m_pRemoteViewHeader;
	CLed m_SendLed;
	CLed m_RecvLed;
	
	void UploadDir(CString dir, CString subdir, BOOL upload, const CString& user = _T(""), const CString& pass =_T(""));
	
	int m_nTransferType;
	//Begin of a Drag&Drop operation. Called by the list and tree views
	void OnBegindrag(CWnd* pDragWnd, CPoint point);

	//Process the queue. If StdTransfer is TRUE, only the direct up- / downloads will be 
	//transferred, not the previous queued items
	void TransferQueue(int nPriority);

	//Add an item to the queue, set stdtransfer to true if it should be a direct up- / download
	void AddQueueItem(BOOL get, CString filename, CString subdir, CString localdir, const CServerPath &remotepath, BOOL stdtransfer, CString user="", CString pass="", int nOpen = 0);

	//// Internal support functions
	void SetupTrayIcon();
	
	//// Internal data
	CSystemTray m_TrayIcon;
	int nTrayNotificationMsg_;

	void SetLocalFolder(CString folder);
	CQueueView*	 GetQueuePane();
	CFtpView*	 GetFtpPane();
	CFtpTreeView*GetFtpTreePane();
	CLocalView*	 GetLocalPane();
	CLocalView2* GetLocalPane2();
	CStatusView* GetStatusPane();
	void RefreshViews(int side=0);

	bool IsConnected() const;

	void SetProgress(t_ffam_transferstatus *status);

	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CStatusBar  m_wndStatusBar;
	
protected:
	void SaveMostRecentServers();

	//Drag&Drop
	CWnd* m_pDragWnd;
	CImageList* m_pDragImage;
	HCURSOR m_hcArrow;
	HCURSOR m_hcNo;
	CString m_DropTarget;

	BOOL m_bShowQueue;
	int initialized;
	CSplitterWndEx m_wndVertSplitter;
	CSplitterWndEx m_wndMainSplitter;
	CSplitterWndEx m_wndLocalSplitter;
	CSplitterWndEx m_wndRemoteSplitter;
	BOOL m_bShowTree;
	CTextProgressCtrl m_ProgressCtrl;
	CToolBar    *m_pWndToolBar;
	CReBar      *m_pWndReBar;
	CTransparentDialogBar  *m_pWndDlgBar;
	CCoolBtn	m_wndQuickconnectButton;
	CImageList* CreateDragImageEx(CListCtrl *pList, LPPOINT lpPoint);
	CImageList* CreateDragImageEx(CTreeCtrl *pTree, LPPOINT lpPoint);
	void SetStatusbarText(int nIndex,CString str);
	BOOL m_bQuit;
	// static data member to hold window class name
	static LPCTSTR s_winClassName;
	std::list<t_server> m_RecentQuickconnectServers;
	int m_nRecentQuickconnectCommandOffset;
	int m_bSwitchPaneLayout;

	//Structure to remember the window size/position
	struct t_posdata
	{
		int state;
		CRect NormalRect;
		int cy1, cy2, localPos, remotePos, cx;
	} m_PosData;

	// Minimize to tray button
	BOOL m_bMaxBeforeTray;
	CMinTrayBtn *m_pMinTrayBtn;

	int m_nLocalTreeViewLocation;
	int m_nRemoteTreeViewLocation;

protected:
	HWND m_hLastFocus;
	BOOL CreateToolbars();
	void ProcessQuickconnect(BOOL bBypass);
	BOOL m_bShowRemoteTree;
	BOOL m_bShowHiddenFiles;
	CStatic* m_pSecureIconCtrl;
	BOOL RestoreSize();
	int m_nLocalListViewStyle;
	int m_nRemoteListViewStyle;
	int m_nHideLocalColumns;
	int m_nHideRemoteColumns;
	BOOL m_bShowMessageLog;
	BOOL m_bShowQuickconnect;
// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnTrayExit();
	afx_msg void OnTrayRestore();
	afx_msg LRESULT OnProcessCmdLine(WPARAM wParam,LPARAM lParam);
	afx_msg void OnProcessQueue();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnQuickConnect();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateShowtree(CCmdUI* pCmdUI);
	afx_msg void OnShowtree();
	afx_msg void OnEditSettings();
	afx_msg void OnSitemanager();
	afx_msg void OnCancel();
	afx_msg void OnUpdateCancelbutton(CCmdUI* pCmdUI);
	afx_msg void OnShowqueue();
	afx_msg void OnUpdateShowqueue(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnUpdateProcessQueue(CCmdUI* pCmdUI);
	afx_msg void OnToolbarDisconnect();
	afx_msg void OnUpdateToolbarDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnToolbarRefresh();
	afx_msg void OnToolbarReconnect();
	afx_msg void OnUpdateToolbarReconnect(CCmdUI* pCmdUI);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCopytositemanager();
	afx_msg void OnUpdateCopytositemanager(CCmdUI* pCmdUI);
	afx_msg void OnTypemenuAscii();
	afx_msg void OnUpdateTypemenuAscii(CCmdUI* pCmdUI);
	afx_msg void OnTypemenuBinary();
	afx_msg void OnUpdateTypemenuBinary(CCmdUI* pCmdUI);
	afx_msg void OnTypemenuDetect();
	afx_msg void OnUpdateTypemenuDetect(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnViewQuickconnectBar();
	afx_msg void OnUpdateViewQuickconnectBar(CCmdUI* pCmdUI);
	afx_msg void OnViewMessagelog();
	afx_msg void OnUpdateViewMessagelog(CCmdUI* pCmdUI);
	afx_msg void OnViewStatusBar();
	afx_msg void OnViewToolbar();
	afx_msg void OnEditExportsettings();
	afx_msg void OnEditImportsettings();
	afx_msg void OnLocalliststyleIcon();
	afx_msg void OnUpdateLocalliststyleIcon(CCmdUI* pCmdUI);
	afx_msg void OnLocalliststyleList();
	afx_msg void OnUpdateLocalliststyleList(CCmdUI* pCmdUI);
	afx_msg void OnLocalliststyleReport();
	afx_msg void OnUpdateLocalliststyleReport(CCmdUI* pCmdUI);
	afx_msg void OnLocalliststyleSmallicon();
	afx_msg void OnUpdateLocalliststyleSmallicon(CCmdUI* pCmdUI);
	afx_msg void OnViewLocallistviewFilesize();
	afx_msg void OnUpdateViewLocallistviewFilesize(CCmdUI* pCmdUI);
	afx_msg void OnViewLocallistviewFiletype();
	afx_msg void OnUpdateViewLocallistviewFiletype(CCmdUI* pCmdUI);
	afx_msg void OnViewLocallistviewLastmodifiedtime();
	afx_msg void OnUpdateViewLocallistviewLastmodifiedtime(CCmdUI* pCmdUI);
	afx_msg void OnRemoteliststyleIcon();
	afx_msg void OnUpdateRemoteliststyleIcon(CCmdUI* pCmdUI);
	afx_msg void OnRemoteliststyleList();
	afx_msg void OnUpdateRemoteliststyleList(CCmdUI* pCmdUI);
	afx_msg void OnRemoteliststyleReport();
	afx_msg void OnUpdateRemoteliststyleReport(CCmdUI* pCmdUI);
	afx_msg void OnRemoteliststyleSmallicon();
	afx_msg void OnUpdateRemoteliststyleSmallicon(CCmdUI* pCmdUI);
	afx_msg void OnViewRemotelistviewDate();
	afx_msg void OnUpdateViewRemotelistviewDate(CCmdUI* pCmdUI);
	afx_msg void OnViewRemotelistviewTime();
	afx_msg void OnUpdateViewRemotelistviewTime(CCmdUI* pCmdUI);
	afx_msg void OnViewRemotelistviewFilesize();
	afx_msg void OnUpdateViewRemotelistviewFilesize(CCmdUI* pCmdUI);
	afx_msg void OnViewRemotelistviewFiletype();
	afx_msg void OnUpdateViewRemotelistviewFiletype(CCmdUI* pCmdUI);
	afx_msg void OnViewRemotelistviewPermissions();
	afx_msg void OnUpdateViewRemotelistviewPermissions(CCmdUI* pCmdUI);
	afx_msg void OnFileConnecttodefaultsite();
	afx_msg void OnHelpmenuContents();
	afx_msg void OnHelpmenuIndex();
	afx_msg void OnHelpmenuSearch();
	afx_msg void OnUpdateFileConnecttodefaultsite(CCmdUI* pCmdUI);
	afx_msg void OnOverwritemenuAsk();
	afx_msg void OnOverwritemenuOverwrite();
	afx_msg void OnOverwritemenuOverwriteifnewer();
	afx_msg void OnOverwritemenuResume();
	afx_msg void OnOverwritemenuSkip();
	afx_msg void OnUpdateOverwritemenuAsk(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOverwritemenuOverwrite(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOverwritemenuOverwriteifnewer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOverwritemenuResume(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOverwritemenuSkip(CCmdUI* pCmdUI);
	afx_msg void OnOverwritemenuRename();
	afx_msg void OnUpdateOverwritemenuRename(CCmdUI* pCmdUI);
	afx_msg void OnMenuQueueExport();
	afx_msg void OnUpdateMenuQueueExport(CCmdUI* pCmdUI);
	afx_msg void OnMenuQueueImport();
	afx_msg void OnUpdateMenuQueueProcessnow(CCmdUI* pCmdUI);
	afx_msg void OnMenuQueueUseMultiple();
	afx_msg void OnUpdateMenuQueueUseMultiple(CCmdUI* pCmdUI);
	afx_msg void OnMenuQueueProcessnow();
	afx_msg void OnMenuTransferManualtransfer();
	afx_msg void OnSysCommand( UINT nID, LPARAM lParam );
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMenuSiteChangepass();
	afx_msg void OnUpdateMenuSiteChangepass(CCmdUI* pCmdUI);
	afx_msg void OnMenuViewShowhidden();
	afx_msg void OnUpdateMenuViewShowhidden(CCmdUI* pCmdUI);
	afx_msg void OnShowremotetree();
	afx_msg void OnUpdateShowremotetree(CCmdUI* pCmdUI);
	afx_msg void OnMenuDebugDumpDirectoryCache();
	afx_msg void OnMenuDebugCrash();
	afx_msg void OnMenuServerEnterrawcommand();
	afx_msg void OnUpdateMenuServerEnterrawcommand(CCmdUI* pCmdUI);
	afx_msg void OnQuickconnectBarMenuClear();
	afx_msg void OnQuickconnectBarMenuEmptyHistory();
	afx_msg void OnQuickconnectBarMenuBypass();
	afx_msg void OnMenuServerCopyurltoclipboard();
	afx_msg void OnUpdateMenuServerCopyurltoclipboard(CCmdUI* pCmdUI);
	afx_msg void OnToolbarDropDown(NMHDR* pnmh, LRESULT* plRes);
	afx_msg void OnMenuViewRemotelistviewStatusbar();
	afx_msg void OnMenuViewLocallistviewStatusbar();
	afx_msg void OnUpdateMenuViewRemotelistviewStatusbar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMenuViewLocallistviewStatusbar(CCmdUI* pCmdUI);
	afx_msg void OnViewRemotelistviewOwnergroup();
	afx_msg void OnUpdateViewRemotelistviewOwnergroup(CCmdUI* pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnUpdateMenuViewLocalSortby(CCmdUI* pCmdUI);
	afx_msg void OnMenuViewLocalSortby(UINT nID);
	afx_msg void OnUpdateMenuViewRemoteSortby(CCmdUI* pCmdUI);
	afx_msg void OnMenuViewRemoteSortby(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_MAINFRM_H__81E5968B_9DF3_4913_911B_1018731A6051__INCLUDED_)
