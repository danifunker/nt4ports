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

// OptionsRemoteViewPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsRemoteViewPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsRemoteViewPage 


COptionsRemoteViewPage::COptionsRemoteViewPage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsRemoteViewPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsRemoteViewPage)
	m_bSize = FALSE;
	m_bTime = FALSE;
	m_bPermissions = FALSE;
	m_nViewMode = 0;
	m_bRememberColumnWidths = FALSE;
	m_bShowHidden = FALSE;
	m_bShowStatusBar = FALSE;
	m_bRememberColumnSort = FALSE;
	m_bOwnerGroup = FALSE;
	m_nDoubleclickAction = -1;
	//}}AFX_DATA_INIT
}


void COptionsRemoteViewPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsRemoteViewPage)
	DDX_Control(pDX, IDC_OPTIONS_REMOTEVIEW_OWNERGROUP, m_cOwnerGroup);
	DDX_Control(pDX, IDC_OPTIONS_REMOTEVIEW_SHOWSTATUSBAR, m_cShowStatusBar);
	DDX_Control(pDX, IDC_OPTIONS_REMOTEVIEW_SIZEFORMAT, m_cSizeFormat);
	DDX_Control(pDX, IDC_OPTIONS_REMOTEVIEW_PERMISSIONS, m_cPermissions);
	DDX_Control(pDX, IDC_OPTIONS_REMOTEVIEW_DATE, m_cDate);
	DDX_Control(pDX, IDC_OPTIONS_REMOTEVIEW_TIME, m_cTime);
	DDX_Control(pDX, IDC_OPTIONS_REMOTEVIEW_STYLE, m_cStyle);
	DDX_Control(pDX, IDC_OPTIONS_REMOTEVIEW_SIZE, m_cSize);
	DDX_Control(pDX, IDC_OPTIONS_REMOTEVIEW_FILETYPE, m_cFiletype);
	DDX_Check(pDX, IDC_OPTIONS_REMOTEVIEW_SIZE, m_bSize);
	DDX_Check(pDX, IDC_OPTIONS_REMOTEVIEW_FILETYPE, m_bFiletype);
	DDX_Check(pDX, IDC_OPTIONS_REMOTEVIEW_DATE, m_bDate);
	DDX_Check(pDX, IDC_OPTIONS_REMOTEVIEW_TIME, m_bTime);
	DDX_Check(pDX, IDC_OPTIONS_REMOTEVIEW_PERMISSIONS, m_bPermissions);
	DDX_Radio(pDX, IDC_OPTIONS_REMOTEVIEW_ALWAYS, m_nViewMode);
	DDX_Check(pDX, IDC_OPTIONS_REMOTEVIEW_REMEMBERCOLUMNWIDTHS, m_bRememberColumnWidths);
	DDX_Check(pDX, IDC_OPTIONS_REMOTEVIEW_ALWAYSSHOWHIDDEN, m_bShowHidden);
	DDX_Check(pDX, IDC_OPTIONS_REMOTEVIEW_SHOWSTATUSBAR, m_bShowStatusBar);
	DDX_Check(pDX, IDC_OPTIONS_REMOTEVIEW_REMEMBERCOLUMNSORT, m_bRememberColumnSort);
	DDX_Check(pDX, IDC_OPTIONS_REMOTEVIEW_OWNERGROUP, m_bOwnerGroup);
	DDX_Radio(pDX, IDC_OPTIONS_INTERFACE_REMOTE_DOUBLECLICK1, m_nDoubleclickAction);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsRemoteViewPage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsRemoteViewPage)
	ON_CBN_SELCHANGE(IDC_OPTIONS_REMOTEVIEW_STYLE, OnViewmode)
	ON_BN_CLICKED(IDC_OPTIONS_REMOTEVIEW_ALWAYS, OnViewmode)
	ON_BN_CLICKED(IDC_OPTIONS_REMOTEVIEW_REMEMBER, OnViewmode)
	ON_CBN_SELCHANGE(IDC_OPTIONS_REMOTEVIEW_SIZEFORMAT, OnSizeFormat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsRemoteViewPage 

void COptionsRemoteViewPage::OnViewmode() 
{
	UpdateData(TRUE);
	m_cStyle.EnableWindow(!m_nViewMode);
	if (!m_cStyle.GetCurSel())
	{
		m_cSize.EnableWindow(!m_nViewMode);
		m_cDate.EnableWindow(!m_nViewMode);
		m_cTime.EnableWindow(!m_nViewMode);
		m_cOwnerGroup.EnableWindow(!m_nViewMode);
		m_cPermissions.EnableWindow(!m_nViewMode);
		m_cFiletype.EnableWindow(!m_nViewMode);
	}
	else
	{
		m_cSize.EnableWindow(FALSE);
		m_cDate.EnableWindow(FALSE);
		m_cTime.EnableWindow(FALSE);
		m_cOwnerGroup.EnableWindow(FALSE);
		m_cPermissions.EnableWindow(FALSE);
		m_cFiletype.EnableWindow(FALSE);
	}
	m_nRemoteStyle = m_cStyle.GetCurSel();

	m_cShowStatusBar.EnableWindow(!m_nViewMode);
}

BOOL COptionsRemoteViewPage::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();
	
	m_cStyle.SetCurSel(m_nRemoteStyle);
	OnViewmode();
		
	m_cSizeFormat.SetCurSel(m_nSizeFormat);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void COptionsRemoteViewPage::OnSizeFormat() 
{
	m_nSizeFormat = m_cSizeFormat.GetCurSel();	
}
