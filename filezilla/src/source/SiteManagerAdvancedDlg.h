#if !defined(AFX_SITEMANAGERADVANCEDDLG_H__5CEAFA96_2D3C_44FC_A8E9_B58C60267880__INCLUDED_)
#define AFX_SITEMANAGERADVANCEDDLG_H__5CEAFA96_2D3C_44FC_A8E9_B58C60267880__INCLUDED_

#include "SiteManager.h"	// Hinzugefügt von der Klassenansicht
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SiteManagerAdvancedDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSiteManagerAdvancedDlg 

class t_SiteManagerItem;
class CSiteManagerAdvancedDlg : public CDialog
{
// Konstruktion
public:
	t_SiteManagerItem *m_pSiteManagerItem;
	CSiteManagerAdvancedDlg(t_SiteManagerItem *pItem);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CSiteManagerAdvancedDlg)
	enum { IDD = IDD_SITEMANAGER_ADVANCED };
	CSpinButtonCtrl	m_SpinButton;
	int		m_nTimeZoneOffset;
	CSpinButtonCtrl	m_SpinButton2;
	int		m_nTimeZoneOffset2;
	int		m_nPassive;
	CString	m_LocalDir;
	CString	m_RemoteDir;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSiteManagerAdvancedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSiteManagerAdvancedDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSitemanagerLocaldirBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_nUTF8;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SITEMANAGERADVANCEDDLG_H__5CEAFA96_2D3C_44FC_A8E9_B58C60267880__INCLUDED_
