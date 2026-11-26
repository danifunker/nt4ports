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

#if !defined(AFX_OPTIONSINTERFACEPAGE_H__5DB0EA38_6FC7_4725_8DFD_C78B94B3006F__INCLUDED_)
#define AFX_OPTIONSINTERFACEPAGE_H__5DB0EA38_6FC7_4725_8DFD_C78B94B3006F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsInterfacePage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsInterfacePage 

class COptionsInterfacePage : public CSAPrefsSubDlg
{
// Konstruktion
public:
	COptionsInterfacePage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(COptionsInterfacePage)
	enum { IDD = IDD_OPTIONS_INTERFACE };
	CButton	m_cShowRemoteTreeView;
	CButton	m_cShowQuickconnectBar;
	CButton	m_cShowQueue;
	CButton	m_cShowToolBar;
	CButton	m_cShowStatusBar;
	CButton	m_cShowMessageLog;
	CButton	m_cShowLocalTree;
	BOOL	m_bShowLocalTree;
	BOOL	m_bShowMessageLog;
	BOOL	m_bShowQuickconnectBar;
	BOOL	m_bShowStatusBar;
	BOOL	m_bShowToolBar;
	BOOL	m_bShowQueue;
	BOOL	m_bShowViewLabels;
	int		m_nViewMode;
	BOOL	m_bRememberWindowPos;
	BOOL	m_bShowRemoteTree;
	int		m_nMinimize;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COptionsInterfacePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsInterfacePage)
	afx_msg void OnViewmode();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OPTIONSINTERFACEPAGE_H__5DB0EA38_6FC7_4725_8DFD_C78B94B3006F__INCLUDED_
