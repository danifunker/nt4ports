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

#if !defined(AFX_FTPVIEW_H__61F5EC46_AA35_4F2D_8154_A70462AD8C40__INCLUDED_)
#define AFX_FTPVIEW_H__61F5EC46_AA35_4F2D_8154_A70462AD8C40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FtpView.h : Header-Datei
//

class CFtpListCtrl;
class CMainFrame;

/////////////////////////////////////////////////////////////////////////////
// Ansicht CFtpView 

class CFtpView : public CView
{
protected:
	CFtpView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CFtpView)

// Attribute
public:

// Operationen
public:
	void Refresh();
	void UpdateViewHeader();
	CWnd* GetListCtrl();
	BOOL GetTransferfile(t_transferfile &transferfile);
	BOOL SetStatusBarText(LPCTSTR pszText);
	BOOL IsStatusbarEnabled();
	void EnableStatusbar(BOOL bEnable = TRUE);

	CFtpListCtrl *m_pListCtrl;

	void List(t_directory *list);

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CFtpView)
	protected:
	virtual void OnDraw(CDC* pDC);      // Überschrieben zum Zeichnen dieser Ansicht
	//}}AFX_VIRTUAL

// Implementierung
protected:
	CStatusBar *m_pStatusBar;
	CMainFrame *m_pOwner;
	BOOL m_bShowStatusbar;

	virtual ~CFtpView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	t_RemoteViewHeader *m_pRemoteViewHeader;

	//{{AFX_MSG(CFtpView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_FTPVIEW_H__61F5EC46_AA35_4F2D_8154_A70462AD8C40__INCLUDED_
