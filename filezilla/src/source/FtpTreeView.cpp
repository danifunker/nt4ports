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

// FtpTreeView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "FileZilla.h"
#include "FtpTreeView.h"
#include "FtpTreeCtrl.h"
#include "misc\combocompletion.h"
#include "mainfrm.h"
#include "misc\VisualStylesXP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFtpTreeView

IMPLEMENT_DYNCREATE(CFtpTreeView, CView)

CFtpTreeView::CFtpTreeView()
{
	m_pTreeCtrl=0;
	m_pRemoteViewHeader=((CMainFrame*)AfxGetMainWnd())->m_pRemoteViewHeader;
}

CFtpTreeView::~CFtpTreeView()
{
	if (m_pTreeCtrl)
		delete m_pTreeCtrl;
}


BEGIN_MESSAGE_MAP(CFtpTreeView, CView)
	//{{AFX_MSG_MAP(CFtpTreeView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CFtpTreeView 

void CFtpTreeView::OnDraw(CDC* pDC)
{
	if (!m_pRemoteViewHeader->bTreeHidden)
	{
		CPen pen(PS_SOLID,1,RGB(0,0,0));
		CPen *pOldPen=pDC->SelectObject(&pen);
		
		CRect rect;
		CRect comboRect;
		
		GetClientRect(rect);
		m_pRemoteViewHeader->m_pEdit->GetClientRect(comboRect);

		pDC->MoveTo(0, comboRect.bottom - 1);
		pDC->LineTo(rect.right, comboRect.bottom - 1);
		
		pDC->SelectObject(pOldPen);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Diagnose CFtpTreeView

#ifdef _DEBUG
void CFtpTreeView::AssertValid() const
{
	CView::AssertValid();
}

void CFtpTreeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CFtpTreeView 

void CFtpTreeView::List(t_directory *pDirectory)
{
	m_pTreeCtrl->List(pDirectory);
}


int CFtpTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pTreeCtrl=new CFtpTreeCtrl;
	BOOL bRet = m_pTreeCtrl->Create(WS_CHILD | TVS_LINESATROOT | 
                          TVS_HASBUTTONS | WS_VISIBLE | 
                          TVS_HASLINES | TVS_SHOWSELALWAYS |
						  TVS_EDITLABELS, 
                          CRect(0, 21, 0, 0), 
                          this, 0);
	return bRet;
}

void CFtpTreeView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	CVisualStylesXP xp;
   	int themeoffset = 0;
	if (xp.IsAppThemed())
		themeoffset = -1;

	CRect comboRect;
	m_pRemoteViewHeader->m_pEdit->GetClientRect(comboRect);

	m_pTreeCtrl->ModifyStyleEx(0, WS_EX_TRANSPARENT);
	if (m_pTreeCtrl->m_hWnd)
		m_pTreeCtrl->SetWindowPos( &wndTop, 0, comboRect.bottom, cx, cy-comboRect.bottom, 0);
	
	if (!m_pRemoteViewHeader->bTreeHidden)
	{
		m_pRemoteViewHeader->m_pEdit->SetParent(this);
		m_pRemoteViewHeader->m_pLabelBackground->SetParent(this);
		m_pRemoteViewHeader->m_pLabel->SetParent(this);
	}
	if (!COptions::GetOptionVal(OPTION_SHOWNOLABEL))
	{
		m_pRemoteViewHeader->m_pLabelBackground->ShowWindow(SW_SHOW);
		m_pRemoteViewHeader->m_pLabel->ShowWindow(SW_SHOW);
		
		m_pRemoteViewHeader->m_pLabelBackground->SetWindowPos( NULL, 0, 0, m_pRemoteViewHeader->m_LabelTextSize.cx, comboRect.bottom-1, SWP_NOZORDER);
		m_pRemoteViewHeader->m_pLabel->SetWindowPos( m_pRemoteViewHeader->m_pLabelBackground, 0, 10-m_pRemoteViewHeader->m_LabelTextSize.cy/2-1, m_pRemoteViewHeader->m_LabelTextSize.cx, comboRect.bottom-1-(10-m_pRemoteViewHeader->m_LabelTextSize.cy/2-1), 0);
		m_pRemoteViewHeader->m_pEdit->SetWindowPos( NULL, m_pRemoteViewHeader->m_LabelTextSize.cx, 0, cx-m_pRemoteViewHeader->m_LabelTextSize.cx + 2 + themeoffset, comboRect.bottom, SWP_NOZORDER);
	}
	else
	{
		m_pRemoteViewHeader->m_pLabelBackground->ShowWindow(SW_HIDE);
		m_pRemoteViewHeader->m_pLabel->ShowWindow(SW_HIDE);
	
		m_pRemoteViewHeader->m_pEdit->SetWindowPos( NULL, 0, 0, cx+2, comboRect.bottom, SWP_NOZORDER);
	}
}

CWnd* CFtpTreeView::GetListCtrl()
{
	return m_pTreeCtrl;
}

void CFtpTreeView::UpdateViewHeader()
{
	CVisualStylesXP xp;
   	int themeoffset = 0;
	if (xp.IsAppThemed())
		themeoffset = -1;

	CString str;
	str.LoadString(IDS_VIEWLABEL_REMOTE);
	str="  " + str + " ";
	m_pRemoteViewHeader->m_pLabel->SetWindowText(str);
	
	CDC *pDC=m_pRemoteViewHeader->m_pLabel->GetDC();
	
	CFont *pOldFont=pDC->SelectObject(m_pRemoteViewHeader->m_pLabel->GetFont());
	GetTextExtentPoint32( pDC->GetSafeHdc(), str, str.GetLength(), &m_pRemoteViewHeader->m_LabelTextSize );
	pDC->SelectObject(pOldFont);
	
	m_pRemoteViewHeader->m_pLabel->ReleaseDC(pDC);

	int cx,cy;
	RECT rect;
	GetClientRect(&rect);
	cx = rect.right;
	cy = rect.bottom;

	CRect comboRect;
	m_pRemoteViewHeader->m_pEdit->GetClientRect(comboRect);

	if (m_pTreeCtrl->m_hWnd)
		m_pTreeCtrl->SetWindowPos( &wndTop, 0, comboRect.bottom, cx, cy-comboRect.bottom, 0);
	if (!m_pRemoteViewHeader->bTreeHidden)
	{
		m_pRemoteViewHeader->m_pEdit->SetParent(this);
		m_pRemoteViewHeader->m_pLabelBackground->SetParent(this);
		m_pRemoteViewHeader->m_pLabel->SetParent(this);
	}
	if (!COptions::GetOptionVal(OPTION_SHOWNOLABEL))
	{
		m_pRemoteViewHeader->m_pLabelBackground->ShowWindow(SW_SHOW);
		m_pRemoteViewHeader->m_pLabel->ShowWindow(SW_SHOW);
		
		m_pRemoteViewHeader->m_pLabelBackground->SetWindowPos( NULL, 0, 0, m_pRemoteViewHeader->m_LabelTextSize.cx, comboRect.bottom-1, SWP_NOZORDER);
		m_pRemoteViewHeader->m_pLabel->SetWindowPos( m_pRemoteViewHeader->m_pLabelBackground, 0, 10-m_pRemoteViewHeader->m_LabelTextSize.cy/2-1, m_pRemoteViewHeader->m_LabelTextSize.cx, comboRect.bottom-1-(10-m_pRemoteViewHeader->m_LabelTextSize.cy/2-1), 0);
		m_pRemoteViewHeader->m_pEdit->SetWindowPos( NULL, m_pRemoteViewHeader->m_LabelTextSize.cx, 0, cx-m_pRemoteViewHeader->m_LabelTextSize.cx+2 + themeoffset, comboRect.bottom, SWP_NOZORDER);
	}
	else
	{
		m_pRemoteViewHeader->m_pLabelBackground->ShowWindow(SW_HIDE);
		m_pRemoteViewHeader->m_pLabel->ShowWindow(SW_HIDE);
	
		m_pRemoteViewHeader->m_pEdit->SetWindowPos( NULL, 0, 0, cx+2, comboRect.bottom, SWP_NOZORDER);
	}
}

CTreeCtrl* CFtpTreeView::GetTreeCtrl()
{
	return m_pTreeCtrl;
}

void CFtpTreeView::EnableTree(BOOL bEnable = TRUE )
{
	m_pTreeCtrl->EnableTree(bEnable);
}
