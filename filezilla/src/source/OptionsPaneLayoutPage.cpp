// FileZilla - a Windows ftp client

// Copyright (C) 2003-2004 - Tim Kosse <tim.kosse@gmx.de>

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

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsPaneLayoutPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsPaneLayoutPage 


COptionsPaneLayoutPage::COptionsPaneLayoutPage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsPaneLayoutPage::IDD, pParent)
	, m_bSwitchLayout(FALSE)
{
	//{{AFX_DATA_INIT(COptionsPaneLayoutPage)
	m_nLocalTreePos = -1;
	m_nRemoteTreePos = -1;
	//}}AFX_DATA_INIT
}


void COptionsPaneLayoutPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsPaneLayoutPage)
	DDX_Radio(pDX, IDC_OPTIONS_PANELAYOUT_LOCALTREE1, m_nLocalTreePos);
	DDX_Radio(pDX, IDC_OPTIONS_PANELAYOUT_REMOTETREE1, m_nRemoteTreePos);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_OPTIONS_PANELAYOUT_SWITCHREMOTE, m_bSwitchLayout);
}


BEGIN_MESSAGE_MAP(COptionsPaneLayoutPage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsPaneLayoutPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsPaneLayoutPage 
