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

// FtpView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "FileZilla.h"
#include "FtpView.h"
#include "FtpListCtrl.h"
#include "mainfrm.h"
#include "misc\VisualStylesXP.h"
#include "ComboCompletion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFtpView

static UINT indicators[] =
{
	ID_SEPARATOR
};

IMPLEMENT_DYNCREATE(CFtpView, CView)

CFtpView::CFtpView()
{
	m_pOwner = (CMainFrame *)AfxGetMainWnd();
	m_pListCtrl = 0;
	m_pRemoteViewHeader = m_pOwner->m_pRemoteViewHeader;
	m_pStatusBar = NULL;
}

CFtpView::~CFtpView()
{
	if (m_pStatusBar)
		delete m_pStatusBar;
	if (m_pListCtrl)
		delete m_pListCtrl;
}


BEGIN_MESSAGE_MAP(CFtpView, CView)
	//{{AFX_MSG_MAP(CFtpView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CFtpView 

void CFtpView::OnDraw(CDC* pDC)
{
	if (m_pRemoteViewHeader->bTreeHidden)
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
// Diagnose CFtpView

#ifdef _DEBUG
void CFtpView::AssertValid() const
{
	CView::AssertValid();
}

void CFtpView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CFtpView 

void CFtpView::List(t_directory *list)
{
	m_pListCtrl->List(list);
}


int CFtpView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pListCtrl=new CFtpListCtrl(this);
	m_pListCtrl->Create(LVS_REPORT|WS_CHILD | WS_VISIBLE | LVS_SHOWSELALWAYS|LVS_SHAREIMAGELISTS|LVS_EDITLABELS|LVS_OWNERDATA,CRect(0,0,0,0),this,0);

	m_pStatusBar = new CStatusBar;
	m_pStatusBar->Create(this);
	m_pStatusBar->SetIndicators(indicators, 1);
	m_pStatusBar->SetPaneStyle(0, SBPS_STRETCH );
	m_bShowStatusbar = COptions::GetOptionVal(OPTION_SHOWREMOTESTATUSBAR);
	m_pStatusBar->ShowWindow(m_bShowStatusbar?SW_SHOW:SW_HIDE);

	return 0;
}

void CFtpView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	CVisualStylesXP xp;
   	int themeoffset = 0;
	if (xp.IsAppThemed())
		themeoffset = -1;

	if (m_pRemoteViewHeader->bTreeHidden)
	{
		CRect comboRect;
		m_pRemoteViewHeader->m_pEdit->GetClientRect(comboRect);

		if (m_pOwner->m_wndStatusBar.GetSafeHwnd())
		{
			CRect windowRect;
			m_pOwner->m_wndStatusBar.GetWindowRect(&windowRect);
			int size = 0;
			if (m_bShowStatusbar)
			{
				size = windowRect.bottom - windowRect.top;
				m_pStatusBar->SetWindowPos( NULL, 0, cy-size, cx, size+1, SWP_NOZORDER);
			}
			m_pListCtrl->SetWindowPos( NULL, 0, comboRect.bottom, cx, cy-comboRect.bottom-size, SWP_NOZORDER);
		}
		else
			m_pListCtrl->SetWindowPos( NULL, 0, comboRect.bottom, cx, cy-comboRect.bottom, SWP_NOZORDER);
		
		m_pRemoteViewHeader->m_pEdit->SetParent(this);
		m_pRemoteViewHeader->m_pLabelBackground->SetParent(this);
		m_pRemoteViewHeader->m_pLabel->SetParent(this);
	
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
		
			m_pRemoteViewHeader->m_pEdit->SetWindowPos( NULL, 0, 0, cx+2 + themeoffset, comboRect.bottom, SWP_NOZORDER);
		}
		
	}
	else
		if (m_pOwner->m_wndStatusBar.GetSafeHwnd())
		{
			CRect windowRect;
			m_pOwner->m_wndStatusBar.GetWindowRect(&windowRect);
			int size = 0;
			if (m_bShowStatusbar)
			{
				size = windowRect.bottom - windowRect.top;
				m_pStatusBar->SetWindowPos( NULL, 0, cy-size, cx, size+1, SWP_NOZORDER);
			}
			m_pListCtrl->SetWindowPos( NULL, 0, 0, cx, cy-size, SWP_NOZORDER);
		}
		else
			m_pListCtrl->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER);
}

BOOL CFtpView::GetTransferfile(t_transferfile &transferfile)
{
	return m_pListCtrl->GetTransferfile(transferfile);
}

CWnd* CFtpView::GetListCtrl()
{
	return m_pListCtrl;
}

void CFtpView::Refresh()
{
	m_pListCtrl->Refresh();
}

void CFtpView::UpdateViewHeader()
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
	cx=rect.right;
	cy=rect.bottom;
	if (m_pRemoteViewHeader->bTreeHidden)
	{
		CRect comboRect;
		m_pRemoteViewHeader->m_pEdit->GetClientRect(comboRect);
	
		if (m_pOwner->m_wndStatusBar.GetSafeHwnd())
		{
			CRect windowRect;
			m_pOwner->m_wndStatusBar.GetWindowRect(&windowRect);
			int size = 0;
			if (m_bShowStatusbar)
			{
				size = windowRect.bottom - windowRect.top;
				m_pStatusBar->SetWindowPos( NULL, 0, cy-size, cx, size+1, SWP_NOZORDER);
			}
			m_pListCtrl->SetWindowPos( NULL, 0, comboRect.bottom, cx, cy-comboRect.bottom-size, SWP_NOZORDER);
		}
		else
			m_pListCtrl->SetWindowPos( NULL, 0, comboRect.bottom, cx, cy-comboRect.bottom, SWP_NOZORDER);
		
		m_pRemoteViewHeader->m_pEdit->SetParent(this);
		m_pRemoteViewHeader->m_pLabelBackground->SetParent(this);
		m_pRemoteViewHeader->m_pLabel->SetParent(this);
	
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
	else
		if (m_pOwner->m_wndStatusBar.GetSafeHwnd())
		{
			CRect windowRect;
			m_pOwner->m_wndStatusBar.GetWindowRect(&windowRect);
			int size = 0;
			if (m_bShowStatusbar)
			{
				size = windowRect.bottom - windowRect.top;
				m_pStatusBar->SetWindowPos( NULL, 0, cy-size, cx, size+1, SWP_NOZORDER);
			}
			m_pListCtrl->SetWindowPos( NULL, 0, 0, cx, cy-size, SWP_NOZORDER);
		}
		else
			m_pListCtrl->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER);

	m_pStatusBar->ShowWindow(m_bShowStatusbar?SW_SHOW:SW_HIDE);
}

BOOL CFtpView::SetStatusBarText(LPCTSTR pszText)
{
	ASSERT(pszText);
	m_pStatusBar->SetPaneText(0, pszText);
	
	return TRUE;
}

BOOL CFtpView::IsStatusbarEnabled()
{
	return m_bShowStatusbar;
}

void CFtpView::EnableStatusbar(BOOL bEnable)
{
	m_bShowStatusbar = bEnable;
	UpdateViewHeader();
}

BOOL CFtpView::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}
