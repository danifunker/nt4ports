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

// LocalView2.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "FileZilla.h"
#include "LocalView2.h"
#include "LocalFileListCtrl.h"
#include "Mainfrm.h"
#include "misc\ComboCompletion.h"
#include "misc\VisualStylesXP.h"
#include "FileChangeNotify.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocalView2

static UINT indicators[] =
{
	ID_SEPARATOR
};

IMPLEMENT_DYNCREATE(CLocalView2, CView)

CLocalView2::CLocalView2()
{
	m_pFileNotify = NULL;
	m_pOwner = (CMainFrame *)AfxGetMainWnd();
	m_pLocalViewHeader = m_pOwner->m_pLocalViewHeader;
	m_pListCtrl = 0;
	m_pStatusBar = NULL;
	m_changedFilelist = 0;
}

CLocalView2::~CLocalView2()
{
	if (m_pFileNotify)
		delete m_pFileNotify;
	if (m_pListCtrl)
		delete m_pListCtrl;
	if (m_pStatusBar)
		delete m_pStatusBar;
}


BEGIN_MESSAGE_MAP(CLocalView2, CView)
	//{{AFX_MSG_MAP(CLocalView2)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_REQUEST_REFRESH, OnRequestRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CLocalView2 

void CLocalView2::OnDraw(CDC* pDC)
{
	if (m_pLocalViewHeader->bTreeHidden)
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
// Diagnose CLocalView2

#ifdef _DEBUG
void CLocalView2::AssertValid() const
{
	CView::AssertValid();
}

void CLocalView2::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CLocalView2 
	
void CLocalView2::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
 	CVisualStylesXP xp;
   	int themeoffset = 0;
	if (xp.IsAppThemed())
		themeoffset = -1;

	if (m_pLocalViewHeader->bTreeHidden)
	{
		CRect comboRect;
		m_pLocalViewHeader->m_pEdit->GetClientRect(comboRect);

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


		m_pLocalViewHeader->m_pEdit->SetParent(this);
		m_pLocalViewHeader->m_pLabelBackground->SetParent(this);
		m_pLocalViewHeader->m_pLabel->SetParent(this);
	
		if (!COptions::GetOptionVal(OPTION_SHOWNOLABEL))
		{
			m_pLocalViewHeader->m_pLabelBackground->ShowWindow(SW_SHOW);
			m_pLocalViewHeader->m_pLabel->ShowWindow(SW_SHOW);
			
			m_pLocalViewHeader->m_pLabelBackground->SetWindowPos( NULL, 0, 0, m_pLocalViewHeader->m_LabelTextSize.cx, comboRect.bottom-1, SWP_NOZORDER);
			m_pLocalViewHeader->m_pLabel->SetWindowPos( m_pLocalViewHeader->m_pLabelBackground, 0, 10-m_pLocalViewHeader->m_LabelTextSize.cy/2-1, m_pLocalViewHeader->m_LabelTextSize.cx, comboRect.bottom-1-(10-m_pLocalViewHeader->m_LabelTextSize.cy/2-1), 0);
			m_pLocalViewHeader->m_pEdit->SetWindowPos( NULL, m_pLocalViewHeader->m_LabelTextSize.cx, 0, cx-m_pLocalViewHeader->m_LabelTextSize.cx+2 + themeoffset, comboRect.bottom, SWP_NOZORDER);
		}
		else
		{
			m_pLocalViewHeader->m_pLabelBackground->ShowWindow(SW_HIDE);
			m_pLocalViewHeader->m_pLabel->ShowWindow(SW_HIDE);
		
			m_pLocalViewHeader->m_pEdit->SetWindowPos( NULL, 0, 0, cx, comboRect.bottom, SWP_NOZORDER);
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

int CLocalView2::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pListCtrl=new CLocalFileListCtrl(this);
	m_pListCtrl->Create(LVS_REPORT|WS_CHILD | WS_VISIBLE | LVS_SHOWSELALWAYS|LVS_SHAREIMAGELISTS|LVS_EDITLABELS|LVS_OWNERDATA,CRect(0,0,0,0),this,0);

	m_pStatusBar = new CStatusBar;
	m_pStatusBar->Create(this);
	m_pStatusBar->SetIndicators(indicators, 1);
	m_pStatusBar->SetPaneStyle(0, SBPS_STRETCH );
	m_bShowStatusbar = COptions::GetOptionVal(OPTION_SHOWLOCALSTATUSBAR);
	m_pStatusBar->ShowWindow(m_bShowStatusbar?SW_SHOW:SW_HIDE);

	return 0;
}

void CLocalView2::SetLocalFolderOut(CString folder)
{
	m_pOwner->SetLocalFolder(folder);
}

void CLocalView2::SetLocalFolder(CString folder)
{
	if (m_pFileNotify != NULL) {
		delete m_pFileNotify;
		m_pFileNotify = NULL;
	}
	m_pFileNotify = new CFileChangeNotify;
	m_pFileNotify->AddWatch(folder, FALSE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);
	m_pFileNotify->AddWatch(folder, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE);
	m_pFileNotify->m_notifyWnd = m_hWnd;
	m_pFileNotify->StartNotify();

	m_pListCtrl->SetFolder(folder);
}

CString CLocalView2::GetLocalFolder()
{
	return m_pListCtrl->GetFolder();
}

CWnd* CLocalView2::GetListCtrl()
{
	return m_pListCtrl;
}

void CLocalView2::UpdateViewHeader()
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
	
	CFont *pOldFont=pDC->SelectObject(m_pLocalViewHeader->m_pLabel->GetFont());
	GetTextExtentPoint32( pDC->GetSafeHdc(), str, str.GetLength(), &m_pLocalViewHeader->m_LabelTextSize );
	pDC->SelectObject(pOldFont);
	
	m_pLocalViewHeader->m_pLabel->ReleaseDC(pDC);

	int cx,cy;
	RECT rect;
	GetClientRect(&rect);
	cx=rect.right;
	cy=rect.bottom;
	if (m_pLocalViewHeader->bTreeHidden)
	{
		CRect comboRect;
		m_pLocalViewHeader->m_pEdit->GetClientRect(comboRect);

		if (m_pOwner->m_wndStatusBar.GetSafeHwnd())
		{
			CRect windowRect;
			m_pOwner->m_wndStatusBar.GetWindowRect(&windowRect);
			int size = 0;
			if (m_bShowStatusbar)
			{
				size = windowRect.bottom - windowRect.top + 1;
				m_pStatusBar->SetWindowPos( NULL, 0, cy-size, cx, size+1, SWP_NOZORDER);
			}
			m_pListCtrl->SetWindowPos( NULL, 0, comboRect.bottom, cx, cy-comboRect.bottom-size, SWP_NOZORDER);
		}
		else
			m_pListCtrl->SetWindowPos( NULL, 0, comboRect.bottom, cx, cy-comboRect.bottom, SWP_NOZORDER);
		
		m_pLocalViewHeader->m_pEdit->SetParent(this);
		m_pLocalViewHeader->m_pLabelBackground->SetParent(this);
		m_pLocalViewHeader->m_pLabel->SetParent(this);
	
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

void CLocalView2::RefreshFile(CString file)
{
	m_pListCtrl->RefreshFile(file);
}

BOOL CLocalView2::SetStatusBarText(LPCTSTR pszText)
{
	ASSERT(pszText);
	m_pStatusBar->SetPaneText(0, pszText);
	
	return TRUE;
}

BOOL CLocalView2::IsStatusbarEnabled()
{
	return m_bShowStatusbar;
}

void CLocalView2::EnableStatusbar(BOOL bEnable)
{
	m_bShowStatusbar = bEnable;
	UpdateViewHeader();
}

BOOL CLocalView2::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}

LRESULT CLocalView2::OnRequestRefresh(WPARAM wParam, LPARAM lParam)
{
	if (!wParam)
	{
		m_pListCtrl->SetFolder(GetLocalFolder());
		m_changedFilelist = false;
	}
	else
	{
		m_changedFilelist = true;
	}
	return 0;
}

void CLocalView2::CheckAutoRefresh()
{
	if (m_changedFilelist)
		m_pListCtrl->SetFolder(GetLocalFolder());
	m_changedFilelist = false;
}
