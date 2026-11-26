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

#if !defined(AFX_OPTIONSLOCALVIEWPAGE_H__12D02EBB_ACC8_40F5_B093_709A0C12999B__INCLUDED_)
#define AFX_OPTIONSLOCALVIEWPAGE_H__12D02EBB_ACC8_40F5_B093_709A0C12999B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsLocalViewPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsLocalViewPage 

class COptionsLocalViewPage : public CSAPrefsSubDlg
{
// Konstruktion
public:
	int m_nSizeFormat;
	int m_nLocalStyle;
	COptionsLocalViewPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(COptionsLocalViewPage)
	enum { IDD = IDD_OPTIONS_LOCALVIEW };
	CButton	m_cShowStatusBar;
	CComboBox	m_cSizeFormat;
	CButton	m_cType;
	CButton	m_cTime;
	CComboBox	m_cStyle;
	CButton	m_cSize;
	BOOL	m_bSize;
	BOOL	m_bTime;
	BOOL	m_bType;
	int		m_nViewMode;
	BOOL	m_bRememberColumnWidths;
	BOOL	m_bShowStatusBar;
	BOOL	m_bRememberColumnSort;
	int		m_nDoubleclickAction;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COptionsLocalViewPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsLocalViewPage)
	afx_msg void OnViewmode();
	virtual BOOL OnInitDialog();
	afx_msg void OnSizeFormat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OPTIONSLOCALVIEWPAGE_H__12D02EBB_ACC8_40F5_B093_709A0C12999B__INCLUDED_
