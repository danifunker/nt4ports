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

#if !defined(AFX_OPTIONSMISCPAGE_H__14D694B2_C319_4942_9595_131871A460D6__INCLUDED_)
#define AFX_OPTIONSMISCPAGE_H__14D694B2_C319_4942_9595_131871A460D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsMiscPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsMiscPage 

class COptionsMiscPage : public CSAPrefsSubDlg
{
	DECLARE_DYNCREATE(COptionsMiscPage)

// Konstruktion
public:
	COptionsMiscPage();
	~COptionsMiscPage();

// Dialogfelddaten
	//{{AFX_DATA(COptionsMiscPage)
	enum { IDD = IDD_OPTIONS_MISC };
	CString	m_DefaultFolder;
	int		m_nFolderType;
	BOOL	m_bOpenSitemanagerOnStart;
	BOOL	m_bSortSitemanagerFoldersFirst;
	BOOL	m_bExpandSitemanagerFolders;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COptionsMiscPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsMiscPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OPTIONSMISCPAGE_H__14D694B2_C319_4942_9595_131871A460D6__INCLUDED_
