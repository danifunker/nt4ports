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

#if !defined(AFX_LOCALVIEW_H__7BA61A84_1EDD_44B7_90E1_0DC7F04452CE__INCLUDED_)
#define AFX_LOCALVIEW_H__7BA61A84_1EDD_44B7_90E1_0DC7F04452CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LocalView.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Ansicht CLocalView 

class CDirTreeCtrl;
class CMainFrame;

class CLocalView : public CView
{
protected:
	CLocalView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CLocalView)

// Attribute
public:

// Operationen
public:
	void UpdateViewHeader();
	t_LocalViewHeader *m_pLocalViewHeader;
	CDirTreeCtrl* GetTreeCtrl();
	void Refresh();
	CString GetLocalFolder();
	void SetLocalFolder(CString folder);
	CMainFrame *m_pOwner;
	void SetLocalFolderOut(CString folder);

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CLocalView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // Überschrieben zum Zeichnen dieser Ansicht
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CLocalView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	CDirTreeCtrl *m_pDirTree;
	//{{AFX_MSG(CLocalView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_LOCALVIEW_H__7BA61A84_1EDD_44B7_90E1_0DC7F04452CE__INCLUDED_
