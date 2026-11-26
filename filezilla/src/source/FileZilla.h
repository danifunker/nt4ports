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

// FileZilla.h : Haupt-Header-Datei für die Anwendung FileZilla
//

#if !defined(AFX_TEST_H__49EB86F4_E707_4B5F_8D8F_18E6DC42A72A__INCLUDED_)
#define AFX_TEST_H__49EB86F4_E707_4B5F_8D8F_18E6DC42A72A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CFileZillaApp:
// Siehe FileZilla.cpp für die Implementierung dieser Klasse
//

class CFileZillaApp : public CWinApp
{
public:
	BOOL m_bLangSet;
	CString m_appPath;
	CFileZillaApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CFileZillaApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementierung
	//{{AFX_MSG(CFileZillaApp)
	afx_msg void OnAppAbout();
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	BOOL InitLanguage();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_TEST_H__49EB86F4_E707_4B5F_8D8F_18E6DC42A72A__INCLUDED_)
