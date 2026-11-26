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

// OptionsLocalViewPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsLocalViewPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsLocalViewPage 


COptionsLocalViewPage::COptionsLocalViewPage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsLocalViewPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsLocalViewPage)
	m_bSize = FALSE;
	m_bTime = FALSE;
	m_bType = FALSE;
	m_nViewMode = 0;
	m_bRememberColumnWidths = FALSE;
	m_bShowStatusBar = FALSE;
	m_bRememberColumnSort = FALSE;
	m_nDoubleclickAction = -1;
	//}}AFX_DATA_INIT
}


void COptionsLocalViewPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsLocalViewPage)
	DDX_Control(pDX, IDC_OPTIONS_LOCALVIEW_SHOWSTATUSBAR, m_cShowStatusBar);
	DDX_Control(pDX, IDC_OPTIONS_LOCALVIEW_SIZEFORMAT, m_cSizeFormat);
	DDX_Control(pDX, IDC_OPTIONS_LOCALVIEW_TYPE, m_cType);
	DDX_Control(pDX, IDC_OPTIONS_LOCALVIEW_TIME, m_cTime);
	DDX_Control(pDX, IDC_OPTIONS_LOCALVIEW_STYLE, m_cStyle);
	DDX_Control(pDX, IDC_OPTIONS_LOCALVIEW_SIZE, m_cSize);
	DDX_Check(pDX, IDC_OPTIONS_LOCALVIEW_SIZE, m_bSize);
	DDX_Check(pDX, IDC_OPTIONS_LOCALVIEW_TIME, m_bTime);
	DDX_Check(pDX, IDC_OPTIONS_LOCALVIEW_TYPE, m_bType);
	DDX_Radio(pDX, IDC_OPTIONS_LOCALVIEW_ALWAYS, m_nViewMode);
	DDX_Check(pDX, IDC_OPTIONS_LOCALVIEW_REMEMBERCOLUMNWIDTHS, m_bRememberColumnWidths);
	DDX_Check(pDX, IDC_OPTIONS_LOCALVIEW_SHOWSTATUSBAR, m_bShowStatusBar);
	DDX_Check(pDX, IDC_OPTIONS_LOCALVIEW_REMEMBERCOLUMNSORT, m_bRememberColumnSort);
	DDX_Radio(pDX, IDC_OPTIONS_INTERFACE_LOCAL_DOUBLECLICK1, m_nDoubleclickAction);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsLocalViewPage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsLocalViewPage)
	ON_CBN_SELCHANGE(IDC_OPTIONS_LOCALVIEW_STYLE, OnViewmode)
	ON_BN_CLICKED(IDC_OPTIONS_LOCALVIEW_ALWAYS, OnViewmode)
	ON_BN_CLICKED(IDC_OPTIONS_LOCALVIEW_REMEMBER, OnViewmode)
	ON_CBN_SELCHANGE(IDC_OPTIONS_LOCALVIEW_SIZEFORMAT, OnSizeFormat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsLocalViewPage 

void COptionsLocalViewPage::OnViewmode() 
{
	UpdateData(TRUE);
	m_cStyle.EnableWindow(!m_nViewMode);
	if (!m_cStyle.GetCurSel())
	{
		m_cSize.EnableWindow(!m_nViewMode);
		m_cType.EnableWindow(!m_nViewMode);
		m_cTime.EnableWindow(!m_nViewMode);
	}
	else
	{
		m_cSize.EnableWindow(FALSE);
		m_cType.EnableWindow(FALSE);
		m_cTime.EnableWindow(FALSE);
	}
	m_nLocalStyle=m_cStyle.GetCurSel();

	m_cShowStatusBar.EnableWindow(!m_nViewMode);
}

BOOL COptionsLocalViewPage::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();
	
	m_cStyle.SetCurSel(m_nLocalStyle);
	OnViewmode();

	m_cSizeFormat.SetCurSel(m_nSizeFormat);	
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void COptionsLocalViewPage::OnSizeFormat() 
{
	m_nSizeFormat=m_cSizeFormat.GetCurSel();	
}
