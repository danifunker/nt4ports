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

#if !defined(AFX_FTPTREECTRL_H__7605AD6D_3F9F_4229_BCFC_21E2784C1FB3__INCLUDED_)
#define AFX_FTPTREECTRL_H__7605AD6D_3F9F_4229_BCFC_21E2784C1FB3__INCLUDED_

#include "structures.h"	// Hinzugefügt von der Klassenansicht
#include "FileZillaApi.h"	// Hinzugefügt von der Klassenansicht
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FtpTreeCtrl.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CFtpTreeCtrl 

class CFtpTreeCtrl : public CTreeCtrl
{
// Konstruktion
public:
	CFtpTreeCtrl();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CFtpTreeCtrl)
	//}}AFX_VIRTUAL

// Implementierung
public:
	void EnableTree(BOOL bEnable /*=TRUE*/ );
	CImageList m_ImgList2;
	void List(t_directory *pDirectory);
	virtual ~CFtpTreeCtrl();
	CServerPath GetDropTarget();

	// Drag&Drop
	BOOL DragPosition(CImageList *pImageList, CWnd* pDragWnd, CPoint point);
	void DragLeave(CImageList *pImageList);
	void OnDragEnd(int target, CPoint point);
	HTREEITEM m_hDragSource;

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	HTREEITEM m_CollapsedItem;
	BOOL m_bChanged;
	CImageList m_SystemImageList;
	int m_nIconIndex;
	BOOL GetSysImgList();
	CServerPath GetPath(HTREEITEM hItem);
	BOOL m_bDontNotifySelchange;
	void UpdateChildren(HTREEITEM hParent, std::list<CString > &SubdirList);
	HTREEITEM FindSegment(HTREEITEM hParent, CString Segment);
	HTREEITEM AddPath(CServerPath path);
	t_directory* m_pDirectory;
	t_server m_Server;

	// Drag&Drop
	HTREEITEM m_hDragHilited;
	
	//{{AFX_MSG(CFtpTreeCtrl)
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_FTPTREECTRL_H__7605AD6D_3F9F_4229_BCFC_21E2784C1FB3__INCLUDED_
