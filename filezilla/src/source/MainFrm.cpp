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

// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "FileZilla.h"

#include "MainFrm.h"
#include "LocalView.h"
#include "LocalView2.h"
#include "StatusView.h"
#include "FtpView.h"
#include "StatusView.h"
#include "TextProgressCtrl.h"
#include "sitemanager.h"
#include "QueueView.h"
#include "queuectrl.h"
#include "misc\led.h"
#include "localfilelistctrl.h"
#include "FtpListCtrl.h"
#include "dirtreectrl.h"
#include "PathFunctions.h"
#include "Entersomething.h"
#include "fileexistsdlg.h"
#include "LocalComboCompletion.h"
#include "RemoteComboCompletion.h"
#include "FileZillaApi.h"
#include "commandqueue.h"
#include "misc/systemtray.h"
#include "ftptreeview.h"
#include "version.h"
#include "ManualTransferDlg.h"
#include "AsyncRequestQueue.h"
#include "misc\MinTrayBtn.h"
#include "misc\VisualStylesXP.h"
#include "ftptreectrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum { CMDLINE_CLOSE = 997 }; // Timer id used to close main dialog from command line

//Implementation in SiteManger.cpp
bool NewItem_Menu( LPVOID lpData, LPCTSTR lpszName, LPVOID lpParent, t_SiteManagerItem *site, bool isDefault, LPVOID lpDefault);
bool NewFolder_Menu( LPVOID lpData, LPCTSTR lpszName, LPVOID &lpParent);
bool After_Menu( LPVOID lpData, LPCTSTR lpszName, LPVOID lpItem);

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_COMMAND(ID_TRAY_EXIT, OnTrayExit)
	ON_COMMAND(ID_TRAY_RESTORE, OnTrayRestore)
    ON_REGISTERED_MESSAGE(WM_FILEZILLA_PROCESSCMDLINE, OnProcessCmdLine)
	ON_COMMAND(ID_PROCESS_QUEUE, OnProcessQueue)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDOK, OnQuickConnect)
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_SHOWTREE, OnUpdateShowtree)
	ON_COMMAND(ID_SHOWTREE, OnShowtree)
	ON_COMMAND(ID_EDIT_SETTINGS, OnEditSettings)
	ON_COMMAND(ID_SITEMANAGER, OnSitemanager)
	ON_UPDATE_COMMAND_UI(ID_CANCELBUTTON, OnUpdateCancelbutton)
	ON_COMMAND(ID_SHOWQUEUE, OnShowqueue)
	ON_UPDATE_COMMAND_UI(ID_SHOWQUEUE, OnUpdateShowqueue)
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_PROCESS_QUEUE, OnUpdateProcessQueue)
	ON_COMMAND(ID_TOOLBAR_DISCONNECT, OnToolbarDisconnect)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_DISCONNECT, OnUpdateToolbarDisconnect)
	ON_COMMAND(ID_TOOLBAR_REFRESH, OnToolbarRefresh)
	ON_COMMAND(ID_TOOLBAR_RECONNECT, OnToolbarReconnect)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_RECONNECT, OnUpdateToolbarReconnect)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_COPYTOSITEMANAGER, OnCopytositemanager)
	ON_UPDATE_COMMAND_UI(ID_COPYTOSITEMANAGER, OnUpdateCopytositemanager)
	ON_COMMAND(ID_TYPEMENU_ASCII, OnTypemenuAscii)
	ON_UPDATE_COMMAND_UI(ID_TYPEMENU_ASCII, OnUpdateTypemenuAscii)
	ON_COMMAND(ID_TYPEMENU_BINARY, OnTypemenuBinary)
	ON_UPDATE_COMMAND_UI(ID_TYPEMENU_BINARY, OnUpdateTypemenuBinary)
	ON_COMMAND(ID_TYPEMENU_DETECT, OnTypemenuDetect)
	ON_UPDATE_COMMAND_UI(ID_TYPEMENU_DETECT, OnUpdateTypemenuDetect)
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_QUICKCONNECT_BAR, OnViewQuickconnectBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_QUICKCONNECT_BAR, OnUpdateViewQuickconnectBar)
	ON_COMMAND(ID_VIEW_MESSAGELOG, OnViewMessagelog)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MESSAGELOG, OnUpdateViewMessagelog)
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_COMMAND(ID_EDIT_EXPORTSETTINGS, OnEditExportsettings)
	ON_COMMAND(ID_EDIT_IMPORTSETTINGS, OnEditImportsettings)
	ON_COMMAND(ID_LOCALLISTSTYLE_ICON, OnLocalliststyleIcon)
	ON_UPDATE_COMMAND_UI(ID_LOCALLISTSTYLE_ICON, OnUpdateLocalliststyleIcon)
	ON_COMMAND(ID_LOCALLISTSTYLE_LIST, OnLocalliststyleList)
	ON_UPDATE_COMMAND_UI(ID_LOCALLISTSTYLE_LIST, OnUpdateLocalliststyleList)
	ON_COMMAND(ID_LOCALLISTSTYLE_REPORT, OnLocalliststyleReport)
	ON_UPDATE_COMMAND_UI(ID_LOCALLISTSTYLE_REPORT, OnUpdateLocalliststyleReport)
	ON_COMMAND(ID_LOCALLISTSTYLE_SMALLICON, OnLocalliststyleSmallicon)
	ON_UPDATE_COMMAND_UI(ID_LOCALLISTSTYLE_SMALLICON, OnUpdateLocalliststyleSmallicon)
	ON_COMMAND(ID_VIEW_LOCALLISTVIEW_FILESIZE, OnViewLocallistviewFilesize)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOCALLISTVIEW_FILESIZE, OnUpdateViewLocallistviewFilesize)
	ON_COMMAND(ID_VIEW_LOCALLISTVIEW_FILETYPE, OnViewLocallistviewFiletype)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOCALLISTVIEW_FILETYPE, OnUpdateViewLocallistviewFiletype)
	ON_COMMAND(ID_VIEW_LOCALLISTVIEW_LASTMODIFIEDTIME, OnViewLocallistviewLastmodifiedtime)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOCALLISTVIEW_LASTMODIFIEDTIME, OnUpdateViewLocallistviewLastmodifiedtime)
	ON_COMMAND(ID_REMOTELISTSTYLE_ICON, OnRemoteliststyleIcon)
	ON_UPDATE_COMMAND_UI(ID_REMOTELISTSTYLE_ICON, OnUpdateRemoteliststyleIcon)
	ON_COMMAND(ID_REMOTELISTSTYLE_LIST, OnRemoteliststyleList)
	ON_UPDATE_COMMAND_UI(ID_REMOTELISTSTYLE_LIST, OnUpdateRemoteliststyleList)
	ON_COMMAND(ID_REMOTELISTSTYLE_REPORT, OnRemoteliststyleReport)
	ON_UPDATE_COMMAND_UI(ID_REMOTELISTSTYLE_REPORT, OnUpdateRemoteliststyleReport)
	ON_COMMAND(ID_REMOTELISTSTYLE_SMALLICON, OnRemoteliststyleSmallicon)
	ON_UPDATE_COMMAND_UI(ID_REMOTELISTSTYLE_SMALLICON, OnUpdateRemoteliststyleSmallicon)
	ON_COMMAND(ID_VIEW_REMOTELISTVIEW_DATE, OnViewRemotelistviewDate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REMOTELISTVIEW_DATE, OnUpdateViewRemotelistviewDate)
	ON_COMMAND(ID_VIEW_REMOTELISTVIEW_TIME, OnViewRemotelistviewTime)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REMOTELISTVIEW_TIME, OnUpdateViewRemotelistviewTime)
	ON_COMMAND(ID_VIEW_REMOTELISTVIEW_FILESIZE, OnViewRemotelistviewFilesize)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REMOTELISTVIEW_FILESIZE, OnUpdateViewRemotelistviewFilesize)
	ON_COMMAND(ID_VIEW_REMOTELISTVIEW_FILETYPE, OnViewRemotelistviewFiletype)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REMOTELISTVIEW_FILETYPE, OnUpdateViewRemotelistviewFiletype)
	ON_COMMAND(ID_VIEW_REMOTELISTVIEW_PERMISSIONS, OnViewRemotelistviewPermissions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REMOTELISTVIEW_PERMISSIONS, OnUpdateViewRemotelistviewPermissions)
	ON_COMMAND(ID_FILE_CONNECTTODEFAULTSITE, OnFileConnecttodefaultsite)
	ON_COMMAND(ID_HELPMENU_CONTENTS, OnHelpmenuContents)
	ON_COMMAND(ID_HELPMENU_INDEX, OnHelpmenuIndex)
	ON_COMMAND(ID_HELPMENU_SEARCH, OnHelpmenuSearch)
	ON_UPDATE_COMMAND_UI(ID_FILE_CONNECTTODEFAULTSITE, OnUpdateFileConnecttodefaultsite)
	ON_COMMAND(ID_OVERWRITEMENU_ASK, OnOverwritemenuAsk)
	ON_COMMAND(ID_OVERWRITEMENU_OVERWRITE, OnOverwritemenuOverwrite)
	ON_COMMAND(ID_OVERWRITEMENU_OVERWRITEIFNEWER, OnOverwritemenuOverwriteifnewer)
	ON_COMMAND(ID_OVERWRITEMENU_RESUME, OnOverwritemenuResume)
	ON_COMMAND(ID_OVERWRITEMENU_SKIP, OnOverwritemenuSkip)
	ON_UPDATE_COMMAND_UI(ID_OVERWRITEMENU_ASK, OnUpdateOverwritemenuAsk)
	ON_UPDATE_COMMAND_UI(ID_OVERWRITEMENU_OVERWRITE, OnUpdateOverwritemenuOverwrite)
	ON_UPDATE_COMMAND_UI(ID_OVERWRITEMENU_OVERWRITEIFNEWER, OnUpdateOverwritemenuOverwriteifnewer)
	ON_UPDATE_COMMAND_UI(ID_OVERWRITEMENU_RESUME, OnUpdateOverwritemenuResume)
	ON_UPDATE_COMMAND_UI(ID_OVERWRITEMENU_SKIP, OnUpdateOverwritemenuSkip)
	ON_COMMAND(ID_OVERWRITEMENU_RENAME, OnOverwritemenuRename)
	ON_UPDATE_COMMAND_UI(ID_OVERWRITEMENU_RENAME, OnUpdateOverwritemenuRename)
	ON_COMMAND(ID_MENU_QUEUE_EXPORT, OnMenuQueueExport)
	ON_UPDATE_COMMAND_UI(ID_MENU_QUEUE_EXPORT, OnUpdateMenuQueueExport)
	ON_COMMAND(ID_MENU_QUEUE_IMPORT, OnMenuQueueImport)
	ON_UPDATE_COMMAND_UI(ID_MENU_QUEUE_PROCESSNOW, OnUpdateMenuQueueProcessnow)
	ON_COMMAND(ID_MENU_QUEUE_USEMULTIPLE, OnMenuQueueUseMultiple)
	ON_UPDATE_COMMAND_UI(ID_MENU_QUEUE_USEMULTIPLE, OnUpdateMenuQueueUseMultiple)
	ON_COMMAND(ID_MENU_QUEUE_PROCESSNOW, OnMenuQueueProcessnow)
	ON_COMMAND(ID_MENU_TRANSFER_MANUALTRANSFER, OnMenuTransferManualtransfer)
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()
	ON_COMMAND(ID_MENU_SERVER_CHANGEPASS, OnMenuSiteChangepass)
	ON_UPDATE_COMMAND_UI(ID_MENU_SERVER_CHANGEPASS, OnUpdateMenuSiteChangepass)
	ON_COMMAND(ID_MENU_VIEW_SHOWHIDDEN, OnMenuViewShowhidden)
	ON_UPDATE_COMMAND_UI(ID_MENU_VIEW_SHOWHIDDEN, OnUpdateMenuViewShowhidden)
	ON_COMMAND(ID_SHOWREMOTETREE, OnShowremotetree)
	ON_UPDATE_COMMAND_UI(ID_SHOWREMOTETREE, OnUpdateShowremotetree)
	ON_COMMAND(ID_MENU_DEBUG_DUMPDIRCACHE, OnMenuDebugDumpDirectoryCache)
	ON_COMMAND(ID_MENU_DEBUG_CRASH, OnMenuDebugCrash)
	ON_COMMAND(ID_MENU_SERVER_ENTERRAWCOMMAND, OnMenuServerEnterrawcommand)
	ON_UPDATE_COMMAND_UI(ID_MENU_SERVER_ENTERRAWCOMMAND, OnUpdateMenuServerEnterrawcommand)
	ON_COMMAND(ID_QUICKCONNECTBAR_MENU_CLEAR, OnQuickconnectBarMenuClear)
	ON_COMMAND(ID_QUICKCONNECTBAR_MENU_EMPTYHISTORY, OnQuickconnectBarMenuEmptyHistory)
	ON_COMMAND(ID_QUICKCONNECTBAR_MENU_BYPASS, OnQuickconnectBarMenuBypass)
	ON_COMMAND(ID_MENU_SERVER_COPYURLTOCLIPBOARD, OnMenuServerCopyurltoclipboard)
	ON_UPDATE_COMMAND_UI(ID_MENU_SERVER_COPYURLTOCLIPBOARD, OnUpdateMenuServerCopyurltoclipboard)
	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnToolbarDropDown)
	ON_COMMAND(ID_MENU_VIEW_REMOTELISTVIEW_STATUSBAR, OnMenuViewRemotelistviewStatusbar)
	ON_COMMAND(ID_MENU_VIEW_LOCALLISTVIEW_STATUSBAR, OnMenuViewLocallistviewStatusbar)
	ON_UPDATE_COMMAND_UI(ID_MENU_VIEW_REMOTELISTVIEW_STATUSBAR, OnUpdateMenuViewRemotelistviewStatusbar)
	ON_UPDATE_COMMAND_UI(ID_MENU_VIEW_LOCALLISTVIEW_STATUSBAR, OnUpdateMenuViewLocallistviewStatusbar)
	ON_COMMAND(ID_VIEW_REMOTELISTVIEW_OWNERGROUP, OnViewRemotelistviewOwnergroup)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REMOTELISTVIEW_OWNERGROUP, OnUpdateViewRemotelistviewOwnergroup)
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_CANCELBUTTON, OnCancel)
	ON_WM_ACTIVATE()
	ON_COMMAND_RANGE(ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_FILENAME, ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_DESCENDING, OnMenuViewLocalSortby)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_FILENAME, ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_DESCENDING, OnUpdateMenuViewLocalSortby)
	ON_COMMAND_RANGE(ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_FILENAME, ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_DESCENDING, OnMenuViewRemoteSortby)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_FILENAME, ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_DESCENDING, OnUpdateMenuViewRemoteSortby)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_WM_ACTIVATEAPP()
	END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_SECURESERVER,
	ID_INDICATOR_ELAPSEDTIME,
	ID_INDICATOR_TIMELEFT,
	ID_INDICATOR_PROGRESS_PANE,// Statusleistenanzeige
	ID_INDICATOR_BYTES,
	ID_INDICATOR_QUEUESIZE,
	ID_INDICATOR_RECVLED,
	ID_INDICATOR_SENDLED
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Konstruktion/Zerstörung

CMainFrame::CMainFrame()
{
	nTrayNotificationMsg_ = RegisterWindowMessage( _T("FileZilla Tray Notification Message") );
	initialized = FALSE;
	m_nTransferType = COptions::GetOptionVal(OPTION_TRANSFERMODE);
	m_nLocalListViewStyle = m_nRemoteListViewStyle = 0;
	m_nHideLocalColumns = m_nHideRemoteColumns = 0;
	m_PosData.state = 0;
	m_PosData.cx = 432;
	m_pCommandQueue = 0;
	m_pFileZillaApi = 0;
	m_pSecureIconCtrl = new CStatic();
	m_bShowHiddenFiles = FALSE;
	m_nRecentQuickconnectCommandOffset = 0;

	m_pLocalViewHeader = NULL;
	m_pRemoteViewHeader = NULL;

	m_pWndReBar = NULL;
	m_pWndDlgBar = NULL;
	m_pWndToolBar = NULL;

	m_pMinTrayBtn = NULL;

	m_nLocalTreeViewLocation = COptions::GetOptionVal(OPTION_LOCALTREEVIEWLOCATION);
	m_nRemoteTreeViewLocation = COptions::GetOptionVal(OPTION_REMOTETREEVIEWLOCATION);

	m_bShowQuickconnect = TRUE;
	m_hLastFocus = NULL;

	m_pDragImage = NULL;
	m_pDragWnd = NULL;
	m_DropTarget = _T("");

	m_bMaxBeforeTray = false;

	m_bSwitchPaneLayout = FALSE;
}

CMainFrame::~CMainFrame()
{
	if (m_pCommandQueue)
		delete m_pCommandQueue;
	m_pCommandQueue=0;

	if (m_pFileZillaApi)
		delete m_pFileZillaApi;
	m_pFileZillaApi=0;

	delete m_pWndReBar;
	delete m_pWndToolBar;
	delete m_pWndDlgBar;

	delete m_pMinTrayBtn;

	delete m_pSecureIconCtrl;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetupTrayIcon();

	SetWindowText(GetVersionString());

	if (!CreateToolbars())
		return -1;

	//Create quickconnect button
	CWnd *pButton=m_pWndDlgBar->GetDlgItem(IDOK);
	CRect rect;
	pButton->GetWindowRect(rect);
	m_pWndDlgBar->ScreenToClient(rect);
	CString title;
	pButton->GetWindowText(title);
	m_wndQuickconnectButton.Create(title, WS_TABSTOP | WS_CHILD | WS_VISIBLE, rect, m_pWndDlgBar, IDOK);
	VERIFY(m_wndQuickconnectButton.AddMenuItem(ID_QUICKCONNECTBAR_MENU_BYPASS, 0));
	VERIFY(m_wndQuickconnectButton.AddMenuItem(ID_QUICKCONNECTBAR_MENU_CLEAR, 0));
	VERIFY(m_wndQuickconnectButton.AddMenuItem(ID_QUICKCONNECTBAR_MENU_EMPTYHISTORY, 0));

	//Load recent server list
	CMarkupSTL *pXML;
	if (COptions::LockXML(&pXML))
	{
		pXML->ResetPos();
		if (pXML->FindChildElem( _T("RecentServers") ))
		{
			pXML->IntoElem();
			t_server server;
			while (COptions::LoadServer(pXML, server))
			{
				CString str=server.host;
				CString tmp;
				if (server.user.CollateNoCase(_T("anonymous")))
					str=server.user + _T("@") + str;
				if (server.nServerType&FZ_SERVERTYPE_LAYER_SSL_IMPLICIT)
				{
					if (server.port!=21)
						tmp.Format(_T("%d"), server.port);
					str=_T("ftps://")+str;
				}
				else if (server.nServerType&FZ_SERVERTYPE_SUB_FTP_SFTP)
				{
					if (server.port!=22)
						tmp.Format(_T("%d"), server.port);
					str=_T("sftp://")+str;
				}
				else if (server.port!=21)
					tmp.Format(_T("%d"), server.port);

				if (tmp!="")
					str+=_T(":")+tmp;

				if (!m_RecentQuickconnectServers.size())
					m_wndQuickconnectButton.AddMenuItem(0, MF_SEPARATOR);
				m_RecentQuickconnectServers.push_front(server);
				m_wndQuickconnectButton.InsertMenuItem(4, ID_QUICKCONNECTBAR_MENU_HISTORY1+m_nRecentQuickconnectCommandOffset++, str, 0);
				m_nRecentQuickconnectCommandOffset%=10;
				if (m_RecentQuickconnectServers.size()>=10)
					break;
			}
			pXML->OutOfElem();
		}
		COptions::UnlockXML();
	}
	else
	{
		for (int i=10; i>0; i--)
		{
			CString name;
			name.Format(_T("Software\\FileZilla\\Recent Servers\\Server %d"), i);
			HKEY key;
			if (RegOpenKey(HKEY_CURRENT_USER, name ,&key)==ERROR_SUCCESS)
			{
				t_server server;
				if (COptions::LoadServer(key, server))
				{
					CString str=server.host;
					CString tmp;
					if (server.user.CollateNoCase(_T("anonymous")))
						str=server.user + _T("@") + str;
					if (server.nServerType&FZ_SERVERTYPE_LAYER_SSL_IMPLICIT)
					{
						if (server.port!=21)
							tmp.Format(_T("%d"), server.port);
						str=_T("ftps://")+str;
					}
					else if (server.nServerType&FZ_SERVERTYPE_SUB_FTP_SFTP)
					{
						if (server.port!=22)
							tmp.Format(_T("%d"), server.port);
						str=_T("sftp://")+str;
					}
					else if (server.port!=21)
						tmp.Format(_T("%d"), server.port);

					if (tmp!="")
						str+=_T(":")+tmp;

					if (!m_RecentQuickconnectServers.size())
						m_wndQuickconnectButton.AddMenuItem(0, MF_SEPARATOR);
					m_RecentQuickconnectServers.push_front(server);
					m_wndQuickconnectButton.InsertMenuItem(3, ID_QUICKCONNECTBAR_MENU_HISTORY1+m_nRecentQuickconnectCommandOffset++, str, 0);
					m_nRecentQuickconnectCommandOffset%=10;
					if (m_RecentQuickconnectServers.size()>=10)
					{
						RegCloseKey(key);
						break;
					}
				}
				RegCloseKey(key);
			}
		}
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Statusleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}

	RECT MyRect;
	m_wndStatusBar.GetItemRect(m_wndStatusBar.CommandToIndex(ID_INDICATOR_PROGRESS_PANE), &MyRect);

	//Create the progress control
	m_ProgressCtrl.Create(WS_VISIBLE|WS_CHILD, MyRect, &m_wndStatusBar, m_wndStatusBar.CommandToIndex(ID_INDICATOR_PROGRESS_PANE));
	m_ProgressCtrl.SetRange(0,100); //Set the range to between 0 and 100
	m_ProgressCtrl.SetStep(1); // Set the step amount
	m_ProgressCtrl.ShowWindow(SW_HIDE);

	m_wndStatusBar.GetItemRect(m_wndStatusBar.CommandToIndex(ID_INDICATOR_RECVLED), &MyRect);

	RECT rc;
	m_wndStatusBar.GetItemRect(m_wndStatusBar.CommandToIndex(ID_INDICATOR_PROGRESS_PANE), &rc);
	// Reposition the progress control correctly!
	m_ProgressCtrl.SetWindowPos(&wndTop, rc.left, rc.top, rc.right - rc.left,
		rc.bottom - rc.top, 0);
	m_ProgressCtrl.SetShowText(TRUE);

	//Create the first LED control
	m_RecvLed.Create(_T(""), WS_VISIBLE|WS_CHILD, MyRect, &m_wndStatusBar, m_wndStatusBar.CommandToIndex(ID_INDICATOR_RECVLED));
	m_RecvLed.SetLed( CLed::LED_COLOR_GREEN, CLed::LED_OFF, CLed::LED_ROUND);


	m_wndStatusBar.GetItemRect(m_wndStatusBar.CommandToIndex(ID_INDICATOR_RECVLED), &rc);
	// Reposition the first LED correctly!
	m_RecvLed.SetWindowPos(&wndTop, rc.left, rc.top, rc.right - rc.left,
		rc.bottom - rc.top, 0);

	//Create the second LED control
	m_SendLed.Create(_T(""), WS_VISIBLE|WS_CHILD, MyRect, &m_wndStatusBar, m_wndStatusBar.CommandToIndex(ID_INDICATOR_SENDLED));
	m_SendLed.SetLed( CLed::LED_COLOR_RED, CLed::LED_OFF, CLed::LED_ROUND);


	m_wndStatusBar.GetItemRect(m_wndStatusBar.CommandToIndex(ID_INDICATOR_SENDLED), &rc);
	// Reposition the second LED correctly!
	m_SendLed.SetWindowPos(&wndTop, rc.left, rc.top, rc.right - rc.left,
		rc.bottom - rc.top, 0);


	m_wndStatusBar.SetPaneInfo(0,m_wndStatusBar.GetItemID(0),SBPS_STRETCH|SBPS_NOBORDERS,200);
	m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(ID_INDICATOR_PROGRESS_PANE),ID_INDICATOR_PROGRESS_PANE,SBPS_NORMAL,100);

	m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(ID_INDICATOR_SECURESERVER),ID_INDICATOR_SECURESERVER,SBPS_NOBORDERS,20);
	m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(ID_INDICATOR_RECVLED),ID_INDICATOR_RECVLED,SBPS_NOBORDERS,6);
	m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(ID_INDICATOR_SENDLED),ID_INDICATOR_SENDLED,SBPS_NOBORDERS,1);

	// ZU ERLEDIGEN: Entfernen, wenn Sie keine QuickInfos wünschen
	m_pWndToolBar->SetBarStyle(m_pWndToolBar->GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
	m_pFileZillaApi=new CFileZillaApi;
	m_pFileZillaApi->Init(GetSafeHwnd());
	m_nFileZillaApiMessageID = m_pFileZillaApi->GetMessageID();
	m_pFileZillaApi->SetDebugLevel(COptions::GetOptionVal(OPTION_DEBUGTRACE)?4:0);
	m_bShowHiddenFiles=COptions::GetOptionVal(OPTION_ALWAYSSHOWHIDDEN);
	if (m_pFileZillaApi)
		m_pFileZillaApi->SetOption(FZAPI_OPTION_SHOWHIDDEN, m_bShowHiddenFiles);

	m_pCommandQueue=new CCommandQueue(m_pFileZillaApi);

	m_bShowTree=TRUE;
	m_bShowRemoteTree=TRUE;
	m_bShowQueue=1;
	m_bQuit=FALSE;

	m_hcArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hcNo = AfxGetApp()->LoadStandardCursor(IDC_NO);

	m_bShowMessageLog=TRUE;

	//Set initial size of queue pane
	int size=0,temp=0;
	m_wndVertSplitter.GetRowInfo(1,size,temp);
	m_wndVertSplitter.SetRowInfo(1,size-120,temp);
	m_wndVertSplitter.RecalcLayout();

	int s1,s2;
	if (!m_nLocalTreeViewLocation)
	{
		m_wndLocalSplitter.GetRowInfo(1,s2,temp);
		m_wndLocalSplitter.GetRowInfo(0,s1,temp);
		if (s1>s2) {
			m_wndLocalSplitter.SetRowInfo(0,(s1+s2)/2,1);
			m_wndLocalSplitter.RecalcLayout();
		}
	}
	else
	{
		m_wndLocalSplitter.GetColumnInfo(1,s2,temp);
		m_wndLocalSplitter.GetColumnInfo(0,s1,temp);
		if (s1 > s2) {
			m_wndLocalSplitter.SetColumnInfo(0,(s1+s2)/2,1);
			m_wndLocalSplitter.RecalcLayout();
		}
	}

	if (!m_nRemoteTreeViewLocation)
	{
		m_wndRemoteSplitter.GetRowInfo(1, s2, temp);
		m_wndRemoteSplitter.GetRowInfo(0, s1, temp);
		if (s1>s2) {
			m_wndRemoteSplitter.SetRowInfo(0,(s1+s2)/2,1);
			m_wndRemoteSplitter.RecalcLayout();
		}
	}
	else
	{
		m_wndRemoteSplitter.GetColumnInfo(1, s2, temp);
		m_wndRemoteSplitter.GetColumnInfo(0, s1, temp);
		if (s1>s2) {
			m_wndRemoteSplitter.SetColumnInfo(0,(s1+s2)/2,1);
			m_wndRemoteSplitter.RecalcLayout();
		}
	}

	GetQueuePane()->GetListCtrl()->LoadQueue();

	//Load the icon which indicates secure sites
	m_pSecureIconCtrl->Create(_T(""), WS_CHILD|SS_ICON,CRect(0,0,16,16),&m_wndStatusBar,m_wndStatusBar.CommandToIndex(ID_INDICATOR_SECURESERVER));
	m_pSecureIconCtrl->SetIcon( ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SECURE)));


	int cx,cx2;
	m_wndMainSplitter.GetColumnInfo(0,cx,temp);
	m_wndMainSplitter.GetColumnInfo(1,cx2,temp);
	m_wndMainSplitter.SetColumnInfo(0,(cx+cx2)/2,0);
	m_wndMainSplitter.RecalcLayout();
	((CQueueCtrl*)GetQueuePane()->GetListCtrl())->UpdateStatusbar();

	//Hide panes disabled in options
	if (COptions::GetOptionVal(OPTION_SHOWNOTOOLBAR))
		OnViewToolbar();
	if (COptions::GetOptionVal(OPTION_SHOWNOQUICKCONNECTBAR))
		OnViewQuickconnectBar();
	if (COptions::GetOptionVal(OPTION_SHOWNOSTATUSBAR))
		OnViewStatusBar();
	if (COptions::GetOptionVal(OPTION_SHOWNOMESSAGELOG))
		OnViewMessagelog();
	if (COptions::GetOptionVal(OPTION_SHOWNOTREEVIEW))
		OnShowtree();
	if (!COptions::GetOptionVal(OPTION_SHOWREMOTETREEVIEW))
		OnShowremotetree();
	if (COptions::GetOptionVal(OPTION_SHOWNOQUEUE))
		OnShowqueue();

	m_nSecTimerID=SetTimer(1, 1000, 0);

	CString folder=COptions::GetOption(OPTION_DEFAULTFOLDER);
	CFileStatus64 status;
	if (folder!="")
	{
		folder.TrimRight( _T("\\") );
		if (folder.Right(1)!=_T(":") )
			if (!GetStatus64(folder, status))
				folder="c:";
		folder+="\\";
	}
	SetLocalFolder(folder);

	if (COptions::GetOptionVal(OPTION_ENABLEDEBUGMENU))
	{
		CMenu *pMenu=GetMenu();
		if (pMenu->AppendMenu(MF_STRING|MF_POPUP, ID_MENU_DEBUG, _T("&Debug")))
		{
			MENUITEMINFO info={0};
			info.cbSize=sizeof(info);
			info.fMask |= MIIM_SUBMENU;
			if (GetMenuItemInfo(pMenu->GetSafeHmenu(), pMenu->GetMenuItemCount( )-1, TRUE, &info))
			{
				CMenu SubMenu;
				SubMenu.CreateMenu();
				info.hSubMenu=SubMenu.Detach();
				if (SetMenuItemInfo(pMenu->m_hMenu, pMenu->GetMenuItemCount( )-1, TRUE, &info))
				{
					pMenu=pMenu->GetSubMenu(pMenu->GetMenuItemCount( )-1);
					if (pMenu)
					{
						pMenu->AppendMenu(MF_STRING, ID_MENU_DEBUG_DUMPDIRCACHE, _T("&Dump Directory &Cache"));
						pMenu->AppendMenu(MF_STRING, ID_MENU_DEBUG_CRASH, _T("&Crash"));
					}
				}
			}
		}
	}

	CFileExistsDlg::SetOverwriteSettings(COptions::GetOptionVal(OPTION_FILEEXISTSACTION) - 1);

	//Initialize Minimize to tray button
	if (COptions::GetOptionVal(OPTION_MINIMIZETOTRAY) == 2)
	{
		if (m_pMinTrayBtn)
			delete m_pMinTrayBtn;
		m_pMinTrayBtn = new CMinTrayBtn(this);
		m_pMinTrayBtn->Hook(this);
		m_pMinTrayBtn->MinTrayBtnShow();
	}

	//Restore window size and position
	RestoreSize();

	if (COptions::GetOptionVal(OPTION_SHOWSITEMANAGERONSTARTUP))
	{
		// Show main window before opening Site Manager
		ShowWindow(SW_SHOW);
		OnSitemanager();
	}

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// Unterteiltes Fenster erstellen
	if (!m_wndVertSplitter.CreateStatic(this, 3, 1))
		return FALSE;


	if (!m_wndVertSplitter.CreateView(0, 0, RUNTIME_CLASS(CStatusView), CSize(100, 95), pContext))
	{
		m_wndVertSplitter.DestroyWindow();
		return FALSE;
	}

	// add the second splitter pane - which is a nested splitter with 2 rows
	if (!m_wndMainSplitter.CreateStatic(
		&m_wndVertSplitter,     // our parent window is the first splitter
		1, 2,               // the new splitter is 2 cols, 1 row
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		m_wndVertSplitter.IdFromRowCol(1, 0)
			// new splitter is in the 2nd row, 1st column of first splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}

	if (!m_wndVertSplitter.CreateView(2, 0, RUNTIME_CLASS(CQueueView), CSize(100, 95), pContext))
	{
		m_wndVertSplitter.DestroyWindow();
		return FALSE;
	}
	m_wndVertSplitter.SetRowInfo(1,4025,1);

	m_bSwitchPaneLayout = COptions::GetOptionVal(OPTION_PANELAYOUT_SWITCHLAYOUT);
	// add the second splitter pane - which is a nested splitter with 2 rows
	int numrows = 2 - m_nLocalTreeViewLocation;
	int numcols = 1 + m_nLocalTreeViewLocation;
	if (!m_wndLocalSplitter.CreateStatic(
		&m_wndMainSplitter,     // our parent window is the first splitter
		numrows, numcols,
		WS_CHILD | WS_VISIBLE | WS_BORDER ,  // style, WS_BORDER is needed
		m_wndMainSplitter.IdFromRowCol(0, m_bSwitchPaneLayout?1:0)
			// new splitter is in the 2nd row, 1st column of first splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}

	// add the second splitter pane - which is a nested splitter with 2 rows
	numrows = 2 - m_nRemoteTreeViewLocation;
	numcols = 1 + m_nRemoteTreeViewLocation;
	if (!m_wndRemoteSplitter.CreateStatic(
		&m_wndMainSplitter,     // our parent window is the first splitter
		numrows, numcols,
		WS_CHILD | WS_VISIBLE | WS_BORDER ,  // style, WS_BORDER is needed
		m_wndMainSplitter.IdFromRowCol(0, m_bSwitchPaneLayout?0:1)
			// new splitter is in the 2nd row, 1st column of first splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}
	m_wndMainSplitter.SetColumnInfo( 0, 432, 1);

	//Create the local view header
	m_pLocalViewHeader=new t_LocalViewHeader;
	memset(m_pLocalViewHeader, 0, sizeof(t_LocalViewHeader));
	m_pLocalViewHeader->m_pEdit = new CLocalComboCompletion;
	m_pLocalViewHeader->m_pEdit->Create(CBS_AUTOHSCROLL|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWN|CBS_SORT, CRect(0,0,100,200), this, 0);
	m_pLocalViewHeader->m_pEdit->InitStorage(10,1000);

	HFONT hFont = ( HFONT )GetStockObject(DEFAULT_GUI_FONT);
	CFont *font=CFont::FromHandle(hFont);
	m_pLocalViewHeader->m_pEdit->SetFont(font);

	m_pLocalViewHeader->m_pLabelBackground=new CStatic;
	m_pLocalViewHeader->m_pLabelBackground->Create(_T(""), WS_CHILD|WS_VISIBLE, CRect(1, 1, 0, 0), this);

	CString str;
	str.LoadString(IDS_VIEWLABEL_LOCAL);
	str="  " + str + " ";
	m_pLocalViewHeader->m_pLabel=new CStatic;
	m_pLocalViewHeader->m_pLabel->Create(str, WS_CHILD|WS_VISIBLE|SS_RIGHT, CRect(1, 1, 0, 0), this);
	m_pLocalViewHeader->m_pLabel->SetFont(font);
	m_pLocalViewHeader->m_LabelTextSize.cx=m_pLocalViewHeader->m_LabelTextSize.cy=-1;
	CDC *pDC=m_pLocalViewHeader->m_pLabel->GetDC();

	CFont *pOldFont=pDC->SelectObject(font);
	GetTextExtentPoint32( pDC->GetSafeHdc(), str, str.GetLength(), &m_pLocalViewHeader->m_LabelTextSize );
	pDC->SelectObject(pOldFont);

	m_pLocalViewHeader->m_pLabel->ReleaseDC(pDC);

	//Create the remote view header
	m_pRemoteViewHeader=new t_RemoteViewHeader;
	memset(m_pRemoteViewHeader, 0, sizeof(t_RemoteViewHeader));
	m_pRemoteViewHeader->m_pEdit = new CRemoteComboCompletion;
	m_pRemoteViewHeader->m_pEdit->Create(CBS_AUTOHSCROLL|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWN|CBS_SORT, CRect(0,0,100,200), this, 0);
	m_pRemoteViewHeader->m_pEdit->InitStorage(10,1000);
	m_pRemoteViewHeader->m_pEdit->EnableWindow(FALSE);

	hFont = ( HFONT )GetStockObject(DEFAULT_GUI_FONT);
	font=CFont::FromHandle(hFont);
	m_pRemoteViewHeader->m_pEdit->SetFont(font);

	m_pRemoteViewHeader->m_pLabelBackground=new CStatic;
	m_pRemoteViewHeader->m_pLabelBackground->Create(_T(""),WS_CHILD|WS_VISIBLE,CRect(1, 1, 0, 0), this);

	str.LoadString(IDS_VIEWLABEL_REMOTE);
	str="  " + str + " ";
	m_pRemoteViewHeader->m_pLabel=new CStatic;
	m_pRemoteViewHeader->m_pLabel->Create(str,WS_CHILD|WS_VISIBLE|SS_RIGHT,CRect(1, 1, 0, 0), this);
	m_pRemoteViewHeader->m_pLabel->SetFont(font);
	m_pRemoteViewHeader->m_LabelTextSize.cx=m_pRemoteViewHeader->m_LabelTextSize.cy=-1;
	pDC=m_pRemoteViewHeader->m_pLabel->GetDC();

	pOldFont=pDC->SelectObject(font);
	GetTextExtentPoint32( pDC->GetSafeHdc(), str, str.GetLength(), &m_pRemoteViewHeader->m_LabelTextSize );
	pDC->SelectObject(pOldFont);

	m_pRemoteViewHeader->m_pLabel->ReleaseDC(pDC);

	// now create the two views inside the nested splitter

	if (!m_wndLocalSplitter.CreateView(0, 0,
		RUNTIME_CLASS(CLocalView), CSize(300, 145), pContext))
	{
		TRACE0("Failed to create third pane\n");
		return FALSE;
	}

	int row = 1 - m_nLocalTreeViewLocation;
	int col = m_nLocalTreeViewLocation;
	if (!m_wndLocalSplitter.CreateView(row, col,
		RUNTIME_CLASS(CLocalView2), CSize(300, 200), pContext))
	{
		TRACE0("Failed to create third pane\n");
		return FALSE;
	}



	//Now create the ftp view

	if (!m_wndRemoteSplitter.CreateView(0, 0,
		RUNTIME_CLASS(CFtpTreeView), CSize(300, 145), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	row = 1 - m_nRemoteTreeViewLocation;
	col = m_nRemoteTreeViewLocation;
	if (!m_wndRemoteSplitter.CreateView(row, col,
		RUNTIME_CLASS(CFtpView), CSize(300, 200), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	GetLocalPane()->m_pOwner=this;
	GetLocalPane2()->m_pOwner=this;

	initialized=1;

	PostMessage(WM_FILEZILLA_PROCESSCMDLINE);

	m_nLocalListViewStyle=COptions::GetOptionVal(OPTION_LOCALLISTVIEWSTYLE);
	m_nRemoteListViewStyle=COptions::GetOptionVal(OPTION_REMOTELISTVIEWSTYLE);
	GetFtpPane()->m_pListCtrl->SetListStyle(m_nRemoteListViewStyle);
	GetLocalPane2()->m_pListCtrl->SetListStyle(m_nLocalListViewStyle);
	m_nHideLocalColumns=COptions::GetOptionVal(OPTION_HIDELOCALCOLUMNS);
	GetLocalPane2()->m_pListCtrl->UpdateColumns(m_nHideLocalColumns);
	m_nHideRemoteColumns=COptions::GetOptionVal(OPTION_HIDEREMOTECOLUMNS);
	GetFtpPane()->m_pListCtrl->UpdateColumns(m_nHideRemoteColumns);

	return TRUE;
}

//////////////////
// Helper function to register a new window class based on an already
// existing window class, but with a different name and icon.
// Returns new name if successful; otherwise NULL.
//
static LPCTSTR RegisterSimilarClass(LPCTSTR lpszNewClassName,
	LPCTSTR lpszOldClassName, UINT nIDResource)
{
	// Get class info for old class.
	//
	HINSTANCE hInst = AfxGetInstanceHandle();
	WNDCLASS wc;
	if (!::GetClassInfo(hInst, lpszOldClassName, &wc)) {
		TRACE(_T("Can't find window class %s\n"), lpszOldClassName);
		return NULL;
	}

	// Register new class with same info, but different name and icon.
	//
	wc.lpszClassName = lpszNewClassName;
	wc.hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(nIDResource));
	if (!AfxRegisterClass(&wc)) {
		TRACE(_T("Unable to register window class%s\n"), lpszNewClassName);
		return NULL;
	}
	return lpszNewClassName;
}

// Static class member holds window class name
// (NULL if not registered yet).
//
LPCTSTR CMainFrame::s_winClassName = NULL;

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.lpszClass = AfxRegisterWndClass(0);

	//Change the window class name
	if (s_winClassName==NULL)
	{
		// One-time initialization: register the class
		//
		s_winClassName = RegisterSimilarClass(_T("FileZilla Main Window"),
						cs.lpszClass, IDR_MAINFRAME);
		if (!s_winClassName)
			return FALSE;
	}
	cs.lpszClass = s_winClassName;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Diagnose

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
	dc << "FileZilla Main Window";
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Nachrichten-Handler

void CMainFrame::OnQuickConnect()
{
	ProcessQuickconnect(FALSE);
}

CStatusView* CMainFrame::GetStatusPane()
{
	CWnd* pWnd = m_wndVertSplitter.GetPane(0, 0);
	CStatusView* pView = DYNAMIC_DOWNCAST(CStatusView, pWnd);
	return pView;
}

CQueueView* CMainFrame::GetQueuePane()
{
	static CQueueView *sView=0;
	if (sView)
		return sView;
	CWnd* pWnd = m_wndVertSplitter.GetPane(2, 0);
	CQueueView* pView = DYNAMIC_DOWNCAST(CQueueView, pWnd);
	sView=pView;
	return pView;
}

CLocalView *CMainFrame::GetLocalPane()
{
	static CLocalView *sView=0;
	if (sView)
		return sView;
	CWnd* pWnd = m_wndLocalSplitter.GetPane(0, 0);
	CLocalView* pView = DYNAMIC_DOWNCAST(CLocalView, pWnd);
	sView=pView;
	return pView;
}

CLocalView2 *CMainFrame::GetLocalPane2()
{
	static CLocalView2 *sView=0;
	if (sView)
		return sView;
	CWnd* pWnd;
	if (m_nLocalTreeViewLocation)
		pWnd = m_wndLocalSplitter.GetPane(0, 1);
	else
		pWnd = m_wndLocalSplitter.GetPane(1, 0);
	CLocalView2* pView = DYNAMIC_DOWNCAST(CLocalView2, pWnd);
	sView=pView;
	return pView;
}

CFtpView *CMainFrame::GetFtpPane()
{
	CWnd* pWnd;
	if (m_nRemoteTreeViewLocation)
		pWnd = m_wndRemoteSplitter.GetPane(0, 1);
	else
		pWnd = m_wndRemoteSplitter.GetPane(1, 0);
	CFtpView* pView = DYNAMIC_DOWNCAST(CFtpView, pWnd);
	return pView;
}

CFtpTreeView *CMainFrame::GetFtpTreePane()
{
	CWnd* pWnd = m_wndRemoteSplitter.GetPane(0, 0);
	CFtpTreeView* pView = DYNAMIC_DOWNCAST(CFtpTreeView, pWnd);
	return pView;
}

void CMainFrame::SetLocalFolder(CString folder)
{
	folder.TrimRight( _T("\\") );
	CString folder2=folder;
	folder+=_T("\\");
	if ( folder2!="" && folder2.Right(1)!=_T(":") )
	{
		CFileStatus64 status;
		if (!GetStatus64(folder2, status) || !(status.m_attribute&0x10))
		{
			m_pLocalViewHeader->m_pEdit->SetWindowText(GetLocalPane2()->GetLocalFolder());
			if (m_pLocalViewHeader->m_pEdit->FindStringExact(-1, GetLocalPane2()->GetLocalFolder())==CB_ERR)
			{
				m_pLocalViewHeader->m_pEdit->AddString(GetLocalPane2()->GetLocalFolder());
			}
			return;
		}
	}
	if (GetLocalPane2()->GetLocalFolder()!=folder)
	{
		GetLocalPane2()->SetLocalFolder(folder);
	}
	if (m_bShowTree)
		if (GetLocalPane()->GetLocalFolder()!=folder)
		{
			GetLocalPane()->SetLocalFolder(folder);
		}
	m_pLocalViewHeader->m_pEdit->SetWindowText(folder);
	if (m_pLocalViewHeader->m_pEdit->FindStringExact(-1, GetLocalPane2()->GetLocalFolder())==CB_ERR)
	{
		m_pLocalViewHeader->m_pEdit->AddString(GetLocalPane2()->GetLocalFolder());
	}
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	if (m_wndStatusBar.GetSafeHwnd())
	{
		if (nType!=SIZE_MAXIMIZED)
			m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(ID_INDICATOR_SENDLED),ID_INDICATOR_SENDLED,SBPS_NOBORDERS,1);
		else
			m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(ID_INDICATOR_SENDLED),ID_INDICATOR_SENDLED,SBPS_NOBORDERS,10);
	}

	if (initialized)
	{
		//Hide the queue if visible
		m_wndVertSplitter.SetRedraw(FALSE);
		if (m_bShowQueue)
			m_wndVertSplitter.HideRow(2,1);
	}
	
	//Now only the main splitter gets resized
	CFrameWnd::OnSize(nType, cx, cy);
	if (initialized)
	{
		//Restore the queue
		if (m_bShowQueue)
		{
			if (nType != SIZE_MINIMIZED)
				RecalcLayout();
			m_wndVertSplitter.ShowRow(2);
		}
		m_wndVertSplitter.SetRedraw(TRUE);
		m_wndVertSplitter.RedrawWindow();
	}
	if (m_wndStatusBar.GetSafeHwnd())
	{
		RECT rc;
		m_wndStatusBar.GetItemRect(m_wndStatusBar.CommandToIndex(ID_INDICATOR_PROGRESS_PANE), &rc);

		// Reposition the progress control correctly!
		m_ProgressCtrl.SetWindowPos(&wndTop, rc.left, rc.top, rc.right - rc.left,
			rc.bottom - rc.top, 0);

		m_wndStatusBar.GetItemRect(m_wndStatusBar.CommandToIndex(ID_INDICATOR_RECVLED), &rc);

		// Reposition the first LED correctly!
		m_RecvLed.SetWindowPos(&wndTop, rc.left, rc.top+1, rc.right - rc.left,
			rc.bottom - rc.top, 0);

		m_wndStatusBar.GetItemRect(m_wndStatusBar.CommandToIndex(ID_INDICATOR_SENDLED), &rc);

		// Reposition the second LED correctly!
		m_SendLed.SetWindowPos(&wndTop, rc.left, rc.top+1, rc.right - rc.left+15,
				rc.bottom - rc.top, SWP_SHOWWINDOW);

		if (m_pSecureIconCtrl)
		{
			m_wndStatusBar.GetItemRect(m_wndStatusBar.CommandToIndex(ID_INDICATOR_SECURESERVER), &rc);
			rc.left=rc.right-16;

			m_pSecureIconCtrl->MoveWindow(&rc);
		}
	}

}

void CMainFrame::SetStatusbarText(int nIndex,CString str)
{
	m_wndStatusBar.GetStatusBarCtrl().SetText(str,nIndex,0);
	HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
	if (str=="")
		str.LoadString(m_wndStatusBar.GetItemID(nIndex));
	CClientDC dcScreen(NULL);
	HGDIOBJ hOldFont = NULL;
	if (hFont != NULL)
	hOldFont = dcScreen.SelectObject(hFont);
	int cx=dcScreen.GetTextExtent(str).cx;
	int cxold;
	unsigned int nID,nStyle;
	m_wndStatusBar.GetPaneInfo(nIndex,nID,nStyle,cxold);
	if (cx!=cxold)
	{
		//m_wndStatusBar.SetPaneInfo(nIndex,nID,nStyle,cx);
		RECT rc;
		m_wndStatusBar.GetItemRect(m_wndStatusBar.CommandToIndex(ID_INDICATOR_PROGRESS_PANE), &rc);

		// Reposition the progress control correctly!
		m_ProgressCtrl.SetWindowPos(&wndTop, rc.left, rc.top, rc.right - rc.left,
			rc.bottom - rc.top, 0);
	}
	if (hOldFont != NULL)
		dcScreen.SelectObject(hOldFont);

}

void CMainFrame::SetProgress(t_ffam_transferstatus *status)
{
	if (!m_wndStatusBar.GetSafeHwnd())
		return;
	if (!status)
	{
		SetStatusbarText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_ELAPSEDTIME),"");
		SetStatusbarText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_TIMELEFT),"");
		SetStatusbarText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_BYTES),"");
		m_ProgressCtrl.ShowWindow(SW_HIDE);
	}
	else
	{
		CString str,str2;
		str2.Format(_T("%d:%02d:%02d"), status->timeelapsed/3600, (status->timeelapsed/60)%60, status->timeelapsed%60);
		str.Format(IDS_STATUSBAR_ELAPSED,str2);
		SetStatusbarText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_ELAPSEDTIME),str);

		if (status->timeleft!=-1)
		{
			str2.Format(_T("%d:%02d:%02d"), status->timeleft/3600, (status->timeleft/60)%60, status->timeleft%60);
			str.Format(IDS_STATUSBAR_LEFT,str2);
			SetStatusbarText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_TIMELEFT),str);
		}
		else
			SetStatusbarText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_TIMELEFT),"");

		CString tmp=_T("? ");
		if (status->transferrate)
		{
			if (status->transferrate>(1000*1000))
			{
				tmp.Format(_T("%d.%d M"),status->transferrate/1000/1000,(status->transferrate/1000/100)%10);
			}
			else if (status->transferrate>1000)
			{
				tmp.Format(_T("%d.%d K"),status->transferrate/1000,(status->transferrate/100)%10);
			}
			else
			{
				tmp.Format(_T("%d "),status->transferrate);
			}
		}
		str.Format(_T("%I64d bytes (%sB/s)"), status->bytes,tmp);
		SetStatusbarText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_BYTES), str);

		if (status->percent!=-1)
		{
			m_ProgressCtrl.ShowWindow(SW_SHOW);
			m_ProgressCtrl.SetPos(status->percent);
		}
		else
			m_ProgressCtrl.ShowWindow(SW_HIDE);
	}
}

void CMainFrame::AddQueueItem(BOOL get, CString filename, CString subdir, CString localdir, const CServerPath &remotepath, BOOL stdtransfer, CString user /*=""*/, CString pass /*=""*/, int nOpen /*0*/)
{
	t_transferfile transferfile;
	transferfile.remotefile = filename;
	if (!m_nTransferType)
	{
		transferfile.nType = 2;
		CString AsciiFiles = COptions::GetOption(OPTION_ASCIIFILES);
		CString fn = filename;
		fn.MakeUpper();
		int i;
		if (fn.Find( _T(".") )==-1)
			while ((i=AsciiFiles.Find( _T(";") ))!=-1)
			{
				if (AsciiFiles.Left(i)==_T("."))
				{
					transferfile.nType = 1;
					break;
				}
				AsciiFiles = AsciiFiles.Mid(i+1);
			}
		else
			while ((i=AsciiFiles.Find( _T(";") ))!=-1)
			{
				if ("."+AsciiFiles.Left(i)==fn.Right(AsciiFiles.Left(i).GetLength()+1))
				{
					transferfile.nType = 1;
					break;
				}
				AsciiFiles = AsciiFiles.Mid(i+1);
			}
	}
	else
		transferfile.nType = m_nTransferType;

	localdir.TrimRight( _T("\\") );
	if (!nOpen)
	{
		if (localdir == _T(""))
		{
			if (m_DropTarget == "")
				transferfile.localfile = GetLocalPane2()->GetLocalFolder();
			else
				transferfile.localfile = m_DropTarget;
			transferfile.localfile = PathAppend(transferfile.localfile, subdir);
			transferfile.localfile = PathAppend(transferfile.localfile, filename);
		}
		else
			transferfile.localfile = localdir+_T("\\")+filename;
	}
	else
	{
		if (!get)
			ASSERT(nOpen==3);
		else
		{
			ASSERT(nOpen==1 || nOpen==2);
			TCHAR temppath[MAX_PATH];
			GetTempPath(MAX_PATH, temppath);
			transferfile.localfile=temppath;
			if (transferfile.localfile.Right(1)!=_T("\\"))
				transferfile.localfile+=_T("\\");
			transferfile.localfile+=transferfile.remotefile;
		}
	}

	if (!remotepath.IsEmpty())
		transferfile.remotepath = remotepath;
	else
	{
		if (m_DropTarget != "")
			transferfile.remotepath.SetSafePath(m_DropTarget);
		else
			transferfile.remotepath = ((CFtpListCtrl *)GetFtpPane()->GetListCtrl())->GetCurrentDirectory();
		transferfile.remotepath.AddSubdirs(subdir);
	}

	transferfile.get = get;
	if (GetFtpPane()->GetTransferfile(transferfile))
	{
		if (!get)
		{
			CFileStatus64 status;
			BOOL res = GetStatus64(transferfile.localfile, status);

			if (!GetLength64(transferfile.localfile, transferfile.size))
				return;
		}
		else
		{
			int pos = transferfile.localfile.ReverseFind('\\');
			if (pos == -1)
			{
				transferfile.localfile.Replace('/', '_');
				transferfile.localfile.Replace(':', '_');
				transferfile.localfile.Replace('*', '_');
				transferfile.localfile.Replace('?', '_');
				transferfile.localfile.Replace('<', '_');
				transferfile.localfile.Replace('>', '_');
				transferfile.localfile.Replace('|', '_');
			}
			else
			{
				CString file = transferfile.localfile.Mid(pos + 1);
				file.Replace('/', '_');
				file.Replace(':', '_');
				file.Replace('*', '_');
				file.Replace('?', '_');
				file.Replace('<', '_');
				file.Replace('>', '_');
				file.Replace('|', '_');
				transferfile.localfile = transferfile.localfile.Left(pos + 1) + file;
			}
		}
		if (user!="")
		{
			transferfile.server.user=user;
			transferfile.server.pass=pass;
		}
		GetQueuePane()->AddItem(transferfile, stdtransfer, nOpen);
	}
}

void CMainFrame::OnUpdateShowtree(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowTree);
}

void CMainFrame::OnShowtree()
{
	if (!m_bShowTree)
	{
		GetLocalPane()->SetLocalFolder(GetLocalPane2()->GetLocalFolder());
		if (m_nLocalTreeViewLocation)
			m_wndLocalSplitter.ShowCol(0);
		else
			m_wndLocalSplitter.ShowRow(0);

		m_bShowTree=TRUE;
		m_pLocalViewHeader->bTreeHidden=FALSE;
	}
	else
	{
		m_bShowTree=FALSE;
		if (m_nLocalTreeViewLocation)
			m_wndLocalSplitter.HideCol(0);
		else
			m_wndLocalSplitter.HideRow(0);
		m_pLocalViewHeader->bTreeHidden=TRUE;
	}
	GetLocalPane()->UpdateViewHeader();
	GetLocalPane2()->UpdateViewHeader();
}

void CMainFrame::OnEditSettings()
{
	if (COptions::GetOptionVal(OPTION_REMEMBERVIEWS))
	{
		COptions::SetOption(OPTION_SHOWNOTOOLBAR, !m_pWndToolBar->IsVisible());
		COptions::SetOption(OPTION_SHOWNOQUICKCONNECTBAR, !m_pWndDlgBar->IsVisible());
		COptions::SetOption(OPTION_SHOWNOSTATUSBAR, !m_wndStatusBar.IsVisible());
		COptions::SetOption(OPTION_SHOWNOMESSAGELOG, !m_bShowMessageLog);
		COptions::SetOption(OPTION_SHOWNOTREEVIEW, !m_bShowTree);
		COptions::SetOption(OPTION_SHOWREMOTETREEVIEW, m_bShowRemoteTree);
		COptions::SetOption(OPTION_SHOWNOQUEUE, !m_bShowQueue);
	}
	if (COptions::GetOptionVal(OPTION_REMEMBERLOCALVIEW))
	{
		COptions::SetOption(OPTION_LOCALLISTVIEWSTYLE, m_nLocalListViewStyle);
		COptions::SetOption(OPTION_HIDELOCALCOLUMNS, m_nHideLocalColumns);
		COptions::SetOption(OPTION_SHOWLOCALSTATUSBAR, GetLocalPane2()->IsStatusbarEnabled()?1:0);
	}
	if (COptions::GetOptionVal(OPTION_REMEMBERREMOTEVIEW))
	{
		COptions::SetOption(OPTION_REMOTELISTVIEWSTYLE, m_nRemoteListViewStyle);
		COptions::SetOption(OPTION_HIDEREMOTECOLUMNS, m_nHideRemoteColumns);
		COptions::SetOption(OPTION_SHOWREMOTESTATUSBAR, GetFtpPane()->IsStatusbarEnabled()?1:0);
	}
	BOOL bOldShowHiddenFiles=COptions::GetOptionVal(OPTION_ALWAYSSHOWHIDDEN);

	int nOldTransferType = COptions::GetOptionVal(OPTION_TRANSFERMODE);

	//Remember contents of quickconnect bar
	CString host, user, pass, port;
	m_pWndDlgBar->GetDlgItemText(IDC_HOST, host);
	m_pWndDlgBar->GetDlgItemText(IDC_USER, user);
	m_pWndDlgBar->GetDlgItemText(IDC_PASS, pass);
	m_pWndDlgBar->GetDlgItemText(IDC_PORT, port);

	int nFileExistsAction = COptions::GetOptionVal(OPTION_FILEEXISTSACTION);

	COptions dlg(this);
	if (!dlg.Show())
		return;

	if (nOldTransferType != COptions::GetOptionVal(OPTION_TRANSFERMODE))
		m_nTransferType = COptions::GetOptionVal(OPTION_TRANSFERMODE);

	//Set the list view styles
	m_nLocalListViewStyle=COptions::GetOptionVal(OPTION_LOCALLISTVIEWSTYLE);
	m_nRemoteListViewStyle=COptions::GetOptionVal(OPTION_REMOTELISTVIEWSTYLE);
	GetFtpPane()->m_pListCtrl->SetListStyle(m_nRemoteListViewStyle);
	GetLocalPane2()->m_pListCtrl->SetListStyle(m_nLocalListViewStyle);
	GetLocalPane2()->EnableStatusbar(COptions::GetOptionVal(OPTION_SHOWLOCALSTATUSBAR));
	//Show the right columns
	m_nHideLocalColumns=COptions::GetOptionVal(OPTION_HIDELOCALCOLUMNS);
	GetLocalPane2()->m_pListCtrl->UpdateColumns(m_nHideLocalColumns);
	m_nHideRemoteColumns=COptions::GetOptionVal(OPTION_HIDEREMOTECOLUMNS);
	GetFtpPane()->m_pListCtrl->UpdateColumns(m_nHideRemoteColumns);
	GetFtpPane()->EnableStatusbar(COptions::GetOptionVal(OPTION_SHOWREMOTESTATUSBAR));


	if (m_bShowTree)
		GetLocalPane()->UpdateViewHeader();
	GetLocalPane2()->UpdateViewHeader();

	if (m_bShowRemoteTree)
		GetFtpTreePane()->UpdateViewHeader();
	GetFtpPane()->UpdateViewHeader();

	CMenu *menu2=GetMenu();
	CMenu *pMenu=new CMenu;
	if (menu2->m_hMenu!=m_hMenuDefault)
	{
		menu2->DestroyMenu();
		delete menu2;
	}
	pMenu->LoadMenu(IDR_MAINFRAME);

	//Add/remove debug menu
	if (COptions::GetOptionVal(OPTION_ENABLEDEBUGMENU))
	{
		MENUITEMINFO info={0};
		info.cbSize=sizeof(info);
		info.fMask |= MIIM_SUBMENU;
		if (!pMenu->GetMenuItemInfo(ID_MENU_DEBUG, &info))
			if (pMenu->AppendMenu(MF_STRING|MF_POPUP, ID_MENU_DEBUG, _T("&Debug")))
			{
				MENUITEMINFO info={0};
				info.cbSize=sizeof(info);
				info.fMask|=MIIM_SUBMENU;
				if (pMenu->GetMenuItemInfo(ID_MENU_DEBUG, &info))
				{
					CMenu SubMenu;
					SubMenu.CreateMenu();
					info.hSubMenu=SubMenu.Detach();
					if (SetMenuItemInfo(pMenu->m_hMenu, ID_MENU_DEBUG, FALSE, &info))
					{
						CMenu *pSubMenu=pMenu->GetSubMenu(pMenu->GetMenuItemCount()-1);
						if (pSubMenu)
						{
							pSubMenu->AppendMenu(MF_STRING, ID_MENU_DEBUG_DUMPDIRCACHE, _T("&Dump Directory &Cache"));
							pSubMenu->AppendMenu(MF_STRING, ID_MENU_DEBUG_CRASH, _T("&Crash"));
						}
					}
				}
			}
	}
	else
	{
		MENUITEMINFO info={0};
		info.cbSize=sizeof(info);
		info.fMask|=MIIM_SUBMENU;
		if (pMenu->GetMenuItemInfo(ID_MENU_DEBUG, &info))
		{
			DestroyMenu(info.hSubMenu);
			pMenu->RemoveMenu(ID_MENU_DEBUG, MF_BYCOMMAND);
		}
	}

	SetMenu(pMenu);

	CString str;

	m_wndQuickconnectButton.ShowWindow(SW_HIDE);
	m_wndQuickconnectButton.SetParent(this);
	delete m_pWndReBar;
	delete m_pWndDlgBar;
	delete m_pWndToolBar;
	VERIFY(CreateToolbars());
	m_wndQuickconnectButton.SetParent(m_pWndDlgBar);

	m_wndQuickconnectButton.SetDlgCtrlID(IDCANCEL);
	CString title = "Quick&connect";

	CWnd *pDlgItem = m_pWndDlgBar->GetDlgItem(IDOK);
	if (pDlgItem)
		pDlgItem->GetWindowText(title);
	m_wndQuickconnectButton.SetWindowText(title);
	CRect rect(0, 0, 20, 20);
	pDlgItem = m_pWndDlgBar->GetDlgItem(IDOK);
	if (pDlgItem)
		pDlgItem->GetWindowRect(&rect);
	m_pWndDlgBar->ScreenToClient(rect);
	m_wndQuickconnectButton.MoveWindow(rect);
	m_wndQuickconnectButton.SetDlgCtrlID(IDOK);
	m_wndQuickconnectButton.ShowWindow(SW_SHOW);

	VERIFY(m_wndQuickconnectButton.RemoveMenuItem(2));
	VERIFY(m_wndQuickconnectButton.RemoveMenuItem(1));
	VERIFY(m_wndQuickconnectButton.RemoveMenuItem(0));
	VERIFY(m_wndQuickconnectButton.InsertMenuItem(0, ID_QUICKCONNECTBAR_MENU_BYPASS, 0));
	VERIFY(m_wndQuickconnectButton.InsertMenuItem(1, ID_QUICKCONNECTBAR_MENU_CLEAR, 0));
	VERIFY(m_wndQuickconnectButton.InsertMenuItem(2, ID_QUICKCONNECTBAR_MENU_EMPTYHISTORY, 0));

	m_pWndDlgBar->SetDlgItemText(IDC_HOST, host);
	m_pWndDlgBar->SetDlgItemText(IDC_USER, user);
	m_pWndDlgBar->SetDlgItemText(IDC_PASS, pass);
	m_pWndDlgBar->SetDlgItemText(IDC_PORT, port);


	//Reload the headers of the list views
	((CLocalFileListCtrl *)GetLocalPane2()->GetListCtrl())->ReloadHeader();
	((CFtpListCtrl *)GetFtpPane()->GetListCtrl())->ReloadHeader();
	((CQueueCtrl *)GetQueuePane()->GetListCtrl())->ReloadHeader();

	//Show/Hide panes
	//Hide panes disabled in options
	if (COptions::GetOptionVal(OPTION_SHOWNOTOOLBAR)==m_pWndToolBar->IsVisible())
		OnViewToolbar();
	if (COptions::GetOptionVal(OPTION_SHOWNOQUICKCONNECTBAR)==m_bShowQuickconnect)
		OnViewQuickconnectBar();
	if (COptions::GetOptionVal(OPTION_SHOWNOSTATUSBAR)==m_wndStatusBar.IsVisible())
		OnViewStatusBar();
	if (COptions::GetOptionVal(OPTION_SHOWNOMESSAGELOG)==m_bShowMessageLog)
		OnViewMessagelog();
	if (COptions::GetOptionVal(OPTION_SHOWNOTREEVIEW)==m_bShowTree)
		OnShowtree();
	if (COptions::GetOptionVal(OPTION_SHOWREMOTETREEVIEW)!=m_bShowRemoteTree)
		OnShowremotetree();
	if (COptions::GetOptionVal(OPTION_SHOWNOQUEUE)==m_bShowQueue)
		OnShowqueue();

	//Check if "Always show hidden files" has changed
	if (bOldShowHiddenFiles!=COptions::GetOptionVal(OPTION_ALWAYSSHOWHIDDEN))
	m_bShowHiddenFiles=COptions::GetOptionVal(OPTION_ALWAYSSHOWHIDDEN);
	if (m_pFileZillaApi)
	{
		m_pFileZillaApi->SetOption(FZAPI_OPTION_SHOWHIDDEN, m_bShowHiddenFiles);
		m_pFileZillaApi->SetDebugLevel(COptions::GetOptionVal(OPTION_DEBUGTRACE)?4:0);
	}

	if (COptions::GetOptionVal(OPTION_TRANSFERUSEMULTIPLE)!=((CQueueCtrl *)GetQueuePane()->GetListCtrl())->DoUseMultiple())
		((CQueueCtrl *)GetQueuePane()->GetListCtrl())->ToggleUseMultiple();

	GetQueuePane()->GetListCtrl()->SetApiCount(COptions::GetOptionVal(OPTION_TRANSFERAPICOUNT));

	if (COptions::GetOptionVal(OPTION_MINIMIZETOTRAY) == 2)
	{
		if (!m_pMinTrayBtn)
		{
			m_pMinTrayBtn = new CMinTrayBtn(this);
			m_pMinTrayBtn->Hook(this);
			m_pMinTrayBtn->MinTrayBtnShow();
		}
	}
	else
	{
		if (m_pMinTrayBtn)
		{
			m_pMinTrayBtn->Unhook();
			delete m_pMinTrayBtn;
			m_pMinTrayBtn = NULL;
		}
	}
	RedrawWindow(0, 0, RDW_FRAME | RDW_UPDATENOW | RDW_INVALIDATE);

	// Change the help file path
	TCHAR buffer[MAX_PATH + 1000];
	GetModuleFileName( 0, buffer, MAX_PATH );
	CString helpFile=buffer;
	PathRemoveArgs(helpFile);
	PathUnquoteSpaces(helpFile);
	PathRemoveFileSpec(helpFile);

	CFileStatus64 status;
	if (COptions::GetOption(OPTION_LANGUAGE) == _T("Français") && GetStatus64(helpFile + _T("FileZillaFrench.chm"), status) && !(status.m_attribute&FILE_ATTRIBUTE_DIRECTORY))
		helpFile += _T("FileZillaFrench.chm");
	else
		helpFile += _T("FileZilla.chm");
	free((void*)AfxGetApp()->m_pszHelpFilePath);
	AfxGetApp()->m_pszHelpFilePath = _tcsdup(helpFile);

	GetStatusPane()->UpdateLogFile();

	if (COptions::GetOptionVal(OPTION_FILEEXISTSACTION) != nFileExistsAction)
		CFileExistsDlg::SetOverwriteSettings(COptions::GetOptionVal(OPTION_FILEEXISTSACTION) - 1);
}

void CMainFrame::OnSitemanager()
{
	CSiteManager dlg;
	int res = dlg.DoModal();
	if (res == IDOK)
		ParseSite(dlg.m_LastSite);
}

void CMainFrame::OnCancel()
{
	int res=AfxMessageBox(IDS_QUESTION_CANCELOPERATION,MB_YESNO|MB_ICONQUESTION);
	if (res==IDYES)
		m_pCommandQueue->Cancel();
}

void CMainFrame::OnUpdateCancelbutton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pCommandQueue->IsBusy());
}

void CMainFrame::OnShowqueue()
{
	if (!m_bShowQueue)
	{
		m_bShowQueue=TRUE;
		m_wndVertSplitter.ShowRow(2);
	}
	else
	{
		GetQueuePane();
		m_bShowQueue=FALSE;
		m_wndVertSplitter.HideRow(2,1);
	}

}

void CMainFrame::OnUpdateShowqueue(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowQueue);
}

BOOL CALLBACK FzEnumThreadWndProc(HWND hwnd, LPARAM lParam)
{
	TCHAR buffer[100];
	GetClassName(hwnd, buffer, 99);
	if (!_tcscmp(buffer, _T("#32770")))
	{
		*((bool*)lParam) = true;
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		return FALSE;
	}

	return TRUE;
}

void CMainFrame::OnClose()
{
	bool dialog = false;
	EnumThreadWindows(GetCurrentThreadId(), FzEnumThreadWndProc, (LPARAM)&dialog);
	if (dialog)
	{
		::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		return;
	}

	ShowWindow(SW_HIDE);
	if (!m_bQuit)
	{
		if (!GetQueuePane()->GetListCtrl()->DoClose())
			return;

		m_bQuit=TRUE;

		m_pFileZillaApi->Destroy();
		return;
	}


	if (COptions::GetOptionVal(OPTION_DEFAULTFOLDERTYPE)==0)
	{
		CString folder=GetLocalPane2()->GetLocalFolder();
		COptions::SetOption(OPTION_DEFAULTFOLDER,folder);
	}
	if (COptions::GetOptionVal(OPTION_REMEMBERVIEWS))
	{
		COptions::SetOption(OPTION_SHOWNOTOOLBAR,!m_pWndToolBar->IsVisible());
		COptions::SetOption(OPTION_SHOWNOQUICKCONNECTBAR,!m_pWndDlgBar->IsVisible());
		COptions::SetOption(OPTION_SHOWNOSTATUSBAR,!m_wndStatusBar.IsVisible());
		COptions::SetOption(OPTION_SHOWNOMESSAGELOG,!m_bShowMessageLog);
		COptions::SetOption(OPTION_SHOWNOTREEVIEW,!m_bShowTree);
		COptions::SetOption(OPTION_SHOWREMOTETREEVIEW, m_bShowRemoteTree);
		COptions::SetOption(OPTION_SHOWNOQUEUE,!m_bShowQueue);
	}
	if (COptions::GetOptionVal(OPTION_REMEMBERLOCALVIEW))
	{
		COptions::SetOption(OPTION_LOCALLISTVIEWSTYLE, m_nLocalListViewStyle);
		COptions::SetOption(OPTION_HIDELOCALCOLUMNS, m_nHideLocalColumns);
		COptions::SetOption(OPTION_SHOWLOCALSTATUSBAR, GetLocalPane2()->IsStatusbarEnabled()?1:0);
	}
	if (COptions::GetOptionVal(OPTION_REMEMBERREMOTEVIEW))
	{
		COptions::SetOption(OPTION_REMOTELISTVIEWSTYLE,m_nRemoteListViewStyle);
		COptions::SetOption(OPTION_HIDEREMOTECOLUMNS, m_nHideRemoteColumns);
		COptions::SetOption(OPTION_SHOWREMOTESTATUSBAR, GetFtpPane()->IsStatusbarEnabled()?1:0);
	}
	if (COptions::GetOptionVal(OPTION_REMEMBERLASTWINDOWPOS))
	{
		WINDOWPLACEMENT placement;
		BOOL res = GetWindowPlacement(&placement);
		CString str;
		int state = 0;
		if (placement.showCmd == SW_MAXIMIZE || placement.showCmd == SW_SHOWMAXIMIZED)
			state = 1;
		if (!m_PosData.state)
			str.Format(_T("%d %d %d %d %d"), state, placement.rcNormalPosition.left, placement.rcNormalPosition.top, placement.rcNormalPosition.right, placement.rcNormalPosition.bottom);
		else
			str.Format(_T("%d %d %d %d %d"), state, m_PosData.NormalRect.left, m_PosData.NormalRect.top, m_PosData.NormalRect.right, m_PosData.NormalRect.bottom);
		COptions::SetOption(OPTION_LASTWINDOWPOS, str);
		if (m_wndVertSplitter.IsRowHidden(0))
			m_wndVertSplitter.ShowRow(0);
		if (m_wndVertSplitter.IsRowHidden(2))
			m_wndVertSplitter.ShowRow(2);
		int cy1, cy2, localPos, remotePos, cx, tmp;
		m_wndVertSplitter.GetRowInfoEx(0, cy1, tmp);
		m_wndVertSplitter.GetRowInfoEx(1, cy2, tmp);
		if (m_nLocalTreeViewLocation)
			m_wndLocalSplitter.GetColumnInfoEx(0, localPos, tmp);
		else
			m_wndLocalSplitter.GetRowInfoEx(0, localPos, tmp);
		if (m_nRemoteTreeViewLocation)
			m_wndRemoteSplitter.GetColumnInfoEx(0, remotePos, tmp);
		else
			m_wndRemoteSplitter.GetRowInfoEx(0, remotePos, tmp);
		m_wndMainSplitter.GetColumnInfo(m_bSwitchPaneLayout ? 1 : 0, cx, tmp);
		str.Format(_T("%d %d %d %d %d"), cy1, cy2, localPos, remotePos, cx);
		COptions::SetOption(OPTION_LASTSPLITTERSIZE, str);
	}

	CMenu *menu2 = GetMenu();
	if (menu2 && menu2->m_hMenu!=m_hMenuDefault)
	{
		menu2->DestroyMenu();
		delete menu2;
		SetMenu(NULL);
	}
	if (m_pMinTrayBtn)
		m_pMinTrayBtn->Unhook();
	delete m_pMinTrayBtn;
	m_pMinTrayBtn = NULL;
	CFrameWnd::OnClose();

}

void CMainFrame::OnProcessQueue()
{
	if (GetQueuePane()->GetListCtrl()->DoProcessQueue())
		GetQueuePane()->GetListCtrl()->StopProcessing();
	else
		TransferQueue(1);
}

void CMainFrame::OnUpdateProcessQueue(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!((CQueueCtrl *)GetQueuePane()->GetListCtrl())->IsQueueEmpty());
	pCmdUI->SetCheck(((CQueueCtrl *)GetQueuePane()->GetListCtrl())->DoProcessQueue()?1:0);
}

void CMainFrame::TransferQueue(int nPriority)
{
	int mode=((CQueueCtrl *)GetQueuePane()->GetListCtrl())->DoProcessQueue();
	((CQueueCtrl *)GetQueuePane()->GetListCtrl())->TransferQueue(nPriority);
}

void CMainFrame::OnToolbarDisconnect()
{
	m_pCommandQueue->Cancel();
	m_pCommandQueue->Disconnect();
}

void CMainFrame::OnUpdateToolbarDisconnect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsConnected() && !m_pCommandQueue->IsBusy());
}

void CMainFrame::OnToolbarRefresh()
{
	RefreshViews();
}

void CMainFrame::RefreshViews(int side)
{
	if (side!=1)
	{
		if (!m_pCommandQueue->IsBusy() && IsConnected())
			((CFtpListCtrl *)GetFtpPane()->GetListCtrl())->Refresh();

	}
	if (side!=2)
	{
		CString folder = GetLocalPane2()->GetLocalFolder();
		GetLocalPane2()->SetLocalFolder(folder);
		GetLocalPane()->GetTreeCtrl()->DisplayTree();
		GetLocalPane()->GetTreeCtrl()->SetSelPath(folder);
	}
}

void CMainFrame::OnToolbarReconnect()
{
	if (m_pCommandQueue->IsBusy() || IsConnected())
		return;

	t_server server;
	CServerPath path;
	if (m_pCommandQueue->GetLastServer(server, path))
	{
		m_pCommandQueue->Connect(server);
		if (path.IsEmpty())
			m_pCommandQueue->List(FZ_LIST_USECACHE,TRUE);
		else
			m_pCommandQueue->List(path,FZ_LIST_USECACHE,TRUE);
	}
}

void CMainFrame::OnUpdateToolbarReconnect(CCmdUI* pCmdUI)
{
	BOOL enable = FALSE;
	if (IsConnected() || m_pCommandQueue->IsBusy())
		pCmdUI->Enable(FALSE);
	else
	{
		CString host, user, pass;
		int port;
		host = COptions::GetOption(OPTION_LASTSERVERHOST);
		port = COptions::GetOptionVal(OPTION_LASTSERVERPORT);
		user = COptions::GetOption(OPTION_LASTSERVERUSER);
		//pass=crypt.decrypt(COptions::GetOption(OPTION_LASTSERVERPASS));
		if ((host != "")&&(port != 0))//&&(user!=""))
			enable = TRUE;

		pCmdUI->Enable(enable);
	}

}

CImageList* CMainFrame::CreateDragImageEx(CListCtrl *pList, LPPOINT lpPoint)
{
	if (pList->GetSelectedCount() <= 0)
		return NULL; // no row selected


	DWORD dwStyle = GetWindowLong(pList->m_hWnd, GWL_STYLE) & LVS_TYPEMASK;

	CRect rectComplete(0, 0, 0, 0);

	// Determine List Control Client width size
	CRect rectClient;
	pList->GetClientRect(rectClient);
	int nWidth = rectClient.Width() + 50;

	// Start and Stop index in view area
	int nIndex = pList->GetTopIndex() - 1;
	int nBottomIndex = pList->GetTopIndex() + pList->GetCountPerPage();
	if (nBottomIndex > (pList->GetItemCount() - 1))
		nBottomIndex = pList->GetItemCount() - 1;

	// Determine the size of the drag image (limited for rows visibled and client width)
	while ((nIndex = pList->GetNextItem(nIndex, LVNI_SELECTED)) != -1)
	{
		if (nIndex > nBottomIndex)
			break;

		CRect rectItem;
		pList->GetItemRect(nIndex, rectItem, LVIR_BOUNDS);

		if (rectItem.left < 0)
			rectItem.left = 0;

		if (rectItem.right > nWidth)
			rectItem.right = nWidth;

		rectComplete.UnionRect(rectComplete, rectItem);
	}

	// Create memory device context
	CClientDC dcClient(this);
	CDC dcMem;
	CBitmap Bitmap;

	if (!dcMem.CreateCompatibleDC(&dcClient))
		return NULL;

	if (!Bitmap.CreateCompatibleBitmap(&dcClient, rectComplete.Width(), rectComplete.Height()))
		return NULL;

	CBitmap *pOldMemDCBitmap = dcMem.SelectObject(&Bitmap);
	// Use green as mask color
 	dcMem.FillSolidRect(0, 0, rectComplete.Width(), rectComplete.Height(), RGB(0,255,0));

	// Don't use antialiased font for the dragimages
	CFont *pFont = pList->GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	lf.lfQuality = NONANTIALIASED_QUALITY;
	CFont newFont;
	newFont.CreateFontIndirect(&lf);

	CFont *oldFont = dcMem.SelectObject(&newFont);

	// Paint each DragImage in the DC
	nIndex = pList->GetTopIndex() - 1;
	while ((nIndex = pList->GetNextItem(nIndex, LVNI_SELECTED)) != -1)
	{
		if (nIndex > nBottomIndex)
			break;

		TCHAR buffer[1000];
		LVITEM item = {0};
		item.mask = LVIF_TEXT | LVIF_IMAGE;
		item.iItem = nIndex;
		item.pszText = buffer;
		item.cchTextMax = 999;

		pList->GetItem(&item);

		// Draw the icon
		CImageList* pSingleImageList = pList->GetImageList((dwStyle == LVS_ICON)?LVSIL_NORMAL:LVSIL_SMALL);
		if (pSingleImageList)
		{
			CRect rectIcon;
			pList->GetItemRect(nIndex, rectIcon, LVIR_ICON);

			IMAGEINFO info;
			pSingleImageList->GetImageInfo(item.iImage, &info);
			CPoint p((rectIcon.left - rectComplete.left + rectIcon.right - rectComplete.left) / 2 - (info.rcImage.right - info.rcImage.left) / 2,
				(rectIcon.top - rectComplete.top + rectIcon.bottom - rectComplete.top) / 2 - (info.rcImage.bottom - info.rcImage.top) / 2 + ((dwStyle == LVS_ICON)?2:0));

			pSingleImageList->Draw( &dcMem, item.iImage,
									p,
									ILD_TRANSPARENT);
		}

		// Draw the text
		CString text;
		text = item.pszText;
		CRect textRect;
		pList->GetItemRect( nIndex, textRect, LVIR_LABEL );
		textRect.top -= rectComplete.top - 2;
		textRect.bottom -= rectComplete.top + 1;
		textRect.left -= rectComplete.left - 2;
		textRect.right -= rectComplete.left;

		DWORD flags = DT_END_ELLIPSIS | DT_MODIFYSTRING;
		if (dwStyle == LVS_ICON)
			flags |= DT_CENTER | DT_WORDBREAK;
		dcMem.DrawText(text, -1, textRect, flags);
	}

	dcMem.SelectObject(oldFont);

	dcMem.SelectObject(pOldMemDCBitmap);

	// Create drag image(list)
	CImageList* pCompleteImageList = new CImageList;
	pCompleteImageList->Create(rectComplete.Width(), rectComplete.Height(), ILC_COLOR32 | ILC_MASK, 0, 1);
	pCompleteImageList->Add(&Bitmap, RGB(0, 255, 0)); // Green is used as mask color
	Bitmap.DeleteObject();

	if (lpPoint)
	{
		lpPoint->x = rectComplete.left;
		lpPoint->y = rectComplete.top;
	}

	return pCompleteImageList;
}

CImageList* CMainFrame::CreateDragImageEx(CTreeCtrl *pTree, LPPOINT lpPoint)
{
	HTREEITEM hItem;

	if (pTree == GetLocalPane()->GetTreeCtrl())
		hItem = reinterpret_cast<CDirTreeCtrl *>(pTree)->m_hDragSource;
	else
		hItem = reinterpret_cast<CFtpTreeCtrl *>(pTree)->m_hDragSource;

	if (!hItem)
		return NULL; // no row selected

	CRect rectComplete(0, 0, 0, 0);

	// Determine List Control Client width size
	CRect rectClient;
	pTree->GetClientRect(rectClient);
	int nWidth = rectClient.Width();

	CRect rectItem;
	pTree->GetItemRect(hItem, rectItem, FALSE);

	if (rectItem.left < 0)
		rectItem.left = 0;

	if (rectItem.right > (nWidth + 50))
		rectItem.right = nWidth + 50;
	rectComplete.UnionRect(rectComplete, rectItem);


	// Create memory device context
	CClientDC dcClient(this);
	CDC dcMem;
	CBitmap Bitmap;

	if (!dcMem.CreateCompatibleDC(&dcClient))
		return NULL;

	if (!Bitmap.CreateCompatibleBitmap(&dcClient, rectComplete.Width(), rectComplete.Height()))
		return NULL;

	CBitmap *pOldMemDCBitmap = dcMem.SelectObject(&Bitmap);
	// Use green as mask color
 	dcMem.FillSolidRect(0, 0, rectComplete.Width(), rectComplete.Height(), RGB(0,255,0));

	// Don't use antialiased font for the dragimages
	CFont *pFont = pTree->GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	lf.lfQuality = NONANTIALIASED_QUALITY;
	CFont newFont;
	newFont.CreateFontIndirect(&lf);

	CFont *oldFont = dcMem.SelectObject(&newFont);

	TCHAR buffer[1000];
	TVITEM tvItem = {0};
	tvItem.mask = TVIF_TEXT | TVIF_IMAGE;
	tvItem.hItem = hItem;
	tvItem.pszText = buffer;
	tvItem.cchTextMax = 999;

	pTree->GetItem(&tvItem);

	// Draw the icon
	CImageList* pSingleImageList = pTree->GetImageList(TVSIL_NORMAL);
	if (pSingleImageList)
	{
		CRect rectIcon;
		pTree->GetItemRect(hItem, rectIcon, true);

		IMAGEINFO info;
		pSingleImageList->GetImageInfo(tvItem.iImage, &info);
		CPoint p(rectIcon.left - rectComplete.left - (info.rcImage.right - info.rcImage.left) - 3,
				rectIcon.top - rectComplete.top);
		pSingleImageList->Draw( &dcMem, tvItem.iImage,
								p,
								ILD_TRANSPARENT);
	}

	// Draw the text
	CString text;
	text = tvItem.pszText;
	CRect textRect;
	pTree->GetItemRect(hItem, textRect, true);
	textRect.top -= rectComplete.top - 2;
	textRect.bottom -= rectComplete.top + 1;
	textRect.left -= rectComplete.left - 2;
	textRect.right -= rectComplete.left;

	DWORD flags = DT_END_ELLIPSIS | DT_MODIFYSTRING;
	dcMem.DrawText(text, -1, textRect, flags);

	dcMem.SelectObject(oldFont);

	dcMem.SelectObject(pOldMemDCBitmap);

	// Create drag image(list)
	CImageList* pCompleteImageList = new CImageList;
	pCompleteImageList->Create(rectComplete.Width(), rectComplete.Height(), ILC_COLOR32 | ILC_MASK, 0, 1);
	pCompleteImageList->Add(&Bitmap, RGB(0, 255, 0)); // Green is used as mask color
	Bitmap.DeleteObject();

	if (lpPoint)
	{
		lpPoint->x = rectComplete.left;
		lpPoint->y = rectComplete.top;
	}

	return pCompleteImageList;
}

void CMainFrame::OnBegindrag(CWnd *pDragWnd, CPoint point)
{
	POINT pt = {0};
	if (pDragWnd == GetLocalPane2()->GetListCtrl() ||
		pDragWnd == GetFtpPane()->GetListCtrl())
	{
		if (reinterpret_cast<CListCtrl *>(pDragWnd)->GetSelectedCount() <= 0)
			return;  // No row selected
		m_pDragImage = CreateDragImageEx(reinterpret_cast<CListCtrl *>(pDragWnd), &pt);
	}
	else
	{
		if (!reinterpret_cast<CTreeCtrl *>(pDragWnd)->GetSelectedItem())
			return;	// No item selected
		m_pDragImage = CreateDragImageEx(reinterpret_cast<CTreeCtrl *>(pDragWnd), &pt);
	}

	if (m_pDragImage == NULL)
		return;

	m_pDragWnd = pDragWnd;
	CPoint ptStart = point;
	ptStart -= pt;
	m_pDragImage->BeginDrag(0, ptStart);
	CRect rect;
	::GetWindowRect(pDragWnd->m_hWnd, &rect);
	CPoint pt2 = point;
	pt2.Offset(rect.left+8, rect.top+8);
	m_pDragImage->DragEnter(NULL, pt2);
	SetCapture();
}

void CMainFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_pDragImage && m_pDragWnd) // In Drag&Drop mode ?
	{
		CPoint ptDropPoint(point);
		ClientToScreen(&ptDropPoint);
		CWnd* pDropWnd = CWnd::WindowFromPoint(ptDropPoint);
		m_pDragImage->DragMove(ptDropPoint);

		if (m_pDragWnd == GetFtpTreePane()->GetTreeCtrl() &&
			pDropWnd != GetFtpTreePane()->GetTreeCtrl() &&
			pDropWnd != GetFtpPane()->GetListCtrl())
		{
			SetCursor(m_hcNo);
			reinterpret_cast<CFtpTreeCtrl *>(GetFtpTreePane()->GetTreeCtrl())->DragLeave(m_pDragImage);
			reinterpret_cast<CFtpListCtrl *>(GetFtpPane()->GetListCtrl())->DragLeave(m_pDragImage);
			reinterpret_cast<CDirTreeCtrl *>(GetLocalPane()->GetTreeCtrl())->SetDragHighlight(0, m_pDragImage);
			reinterpret_cast<CLocalFileListCtrl *>(GetLocalPane2()->GetListCtrl())->DragLeave(m_pDragImage);
		}
		else if (pDropWnd == GetQueuePane()->GetListCtrl())
		{
			SetCursor(m_hcArrow);
			reinterpret_cast<CFtpTreeCtrl *>(GetFtpTreePane()->GetTreeCtrl())->DragLeave(m_pDragImage);
			reinterpret_cast<CFtpListCtrl *>(GetFtpPane()->GetListCtrl())->DragLeave(m_pDragImage);
			reinterpret_cast<CDirTreeCtrl *>(GetLocalPane()->GetTreeCtrl())->SetDragHighlight(0, m_pDragImage);
			reinterpret_cast<CLocalFileListCtrl *>(GetLocalPane2()->GetListCtrl())->DragLeave(m_pDragImage);
		}
		else if (pDropWnd == GetLocalPane2()->GetListCtrl())
		{
			if (reinterpret_cast<CLocalFileListCtrl *>(GetLocalPane2()->GetListCtrl())->DragPosition(m_pDragImage, m_pDragWnd, ptDropPoint))
				SetCursor(m_hcArrow);
			else
				SetCursor(m_hcNo);
			reinterpret_cast<CFtpListCtrl *>(GetFtpPane()->GetListCtrl())->DragLeave(m_pDragImage);
			reinterpret_cast<CFtpTreeCtrl *>(GetFtpTreePane()->GetTreeCtrl())->DragLeave(m_pDragImage);
			reinterpret_cast<CDirTreeCtrl *>(GetLocalPane()->GetTreeCtrl())->SetDragHighlight(0, m_pDragImage);
		}
		else if (pDropWnd == GetFtpPane()->GetListCtrl())
		{
			if (reinterpret_cast<CFtpListCtrl *>(GetFtpPane()->GetListCtrl())->DragPosition(m_pDragImage, m_pDragWnd, ptDropPoint))
				SetCursor(m_hcArrow);
			else
				SetCursor(m_hcNo);
			reinterpret_cast<CFtpTreeCtrl *>(GetFtpTreePane()->GetTreeCtrl())->DragLeave(m_pDragImage);
			reinterpret_cast<CDirTreeCtrl *>(GetLocalPane()->GetTreeCtrl())->SetDragHighlight(0, m_pDragImage);
			reinterpret_cast<CLocalFileListCtrl *>(GetLocalPane2()->GetListCtrl())->DragLeave(m_pDragImage);
		}
		else if (pDropWnd == GetFtpTreePane()->GetTreeCtrl())
		{
			if (reinterpret_cast<CFtpTreeCtrl *>(GetFtpTreePane()->GetTreeCtrl())->DragPosition(m_pDragImage, m_pDragWnd, ptDropPoint))
				SetCursor(m_hcArrow);
			else
				SetCursor(m_hcNo);
			reinterpret_cast<CFtpListCtrl *>(GetFtpPane()->GetListCtrl())->DragLeave(m_pDragImage);
			reinterpret_cast<CDirTreeCtrl *>(GetLocalPane()->GetTreeCtrl())->SetDragHighlight(0, m_pDragImage);
			reinterpret_cast<CLocalFileListCtrl *>(GetLocalPane2()->GetListCtrl())->DragLeave(m_pDragImage);
		}
		else if (pDropWnd == GetLocalPane()->GetTreeCtrl())
		{
			if (reinterpret_cast<CDirTreeCtrl *>(GetLocalPane()->GetTreeCtrl())->DragPosition(m_pDragImage, m_pDragWnd, ptDropPoint))
				SetCursor(m_hcArrow);
			else
				SetCursor(m_hcNo);
			reinterpret_cast<CFtpListCtrl *>(GetFtpPane()->GetListCtrl())->DragLeave(m_pDragImage);
			reinterpret_cast<CLocalFileListCtrl *>(GetLocalPane2()->GetListCtrl())->DragLeave(m_pDragImage);
			reinterpret_cast<CFtpTreeCtrl *>(GetFtpTreePane()->GetTreeCtrl())->DragLeave(m_pDragImage);
		}
		else
		{
			reinterpret_cast<CFtpListCtrl *>(GetFtpPane()->GetListCtrl())->DragLeave(m_pDragImage);
			reinterpret_cast<CFtpTreeCtrl *>(GetFtpTreePane()->GetTreeCtrl())->DragLeave(m_pDragImage);
			reinterpret_cast<CDirTreeCtrl *>(GetLocalPane()->GetTreeCtrl())->SetDragHighlight(0, m_pDragImage);
			reinterpret_cast<CLocalFileListCtrl *>(GetLocalPane2()->GetListCtrl())->DragLeave(m_pDragImage);
			SetCursor(m_hcNo);
		}
	}
	CFrameWnd::OnMouseMove(nFlags, point);
}

void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_pDragImage && m_pDragWnd) // In Drag&Drop mode ?
	{
		::ReleaseCapture();
		m_pDragImage->DragLeave(GetDesktopWindow());
		m_pDragImage->EndDrag();

		CPoint pt(point);
		ClientToScreen(&pt);
	 	CWnd* m_pDropWnd = WindowFromPoint(pt);

		if (m_pDragWnd == GetLocalPane2()->GetListCtrl())
		{
			if (m_pDropWnd == GetFtpPane()->GetListCtrl())
			{
				m_DropTarget = ((CFtpListCtrl*)GetFtpPane()->GetListCtrl())->GetDropTarget();
				if (((CFtpListCtrl*)GetFtpPane()->GetListCtrl())->GetItemCount())
					((CLocalFileListCtrl*)GetLocalPane2()->GetListCtrl())->OnDragEnd(0, pt);
			}
			else if (m_pDropWnd == GetFtpTreePane()->GetTreeCtrl())
			{
				m_DropTarget = ((CFtpTreeCtrl*)GetFtpTreePane()->GetTreeCtrl())->GetDropTarget().GetSafePath();
				if (m_DropTarget != _T(""))
					((CLocalFileListCtrl*)GetLocalPane2()->GetListCtrl())->OnDragEnd(0, pt);
			}
			else if (m_pDropWnd == GetQueuePane()->GetListCtrl())
			{
				if (((CFtpListCtrl*)GetFtpPane()->GetListCtrl())->GetItemCount())
					((CLocalFileListCtrl*)GetLocalPane2()->GetListCtrl())->OnDragEnd(1, pt);
			}
			else if (m_pDropWnd == GetLocalPane2()->GetListCtrl())
			{
				((CLocalFileListCtrl*)GetLocalPane2()->GetListCtrl())->OnDragEnd(2, pt);
			}
			else if (m_pDropWnd == GetLocalPane()->GetTreeCtrl())
			{
				((CLocalFileListCtrl*)GetLocalPane2()->GetListCtrl())->OnDragEnd(3, pt);
			}
		}
		else if (m_pDragWnd == GetLocalPane()->GetTreeCtrl())
		{
			if (m_pDropWnd == GetFtpPane()->GetListCtrl())
			{
				m_DropTarget = ((CFtpListCtrl*)GetFtpPane()->GetListCtrl())->GetDropTarget();
				if (((CFtpListCtrl*)GetFtpPane()->GetListCtrl())->GetItemCount())
					((CDirTreeCtrl *)GetLocalPane()->GetTreeCtrl())->OnDragEnd(0, pt);
			}
			else if (m_pDropWnd == GetFtpTreePane()->GetTreeCtrl())
			{
				m_DropTarget = ((CFtpTreeCtrl*)GetFtpTreePane()->GetTreeCtrl())->GetDropTarget().GetSafePath();
				if (m_DropTarget != _T(""))
					((CDirTreeCtrl *)GetLocalPane()->GetTreeCtrl())->OnDragEnd(0, pt);
			}
			else if (m_pDropWnd == GetQueuePane()->GetListCtrl())
			{
				if (((CFtpListCtrl*)GetFtpPane()->GetListCtrl())->GetItemCount())
					((CDirTreeCtrl *)GetLocalPane()->GetTreeCtrl())->OnDragEnd(1, pt);
			}
			else if (m_pDropWnd == GetLocalPane()->GetTreeCtrl())
				((CDirTreeCtrl *)GetLocalPane()->GetTreeCtrl())->OnDragEnd(2, pt);
			else if (m_pDropWnd == GetLocalPane2()->GetListCtrl())
				((CDirTreeCtrl *)GetLocalPane()->GetTreeCtrl())->OnDragEnd(3, pt);
		}
		else if (m_pDragWnd == GetFtpPane()->GetListCtrl())
		{
			if (m_pDropWnd == GetLocalPane2()->GetListCtrl())
			{
				m_DropTarget = ((CLocalFileListCtrl *)GetLocalPane2()->GetListCtrl())->GetDropTarget();
				((CFtpListCtrl*)GetFtpPane()->GetListCtrl())->OnDragEnd(0, pt);
			}
			else if (m_pDropWnd == GetLocalPane()->GetTreeCtrl())
			{
				m_DropTarget = reinterpret_cast<CDirTreeCtrl *>(m_pDropWnd)->GetDropTarget();
				((CFtpListCtrl*)GetFtpPane()->GetListCtrl())->OnDragEnd(0, pt);
			}
			else if (m_pDropWnd == GetQueuePane()->GetListCtrl())
				((CFtpListCtrl*)GetFtpPane()->GetListCtrl())->OnDragEnd(1, pt);
			else if (m_pDropWnd == GetFtpPane()->GetListCtrl())
				((CFtpListCtrl*)GetFtpPane()->GetListCtrl())->OnDragEnd(2, pt);
			else if (m_pDropWnd == GetFtpTreePane()->GetTreeCtrl())
				((CFtpListCtrl*)GetFtpPane()->GetListCtrl())->OnDragEnd(3, pt);
		}
		else if (m_pDragWnd == GetFtpTreePane()->GetTreeCtrl())
		{
			if (m_pDropWnd == GetFtpTreePane()->GetTreeCtrl())
				reinterpret_cast<CFtpTreeCtrl *>(GetFtpTreePane()->GetTreeCtrl())->OnDragEnd(0, pt);
		}

		reinterpret_cast<CFtpListCtrl *>(GetFtpPane()->GetListCtrl())->DragLeave(NULL);
		reinterpret_cast<CFtpTreeCtrl *>(GetFtpTreePane()->GetTreeCtrl())->DragLeave(NULL);
		reinterpret_cast<CDirTreeCtrl *>(GetLocalPane()->GetTreeCtrl())->SetDragHighlight(0, m_pDragImage);
		reinterpret_cast<CLocalFileListCtrl *>(GetLocalPane2()->GetListCtrl())->DragLeave(m_pDragImage);

		m_pDragImage->DeleteImageList();
		delete m_pDragImage;
		m_pDragImage = NULL;
		m_pDragWnd = NULL;
		m_DropTarget = _T("");
	}

	CFrameWnd::OnLButtonUp(nFlags, point);
}

static BOOL ParseURL(CString &remote, t_server &server, CServerPath &path, BOOL bIsFile = FALSE)
{
	server.port=0;
	server.fwbypass=0;
	server.bDontRememberPass = FALSE;
	server.nServerType=0;
	if (remote=="")
	{
		remote.LoadString(IDS_ERRORMSG_HOSTNEEDED);
		return FALSE;
	}
	int pos=remote.Find( _T("://") );
	if (pos!=-1)
	{
		CString tmp=remote.Left(pos+3);
		tmp.MakeLower();
		if (tmp==_T("ftp://") || tmp==_T("fz_ftp://"))
		{
			remote=remote.Right(remote.GetLength()-pos-3);
			server.nServerType=FZ_SERVERTYPE_FTP;
			pos=0;
		}
		else if (tmp==_T("ftps://") || tmp==_T("fz_ftps://"))
		{
			remote=remote.Right(remote.GetLength()-pos-3);
			server.nServerType=FZ_SERVERTYPE_FTP|FZ_SERVERTYPE_LAYER_SSL_IMPLICIT;
		}
		else if (tmp==_T("sftp://") || tmp==_T("fz_sftp://"))
		{
			remote=remote.Right(remote.GetLength()-pos-3);
			server.nServerType=FZ_SERVERTYPE_FTP|FZ_SERVERTYPE_SUB_FTP_SFTP;
		}
		else
		{
			remote.LoadString(IDS_ERRORMSG_URLSYNTAXINVALID);
			return FALSE;
		}
		pos+=3;
	}
	else
		pos=0;

	pos=remote.Find('/',pos);
	CString rawpath;
	if (pos!=-1)
	{
		if ( rawpath.GetLength()>(pos+1) && rawpath[pos+1]==' ' )
			rawpath=remote.Right(remote.GetLength()-pos);
		else
			rawpath=remote.Right(remote.GetLength()-pos-1);
		remote=remote.Left(pos);
		if (remote=="")
		{
			remote.LoadString(IDS_ERRORMSG_URLSYNTAXINVALID);
			return FALSE;
		}
	}
	pos = remote.ReverseFind('@');
	if (pos > 0)
	{
		CString tmp=remote.Left(pos);
		int pos2=tmp.Find(':');
		if (!pos2)
		{
			remote.LoadString(IDS_ERRORMSG_URLSYNTAXINVALID);
			return FALSE;
		}
		else if (pos2==-1)
		{
			server.user = remote.Left(pos);
			server.bDontRememberPass = TRUE;
		}
		else
		{
			server.user=tmp.Left(pos2);
			server.pass = tmp.Right(tmp.GetLength()-pos2-1);
		}
		remote=remote.Right(remote.GetLength()-pos-1);
	}
	pos = remote.ReverseFind(':');
	if (pos != -1)
	{
		server.port=_ttoi(remote.Right(remote.GetLength()-pos-1));
		remote=remote.Left(pos);
	}

	if (!server.port)
		server.port=21;

	if (server.user=="")
	{
		BOOL bUseGSS=FALSE;
		if (COptions::GetOptionVal(OPTION_USEGSS))
		{
			USES_CONVERSION;

			CString GssServers=COptions::GetOption(OPTION_GSSSERVERS);
			hostent *fullname=gethostbyname(T2CA(remote));
			CString host;
			if (fullname)
				host=fullname->h_name;
			else
				host=remote;
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
		if (!bUseGSS)
		{
			server.user="anonymous";
			server.pass=COptions::GetOption(OPTION_ANONPWD);
		}
		else
		{
			server.user="";
			server.pass="";
		}
	}
	if (!server.nServerType)
	{
		server.nServerType=FZ_SERVERTYPE_FTP;
		if (server.port==990)
			server.nServerType|=FZ_SERVERTYPE_LAYER_SSL_IMPLICIT;
	}

	path.SetServer(server);
	if (!path.SetPath(rawpath, bIsFile))
	{
		remote=_T("ERROR, can't parse path");
		return FALSE;
	}

	if (path.IsEmpty())
		path.SetPath("/");
	server.host = remote;
	remote = rawpath;

	return TRUE;
}

LRESULT CMainFrame::OnProcessCmdLine(WPARAM wParam,LPARAM lParam)
{
	CString cmdline=((CFileZillaApp *)AfxGetApp())->m_lpCmdLine;
	if (cmdline!="")
	{
		std::list<CString> ArgList;
		while (cmdline!="")
		{
			int space=cmdline.Find( _T(" ") );
			if (cmdline[0]=='\"')
			{
				cmdline=cmdline.Mid(1);
				int quote=cmdline.Find(_T("\""));
				if (quote==-1)
				{
					ArgList.push_back(cmdline);
					break;
				}
				else
				{
					ArgList.push_back(cmdline.Left(quote));
					cmdline=cmdline.Mid(quote+1);
					cmdline.TrimLeft( _T(" ") );
				}
			}
			else
			{
				if (space==-1)
				{
					ArgList.push_back(cmdline);
					break;
				}
				ArgList.push_back(cmdline.Left(space));
				cmdline=cmdline.Mid(space+1);
				cmdline.TrimLeft(_T(" "));
			}

		}

		bool bDidConnect = false;
		bool closing = false; // for scripts to shut down dialog after action

		while (!ArgList.empty())
		{
			CString arg = ArgList.front();
			ArgList.pop_front();
			if (arg[0] != '/' && arg[0] != '-' && !bDidConnect)
			{
				bDidConnect = true;
				t_server server;
				CServerPath remotepath;
				if (!ParseURL(arg, server, remotepath))
				{
					AfxMessageBox(IDS_ERRORMSG_CANTPARSECMDLINE);
					return 0;
				}

				if (server.bDontRememberPass)
				{
					CEnterSomething dlg(IDS_INPUTDIALOGTITLE_INPUTPASSWORD, IDS_INPUTDIALOGTEXT_INPUTPASSWORD, '*');
					if (dlg.DoModal() != IDOK)
						return 0;
					server.bDontRememberPass = FALSE;
					server.pass = dlg.m_String;
				}
				if (m_pCommandQueue->Connect(server))
					m_pCommandQueue->List(remotepath, FZ_LIST_USECACHE, TRUE);
			}
			else
			{
				if (arg[0]=='/')
					arg.SetAt(0, '-');
				arg.MakeLower();
				if (arg==_T("-s") && !bDidConnect)
				{
					if (ArgList.empty())
					{
						AfxMessageBox(IDS_ERRORMSG_CANTPARSECMDLINE);
						return 0;
					}
					t_SiteManagerItem *site = CSiteManager::GetSiteByName(ArgList.front());
					ArgList.pop_front();
					if (!site)
						continue;

					bDidConnect = true;
					ParseSite(*site);
					delete site;
				}
				if (arg==_T("-config") || arg==_T("-t"))
				{
					if (ArgList.empty())
					{
						AfxMessageBox(IDS_ERRORMSG_CANTPARSECMDLINE);
						return 0;
					}
					ArgList.pop_front();
				}
				else if (arg==_T("-i"))
				{
					if (ArgList.empty())
					{
						AfxMessageBox(IDS_ERRORMSG_CANTPARSECMDLINE);
						return 0;
					}

					arg = ArgList.front();
					ArgList.pop_front();

					CSiteManager dlg;
					if (dlg.ImportAndShow(arg)==IDOK)
					{
						if (!bDidConnect)
						{
							bDidConnect = true;
							ParseSite(dlg.m_LastSite);
						}
					}
				}
				else if (arg==_T("-u") || arg==_T("-uf"))
				{
					if (ArgList.size() < 2)
					{
						AfxMessageBox(IDS_ERRORMSG_CANTPARSECMDLINE);
						return 0;
					}
					CString localfile = ArgList.front();
					ArgList.pop_front();
					int pos=localfile.ReverseFind('\\');
					if (pos==-1 || !pos || pos==(localfile.GetLength()-1))
						AfxMessageBox(IDS_ERRORMSG_CANTPARSECMDLINE);
					else
					{
						__int64 size;
						if (!GetLength64(localfile, size))
							size=-1;
						CString localpath=localfile.Left(pos);
						localfile=localfile.Mid(pos+1);

						CString remote=ArgList.front();
						ArgList.pop_front();
						t_server server;
						CServerPath remotepath;
						if (!ParseURL(remote, server, remotepath))
						{
							AfxMessageBox(IDS_ERRORMSG_CANTPARSECMDLINE);
							return 0;
						}

						t_transferfile transferfile;
						transferfile.get=FALSE;
						transferfile.localfile=localpath+"\\"+localfile;
						transferfile.remotefile=localfile;
						transferfile.remotepath=remotepath;
						transferfile.server=server;
						transferfile.size=size;

						transferfile.nType=m_nTransferType;
						if (!m_nTransferType)
						{
							transferfile.nType=2;
							CString AsciiFiles=COptions::GetOption(OPTION_ASCIIFILES);
							CString fn=transferfile.get?transferfile.remotefile:transferfile.localfile;
							fn.MakeUpper();
							int i;
							while ((i = AsciiFiles.Find( _T(";") )) != -1)
							{
								if ("." + AsciiFiles.Left(i) == fn.Right(AsciiFiles.Left(i).GetLength() + 1))
								{
									transferfile.nType = 1;
									break;
								}
								AsciiFiles = AsciiFiles.Mid(i + 1);
							}
						}

						if (!ArgList.empty())
						{
							CString arg = ArgList.front();
							arg.MakeLower();
							if (arg == _T("-t"))
							{
								ArgList.pop_front();
								if (ArgList.empty())
								{
									AfxMessageBox(IDS_ERRORMSG_CANTPARSECMDLINE);
									return 0;
								}
								arg = ArgList.front();
								ArgList.pop_front();
								transferfile.remotefile = arg;
							}
						}

						if (transferfile.server.bDontRememberPass)
						{
							CEnterSomething dlg(IDS_INPUTDIALOGTITLE_INPUTPASSWORD, IDS_INPUTDIALOGTEXT_INPUTPASSWORD, '*');
							if (dlg.DoModal() != IDOK)
								return 0;
							transferfile.server.bDontRememberPass = FALSE;
							transferfile.server.pass = dlg.m_String;
						}

						GetQueuePane()->AddItem(transferfile, TRUE, 0);

						TransferQueue(2);
					}
				}
				else if (arg==_T("-d") || arg==_T("-df"))
				{
					if (ArgList.size() < 2)
					{
						AfxMessageBox(IDS_ERRORMSG_CANTPARSECMDLINE);
						return 0;
					}
					CString remotefile=ArgList.front();
					ArgList.pop_front();
					CString localpath=ArgList.front();
					ArgList.pop_front();
					localpath.TrimRight( _T("\\") );

					t_server server;
					CServerPath remotepath;

					if (!ParseURL(remotefile, server, remotepath, TRUE))
					{
						AfxMessageBox(IDS_ERRORMSG_CANTPARSECMDLINE);
						return 0;
					}

					__int64 size=-1;

					t_transferfile transferfile;
					transferfile.get=TRUE;
					transferfile.localfile=localpath+"\\"+remotefile;
					transferfile.remotefile=remotefile;
					transferfile.remotepath=remotepath;
					transferfile.server=server;
					transferfile.size=size;

					transferfile.nType=m_nTransferType;
					if (!m_nTransferType)
					{
						transferfile.nType=2;
						CString AsciiFiles=COptions::GetOption(OPTION_ASCIIFILES);
						CString fn=transferfile.get?transferfile.remotefile:transferfile.localfile;
						fn.MakeUpper();
						int i;
						while ((i = AsciiFiles.Find( _T(";") )) != -1)
						{
							if ("." + AsciiFiles.Left(i) == fn.Right(AsciiFiles.Left(i).GetLength() + 1))
							{
								transferfile.nType = 1;
								break;
							}
							AsciiFiles = AsciiFiles.Mid(i + 1);
						}
					}

					if (!ArgList.empty())
					{
						CString arg=ArgList.front();
						ArgList.pop_front();
						arg.MakeLower();
						if (arg == _T("-t"))
						{
							ArgList.pop_front();
							if (ArgList.empty())
							{
								AfxMessageBox(IDS_ERRORMSG_CANTPARSECMDLINE);
								return 0;
							}
							arg = ArgList.front();
							ArgList.pop_front();
							transferfile.localfile = localpath + _T("\\") + arg;
						}
					}

					if (transferfile.server.bDontRememberPass)
					{
						CEnterSomething dlg(IDS_INPUTDIALOGTITLE_INPUTPASSWORD, IDS_INPUTDIALOGTEXT_INPUTPASSWORD, '*');
						if (dlg.DoModal() != IDOK)
							return 0;
						transferfile.server.bDontRememberPass = FALSE;
						transferfile.server.pass = dlg.m_String;
					}

					GetQueuePane()->AddItem(transferfile, TRUE, 0);

					TransferQueue(2);
				}
				else if (arg==_T("-close"))
				{
					closing=true;
				}
				else if (arg==_T("-noclose"))
				{
					closing=false;
				}
				else if (arg==_T("-overwrite"))
				{
					CFileExistsDlg::SetOverwriteSettings(FILEEXISTS_OVERWRITE);
				}
			}
		}
		if (closing)
		{
			SetTimer(CMDLINE_CLOSE, 1000, 0);
		}
	}
	return 0;
}

void CMainFrame::OnCopytositemanager()
{
	t_server server;
	if (m_pFileZillaApi->GetCurrentServer(server)==FZ_REPLY_OK)
	{
		server.path=((CFtpListCtrl *)GetFtpPane()->GetListCtrl())->GetCurrentDirectory().GetPath();
		CSiteManager dlg;
		int res=dlg.AddAndShow(server);
		if (res==IDOK)
			ParseSite(dlg.m_LastSite);
	}
}

void CMainFrame::OnUpdateCopytositemanager(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsConnected());
}

void CMainFrame::OnUpdateFileConnecttodefaultsite(CCmdUI* pCmdUI)
{
	t_SiteManagerItem *site=CSiteManager::GetDefaultSite();
	pCmdUI->Enable((BOOL)site);
	if (site)
		delete site;
}

void CMainFrame::OnTypemenuAscii()
{
	m_nTransferType=1;
}

void CMainFrame::OnUpdateTypemenuAscii(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nTransferType==1);
}

void CMainFrame::OnTypemenuBinary()
{
	m_nTransferType=2;
}

void CMainFrame::OnUpdateTypemenuBinary(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nTransferType==2);
}

void CMainFrame::OnTypemenuDetect()
{
	m_nTransferType=0;
}

void CMainFrame::OnUpdateTypemenuDetect(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nTransferType==0);
}

void CMainFrame::OnDestroy()
{
	if (m_pSecureIconCtrl->GetSafeHwnd() && m_pSecureIconCtrl->GetIcon())
		DestroyIcon(m_pSecureIconCtrl->GetIcon());
	delete m_pSecureIconCtrl;
	m_pSecureIconCtrl=0;

	CFrameWnd::OnDestroy();

	if (m_pLocalViewHeader)
	{
		delete m_pLocalViewHeader->m_pEdit;
		delete m_pLocalViewHeader->m_pLabelBackground;
		delete m_pLocalViewHeader->m_pLabel;
		delete m_pLocalViewHeader;
		m_pLocalViewHeader=0;
	}

	if (m_pRemoteViewHeader)
	{
		delete m_pRemoteViewHeader->m_pEdit;
		delete m_pRemoteViewHeader->m_pLabelBackground;
		delete m_pRemoteViewHeader->m_pLabel;
		delete m_pRemoteViewHeader;
		m_pRemoteViewHeader=0;
	}

	SaveMostRecentServers();
}

void CMainFrame::OnViewQuickconnectBar()
{
	m_bShowQuickconnect = !m_bShowQuickconnect;
	if (m_bShowQuickconnect)
		m_pWndDlgBar->ShowWindow(SW_SHOW);
	else
	{
		CWnd *pFocus = GetFocus();
		if (pFocus)
		{
			if (pFocus == m_pWndDlgBar ||
				pFocus->GetParent() == m_pWndDlgBar)
				SetFocus();
		}
		m_pWndDlgBar->ShowWindow(SW_HIDE);
	}
	RecalcLayout();
}

void CMainFrame::OnUpdateViewQuickconnectBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowQuickconnect);
}

void CMainFrame::OnViewMessagelog()
{
	m_bShowMessageLog = !m_bShowMessageLog;
	if (m_bShowMessageLog)
		m_wndVertSplitter.ShowRow(0);
	else
		m_wndVertSplitter.HideRow(0,1);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewMessagelog(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowMessageLog);
}

void CMainFrame::OnViewStatusBar()
{
	m_wndStatusBar.ShowWindow(m_wndStatusBar.IsVisible()?SW_HIDE:SW_SHOW);
	RecalcLayout();
}

void CMainFrame::OnViewToolbar()
{
	m_pWndToolBar->ShowWindow(m_pWndToolBar->IsVisible()?SW_HIDE:SW_SHOW);
	RecalcLayout();
}

void CMainFrame::OnEditExportsettings()
{
	COptions::Export();
}

void CMainFrame::OnEditImportsettings()
{
	COptions::Import();
}

//*****************************************
//Code to change the local list view styles
//*****************************************

void CMainFrame::OnLocalliststyleIcon()
{
	m_nLocalListViewStyle=2;
	GetLocalPane2()->m_pListCtrl->SetListStyle(m_nLocalListViewStyle);
}

void CMainFrame::OnUpdateLocalliststyleIcon(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLocalListViewStyle==2);
}

void CMainFrame::OnLocalliststyleList()
{
	m_nLocalListViewStyle=1;
	GetLocalPane2()->m_pListCtrl->SetListStyle(m_nLocalListViewStyle);
}

void CMainFrame::OnUpdateLocalliststyleList(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLocalListViewStyle==1);
}

void CMainFrame::OnLocalliststyleReport()
{
	m_nLocalListViewStyle=0;
	GetLocalPane2()->m_pListCtrl->SetListStyle(m_nLocalListViewStyle);
}

void CMainFrame::OnUpdateLocalliststyleReport(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLocalListViewStyle==0);
}

void CMainFrame::OnLocalliststyleSmallicon()
{
	m_nLocalListViewStyle=3;
	GetLocalPane2()->m_pListCtrl->SetListStyle(m_nLocalListViewStyle);
}

void CMainFrame::OnUpdateLocalliststyleSmallicon(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLocalListViewStyle==3);
}

void CMainFrame::OnViewLocallistviewFilesize()
{
	m_nHideLocalColumns^=1;
	GetLocalPane2()->m_pListCtrl->UpdateColumns(m_nHideLocalColumns);
}

void CMainFrame::OnUpdateViewLocallistviewFilesize(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_nLocalListViewStyle);
	pCmdUI->SetCheck(!(m_nHideLocalColumns&1));
}

void CMainFrame::OnViewLocallistviewFiletype()
{
	m_nHideLocalColumns^=2;
	GetLocalPane2()->m_pListCtrl->UpdateColumns(m_nHideLocalColumns);
}

void CMainFrame::OnUpdateViewLocallistviewFiletype(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_nLocalListViewStyle);
	pCmdUI->SetCheck(!(m_nHideLocalColumns&2));
}

void CMainFrame::OnViewLocallistviewLastmodifiedtime()
{
	m_nHideLocalColumns^=4;
	GetLocalPane2()->m_pListCtrl->UpdateColumns(m_nHideLocalColumns);
}

void CMainFrame::OnUpdateViewLocallistviewLastmodifiedtime(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_nLocalListViewStyle);
	pCmdUI->SetCheck(!(m_nHideLocalColumns&4));
}

//******************************************
//Code to change the remote list view styles
//******************************************

void CMainFrame::OnRemoteliststyleIcon()
{
	m_nRemoteListViewStyle=2;
	GetFtpPane()->m_pListCtrl->SetListStyle(m_nRemoteListViewStyle);
}

void CMainFrame::OnUpdateRemoteliststyleIcon(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nRemoteListViewStyle==2);
}

void CMainFrame::OnRemoteliststyleList()
{
	m_nRemoteListViewStyle=1;
	GetFtpPane()->m_pListCtrl->SetListStyle(m_nRemoteListViewStyle);
}

void CMainFrame::OnUpdateRemoteliststyleList(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nRemoteListViewStyle==1);
}

void CMainFrame::OnRemoteliststyleReport()
{
	m_nRemoteListViewStyle=0;
	GetFtpPane()->m_pListCtrl->SetListStyle(m_nRemoteListViewStyle);
}

void CMainFrame::OnUpdateRemoteliststyleReport(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nRemoteListViewStyle==0);
}

void CMainFrame::OnRemoteliststyleSmallicon()
{
	m_nRemoteListViewStyle=3;
	GetFtpPane()->m_pListCtrl->SetListStyle(m_nRemoteListViewStyle);
}

void CMainFrame::OnUpdateRemoteliststyleSmallicon(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nRemoteListViewStyle==3);
}

void CMainFrame::OnViewRemotelistviewFilesize()
{
	m_nHideRemoteColumns ^= 1;
	GetFtpPane()->m_pListCtrl->UpdateColumns(m_nHideRemoteColumns);
}

void CMainFrame::OnUpdateViewRemotelistviewFilesize(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_nRemoteListViewStyle);
	pCmdUI->SetCheck(!(m_nHideRemoteColumns&1));
}

void CMainFrame::OnViewRemotelistviewFiletype()
{
	m_nHideRemoteColumns ^= 2;
	GetFtpPane()->m_pListCtrl->UpdateColumns(m_nHideRemoteColumns);
}

void CMainFrame::OnUpdateViewRemotelistviewFiletype(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_nRemoteListViewStyle);
	pCmdUI->SetCheck(!(m_nHideRemoteColumns&2));
}

void CMainFrame::OnViewRemotelistviewDate()
{
	m_nHideRemoteColumns ^= 4;
	GetFtpPane()->m_pListCtrl->UpdateColumns(m_nHideRemoteColumns);
}

void CMainFrame::OnUpdateViewRemotelistviewDate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_nRemoteListViewStyle);
	pCmdUI->SetCheck(!(m_nHideRemoteColumns&4));
}

void CMainFrame::OnViewRemotelistviewTime()
{
	m_nHideRemoteColumns ^= 8;
	GetFtpPane()->m_pListCtrl->UpdateColumns(m_nHideRemoteColumns);
}

void CMainFrame::OnUpdateViewRemotelistviewTime(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_nRemoteListViewStyle);
	pCmdUI->SetCheck(!(m_nHideRemoteColumns&8));
}

void CMainFrame::OnViewRemotelistviewPermissions()
{
	m_nHideRemoteColumns ^= 0x10;
	GetFtpPane()->m_pListCtrl->UpdateColumns(m_nHideRemoteColumns);
}

void CMainFrame::OnUpdateViewRemotelistviewPermissions(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_nRemoteListViewStyle);
	pCmdUI->SetCheck(!(m_nHideRemoteColumns&0x10));
}

void CMainFrame::OnFileConnecttodefaultsite()
{
	t_SiteManagerItem *site=CSiteManager::GetDefaultSite();
	if (!site)
		return;

	ParseSite(*site);
	delete site;
	return;
}

void CMainFrame::OnHelpmenuContents()
{
	HINSTANCE hDLL=LoadLibrary( _T("hhctrl.ocx") );
	if (hDLL)
	{
		#ifdef UNICODE
			HWND (WINAPI* pHtmlHelp)(HWND hwndCaller, LPCWSTR pszFile, UINT uCommand, DWORD_PTR dwData)=(HWND (WINAPI*)(HWND hwndCaller, LPCWSTR pszFile, UINT uCommand, DWORD_PTR dwData))GetProcAddress(hDLL,"HtmlHelpW");
		#else
			HWND (WINAPI* pHtmlHelp)(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD_PTR dwData)=(HWND (WINAPI*)(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD_PTR dwData))GetProcAddress(hDLL,"HtmlHelpA");
		#endif

		if (pHtmlHelp)
			pHtmlHelp(m_hWnd, AfxGetApp()->m_pszHelpFilePath, HH_DISPLAY_TOC, 0);
	}
}

void CMainFrame::OnHelpmenuIndex()
{
	HINSTANCE hDLL=LoadLibrary( _T("hhctrl.ocx") );
	if (hDLL)
	{
		#ifdef UNICODE
			HWND (WINAPI* pHtmlHelp)(HWND hwndCaller, LPCWSTR pszFile, UINT uCommand, DWORD_PTR dwData)=(HWND (WINAPI*)(HWND hwndCaller, LPCWSTR pszFile, UINT uCommand, DWORD_PTR dwData))GetProcAddress(hDLL,"HtmlHelpW");
		#else
			HWND (WINAPI* pHtmlHelp)(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD_PTR dwData)=(HWND (WINAPI*)(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD_PTR dwData))GetProcAddress(hDLL,"HtmlHelpA");
		#endif

		if (pHtmlHelp)
			pHtmlHelp(m_hWnd, AfxGetApp()->m_pszHelpFilePath, HH_DISPLAY_INDEX, 0);
	}
}

void CMainFrame::OnHelpmenuSearch()
{
	HH_FTS_QUERY q;
	memset(&q,0,sizeof(q));
	q.cbStruct=sizeof(q);

	HINSTANCE hDLL=LoadLibrary( _T("hhctrl.ocx") );
	if (hDLL)
	{
		#ifdef UNICODE
			HWND (WINAPI* pHtmlHelp)(HWND hwndCaller, LPCWSTR pszFile, UINT uCommand, DWORD_PTR dwData)=(HWND (WINAPI*)(HWND hwndCaller, LPCWSTR pszFile, UINT uCommand, DWORD_PTR dwData))GetProcAddress(hDLL,"HtmlHelpW");
		#else
			HWND (WINAPI* pHtmlHelp)(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD_PTR dwData)=(HWND (WINAPI*)(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD_PTR dwData))GetProcAddress(hDLL,"HtmlHelpA");
		#endif

		if (pHtmlHelp)
			pHtmlHelp(m_hWnd, AfxGetApp()->m_pszHelpFilePath, HH_DISPLAY_SEARCH, (DWORD)&q);
	}
}

void CMainFrame::WinHelp(DWORD dwData, UINT nCmd)
{
	HINSTANCE hDLL=LoadLibrary( _T("hhctrl.ocx") );
	if (hDLL)
	{
		#ifdef UNICODE
			HWND (WINAPI* pHtmlHelp)(HWND hwndCaller, LPCWSTR pszFile, UINT uCommand, DWORD_PTR dwData)=(HWND (WINAPI*)(HWND hwndCaller, LPCWSTR pszFile, UINT uCommand, DWORD_PTR dwData))GetProcAddress(hDLL,"HtmlHelpW");
		#else
			HWND (WINAPI* pHtmlHelp)(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD_PTR dwData)=(HWND (WINAPI*)(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD_PTR dwData))GetProcAddress(hDLL,"HtmlHelpA");
		#endif

		if (pHtmlHelp)
			pHtmlHelp(m_hWnd, AfxGetApp()->m_pszHelpFilePath, HH_HELP_CONTEXT, nCmd == HELP_CONTEXT ? dwData : 0);
	}
}

void CMainFrame::OnOverwritemenuAsk()
{
	CFileExistsDlg::SetOverwriteSettings(FILEEXISTS_ASK);
}

void CMainFrame::OnOverwritemenuOverwrite()
{
	CFileExistsDlg::SetOverwriteSettings(FILEEXISTS_OVERWRITE);
}

void CMainFrame::OnOverwritemenuOverwriteifnewer()
{
	CFileExistsDlg::SetOverwriteSettings(FILEEXISTS_OVERWRITEIFNEWER);
}

void CMainFrame::OnOverwritemenuResume()
{
	CFileExistsDlg::SetOverwriteSettings(FILEEXISTS_RESUME);
}

void CMainFrame::OnOverwritemenuRename()
{
	CFileExistsDlg::SetOverwriteSettings(FILEEXISTS_RENAME);
}

void CMainFrame::OnOverwritemenuSkip()
{
	CFileExistsDlg::SetOverwriteSettings(FILEEXISTS_SKIP);
}

void CMainFrame::OnUpdateOverwritemenuAsk(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(!CFileExistsDlg::Always());
}

void CMainFrame::OnUpdateOverwritemenuOverwrite(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CFileExistsDlg::Always()?(CFileExistsDlg::GetAction()==FILEEXISTS_OVERWRITE):FALSE);
}

void CMainFrame::OnUpdateOverwritemenuOverwriteifnewer(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CFileExistsDlg::Always()?(CFileExistsDlg::GetAction()==FILEEXISTS_OVERWRITEIFNEWER):FALSE);
}

void CMainFrame::OnUpdateOverwritemenuResume(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CFileExistsDlg::Always()?(CFileExistsDlg::GetAction()==FILEEXISTS_RESUME):FALSE);
}

void CMainFrame::OnUpdateOverwritemenuRename(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CFileExistsDlg::Always()?(CFileExistsDlg::GetAction()==FILEEXISTS_RENAME):FALSE);
}

void CMainFrame::OnUpdateOverwritemenuSkip(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CFileExistsDlg::Always()?(CFileExistsDlg::GetAction()==FILEEXISTS_SKIP):FALSE);
}

BOOL CMainFrame::RestoreSize()
{
	if (!COptions::GetOptionVal(OPTION_REMEMBERLASTWINDOWPOS))
	{
		CenterWindow();
		return FALSE;
	}

	CString str=COptions::GetOption(OPTION_LASTWINDOWPOS);
	WINDOWPLACEMENT placement;
	placement.length=sizeof(placement);

	int pos;
	pos=str.Find( _T(" ") );
	if (pos==-1)
	{
		CenterWindow();
		return FALSE;
	}
	m_PosData.state=_ttoi(str.Left(pos));
	str=str.Mid(pos+1);

	pos=str.Find( _T(" ") );
	if (pos==-1)
	{
		CenterWindow();
		return FALSE;
	}
	m_PosData.NormalRect.left=_ttoi(str.Left(pos));
	str=str.Mid(pos+1);

	pos=str.Find( _T(" ") );
	if (pos==-1)
	{
		CenterWindow();
		return FALSE;
	}
	m_PosData.NormalRect.top=_ttoi(str.Left(pos));
	str=str.Mid(pos+1);

	pos=str.Find( _T(" ") );
	if (pos==-1)
	{
		CenterWindow();
		return FALSE;
	}
	m_PosData.NormalRect.right=_ttoi(str.Left(pos));
	str=str.Mid(pos+1);

	if (str=="")
	{
		CenterWindow();
		return FALSE;
	}
	m_PosData.NormalRect.bottom=_ttoi(str);

	str=COptions::GetOption(OPTION_LASTSPLITTERSIZE);

	pos=str.Find( _T(" ") );
	if (pos==-1)
	{
		CenterWindow();
		return FALSE;
	}
	m_PosData.cy1=_ttoi(str.Left(pos));
	str=str.Mid(pos+1);

	pos=str.Find( _T(" ") );
	if (pos==-1)
	{
		CenterWindow();
		return FALSE;
	}
	m_PosData.cy2=_ttoi(str.Left(pos));
	str=str.Mid(pos+1);

	pos=str.Find( _T(" ") );
	if (pos==-1)
	{
		CenterWindow();
		return FALSE;
	}
	m_PosData.localPos = _ttoi(str.Left(pos));
	str=str.Mid(pos+1);

	pos=str.Find( _T(" ") );
	if (pos==-1)
	{
		CenterWindow();
		return FALSE;
	}
	m_PosData.remotePos = _ttoi(str.Left(pos));
	str=str.Mid(pos+1);

	if (str=="")
	{
		CenterWindow();
		return FALSE;
	}
	m_PosData.cx=_ttoi(str);



/* //Todo: Coordinate checking which is compatible with multi monitor systems
	if (placement.rcNormalPosition.left<0)
	{
		placement.rcNormalPosition.right+=placement.rcNormalPosition.left;
		placement.rcNormalPosition.left=0;
	}
	if (placement.rcNormalPosition.top<0)
	{
		placement.rcNormalPosition.bottom+=placement.rcNormalPosition.top;
		placement.rcNormalPosition.top=0;
	}
*/
	placement.flags = 0;
	placement.rcNormalPosition = m_PosData.NormalRect;
	placement.showCmd = (m_PosData.state == 1) ? SW_MAXIMIZE : SW_RESTORE;
	if (!SetWindowPlacement(&placement))
		return FALSE;
	m_wndVertSplitter.RecalcLayout();
	m_wndVertSplitter.SetRowInfo(0,m_PosData.cy1,0);
	m_wndVertSplitter.SetRowInfo(1,m_PosData.cy2,0);

	if (m_nLocalTreeViewLocation)
		m_wndLocalSplitter.SetColumnInfo(0,m_PosData.localPos,0);
	else
		m_wndLocalSplitter.SetRowInfo(0,m_PosData.localPos,0);
	m_wndLocalSplitter.RecalcLayout();
	if (m_nRemoteTreeViewLocation)
		m_wndRemoteSplitter.SetColumnInfo(0,m_PosData.remotePos,0);
	else
		m_wndRemoteSplitter.SetRowInfo(0,m_PosData.remotePos,0);
	m_wndRemoteSplitter.RecalcLayout();
	if (m_bSwitchPaneLayout)
	{
		int cx1, cx2, tmp;
		m_wndMainSplitter.GetColumnInfo(0, cx1, tmp);
		m_wndMainSplitter.GetColumnInfo(1, cx2, tmp);
		m_wndMainSplitter.SetColumnInfo(0, cx1 + cx2 - m_PosData.cx, 0);
	}
	else
		m_wndMainSplitter.SetColumnInfo(0, m_PosData.cx, 0);
	m_wndMainSplitter.RecalcLayout();
	m_wndVertSplitter.RecalcLayout();

	return TRUE;
}


#define VK_A		65
#define VK_C		67
#define VK_Q		81
#define VK_V		86
#define VK_X		88
#define VK_Z		90

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		if (pMsg->wParam==VK_TAB)
		{
			if (!(GetKeyState(VK_SHIFT)&128))
			{
				//Switching between controls using the tab key
				//Looks ugly but works
				CWnd *pCurrent=GetFocus();
				if (!pCurrent)
					if (m_bShowQuickconnect)
						m_pWndDlgBar->SetFocus();
					else
						m_pLocalViewHeader->m_pEdit->SetFocus();
				//Quickconnectbar has focus
				else if (pCurrent==m_pWndDlgBar->GetDlgItem(IDOK) || pCurrent==&m_wndQuickconnectButton)
					if (m_bShowMessageLog)
						GetStatusPane()->SetFocus();
					else
						m_pLocalViewHeader->m_pEdit->SetFocus();
				else if (pCurrent->GetParent()==m_pWndDlgBar)
					return CFrameWnd::PreTranslateMessage(pMsg);
				//Message log has focus
				else if (pCurrent==GetStatusPane()->GetEditCtrl())
					m_pLocalViewHeader->m_pEdit->SetFocus();
				//Local edit field has focus
				//Ftp combo box has focus
				else if (pCurrent==m_pLocalViewHeader->m_pEdit|| pCurrent->GetParent()==m_pLocalViewHeader->m_pEdit)
					if (m_bShowTree)
						GetLocalPane()->GetTreeCtrl()->SetFocus();
					else
						GetLocalPane2()->GetListCtrl()->SetFocus();
				//Treeview has focus
				else if (pCurrent==GetLocalPane()->GetTreeCtrl())
					GetLocalPane2()->GetListCtrl()->SetFocus();
				//Local List control has focus
				else if (pCurrent==GetLocalPane2()->GetListCtrl())
					if (m_pRemoteViewHeader->m_pEdit->IsWindowEnabled())
						m_pRemoteViewHeader->m_pEdit->SetFocus();
					else
						if (m_bShowQueue)
							GetQueuePane()->GetListCtrl()->SetFocus();
						else
							if (m_bShowQuickconnect)
								m_pWndDlgBar->SetFocus();
							else
								if (m_bShowMessageLog)
									GetStatusPane()->SetFocus();
								else
									m_pLocalViewHeader->m_pEdit->SetFocus();
				//Ftp combo box has focus
				else if (pCurrent==m_pRemoteViewHeader->m_pEdit|| pCurrent->GetParent()==m_pRemoteViewHeader->m_pEdit)
					if (m_pRemoteViewHeader->m_pEdit->IsWindowEnabled())
						if (m_bShowRemoteTree)
							GetFtpTreePane()->GetTreeCtrl()->SetFocus();
						else
							GetFtpPane()->GetListCtrl()->SetFocus();
					else
						if (m_bShowQueue)
							GetQueuePane()->GetListCtrl()->SetFocus();
						else
							if (m_bShowQuickconnect)
								m_pWndDlgBar->SetFocus();
							else
								if (m_bShowMessageLog)
									GetStatusPane()->SetFocus();
								else
									m_pLocalViewHeader->m_pEdit->SetFocus();
				//Ftp Tree control has focus
				else if (pCurrent==GetFtpTreePane()->GetTreeCtrl())
					if (m_pRemoteViewHeader->m_pEdit->IsWindowEnabled())
						GetFtpPane()->GetListCtrl()->SetFocus();
					else
						if (m_bShowQueue)
							GetQueuePane()->GetListCtrl()->SetFocus();
						else
							if (m_bShowQuickconnect)
								m_pWndDlgBar->SetFocus();
							else
								if (m_bShowMessageLog)
									GetStatusPane()->SetFocus();
								else
									m_pLocalViewHeader->m_pEdit->SetFocus();
				//Ftp Pane has focus
				else if (pCurrent==GetFtpPane()->GetListCtrl())
					if (m_bShowQueue)
						GetQueuePane()->GetListCtrl()->SetFocus();
					else
						if (m_bShowQuickconnect)
							m_pWndDlgBar->SetFocus();
						else
							if (m_bShowMessageLog)
								GetStatusPane()->SetFocus();
							else
								m_pLocalViewHeader->m_pEdit->SetFocus();
				//Queue has focus
				else if (pCurrent==GetQueuePane()->GetListCtrl())
					if (m_bShowQuickconnect)
						m_pWndDlgBar->SetFocus();
					else
						if (m_bShowMessageLog)
							GetStatusPane()->SetFocus();
						else
							m_pLocalViewHeader->m_pEdit->SetFocus();
				//No known window has focus
				else
					if (m_bShowQuickconnect)
						m_pWndDlgBar->SetFocus();
					else
						if (m_bShowMessageLog)
							GetStatusPane()->SetFocus();
						else
							m_pLocalViewHeader->m_pEdit->SetFocus();
				return TRUE;
			}
			else
			{
				//Switching between controls using the tab key
				//Looks ugly but works
				CWnd *pCurrent=GetFocus();
				if (!pCurrent)
					if (m_bShowQueue)
						GetQueuePane()->GetListCtrl()->SetFocus();
					else
						if (m_pRemoteViewHeader->m_pEdit->IsWindowEnabled())
							GetFtpPane()->GetListCtrl()->SetFocus();
						else
							GetLocalPane2()->GetListCtrl()->SetFocus();
				//Queue has focus
				else if (pCurrent==GetQueuePane()->GetListCtrl())
					if (m_pRemoteViewHeader->m_pEdit->IsWindowEnabled())
						GetFtpPane()->GetListCtrl()->SetFocus();
					else
						GetLocalPane2()->GetListCtrl()->SetFocus();
				//Ftp Pane has focus
				else if (pCurrent==GetFtpPane()->GetListCtrl())
					if (m_pRemoteViewHeader->m_pEdit->IsWindowEnabled())
						if (m_bShowRemoteTree)
							GetFtpTreePane()->GetTreeCtrl()->SetFocus();
						else
							m_pRemoteViewHeader->m_pEdit->SetFocus();
					else
						GetLocalPane2()->GetListCtrl()->SetFocus();
				//Ftp tree control has focus
				else if (pCurrent==GetFtpTreePane()->GetTreeCtrl())
					if (m_pRemoteViewHeader->m_pEdit->IsWindowEnabled())
						m_pRemoteViewHeader->m_pEdit->SetFocus();
					else
						GetLocalPane2()->GetListCtrl()->SetFocus();
				//Ftp combo box has focus
				else if (pCurrent==m_pRemoteViewHeader->m_pEdit|| pCurrent->GetParent()==m_pRemoteViewHeader->m_pEdit)
					GetLocalPane2()->GetListCtrl()->SetFocus();
				//Local List control has focus
				else if (pCurrent==GetLocalPane2()->GetListCtrl())
					if (m_bShowTree)
						GetLocalPane()->GetTreeCtrl()->SetFocus();
					else
						m_pLocalViewHeader->m_pEdit->SetFocus();
				//Treeview has focus
				else if (pCurrent==GetLocalPane()->GetTreeCtrl())
					m_pLocalViewHeader->m_pEdit->SetFocus();
				//Local edit field has focus
				else if (pCurrent==m_pLocalViewHeader->m_pEdit|| pCurrent->GetParent()==m_pLocalViewHeader->m_pEdit)
					if (m_bShowMessageLog)
						GetStatusPane()->SetFocus();
					else
						if (m_bShowQuickconnect)
							m_wndQuickconnectButton.SetFocus();
						else
							if (m_bShowQueue)
								GetQueuePane()->GetListCtrl()->SetFocus();
							else
								if (m_pRemoteViewHeader->m_pEdit->IsWindowEnabled())
									GetFtpPane()->GetListCtrl()->SetFocus();
								else
									GetLocalPane2()->GetListCtrl()->SetFocus();
				//Message log has focus
				else if (pCurrent==GetStatusPane()->GetEditCtrl())
					if (m_bShowQuickconnect)
						m_wndQuickconnectButton.SetFocus();
					else
						if (m_bShowQueue)
							GetQueuePane()->GetListCtrl()->SetFocus();
						else
							if (m_pRemoteViewHeader->m_pEdit->IsWindowEnabled())
								GetFtpPane()->GetListCtrl()->SetFocus();
							else
								GetLocalPane2()->GetListCtrl()->SetFocus();
				//Quickconnectbar has focus
				else if (pCurrent==m_pWndDlgBar->GetDlgItem(IDC_HOST))
					if (m_bShowQueue)
						GetQueuePane()->GetListCtrl()->SetFocus();
					else
						if (m_pRemoteViewHeader->m_pEdit->IsWindowEnabled())
							GetFtpPane()->GetListCtrl()->SetFocus();
						else
							GetLocalPane2()->GetListCtrl()->SetFocus();
				else if (pCurrent->GetParent()==m_pWndDlgBar)
					return CFrameWnd::PreTranslateMessage(pMsg);


				//No known window has focus
				else
					if (m_bShowQueue)
						GetQueuePane()->GetListCtrl()->SetFocus();
					else
						if (m_pRemoteViewHeader->m_pEdit->IsWindowEnabled())
							GetFtpPane()->GetListCtrl()->SetFocus();
						else
							GetLocalPane2()->GetListCtrl()->SetFocus();
				return TRUE;
			}
		}
		else if (GetKeyState(VK_CONTROL)&128 && GetKeyState(VK_SHIFT)&128)
		{
			if (pMsg->wParam==VK_Q && m_bShowQuickconnect)
				m_pWndDlgBar->SetFocus();
		}
		return CFrameWnd::PreTranslateMessage(pMsg);
	}

	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnMenuQueueExport()
{
	((CQueueCtrl *)GetQueuePane()->GetListCtrl())->Export();
}

void CMainFrame::OnUpdateMenuQueueExport(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!((CQueueCtrl *)GetQueuePane()->GetListCtrl())->IsQueueEmpty());
}

void CMainFrame::OnMenuQueueImport()
{
	((CQueueCtrl *)GetQueuePane()->GetListCtrl())->Import();
}

void CMainFrame::OnUpdateMenuQueueProcessnow(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(((CQueueCtrl *)GetQueuePane()->GetListCtrl())->DoProcessQueue());
	pCmdUI->Enable(!((CQueueCtrl *)GetQueuePane()->GetListCtrl())->IsQueueEmpty());
}

void CMainFrame::OnMenuQueueUseMultiple()
{
	((CQueueCtrl *)GetQueuePane()->GetListCtrl())->ToggleUseMultiple();
}

void CMainFrame::OnUpdateMenuQueueUseMultiple(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(((CQueueCtrl *)GetQueuePane()->GetListCtrl())->DoUseMultiple());
}

void CMainFrame::OnMenuQueueProcessnow()
{
	if (((CQueueCtrl *)GetQueuePane()->GetListCtrl())->DoProcessQueue())
		((CQueueCtrl *)GetQueuePane()->GetListCtrl())->StopProcessing();
	else
		TransferQueue(1);
}

void CMainFrame::OnMenuTransferManualtransfer()
{
	CManualTransferDlg dlg;
	t_transferfile transferfile;
	t_server *pServer=0;
	t_server server;
	if (m_pFileZillaApi->GetCurrentServer(server)==FZ_REPLY_OK)
		pServer=&server;

	int res=dlg.Show(transferfile, pServer, GetLocalPane2()->GetLocalFolder(), ((CFtpListCtrl *)GetFtpPane()->GetListCtrl())->GetCurrentDirectory());
	if (res)
	{
		transferfile.nType=m_nTransferType;
		if (!m_nTransferType)
		{
			transferfile.nType=2;
			CString AsciiFiles=COptions::GetOption(OPTION_ASCIIFILES);
			CString fn=transferfile.get?transferfile.remotefile:transferfile.localfile;
			fn.MakeUpper();
			int i;
			while ((i = AsciiFiles.Find( _T(";") )) != -1)
			{
				if ("." + AsciiFiles.Left(i) == fn.Right(AsciiFiles.Left(i).GetLength() + 1))
				{
					transferfile.nType = 1;
					break;
				}
				AsciiFiles = AsciiFiles.Mid(i + 1);
			}
		}
		GetQueuePane()->AddItem(transferfile, (res==2)?TRUE:FALSE, 0);
		if (res==2)
			TransferQueue(2);
	}

}

void CMainFrame::OnSysCommand( UINT nID, LPARAM lParam )
{
	if ((nID&0xFFF0)==SC_RESTORE && m_PosData.state)
	{
		WINDOWPLACEMENT placement;
		placement.length = sizeof(WINDOWPLACEMENT);
		placement.rcNormalPosition = m_PosData.NormalRect;
		if (m_bMaxBeforeTray)
			placement.showCmd = SW_SHOWMAXIMIZED;
		else
			placement.showCmd = SW_RESTORE;
		SetWindowPlacement(&placement);
		m_PosData.state = 0;
	}
	else
	{
		if (nID == SC_MINIMIZE || nID == SC_MINIMIZETRAY)
		{
			WINDOWPLACEMENT placement;
			BOOL res=GetWindowPlacement(&placement);
			if (placement.showCmd==SW_MAXIMIZE || placement.showCmd==SW_SHOWMAXIMIZED)
				m_bMaxBeforeTray = true;
			else
				m_bMaxBeforeTray = false;
		}
		CFrameWnd::OnSysCommand( nID, lParam );
	}
	if (nID == SC_MINIMIZE)
	{
		if (COptions::GetOptionVal(OPTION_MINIMIZETOTRAY) == 1)
		{
			ShowWindow(SW_HIDE);
			m_TrayIcon.AddIcon();
			m_TrayIcon.ShowIcon();
		}
	}
	else if (nID == SC_MINIMIZETRAY)
	{
		ShowWindow(SW_HIDE);
		m_TrayIcon.AddIcon();
		m_TrayIcon.ShowIcon();
	}
	else if (nID == SC_RESTORE)
	{
		if (m_bMaxBeforeTray)
			ShowWindow(SW_SHOWMAXIMIZED);
		else
			ShowWindow(SW_SHOW);
		m_bMaxBeforeTray = false;
		m_TrayIcon.HideIcon();
	}
}

LRESULT CMainFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pFileZillaApi && message==m_nFileZillaApiMessageID)
	{
		switch(FZ_MSG_ID(wParam))
		{
		case FZ_MSG_SOCKETSTATUS:
			switch(FZ_MSG_PARAM(wParam))
			{
			case FZ_SOCKETSTATUS_RECV:
				m_RecvLed.Ping(100);
				break;
			case FZ_SOCKETSTATUS_SEND:
				m_SendLed.Ping(100);
				break;
			}
			break;
		case FZ_MSG_LISTDATA:
			{
				t_directory *pDir=(t_directory *)lParam;
				if (pDir)
				{
					m_pCommandQueue->SetLastServerPath(pDir->path);
					reinterpret_cast<CRemoteComboCompletion *>(m_pRemoteViewHeader->m_pEdit)->SetServer(pDir->server);
					m_pRemoteViewHeader->m_pEdit->EnableWindow(TRUE);
					m_pRemoteViewHeader->m_pEdit->SetWindowText(pDir->path.GetPath());
					if (m_pRemoteViewHeader->m_pEdit->FindStringExact(-1,pDir->path.GetPath())==CB_ERR)
					{
						m_pRemoteViewHeader->m_pEdit->AddString(pDir->path.GetPath());
					}
					reinterpret_cast<CRemoteComboCompletion *>(m_pRemoteViewHeader->m_pEdit)->SetPath(pDir->path);
				}
				GetFtpPane()->List(pDir);
				GetFtpTreePane()->List(pDir);
				COptions::SetOption(OPTION_LASTSERVERPATH,((t_directory *)lParam)->path.GetSafePath());
				COptions::SetOption(OPTION_LASTSERVERTYPE,((t_directory *)lParam)->server.nServerType);
			}
			break;
		case FZ_MSG_REPLY:
			if (lParam&FZ_REPLY_DISCONNECTED)
			{
				GetFtpPane()->List(0);
				GetFtpTreePane()->List(0);
				m_pRemoteViewHeader->m_pEdit->SetWindowText(_T(""));
				m_pRemoteViewHeader->m_pEdit->EnableWindow(FALSE);
				SetProgress(0);
			}
			if (!m_bQuit)
				m_pCommandQueue->ProcessReply(lParam, FZ_MSG_PARAM(wParam));
			break;
		case FZ_MSG_SECURESERVER:
			if (m_pSecureIconCtrl)
				m_pSecureIconCtrl->ShowWindow(FZ_MSG_PARAM(wParam)?SW_SHOW:SW_HIDE);
			break;
		case FZ_MSG_ASYNCREQUEST:
			if (!m_bQuit)
			{
				CAsyncRequestData *pRequest = reinterpret_cast<CAsyncRequestData *>(lParam);
				if (pRequest && FZ_MSG_PARAM(wParam) == FZ_ASYNCREQUEST_OVERWRITE)
				{
					if (GetQueuePane()->GetListCtrl()->MayResumePrimaryTransfer())
					{
						m_pFileZillaApi->SetAsyncRequestResult(FILEEXISTS_RESUME, pRequest);
						break;
					}
				}
				CAsyncRequestQueue::ProcessRequest(pRequest, m_pFileZillaApi, FALSE);
			}
			break;
		case FZ_MSG_STATUS:
			{
				t_ffam_statusmessage *pStatus=(t_ffam_statusmessage *)lParam;
				if (!pStatus)
					break;
				GetStatusPane()->ShowStatus(pStatus->status, pStatus->type);
				if (pStatus->post)
					delete pStatus;
				else
					ASSERT(FALSE);
				break;
			}
			break;
		case FZ_MSG_TRANSFERSTATUS:
			{
				BOOL res=TRUE;
				t_ffam_transferstatus *status=(t_ffam_transferstatus *)lParam;
				if (status && status->bFileTransfer)
					res=GetQueuePane()->GetListCtrl()->SetProgress(0, status);
				SetProgress(status);
				if (status && (!status->bFileTransfer || !res) )
					delete status;
			}
			break;
		case FZ_MSG_QUITCOMPLETE:
			VERIFY(PostMessage(WM_CLOSE));
			break;
		}
		return TRUE;
	}
	else if (message==WM_APP)
	{
		if (!wParam || lParam)
			return CFrameWnd::DefWindowProc(message, wParam, lParam);

		if (wParam % 4)
			return CFrameWnd::DefWindowProc(message, wParam, lParam);

		CQueueData *pQueueData = reinterpret_cast<CQueueData *>(wParam);
		if (!pQueueData->transferFile.get)
			return CFrameWnd::DefWindowProc(message, wParam, lParam);
		if (pQueueData->nOpen != 1 && pQueueData->nOpen != 2)
			return CFrameWnd::DefWindowProc(message, wParam, lParam);
		pQueueData->transferFile.get = FALSE;

		CString text;
		text.Format(IDS_QUESTION_VIEWEDIT_MODIFIED, pQueueData->transferFile.localfile);
		CString caption;
		caption.LoadString(IDS_QUESTION_VIEWEDIT_MODIFIED_CAPTION);
		if (MessageBox(text, caption, MB_YESNO | MB_ICONQUESTION)==IDYES)
		{
			GetQueuePane()->AddItem(pQueueData->transferFile, TRUE, 3);
			TransferQueue(2);
		}
		delete pQueueData;
		return TRUE;
	}
	return CFrameWnd::DefWindowProc(message, wParam, lParam);
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent==m_nSecTimerID)
	{
		m_pCommandQueue->OnRetryTimer();
		return;
	}
	else if (nIDEvent==CMDLINE_CLOSE)
	{
		// command line said to close, so check if queue is empty & close if so
		if (!((CQueueCtrl *)GetQueuePane()->GetListCtrl())->DoProcessQueue())
		{
			KillTimer(nIDEvent);
			PostMessage(WM_CLOSE);
		}
	}
	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnMenuSiteChangepass()
{
	CServerChangePass dlg;
	if (dlg.DoModal()==IDOK)
	{
		if (!m_pCommandQueue->Command("SITE PSWD \""+dlg.m_Old+"\" \""+dlg.m_New+"\""))
			MessageBeep(MB_ICONEXCLAMATION);
	}

}

void CMainFrame::OnUpdateMenuSiteChangepass(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsConnected() && !m_pCommandQueue->IsBusy() );
}

//// SetupTrayIcon /////////////////////////////////////////////////////
// If we're minimized, create an icon in the systray.  Otherwise, remove
// the icon, if one is present.

void CMainFrame::SetupTrayIcon()
{
	m_TrayIcon.Create(0, nTrayNotificationMsg_, _T("FileZilla"),
		0, IDR_SYSTRAY_MENU, TRUE);
	m_TrayIcon.SetIcon( ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_TRAYICON)));
}


//// SetupTaskBarButton ////////////////////////////////////////////////
// Show or hide the taskbar button for this app, depending on whether
// we're minimized right now or not.

void CMainFrame::OnTrayExit()
{
	if (!m_bQuit)
		OnClose();
}

void CMainFrame::OnTrayRestore()
{
	ShowWindow(SW_RESTORE);
	if (m_bMaxBeforeTray)
		ShowWindow(SW_SHOWMAXIMIZED);
	else
		ShowWindow(SW_SHOW);
	m_bMaxBeforeTray = false;
	m_TrayIcon.HideIcon();
}

void CMainFrame::OnMenuViewShowhidden()
{
	m_bShowHiddenFiles=!m_bShowHiddenFiles;
	if (m_pFileZillaApi)
		m_pFileZillaApi->SetOption(FZAPI_OPTION_SHOWHIDDEN, m_bShowHiddenFiles);
	if (m_bShowHiddenFiles)
		RefreshViews(2);
}

void CMainFrame::OnUpdateMenuViewShowhidden(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowHiddenFiles);

}

void CMainFrame::OnShowremotetree()
{
	if (!m_bShowRemoteTree)
	{
		if (m_nRemoteTreeViewLocation)
			m_wndRemoteSplitter.ShowCol(0);
		else
			m_wndRemoteSplitter.ShowRow(0);
		m_bShowRemoteTree = TRUE;
		if (m_pRemoteViewHeader)
			m_pRemoteViewHeader->bTreeHidden = FALSE;
		GetFtpTreePane()->EnableTree(TRUE);
	}
	else
	{
		m_bShowRemoteTree = FALSE;
		if (m_nRemoteTreeViewLocation)
			m_wndRemoteSplitter.HideCol(0);
		else
			m_wndRemoteSplitter.HideRow(0);
		if (m_pRemoteViewHeader)
			m_pRemoteViewHeader->bTreeHidden = TRUE;
		GetFtpTreePane()->EnableTree(FALSE);
	}
	GetFtpTreePane()->UpdateViewHeader();
	GetFtpPane()->UpdateViewHeader();
}

void CMainFrame::OnUpdateShowremotetree(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowRemoteTree);
}

void CMainFrame::OnMenuDebugDumpDirectoryCache()
{
	CFileDialog dlg(FALSE, _T("*.txt"), _T("Directory Cache Dump.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text files (*.txt)|*.txt|All files (*.*)|*.*||"), this);
	if (dlg.DoModal()==IDOK)
		CFileZillaApi::DumpDirectoryCache(dlg.GetPathName());
}

void CMainFrame::OnMenuDebugCrash()
{
	//Do something really bad to trigger the exception handling
	char *p = 0; //So far so good
	*p= 0; //Boom!
}

void CMainFrame::OnMenuServerEnterrawcommand()
{
	if (!IsConnected())
		return;
	CEnterSomething dlg(IDS_INPUTDIALOGTITLE_FTPCOMMAND,IDS_INPUTDIALOGTEXT_FTPCOMMAND);
	int res=dlg.DoModal();
	if (res==IDOK)
	{
		CString command=dlg.m_String;
		CFrameWnd *pWnd=GetParentFrame();
		CMainFrame *pOwner=DYNAMIC_DOWNCAST(CMainFrame,pWnd);
		if (!m_pCommandQueue->Command(command))
			MessageBeep(MB_ICONEXCLAMATION);
	}
}

void CMainFrame::OnUpdateMenuServerEnterrawcommand(CCmdUI* pCmdUI)
{
	if (IsConnected())
	{
		t_server server;
		if (m_pFileZillaApi->GetCurrentServer(server)==FZ_REPLY_OK)
			if (!(server.nServerType&FZ_SERVERTYPE_SUB_FTP_SFTP))
			{
				pCmdUI->Enable(TRUE);
				return;
			}
	}
	pCmdUI->Enable(FALSE);
}

void CMainFrame::ParseSite(t_SiteManagerItem &site)
{
	CString Host, User, Pass, nPort;
	Host = site.Host;
	nPort.Format( _T("%d"), site.nPort);
	User=site.User;
	Pass=site.Pass;

	if (site.nLogonType == 0)
	{
		User = "anonymous";
		Pass = COptions::GetOption(OPTION_ANONPWD);
	}

	if (Host=="")
	{
		AfxMessageBox(IDS_ERRORMSG_HOSTNEEDED, MB_ICONEXCLAMATION);
		return;
	}

	int pos=Host.Find(':');
	if (pos!=-1)
	{
		int pos2=Host.Find('/');
		if (pos2!=-1 && pos2==(pos+1))
		{
			int pos3=Host.Find('/',pos2+1);
			if (pos3!=-1 && pos3==(pos2+1))
			{
				CString tmp=Host.Left(pos+3);
				tmp.MakeLower();
				if (tmp!="ftp://")
				{
					AfxMessageBox(IDS_ERRORMSG_URLSYNTAXINVALID,MB_ICONEXCLAMATION);
					return;
				}
				else
				{
					Host=Host.Right(Host.GetLength()-pos-3);
				}
			}
		}
	}
	pos=Host.Find('/');
	CString path="";
	if (pos!=-1)
	{
		if ( Host.GetLength()>(pos+1) && Host[pos+1]==' ' )
			path=Host.Right(Host.GetLength()-pos);
		else
			path=Host.Right(Host.GetLength()-pos-1);
		Host=Host.Left(pos);
		if (Host=="")
		{
			AfxMessageBox(IDS_ERRORMSG_URLSYNTAXINVALID,MB_ICONEXCLAMATION);
			return;
		}
	}

	pos=Host.ReverseFind('@');
	if (pos!=-1)
	{
		CString tmp=Host.Left(pos);
		int pos2=tmp.Find(':');
		if (pos2==-1)
		{
			AfxMessageBox(IDS_ERRORMSG_URLSYNTAXINVALID,MB_ICONEXCLAMATION);
			return;
		}
		Host=Host.Right(Host.GetLength()-pos-1);
		User=tmp.Left(pos2);
		Pass=tmp.Right(tmp.GetLength()-pos2-1);

	}

	int pos2 = Host.ReverseFind(']');
	pos = Host.ReverseFind(':');
	if (pos > pos2)
	{
		nPort = Host.Right(Host.GetLength()-pos-1);
		Host = Host.Left(pos);
	}
	
	if (Host[0] == '[' && Host[Host.GetLength() - 1] == ']')
	{
		Host = Host.Mid(1, Host.GetLength() - 2);
	}

	if (nPort == "")
	{
		nPort = "21";
	}
	else
	{
		if (!_ttoi(nPort))
		{
			AfxMessageBox(IDS_ERRORMSG_INVALIDPORT,MB_ICONEXCLAMATION);
			return;
		}
	}
	if (User == "")
	{
		User="anonymous";
		Pass=COptions::GetOption(OPTION_ANONPWD);
	}

	t_server server;
	server.host=Host;
	server.port=_ttoi(nPort);
	server.user=User;
	server.account = site.Account;
	server.pass=Pass;
	server.path=site.RemoteDir;
	server.fwbypass=site.bFirewallBypass;
	server.bDontRememberPass=site.bPassDontSave;
	server.name = site.name;
	server.nPasv = site.nPasv;
	server.nTimeZoneOffset = site.nTimeZoneOffset * 60 + site.nTimeZoneOffset2;
	server.nUTF8 = site.nUTF8;
	switch (site.nServerType)
	{
	case 0:
		server.nServerType = FZ_SERVERTYPE_FTP;
		break;
	case 1:
		server.nServerType = FZ_SERVERTYPE_FTP|FZ_SERVERTYPE_LAYER_SSL_IMPLICIT;
		break;
	case 2:
		server.nServerType = FZ_SERVERTYPE_FTP|FZ_SERVERTYPE_LAYER_SSL_EXPLICIT;
		break;
	case 3:
		server.nServerType = FZ_SERVERTYPE_FTP|FZ_SERVERTYPE_SUB_FTP_SFTP;
		break;
	case 4:
		server.nServerType = FZ_SERVERTYPE_FTP|FZ_SERVERTYPE_LAYER_TLS_EXPLICIT;
		break;
	default:
		server.nServerType = FZ_SERVERTYPE_FTP;
	}

	t_server currentserver;
	if (site.RemoteDir!="" && m_pFileZillaApi->GetCurrentServer(currentserver) == FZ_REPLY_OK &&
		server==currentserver && !m_pCommandQueue->IsBusy())
	{
		if (m_pCommandQueue->List(site.RemoteDir, FZ_LIST_USECACHE))
		{
			if (site.LocalDir!="")
				SetLocalFolder(site.LocalDir);
			return;
		}
	}
	if (m_pCommandQueue->IsBusy() || IsConnected())
	{
		int res=AfxMessageBox(IDS_QUESTION_BREAKCONNECTION,MB_YESNO|MB_ICONQUESTION);
		if (res!=IDYES)
			return;
		m_pCommandQueue->Cancel();
		m_pCommandQueue->Disconnect();
	}

	if (site.LocalDir!="")
		SetLocalFolder(site.LocalDir);
	m_pCommandQueue->Connect(server);
	if (site.RemoteDir!="")
		m_pCommandQueue->List(site.RemoteDir,FZ_LIST_USECACHE,TRUE);
	else
		m_pCommandQueue->List(FZ_LIST_USECACHE,TRUE);
}

void CMainFrame::OnQuickconnectBarMenuClear()
{
	m_pWndDlgBar->SetDlgItemText(IDC_HOST, _T(""));
	m_pWndDlgBar->SetDlgItemText(IDC_PORT, _T(""));
	m_pWndDlgBar->SetDlgItemText(IDC_USER, _T(""));
	m_pWndDlgBar->SetDlgItemText(IDC_PASS, _T(""));
	m_pWndDlgBar->RedrawWindow();
}

void CMainFrame::OnQuickconnectBarMenuEmptyHistory()
{
	for (int i = m_RecentQuickconnectServers.size() + 3; i > 2; i--)
		VERIFY(m_wndQuickconnectButton.RemoveMenuItem(i));
	m_RecentQuickconnectServers.clear();
	m_nRecentQuickconnectCommandOffset=0;

	SaveMostRecentServers();
}

void CMainFrame::OnQuickconnectBarMenuBypass()
{
	ProcessQuickconnect(TRUE);
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam)>=ID_QUICKCONNECTBAR_MENU_HISTORY1 && LOWORD(wParam)<=ID_QUICKCONNECTBAR_MENU_HISTORY10)
	{
		int nIndex=m_nRecentQuickconnectCommandOffset-LOWORD(wParam)+ID_QUICKCONNECTBAR_MENU_HISTORY1-1;
		if (nIndex<0)
			nIndex+=10;
		for (std::list<t_server>::iterator iter=m_RecentQuickconnectServers.begin(); iter!=m_RecentQuickconnectServers.end(); iter++, nIndex--)
		{
			if (!nIndex)
			{
				if (m_pCommandQueue->IsBusy() || IsConnected())
				{
					int res=AfxMessageBox(IDS_QUESTION_BREAKCONNECTION, MB_YESNO|MB_ICONQUESTION);
					if (res!=IDYES)
						return TRUE;

					if ((_ttoi(COptions::GetOption(OPTION_RUNINSECUREMODE)) || iter->bDontRememberPass) && iter->pass=="")
					{
						CEnterSomething dlg(IDS_INPUTDIALOGTITLE_INPUTPASSWORD,IDS_INPUTDIALOGTEXT_INPUTPASSWORD,'*');
						if (dlg.DoModal()!=IDOK)
							return TRUE;
						iter->pass = dlg.m_String;
					}
					m_pCommandQueue->Cancel();
					m_pCommandQueue->Disconnect();
				}
				else
				{
					if ((_ttoi(COptions::GetOption(OPTION_RUNINSECUREMODE)) || iter->bDontRememberPass) && iter->pass=="")
					{
						CEnterSomething dlg(IDS_INPUTDIALOGTITLE_INPUTPASSWORD,IDS_INPUTDIALOGTEXT_INPUTPASSWORD,'*');
						if (dlg.DoModal()!=IDOK)
							return TRUE;
						iter->pass = dlg.m_String;
					}
				}
				m_pCommandQueue->Connect(*iter);
				m_pCommandQueue->List(FZ_LIST_USECACHE, TRUE);
				return TRUE;
			}
		}
		return TRUE;
	}

	return CFrameWnd::OnCommand(wParam, lParam);
}

void CMainFrame::OnMenuServerCopyurltoclipboard()
{
	t_server server;
	if (m_pFileZillaApi->GetCurrentServer(server)==FZ_REPLY_OK)
	{
		CString str=server.host;
		CString tmp;
		if (server.user.CollateNoCase(_T("anonymous")))
			str=server.user + _T("@") + str;
		if (server.nServerType&FZ_SERVERTYPE_LAYER_SSL_IMPLICIT)
		{
			if (server.port!=21)
				tmp.Format(_T("%d"), server.port);
			str=_T("ftps://")+str;
		}
		else if (server.nServerType&FZ_SERVERTYPE_SUB_FTP_SFTP)
		{
			if (server.port!=22)
				tmp.Format(_T("%d"), server.port);
			str=_T("sftp://")+str;
		}
		else if (server.port!=21)
			tmp.Format(_T("%d"), server.port);

		if (tmp!="")
			str+=_T(":")+tmp;
		CString path=((CFtpListCtrl *)GetFtpPane()->GetListCtrl())->GetCurrentDirectory().GetPath();
		if (path.Left(1)!=_T("/") && path!=_T(""))
			str+=_T("/");
		str+=path;

		OpenClipboard();
		EmptyClipboard();
		HGLOBAL hData;

		hData = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, str.GetLength()+1);
		LPVOID pData=GlobalLock(hData);
		USES_CONVERSION;
		strcpy((LPSTR)pData, T2CA(str));
		GlobalUnlock(hData);
		SetClipboardData(CF_TEXT,hData);
		CloseClipboard();
	}
}

void CMainFrame::OnUpdateMenuServerCopyurltoclipboard(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsConnected());
}


void CMainFrame::OnToolbarDropDown(NMHDR *pnmtb, LRESULT *plr)
{
	NMTOOLBAR* nmtb=reinterpret_cast<NMTOOLBAR *>(pnmtb);
	if ( nmtb->iItem != ID_SITEMANAGER)
		return;

	CWnd *pWnd;

	pWnd = m_pWndToolBar;

	// load and display popup menu
	CMenu menu;
	menu.CreatePopupMenu();
	CMenuTree mt;
	mt.m_CurrentItem = 10;

	CMarkupSTL *pMarkup;
	if (COptions::LockXML(&pMarkup))
	{
		pMarkup->ResetPos();
		if (pMarkup->FindChildElem( _T("Sites") ))
		{
			pMarkup->IntoElem();
			CSiteManager::ReadTreeXML( pMarkup, menu.m_hMenu, ( LPVOID)&mt, NULL, NewItem_Menu, NewFolder_Menu, After_Menu);
		}
		else
			CSiteManager::ReadTree( _T(""), menu.m_hMenu, ( LPVOID)&mt, NULL, NewItem_Menu, NewFolder_Menu, After_Menu);

		COptions::UnlockXML();
	}
	else
		CSiteManager::ReadTree( _T(""), menu.m_hMenu, ( LPVOID)&mt, NULL, NewItem_Menu, NewFolder_Menu, After_Menu);
	//CSiteManager::ReadTree( _T(""), menu.m_hMenu, ( LPVOID)&mt, NULL, NewItem_Menu, NewFolder_Menu, After_Menu);
	CMenu* pPopup = &menu;//.GetSubMenu(0);
	ASSERT(pPopup);

	CRect rc;
	pWnd->SendMessage(TB_GETRECT, nmtb->iItem, (LPARAM)&rc);
	pWnd->ClientToScreen(&rc);

	UINT res = pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL | TPM_RETURNCMD | TPM_NONOTIFY,
		rc.left, rc.bottom, this, &rc);

	if ( res > 0)
	{
		t_SiteManagerItem *site;

		int i;
		std::list<t_SiteManagerItem *>::iterator iter=mt.m_Sites.begin();
		for (i=(res-10); i>0; i--)
			iter++;

		site=*iter;

		if (site)
		{
			CSiteManager::ConnectSitePrepare(site);

			ParseSite(*site);
		}
	}
}

void CMainFrame::ProcessQuickconnect(BOOL bBypass)
{
	CString m_Host,m_User,m_Pass,m_Port;
	CString ServerPrefix;
	m_pWndDlgBar->GetDlgItemText(IDC_HOST,m_Host);
	m_pWndDlgBar->GetDlgItemText(IDC_USER,m_User);
	m_pWndDlgBar->GetDlgItemText(IDC_PASS,m_Pass);
	m_pWndDlgBar->GetDlgItemText(IDC_PORT,m_Port);

	m_Host.TrimLeft(_T(" "));
	t_server server;
	server.nServerType=0;
	if (m_Host=="")
	{
		m_pWndDlgBar->RedrawWindow();
		AfxMessageBox(IDS_ERRORMSG_HOSTNEEDED,MB_ICONEXCLAMATION);
		m_pWndDlgBar->GetDlgItem(IDC_HOST)->SetFocus();
		return;
	}
	int pos=m_Host.Find( _T("://") );
	if (pos!=-1)
	{
		CString tmp=m_Host.Left(pos+3);
		tmp.MakeLower();
		if (tmp==_T("ftp://"))
		{
			m_Host=m_Host.Right(m_Host.GetLength()-pos-3);
			m_pWndDlgBar->SetDlgItemText(IDC_HOST,m_Host);
			server.nServerType=FZ_SERVERTYPE_FTP;
			pos=0;
		}
		else if (tmp==_T("ftps://") )
		{
			m_Host=m_Host.Right(m_Host.GetLength()-pos-3);
			m_pWndDlgBar->SetDlgItemText(IDC_HOST,m_Host);
			ServerPrefix=_T("ftps://");
			m_pWndDlgBar->SetDlgItemText(IDC_HOST,ServerPrefix+m_Host);
			server.nServerType=FZ_SERVERTYPE_FTP|FZ_SERVERTYPE_LAYER_SSL_IMPLICIT;
		}
		else if (tmp==_T("sftp://") )
		{
			m_Host=m_Host.Right(m_Host.GetLength()-pos-3);
			m_pWndDlgBar->SetDlgItemText(IDC_HOST,m_Host);
			ServerPrefix=_T("sftp://");
			m_pWndDlgBar->SetDlgItemText(IDC_HOST,ServerPrefix+m_Host);
			server.nServerType=FZ_SERVERTYPE_FTP | FZ_SERVERTYPE_SUB_FTP_SFTP;
		}
		else
		{
			m_pWndDlgBar->RedrawWindow();
			AfxMessageBox(IDS_ERRORMSG_URLSYNTAXINVALID, MB_ICONEXCLAMATION);
			m_pWndDlgBar->GetDlgItem(IDC_HOST)->SetFocus();
			return;
		}
	}
	pos = m_Host.Find('/');
	CString path = _T("");
	if (pos != -1)
	{
		if ( m_Host.GetLength()>(pos+1) && m_Host[pos+1]==' ')
			path=m_Host.Right(m_Host.GetLength()-pos);
		else
			path=m_Host.Right(m_Host.GetLength()-pos-1);
		m_Host=m_Host.Left(pos);
		if (m_Host=="")
		{
			m_pWndDlgBar->RedrawWindow();
			AfxMessageBox(IDS_ERRORMSG_URLSYNTAXINVALID, MB_ICONEXCLAMATION);
			m_pWndDlgBar->GetDlgItem(IDC_HOST)->SetFocus();
			return;
		}
		m_pWndDlgBar->SetDlgItemText(IDC_HOST, ServerPrefix + m_Host);
	}
	pos=m_Host.ReverseFind('@');
	if (pos!=-1)
	{
		CString tmp = m_Host.Left(pos);
		int pos2 = tmp.Find(':');
		if (pos2 != -1)
		{
			m_User = tmp.Left(pos2);
			m_Pass = tmp.Right(tmp.GetLength()-pos2-1);
		}
		else
		{
			m_User = tmp.Left(pos);
			if (m_Pass == _T(""))
			{
				CEnterSomething dlg(IDS_INPUTDIALOGTITLE_INPUTPASSWORD, IDS_INPUTDIALOGTEXT_INPUTPASSWORD, '*');
				if (dlg.DoModal()==IDOK)
					m_Pass = dlg.m_String;
				else
					return;
			}
		}
		m_Host = m_Host.Right(m_Host.GetLength()-pos-1);
		m_pWndDlgBar->SetDlgItemText(IDC_USER,m_User);
		m_pWndDlgBar->SetDlgItemText(IDC_PASS,m_Pass);
		m_pWndDlgBar->SetDlgItemText(IDC_HOST,ServerPrefix+m_Host);
	}

	int pos2 = m_Host.ReverseFind(']');
	pos = m_Host.ReverseFind(':');
	if (pos > pos2)
	{
		m_Port=m_Host.Right(m_Host.GetLength()-pos-1);
		m_pWndDlgBar->SetDlgItemText(IDC_PORT,m_Port);
		m_Host=m_Host.Left(pos);
		m_pWndDlgBar->SetDlgItemText(IDC_HOST,ServerPrefix+m_Host);
	}

	if (m_Host[0] == '[' && m_Host[m_Host.GetLength() - 1] == ']')
	{
		m_Host = m_Host.Mid(1, m_Host.GetLength() - 2);
	}

	if (m_Port=="")
	{
		if (server.nServerType & FZ_SERVERTYPE_SUB_FTP_SFTP)
			m_Port = "22";
		else if (server.nServerType & FZ_SERVERTYPE_LAYER_SSL_IMPLICIT)
			m_Port = "990";
		else
			m_Port = "21";
		m_pWndDlgBar->SetDlgItemText(IDC_PORT, m_Port);
	}
	else
	{
		if (!_ttoi(m_Port))
		{
			m_pWndDlgBar->RedrawWindow();
			AfxMessageBox(IDS_ERRORMSG_PORTNEEDED, MB_ICONEXCLAMATION);
			m_pWndDlgBar->GetDlgItem(IDC_PORT)->SetFocus();
			return;
		}
	}
	if (m_User=="")
	{
		BOOL bUseGSS=FALSE;
		if (COptions::GetOptionVal(OPTION_USEGSS))
		{
			USES_CONVERSION;

			CString GssServers=COptions::GetOption(OPTION_GSSSERVERS);
			hostent *fullname=gethostbyname(T2CA(m_Host));
			CString host;
			if (fullname)
				host=fullname->h_name;
			else
				host=m_Host;
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
		if (!bUseGSS)
		{
			m_User="anonymous";
			m_Pass=COptions::GetOption(OPTION_ANONPWD);
		}
		else
		{
			m_User="";
			m_Pass="";
		}
		m_pWndDlgBar->SetDlgItemText(IDC_USER,m_User);
		m_pWndDlgBar->SetDlgItemText(IDC_PASS,m_Pass);

	}
	if (!server.nServerType)
	{
		server.nServerType=FZ_SERVERTYPE_FTP;
		if (_ttoi(m_Port)==990)
			server.nServerType|=FZ_SERVERTYPE_LAYER_SSL_IMPLICIT;
	}

	if (m_pCommandQueue->IsBusy() || IsConnected())
	{
		m_pWndDlgBar->RedrawWindow();
		int res=AfxMessageBox(IDS_QUESTION_BREAKCONNECTION,MB_YESNO|MB_ICONQUESTION);
		if (res!=IDYES)
			return;
		m_pCommandQueue->Cancel();
		m_pCommandQueue->Disconnect();
	}
	m_Host.Trim();
	server.host=m_Host;
	server.user=m_User;
	server.pass=m_Pass;
	server.port=_ttoi(m_Port);
	server.path=path;
	server.fwbypass=bBypass;
	server.bDontRememberPass=FALSE;
	m_pCommandQueue->Connect(server);
	if (path!="")
		m_pCommandQueue->List(path,FZ_LIST_USECACHE,TRUE);
	else
		m_pCommandQueue->List(FZ_LIST_USECACHE,TRUE);

	m_pWndDlgBar->RedrawWindow();

	CString str=server.host;
	CString tmp;
	if (server.user.CollateNoCase(_T("anonymous")))
		str=server.user + _T("@") + str;
	if (server.nServerType&FZ_SERVERTYPE_LAYER_SSL_IMPLICIT)
	{
		if (server.port!=21)
			tmp.Format(_T("%d"), server.port);
		str=_T("ftps://")+str;
	}
	else if (server.nServerType&FZ_SERVERTYPE_SUB_FTP_SFTP)
	{
		if (server.port!=22)
			tmp.Format(_T("%d"), server.port);
		str=_T("sftp://")+str;
	}
	else if (server.port!=21)
		tmp.Format(_T("%d"), server.port);

	if (tmp!="")
		str+=_T(":")+tmp;

	for (std::list<t_server>::iterator iter=m_RecentQuickconnectServers.begin(); iter!=m_RecentQuickconnectServers.end(); iter++)
	{
		CString str2=iter->host;
		CString tmp;
		if (iter->user.CollateNoCase(_T("anonymous")))
			str2=iter->user + _T("@") + str2;
		if (iter->nServerType&FZ_SERVERTYPE_LAYER_SSL_IMPLICIT)
		{
			if (iter->port!=21)
				tmp.Format(_T("%d"), iter->port);
			str2=_T("ftps://")+str2;
		}
		else if (iter->nServerType&FZ_SERVERTYPE_SUB_FTP_SFTP)
		{
			if (iter->port!=22)
				tmp.Format(_T("%d"), iter->port);
			str2=_T("sftp://")+str2;
		}
		else if (iter->port!=21)
			tmp.Format(_T("%d"), iter->port);

		if (tmp!="")
			str2+=_T(":")+tmp;

		if (!str.CollateNoCase(str2))
		{
			*iter=server;
			return;
		}
	}
	if (!m_RecentQuickconnectServers.size())
		m_wndQuickconnectButton.AddMenuItem(0, MF_SEPARATOR);
	if (m_RecentQuickconnectServers.size()==10)
	{
		VERIFY(m_wndQuickconnectButton.RemoveMenuItem(13));
		m_RecentQuickconnectServers.pop_back();
	}
	m_RecentQuickconnectServers.push_front(server);
	m_wndQuickconnectButton.InsertMenuItem(4, ID_QUICKCONNECTBAR_MENU_HISTORY1+m_nRecentQuickconnectCommandOffset++, str, 0);
	m_nRecentQuickconnectCommandOffset%=10;
	SaveMostRecentServers();
}

void CMainFrame::OnMenuViewRemotelistviewStatusbar()
{
	GetFtpPane()->EnableStatusbar(!GetFtpPane()->IsStatusbarEnabled());
}

void CMainFrame::OnMenuViewLocallistviewStatusbar()
{
	GetLocalPane2()->EnableStatusbar(!GetLocalPane2()->IsStatusbarEnabled());
}

void CMainFrame::OnUpdateMenuViewRemotelistviewStatusbar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetFtpPane()->IsStatusbarEnabled());
}

void CMainFrame::OnUpdateMenuViewLocallistviewStatusbar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetLocalPane2()->IsStatusbarEnabled());
}

void CMainFrame::OnViewRemotelistviewOwnergroup()
{
	m_nHideRemoteColumns ^= 0x20;
	GetFtpPane()->m_pListCtrl->UpdateColumns(m_nHideRemoteColumns);
}

void CMainFrame::OnUpdateViewRemotelistviewOwnergroup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_nRemoteListViewStyle);
	pCmdUI->SetCheck(!(m_nHideRemoteColumns&0x20));
}

BOOL CMainFrame::CreateToolbars()
{
	const UINT ButtonIDs[] = {
		ID_SITEMANAGER,
		ID_SEPARATOR,
		ID_SHOWTREE,
		ID_SHOWREMOTETREE,
		ID_SHOWQUEUE,
		ID_VIEW_MESSAGELOG,
		ID_SEPARATOR,
		ID_TOOLBAR_REFRESH,
		ID_PROCESS_QUEUE,
		ID_CANCELBUTTON,
		ID_TOOLBAR_DISCONNECT,
		ID_TOOLBAR_RECONNECT,
		ID_SEPARATOR,
		ID_HELPMENU_CONTENTS
	};
	m_pWndToolBar = new CToolBar;

	if (!m_pWndToolBar->CreateEx(this, 0, WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP, CRect(2, 2, 0, 0)))
	{
		TRACE0("Symbolleiste konnte nicht erstellt werden\n");
		return FALSE;      // Fehler bei Erstellung
	}
	SIZE sizeButton, sizeImage;
	sizeButton.cx = 23;
	sizeButton.cy = 23;
	sizeImage.cx = 16;
	sizeImage.cy = 16;
	m_pWndToolBar->SetSizes(sizeButton, sizeImage);
	if (!m_pWndToolBar->LoadBitmap(IDR_MAINFRAME) ||
		!m_pWndToolBar->SetButtons(ButtonIDs, sizeof(ButtonIDs) / sizeof(UINT)))
	{
		TRACE0("Symbolleiste konnte nicht erstellt werden\n");
		return FALSE;      // Fehler bei Erstellung
	}

	int style = TBSTYLE_FLAT;
	CVisualStylesXP xp;
	if (xp.IsAppThemed())
		style |= TBSTYLE_TRANSPARENT;
	m_pWndToolBar->ModifyStyle(0, style);
	m_pWndToolBar->GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	DWORD dwStyle = m_pWndToolBar->GetButtonStyle(m_pWndToolBar->CommandToIndex(ID_SITEMANAGER));
	dwStyle |= TBSTYLE_DROPDOWN;
	m_pWndToolBar->SetButtonStyle(m_pWndToolBar->CommandToIndex(ID_SITEMANAGER), dwStyle);

	m_pWndDlgBar = new CTransparentDialogBar;
	if (!m_pWndDlgBar->Create(this, IDR_MAINFRAME,
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Dialogleiste konnte nicht erstellt werden\n");
		return FALSE;		// Fehler bei Erstellung
	}

	m_pWndReBar = new CReBar;
	if (!m_pWndReBar->Create(this, RBS_BANDBORDERS, WS_CHILD | WS_VISIBLE | CBRS_TOP) ||
		!m_pWndReBar->AddBar(m_pWndToolBar) ||
		!m_pWndReBar->AddBar(m_pWndDlgBar))
	{
		TRACE0("Infoleiste konnte nicht erstellt werden\n");
		return FALSE;      // Fehler bei Erstellung
	}
	m_pWndReBar->GetReBarCtrl().MinimizeBand(0);
	CWnd *pButton = m_pWndDlgBar->GetDlgItem(IDOK);
	pButton->ShowWindow(SW_HIDE);

	m_pWndDlgBar->ShowWindow(m_bShowQuickconnect?SW_SHOW:SW_HIDE);
	RecalcLayout();

	return TRUE;
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_INACTIVE)
		m_hLastFocus = ::GetFocus();
	else
		::SetFocus(m_hLastFocus);
}

void CMainFrame::OnUpdateMenuViewLocalSortby(CCmdUI* pCmdUI)
{
	CLocalFileListCtrl *pFileList = reinterpret_cast<CLocalFileListCtrl *>(GetLocalPane2()->GetListCtrl());
	int column;
	int dir;
	pFileList->GetSortInfo(column, dir);
	switch (pCmdUI->m_nID)
	{
	case ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_FILENAME:
		pCmdUI->SetCheck(column == 0);
		break;
	case ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_FILESIZE:
		pCmdUI->SetCheck(column == 1);
		break;
	case ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_FILETYPE:
		pCmdUI->SetCheck(column == 2);
		break;
	case ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_LASTMODIFIED:
		pCmdUI->SetCheck(column == 3);
		break;
	case ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_ASCENDING:
		pCmdUI->SetCheck(dir == 1);
		break;
	case ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_DESCENDING:
		pCmdUI->SetCheck(dir == 2);
		break;
	}
}

void CMainFrame::OnMenuViewLocalSortby(UINT nID)
{
	CLocalFileListCtrl *pFileList = reinterpret_cast<CLocalFileListCtrl *>(GetLocalPane2()->GetListCtrl());
	switch (nID)
	{
	case ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_FILENAME:
		pFileList->SortList(0, -1);
		break;
	case ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_FILESIZE:
		pFileList->SortList(1, -1);
		break;
	case ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_FILETYPE:
		pFileList->SortList(2, -1);
		break;
	case ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_LASTMODIFIED:
		pFileList->SortList(3, -1);
		break;
	case ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_ASCENDING:
		pFileList->SortList(-1, 1);
		break;
	case ID_MENU_VIEW_LOCALLISTVIEW_SORTBY_DESCENDING:
		pFileList->SortList(-1, 2);
		break;
	}
}

void CMainFrame::OnUpdateMenuViewRemoteSortby(CCmdUI* pCmdUI)
{
	CFtpListCtrl *pFileList = reinterpret_cast<CFtpListCtrl *>(GetFtpPane()->GetListCtrl());
	int column;
	int dir;
	pFileList->GetSortInfo(column, dir);
	switch (pCmdUI->m_nID)
	{
	case ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_FILENAME:
		pCmdUI->SetCheck(column == 0);
		break;
	case ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_FILESIZE:
		pCmdUI->SetCheck(column == 1);
		break;
	case ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_FILETYPE:
		pCmdUI->SetCheck(column == 2);
		break;
	case ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_DATETIME:
		pCmdUI->SetCheck(column == 3);
		break;
	case ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_ASCENDING:
		pCmdUI->SetCheck(dir == 1);
		break;
	case ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_DESCENDING:
		pCmdUI->SetCheck(dir == 2);
		break;
	}
}

void CMainFrame::OnMenuViewRemoteSortby(UINT nID)
{
	CFtpListCtrl *pFileList = reinterpret_cast<CFtpListCtrl *>(GetFtpPane()->GetListCtrl());
	switch (nID)
	{
	case ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_FILENAME:
		pFileList->SortList(0, -1);
		break;
	case ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_FILESIZE:
		pFileList->SortList(1, -1);
		break;
	case ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_FILETYPE:
		pFileList->SortList(2, -1);
		break;
	case ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_DATETIME:
		pFileList->SortList(3, -1);
		break;
	case ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_ASCENDING:
		pFileList->SortList(-1, 1);
		break;
	case ID_MENU_VIEW_REMOTELISTVIEW_SORTBY_DESCENDING:
		pFileList->SortList(-1, 2);
		break;
	}
}


void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	if (!bActive)
		return;

	CLocalView2* pView = GetLocalPane2();
	if (!pView)
		return;

	pView->CheckAutoRefresh();
}

bool CMainFrame::IsConnected() const
{
	return m_pCommandQueue && m_pCommandQueue->IsConnected();
}

void CMainFrame::UploadDir(CString dir, CString subdir, BOOL upload, const CString& user /*=_T("")*/, const CString& pass /*=_T("")*/)
{
	while (dir.Replace(_T("\\\\"), _T("\\")));
	CFileFind find;
	BOOL res = find.FindFile(dir);
	BOOL bDirIsEmpty = res;
	while (res)
	{
		res = find.FindNextFile();
		if (find.IsDirectory())
		{
			if (!find.IsDots())
			{
				bDirIsEmpty = FALSE;
				UploadDir(find.GetFilePath()+"\\*.*", subdir+find.GetFileName()+"\\", upload);
			}
		}
		else
		{
			bDirIsEmpty = FALSE;

			CString sdir="";
			CString path=subdir;
			path.TrimLeft( _T("\\") );
			path.TrimRight(_T("\\") );
			while (path!=_T("") )
			{
				CString tmp;
				int pos=path.Find( _T("\\") );
				if (pos==-1)
				{
					tmp.Format(_T(" %d %s"), path.GetLength(), path);
					path="";
				}
				else
				{
					tmp.Format(_T(" %d %s"), path.Left(pos).GetLength(), path.Left(pos));
					path=path.Mid(pos+1);
				}
				sdir+=tmp;
			}
			sdir.TrimLeft( _T(" ") );
			
			CString lPath=find.GetFilePath();
			int pos=lPath.ReverseFind('\\');
			ASSERT(pos!=-1);
			lPath=lPath.Left(pos+1);
			AddQueueItem(FALSE, find.GetFileName(), sdir, lPath, CServerPath(), upload, user, pass);
		}
	}
	if (bDirIsEmpty)
	{
		CServerPath serverpath = reinterpret_cast<CFtpListCtrl *>(GetFtpPane()->GetListCtrl())->GetCurrentDirectory();
		if (!serverpath.IsEmpty())
		{
			CString sdir;
			CString path=subdir;
			path.TrimLeft( _T("\\") );
			path.TrimRight(_T("\\") );
			while (path!=_T("") )
			{
				CString tmp;
				int pos=path.Find( _T("\\") );
				if (pos==-1)
				{
					tmp.Format(_T(" %d %s"), path.GetLength(), path);
					path="";
				}
				else
				{
					tmp.Format(_T(" %d %s"), path.Left(pos).GetLength(), path.Left(pos));
					path=path.Mid(pos+1);
				}
				sdir+=tmp;
			}
			sdir.TrimLeft( _T(" ") );
			serverpath.AddSubdirs(sdir);
			m_pCommandQueue->MakeDir(serverpath);
		}
	}
}

void CMainFrame::SaveMostRecentServers()
{
	CMarkupSTL *pXML;
	if (COptions::LockXML(&pXML))
	{
		pXML->ResetPos();
		while (pXML->FindChildElem( _T("RecentServers") ))
			pXML->RemoveChildElem();

		pXML->AddChildElem( _T("RecentServers") );
		pXML->IntoElem();
		for (std::list<t_server>::reverse_iterator iter=m_RecentQuickconnectServers.rbegin(); iter!=m_RecentQuickconnectServers.rend(); iter++)
		{
			if (_ttoi(COptions::GetOption(OPTION_RUNINSECUREMODE)) || iter->bDontRememberPass)
				iter->pass = "";
			COptions::SaveServer(pXML, *iter);
		}

		pXML->OutOfElem();

		COptions::UnlockXML();
	}
	else
	{
		TCHAR buffer[1000];
		HKEY key;
		int j=0;
		if (RegOpenKey(HKEY_CURRENT_USER, _T("Software\\FileZilla\\Recent Servers\\") ,&key)==ERROR_SUCCESS)
		{
			while (RegEnumKey(key, j, buffer, 1000)==ERROR_SUCCESS)
			{
				if (RegDeleteKey(key, buffer) != ERROR_SUCCESS)
					j++;
			}
		}
		else
			if (RegCreateKey(HKEY_CURRENT_USER, _T("Software\\FileZilla\\Recent Servers\\") ,&key)!=ERROR_SUCCESS)
				return;
		int i=1;
		for (std::list<t_server>::reverse_iterator iter=m_RecentQuickconnectServers.rbegin(); iter!=m_RecentQuickconnectServers.rend(); iter++)
		{
			CString str;
			str.Format(_T("Server %d"), i++);
			HKEY key2;
			if (RegCreateKey(key, str, &key2) == ERROR_SUCCESS)
			{
				if (_ttoi(COptions::GetOption(OPTION_RUNINSECUREMODE)) || iter->bDontRememberPass)
					iter->pass = "";
				COptions::SaveServer(key2, *iter);
				RegCloseKey(key2);
			}
		}
		RegCloseKey(key);
	}
}
