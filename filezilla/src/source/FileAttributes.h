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

#if !defined(AFX_FILEATTRIBUTES_H__48379EDF_159E_4623_ADFD_0D42E6E9B9B3__INCLUDED_)
#define AFX_FILEATTRIBUTES_H__48379EDF_159E_4623_ADFD_0D42E6E9B9B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileAttributes.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFileAttributes 

class CFileAttributes : public CDialog
{
// Konstruktion
public:
	void SetFileCount(int nFileCount);
	CString m_text;
	CFileAttributes(CWnd* pParent = NULL);   // Standardkonstruktor
	int m_permbuffer[10];

// Dialogfelddaten
	//{{AFX_DATA(CFileAttributes)
	enum { IDD = IDD_CHMOD };
	CButton	m_cPublicWrite;
	CButton	m_cPublicRead;
	CButton	m_cPublicExecute;
	CButton	m_cOwnerWrite;
	CButton	m_cOwnerRead;
	CButton	m_cOwnerExecute;
	CButton	m_cGroupWrite;
	CButton	m_cGroupRead;
	CButton	m_cGroupExecute;
	CStatic	m_TextCtrl;
	CString	m_Numeric;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CFileAttributes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	int m_nFileCount;
	void UpdateNumeric();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CFileAttributes)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnGroupexecute();
	afx_msg void OnOwnerexecute();
	afx_msg void OnOwnerread();
	afx_msg void OnOwnerwrite();
	afx_msg void OnPublicexecute();
	afx_msg void OnPublicread();
	afx_msg void OnPublicwrite();
	afx_msg void OnGroupread();
	afx_msg void OnGroupwrite();
	afx_msg void OnChangeNumeric();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_FILEATTRIBUTES_H__48379EDF_159E_4623_ADFD_0D42E6E9B9B3__INCLUDED_
