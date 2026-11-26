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

// OptionsConnection3.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsConnection3.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite COptionsConnection3 

IMPLEMENT_DYNCREATE(COptionsConnection3, CPropertyPage)

COptionsConnection3::COptionsConnection3() : CPropertyPage(COptionsConnection3::IDD)
{
	//{{AFX_DATA_INIT(COptionsConnection3)
	m_Delay = _T("");
	m_NumRetries = _T("");
	//}}AFX_DATA_INIT
}

COptionsConnection3::~COptionsConnection3()
{
}

void COptionsConnection3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsConnection3)
	DDX_Text(pDX, IDC_DELAY, m_Delay);
	DDX_Text(pDX, IDC_NUMRETRY, m_NumRetries);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsConnection3, CPropertyPage)
	//{{AFX_MSG_MAP(COptionsConnection3)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsConnection3 
