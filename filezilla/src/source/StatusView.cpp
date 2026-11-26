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

// StatusView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "FileZilla.h"
#include "StatusView.h"
#include "StatusCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusView

IMPLEMENT_DYNCREATE(CStatusView, CView)

CStatusView::CStatusView()
{
	m_pLogFile = 0;
	m_pRichEditCtrl = new CStatusCtrl();
}

CStatusView::~CStatusView()
{
	delete m_pRichEditCtrl;
	m_pRichEditCtrl = NULL;
	delete m_pLogFile;
	m_pLogFile = NULL;
}

BEGIN_MESSAGE_MAP(CStatusView, CView)
	//{{AFX_MSG_MAP(CStatusView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CStatusView 

void CStatusView::OnDraw(CDC* pDC)
{
	// Nothing to do here
}

/////////////////////////////////////////////////////////////////////////////
// Diagnose CStatusView

#ifdef _DEBUG
void CStatusView::AssertValid() const
{
	CView::AssertValid();
}

void CStatusView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

DWORD __stdcall RichEditStreamInCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	int *pos = (int *)dwCookie;
	char *pBuffer = ((char *)dwCookie) + 4;

	if (cb > static_cast<LONG>(strlen(pBuffer + *pos))) 
		cb = strlen(pBuffer + *pos);

	memcpy(pbBuff, pBuffer + *pos, cb);

	*pcb = cb;

	*pos += cb;

	return 0;
}

int CStatusView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	USES_CONVERSION;
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the style
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_VSCROLL | ES_AUTOVSCROLL | ES_NOHIDESEL;
	
	// Custom initialization of the richedit control
#if _MFC_VER > 0x0600
	VERIFY(AfxInitRichEdit() || AfxInitRichEdit2());
#else
	VERIFY(AfxInitRichEdit());
#endif

	CWnd* pWnd = m_pRichEditCtrl;
	BOOL bResult = pWnd->Create(RICHEDIT_CLASS, NULL, dwStyle, CRect(1, 1, 10, 10), this, 0);
	if (!bResult)
		bResult = pWnd->Create(_T(RICHEDIT_CLASS10A), NULL, dwStyle, CRect(1, 1, 10, 10), this, 0);

	UpdateLogFile();

	return (bResult ? 0 : -1);
}


// Override OnSize to resize the control to match the view
void CStatusView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_pRichEditCtrl->m_hWnd))
		m_pRichEditCtrl->MoveWindow(0, 0, cx, cy, TRUE);
}//OnSize

void CStatusView::ShowStatus(CString status, int type)
{
	USES_CONVERSION;
	
	if (m_pRichEditCtrl)
		m_pRichEditCtrl->ShowStatus(status, type);

	if (m_pLogFile)
	{
		m_pLogFile->Seek(-1, CFile::current);

		CString tmp=status;
		tmp.Replace(_T("\\"), _T("\\\\"));
		tmp.Replace(_T("{"), _T("\\{"));
		tmp.Replace(_T("}"), _T("\\}"));
		tmp.Replace(_T("\r"), _T(""));
		tmp.Replace(_T("\n"), _T("\\line"));

		CString stime;
		if (COptions::GetOptionVal(OPTION_LOGTIMESTAMPS))
		{
			char buf[100];
			__time64_t t = _time64(0);
			if (!strftime(buf, 100, "[%H:%M:%S] ", _localtime64(&t)))
				buf[0] = 0;
			CString stime = A2CT(buf);
		}
		
		CString str;
		switch (type)
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
		LPCSTR lpszAscii=T2CA(str);
		m_pLogFile->Write(lpszAscii, strlen(lpszAscii));
		
		str="\\tab "+tmp+"\\par}";
		lpszAscii=T2CA(str);
		m_pLogFile->Write(lpszAscii, strlen(lpszAscii));
	}
}

void CStatusView::UpdateLogFile()
{
	USES_CONVERSION;
	if (m_pLogFile)
	{
		delete m_pLogFile;
		m_pLogFile=0;
	}
	if (COptions::GetOptionVal(OPTION_DEBUGLOGTOFILE))
	{
		m_pLogFile=new CFile;
		CString filename=COptions::GetOption(OPTION_DEBUGLOGFILE);
		BOOL res=m_pLogFile->Open(filename,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyWrite);
		if (res)
		{
			_int64 len=GetLength64(*m_pLogFile);
			if (!len)
			{
				CString rtfstr;
				rtfstr="{\\rtf1\\ansi\\deff0";
				
				HFONT hSysFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
				
				LOGFONT lf;
				CFont* pFont = CFont::FromHandle( hSysFont );
				pFont->GetLogFont( &lf );
				
				LOGFONT m_lfFont;
				pFont->GetLogFont(&m_lfFont);
				
				rtfstr+="{\\fonttbl{\\f0\\fnil "+CString(m_lfFont.lfFaceName)+";}}";
				rtfstr+="{\\colortbl ;";
				for (int i=0;i<16;i++)
				{
					CString tmp;
					tmp.Format(_T( "\\red%d\\green%d\\blue%d;"), GetRValue(m_pRichEditCtrl->m_ColTable[i]), GetGValue(m_pRichEditCtrl->m_ColTable[i]), GetBValue(m_pRichEditCtrl->m_ColTable[i]));
					rtfstr+=tmp;
				}
				rtfstr+="}";
				
				int pointsize=(-m_lfFont.lfHeight*72/ GetDeviceCaps(GetDC()->GetSafeHdc(), LOGPIXELSY))*2;
				CString tmp;
				tmp.Format(_T("%d"), pointsize);
				if (COptions::GetOptionVal(OPTION_MESSAGELOG_USECUSTOMFONT))
					rtfstr+="\\uc1\\pard\\fi-1600\\li1600\\tx1600\\f0\\fs" + tmp; //180*m_nAvgCharWidth;
				else
					rtfstr+="\\uc1\\pard\\fi-1100\\li1100\\tx1100\\f0\\fs" + tmp; //180*m_nAvgCharWidth;
				LPCSTR lpszAscii=T2CA(rtfstr);
				m_pLogFile->Write(lpszAscii, strlen(lpszAscii));
			}
			else
			{
				char buffer[2];
				LONG low=static_cast<LONG>((len-1)&0xFFFFFFFF);
				LONG high=static_cast<LONG>((len-1)>>32);
				VERIFY(SetFilePointer((HANDLE)m_pLogFile->m_hFile, low, &high, FILE_BEGIN)!=0xFFFFFFFF || GetLastError()==NO_ERROR);
				m_pLogFile->Read(buffer,1);
				if (buffer[0]!='}')
				{
					m_pLogFile->SetLength(0);
					CString rtfstr;
					rtfstr="{\\rtf1\\ansi";
					
					HFONT hSysFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
					
					LOGFONT lf;
					CFont* pFont = CFont::FromHandle( hSysFont );
					pFont->GetLogFont( &lf );
					
					LOGFONT m_lfFont;
					pFont->GetLogFont(&m_lfFont);
					
					rtfstr+="{\\fonttbl{\\f0\\fnil "+CString(m_lfFont.lfFaceName)+";}}";
					rtfstr+="{\\colortbl ;";
					for (int i=0;i<16;i++)
					{
						CString tmp;
						tmp.Format(_T( "\\red%d\\green%d\\blue%d;"), GetRValue(m_pRichEditCtrl->m_ColTable[i]), GetGValue(m_pRichEditCtrl->m_ColTable[i]), GetBValue(m_pRichEditCtrl->m_ColTable[i]));
						rtfstr+=tmp;
					}
					rtfstr+="}";
					
					int pointsize=(-m_lfFont.lfHeight*72/ GetDeviceCaps(GetDC()->GetSafeHdc(), LOGPIXELSY))*2;
					CString tmp;
					tmp.Format(_T("%d"), pointsize);
					if (COptions::GetOptionVal(OPTION_MESSAGELOG_USECUSTOMFONT))
						rtfstr+="\\uc1\\pard\\fi-1600\\li1600\\tx1600\\f0\\fs" + tmp; //180*m_nAvgCharWidth;
					else
						rtfstr+="\\uc1\\pard\\fi-1100\\li1100\\tx1100\\f0\\fs" + tmp; //180*m_nAvgCharWidth;
					LPCSTR lpszAscii=T2CA(rtfstr);
					m_pLogFile->Write(lpszAscii, strlen(lpszAscii));
				}
				else
				{
					LONG low=static_cast<LONG>((len-1)&0xFFFFFFFF);
					LONG high=static_cast<LONG>((len-1)>>32);
					VERIFY(SetFilePointer((HANDLE)m_pLogFile->m_hFile, low, &high, CFile::begin)!=0xFFFFFFFF || GetLastError()==NO_ERROR);
				}
			}
			CTime datetime=CTime::GetCurrentTime();
			CString tmp=datetime.FormatGmt( _T("%m/%d/%Y %H:%M:%S") );
			int x=GetLastError();
			CString str;
			str.Format(_T("\\cf8FileZilla started (%s)\\par}"), tmp);
			LPCSTR lpszAscii=T2CA(str);
			m_pLogFile->Write(lpszAscii, strlen(lpszAscii));
		}
		else
		{
			delete m_pLogFile;
			m_pLogFile=0;
			CString str;
			str.Format(IDS_ERRORMSG_CANTOPENLOGFILE,filename);
			AfxMessageBox(str, MB_ICONEXCLAMATION);
		}
	}

	if (m_pRichEditCtrl)
		m_pRichEditCtrl->ChangeFont();
}

BOOL CStatusView::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}

void CStatusView::SetFocus()
{
	m_pRichEditCtrl->SetFocus();
}

const CWnd *CStatusView::GetEditCtrl() const
{
	return m_pRichEditCtrl;
}
