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

// OptionsDirCachePage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsDirCachePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsDirCachePage 


COptionsDirCachePage::COptionsDirCachePage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsDirCachePage::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDirCachePage)
	m_nHours = 0;
	m_nMinutes = 0;
	m_nSeconds = 0;
	m_bUseCache = -1;
	//}}AFX_DATA_INIT
}


void COptionsDirCachePage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDirCachePage)
	DDX_Text(pDX, IDC_OPTIONS_CACHE_HOURS, m_nHours);
	DDX_Text(pDX, IDC_OPTIONS_CACHE_MINUTES, m_nMinutes);
	DDX_Text(pDX, IDC_OPTIONS_CACHE_SECONDS, m_nSeconds);
	DDX_Radio(pDX, IDC_OPTIONS_CACHE_USECACHE, m_bUseCache);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDirCachePage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsDirCachePage)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsDirCachePage 
