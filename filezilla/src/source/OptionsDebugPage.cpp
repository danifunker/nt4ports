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

// OptionsDebugPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsDebugPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsDebugPage 


COptionsDebugPage::COptionsDebugPage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsDebugPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDebugPage)
	m_bEngineTrace = FALSE;
	m_bShowListing = FALSE;
	m_bDebugMenu = FALSE;
	//}}AFX_DATA_INIT
}


void COptionsDebugPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDebugPage)
	DDX_Check(pDX, IDC_DEBUG_ENGINETRACE, m_bEngineTrace);
	DDX_Check(pDX, IDC_DEBUG_SHOWRAWLISTING, m_bShowListing);
	DDX_Check(pDX, IDC_DEBUG_DEBUGMENU, m_bDebugMenu);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDebugPage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsDebugPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsDebugPage 

BOOL COptionsDebugPage::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
