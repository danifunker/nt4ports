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

#if !defined(AFX_OPTIONSCONNECTION3_H__194195C1_A9F0_4AE4_B632_3AD416441213__INCLUDED_)
#define AFX_OPTIONSCONNECTION3_H__194195C1_A9F0_4AE4_B632_3AD416441213__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsConnection3.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsConnection3 

class COptionsConnection3 : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionsConnection3)

// Konstruktion
public:
	COptionsConnection3();
	~COptionsConnection3();

// Dialogfelddaten
	//{{AFX_DATA(COptionsConnection3)
	enum { IDD = IDD_CONNECTION3 };
	CString	m_Delay;
	CString	m_NumRetries;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COptionsConnection3)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsConnection3)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OPTIONSCONNECTION3_H__194195C1_A9F0_4AE4_B632_3AD416441213__INCLUDED_
