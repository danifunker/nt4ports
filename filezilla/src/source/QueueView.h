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

#if !defined(AFX_QUEUE_H__B967C66E_C4E8_4FCC_87F3_D74DEC96DDE4__INCLUDED_)
#define AFX_QUEUE_H__B967C66E_C4E8_4FCC_87F3_D74DEC96DDE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Queue.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Ansicht CQueueView 
class CQueueCtrl;

class CQueueView : public CView
{
protected:
	CQueueView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CQueueView)

// Attribute
public:

// Operationen
public:
	CQueueCtrl * GetListCtrl();
	void AddItem(t_transferfile &transferfile, BOOL stdtransfer = FALSE, int nOpen = 0);

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CQueueView)
	protected:
	virtual void OnDraw(CDC* pDC);      // Überschrieben zum Zeichnen dieser Ansicht
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CQueueView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	CQueueCtrl *m_pQueueCtrl;
	//{{AFX_MSG(CQueueView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_QUEUE_H__B967C66E_C4E8_4FCC_87F3_D74DEC96DDE4__INCLUDED_
