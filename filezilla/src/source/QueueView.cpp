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

// Queue.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "QueueView.h"
#include "QueueCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQueueView

IMPLEMENT_DYNCREATE(CQueueView, CView)
CQueueView::CQueueView()
{
	m_pQueueCtrl=0;
}

CQueueView::~CQueueView()
{
	if (m_pQueueCtrl)
		delete m_pQueueCtrl;
}


BEGIN_MESSAGE_MAP(CQueueView, CView)
	//{{AFX_MSG_MAP(CQueueView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CQueueView 

void CQueueView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// ZU ERLEDIGEN: Code zum Zeichnen hier einfügen
}

/////////////////////////////////////////////////////////////////////////////
// Diagnose CQueueView

#ifdef _DEBUG
void CQueueView::AssertValid() const
{
	CView::AssertValid();
}

void CQueueView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CQueueView 

void CQueueView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	m_pQueueCtrl->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE );
		
}

int CQueueView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pQueueCtrl=new CQueueCtrl;
#ifdef FZ_NT4
	// The comctl32 of NT 4.0 and Internet Explorer 3 crashes painting a list that is
	// both virtual and owner-drawn. There the queue is drawn by comctl32 itself: the
	// same columns, without the progress bars.
	m_pQueueCtrl->Create(LVS_REPORT|WS_CHILD | WS_VISIBLE | LVS_SHOWSELALWAYS | LVS_OWNERDATA |
		(fz_comctl32_version() >= 0x00040047 ? LVS_OWNERDRAWFIXED : 0), CRect(0,0,0,0), this, 0);
#else
	m_pQueueCtrl->Create(LVS_REPORT|WS_CHILD | WS_VISIBLE | LVS_SHOWSELALWAYS | LVS_OWNERDRAWFIXED | LVS_OWNERDATA, CRect(0,0,0,0), this, 0);
#endif
	return 0;
}

void CQueueView::AddItem(t_transferfile &transferfile, BOOL stdtransfer, int nOpen)
{
	m_pQueueCtrl->AddItem(transferfile, stdtransfer, nOpen);
}

CQueueCtrl * CQueueView::GetListCtrl()
{
	return m_pQueueCtrl;
}

BOOL CQueueView::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}
