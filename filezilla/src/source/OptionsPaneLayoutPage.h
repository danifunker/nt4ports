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

#if !defined(AFX_OPTIONSPANELAYOUTPAGE_H__9AE5A482_6365_4CC2_85DD_7E3F96DF7A95__INCLUDED_)
#define AFX_OPTIONSPANELAYOUTPAGE_H__9AE5A482_6365_4CC2_85DD_7E3F96DF7A95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsPaneLayoutPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsPaneLayoutPage 

class COptionsPaneLayoutPage : public CSAPrefsSubDlg
{
// Konstruktion
public:
	COptionsPaneLayoutPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(COptionsPaneLayoutPage)
	enum { IDD = IDD_OPTIONS_PANELAYOUT };
	int		m_nLocalTreePos;
	int		m_nRemoteTreePos;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COptionsPaneLayoutPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsPaneLayoutPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bSwitchLayout;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OPTIONSPANELAYOUTPAGE_H__9AE5A482_6365_4CC2_85DD_7E3F96DF7A95__INCLUDED_
