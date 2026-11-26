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

// OptionsMiscPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsMiscPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite COptionsMiscPage 

IMPLEMENT_DYNCREATE(COptionsMiscPage, CSAPrefsSubDlg)

COptionsMiscPage::COptionsMiscPage() : CSAPrefsSubDlg(COptionsMiscPage::IDD)
{
	//{{AFX_DATA_INIT(COptionsMiscPage)
	m_DefaultFolder = _T("");
	m_nFolderType = -1;
	m_bOpenSitemanagerOnStart = FALSE;
	m_bSortSitemanagerFoldersFirst = FALSE;
	m_bExpandSitemanagerFolders = FALSE;
	//}}AFX_DATA_INIT
}

COptionsMiscPage::~COptionsMiscPage()
{
}

void COptionsMiscPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsMiscPage)
	DDX_Text(pDX, IDC_DEFAULTFOLDER, m_DefaultFolder);
	DDX_Radio(pDX, IDC_SELECTDEFAULTFOLDERTYPE, m_nFolderType);
	DDX_Check(pDX, IDC_SITEMANAGERONSTART, m_bOpenSitemanagerOnStart);
	DDX_Check(pDX, IDC_SITEMANAGERSORTFOLDERSFIRST, m_bSortSitemanagerFoldersFirst);
	DDX_Check(pDX, IDC_SITEMANAGEREXPANDFOLDERS, m_bExpandSitemanagerFolders);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsMiscPage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsMiscPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsMiscPage 
