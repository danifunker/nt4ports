// StatusCtrl.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "StatusCtrl.h"
#include "EnterSomething.h"
#include "MainFrm.h"
#include "CommandQueue.h"
#include "statusctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const COLORREF CStatusCtrl::m_ColTable[16] = {RGB(255, 255, 255),
										RGB(0, 0, 0),
										RGB(0, 0, 128),
										RGB(0, 128, 0),
										RGB(255, 0, 0),
										RGB(128, 0, 0),
										RGB(128, 0, 128),
										RGB(128, 128, 0),
										RGB(255, 255, 0),
										RGB(0, 255, 0),
										RGB(0, 128, 128),
										RGB(0, 255, 255),
										RGB(0, 0, 255),
										RGB(255, 0, 255),
										RGB(128, 128, 128),
										RGB(192, 192, 192)
										};

/////////////////////////////////////////////////////////////////////////////
// CStatusCtrl

CStatusCtrl::CStatusCtrl()
{
	m_doPopupCursor = FALSE;
	m_bEmpty = TRUE;
	m_nMoveToBottom = 0;
	m_nTimerID = 0;}

CStatusCtrl::~CStatusCtrl()
{
}

BEGIN_MESSAGE_MAP(CStatusCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(CStatusCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_OUTPUTCONTEXT_CLEARALL, OnOutputcontextClearall)
	ON_COMMAND(ID_OUTPUTCONTEXT_ENTERCOMMAND, OnOutputcontextEntercommand)
	ON_COMMAND(ID_OUTPUTCONTEXT_COPYTOCLIPBOARD, OnOutputcontextCopytoclipboard)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CStatusCtrl 

BOOL CStatusCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}

void CStatusCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	ClientToScreen(&point);

	CMenu menu;
	menu.LoadMenu(IDR_OUTPUTCONTEXT);

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;
	//while (pWndPopupOwner->GetStyle() & WS_CHILD)
	//	pWndPopupOwner = pWndPopupOwner->GetParent();

	CFrameWnd *pFrameWnd = GetParentFrame();
	CMainFrame *pOwner = DYNAMIC_DOWNCAST(CMainFrame, pFrameWnd);
	if (!pOwner->IsConnected())
		pPopup->EnableMenuItem(ID_OUTPUTCONTEXT_ENTERCOMMAND, MF_GRAYED);

	if (!GetLineCount())
	{
		pPopup->EnableMenuItem(ID_OUTPUTCONTEXT_COPYTOCLIPBOARD, MF_GRAYED);
		pPopup->EnableMenuItem(ID_OUTPUTCONTEXT_CLEARALL, MF_GRAYED);
	}
	HCURSOR	hCursor;
	hCursor=AfxGetApp()->LoadStandardCursor( IDC_ARROW );
	m_doPopupCursor = TRUE;
	SetCursor(hCursor);
		
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
}

BOOL CStatusCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (!m_doPopupCursor)
	{
		m_doPopupCursor = 0;
		return CWnd::OnSetCursor(pWnd, nHitTest, message );
	}
	else
		m_doPopupCursor = 0;
	return 0;
}

static DWORD __stdcall RichEditStreamInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	int *pos = (int *)dwCookie;
	char *pBuffer = ((char *)dwCookie) + 4;

	if (!pos || !pBuffer)
		return 0;

	if (cb > static_cast<LONG>(strlen(pBuffer + *pos))) 
		cb = strlen(pBuffer + *pos);

	memcpy(pbBuff, pBuffer + *pos, cb);

	*pcb = cb;

	*pos += cb;

	return 0;
}

void CStatusCtrl::OnOutputcontextClearall() 
{
	USES_CONVERSION;
	
	CString rtfstr = m_RTFHeader;

	rtfstr += "} ";

	char *buffer = new char[rtfstr.GetLength() * 2 + 5];
	strcpy(buffer + 4, T2CA(rtfstr));
	*(int *)buffer = 0;

	EDITSTREAM es;
	es.dwCookie = (DWORD)buffer; // Pass a pointer to the CString to the callback function 
	es.pfnCallback = RichEditStreamInCallback; // Specify the pointer to the callback function.
	
	StreamIn(SF_RTF, es); // Perform the streaming
	SetSel(-1, -1);
	LimitText(1000*1000);
	int res = GetLimitText();
	
	delete [] buffer;
	
	m_bEmpty = TRUE;
	m_nMoveToBottom = 0;
}

void CStatusCtrl::OnOutputcontextEntercommand() 
{
	CEnterSomething dlg(IDS_INPUTDIALOGTITLE_FTPCOMMAND,IDS_INPUTDIALOGTEXT_FTPCOMMAND);
	int res=dlg.DoModal();
	if (res==IDOK)
	{
		CString command=dlg.m_String;
		CFrameWnd *pWnd=GetParentFrame();
		CMainFrame *pOwner=DYNAMIC_DOWNCAST(CMainFrame,pWnd);
		if (!pOwner->m_pCommandQueue->Command(command))
			MessageBeep(MB_ICONEXCLAMATION);
	}
}

void CStatusCtrl::OnOutputcontextCopytoclipboard() 
{
	long nStart, nEnd;
	GetSel(nStart, nEnd);
	if (nStart == nEnd)
	{
		HideSelection(TRUE, FALSE);
		SetSel(0, -1);
		Copy();
		SetSel(nStart, nEnd);
		HideSelection(FALSE, FALSE);
	}
	else
		Copy();
}

#ifdef _UNICODE
BOOL CStatusCtrl::Create(DWORD in_dwStyle,  const RECT& in_rcRect,
                             CWnd* in_pParentWnd, UINT in_nID)
{
    if(!::AfxInitRichEditEx())
    {
        return FALSE ;
    }
    
    CWnd* l_pWnd = this ;
#ifdef _UNICODE
	return l_pWnd->Create(_T( "RichEdit20W" ), NULL, in_dwStyle, in_rcRect, in_pParentWnd, in_nID );
#else
	return l_pWnd->Create(_T( "RichEdit20A" ), NULL, in_dwStyle, in_rcRect, in_pParentWnd, in_nID );
#endif
}

_AFX_RICHEDITEX_STATE::_AFX_RICHEDITEX_STATE()
{
    m_hInstRichEdit20 = NULL ;
}

_AFX_RICHEDITEX_STATE::~_AFX_RICHEDITEX_STATE()
{
    if( m_hInstRichEdit20 != NULL )
    {
        ::FreeLibrary( m_hInstRichEdit20 ) ;
    }
}

_AFX_RICHEDITEX_STATE _afxRichEditStateEx ;

BOOL PASCAL AfxInitRichEditEx()
{
    if( ! ::AfxInitRichEdit() )
    {
        return FALSE ;
    }
    
    _AFX_RICHEDITEX_STATE* l_pState = &_afxRichEditStateEx ;
    
    if( l_pState->m_hInstRichEdit20 == NULL )
    {
        l_pState->m_hInstRichEdit20 = LoadLibraryA("RICHED20.DLL") ;
    }
    
    return l_pState->m_hInstRichEdit20 != NULL ;
}

#endif

int CStatusCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	USES_CONVERSION;

#ifdef _UNICODE
	m_RTFHeader = "{\\rtf1\\ansi\\deff0\\ud";
#else
	m_RTFHeader = "{\\rtf1\\ansi\\deff0";
#endif

	HFONT hSysFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	LOGFONT lf;
	CFont* pFont = CFont::FromHandle( hSysFont );
	pFont->GetLogFont( &lf );

	LOGFONT m_lfFont;
	pFont->GetLogFont(&m_lfFont);
	int pointsize = (-m_lfFont.lfHeight*72/ GetDeviceCaps(GetDC()->GetSafeHdc(), LOGPIXELSY))*2;
	CString fontname = m_lfFont.lfFaceName;
	if (COptions::GetOptionVal(OPTION_MESSAGELOG_USECUSTOMFONT))
	{
		CString fn = COptions::GetOption(OPTION_MESSAGELOG_FONTNAME);
		int pt = COptions::GetOptionVal(OPTION_MESSAGELOG_FONTSIZE);
		if (fn != "")
			fontname = fn;
		if (pt != 0)
			pointsize = pt * 2;
	}
	
	m_RTFHeader += "{\\fonttbl{\\f0\\fnil "+ fontname +";}}";
	m_RTFHeader += "{\\colortbl ;";
	for (int i=0; i<16; i++)
	{
		CString tmp;
		tmp.Format(_T("\\red%d\\green%d\\blue%d;"), GetRValue(m_ColTable[i]), GetGValue(m_ColTable[i]), GetBValue(m_ColTable[i]));
		m_RTFHeader+=tmp;
	}
	m_RTFHeader += "}";
	
	CString tmp;
	tmp.Format(_T("%d"), pointsize);
	m_RTFHeader += "\\uc1\\pard\\fi-1100\\li1100\\tx1100\\f0\\fs"+tmp;

	CString rtfstr = m_RTFHeader;
	rtfstr += "} ";

	char *buffer = new char[rtfstr.GetLength() * 2 + 5];
	strcpy(buffer + 4, T2CA(rtfstr));
	*(int *)buffer = 0;

	EDITSTREAM es;
	es.dwCookie = (DWORD)buffer; // Pass a pointer to the CString to the callback function 
	es.pfnCallback = RichEditStreamInCallback; // Specify the pointer to the callback function.
	
	StreamIn(SF_RTF, es); // Perform the streaming
	SetSel(-1, -1);
	LimitText(1000*1000);
	
	delete [] buffer;

	m_RTFHeader2 = m_RTFHeader;
	m_RTFHeader2.Replace(_T("1100"), _T("1600"));

	return 0;
}

void CStatusCtrl::ShowStatus(CString status, int nType)
{
	USES_CONVERSION;

	bool timestamps = COptions::GetOptionVal(OPTION_LOGTIMESTAMPS) != 0;

	CString rtfstr;
	if (timestamps)
		rtfstr = m_RTFHeader2;
	else
		rtfstr = m_RTFHeader;
	
	status.Replace(_T("\\"), _T("\\\\"));
	status.Replace(_T("{"), _T("\\{"));
	status.Replace(_T("}"), _T("\\}"));
	status.Replace(_T("\r"), _T(""));
	status.Replace(_T("\n"), _T("\\status"));

#ifdef _UNICODE
	int len = status.GetLength();
	int i;
	CString ustatus;
	for (i = 0; i < len; i++) {
		wchar_t x;
		char buf[20];

		x = status[i];
		if (x < 256) {
			ustatus += x;
		}
		else {
			sprintf(buf, "\\u%d.", (signed short)x);
			ustatus += buf;
		}
	}
	status = ustatus;
#endif

	CString stime;
	if (timestamps)
	{
		char buf[100];
		__time64_t t = _time64(0);
		if (!strftime(buf, 100, "[%H:%M:%S] ", _localtime64(&t)))
			buf[0] = 0;
		stime = A2CT(buf);
	}

	CString str;
	switch (nType)
	{
	case FZ_LOG_STATUS:
		str.LoadString(IDS_STATUSMSG_PREFIX);
		str = "\\cf2" + stime + str;
		break;
	case FZ_LOG_ERROR:
		str.LoadString(IDS_ERRORMSG_PREFIX);
		str = "\\cf5" + stime + str;
		break;
	case FZ_LOG_COMMAND:
		str.LoadString(IDS_COMMANDMSG_PREFIX);
		str = "\\cf3" + stime + str;
		break;
	case FZ_LOG_REPLY:
		str.LoadString(IDS_RESPONSEMSG_PREFIX);
		str = "\\cf4" + stime + str;
		break;
	case FZ_LOG_LIST:
		str.LoadString(IDS_TRACEMSG_TRACE);
		str = "\\cf11" + stime + str;
		break;
	case FZ_LOG_APIERROR:
	case FZ_LOG_WARNING:
	case FZ_LOG_INFO:
	case FZ_LOG_DEBUG:
		str.LoadString(IDS_TRACEMSG_TRACE);
		str = "\\cf7" + stime + str;
		break;
	}
	
	status = str + "\\tab " + status;

	if (!m_bEmpty)
		rtfstr += "\\par " + status;
	else
	{
		m_bEmpty = FALSE;
		rtfstr += status;
	}
	
	rtfstr += "} ";

	char *buffer = new char[rtfstr.GetLength() * 2 + 5]; //Make it large enough to hold unicode data
	strcpy(buffer + 4, T2CA(rtfstr));
	*(int *)buffer = 0;

	EDITSTREAM es;

	es.dwCookie = (DWORD)buffer; // Pass a pointer to the CString to the callback function 
	es.pfnCallback = RichEditStreamInCallback; // Specify the pointer to the callback function.

	CWnd *pFocusWnd = GetFocus();
	if (pFocusWnd && pFocusWnd == this)
		AfxGetMainWnd()->SetFocus();
	
	long nStart, nEnd;
	GetSel(nStart, nEnd);
	if (nStart != -1 && nStart == nEnd && LineFromChar(nStart) == (GetLineCount() - 1))
	{
		nStart = -1;
		nEnd = -1;
	}
	BOOL nScrollToEnd = FALSE;
	
	int num = 0;            //this is the number of visible lines
	CRect rect;
	GetRect(rect);
	int height = rect.Height();
	
	for (int i = GetFirstVisibleLine();
			i < GetLineCount() && GetCharPos(LineIndex(i)).y < height;
			i++)
		num++;


	if (GetFirstVisibleLine() + num+m_nMoveToBottom >= GetLineCount())
		nScrollToEnd = TRUE;
	HideSelection(TRUE, FALSE);
	SetSel(-1, -1);
	StreamIn(SF_RTF | SFF_SELECTION, es); // Perform the streaming

	if (GetLineCount() > 1000)
	{
		nStart -= LineLength(0) + 2;
		nEnd -= LineLength(0) + 2;
		if (nStart < 0)
			nEnd = 0;
		if (nEnd < 0)
			nEnd = 0;
		SetSel(0, LineLength(0) + 2);
		ReplaceSel(_T(""));
	}

	SetSel(nStart, nEnd);
	
	if (pFocusWnd && pFocusWnd == this)
		SetFocus();

	HideSelection(FALSE, FALSE);
	if (nScrollToEnd)
	{
		if (nStart != nEnd && (LineFromChar(nStart) >= GetFirstVisibleLine() && LineFromChar(nStart) <= GetFirstVisibleLine() + num ||
							   LineFromChar(nEnd) >= GetFirstVisibleLine() && LineFromChar(nEnd) <= GetFirstVisibleLine() + num))
			LineScroll(1);
		else 
		{
			m_nMoveToBottom++;
			if (!m_nTimerID)
				m_nTimerID = SetTimer(654, 25, NULL);
		}
	}

	delete [] buffer;
}

void CStatusCtrl::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == m_nTimerID)
	{
		if (m_nMoveToBottom)
		{
			SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
			m_nMoveToBottom = 0;
		}
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}	
	CRichEditCtrl::OnTimer(nIDEvent);
}

void CStatusCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	OnContextMenu(this, point);
}

BOOL CStatusCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	OSVERSIONINFO info = {0};
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&info);
	if (info.dwMajorVersion >= 5)
		return CRichEditCtrl::OnMouseWheel(nFlags, zDelta, pt);

	LineScroll(-zDelta / 120 * 3);

	return TRUE;
}

void CStatusCtrl::ChangeFont(void)
{
#ifdef _UNICODE
	m_RTFHeader = "{\\rtf1\\ansi\\deff0\\ud";
#else
	m_RTFHeader = "{\\rtf1\\ansi\\deff0";
#endif

	HFONT hSysFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	LOGFONT lf;
	CFont* pFont = CFont::FromHandle( hSysFont );
	pFont->GetLogFont( &lf );

	LOGFONT m_lfFont;
	pFont->GetLogFont(&m_lfFont);
	int pointsize = (-m_lfFont.lfHeight*72/ GetDeviceCaps(GetDC()->GetSafeHdc(), LOGPIXELSY))*2;
	CString fontname = m_lfFont.lfFaceName;
	if (COptions::GetOptionVal(OPTION_MESSAGELOG_USECUSTOMFONT))
	{
		CString fn = COptions::GetOption(OPTION_MESSAGELOG_FONTNAME);
		int pt = COptions::GetOptionVal(OPTION_MESSAGELOG_FONTSIZE);
		if (fn != "")
			fontname = fn;
		if (pt != 0)
			pointsize = pt * 2;
	}
	
	m_RTFHeader += "{\\fonttbl{\\f0\\fnil "+ fontname +";}}";
	m_RTFHeader += "{\\colortbl ;";
	for (int i=0; i<16; i++)
	{
		CString tmp;
		tmp.Format(_T("\\red%d\\green%d\\blue%d;"), GetRValue(m_ColTable[i]), GetGValue(m_ColTable[i]), GetBValue(m_ColTable[i]));
		m_RTFHeader+=tmp;
	}
	m_RTFHeader += "}";
	
	CString tmp;
	tmp.Format(_T("%d"), pointsize);
	m_RTFHeader += "\\uc1\\pard\\fi-1100\\li1100\\tx1100\\f0\\fs"+tmp;

	m_RTFHeader2 = m_RTFHeader;
	m_RTFHeader2.Replace(_T("1100"), _T("1600"));
}
