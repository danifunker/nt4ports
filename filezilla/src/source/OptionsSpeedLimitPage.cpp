// OptionsSpeedLimitPage.cpp : implementation file
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsSpeedLimitPage.h"
#include "SpeedLimitRuleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsSpeedLimitPage dialog


COptionsSpeedLimitPage::COptionsSpeedLimitPage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsSpeedLimitPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsSpeedLimitPage)
	m_DownloadSpeedLimitType = -1;
	m_UploadSpeedLimitType = -1;
	m_DownloadValue = 0;
	m_UploadValue = 0;
	//}}AFX_DATA_INIT
}

COptionsSpeedLimitPage::~COptionsSpeedLimitPage()
{
	unsigned int i;
	for (i = 0; i < m_DownloadSpeedLimits.size(); i++)
		delete m_DownloadSpeedLimits[i];
	
	for (i = 0; i < m_UploadSpeedLimits.size(); i++)
		delete m_UploadSpeedLimits[i];
	
	m_UploadSpeedLimits.clear();
}

void COptionsSpeedLimitPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsSpeedLimitPage)
	DDX_Control(pDX, IDC_SPEEDLIMIT_DOWNLOAD_UP, m_DownloadUpCtrl);
	DDX_Control(pDX, IDC_SPEEDLIMIT_DOWNLOAD_RULES_LIST, m_DownloadRulesListCtrl);
	DDX_Control(pDX, IDC_SPEEDLIMIT_DOWNLOAD_REMOVE, m_DownloadRemoveCtrl);
	DDX_Control(pDX, IDC_SPEEDLIMIT_DOWNLOAD_DOWN, m_DownloadDownCtrl);
	DDX_Control(pDX, IDC_SPEEDLIMIT_DOWNLOAD_ADD, m_DownloadAddCtrl);
	DDX_Control(pDX, IDC_SPEEDLIMIT_UPLOAD_UP, m_UploadUpCtrl);
	DDX_Control(pDX, IDC_SPEEDLIMIT_UPLOAD_RULES_LIST, m_UploadRulesListCtrl);
	DDX_Control(pDX, IDC_SPEEDLIMIT_UPLOAD_REMOVE, m_UploadRemoveCtrl);
	DDX_Control(pDX, IDC_SPEEDLIMIT_UPLOAD_DOWN, m_UploadDownCtrl);
	DDX_Control(pDX, IDC_SPEEDLIMIT_UPLOAD_ADD, m_UploadAddCtrl);
	DDX_Control(pDX, IDC_SPEEDLIMIT_UPLOAD_VALUE, m_UploadValueCtrl);
	DDX_Control(pDX, IDC_SPEEDLIMIT_DOWNLOAD_VALUE, m_DownloadValueCtrl);
	DDX_Radio(pDX, IDC_SPEEDLIMIT_DOWNLOAD_NOLIMIT, m_DownloadSpeedLimitType);
	DDX_Radio(pDX, IDC_SPEEDLIMIT_UPLOAD_NOLIMIT, m_UploadSpeedLimitType);
	DDX_Text(pDX, IDC_SPEEDLIMIT_DOWNLOAD_VALUE, m_DownloadValue);
	DDV_MinMaxInt(pDX, m_DownloadValue, 1, 1000000000);
	DDX_Text(pDX, IDC_SPEEDLIMIT_UPLOAD_VALUE, m_UploadValue);
	DDV_MinMaxInt(pDX, m_UploadValue, 1, 1000000000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsSpeedLimitPage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsSpeedLimitPage)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_DOWNLOAD_NOLIMIT, OnRadio)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_DOWNLOAD_ADD, OnSpeedlimitDownloadAdd)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_DOWNLOAD_REMOVE, OnSpeedlimitDownloadRemove)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_DOWNLOAD_UP, OnSpeedlimitDownloadUp)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_DOWNLOAD_DOWN, OnSpeedlimitDownloadDown)
	ON_LBN_DBLCLK(IDC_SPEEDLIMIT_DOWNLOAD_RULES_LIST, OnDblclkSpeedlimitDownloadRulesList)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_UPLOAD_ADD, OnSpeedlimitUploadAdd)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_UPLOAD_REMOVE, OnSpeedlimitUploadRemove)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_UPLOAD_UP, OnSpeedlimitUploadUp)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_UPLOAD_DOWN, OnSpeedlimitUploadDown)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_UPLOAD_NOLIMIT, OnRadio)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_DOWNLOAD_CONSTANT, OnRadio)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_UPLOAD_CONSTANT, OnRadio)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_DOWNLOAD_RULES, OnRadio)
	ON_BN_CLICKED(IDC_SPEEDLIMIT_UPLOAD_RULES, OnRadio)
	ON_LBN_DBLCLK(IDC_SPEEDLIMIT_UPLOAD_RULES_LIST, OnDblclkSpeedlimitUploadRulesList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsSpeedLimitPage message handlers

void COptionsSpeedLimitPage::SetCtrlState()
{
	UpdateData(TRUE);

	m_DownloadValueCtrl.EnableWindow( m_DownloadSpeedLimitType == 1);
	m_DownloadUpCtrl.EnableWindow( m_DownloadSpeedLimitType == 2);
	m_DownloadRulesListCtrl.EnableWindow( m_DownloadSpeedLimitType == 2);
	m_DownloadRemoveCtrl.EnableWindow( m_DownloadSpeedLimitType == 2);
	m_DownloadDownCtrl.EnableWindow( m_DownloadSpeedLimitType == 2);
	m_DownloadAddCtrl.EnableWindow( m_DownloadSpeedLimitType == 2);

	m_UploadValueCtrl.EnableWindow( m_UploadSpeedLimitType == 1);
	m_UploadUpCtrl.EnableWindow( m_UploadSpeedLimitType == 2);
	m_UploadRulesListCtrl.EnableWindow( m_UploadSpeedLimitType == 2);;
	m_UploadRemoveCtrl.EnableWindow( m_UploadSpeedLimitType == 2);;
	m_UploadDownCtrl.EnableWindow( m_UploadSpeedLimitType == 2);;
	m_UploadAddCtrl.EnableWindow( m_UploadSpeedLimitType == 2);;
}

BOOL COptionsSpeedLimitPage::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetCtrlState();

	ShowSpeedLimit( m_DownloadRulesListCtrl, m_DownloadSpeedLimits);
	ShowSpeedLimit( m_UploadRulesListCtrl, m_UploadSpeedLimits);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsSpeedLimitPage::OnRadio() 
{
	SetCtrlState();
}

void COptionsSpeedLimitPage::OnSpeedlimitDownloadAdd() 
{
	// TODO: Add your control notification handler code here
	CSpeedLimitRuleDlg dlg;

	if ( dlg.DoModal() == IDOK)
	{
		m_DownloadSpeedLimits.push_back( dlg.GetSpeedLimit());

		ShowSpeedLimit( m_DownloadRulesListCtrl, m_DownloadSpeedLimits);
	}
}

void COptionsSpeedLimitPage::ShowSpeedLimit(CListBox &listBox, SPEEDLIMITSLIST &list)
{
	listBox.ResetContent();

	for (unsigned int i = 0; i < list.size(); i++)
		listBox.AddString( list[ i]->GetListBoxString());
}

void COptionsSpeedLimitPage::Copy(SPEEDLIMITSLIST &from, SPEEDLIMITSLIST &to)
{
	unsigned int i;
	for (i = 0; i < to.size(); i++)
		delete to[i];

	to.clear();

	for (i = 0; i < from.size(); i++)
		to.push_back(from[i]->GetCopy());
}

void COptionsSpeedLimitPage::OnSpeedlimitDownloadRemove() 
{
	// TODO: Add your control notification handler code here
	int curSel = m_DownloadRulesListCtrl.GetCurSel();

	if ( ( curSel >= 0) && ( static_cast<unsigned int>(curSel) < m_DownloadSpeedLimits.size()))
	{
		delete m_DownloadSpeedLimits[ curSel];
		for (SPEEDLIMITSLIST::iterator iter=m_DownloadSpeedLimits.begin(); iter!=m_DownloadSpeedLimits.end(); iter++)
		{
			if (!curSel)
			{
				m_DownloadSpeedLimits.erase(iter);
				break;
			}
			curSel--;
		}

		ShowSpeedLimit( m_DownloadRulesListCtrl, m_DownloadSpeedLimits);
	}
}

void COptionsSpeedLimitPage::OnSpeedlimitDownloadUp() 
{
	// TODO: Add your control notification handler code here
	int curSel = m_DownloadRulesListCtrl.GetCurSel();

	if ( ( curSel >= 1) && ( static_cast<unsigned int>(curSel) < m_DownloadSpeedLimits.size()))
	{
		CSpeedLimit *sl = m_DownloadSpeedLimits[ curSel];
		m_DownloadSpeedLimits[ curSel] = m_DownloadSpeedLimits[ curSel - 1];
		m_DownloadSpeedLimits[ curSel - 1] = sl;

		ShowSpeedLimit( m_DownloadRulesListCtrl, m_DownloadSpeedLimits);

		m_DownloadRulesListCtrl.SetCurSel( curSel - 1);
	}
}

void COptionsSpeedLimitPage::OnSpeedlimitDownloadDown() 
{
	// TODO: Add your control notification handler code here
	int curSel = m_DownloadRulesListCtrl.GetCurSel();

	if ( ( curSel >= 0) && ( static_cast<unsigned int>(curSel) < m_DownloadSpeedLimits.size() - 1))
	{
		CSpeedLimit *sl = m_DownloadSpeedLimits[ curSel];
		m_DownloadSpeedLimits[ curSel] = m_DownloadSpeedLimits[ curSel + 1];
		m_DownloadSpeedLimits[ curSel + 1] = sl;

		ShowSpeedLimit( m_DownloadRulesListCtrl, m_DownloadSpeedLimits);

		m_DownloadRulesListCtrl.SetCurSel( curSel + 1);
	}
}

void COptionsSpeedLimitPage::OnDblclkSpeedlimitDownloadRulesList() 
{
	// TODO: Add your control notification handler code here
	int curSel = m_DownloadRulesListCtrl.GetCurSel();

	if ( ( curSel >= 0) && ( static_cast<unsigned int>(curSel) < m_DownloadSpeedLimits.size()))
	{
		CSpeedLimitRuleDlg dlg;

		dlg.FillFromSpeedLimt( m_DownloadSpeedLimits[ curSel]);

		if ( dlg.DoModal() == IDOK)
		{
			delete m_DownloadSpeedLimits[ curSel];
			m_DownloadSpeedLimits[ curSel] = dlg.GetSpeedLimit();

			ShowSpeedLimit( m_DownloadRulesListCtrl, m_DownloadSpeedLimits);
			m_DownloadRulesListCtrl.SetCurSel( curSel);
		}
	}
}

void COptionsSpeedLimitPage::OnSpeedlimitUploadRemove() 
{
	// TODO: Add your control notification handler code here
	int curSel = m_UploadRulesListCtrl.GetCurSel();

	if ( ( curSel >= 0) && ( static_cast<unsigned int>(curSel) < m_UploadSpeedLimits.size()))
	{
		delete m_UploadSpeedLimits[ curSel];
		for (SPEEDLIMITSLIST::iterator iter=m_UploadSpeedLimits.begin(); iter!=m_UploadSpeedLimits.end(); iter++)
		{
			if (!curSel)
			{
				m_UploadSpeedLimits.erase(iter);
				break;
			}
			curSel--;
		}

		ShowSpeedLimit( m_UploadRulesListCtrl, m_UploadSpeedLimits);
	}
}

void COptionsSpeedLimitPage::OnSpeedlimitUploadUp() 
{
	// TODO: Add your control notification handler code here
	int curSel = m_UploadRulesListCtrl.GetCurSel();

	if ( ( curSel >= 1) && ( static_cast<unsigned int>(curSel) < m_UploadSpeedLimits.size()))
	{
		CSpeedLimit *sl = m_UploadSpeedLimits[ curSel];
		m_UploadSpeedLimits[ curSel] = m_UploadSpeedLimits[ curSel - 1];
		m_UploadSpeedLimits[ curSel - 1] = sl;

		ShowSpeedLimit( m_UploadRulesListCtrl, m_UploadSpeedLimits);

		m_UploadRulesListCtrl.SetCurSel( curSel - 1);
	}
}

void COptionsSpeedLimitPage::OnSpeedlimitUploadDown() 
{
	// TODO: Add your control notification handler code here
	int curSel = m_UploadRulesListCtrl.GetCurSel();

	if ( ( curSel >= 0) && ( static_cast<unsigned int>(curSel) < m_UploadSpeedLimits.size() - 1))
	{
		CSpeedLimit *sl = m_UploadSpeedLimits[ curSel];
		m_UploadSpeedLimits[ curSel] = m_UploadSpeedLimits[ curSel + 1];
		m_UploadSpeedLimits[ curSel + 1] = sl;

		ShowSpeedLimit( m_UploadRulesListCtrl, m_UploadSpeedLimits);

		m_UploadRulesListCtrl.SetCurSel( curSel + 1);
	}
}

void COptionsSpeedLimitPage::OnSpeedlimitUploadAdd() 
{
	// TODO: Add your control notification handler code here
	CSpeedLimitRuleDlg dlg;

	if ( dlg.DoModal() == IDOK)
	{
		m_UploadSpeedLimits.push_back( dlg.GetSpeedLimit());

		ShowSpeedLimit( m_UploadRulesListCtrl, m_UploadSpeedLimits);
	}
}

void COptionsSpeedLimitPage::OnDblclkSpeedlimitUploadRulesList() 
{
	// TODO: Add your control notification handler code here
	int curSel = m_UploadRulesListCtrl.GetCurSel();

	if ( ( curSel >= 0) && ( static_cast<unsigned int>(curSel) < m_UploadSpeedLimits.size()))
	{
		CSpeedLimitRuleDlg dlg;

		dlg.FillFromSpeedLimt( m_UploadSpeedLimits[ curSel]);

		if ( dlg.DoModal() == IDOK)
		{
			delete m_UploadSpeedLimits[ curSel];
			m_UploadSpeedLimits[ curSel] = dlg.GetSpeedLimit();

			ShowSpeedLimit( m_UploadRulesListCtrl, m_UploadSpeedLimits);
			m_UploadRulesListCtrl.SetCurSel( curSel);
		}
	}
}
