#if !defined(AFX_OPTIONSIDENTPAGE_H__BA06AA0F_51EF_454A_B2C4_63D9C56E6E0E__INCLUDED_)
#define AFX_OPTIONSIDENTPAGE_H__BA06AA0F_51EF_454A_B2C4_63D9C56E6E0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsIdentPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsIdentPage 

class COptionsIdentPage : public CSAPrefsSubDlg
{
// Konstruktion
public:
	COptionsIdentPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(COptionsIdentPage)
	enum { IDD = IDD_OPTIONS_IDENT };
	CButton	m_cIdentConnect;
	CButton	m_cSameIP;
	CEdit	m_cSystem;
	CEdit	m_cUserID;
	BOOL	m_bIdent;
	BOOL	m_bIdentConnect;
	BOOL	m_bSameIP;
	CString	m_System;
	CString	m_UserID;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COptionsIdentPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsIdentPage)
	afx_msg void OnOptionsIdentIdent();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OPTIONSIDENTPAGE_H__BA06AA0F_51EF_454A_B2C4_63D9C56E6E0E__INCLUDED_
