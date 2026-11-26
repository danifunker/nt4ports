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

// OptionsLanguagePage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsLanguagePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite COptionsLanguagePage 

IMPLEMENT_DYNCREATE(COptionsLanguagePage, CSAPrefsSubDlg)

COptionsLanguagePage::COptionsLanguagePage() : CSAPrefsSubDlg(COptionsLanguagePage::IDD)
{
	//{{AFX_DATA_INIT(COptionsLanguagePage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}

COptionsLanguagePage::~COptionsLanguagePage()
{
}

void COptionsLanguagePage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsLanguagePage)
	DDX_Control(pDX, IDC_LANGUAGELIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsLanguagePage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsLanguagePage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsLanguagePage 

BOOL COptionsLanguagePage::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();
	
	CString selLang=COptions::GetOption(OPTION_LANGUAGE);
	if (selLang=="")
		selLang="English";
	m_List.SetCurSel(0);
	int i=0;
	for (std::set<CString>::const_iterator iter=m_LanguageStringList.begin(); iter!=m_LanguageStringList.end(); iter++, i++)
	{
		m_List.AddString(*iter);
		if (*iter==selLang)
			m_List.SetCurSel(i);
		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void COptionsLanguagePage::OnDestroy() 
{
	CSAPrefsSubDlg::OnDestroy();
	
	m_List.GetText(m_List.GetCurSel(),m_selLang);
	
}
