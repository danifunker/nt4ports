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

// FileZilla.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "FileZilla.h"

#include "MainFrm.h"
#include "misc\hyperlink.h"
#include "sitemanager.h"
#include "PathFunctions.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileZillaApp

BEGIN_MESSAGE_MAP(CFileZillaApp, CWinApp)
	//{{AFX_MSG_MAP(CFileZillaApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileZillaApp Konstruktion

CFileZillaApp::CFileZillaApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CFileZillaApp-Objekt

CFileZillaApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFileZillaApp Initialisierung

BOOL CFileZillaApp::InitInstance()
{
	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.

#if _MSC_VER < 1300
#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else //_AFXDLL
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif //_AFXDLL
#endif //_MSC_VER < 1300

	
	// initialize Winsock library
	BOOL res=TRUE;
	WSADATA wsaData;
	
	WORD wVersionRequested = MAKEWORD(1, 1);
	int nResult = WSAStartup(wVersionRequested, &wsaData);
	if (nResult != 0)
		res=FALSE;
	else if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		res=FALSE;
	}

	if(!res)
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED,MB_ICONSTOP);
		return FALSE;
	}

	TCHAR buffer[MAX_PATH + 1000]; //Make it large enough
	GetModuleFileName( 0, buffer, MAX_PATH );
	m_appPath=buffer;
	PathRemoveArgs(m_appPath);
	PathUnquoteSpaces(m_appPath);
	PathRemoveFileSpec(m_appPath);

	CString cmdLine = m_lpCmdLine;
	bool bConfig = false;
	CString config = "";
	while (cmdLine != "")
	{
		int pos = cmdLine.Find(' ');
		CString token;
		if (pos == -1)
		{
			token = cmdLine;
			cmdLine = "";
		}
		else
		{
			token = cmdLine.Left(pos);
			cmdLine = cmdLine.Mid(pos + 1);
		}

		cmdLine.TrimLeft(' ');
		if (!bConfig)
		{
			token.MakeLower();
			if (token == "/quit" || token == "-quit")
			{
				HWND hwnd;
				while (hwnd = ::FindWindow(_T("FileZilla Main Window"), 0))
				{
					PostMessage(hwnd, WM_CLOSE, 0, 0);
					Sleep(100);
				}
				return FALSE;
			}

			if (token == "/config" || token == "-config")
				bConfig = true;
		}
		else
		{
			bool bQuoted = false;
			if (token.Left(1) == _T("\""))
			{
				token = token.Mid(1);
				bQuoted = true;
			}
			if (token.Right(1) == "\"")
			{
				token = token.Left(token.GetLength() - 1);
				bQuoted = false;
			}
			config += token;

			if (bQuoted)
			{
				config += _T(" ");
				pos = cmdLine.Find('\"');
				if (pos == -1)
					config += cmdLine;
				else
					config += cmdLine.Left(pos);
			}
			break;
		}
	}
	if (config != _T(""))
	{
		int pos;
		while ((pos = config.Find(_T("%"))) != -1)
		{
			int pos2 = config.Find(_T("%"), pos + 1);
			if (pos2 == -1)
				break;

			CString var;
			var.GetEnvironmentVariable(config.Mid(pos + 1, pos2 - pos - 1));
			config = config.Left(pos) + var + config.Mid(pos2 + 1);
		}
		COptions::SetConfig(config);
	}
	
	if (!InitLanguage())
		return FALSE;

	COptions::CheckUseXML();

	if (!COptions::InitUDRules())
		return FALSE;

	// Change the help file path
	GetModuleFileName( 0, buffer, MAX_PATH );
	CString helpFile=buffer;
	PathRemoveArgs(helpFile);
	PathUnquoteSpaces(helpFile);
	PathRemoveFileSpec(helpFile);

	CFileStatus64 status;
	if (COptions::GetOption(OPTION_LANGUAGE) == _T("Français") && GetStatus64(helpFile + _T("FileZillaFrench.chm"), status) && !(status.m_attribute&FILE_ATTRIBUTE_DIRECTORY))
		helpFile += _T("FileZillaFrench.chm");
	else
		helpFile += _T("FileZilla.chm");
	free((void*)m_pszHelpFilePath);
	m_pszHelpFilePath = _tcsdup(helpFile);

	// Dokumentvorlagen der Anwendung registrieren. Dokumentvorlagen
	//  dienen als Verbindung zwischen Dokumenten, Rahmenfenstern und Ansichten.

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// Rahmen mit Ressourcen erstellen und laden

	if (pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW, NULL,
		NULL))
	{
		// Das einzige Fenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
		pFrame->ShowWindow(SW_SHOW);
		pFrame->UpdateWindow();
	}

#ifdef _DEBUG
	AfxCheckMemory();
#endif

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialog für Info über Anwendung

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogdaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
//	CStatic	m_cDonate;
	CStatic	m_TranslatedBy;
	CHyperLink	m_mail2;
	CStatic	m_Version;
	CHyperLink m_mail;
	CHyperLink m_homepage;
	//}}AFX_DATA

	// Überladungen für virtuelle Funktionen, die vom Anwendungs-Assistenten erzeugt wurden
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	CHyperLink m_cDonateLink;

	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_TRANSLATEDBY, m_TranslatedBy);
	DDX_Control(pDX, IDC_MAIL2, m_mail2);
	DDX_Control(pDX, IDC_VERSION, m_Version);
	DDX_Control(pDX, IDC_MAIL, m_mail);
	DDX_Control(pDX, IDC_HOMEPAGE, m_homepage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Anwendungsbefehl zum Ausführen des Dialogfelds
void CFileZillaApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_homepage.ModifyLinkStyle(0, CHyperLink::StyleUseHover);
	m_homepage.SetColors(0xFF0000, 0xFF0000, 
				   0xFF0000, 0xFF);
	m_mail.ModifyLinkStyle(0, CHyperLink::StyleUseHover);
	m_mail.SetColors(0xFF0000, 0xFF0000, 
				   0xFF0000, 0xFF);
	m_mail.SetURL( _T("mailto:Tim.Kosse@gmx.de") );

	m_mail2.ModifyLinkStyle(0, CHyperLink::StyleUseHover);
	m_mail2.SetColors(0xFF0000, 0xFF0000, 
				   0xFF0000, 0xFF);
	CString mail2;
	m_mail2.GetWindowText(mail2);
	if (mail2!=_T("<your name> (<your e-mail>)") )
	{
		mail2.TrimRight( _T(")") );
		m_mail2.ShowWindow(SW_SHOW);
		m_TranslatedBy.ShowWindow(SW_SHOW);
		int pos=mail2.Find( _T("(") );
		if (pos!=-1)
		{
			m_mail2.SetWindowText(mail2.Left(pos-1));
			m_mail2.SetURL(_T("mailto:") + mail2.Mid(pos+1));
		}
	}

	m_Version.SetWindowText(GetVersionString());
	
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	CStatic *ctrl=((CStatic *)GetDlgItem(IDC_FZICON));
	ctrl->SetIcon(hIcon);

	HBITMAP hDonate = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_DONATE));
	m_cDonateLink.SubclassDlgItem(IDC_DONATE, this, _T("http://filezilla.sourceforge.net/donate.php"));
	m_cDonateLink.SetBitmap(hDonate);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BOOL CFileZillaApp::InitLanguage()
{
	unsigned __int64 version=0;
	m_bLangSet=FALSE;
	#ifdef _AFXDLL
		new CDynLinkLibrary( AfxGetInstanceHandle(), AfxGetInstanceHandle() );
	#endif
	CString lang=COptions::GetOption(OPTION_LANGUAGE);
	if (lang=="")
		lang="English";
	if (lang=="English")
		return TRUE;
	
	CFileFind find;
	if (find.FindFile(((CFileZillaApp *)AfxGetApp())->m_appPath+"\\*.dll"))
	{
		BOOL bFind=TRUE;
		while(bFind)
		{
			bFind=find.FindNextFile();
			CString fn=find.GetFileName();
			DWORD tmp=0;
			LPTSTR str=new TCHAR[fn.GetLength()+1];
			_tcscpy(str,fn);
			DWORD len=GetFileVersionInfoSize(str,&tmp);
			LPVOID pBlock=new char[len];
			if (GetFileVersionInfo(str,0,len,pBlock))
			{
				LPVOID ptr=0;
				UINT ptrlen;
	
				TCHAR SubBlock[50];
				
				// Structure used to store enumerated languages and code pages.
				struct LANGANDCODEPAGE {
				WORD wLanguage;
				WORD wCodePage;
				} *lpTranslate;
	
				UINT cbTranslate;
				
				// Read the list of languages and code pages.
				if (VerQueryValue(pBlock, 
							TEXT("\\VarFileInfo\\Translation"),
							(LPVOID*)&lpTranslate,
							&cbTranslate))
				{
					// Read the file description for each language and code page.
				
					_stprintf( SubBlock, 
						_T("\\StringFileInfo\\%04x%04x\\ProductName"),
						lpTranslate[0].wLanguage,
						lpTranslate[0].wCodePage);
					// Retrieve file description for language and code page "0". 
					if (VerQueryValue(pBlock, 
							SubBlock, 
							&ptr, 
								&ptrlen))
					{
						LPTSTR pname=(LPTSTR)ptr;
						CString productname=pname;
						if (productname=="FileZilla Language DLL")
						{
							_stprintf( SubBlock, 
							_T("\\StringFileInfo\\%04x%04x\\Comments"),
							lpTranslate[0].wLanguage,
							lpTranslate[0].wCodePage);
					
							if (VerQueryValue(pBlock, 
								SubBlock, 
								&ptr, 
								&ptrlen))
							{
								LPTSTR comment=(LPTSTR)ptr;
								CString Comment=comment;
								if (Comment.Find(lang) != -1)
								{
									if (VerQueryValue(pBlock,_T("\\"),&ptr,&ptrlen))
									{
										VS_FIXEDFILEINFO *fi=(VS_FIXEDFILEINFO*)ptr;
										unsigned __int64 curver=(((__int64)fi->dwFileVersionMS)<<32)+fi->dwFileVersionLS;
										if (curver>version)
										{
											version=curver;
											if (version>=MINVALIDDLLVERSION)
											{
												//Set the new language
												HINSTANCE dll=LoadLibrary(fn);
												if (dll)
													AfxSetResourceHandle(dll);
												m_bLangSet=TRUE;
											}
										}
									}
								}
							}
						}
					}
				}
			}
			delete [] str;
			delete [] pBlock;
		}
	}
	if (version)
		if (version<MINVALIDDLLVERSION)
			AfxMessageBox(IDS_ERRORMSG_LANGUAGEDLLVERSIONINVALID);
		else if (version<MINDLLVERSION)
			AfxMessageBox(IDS_STATUSMSG_LANGUAGEVERSIONDIFFERENT);

	if (m_bLangSet)
		return TRUE;
	CString str;
	str.Format(IDS_ERRORMSG_LANGUAGEDLLNOTFOUND,lang);
	AfxMessageBox(str);
	COptions::SetOption(OPTION_LANGUAGE,"English");
	return TRUE;
}

int CFileZillaApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	COptions::ClearStaticOptions();

	HMODULE hDLL = AfxGetResourceHandle();
	if (hDLL != AfxGetInstanceHandle())
	{
		AfxSetResourceHandle(AfxGetInstanceHandle());
		FreeLibrary(hDLL);
	}

	return CWinApp::ExitInstance();
}
