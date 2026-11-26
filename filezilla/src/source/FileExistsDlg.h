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

#if !defined(AFX_FILEEXISTSDLG_H__A9459EE3_943C_49F2_84A8_74CE7193AE87__INCLUDED_)
#define AFX_FILEEXISTSDLG_H__A9459EE3_943C_49F2_84A8_74CE7193AE87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileExistsDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFileExistsDlg 

class CFileExistsDlg : protected CDialog
{
// Konstruktion
public:
	static BOOL ProcessRequest(COverwriteRequestData *pData, CFileZillaApi  *pFileZillaApi, BOOL bNoDisplayDialog);
	static void SetOverwriteSettings(int nAction);
	static BOOL Always();
	static int GetAction();

protected:
// Dialogfelddaten
	//{{AFX_DATA(CFileExistsDlg)
	enum { IDD = IDD_FILEEXISTS };
	CStatic	m_cTime2;
	CStatic	m_cTime1;
	CStatic	m_cSize2;
	CStatic	m_cSize1;
	CStatic	m_cIcon2;
	CStatic	m_cIcon1;
	CStatic	m_cFile2;
	CStatic	m_cFile1;
	int		m_CurAction;
	BOOL	m_CurAll;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CFileExistsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	void SetData(CString filename1, _int64 size1, CTime *time1, CString filename2, _int64 size2, CTime *time2);
	BOOL m_bShowResumeCtrl;
	CFileExistsDlg(CWnd* pParent = NULL);   // Standardkonstruktor
	~CFileExistsDlg();
	static int m_nAction;
	static BOOL m_bAlways;
	CString m_DataFile1;
	CString m_DataFile2;
	_int64 m_DataFileSize1;
	_int64 m_DataFileSize2;
	CTime *m_DataFileTime1;
	CTime *m_DataFileTime2;
	CString FormatLongPath(UINT nID,CString oLongPath) const;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CFileExistsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define FILEEXISTS_ASK -1
#define FILEEXISTS_OVERWRITE 0
#define FILEEXISTS_OVERWRITEIFNEWER 1
#define FILEEXISTS_RESUME 2
#define FILEEXISTS_RENAME 3
#define FILEEXISTS_SKIP 4
#define FILEEXISTS_RESUME_ASKONFAIL 5 // Used by queue for automatic resuming. If APPE failes, ask what to do instead.

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_FILEEXISTSDLG_H__A9459EE3_943C_49F2_84A8_74CE7193AE87__INCLUDED_
