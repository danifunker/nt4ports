// TransparentDialogBar.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "..\filezilla.h"
#include "TransparentDialogBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransparentDialogBar

CTransparentDialogBar::CTransparentDialogBar()
{
	LOGBRUSH lb;
	lb.lbStyle = BS_NULL;
	m_Brush.CreateBrushIndirect(&lb);            
}

CTransparentDialogBar::~CTransparentDialogBar()
{
}


BEGIN_MESSAGE_MAP(CTransparentDialogBar, CDialogBar)
	//{{AFX_MSG_MAP(CTransparentDialogBar)
	ON_WM_ERASEBKGND()
	ON_WM_MOVE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTransparentDialogBar 

#ifdef FZ_NT4
// Transparent only in a rebar: on NT 4.0 without one (MainFrm.cpp) the bar sits on the
// frame, which paints nothing behind it, so it paints itself as dialog bars do.
static BOOL InReBar(CWnd *pWnd)
{
	TCHAR name[32];
	CWnd *pParent = pWnd->GetParent();
	return pParent && ::GetClassName(pParent->m_hWnd, name, 32) && !_tcsicmp(name, REBARCLASSNAME);
}
#endif

BOOL CTransparentDialogBar::OnEraseBkgnd(CDC* pDC) 
{
#ifdef FZ_NT4
	if (!InReBar(this))
		return CDialogBar::OnEraseBkgnd(pDC);
#endif
	pDC->SetBkColor(TRANSPARENT);
	CWnd* pParent = GetParent(); 
	ASSERT_VALID(pParent); 
	CPoint pt(0, 0); 
	MapWindowPoints(pParent, &pt, 1); 
	pt = pDC->OffsetWindowOrg(pt.x, pt.y); 
	LRESULT lResult = pParent->SendMessage(WM_ERASEBKGND, 
		(WPARAM)pDC->m_hDC, 0L); 
	pDC->SetWindowOrg(pt.x, pt.y); 
	return lResult;
}

void CTransparentDialogBar::OnMove(int x, int y) 
{
	Invalidate(); 
}

HBRUSH CTransparentDialogBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	// Call the base class implementation first! Otherwise, it may
	// undo what we're trying to accomplish here.
	HBRUSH hbr = CDialogBar::OnCtlColor(pDC, pWnd, nCtlColor);
#ifdef FZ_NT4
	if (!InReBar(this))
		return hbr;
#endif

	switch (nCtlColor)
	{
		case CTLCOLOR_BTN:
		case CTLCOLOR_STATIC:
		{
			pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			pDC->SetBkMode(TRANSPARENT);
		}
		case CTLCOLOR_DLG:
		{
		   return (HBRUSH) (m_Brush.m_hObject);
		}
	}
	return hbr;
}
