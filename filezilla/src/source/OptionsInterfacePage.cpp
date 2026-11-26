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

// OptionsInterfacePage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsInterfacePage.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsInterfacePage 


COptionsInterfacePage::COptionsInterfacePage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsInterfacePage::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsInterfacePage)
	m_bShowLocalTree = FALSE;
	m_bShowMessageLog = FALSE;
	m_bShowQuickconnectBar = FALSE;
	m_bShowStatusBar = FALSE;
	m_bShowToolBar = FALSE;
	m_bShowQueue = FALSE;
	m_bShowViewLabels = FALSE;
	m_nViewMode = -1;
	m_bRememberWindowPos = FALSE;
	m_bShowRemoteTree = FALSE;
	m_nMinimize = -1;
	//}}AFX_DATA_INIT
}


void COptionsInterfacePage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsInterfacePage)
	DDX_Control(pDX, IDC_SHOWREMOTETREEVIEW, m_cShowRemoteTreeView);
	DDX_Control(pDX, IDC_SHOWQUICKCONNECTBAR, m_cShowQuickconnectBar);
	DDX_Control(pDX, IDC_SHOWTRANSFERQUEUE, m_cShowQueue);
	DDX_Control(pDX, IDC_SHOWTOOLBAR, m_cShowToolBar);
	DDX_Control(pDX, IDC_SHOWSTATUSBAR, m_cShowStatusBar);
	DDX_Control(pDX, IDC_SHOWMESSAGELOG, m_cShowMessageLog);
	DDX_Control(pDX, IDC_SHOWLOCALTREEVIEW, m_cShowLocalTree);
	DDX_Check(pDX, IDC_SHOWLOCALTREEVIEW, m_bShowLocalTree);
	DDX_Check(pDX, IDC_SHOWMESSAGELOG, m_bShowMessageLog);
	DDX_Check(pDX, IDC_SHOWQUICKCONNECTBAR, m_bShowQuickconnectBar);
	DDX_Check(pDX, IDC_SHOWSTATUSBAR, m_bShowStatusBar);
	DDX_Check(pDX, IDC_SHOWTOOLBAR, m_bShowToolBar);
	DDX_Check(pDX, IDC_SHOWTRANSFERQUEUE, m_bShowQueue);
	DDX_Check(pDX, IDC_VIEWLABELS, m_bShowViewLabels);
	DDX_Radio(pDX, IDC_VIEWMODE, m_nViewMode);
	DDX_Check(pDX, IDC_REMEMBERWINDOWPOS, m_bRememberWindowPos);
	DDX_Check(pDX, IDC_SHOWREMOTETREEVIEW, m_bShowRemoteTree);
	DDX_Radio(pDX, IDC_OPTIONS_INTERFACE_MINIMIZETOTRAY, m_nMinimize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsInterfacePage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsInterfacePage)
	ON_BN_CLICKED(IDC_VIEWMODE, OnViewmode)
	ON_BN_CLICKED(IDC_VIEWMODE2, OnViewmode)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsInterfacePage 

void COptionsInterfacePage::OnViewmode() 
{
	UpdateData(TRUE);
	m_cShowToolBar.EnableWindow(!m_nViewMode);
	m_cShowQuickconnectBar.EnableWindow(!m_nViewMode);
	m_cShowStatusBar.EnableWindow(!m_nViewMode);
	m_cShowMessageLog.EnableWindow(!m_nViewMode);
	m_cShowLocalTree.EnableWindow(!m_nViewMode);
	m_cShowQueue.EnableWindow(!m_nViewMode);
	m_cShowRemoteTreeView.EnableWindow(!m_nViewMode);
}

BOOL COptionsInterfacePage::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();

	OnViewmode();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
