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

#if !defined(AFX_SITEMANAGER_H__624E68D8_6ADA_40C8_A713_AEA80E0EA737__INCLUDED_)
#define AFX_SITEMANAGER_H__624E68D8_6ADA_40C8_A713_AEA80E0EA737__INCLUDED_

#include "structures.h"	// Hinzugefügt von der Klassenansicht
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SiteManager.h : Header-Datei
//

enum ItemType
{
	SITE,
	FOLDER
};

class t_SiteManagerItem
{
public:
	t_SiteManagerItem();
	CString Host;
	int nPort;
	CString User;
	CString Account;
	CString Pass;
	CString RemoteDir;
	CString LocalDir;
	int nLogonType;
	BOOL bFirewallBypass;
	BOOL bPassDontSave;
	int nServerType;
	CString name;
	ItemType NodeType;
	int nPasv;
	int nTimeZoneOffset; //Hours
	int nTimeZoneOffset2; //Minutes
	CString Comments;
	int nUTF8;
};

class CMenuTree : public CObject
{
public :
	int m_CurrentItem;
	std::list<t_SiteManagerItem *> m_Sites;

	CMenuTree() {};
	~CMenuTree();
};

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSiteManager 

typedef bool ( * BUILDSITETREEPROC_NEW_ITEM)( LPVOID lpData, LPCTSTR lpszName, LPVOID lpParent, t_SiteManagerItem *site, bool isDefault, LPVOID lpDefault);
typedef bool ( * BUILDSITETREEPROC_NEW_FOLDER)( LPVOID lpData, LPCTSTR lpszName, LPVOID &lpParent);
typedef bool ( * BUILDSITETREEPROC_AFTER)( LPVOID lpData, LPCTSTR lpszName, LPVOID lpItem);

class CSiteManager : public CDialog
{
// Konstruktion
public:
	static bool ConnectSitePrepare( t_SiteManagerItem *site);
	static void ReadTree(CString substring, const LPVOID &parent, LPVOID lpData, LPVOID lpDefault, BUILDSITETREEPROC_NEW_ITEM NewItemProc,
							BUILDSITETREEPROC_NEW_FOLDER NewFolderProc, BUILDSITETREEPROC_AFTER AfterProc);
	static void ReadTreeXML(CMarkupSTL *pMarkup, const LPVOID &parent, LPVOID lpData, LPVOID lpDefault, BUILDSITETREEPROC_NEW_ITEM NewItemProc,
							BUILDSITETREEPROC_NEW_FOLDER NewFolderProc, BUILDSITETREEPROC_AFTER AfterProc);
	virtual  ~CSiteManager();
	int m_nServerType;
	static t_SiteManagerItem * GetSiteByName(CString name);
	static t_SiteManagerItem * GetDefaultSite(CString substring="");
	BOOL m_bAddServer;
	t_server m_addserver;
	int AddAndShow(t_server &server);
	int ImportAndShow(CString FileToImport);
	void Import(CString filename);
	void ImportXML(CMarkupSTL *pMarkup, HTREEITEM &parent);
	CSiteManager(CWnd* pParent = NULL);   // Standardkonstruktor
	static CString GetKey(HKEY hRootKey, CString subkey, CString keyname);

	t_SiteManagerItem m_LastSite;
	
// Dialogfelddaten
	//{{AFX_DATA(CSiteManager)
	enum { IDD = IDD_SITEMANAGER };
	CComboBox	m_cServerType;
	CTreeCtrl	m_cTree;
	CButton	m_cPassDontSave;
	CButton	m_cDefault;
	CButton	m_FwBypassCtrl;
	CButton	m_ConnectCtrl;
	CEdit	m_UserCtrl;
	CEdit	m_AccountCtrl;
	CButton	m_LogontypeCtrl;
	CButton	m_LogontypeCtrl2;
	CButton	m_LogontypeCtrl3;
	CEdit	m_PortCtrl;
	CEdit	m_PassCtrl;
	CButton	m_DeleteCtrl;
	CEdit	m_HostCtrl;
	int		m_Logontype;
	CString	m_Host;
	CString	m_Pass;
	CString	m_Port;
	CString	m_User;
	CString m_Account;
	BOOL	m_bFwBypass;
	int		m_nDefault;
	BOOL	m_bPassDontSave;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSiteManager)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	static t_SiteManagerItem * GetSiteByNameXML(CMarkupSTL *pMarkup, CString name);
	static t_SiteManagerItem * GetDefaultSiteXML(CMarkupSTL *pMarkup);

	static t_SiteManagerItem *GetData(HKEY hRootKey, CString Name);
	static t_SiteManagerItem *GetDataXML(CMarkupSTL *pMarkup);
	HTREEITEM m_hDragItem;
	HTREEITEM m_hDropItem;
	HCURSOR m_hcArrow;
	HCURSOR m_hcNo;
	CImageList* m_pDragImage;
	CPoint m_CurPos;
	UINT_PTR m_nDragScrollTimerID;

	std::list<CString> m_KeysToDelete;
	DWORD RegDeleteKeyEx(CString subkey);
	CBitmap m_Bitmap;
	CImageList m_ImageList;
	void DisplaySite(t_SiteManagerItem *site);
	HTREEITEM m_DefaultSite;
	static void SaveSingleSite(CString name, t_SiteManagerItem *site);
	static void SaveSingleSiteXML(CMarkupSTL *pMarkup, t_SiteManagerItem *site);
	void SaveData();
	void SaveDataXML(CMarkupSTL *pMarkup);
	void SetCtrlState();
	BOOL CheckValid();
	static void SetKey(CString subkey,CString keyname,CString value);
	CString m_FileToImport;
	static int CALLBACK SortFoldersFirst(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static void Log(CString str);

	static bool NewItem_Tree(LPVOID lpData, LPCTSTR lpszName, LPVOID lpParent, t_SiteManagerItem *site, bool isDefault, LPVOID lpDefault);
	static bool NewFolder_Tree( LPVOID lpData, LPCTSTR lpszName, LPVOID &lpParent);
	static bool After_Tree(LPVOID lpData, LPCTSTR lpszName, LPVOID lpItem);
	
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSiteManager)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnDelete();
	virtual void OnCancel();
	afx_msg void OnRadio();
	afx_msg void OnExit();
	afx_msg void OnKillfocusHost();
	afx_msg void OnSitemanagerSaveexit();
	afx_msg void OnSitemanagerFileImport();
	afx_msg void OnSitemanagerFileExportAsurllist();
	afx_msg void OnSitemanagerFileExportWithalldetails();
	afx_msg void OnClose();
	afx_msg void OnPassdontsave();
	afx_msg void OnSelchangingSitemanagertree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedSitemanagertree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandedSitemanagertree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkSitemanagertree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditSitemanagertree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSitemanagerNewfolder();
	afx_msg void OnBegindragSitemanagertree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSelchangeServertype();
	afx_msg void OnSitemanagerAdvanced();
	afx_msg void OnSitemanagerCopy();
	afx_msg void OnSitemanagerRename();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SITEMANAGER_H__624E68D8_6ADA_40C8_A713_AEA80E0EA737__INCLUDED_
