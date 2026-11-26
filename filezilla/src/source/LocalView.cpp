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

// LocalView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "FileZilla.h"
#include "LocalView.h"
#include "DirTreeCtrl.h"
#include "MainFrm.h"
#include "misc\ComboCompletion.h"
#include "misc\VisualStylesXP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocalView

IMPLEMENT_DYNCREATE(CLocalView, CView)

CLocalView::CLocalView()
{
	m_pDirTree=0;
	m_pLocalViewHeader=((CMainFrame*)AfxGetMainWnd())->m_pLocalViewHeader;
}	

CLocalView::~CLocalView()
{
	if (m_pDirTree) delete m_pDirTree;
}


BEGIN_MESSAGE_MAP(CLocalView, CView)
	//{{AFX_MSG_MAP(CLocalView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CLocalView 

void CLocalView::OnDraw(CDC* pDC)
{
	if (!m_pLocalViewHeader->bTreeHidden)
	{
		CPen pen(PS_SOLID,1,RGB(0,0,0));
		CPen *pOldPen=pDC->SelectObject(&pen);
		
		CRect rect;
		CRect comboRect;
		
		GetClientRect(rect);
		m_pLocalViewHeader->m_pEdit->GetClientRect(comboRect);

		pDC->MoveTo(0, comboRect.bottom - 1);
		pDC->LineTo(rect.right, comboRect.bottom - 1);
		
		pDC->SelectObject(pOldPen);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Diagnose CLocalView

#ifdef _DEBUG
void CLocalView::AssertValid() const
{
	CView::AssertValid();
}

void CLocalView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CLocalView 

int CLocalView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pDirTree=new CDirTreeCtrl;
	BOOL bRet = m_pDirTree->Create(WS_CHILD | TVS_LINESATROOT | 
                          TVS_HASBUTTONS | WS_VISIBLE | 
                          TVS_HASLINES | TVS_SHOWSELALWAYS |
						  TVS_EDITLABELS, 
                          CRect(0, 21, 0, 0), 
                          this,0);
    if (bRet)
	{
		m_pDirTree->m_pOwner=this;
		m_pDirTree->DisplayTree();
	}
	
	return bRet;
}

void CLocalView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	CVisualStylesXP xp;
   	int themeoffset = 0;
	if (xp.IsAppThemed())
		themeoffset = -1;

	CRect comboRect;
	m_pLocalViewHeader->m_pEdit->GetClientRect(comboRect);

	if (m_pDirTree->m_hWnd)
		m_pDirTree->SetWindowPos( NULL, 0, comboRect.bottom, cx, cy-comboRect.bottom, SWP_NOZORDER );
	if (!m_pLocalViewHeader->bTreeHidden)
	{
		m_pLocalViewHeader->m_pEdit->SetParent(this);
		m_pLocalViewHeader->m_pLabelBackground->SetParent(this);
		m_pLocalViewHeader->m_pLabel->SetParent(this);
	}
	if (!COptions::GetOptionVal(OPTION_SHOWNOLABEL))
	{
		m_pLocalViewHeader->m_pLabelBackground->ShowWindow(SW_SHOW);
		m_pLocalViewHeader->m_pLabel->ShowWindow(SW_SHOW);
		
		m_pLocalViewHeader->m_pLabelBackground->SetWindowPos( NULL, 0, 0, m_pLocalViewHeader->m_LabelTextSize.cx, comboRect.bottom-1, SWP_NOZORDER);
		m_pLocalViewHeader->m_pLabel->SetWindowPos( m_pLocalViewHeader->m_pLabelBackground, 0, 10-m_pLocalViewHeader->m_LabelTextSize.cy/2-1, m_pLocalViewHeader->m_LabelTextSize.cx, comboRect.bottom-1-(10-m_pLocalViewHeader->m_LabelTextSize.cy/2-1), 0);
		m_pLocalViewHeader->m_pEdit->SetWindowPos( NULL, m_pLocalViewHeader->m_LabelTextSize.cx, 0, cx-m_pLocalViewHeader->m_LabelTextSize.cx + 2 + themeoffset, comboRect.bottom, SWP_NOZORDER);
	}
	else
	{
		m_pLocalViewHeader->m_pLabelBackground->ShowWindow(SW_HIDE);
		m_pLocalViewHeader->m_pLabel->ShowWindow(SW_HIDE);
	
		m_pLocalViewHeader->m_pEdit->SetWindowPos( NULL, 0, 0, cx+2, comboRect.bottom, SWP_NOZORDER);
	}
}

void CLocalView::SetLocalFolderOut(CString folder)
{
	m_pOwner->SetLocalFolder(folder);
}

void CLocalView::SetLocalFolder(CString folder)
{
	m_pDirTree->SetSelPath(folder);
}

void CLocalView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
}

CString CLocalView::GetLocalFolder()
{
	return m_pDirTree->GetFullPath(m_pDirTree->GetSelectedItem());
}

void CLocalView::Refresh()
{
	CString path=m_pDirTree->GetFullPath(m_pDirTree->GetSelectedItem());
	m_pDirTree->DisplayTree();
	m_pDirTree->SetSelPath(path);
}

CDirTreeCtrl* CLocalView::GetTreeCtrl()
{
	return m_pDirTree;
}

void CLocalView::UpdateViewHeader()
{
	CVisualStylesXP xp;
   	int themeoffset = 0;
	if (xp.IsAppThemed())
		themeoffset = -1;
	
	CString str;
	str.LoadString(IDS_VIEWLABEL_LOCAL);
	str="  " + str + " ";
	m_pLocalViewHeader->m_pLabel->SetWindowText(str);

	CDC *pDC=m_pLocalViewHeader->m_pLabel->GetDC();
	m_pLocalViewHeader->m_pLabel->GetFont();

	CFont *pOldFont=pDC->SelectObject(m_pLocalViewHeader->m_pLabel->GetFont());
	GetTextExtentPoint32( pDC->GetSafeHdc(), str, str.GetLength(), &m_pLocalViewHeader->m_LabelTextSize );
	pDC->SelectObject(pOldFont);
	
	m_pLocalViewHeader->m_pLabel->ReleaseDC(pDC);

	int cx,cy;
	RECT rect;
	GetClientRect(&rect);
	cx=rect.right;
	cy=rect.bottom;

	CRect comboRect;
	m_pLocalViewHeader->m_pEdit->GetClientRect(comboRect);

	if (m_pDirTree->m_hWnd)
		m_pDirTree->SetWindowPos( NULL, 0, comboRect.bottom, cx, cy-comboRect.bottom, SWP_NOZORDER );
	if (!m_pLocalViewHeader->bTreeHidden)
	{
		m_pLocalViewHeader->m_pEdit->SetParent(this);
		m_pLocalViewHeader->m_pLabelBackground->SetParent(this);
		m_pLocalViewHeader->m_pLabel->SetParent(this);
	}
	if (!COptions::GetOptionVal(OPTION_SHOWNOLABEL))
	{
		m_pLocalViewHeader->m_pLabelBackground->ShowWindow(SW_SHOW);
		m_pLocalViewHeader->m_pLabel->ShowWindow(SW_SHOW);
		
		m_pLocalViewHeader->m_pLabelBackground->SetWindowPos( NULL, 0, 0, m_pLocalViewHeader->m_LabelTextSize.cx, comboRect.bottom-1, SWP_NOZORDER);
		m_pLocalViewHeader->m_pLabel->SetWindowPos( m_pLocalViewHeader->m_pLabelBackground, 0, 10-m_pLocalViewHeader->m_LabelTextSize.cy/2-1, m_pLocalViewHeader->m_LabelTextSize.cx, comboRect.bottom-1-(10-m_pLocalViewHeader->m_LabelTextSize.cy/2-1), 0);
		m_pLocalViewHeader->m_pEdit->SetWindowPos( NULL, m_pLocalViewHeader->m_LabelTextSize.cx, 0, cx-m_pLocalViewHeader->m_LabelTextSize.cx + 2 + themeoffset, comboRect.bottom, SWP_NOZORDER);
	}
	else
	{
		m_pLocalViewHeader->m_pLabelBackground->ShowWindow(SW_HIDE);
		m_pLocalViewHeader->m_pLabel->ShowWindow(SW_HIDE);
	
		m_pLocalViewHeader->m_pEdit->SetWindowPos( NULL, 0, 0, cx+2, comboRect.bottom, SWP_NOZORDER);
	}
}

BOOL CLocalView::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}
