#if !defined(AFX_OPTIONSSSHPAGE_H__3B6AB3BA_724D_458E_92F9_36ABA6BE090B__INCLUDED_)
#define AFX_OPTIONSSSHPAGE_H__3B6AB3BA_724D_458E_92F9_36ABA6BE090B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsSshPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsSshPage 

class COptionsSshPage : public CSAPrefsSubDlg
{
// Konstruktion
public:
	COptionsSshPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(COptionsSshPage)
	enum { IDD = IDD_OPTIONS_SSHPAGE };
	int		m_nCompression;
	int		m_nProtocol;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COptionsSshPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsSshPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OPTIONSSSHPAGE_H__3B6AB3BA_724D_458E_92F9_36ABA6BE090B__INCLUDED_
