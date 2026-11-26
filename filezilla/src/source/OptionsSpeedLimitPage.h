#if !defined(AFX_OPTIONSSPEEDLIMITPAGE_H__12038661_64F1_46F2_8A68_45C21F79755D__INCLUDED_)
#define AFX_OPTIONSSPEEDLIMITPAGE_H__12038661_64F1_46F2_8A68_45C21F79755D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsSpeedLimitPage.h : header file
//

//#include "Options.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsSpeedLimitPage dialog
class COptionsSpeedLimitPage : public CSAPrefsSubDlg
{
// Construction
public:
	void Copy( SPEEDLIMITSLIST &from, SPEEDLIMITSLIST &to);
	void ShowSpeedLimit( CListBox &listBox, SPEEDLIMITSLIST &list);
	void SetCtrlState();
	COptionsSpeedLimitPage(CWnd* pParent = NULL);   // standard constructor
	COptionsSpeedLimitPage::~COptionsSpeedLimitPage();
	SPEEDLIMITSLIST m_DownloadSpeedLimits;
	SPEEDLIMITSLIST m_UploadSpeedLimits;
// Dialog Data
	//{{AFX_DATA(COptionsSpeedLimitPage)
	enum { IDD = IDD_OPTIONS_SPEEDLIMIT };
	CButton	m_DownloadUpCtrl;
	CListBox	m_DownloadRulesListCtrl;
	CButton	m_DownloadRemoveCtrl;
	CButton	m_DownloadDownCtrl;
	CButton	m_DownloadAddCtrl;
	CButton	m_UploadUpCtrl;
	CListBox	m_UploadRulesListCtrl;
	CButton	m_UploadRemoveCtrl;
	CButton	m_UploadDownCtrl;
	CButton	m_UploadAddCtrl;
	CEdit	m_UploadValueCtrl;
	CEdit	m_DownloadValueCtrl;
	int		m_DownloadSpeedLimitType;
	int		m_UploadSpeedLimitType;
	int		m_DownloadValue;
	int		m_UploadValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsSpeedLimitPage)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsSpeedLimitPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio();
	afx_msg void OnSpeedlimitDownloadAdd();
	afx_msg void OnSpeedlimitDownloadRemove();
	afx_msg void OnSpeedlimitDownloadUp();
	afx_msg void OnSpeedlimitDownloadDown();
	afx_msg void OnDblclkSpeedlimitDownloadRulesList();
	afx_msg void OnSpeedlimitUploadAdd();
	afx_msg void OnSpeedlimitUploadRemove();
	afx_msg void OnSpeedlimitUploadUp();
	afx_msg void OnSpeedlimitUploadDown();
	afx_msg void OnDblclkSpeedlimitUploadRulesList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSSPEEDLIMITPAGE_H__12038661_64F1_46F2_8A68_45C21F79755D__INCLUDED_)
