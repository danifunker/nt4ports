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

#if !defined(AFX_OPTIONSTYPEPAGE_H__EA8220AE_D9F1_49F1_89C8_FD8F216D8445__INCLUDED_)
#define AFX_OPTIONSTYPEPAGE_H__EA8220AE_D9F1_49F1_89C8_FD8F216D8445__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsTypePage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsTypePage 

class COptionsTypePage : public CSAPrefsSubDlg
{
// Konstruktion
public:
	CString m_AsciiFiles;
	COptionsTypePage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(COptionsTypePage)
	enum { IDD = IDD_OPTIONS_TYPEPAGE };
	CButton	m_cTypeRemove;
	CListBox	m_cTypeList;
	CButton	m_cTypeAdd;
	int		m_nTypeMode;
	CString	m_TypeName;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COptionsTypePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsTypePage)
	afx_msg void OnSelchangeTypelist();
	afx_msg void OnTypeadd();
	afx_msg void OnTyperemove();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeTypename();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OPTIONSTYPEPAGE_H__EA8220AE_D9F1_49F1_89C8_FD8F216D8445__INCLUDED_
