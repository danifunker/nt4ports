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

#if !defined(AFX_LOCALVIEW2_H__416D5DDA_B100_4979_AE70_3C8541D290E0__INCLUDED_)
#define AFX_LOCALVIEW2_H__416D5DDA_B100_4979_AE70_3C8541D290E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LocalView2.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Ansicht CLocalView2 

class CMainFrame;
class CLocalFileListCtrl;
class CFileChangeNotify;

class CLocalView2 : public CView
{
protected:
	CLocalView2();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CLocalView2)

// Attribute
public:

// Operationen
public:
	void EnableStatusbar(BOOL bEnable = TRUE);
	BOOL IsStatusbarEnabled();
	BOOL SetStatusBarText(LPCTSTR pszText);
	void RefreshFile(CString file);
	CLocalFileListCtrl *m_pListCtrl;
	void UpdateViewHeader();
	CWnd* GetListCtrl();
	//void Refresh();
	CString GetLocalFolder();
	void SetLocalFolder(CString folder);
	void SetLocalFolderOut(CString folder);
	CMainFrame* m_pOwner;
	void CheckAutoRefresh();

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CLocalView2)
	protected:
	virtual void OnDraw(CDC* pDC);      // Überschrieben zum Zeichnen dieser Ansicht
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CLocalView2();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	bool m_changedFilelist;
	BOOL m_bShowStatusbar;
	CFileChangeNotify *m_pFileNotify;
	t_LocalViewHeader *m_pLocalViewHeader;
	CStatusBar *m_pStatusBar;
	//{{AFX_MSG(CLocalView2)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnRequestRefresh(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_LOCALVIEW2_H__416D5DDA_B100_4979_AE70_3C8541D290E0__INCLUDED_
