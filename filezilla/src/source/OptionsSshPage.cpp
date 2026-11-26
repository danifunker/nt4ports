// OptionsSshPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsSshPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsSshPage 


COptionsSshPage::COptionsSshPage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsSshPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsSshPage)
	m_nCompression = -1;
	m_nProtocol = -1;
	//}}AFX_DATA_INIT
}


void COptionsSshPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsSshPage)
	DDX_Radio(pDX, IDC_OPTIONS_SSHPAGE_COMPRESSION1, m_nCompression);
	DDX_Radio(pDX, IDC_OPTIONS_SSHPAGE_PROTOCOL1, m_nProtocol);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsSshPage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsSshPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsSshPage 
