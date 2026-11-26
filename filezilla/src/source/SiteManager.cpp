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

// SiteManager.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "SiteManager.h"
#include "entersomething.h"
#include "SiteManagerAdvancedDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMenuTree::~CMenuTree()
{
	for (std::list<t_SiteManagerItem *>::iterator iter=m_Sites.begin(); iter!=m_Sites.end(); iter++)
		delete *iter;
}

bool NewItem_Menu( LPVOID lpData, LPCTSTR lpszName, LPVOID lpParent, t_SiteManagerItem *site, bool isDefault, LPVOID lpDefault)
{
	CMenuTree *mt = ( CMenuTree *)lpData;

	AppendMenu( ( HMENU)lpParent, MF_STRING, mt->m_CurrentItem, lpszName);

	if (!site)
		EnableMenuItem( (HMENU)lpParent, mt->m_CurrentItem, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);

	if (isDefault)
		SetMenuDefaultItem( ( HMENU)lpParent, mt->m_CurrentItem, FALSE);

	mt->m_CurrentItem++;

	mt->m_Sites.push_back(site);

	return true;
}

bool NewFolder_Menu( LPVOID lpData, LPCTSTR lpszName, LPVOID &lpParent)
{
	HMENU hMenu = CreatePopupMenu();
	AppendMenu( ( HMENU)lpParent, MF_STRING | MF_POPUP, ( UINT)hMenu, lpszName);
	lpParent = ( LPVOID)hMenu;

	return true;
}

bool After_Menu( LPVOID lpData, LPCTSTR lpszName, LPVOID lpItem)
{
	if (!GetMenuItemCount(( HMENU)lpItem))
	{
		CString name;
		name.LoadString(IDS_TOOLBAR_SITEDROPDOWN_EMPTYFOLDER);
		NewItem_Menu(lpData, name, lpItem, 0, 0, 0);
	}
	return true;
}

bool CSiteManager::NewItem_Tree(LPVOID lpData, LPCTSTR lpszName, LPVOID lpParent, t_SiteManagerItem *site, bool isDefault, LPVOID lpDefault)
{
	HTREEITEM item = ((CTreeCtrl *)lpData)->InsertItem(lpszName, 2, 2, (HTREEITEM)lpParent, TVI_SORT);
	((CTreeCtrl *)lpData)->SetItemData(item, (DWORD)site);
	if ( isDefault)
		*((HTREEITEM *)lpDefault) = item;

	return true;
}

bool CSiteManager::NewFolder_Tree( LPVOID lpData, LPCTSTR lpszName, LPVOID &lpParent)
{
	lpParent = ((CTreeCtrl *)lpData)->InsertItem(lpszName, (HTREEITEM)lpParent, TVI_SORT);
	t_SiteManagerItem *pFolder = new t_SiteManagerItem;
	pFolder->name = lpszName;
	pFolder->NodeType = FOLDER;
	((CTreeCtrl *)lpData)->SetItemData((HTREEITEM)lpParent, (DWORD)pFolder);
	return true;
}

bool CSiteManager::After_Tree(LPVOID lpData, LPCTSTR lpszName, LPVOID lpItem)
{
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)lpData;

	if (!COptions::GetOptionVal(OPTION_SITEMANAGERNOEXPANDFOLDERS))
		pTreeCtrl->Expand((HTREEITEM)lpItem, TVE_EXPAND);

	if (COptions::GetOptionVal(OPTION_SORTSITEMANAGERFOLDERSFIRST))
	{
		TVSORTCB pMy_Sort;
		pMy_Sort.hParent = (HTREEITEM)lpItem;
		pMy_Sort.lpfnCompare = CSiteManager::SortFoldersFirst;
		pMy_Sort.lParam = (LPARAM) pTreeCtrl;
		pTreeCtrl->SortChildrenCB(&pMy_Sort);
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSiteManager


CSiteManager::CSiteManager(CWnd* pParent /*=NULL*/)
	: CDialog(CSiteManager::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSiteManager)
	m_Logontype = -1;
	m_Host = _T("");
	m_Pass = _T("");
	m_Port = _T("");
	m_User = _T("");
	m_bFwBypass = FALSE;
	m_nDefault = -1;
	m_bPassDontSave = FALSE;
	//}}AFX_DATA_INIT
	m_bAddServer=FALSE;
	m_nServerType = -1;
	m_pDragImage=0;
	m_hDragItem=0;
	m_nDragScrollTimerID=0;
	m_hDropItem=0;
}


void CSiteManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSiteManager)
	DDX_Control(pDX, IDC_SERVERTYPE, m_cServerType);
	DDX_Control(pDX, IDC_SITEMANAGERTREE, m_cTree);
	DDX_Control(pDX, IDC_PASSDONTSAVE, m_cPassDontSave);
	DDX_Control(pDX, IDC_DEFAULT, m_cDefault);
	DDX_Control(pDX, IDC_FWBYPASS, m_FwBypassCtrl);
	DDX_Control(pDX, IDOK, m_ConnectCtrl);
	DDX_Control(pDX, IDC_USER, m_UserCtrl);
	DDX_Control(pDX, IDC_USER2, m_AccountCtrl);
	DDX_Control(pDX, IDC_RADIO1, m_LogontypeCtrl);
	DDX_Control(pDX, IDC_RADIO2, m_LogontypeCtrl2);
	DDX_Control(pDX, IDC_RADIO3, m_LogontypeCtrl3);
	DDX_Control(pDX, IDC_PORT, m_PortCtrl);
	DDX_Control(pDX, IDC_PASS, m_PassCtrl);
	DDX_Control(pDX, IDC_DELETE, m_DeleteCtrl);
	DDX_Control(pDX, IDC_HOST, m_HostCtrl);
	DDX_Radio(pDX, IDC_RADIO1, m_Logontype);
	DDX_Text(pDX, IDC_HOST, m_Host);
	DDX_Text(pDX, IDC_PASS, m_Pass);
	DDX_Text(pDX, IDC_PORT, m_Port);
	DDX_Text(pDX, IDC_USER, m_User);
	DDX_Text(pDX, IDC_USER2, m_Account);
	DDX_Check(pDX, IDC_FWBYPASS, m_bFwBypass);
	DDX_Radio(pDX, IDC_DEFAULT, m_nDefault);
	DDX_Check(pDX, IDC_PASSDONTSAVE, m_bPassDontSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSiteManager, CDialog)
	//{{AFX_MSG_MAP(CSiteManager)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_EN_KILLFOCUS(IDC_HOST, OnKillfocusHost)
	ON_COMMAND(ID_SITEMANAGER_SAVEEXIT, OnSitemanagerSaveexit)
	ON_COMMAND(ID_SITEMANAGER_FILE_IMPORT, OnSitemanagerFileImport)
	ON_COMMAND(ID_SITEMANAGER_FILE_EXPORT_ASURLLIST, OnSitemanagerFileExportAsurllist)
	ON_COMMAND(ID_SITEMANAGER_FILE_EXPORT_WITHALLDETAILS, OnSitemanagerFileExportWithalldetails)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_PASSDONTSAVE, OnPassdontsave)
	ON_NOTIFY(TVN_SELCHANGING, IDC_SITEMANAGERTREE, OnSelchangingSitemanagertree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_SITEMANAGERTREE, OnSelchangedSitemanagertree)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_SITEMANAGERTREE, OnItemexpandedSitemanagertree)
	ON_NOTIFY(NM_DBLCLK, IDC_SITEMANAGERTREE, OnDblclkSitemanagertree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_SITEMANAGERTREE, OnEndlabeleditSitemanagertree)
	ON_BN_CLICKED(IDC_SITEMANAGER_NEWFOLDER, OnSitemanagerNewfolder)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_SITEMANAGERTREE, OnBegindragSitemanagertree)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_SERVERTYPE, OnSelchangeServertype)
	ON_BN_CLICKED(IDC_SITEMANAGER_ADVANCED, OnSitemanagerAdvanced)
	ON_BN_CLICKED(IDC_SITEMANAGER_COPY, OnSitemanagerCopy)
	ON_BN_CLICKED(IDC_SITEMANAGER_RENAME, OnSitemanagerRename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSiteManager

bool CSiteManager::ConnectSitePrepare(t_SiteManagerItem *site)
{
	if (site && site->NodeType == SITE)
	{
		BOOL bUseGSS=FALSE;
		if (COptions::GetOptionVal(OPTION_USEGSS) && site->bPassDontSave)
		{
			USES_CONVERSION;

			CString GssServers=COptions::GetOption(OPTION_GSSSERVERS);
			LPCSTR lpszAsciiHost=T2CA(site->Host);
			hostent *fullname=gethostbyname(lpszAsciiHost);
			CString host;
			if (fullname)
				host=fullname->h_name;
			else
				host=site->Host;
			host.MakeLower();
			int i;
			while ((i=GssServers.Find( _T(";") ))!=-1)
			{
				if (("."+GssServers.Left(i))==host.Right(GssServers.Left(i).GetLength()+1) || GssServers.Left(i)==host)
				{
					bUseGSS=TRUE;
					break;
				}
				GssServers=GssServers.Mid(i+1);
			}
		}
		if (!bUseGSS)
		{
			if (site->nLogonType && ( site->bPassDontSave || _ttoi(COptions::GetOption(OPTION_RUNINSECUREMODE))) )
			{
				CEnterSomething dlg(IDS_INPUTDIALOGTITLE_INPUTPASSWORD,IDS_INPUTDIALOGTEXT_INPUTPASSWORD,'*');
				if (dlg.DoModal()!=IDOK)
					return false;
				site->Pass = dlg.m_String;
			}
		}
		return true;
	}

	return false;
}

void CSiteManager::OnOK()
{
	HTREEITEM item=m_cTree.GetSelectedItem();
	t_SiteManagerItem *site=0;
	if (item)
		site=(t_SiteManagerItem *)m_cTree.GetItemData(item);
	if (!site || site->NodeType != SITE)
		return;

	if (CheckValid())
	{   //If an item is selected and the current selection is valid,
		//we can save the data and close the dialog
		//And don't forget to ask for the password first!
		t_SiteManagerItem site2 = *(t_SiteManagerItem *)m_cTree.GetItemData(item);
		BOOL bUseGSS=FALSE;
		if (COptions::GetOptionVal(OPTION_USEGSS) && m_bPassDontSave)
		{
			USES_CONVERSION;
				CString GssServers=COptions::GetOption(OPTION_GSSSERVERS);
			LPCSTR lpszAsciiHost=T2CA(m_Host);
			hostent *fullname=gethostbyname(lpszAsciiHost);
			CString host;
			if (fullname)
				host=fullname->h_name;
			else
				host=m_Host;
			host.MakeLower();
			int i;
			while ((i=GssServers.Find( _T(";") ))!=-1)
			{
				if (("."+GssServers.Left(i))==host.Right(GssServers.Left(i).GetLength()+1) || GssServers.Left(i)==host)
				{
					bUseGSS=TRUE;
					break;
				}
				GssServers=GssServers.Mid(i+1);
			}
		}

		if (!bUseGSS)
		{
			if (m_Logontype && (m_bPassDontSave || _ttoi(COptions::GetOption(OPTION_RUNINSECUREMODE))) )
			{
				CEnterSomething dlg(IDS_INPUTDIALOGTITLE_INPUTPASSWORD,IDS_INPUTDIALOGTEXT_INPUTPASSWORD,'*');
				if (dlg.DoModal()!=IDOK)
					return;
				site2.Pass = dlg.m_String;
			}
		}

		SaveData();
		OnClose();
		m_LastSite = site2;
		m_nServerType = m_cServerType.GetCurSel();
		CDialog::OnOK();
	}
}

CString CSiteManager::GetKey(HKEY hRootKey, CString subkey,CString keyname)
{
	HKEY key;
	if (RegOpenKey(hRootKey, "Software\\FileZilla\\Site Manager\\"+subkey,&key)==ERROR_SUCCESS)
	{
		unsigned char *buffer=new unsigned char[1500];
		memset(buffer,0,1500);

		unsigned long tmp=1500;
		if (RegQueryValueEx(key,keyname,NULL,NULL,buffer,&tmp)!=ERROR_SUCCESS)
		{
			RegCloseKey(key);
			delete [] buffer;
			return "";
		}
		else
		{
			RegCloseKey(key);
			CString res=(LPTSTR)buffer;
			delete [] buffer;
			return res;
		}
	}
	return "";
}

void CSiteManager::SetKey(CString subkey,CString keyname,CString value)
{
	subkey.TrimLeft( _T("\\") );
	LPTSTR tmp=new TCHAR[value.GetLength()+1];
	LPCTSTR str=value;
	_tcscpy(tmp,str);
	HKEY key;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\FileZilla\\Site Manager\\" + subkey, 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &key, NULL)==ERROR_SUCCESS)
	{
		RegSetValueEx(key,keyname,0,REG_SZ,(unsigned char *)tmp,(value.GetLength()+1)*sizeof(TCHAR));
		RegCloseKey(key);
	}
	delete [] tmp;
}

BOOL CSiteManager::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetCtrlState();
	m_DefaultSite=0;
	if (!m_ImageList.GetSafeHandle())
		m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,0,3);
	if (!m_Bitmap.GetSafeHandle())
	{
		m_Bitmap.Attach(::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SITEMANAGERTREE)));
	}
	m_ImageList.Add(&m_Bitmap,RGB(255,0,255));
	m_cTree.SetImageList(&m_ImageList,TVSIL_NORMAL);
	CString str;
	str.LoadString(IDS_SITEMANAGER_TREEROOT);

	// Insert the parent item into the tree
	HTREEITEM item=m_cTree.InsertItem(str, TVI_ROOT, TVI_SORT);
	t_SiteManagerItem *root_pFolder = new t_SiteManagerItem;
	root_pFolder->name = str;
	root_pFolder->NodeType = FOLDER;
	m_cTree.SetItemData(item, (DWORD)root_pFolder);

	CMarkupSTL *pMarkup;
	if (COptions::LockXML(&pMarkup))
	{
		pMarkup->ResetPos();
		if (pMarkup->FindChildElem( _T("Sites") ))
		{
			pMarkup->IntoElem();
			ReadTreeXML(pMarkup, item, (LPVOID)&m_cTree, (LPVOID)&m_DefaultSite, NewItem_Tree, NewFolder_Tree, After_Tree);
		}
		else
			ReadTree( _T(""), item, (LPVOID)&m_cTree, (LPVOID)&m_DefaultSite, NewItem_Tree, NewFolder_Tree, After_Tree);

		COptions::UnlockXML();
	}
	else
		ReadTree( _T(""), item, (LPVOID)&m_cTree, (LPVOID)&m_DefaultSite, NewItem_Tree, NewFolder_Tree, After_Tree);
	// Sort with folders first.  The list is already sorted alphabetically at this point.
	if (COptions::GetOptionVal(OPTION_SORTSITEMANAGERFOLDERSFIRST))
	{
		TVSORTCB pMy_Sort;
		pMy_Sort.hParent = item;
		pMy_Sort.lpfnCompare = SortFoldersFirst;
		pMy_Sort.lParam = (LPARAM) &m_cTree;
		m_cTree.SortChildrenCB(&pMy_Sort);
	}

	m_cTree.Expand(item, TVE_EXPAND);

	if (!m_DefaultSite)
		m_DefaultSite=item;
	m_cTree.SelectItem(m_DefaultSite);
	t_SiteManagerItem *site=0;
	if (m_DefaultSite)
		site=(t_SiteManagerItem *)m_cTree.GetItemData(m_DefaultSite);
	DisplaySite(site);

	if (m_bAddServer)
	{
		CString str;

		str.Format(IDS_SITEMANAGER_NEWFTPSITE);
		int i=2;
		HTREEITEM parent=m_cTree.GetRootItem();
		HTREEITEM child=m_cTree.GetChildItem(parent);
		while (child)
		{
			CString str2=str;
			str2.MakeLower();
			CString childstr=m_cTree.GetItemText(child);
			childstr.MakeLower();
			if (childstr==str2)
			{
				str.Format(IDS_SITEMANAGER_NEWFTPSITEWITHNUM,i);
				i++;
				child=m_cTree.GetChildItem(parent);
				continue;
			}
			child=m_cTree.GetNextSiblingItem(child);

		}
		HTREEITEM item=m_cTree.InsertItem(str,2,2,parent,TVI_SORT);

		t_SiteManagerItem *site = new t_SiteManagerItem;
		site->name = str;
		site->Host = m_addserver.host;
		site->nPort = m_addserver.port;
		site->bPassDontSave = m_addserver.bDontRememberPass;
		if (m_addserver.user != "anonymous")
		{
			site->User = m_addserver.user;
			site->Pass = m_addserver.pass;
			site->nLogonType = 1;
		}
		else
			site->nLogonType = 0;
		site->nServerType = 0;
		if ( (m_addserver.nServerType&FZ_SERVERTYPE_HIGHMASK) == FZ_SERVERTYPE_FTP)
		{
			if ( (m_addserver.nServerType&FZ_SERVERTYPE_LAYERMASK) == FZ_SERVERTYPE_LAYER_SSL_IMPLICIT)
				site->nServerType = 1;
			else if ( (m_addserver.nServerType&FZ_SERVERTYPE_LAYERMASK) == FZ_SERVERTYPE_LAYER_SSL_EXPLICIT)
				site->nServerType = 2;
			else if ( (m_addserver.nServerType&FZ_SERVERTYPE_SUBMASK) == FZ_SERVERTYPE_SUB_FTP_SFTP)
				site->nServerType = 3;
			else if ((m_addserver.nServerType & FZ_SERVERTYPE_LAYERMASK) == FZ_SERVERTYPE_LAYER_TLS_EXPLICIT)
				site->nServerType = 4;
		}
		else
			ASSERT(FALSE);
		site->RemoteDir = m_addserver.path;
		site->bFirewallBypass = m_addserver.fwbypass;
		site->nTimeZoneOffset = m_addserver.nTimeZoneOffset / 60;
		site->nTimeZoneOffset2 = m_addserver.nTimeZoneOffset % 60;
		site->NodeType = SITE;

		m_cTree.SetItemData(item,(DWORD)site);
		m_cTree.SelectItem(item);
		DisplaySite(site);
		m_cTree.SetFocus();
		m_cTree.EditLabel(item);

		m_bAddServer = FALSE;
	}

	m_hcArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hcNo = AfxGetApp()->LoadStandardCursor(IDC_NO);

	if (m_FileToImport!="")
		Import(m_FileToImport);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CSiteManager::OnNew()
{
	if (!CheckValid())
		return;
	HTREEITEM parent=m_cTree.GetSelectedItem();
	if (!parent)
		parent=m_cTree.GetRootItem();
	ASSERT(m_cTree.GetItemData(parent));
	if (((t_SiteManagerItem *)m_cTree.GetItemData(parent))->NodeType==SITE)
		parent=m_cTree.GetParentItem(parent);

	ASSERT(m_cTree.GetItemData(parent) && ((t_SiteManagerItem *)m_cTree.GetItemData(parent))->NodeType==FOLDER);

	CString name;
	name.Format(IDS_SITEMANAGER_NEWFTPSITE);

	HTREEITEM child=m_cTree.GetChildItem(parent);
	int i=2;
	while (child)
	{
		CString name2=name;
		name2.MakeLower();
		CString childstr=m_cTree.GetItemText(child);
		childstr.MakeLower();
		if (childstr==name2)
		{
			child=m_cTree.GetChildItem(parent);
			name.Format(IDS_SITEMANAGER_NEWFTPSITEWITHNUM,i);
			i++;
			continue;
		}
		child=m_cTree.GetNextItem(child,TVGN_NEXT);
	}

	HTREEITEM item=m_cTree.InsertItem(name,2,2,parent,TVI_SORT);
	m_cTree.Expand(parent, TVE_EXPAND);
	t_SiteManagerItem *site=new t_SiteManagerItem;
	site->nPort = 21;
	site->nLogonType = 0;
	site->bFirewallBypass=FALSE;
	site->bPassDontSave=FALSE;
	site->nServerType=0;
	site->name=name;
	site->NodeType=SITE;
	m_cTree.SetItemData(item,(DWORD)site);

	// Re-sort with folders first if that option is selected.  This must come AFTER the SetItemData command above.
	if (COptions::GetOptionVal(OPTION_SORTSITEMANAGERFOLDERSFIRST))
	{
		TVSORTCB pMy_Sort;
		pMy_Sort.hParent = parent;
		pMy_Sort.lpfnCompare = SortFoldersFirst;
		pMy_Sort.lParam = (LPARAM) &m_cTree;
		m_cTree.SortChildrenCB(&pMy_Sort);
	}

	m_cTree.SelectItem(item);
	DisplaySite(site);
	m_cTree.EditLabel(item);
}

/*
 * CheckValid()
 *
 * Returns:
 *   TRUE  - if Currently selected tree item is a folder or a site with complete data.
 *   FALSE - if Currently selected tree item is a site without complete data.
 */
BOOL CSiteManager::CheckValid()
{
	HTREEITEM olditem=m_cTree.GetSelectedItem();
	UpdateData(true);
	if (!olditem)
		return TRUE;
	t_SiteManagerItem *data=(t_SiteManagerItem *)m_cTree.GetItemData(olditem);
	ASSERT(data);
	if (!data)
		return FALSE;
	if (data->NodeType == FOLDER)
		return TRUE;
	else if (data->NodeType == SITE)
	{
		CString str;
		CString str2;

		if (m_Host=="")
		{
			m_cTree.SelectItem(olditem);
			AfxMessageBox(IDS_ERRORMSG_HOSTNEEDED,MB_ICONEXCLAMATION);
			m_HostCtrl.SetFocus();
			return FALSE;
		}

		if (m_Port=="")
		{
			m_cTree.SelectItem(olditem);
			AfxMessageBox(IDS_ERRORMSG_PORTNEEDED,MB_ICONEXCLAMATION);
			m_PortCtrl.SetFocus();
			return FALSE;
		}
		else if (!_ttoi(m_Port))
		{
			m_cTree.SelectItem(olditem);
			AfxMessageBox(IDS_ERRORMSG_PORTNEEDED,MB_ICONEXCLAMATION);
			m_PortCtrl.SetFocus();
			return FALSE;
		}

		if (m_Logontype==-1)
		{
			m_cTree.SelectItem(olditem);
			AfxMessageBox(IDS_ERRORMSG_SITEMANAGERNEEDLOGONTYPE,MB_ICONEXCLAMATION);
			m_LogontypeCtrl.SetFocus();
			return FALSE;
		}
		else if (m_Logontype)
		{
			if (m_User=="")
			{
				m_cTree.SelectItem(olditem);
				AfxMessageBox(IDS_ERRORMSG_SITEMANAGERNEEDUSER,MB_ICONEXCLAMATION);
				m_UserCtrl.SetFocus();
				return FALSE;
			}
			if (m_Logontype == 2 && m_Account == _T(""))
			{
				m_cTree.SelectItem(olditem);
				AfxMessageBox(_T("Please enter an account"), MB_ICONEXCLAMATION);
				m_AccountCtrl.SetFocus();
				return FALSE;
			}

		/*	if (m_Pass=="")
			{
				m_cTree.SelectItem(olditem);
				AfxMessageBox(IDS_ERRORMSG_SITEMANAGERNEEDPASS,MB_ICONEXCLAMATION);
				m_PassCtrl.SetFocus();
				return FALSE;
			}	*/
		}
		if (m_cServerType.GetCurSel()==CB_ERR)
			m_cServerType.SetCurSel(0);
		UpdateData(FALSE);

		t_SiteManagerItem *site=(t_SiteManagerItem*)m_cTree.GetItemData(olditem);
		ASSERT(site);
		site->Host=m_Host;
		site->nPort=_ttoi(m_Port);
		site->nLogonType = m_Logontype;
		if (m_Logontype)
		{
			site->User = m_User;
			site->Pass = m_Pass;
			site->Account = m_Account;
		}
		site->bFirewallBypass=m_bFwBypass;
		site->bPassDontSave=m_bPassDontSave;
		if (m_bPassDontSave)
			site->Pass=""; //Ensure that the pass doesn't get saved
		site->nServerType=m_cServerType.GetCurSel();
		CWnd *pComments = GetDlgItem(IDC_SITEMANAGER_COMMENTS);
		if (pComments)
			pComments->GetWindowText(site->Comments);

		if (!m_nDefault)
			m_DefaultSite=olditem;
	}
	return TRUE;
}

t_SiteManagerItem *CSiteManager::GetData(HKEY hRootKey, CString Name)
{
	t_SiteManagerItem *site = new t_SiteManagerItem;
	site->Host = GetKey(hRootKey, Name, "Host");
	site->nPort = _ttoi(GetKey(hRootKey, Name, "Port"));
	site->User = GetKey(hRootKey, Name, "User");
	site->Account = GetKey(hRootKey, Name, "Account");
	site->Pass = CCrypt::decrypt(GetKey(hRootKey, Name, "Pass"));
	site->nLogonType = _ttoi(GetKey(hRootKey, Name, "Logontype"))-1;
	site->RemoteDir = GetKey(hRootKey, Name, "Remote Dir");
	site->LocalDir = GetKey(hRootKey, Name, "Local Dir");
	site->bFirewallBypass = _ttoi(GetKey(hRootKey, Name, "FW Bypass"));
	site->bPassDontSave = _ttoi(GetKey(hRootKey, Name, "Dont Save Pass"));
	site->nServerType = _ttoi(GetKey(hRootKey, Name, "Server Type"));
	site->nPasv = _ttoi(GetKey(hRootKey, Name, "Pasv Mode"));
	site->nTimeZoneOffset = _ttoi(GetKey(hRootKey, Name, "Time Zone Offset"));
	if (site->nTimeZoneOffset < -24 || site->nTimeZoneOffset > 24)
		site->nTimeZoneOffset = 0;
	site->nTimeZoneOffset2 = _ttoi(GetKey(hRootKey, Name, "Time Zone Offset Minutes"));
	if (site->nTimeZoneOffset2 < -59 || site->nTimeZoneOffset2 > 59)
		site->nTimeZoneOffset2 = 0;
	site->nUTF8 = _ttoi(GetKey(hRootKey, Name, "UTF8"));
	if (site->nUTF8 < 0 || site->nUTF8 > 2)
		site->nUTF8 = 0;
	site->Comments = GetKey(hRootKey, Name, "Comments");
	site->name = Name;
	if (site->Host=="" || !site->nPort || site->nLogonType==-1)
	{
		delete site;
		return 0;
	}
	return site;
}

t_SiteManagerItem *CSiteManager::GetDataXML(CMarkupSTL *pMarkup)
{
	t_SiteManagerItem *site = new t_SiteManagerItem;
	site->Host = pMarkup->GetChildAttrib( _T("Host") );
	site->nPort = _ttoi(pMarkup->GetChildAttrib( _T("Port") ));
	site->User = pMarkup->GetChildAttrib( _T("User") );
	site->Account = pMarkup->GetChildAttrib( _T("Account") );
	site->Pass = CCrypt::decrypt(pMarkup->GetChildAttrib( _T("Pass") ));
	site->nLogonType = _ttoi(pMarkup->GetChildAttrib( _T("Logontype") ));
	site->RemoteDir = pMarkup->GetChildAttrib( _T("RemoteDir") );
	site->LocalDir = pMarkup->GetChildAttrib( _T("LocalDir") );
	site->bFirewallBypass = _ttoi(pMarkup->GetChildAttrib( _T("FWBypass") ));
	site->bPassDontSave = _ttoi(pMarkup->GetChildAttrib( _T("DontSavePass") ));
	site->nServerType = _ttoi(pMarkup->GetChildAttrib( _T("ServerType") ));
	site->name = pMarkup->GetChildAttrib( _T("Name") );
	site->nPasv = _ttoi(pMarkup->GetChildAttrib(_T("PasvMode")));
	site->nTimeZoneOffset = _ttoi(pMarkup->GetChildAttrib(_T("TimeZoneOffset")));
	if (site->nTimeZoneOffset < -24 || site->nTimeZoneOffset > 24)
		site->nTimeZoneOffset = 0;
	site->nTimeZoneOffset2 = _ttoi(pMarkup->GetChildAttrib(_T("TimeZoneOffsetMinutes")));
	if (site->nTimeZoneOffset2 < -59 || site->nTimeZoneOffset2 > 59)
		site->nTimeZoneOffset2 = 0;
	site->nUTF8 = _ttoi(pMarkup->GetChildAttrib(_T("UTF8")));
	if (site->nUTF8 < 0 || site->nUTF8 > 2)
		site->nUTF8 = 0;
	site->Comments = pMarkup->GetChildAttrib(_T("Comments"));
	if (site->Host=="" || !site->nPort || site->nLogonType==-1)
	{
		delete site;
		return 0;
	}
	return site;
}

void CSiteManager::SetCtrlState()
{
	bool bItemIsSite = false;
	HTREEITEM item = m_cTree.GetSelectedItem();
	t_SiteManagerItem *data = 0;
	if (item)
		data = (t_SiteManagerItem*)m_cTree.GetItemData(item);

	if (data)
		if (data->NodeType == SITE)
			bItemIsSite = true;

	if (bItemIsSite)
	{
		m_DeleteCtrl.EnableWindow(TRUE);
		m_HostCtrl.EnableWindow(TRUE);
		m_PortCtrl.EnableWindow(TRUE);
		m_UserCtrl.EnableWindow(TRUE);
		m_AccountCtrl.EnableWindow(TRUE);
		m_PassCtrl.EnableWindow(TRUE);
		m_LogontypeCtrl.EnableWindow(TRUE);
		m_LogontypeCtrl2.EnableWindow(TRUE);
		m_LogontypeCtrl3.EnableWindow(TRUE);
		m_ConnectCtrl.EnableWindow(TRUE);
		m_FwBypassCtrl.EnableWindow(TRUE);
		m_UserCtrl.SetReadOnly(m_Logontype ? FALSE : TRUE);
		m_PassCtrl.SetReadOnly(m_Logontype ? FALSE : TRUE);
		m_AccountCtrl.SetReadOnly(m_Logontype != 2);
		m_PassCtrl.EnableWindow((!m_bPassDontSave || !m_Logontype) && !_ttoi(COptions::GetOption(OPTION_RUNINSECUREMODE)));
		m_cDefault.EnableWindow(TRUE);
		m_cPassDontSave.EnableWindow((m_Logontype && !_ttoi(COptions::GetOption(OPTION_RUNINSECUREMODE)))?TRUE:FALSE);
		m_cServerType.EnableWindow(TRUE);
		GetDlgItem(IDC_SITEMANAGER_ADVANCED)->EnableWindow(TRUE);
		GetDlgItem(IDC_SITEMANAGER_COPY)->EnableWindow(TRUE);
		GetDlgItem(IDC_SITEMANAGER_RENAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_SITEMANAGER_COMMENTS)->EnableWindow(TRUE);
	}
	else
	{
		if (item && m_cTree.GetParentItem(item))
		{
			m_DeleteCtrl.EnableWindow(TRUE);
			GetDlgItem(IDC_SITEMANAGER_RENAME)->EnableWindow(TRUE);
		}
		else
		{
			m_DeleteCtrl.EnableWindow(FALSE);
			GetDlgItem(IDC_SITEMANAGER_RENAME)->EnableWindow(FALSE);

		}
		m_HostCtrl.EnableWindow(FALSE);
		m_PortCtrl.EnableWindow(FALSE);
		m_UserCtrl.EnableWindow(FALSE);
		m_AccountCtrl.EnableWindow(FALSE);
		m_PassCtrl.EnableWindow(FALSE);
		m_LogontypeCtrl.EnableWindow(FALSE);
		m_LogontypeCtrl2.EnableWindow(FALSE);
		m_LogontypeCtrl3.EnableWindow(FALSE);
		m_ConnectCtrl.EnableWindow(FALSE);
		m_FwBypassCtrl.EnableWindow(FALSE);
		m_cDefault.EnableWindow(FALSE);
		m_cPassDontSave.EnableWindow(FALSE);
		m_cServerType.EnableWindow(FALSE);
		GetDlgItem(IDC_SITEMANAGER_ADVANCED)->EnableWindow(FALSE);
		GetDlgItem(IDC_SITEMANAGER_COPY)->EnableWindow(FALSE);
		GetDlgItem(IDC_SITEMANAGER_COMMENTS)->EnableWindow(FALSE);
		UpdateData(false);
	}
}

void CSiteManager::OnDelete()
{
	HTREEITEM item=m_cTree.GetSelectedItem();
	if (!item)
		return;
	if (!m_cTree.GetParentItem(item))
		return;
	HTREEITEM parent=m_cTree.GetParentItem(item);
	CString path;
	while (m_cTree.GetParentItem(item))
	{
		path=m_cTree.GetItemText(item)+"\\"+path;
		item=m_cTree.GetParentItem(item);
	}
	path.TrimRight( _T("\\") );
	m_KeysToDelete.push_back(path);

	item=m_cTree.GetSelectedItem();
	while(item!=parent)
	{
		if (m_cTree.ItemHasChildren(item))
		{
			item=m_cTree.GetChildItem(item);
			continue;
		}
		t_SiteManagerItem *site=(t_SiteManagerItem *)m_cTree.GetItemData(item);
		if (site)
			delete site;
		HTREEITEM parent=m_cTree.GetParentItem(item);
		if (m_DefaultSite==item)
			m_DefaultSite=m_cTree.GetRootItem();
		m_cTree.DeleteItem(item);
		item=parent;
	}
	m_cTree.SelectItem(parent);
}

void CSiteManager::SaveData()
{
	CMarkupSTL *pMarkup;
	if (COptions::LockXML(&pMarkup))
	{
		SaveDataXML(pMarkup);
		COptions::UnlockXML();
	}
	HTREEITEM item=m_cTree.GetChildItem(TVI_ROOT);
	CString path="";
	//Only delete the old items that have been renamed or removed
	//Don't delete complete sitemanager key so that crashes while saving would not be fatal
	for (std::list<CString>::iterator iter=m_KeysToDelete.begin(); iter!=m_KeysToDelete.end(); iter++)
		RegDeleteKeyEx("Software\\FileZilla\\Site Manager\\"+*iter);
	m_KeysToDelete.clear();
	//Save the sites, old items will be overwritten.
	while(item)
	{
		t_SiteManagerItem *site=0;
		if (m_cTree.ItemHasChildren(item))
		{
			item=m_cTree.GetChildItem(item);
			path+="\\"+m_cTree.GetItemText(item);
			continue;
		}
		site=(t_SiteManagerItem *)m_cTree.GetItemData(item);
		ASSERT(site);
		if (site->NodeType == SITE)
		{
			SetKey(path,"Default Site",(item==m_DefaultSite)?"1":"0");
			SaveSingleSite(path,site);
		}
		else if (path!="")
			SetKey(path,"","Folder");
		delete site;

		HTREEITEM parent=m_cTree.GetParentItem(item);
		m_cTree.DeleteItem(item);
		item=parent;
		int pos=path.ReverseFind('\\');
		if (pos!=-1)
			path=path.Left(pos);
		else
			path="";
	}
}

void CSiteManager::SaveDataXML(CMarkupSTL *pMarkup)
{
	pMarkup->ResetPos();
	if (!pMarkup->FindChildElem( _T("Sites") ))
		pMarkup->AddChildElem( _T("Sites") );

	pMarkup->IntoElem();

	//Only delete the old items that have been renamed or removed
	//Don't delete complete sitemanager key so that crashes while saving would not be fatal
	pMarkup->SavePos( _T("PREDELETE") );
	CString current;
	BOOL res=pMarkup->FindChildElem();
	while (res)
	{
		if (pMarkup->GetChildTagName()==_T("Folder"))
		{
			if (current!="")
				current+="\\"+pMarkup->GetChildAttrib( _T("Name") );
			else
				current=pMarkup->GetChildAttrib( _T("Name") );

			if (m_KeysToDelete.empty())
				break;

			std::list<CString>::iterator iter;
			for (iter=m_KeysToDelete.begin(); iter!=m_KeysToDelete.end(); iter++)
				if (*iter==current)
				{
					VERIFY(pMarkup->RemoveChildElem());
					m_KeysToDelete.erase(iter);
					break;
				}
			if (m_KeysToDelete.empty())
				break;

			if (iter==m_KeysToDelete.end())
				pMarkup->IntoElem();
			else
			{
				int pos=current.ReverseFind('\\');
				if (pos==-1)
					current="";
				else
					current=current.Left(pos);
			}

			res=pMarkup->FindChildElem();
			while (!res && current!="")
			{
				int pos=current.ReverseFind('\\');
				if (pos==-1)
					current="";
				else
					current=current.Left(pos);
				pMarkup->OutOfElem();
				res=pMarkup->FindChildElem();
			}
		}
		else
		{
			if (m_KeysToDelete.empty())
				break;
			CString str=current+"\\"+pMarkup->GetChildAttrib( _T("Name") );
			str.TrimLeft( _T("\\") );

			for (std::list<CString>::iterator iter=m_KeysToDelete.begin(); iter!=m_KeysToDelete.end(); iter++)
				if (*iter==str)
				{
					VERIFY(pMarkup->RemoveChildElem());
					m_KeysToDelete.erase(iter);
					break;
				}
			if (m_KeysToDelete.empty())
				break;

			res=pMarkup->FindChildElem();
			while (!res && current!="")
			{
				int pos=current.ReverseFind('\\');
				if (pos==-1)
					current="";
				else
					current=current.Left(pos);
				pMarkup->OutOfElem();
				res=pMarkup->FindChildElem();
			}
		}

	}
	VERIFY(pMarkup->RestorePos( _T("PREDELETE") ));

	HTREEITEM item=m_cTree.GetChildItem(TVI_ROOT);
	CString path="";
	//Save the sites, old items will be overwritten.
	while(item)
	{
		t_SiteManagerItem *pSite=0;
		if (m_cTree.ItemHasChildren(item))
		{
			item=m_cTree.GetChildItem(item);
			pSite = reinterpret_cast<t_SiteManagerItem *>(m_cTree.GetItemData(item));

			ASSERT(pSite);

			//Find a entry in the XML file with the same name
			BOOL res=pMarkup->FindChildElem();
			while (res)
			{
				if (!pMarkup->GetChildAttrib( _T("Name") ).CollateNoCase(m_cTree.GetItemText(item)))
					break;
				res=pMarkup->FindChildElem();
			}
			if (!res)
			{
				pMarkup->AddChildElem((pSite->NodeType == SITE) ? _T("Site") : _T("Folder") );
				pMarkup->AddChildAttrib(_T("Name"), m_cTree.GetItemText(item));
			}
			else
			{
				CString name=(pSite->NodeType == SITE) ? _T("Site") : _T("Folder");
				if (pMarkup->GetTagName()!=name)
				{
					pMarkup->RemoveChildElem();
					pMarkup->AddChildElem((pSite->NodeType == SITE) ? _T("Site") : _T("Folder") );
					pMarkup->AddChildAttrib(_T("Name"), m_cTree.GetItemText(item));
				}
			}
			if (pSite->NodeType == SITE)
			{
				SaveSingleSiteXML(pMarkup, pSite);
				pMarkup->SetChildAttrib( _T("DefaultSite"), (item==m_DefaultSite) ? _T("1") : _T("0") );
			}

			pMarkup->IntoElem();

			if (path!="")
				path+="/"+m_cTree.GetItemText(item);
			else
				path=m_cTree.GetItemText(item);
			continue;
		}

		HTREEITEM parent=m_cTree.GetParentItem(item);
		pSite = reinterpret_cast<t_SiteManagerItem *>(m_cTree.GetItemData(item));
		if (pSite)
			delete pSite;
		m_cTree.DeleteItem(item);
		item=parent;
		int pos=path.ReverseFind('/');
		if (pos!=-1)
			path=path.Left(pos);
		else
			path="";
		pMarkup->OutOfElem();
		pMarkup->ResetChildPos();
	}
}

void CSiteManager::OnCancel()
{
	m_cTree.SetRedraw(FALSE);
	HTREEITEM item=m_cTree.GetChildItem(TVI_ROOT);
	while(item)
	{
		if (m_cTree.ItemHasChildren(item))
		{
			item=m_cTree.GetChildItem(item);
			continue;
		}
		t_SiteManagerItem *site=(t_SiteManagerItem *)m_cTree.GetItemData(item);
		if (site)
			delete site;
		HTREEITEM parent=m_cTree.GetParentItem(item);
		m_cTree.DeleteItem(item);
		item=parent;
	}
	m_cTree.SetRedraw(TRUE);
	CDialog::OnCancel();
}

void CSiteManager::OnRadio()
{
	UpdateData(TRUE);
	if (!m_Logontype)
	{
		m_User="anonymous";
		m_Pass=COptions::GetOption(OPTION_ANONPWD);
	}
	else
	{
		HTREEITEM item=m_cTree.GetSelectedItem();
		ASSERT(item);
		t_SiteManagerItem *site=(t_SiteManagerItem*)m_cTree.GetItemData(item);
		ASSERT(site);
		m_User=site->User;
		if (!m_bPassDontSave)
			m_Pass=site->Pass;
		else
			m_Pass="";
	}
	UpdateData(FALSE);
	SetCtrlState();
}

void CSiteManager::OnExit()
{
	m_cTree.SetRedraw(FALSE);
	if (CheckValid())
	{
		SaveData();
		CDialog::OnCancel();
		OnClose();
	}
	m_cTree.SetRedraw(TRUE);
}

void CSiteManager::OnKillfocusHost()
{
	UpdateData(TRUE);
	t_SiteManagerItem *data=(t_SiteManagerItem *)m_cTree.GetItemData(m_cTree.GetSelectedItem());

	int pos=m_Host.Find(':');
	if (pos!=-1)
	{
		int pos2=m_Host.Find('/');
		if (pos2!=-1 && pos2==(pos+1))
		{
			int pos3=m_Host.Find('/',pos2+1);
			if (pos3!=-1 && pos3==(pos2+1))
			{
				if (pos==3)
				{
					CString tmp=m_Host.Left(pos+3);
					tmp.MakeLower();
					if (tmp!="ftp://")
					{
						return;
					}
					else
					{
						m_Host=m_Host.Right(m_Host.GetLength()-pos-3);
					}
				}
				else if (pos==4)
				{
					CString tmp=m_Host.Left(pos+3);
					tmp.MakeLower();
					if (tmp=="ftps://")
					{
						m_cServerType.SetCurSel(1);
						m_Host=m_Host.Right(m_Host.GetLength()-pos-3);
					}
					else if (tmp=="sftp://")
					{
						m_cServerType.SetCurSel(3);
						m_Host=m_Host.Right(m_Host.GetLength()-pos-3);
					}
					else
						return;
				}
			}
		}
	}
	pos=m_Host.Find('/');
	if (pos!=-1)
	{
		if (m_Host.GetLength()>(pos+1) && m_Host[pos+1]==' ')
			data->RemoteDir=m_Host.Right(m_Host.GetLength()-pos);
		else
			data->RemoteDir=m_Host.Right(m_Host.GetLength()-pos-1);

		CServerPath path(data->RemoteDir);
		data->RemoteDir = path.GetPath();

		m_Host=m_Host.Left(pos);
		if (m_Host=="")
		{
			return;
		}


	}
	pos=m_Host.ReverseFind('@');
	if (pos!=-1)
	{
		CString tmp=m_Host.Left(pos);
		int pos2=tmp.Find(':');
		if (pos2==-1)
		{
			return;
		}
		m_Host=m_Host.Right(m_Host.GetLength()-pos-1);
		m_User=tmp.Left(pos2);
		m_Pass=tmp.Right(tmp.GetLength()-pos2-1);
		m_Logontype=1;
		SetCtrlState();
	}
	int pos2 = m_Host.ReverseFind(']');
	pos = m_Host.ReverseFind(':');
	if (pos > pos2)
	{
		m_Port = m_Host.Right(m_Host.GetLength()-pos-1);
		m_Host = m_Host.Left(pos);
	}
	UpdateData(FALSE);
}

void CSiteManager::OnSitemanagerSaveexit()
{
	OnExit();
}

BOOL ReadString(CArchive &ar,CString &str)
{
	if (!ar.ReadString(str))
		return FALSE;
	int pos=str.Find( _T(": ") );
	if (pos==-1)
		return FALSE;
	str=str.Mid(pos+2);

	return TRUE;
}

void CSiteManager::OnSitemanagerFileImport()
{
	if (!CheckValid())
		return;
	CString str;
	str.LoadString(IDS_SITEMANAGER_XMLFILES);
	CFileDialog dlg(TRUE, _T("xml"), _T("FTP Sites"), OFN_FILEMUSTEXIST, str);
	if (dlg.DoModal()==IDOK)
	{
		Import(dlg.GetPathName());
	}
}

void CSiteManager::OnSitemanagerFileExportAsurllist()
{
	if (!CheckValid())
		return;

	USES_CONVERSION;

	CString str;
	str.LoadString(IDS_SITEMANAGER_TEXTFILES);
	CFileDialog dlg(FALSE, _T("txt"), _T("FTP Sites"), OFN_OVERWRITEPROMPT, str);
	if (dlg.DoModal()!=IDOK)
		return;

	CFile file;
	if (!file.Open(dlg.GetFileName(),CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite))
	{
		AfxMessageBox(IDS_ERRORMSG_CANTCREATEFILE,MB_ICONSTOP);
		return;
	}
	HTREEITEM item=m_cTree.GetRootItem();
	while (item)
	{
		while(m_cTree.ItemHasChildren(item))
			item=m_cTree.GetChildItem(item);
		t_SiteManagerItem *site=(t_SiteManagerItem *)m_cTree.GetItemData(item);
		ASSERT(site);
		if (site->NodeType == SITE)
		{
			CString url, tmp;
			if (site->nLogonType)
			{
				url = site->User + ":" + site->Pass + "@";
			}
			url+=site->Host;
			tmp.Format(_T("%d"), site->nPort);
			if (tmp!="21")
				url+=":"+tmp;
			if (site->RemoteDir != "")
				url += "/" + site->RemoteDir;
			url += "\r\n";
			LPCSTR lpszAscii = T2CA(url);
			file.Write(lpszAscii, strlen(lpszAscii));
		}
		HTREEITEM item2 = m_cTree.GetNextSiblingItem(item);
		while (!item2)
		{
			item = m_cTree.GetParentItem(item);
			item2 = m_cTree.GetNextSiblingItem(item);
			if (!item)
				break;
		}
		item=item2;
	}
	file.Close();
}

void CSiteManager::OnSitemanagerFileExportWithalldetails()
{
	USES_CONVERSION;

	if (!CheckValid())
		return;
	CString str;
	str.LoadString(IDS_SITEMANAGER_XMLFILES);
	CFileDialog dlg(FALSE, _T("xml"), _T("FTP Sites"), OFN_OVERWRITEPROMPT, str);
	if (dlg.DoModal()==IDOK)
	{
		CMarkupSTL markup;
		markup.AddElem( _T("FileZilla") );
		markup.AddChildElem( _T("Sites") );
		markup.IntoElem();

		HTREEITEM item=m_cTree.GetRootItem();
		while (item)
		{
			while(m_cTree.ItemHasChildren(item))
			{
				if (item!=m_cTree.GetRootItem())
				{
					markup.AddChildElem( _T("Folder") );
					markup.AddChildAttrib( _T("Name"), m_cTree.GetItemText(item));
					markup.IntoElem();
				}
				item=m_cTree.GetChildItem(item);
			}
			t_SiteManagerItem *site=(t_SiteManagerItem *)m_cTree.GetItemData(item);
			ASSERT(site);
			if (site->NodeType == SITE)
			{
				markup.AddChildElem( _T("Site") );
				markup.AddChildAttrib(_T("Name"), m_cTree.GetItemText(item));
				SaveSingleSiteXML(&markup, site);
			}
			else
			{
				markup.AddChildElem(  _T("Folder") );
				markup.AddChildAttrib( _T("Name"), m_cTree.GetItemText(item));
			}

			HTREEITEM item2=m_cTree.GetNextSiblingItem(item);
			while (!item2)
			{
				markup.OutOfElem();
				item=m_cTree.GetParentItem(item);
				item2=m_cTree.GetNextSiblingItem(item);
				if (!item)
					break;
			}
			item=item2;
		}
		if (!markup.Save(dlg.GetPathName()))
			AfxMessageBox(IDS_ERRORMSG_CANTCREATEFILE,MB_ICONSTOP);
	}
}

static bool startsWith(CString &str, CString start)
{
	bool res = str.Left(start.GetLength()) == start;
	if (res)
		str = str.Mid(start.GetLength());

	return res;
}

void CSiteManager::Import(CString filename)
{
	//First try to read it as XML file
	CMarkupSTL markup;
	if (markup.Load(filename))
	{
		if (markup.FindChildElem( _T("Sites") ))
		{
			markup.IntoElem();
			HTREEITEM item=m_cTree.GetRootItem();
			ImportXML(&markup, item);
			m_cTree.Expand(m_cTree.GetRootItem(), TVE_EXPAND);
			CString msg;
			msg.Format(IDS_STATUSMSG_FILEIMPORTED, filename);
			AfxMessageBox(msg,MB_ICONINFORMATION);
		}
		else
			AfxMessageBox(IDS_ERRORMSG_CANTIMPORTFILE, MB_ICONSTOP);
		return;
	}
	CFile file;
	if (!file.Open(filename,CFile::modeRead|CFile::shareDenyWrite))
	{
		AfxMessageBox(IDS_ERRORMSG_CANTOPENFILE,MB_ICONSTOP);
		return;
	}
	CArchive ar(&file,CArchive::load );
	CString str;

	while (ar.ReadString(str))
	{
		if (str=="")
			continue;
		int pos;

		if ((pos=str.Find( _T(": ") ))!=-1)
		{ //Server list with details
			t_SiteManagerItem *pSite = new t_SiteManagerItem;
			pSite->nServerType = 0;
			pSite->nPort = 21;
			pSite->nLogonType = -1;
			CString folder;

			do
			{
				if (str=="")
					break;
				if (startsWith(str, _T("Name: ")) ||
					startsWith(str, _T("Site Label:")))
					pSite->name = str;
				else if (startsWith(str, _T("Folder: ")))
					folder = str;
				else if (startsWith(str, _T("Host: ")) ||
					startsWith(str, _T("HostName: ")))
					pSite->Host = str;
				else if (startsWith(str, _T("Port: ")))
					pSite->nPort = _ttoi(str);
				else if (startsWith(str, _T("User: ")) ||
					startsWith(str, _T("User Name: ")) ||
					startsWith(str, _T("Username: ")))
					pSite->User = str;
				else if (startsWith(str, _T("Pass: ")) ||
					startsWith(str, _T("Password: ")))
					pSite->Pass = str;
				else if (startsWith(str, _T("Local Dir: ")) ||
					startsWith(str, _T("Default local dir: ")) ||
					startsWith(str, _T("Default local folder: ")))
					pSite->LocalDir = str;
				else if (startsWith(str, _T("Remote Dir: ")) ||
					startsWith(str, _T("Default remote dir: ")) ||
					startsWith(str, _T("Default remote folder: ")))
					pSite->RemoteDir = str;
				else if (startsWith(str, _T("Firewall Bypass: ")))
					pSite->bFirewallBypass = _ttoi(str);
				else if (startsWith(str, _T("Dont Save Pass: ")))
					pSite->bPassDontSave = _ttoi(str);
				else if (startsWith(str, _T("Server Type: ")))
					pSite->nServerType = _ttoi(str);
				else if (startsWith(str, _T("Comments: ")))
					pSite->Comments = str;
				else if (startsWith(str, _T("LogonType: ")))
					pSite->nLogonType = _ttoi(str);
				else if (str == _T("Uses passive mode"))
					pSite->nPasv = 1;
				else if (startsWith(str, _T("Protocol Type: ")))
				{
					if (str == _T("ftps"))
						pSite->nServerType = 1;
					else if (str == _T("ftps2"))
						pSite->nServerType = 2;
					else if (str == _T("ftps3"))
						pSite->nServerType = 4;
					else if (str == _T("sftp"))
						pSite->nServerType = 3;
				}
			} while (ar.ReadString(str));
			if (pSite->bPassDontSave)
				pSite->Pass = "";
			if (pSite->name == _T(""))
				pSite->name = pSite->Host;
			if (pSite->nLogonType == -1)
				if (pSite->User != _T("anonymous") && pSite->User != _T(""))
					pSite->nLogonType = 1;
				else
					pSite->nLogonType = 0;

			if (pSite->nLogonType == 1 && pSite->User == _T(""))
			{
				delete pSite;
				AfxMessageBox(IDS_ERRORMSG_CANTIMPORTFILE, MB_ICONSTOP);
				return;
			}
			if (!pSite->nPort || pSite->Host=="")
			{
				delete pSite;
				AfxMessageBox(IDS_ERRORMSG_CANTIMPORTFILE, MB_ICONSTOP);
				return;
			}
			HTREEITEM hParent=m_cTree.GetRootItem();
			folder.TrimRight( _T("\\") );
			folder.TrimLeft( _T("\\") );
			folder.Replace( _T("\\\\"), _T("\\"));
			while (folder!="")
			{
				CString segment;
				int pos=folder.Find( _T("\\") );
				if (pos==-1)
				{
					segment=folder;
					folder="";
				}
				else
				{
					segment=folder.Left(pos);
					folder=folder.Mid(pos+1);
				}
				HTREEITEM hItem=m_cTree.GetChildItem(hParent);
				while (hItem)
				{
					if (!m_cTree.GetItemText(hItem).CompareNoCase(segment))
						break;
					hItem=m_cTree.GetNextSiblingItem(hItem);
				}
				if (!hItem)
				{
					hItem=m_cTree.InsertItem(segment, hParent, TVI_SORT);
					pSite->name = segment;
					pSite->NodeType = FOLDER;
					m_cTree.SetItemData(hItem, DWORD(pSite));
				}
				if (!COptions::GetOptionVal(OPTION_SITEMANAGERNOEXPANDFOLDERS) || m_cTree.GetRootItem()==hParent)
					m_cTree.Expand(hParent, TVE_EXPAND);
				hParent=hItem;
			}

			CString newname = pSite->name;
			HTREEITEM child=m_cTree.GetChildItem(hParent);
			int i=2;
			while (child)
			{
				CString name2=newname;
				name2.MakeLower();
				CString childstr=m_cTree.GetItemText(child);
				childstr.MakeLower();
				if (childstr==name2)
				{
					child=m_cTree.GetChildItem(hParent);
					newname.Format(_T("%s (%d)"), pSite->name, i);
					i++;
					continue;
				}
				child=m_cTree.GetNextSiblingItem(child);
			}
			HTREEITEM item=m_cTree.InsertItem(newname, 2, 2, hParent, TVI_SORT);
			pSite->name = newname;
			pSite->NodeType=SITE;
			m_cTree.SetItemData(item, (DWORD)pSite);
			if (!COptions::GetOptionVal(OPTION_SITEMANAGERNOEXPANDFOLDERS))
				m_cTree.Expand(hParent, TVE_EXPAND);
		}
		else
		{ //URL list
			t_SiteManagerItem *pSite=new t_SiteManagerItem;
			pSite->nPort=0;
			pSite->bFirewallBypass=0;
			pSite->bPassDontSave=0;

			pos=str.Find('/');
			if (pos!=-1)
			{
				pSite->RemoteDir=str.Mid(pos+1);
				str=str.Left(pos);
				if (str=="")
				{
					delete pSite;
					AfxMessageBox(IDS_ERRORMSG_CANTIMPORTFILE,MB_ICONSTOP);
					return;
				}
			}
			pos=str.ReverseFind('@');
			if (pos!=-1)
			{
				CString tmp=str.Left(pos);
				int pos2=tmp.ReverseFind(':');
				if (pos2==-1)
				{
					delete pSite;
					AfxMessageBox(IDS_ERRORMSG_CANTIMPORTFILE,MB_ICONSTOP);
					return;
				}
				str=str.Mid(pos+1);
				pSite->User=tmp.Left(pos2);
				pSite->Pass=tmp.Mid(pos2+1);
			}
			pos=str.ReverseFind(':');
			if (pos!=-1)
			{
				pSite->nPort=_ttoi(str.Mid(pos+1));
				str=str.Left(pos);

			}

			if (!pSite->nPort)
			{
				pSite->nPort=21;
			}
			if (pSite->User=="")
				pSite->nLogonType=0;
			else
				pSite->nLogonType=1;
			pSite->Host=str;
			pSite->name = str;
			HTREEITEM hParent=m_cTree.GetRootItem();
			CString newname = pSite->name;
			HTREEITEM child=m_cTree.GetChildItem(hParent);
			int i=2;
			while (child)
			{
				CString name2=newname;
				name2.MakeLower();
				CString childstr=m_cTree.GetItemText(child);
				childstr.MakeLower();
				if (childstr==name2)
				{
					child=m_cTree.GetChildItem(hParent);
					newname.Format(_T("%s (%d)"), pSite->name, i);
					i++;
					continue;
				}
				child=m_cTree.GetNextSiblingItem(child);
			}
			HTREEITEM item=m_cTree.InsertItem(newname, 2, 2, hParent, TVI_SORT);
			pSite->name=newname;
			pSite->NodeType=SITE;
			m_cTree.SetItemData(item, (DWORD)pSite);
			if (!COptions::GetOptionVal(OPTION_SITEMANAGERNOEXPANDFOLDERS))
				m_cTree.Expand(hParent, TVE_EXPAND);
		}
	}
	m_cTree.Expand(m_cTree.GetRootItem(), TVE_EXPAND);
	CString msg;
	msg.Format(IDS_STATUSMSG_FILEIMPORTED,filename);
	AfxMessageBox(msg, MB_ICONINFORMATION);
}

void CSiteManager::ImportXML(CMarkupSTL *pMarkup, HTREEITEM &parent)
{
	while (pMarkup->FindChildElem())
	{
		if (pMarkup->GetChildTagName()==_T("Folder") )
		{
			CString name=pMarkup->GetChildAttrib( _T("Name") );
			HTREEITEM child=m_cTree.GetChildItem(parent);
			int i=2;
			//Iterate parent's children, looking for one that has the same name as the one we are importing.
			while (child)
			{
				CString name2=name;
				name2.MakeLower();
				CString childstr=m_cTree.GetItemText(child);
				childstr.MakeLower();
				if (childstr==name2)
					break;
				child=m_cTree.GetNextSiblingItem(child);
			}
			HTREEITEM hItem;
			if (!child) //We didn't find one, insert one
			{
				hItem=m_cTree.InsertItem(name, parent, TVI_SORT);
				t_SiteManagerItem *pFolder=new t_SiteManagerItem;
				pFolder->name = name;
				pFolder->NodeType = FOLDER;
				m_cTree.SetItemData(hItem, (DWORD)pFolder);
			}
			else //We found one, so we will update it
				hItem=child;
			pMarkup->IntoElem();
			ImportXML(pMarkup, hItem);
			pMarkup->OutOfElem();

			if (!COptions::GetOptionVal(OPTION_SITEMANAGERNOEXPANDFOLDERS))
				m_cTree.Expand(hItem, TVE_EXPAND);
		}
		else if (pMarkup->GetChildTagName()==_T("Site") )
		{
			t_SiteManagerItem *site=GetDataXML(pMarkup);
			if (site)
			{
				CString name=pMarkup->GetChildAttrib( _T("Name") );
				CString newname=name;
				HTREEITEM child=m_cTree.GetChildItem(parent);
				int i=2;
				while (child)
				{
					CString name2=newname;
					name2.MakeLower();
					CString childstr=m_cTree.GetItemText(child);
					childstr.MakeLower();
					if (childstr==name2)
					{
						child=m_cTree.GetChildItem(parent);
						newname.Format(_T("%s (%d)"), name, i);
						i++;
						continue;
					}
					child=m_cTree.GetNextSiblingItem(child);
				}
				HTREEITEM item=m_cTree.InsertItem(newname, 2, 2, parent, TVI_SORT);
				site->name = newname;
				site->NodeType = SITE;
				m_cTree.SetItemData(item, (DWORD)site);
				if (!COptions::GetOptionVal(OPTION_SITEMANAGERNOEXPANDFOLDERS))
					m_cTree.Expand(item, TVE_EXPAND);
				if (_ttoi(pMarkup->GetChildAttrib( _T("DefaultSite") )))
					m_DefaultSite=item;
			}
		}
	}
}

int CSiteManager::AddAndShow(t_server &server)
{
	m_addserver=server;
	m_bAddServer=TRUE;
	return DoModal();
}

int CSiteManager::ImportAndShow(CString FileToImport)
{
	m_FileToImport=FileToImport;
	return DoModal();
}

void CSiteManager::OnClose()
{
	HTREEITEM item=m_cTree.GetChildItem(TVI_ROOT);
	while(item)
	{
		if (m_cTree.ItemHasChildren(item))
		{
			item=m_cTree.GetChildItem(item);
			continue;
		}
		t_SiteManagerItem *site=(t_SiteManagerItem *)m_cTree.GetItemData(item);
		if (site)
			delete site;
		HTREEITEM parent=m_cTree.GetParentItem(item);
		m_cTree.DeleteItem(item);
		item=parent;
	}
	CDialog::OnClose();
}

void CSiteManager::SaveSingleSite(CString name, t_SiteManagerItem *site)
{
	name.TrimLeft( _T("\\") );
	SetKey(name,"Host",site->Host);
	CString str;
	str.Format(_T("%d"), site->nPort);
	SetKey(name,"Port",str);
	SetKey(name,"User",site->User);
	SetKey(name, "Account", site->Account);
	SetKey(name,"Remote Dir",site->RemoteDir);
	SetKey(name,"Local Dir",site->LocalDir);
	if (_ttoi(COptions::GetOption(OPTION_RUNINSECUREMODE)) || site->bPassDontSave)
		SetKey(name, _T("Pass"), _T(""));
	else
		SetKey(name, "Pass", CCrypt::encrypt(site->Pass));
	str.Format(_T("%d"), site->nLogonType+1);
	SetKey(name,"Logontype",str);
	str=site->bFirewallBypass?"1":"0";
	SetKey(name,"FW Bypass",str);
	str=site->bPassDontSave?"1":"0";
	SetKey(name,"Dont Save Pass",str);
	str.Format(_T("%d"), site->nServerType);
	SetKey(name,"Server Type",str);
	str.Format(_T("%d"), site->nPasv);
	SetKey(name, "Pasv Mode", str);
	str.Format(_T("%d"), site->nTimeZoneOffset);
	SetKey(name, "Time Zone Offset", str);
	str.Format(_T("%d"), site->nTimeZoneOffset2);
	SetKey(name, "Time Zone Offset Minutes", str);
	SetKey(name, "Comments", site->Comments);
	str.Format(_T("%d"), site->nUTF8);
	SetKey(name, "UTF8", str);
}

void CSiteManager::SaveSingleSiteXML(CMarkupSTL *pMarkup, t_SiteManagerItem *site)
{
	pMarkup->AddChildAttrib( _T("Host"), site->Host);
	pMarkup->AddChildAttrib( _T("Port"), site->nPort);
	pMarkup->AddChildAttrib( _T("User"), site->User);
	pMarkup->AddChildAttrib( _T("Account"), site->Account);
	pMarkup->AddChildAttrib( _T("RemoteDir"), site->RemoteDir);
	pMarkup->AddChildAttrib( _T("LocalDir"), site->LocalDir);
	if (_ttoi(COptions::GetOption(OPTION_RUNINSECUREMODE)) || site->bPassDontSave)
		pMarkup->AddChildAttrib( _T("Pass"), _T(""));
	else
		pMarkup->AddChildAttrib( _T("Pass"), CCrypt::encrypt(site->Pass));
	pMarkup->AddChildAttrib( _T("Logontype"), site->nLogonType);
	pMarkup->AddChildAttrib( _T("FWBypass"), site->bFirewallBypass);
	pMarkup->AddChildAttrib( _T("DontSavePass"), site->bPassDontSave);
	pMarkup->AddChildAttrib( _T("ServerType"), site->nServerType);
	pMarkup->AddChildAttrib( _T("PasvMode"), site->nPasv);
	pMarkup->AddChildAttrib( _T("TimeZoneOffset"), site->nTimeZoneOffset);
	pMarkup->AddChildAttrib( _T("TimeZoneOffsetMinutes"), site->nTimeZoneOffset2);
	pMarkup->AddChildAttrib( _T("Comments"), site->Comments);
	pMarkup->AddChildAttrib( _T("UTF8"), site->nUTF8);
}

void CSiteManager::OnPassdontsave()
{
	UpdateData(TRUE);
	m_PassCtrl.EnableWindow(!m_bPassDontSave);
	m_Pass="";
	UpdateData(FALSE);
}

//
// Read Sites from the Windows Registry
//
// Parameters:
// substring - registry subkey which contains the set of sites to read.
// parent	 - parent to which the sites and folders found within given
//			   Registry key are children.
//
void CSiteManager::ReadTree(CString substring, const LPVOID &parent, LPVOID lpData, LPVOID lpDefault,
							BUILDSITETREEPROC_NEW_ITEM NewItemProc, BUILDSITETREEPROC_NEW_FOLDER NewFolderProc,
							BUILDSITETREEPROC_AFTER AfterProc)
{
	substring.TrimLeft( _T("\\") );
	substring.TrimRight( _T("\\") );
	HKEY key;
	HKEY hRootKey = HKEY_CURRENT_USER;
	if (RegOpenKey(HKEY_CURRENT_USER,"Software\\FileZilla\\Site Manager\\"+substring,&key) != ERROR_SUCCESS)
	{
		if (RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\FileZilla\\Site Manager\\"+substring,&key) != ERROR_SUCCESS)
			return;
		hRootKey = HKEY_LOCAL_MACHINE;
	}

	TCHAR buffer[1000];
	int index=0;
	unsigned char value[1000];
	DWORD length=1000;
	CString value2=value;
	value[0]=0;
	RegQueryValueEx(key, _T(""), 0, 0, value, &length);
	if (!_tcscmp((LPCTSTR)value, _T("Folder") ) || substring=="")
	{
		CString name;
		if (substring.ReverseFind( '\\' )!=-1)
			name=substring.Mid(substring.ReverseFind( '\\' )+1);
		else
			name=substring;
		LPVOID item = parent;
		if (substring!="")
		{
			if (NewFolderProc)
				NewFolderProc( lpData, name, item);
		}
		length=1000;
		while(RegEnumKeyEx(key, index, buffer, &length, 0, 0, 0, 0)==ERROR_SUCCESS)
		{
			ReadTree(substring+"\\"+buffer, item, lpData, lpDefault, NewItemProc, NewFolderProc, AfterProc);
			index++;
			length=1000;
		}
		if (AfterProc)
			AfterProc(lpData, name, item);
	}
	else
	{
		CString name;
		if (substring.ReverseFind('\\')!=-1)
			name = substring.Mid(substring.ReverseFind('\\')+1);
		else
			name = substring;
		t_SiteManagerItem *site = GetData(hRootKey, substring);
		if (site)
		{
			site->name = name;
			site->NodeType = SITE;
			if (NewItemProc)
				NewItemProc(lpData, name, parent, site, GetKey(hRootKey, substring, "Default Site")=="1", lpDefault);
		}
		index++;
		length = 1000;
	}
	RegCloseKey(key);
}

//
// Read Sites from the XML Markup
//
// Parameters:
// pMarkup - pointer to CMarkup object preset to be pointing to the first item
//           in a set of sites (either a <Sites> or <Folder> tag)
// parent  - parent to which the sites and folders found under the current
//           branch are children.
//
void CSiteManager::ReadTreeXML(CMarkupSTL *pMarkup, const LPVOID &parent, LPVOID lpData, LPVOID lpDefault, BUILDSITETREEPROC_NEW_ITEM NewItemProc,
							BUILDSITETREEPROC_NEW_FOLDER NewFolderProc, BUILDSITETREEPROC_AFTER AfterProc)
{
	while (pMarkup->FindChildElem())
	{
		if (pMarkup->GetChildTagName()==_T("Folder") )
		{
			LPVOID item = parent;
			CString name=pMarkup->GetChildAttrib( _T("Name") );
			if (NewFolderProc)
					NewFolderProc(lpData, name, item);
			pMarkup->IntoElem();
			ReadTreeXML(pMarkup, item, lpData, lpDefault, NewItemProc, NewFolderProc, AfterProc);
			pMarkup->OutOfElem();
			if (AfterProc)
				AfterProc(lpData, name, item);

		}
		else if (pMarkup->GetChildTagName()==_T("Site") )
		{
			t_SiteManagerItem *site=GetDataXML(pMarkup);
			if (site)
			{
				site->NodeType = SITE;
				CString name=pMarkup->GetChildAttrib( _T("Name") );
				if ( NewItemProc)
					NewItemProc(lpData, name, parent, site, _ttoi(pMarkup->GetChildAttrib( _T("DefaultSite") ))?1:0, lpDefault);
			}
		}
	}
}

void CSiteManager::OnSelchangingSitemanagertree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if (!CheckValid())
	{
		*pResult = 1;
		return;
	}
	pNMTreeView->itemNew;
	pNMTreeView->itemOld;
	*pResult = 0;
}

void CSiteManager::OnSelchangedSitemanagertree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	t_SiteManagerItem *pSiteManagerItem = (t_SiteManagerItem *)pNMTreeView->itemNew.lParam;
	ASSERT(pSiteManagerItem);
	if (pSiteManagerItem->NodeType == SITE)
	{
		DisplaySite(pSiteManagerItem);
	}
	else
		DisplaySite(0);
	*pResult = 0;
}

void CSiteManager::DisplaySite(t_SiteManagerItem *site)
{
	if (site && site->NodeType == SITE)
	{
		m_Host=site->Host;
		m_Port.Format( _T("%d"), site->nPort);
		m_Logontype = site->nLogonType;
		if (site->bPassDontSave)
			site->Pass="";
		if (m_Logontype)
		{
			m_User = site->User;
			m_Pass = site->Pass;
			m_Account = site->Account;
		}
		else
		{
			m_User = _T("anonymous");
			m_Pass = COptions::GetOption(OPTION_ANONPWD);
			m_Account = _T("");
		}
		m_bFwBypass=site->bFirewallBypass;
		m_bPassDontSave=site->bPassDontSave;
		m_nDefault=(m_cTree.GetSelectedItem()==m_DefaultSite)?0:-1;
		m_cServerType.SetCurSel(site->nServerType);
		m_nServerType = site->nServerType;
		CWnd *pComments = GetDlgItem(IDC_SITEMANAGER_COMMENTS);
		if (pComments)
			pComments->SetWindowText(site->Comments);
	}
	else
	{
		m_Host="";
		m_Port="";
		m_User="";
		m_Account = _T("");
		m_Pass="";
		m_Logontype=-1;
		m_bFwBypass=0;
		m_nDefault=-1;
		m_cServerType.SetCurSel(-1);
		CWnd *pComments = GetDlgItem(IDC_SITEMANAGER_COMMENTS);
		if (pComments)
			pComments->SetWindowText(_T(""));
	}
	UpdateData(FALSE);
	SetCtrlState();
}

void CSiteManager::OnItemexpandedSitemanagertree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	int image=(pNMTreeView->itemNew.state&TVIS_EXPANDED)?1:0;
	m_cTree.SetItemImage(pNMTreeView->itemNew.hItem,image,image);
	*pResult = 0;
}

BOOL CSiteManager::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message==WM_KEYDOWN)
	{
		CEdit *edit=m_cTree.GetEditControl();
		if (edit) //Send key messages to the edit field
		{
			if (pMsg->wParam==VK_RETURN || pMsg->wParam==VK_ESCAPE)
			{
				edit->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
		}
		else if (pMsg->wParam==VK_F2)
		{
			HTREEITEM item=m_cTree.GetSelectedItem();
			if (!item)
				return TRUE;
			if (!m_cTree.GetParentItem(item))
				return TRUE;
			m_cTree.EditLabel(item);
			return TRUE;
		}
		else if (pMsg->wParam==VK_DELETE)
		{
			if (GetFocus()==&m_cTree)
			{
				HTREEITEM item=m_cTree.GetSelectedItem();
				if (!item)
					return TRUE;
				if (!m_cTree.GetParentItem(item))
					return TRUE;
				OnDelete();
				return TRUE;
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSiteManager::OnDblclkSitemanagertree(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	HTREEITEM item=m_cTree.GetSelectedItem();
	t_SiteManagerItem *site=0;
	if (item)
		site=(t_SiteManagerItem *)m_cTree.GetItemData(item);
	ASSERT(site);
	if (site->NodeType != SITE)
		return;
	*pResult = TRUE;
	OnOK();
}

DWORD CSiteManager::RegDeleteKeyEx(CString subkey)
{
	HKEY key;
	int retval = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_ALL_ACCESS, &key);
	if (retval==ERROR_SUCCESS)
	{
		TCHAR buffer[1024];
		int index=0;
		while (RegEnumKey(key,index,buffer,1024)==ERROR_SUCCESS)
		{
			CString newkey=subkey+"\\";
			newkey+=buffer;
			if ((retval=RegDeleteKeyEx(newkey)) != ERROR_SUCCESS)
				index++;
		}
		RegCloseKey(key);
		retval = RegDeleteKey(HKEY_CURRENT_USER, subkey);
	}
	return retval;
}

void CSiteManager::OnEndlabeleditSitemanagertree(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	if (!pTVDispInfo->item.pszText)
		return;

	CString text = pTVDispInfo->item.pszText;
	text.Replace(_T("\\"), _T(""));
	text.Replace(_T("\""), _T(""));
	text.TrimLeft(' ');
	text.TrimRight(' ');
	_tcscpy(pTVDispInfo->item.pszText, text);
	if (text == "")
		return;

	CString newstr = text;
	newstr.MakeLower();
	HTREEITEM item=m_cTree.GetNextSiblingItem(pTVDispInfo->item.hItem);
	while(item)
	{
		CString oldstr=m_cTree.GetItemText(item);
		oldstr.MakeLower();
		if (oldstr==newstr)
		{
			AfxMessageBox(IDS_ERRORMSG_SITEMANAGERNAMEINUSE,MB_OK|MB_ICONEXCLAMATION);
			m_cTree.SetFocus();
			m_cTree.EditLabel(pTVDispInfo->item.hItem);
			*pResult=0;
			return;
		}
		item=m_cTree.GetNextSiblingItem(item);
	}
	item=m_cTree.GetPrevSiblingItem(pTVDispInfo->item.hItem);
	while(item)
	{
		CString oldstr=m_cTree.GetItemText(item);
		oldstr.MakeLower();
		if (oldstr==newstr)
		{
			AfxMessageBox(IDS_ERRORMSG_SITEMANAGERNAMEINUSE,MB_OK|MB_ICONEXCLAMATION);
			m_cTree.SetFocus();
			m_cTree.EditLabel(pTVDispInfo->item.hItem);
			*pResult=0;
			return;
		}
		item=m_cTree.GetPrevSiblingItem(item);
	}
	item=pTVDispInfo->item.hItem;
	CString path;
	while (m_cTree.GetParentItem(item))
	{
		path=m_cTree.GetItemText(item)+"\\"+path;
		item=m_cTree.GetParentItem(item);
	}
	path.TrimRight( _T("\\") );
	m_KeysToDelete.push_back(path);
	m_cTree.SetItemText(pTVDispInfo->item.hItem, text);
	((t_SiteManagerItem*)m_cTree.GetItemData(pTVDispInfo->item.hItem))->name = text;

	if (COptions::GetOptionVal(OPTION_SORTSITEMANAGERFOLDERSFIRST))
	{
		TVSORTCB pMy_Sort;
		pMy_Sort.hParent = m_cTree.GetParentItem(pTVDispInfo->item.hItem);
		pMy_Sort.lpfnCompare = SortFoldersFirst;
		pMy_Sort.lParam = (LPARAM) &m_cTree;
		m_cTree.SortChildrenCB(&pMy_Sort);
	}
	else
		m_cTree.SortChildren(m_cTree.GetParentItem(pTVDispInfo->item.hItem));

	*pResult = TRUE;
}

void CSiteManager::OnSitemanagerNewfolder()
{
	if (!CheckValid())
		return;
	HTREEITEM parent=m_cTree.GetSelectedItem();
	if (!parent)
		parent=m_cTree.GetRootItem();
	if (((t_SiteManagerItem *)m_cTree.GetItemData(parent))->NodeType == SITE)
		parent=m_cTree.GetParentItem(parent);

	CString name;
	name.Format(IDS_SITEMANAGER_NEWFOLDER);

	HTREEITEM child=m_cTree.GetChildItem(parent);
	int i=2;
	while (child)
	{
		CString name2=name;
		name2.MakeLower();
		CString childstr=m_cTree.GetItemText(child);
		childstr.MakeLower();
		if (childstr==name2)
		{
			child=m_cTree.GetChildItem(parent);
			name.Format(IDS_SITEMANAGER_NEWFOLDERWITHNUM,i);
			i++;
			continue;
		}
		child=m_cTree.GetNextItem(child,TVGN_NEXT);
	}

	HTREEITEM item=m_cTree.InsertItem(name,0,0,parent,TVI_SORT);
	t_SiteManagerItem *pFolder = new t_SiteManagerItem;
	pFolder->name=name;
	pFolder->NodeType=FOLDER;
	m_cTree.SetItemData(item, (DWORD)pFolder);
	// Re-sort with folders first if that option is selected.  This must come AFTER the SetItemData command above.
	if (COptions::GetOptionVal(OPTION_SORTSITEMANAGERFOLDERSFIRST))
	{
		TVSORTCB pMy_Sort;
		pMy_Sort.hParent = parent;
		pMy_Sort.lpfnCompare = SortFoldersFirst;
		pMy_Sort.lParam = (LPARAM) &m_cTree;
		m_cTree.SortChildrenCB(&pMy_Sort);
	}

	m_cTree.Expand(parent,TVE_EXPAND);m_cTree.SelectItem(item);
	DisplaySite(0);
	m_cTree.EditLabel(item);
}

t_SiteManagerItem * CSiteManager::GetDefaultSite(CString substring)
{
	if (substring == "")
	{
		CMarkupSTL *pMarkup;
		if (COptions::LockXML(&pMarkup))
		{
			pMarkup->ResetPos();
			if (pMarkup->FindChildElem( _T("Sites") ))
			{
				pMarkup->IntoElem();
				t_SiteManagerItem *pSite = GetDefaultSiteXML(pMarkup);
				COptions::UnlockXML();
				return pSite;
			}
			COptions::UnlockXML();
		}
	}

	HKEY key;
	HKEY hRootKey = HKEY_CURRENT_USER;
	if (RegOpenKey(HKEY_CURRENT_USER, "Software\\FileZilla\\Site Manager\\" + substring, &key) != ERROR_SUCCESS)
	{
		if (RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\FileZilla\\Site Manager\\"+substring, &key) != ERROR_SUCCESS)
			return 0;

		hRootKey = HKEY_LOCAL_MACHINE;
	}

	TCHAR buffer[1000];
	int index=0;
	unsigned char value[1000];
	DWORD length=1000;
	CString value2=value;
	value[0]=0;
	RegQueryValueEx(key, _T(""), 0, 0, value, &length);
	if (!_tcscmp((LPCTSTR)value, _T("Folder") ) || substring=="")
	{
		length=1000;
		while(RegEnumKeyEx(key,index,buffer,&length,0,0,0,0)==ERROR_SUCCESS)
		{
			t_SiteManagerItem *site;
			if(substring == "")
				site=GetDefaultSite(buffer);
			else
				site=GetDefaultSite(substring+"\\"+buffer);
			if (site)
				return site;
			index++;
			length=1000;
		}
	}
	else
	{
		t_SiteManagerItem *site = GetData(hRootKey, substring);
		if (site)
		{
			if (GetKey(hRootKey, substring,"Default Site")=="1")
				return site;
			else
				delete site;
		}
		index++;
		length=1000;
	}
	RegCloseKey(key);

	return 0;
}

t_SiteManagerItem * CSiteManager::GetDefaultSiteXML(CMarkupSTL *pMarkup)
{
	while (pMarkup->FindChildElem())
	{
		if (pMarkup->GetChildTagName()==_T("Folder") )
		{
			pMarkup->IntoElem();
			t_SiteManagerItem *pSite=GetDefaultSiteXML(pMarkup);
			if (pSite)
				return pSite;
			pMarkup->OutOfElem();
		}
		else if (pMarkup->GetChildTagName()==_T("Site") )
		{
			t_SiteManagerItem *site=GetDataXML(pMarkup);
			if (site)
			{
				if (_ttoi(pMarkup->GetChildAttrib( _T("DefaultSite") )))
					return site;
				else
					delete site;
			}

		}
	}
	return 0;
}

void CSiteManager::OnBegindragSitemanagertree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen

	//Start of a Drag&Drop operation
	*pResult = 0;

	if (!CheckValid())
	{
		return;
	}

	if (!pNMTreeView->itemNew.hItem || pNMTreeView->itemNew.hItem==m_cTree.GetRootItem())
		return;

	m_cTree.EnsureVisible(pNMTreeView->itemNew.hItem);


	m_pDragImage=m_cTree.CreateDragImage(pNMTreeView->itemNew.hItem);
	if (m_pDragImage == NULL)
	{
		return;
	}
	m_hDragItem=pNMTreeView->itemNew.hItem;

	CRect rect;
	::GetWindowRect(m_cTree.m_hWnd, &rect);
	CPoint pt2 = pNMTreeView->ptDrag;
	pt2.Offset(rect.left, rect.top);
	m_pDragImage->BeginDrag(0, CPoint(0,0));
	m_pDragImage->DragEnter(0, pt2);

	SetCapture();
}

void CSiteManager::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_pDragImage) // In Drag&Drop mode ?
	{
		CPoint ptDropPoint(point);
		ClientToScreen(&ptDropPoint);
		CWnd* pDropWnd = CWnd::WindowFromPoint(ptDropPoint);

		m_pDragImage->DragMove(ptDropPoint);

		//Scroll through tree
		RECT rect;
		m_cTree.GetWindowRect(&rect);
		if (ptDropPoint.x>=rect.left && ptDropPoint.x<=rect.right)
		{
			m_CurPos=ptDropPoint;
			if (ptDropPoint.y<(rect.top+5))
			{
				if (!m_nDragScrollTimerID)
				{

					HTREEITEM hItem=m_cTree.GetFirstVisibleItem();
					if (hItem)
						hItem=m_cTree.GetNextItem(hItem, TVGN_PREVIOUSVISIBLE);
					if (hItem)
					{
						m_nDragScrollTimerID=SetTimer(365, 250, 0);
						m_pDragImage->DragShowNolock(FALSE);
						m_cTree.EnsureVisible(hItem);
						m_cTree.RedrawWindow();
						m_pDragImage->DragShowNolock(TRUE);
					}
				}
			}
			else if (ptDropPoint.y>(rect.bottom-5))
			{
				if (!m_nDragScrollTimerID)
				{
					HTREEITEM hItem=m_cTree.GetFirstVisibleItem();
					while(hItem)
					{

						RECT iRect;
						m_cTree.GetItemRect(hItem, &iRect, TRUE);
						if (iRect.bottom>=(rect.bottom-rect.top))
						{
							m_nDragScrollTimerID=SetTimer(365, 250, 0);
							m_pDragImage->DragShowNolock(FALSE);
							m_cTree.EnsureVisible(hItem);
							m_pDragImage->DragShowNolock(TRUE);
							break;
						}
						hItem=m_cTree.GetNextItem(hItem, TVGN_NEXTVISIBLE);
					}
				}
			}
			else
			{
				if (m_nDragScrollTimerID)
					KillTimer(m_nDragScrollTimerID);
				m_nDragScrollTimerID=0;
			}
		}
		if (pDropWnd != &m_cTree)
		{
			SetCursor(m_hcNo);
			return;
		}

		HTREEITEM hItem=m_cTree.HitTest(point);
		if (hItem!=m_hDropItem)
		{
			m_pDragImage->DragShowNolock(FALSE);
			m_cTree.SelectDropTarget(hItem);
			m_pDragImage->DragShowNolock(TRUE);
			m_hDropItem=hItem;
		}

		int bError=FALSE;
		if (hItem==m_hDragItem)
			bError=FALSE;
		else if (!hItem || ((t_SiteManagerItem *)m_cTree.GetItemData(hItem))->NodeType == SITE)
			bError=TRUE;
		else
			bError=FALSE;
		HTREEITEM hTemp=hItem;
		hTemp=m_cTree.GetParentItem(hTemp);
		while ( (hTemp) )
		{
			if (hTemp==m_hDragItem)
				bError=TRUE;
			hTemp=m_cTree.GetParentItem(hTemp);
		}

		if (!bError)
		{
			CString dragstr=m_cTree.GetItemText(m_hDragItem);
			dragstr.MakeLower();
			HTREEITEM hChild=m_cTree.GetChildItem(hItem);
			while (hChild && !bError)
			{
				if (hChild!=m_hDragItem)
				{
					CString str=m_cTree.GetItemText(hChild);
					str.MakeLower();
					if (str==dragstr)
						bError=TRUE;
					else
						hChild=m_cTree.GetNextItem(hChild, TVGN_NEXT);
				}
				else
					hChild=m_cTree.GetNextItem(hChild, TVGN_NEXT);
			}
		}

		SetCursor( bError ? m_hcNo : m_hcArrow );

		return;
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CSiteManager::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_pDragImage) // In Drag&Drop mode ?
	{
		if (m_nDragScrollTimerID)
			KillTimer(m_nDragScrollTimerID);
		m_nDragScrollTimerID=0;

		ASSERT(m_hDragItem);
		HTREEITEM hDropItem=m_cTree.GetDropHilightItem();
		m_cTree.SelectDropTarget(0);
		::ReleaseCapture();
		m_pDragImage->DragLeave(GetDesktopWindow());
		m_pDragImage->EndDrag();

		CPoint pt(point);
		ClientToScreen(&pt);
		CWnd* m_pDropWnd = WindowFromPoint(pt);

		if (m_pDropWnd == &m_cTree)
		{
			BOOL nError=0;
			if (hDropItem==m_hDragItem || m_cTree.GetParentItem(m_hDragItem)==hDropItem)
				nError=2;
			else if (!hDropItem || ((t_SiteManagerItem*)m_cTree.GetItemData(hDropItem))->NodeType == SITE)
				nError=1;
			else
				nError=0;
			HTREEITEM hTemp=hDropItem;
			hTemp=m_cTree.GetParentItem(hTemp);
			while ( (hTemp) )
			{
				if (hTemp==m_hDragItem)
					nError=1;
				hTemp=m_cTree.GetParentItem(hTemp);
			}
			if (!nError)
			{
				CString dragstr=m_cTree.GetItemText(m_hDragItem);
				dragstr.MakeLower();
				HTREEITEM hChild=m_cTree.GetChildItem(hDropItem);
				while (hChild && !nError)
				{
					if (hChild!=m_hDragItem)
					{
						CString str=m_cTree.GetItemText(hChild);
						str.MakeLower();
						if (str==dragstr)
							nError=1;
						else
							hChild=m_cTree.GetNextItem(hChild, TVGN_NEXT);
					}
					else
						hChild=m_cTree.GetNextItem(hChild, TVGN_NEXT);
				}
			}

			if (nError==1)
				MessageBeep(MB_ICONEXCLAMATION);
			if (!nError)
			{ //Do the actual moving
				HTREEITEM hDragParent=m_cTree.GetParentItem(m_hDragItem);
				CString path;
				hTemp=hDragParent;
				while (m_cTree.GetParentItem(hTemp))
				{
					path=m_cTree.GetItemText(hTemp)+"\\"+path;
					hTemp=m_cTree.GetParentItem(hTemp);
				}
				path+=m_cTree.GetItemText(m_hDragItem);
				m_KeysToDelete.push_back(path);

				HTREEITEM hDragTemp=m_hDragItem;
				HTREEITEM hDropTemp=hDropItem;
				TCHAR str[1000];
				TVITEM item={0};
				item.pszText=str;
				item.cchTextMax=1000;
				item.hItem=hDragTemp;
				item.mask=TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM|TVIF_TEXT;
				VERIFY(m_cTree.GetItem(&item));
				hDropTemp=m_cTree.InsertItem(item.pszText, item.iImage, item.iSelectedImage, hDropTemp);
				m_cTree.SetItemData(hDropTemp, item.lParam);

				if (COptions::GetOptionVal(OPTION_SORTSITEMANAGERFOLDERSFIRST))
				{
					TVSORTCB pMy_Sort;
					pMy_Sort.hParent = m_cTree.GetParentItem(hDropTemp);
					pMy_Sort.lpfnCompare = SortFoldersFirst;
					pMy_Sort.lParam = (LPARAM) &m_cTree;
					m_cTree.SortChildrenCB(&pMy_Sort);
				}
				else
					m_cTree.SortChildren(m_cTree.GetParentItem(hDropTemp));

				m_cTree.Expand(m_cTree.GetParentItem(hDropTemp), TVE_EXPAND);

				while(hDragTemp!=hDragParent)
				{
					if (m_cTree.ItemHasChildren(hDragTemp))
					{
						hDragTemp=m_cTree.GetChildItem(hDragTemp);
						TVITEM item={0};
						item.pszText=str;
						item.cchTextMax=1000;
						item.hItem=hDragTemp;
						item.mask=TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM|TVIF_TEXT;
						VERIFY(m_cTree.GetItem(&item));
						hDropTemp=m_cTree.InsertItem(item.pszText, item.iImage, item.iSelectedImage, hDropTemp);
						m_cTree.SetItemData(hDropTemp, item.lParam);

						if (COptions::GetOptionVal(OPTION_SORTSITEMANAGERFOLDERSFIRST))
						{
							TVSORTCB pMy_Sort;
							pMy_Sort.hParent = m_cTree.GetParentItem(hDropTemp);
							pMy_Sort.lpfnCompare = SortFoldersFirst;
							pMy_Sort.lParam = (LPARAM) &m_cTree;
							m_cTree.SortChildrenCB(&pMy_Sort);
						}
						else
							m_cTree.SortChildren(m_cTree.GetParentItem(hDropTemp));

						if (!COptions::GetOptionVal(OPTION_SITEMANAGERNOEXPANDFOLDERS))
						{
							m_cTree.Expand(m_cTree.GetParentItem(hDropTemp), TVE_EXPAND);
						}
						continue;
					}

					HTREEITEM hDragTmpParent=m_cTree.GetParentItem(hDragTemp);
					hDropTemp=m_cTree.GetParentItem(hDropTemp);
					if (m_DefaultSite==hDragTemp)
						m_DefaultSite=m_cTree.GetRootItem();
					m_cTree.DeleteItem(hDragTemp);
					hDragTemp=hDragTmpParent;
				}
			}
		}
		else
			MessageBeep(MB_ICONEXCLAMATION);

		m_pDragImage->DeleteImageList();
		delete m_pDragImage;
		m_pDragImage = NULL;
		m_hDragItem = 0;
		m_hDropItem = 0;

		m_cTree.RedrawWindow(0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
	}

	CDialog::OnLButtonUp(nFlags, point);
}

CSiteManager::~CSiteManager()
{
	delete m_pDragImage;
	m_pDragImage = NULL;
	m_hDragItem = 0;
}

void CSiteManager::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent==m_nDragScrollTimerID)
	{
		if (!m_hDragItem)
		{
			KillTimer(m_nDragScrollTimerID);
			return;
		}
		RECT rect;
		m_cTree.GetWindowRect(&rect);
		if (m_CurPos.y<(rect.top+5))
		{
			HTREEITEM hItem=m_cTree.GetFirstVisibleItem();
			if (hItem)
				hItem=m_cTree.GetNextItem(hItem, TVGN_PREVIOUSVISIBLE);
			if (hItem)
			{
				m_pDragImage->DragShowNolock(FALSE);
				m_cTree.EnsureVisible(hItem);
				m_cTree.RedrawWindow();
				m_pDragImage->DragShowNolock(TRUE);
			}
		}
		else if (m_CurPos.y>(rect.bottom-5))
		{
			HTREEITEM hItem=m_cTree.GetFirstVisibleItem();
			while(hItem)
			{
				RECT iRect;
				m_cTree.GetItemRect(hItem, &iRect, TRUE);
				if (iRect.bottom>=(rect.bottom-rect.top-5))
				{
					m_pDragImage->DragShowNolock(FALSE);
					m_cTree.EnsureVisible(hItem);
					m_pDragImage->DragShowNolock(TRUE);
					break;
				}
				hItem=m_cTree.GetNextItem(hItem, TVGN_NEXTVISIBLE);
			}
		}
		else
		{
			if (m_nDragScrollTimerID)
				KillTimer(m_nDragScrollTimerID);
			m_nDragScrollTimerID=0;
		}
		HTREEITEM hItem=m_cTree.HitTest(m_CurPos);
		if (hItem!=m_hDropItem)
		{
			m_pDragImage->DragShowNolock(FALSE);
			m_cTree.SelectDropTarget(hItem);
			m_pDragImage->DragShowNolock(TRUE);
			m_hDropItem=hItem;
		}
	}
}

void CSiteManager::OnSelchangeServertype()
{
	UpdateData();

	int nServerType=m_cServerType.GetCurSel();
	if (nServerType==3 && m_nServerType!=3 && m_Port==_T("21"))
		m_Port=_T("22");
	else if (nServerType!=3 && m_nServerType==3 && m_Port==_T("22"))
		m_Port=_T("21");

	UpdateData(FALSE);

	m_nServerType = nServerType;
}

int CALLBACK CSiteManager::SortFoldersFirst(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	bool item1_is_folder;
	bool item2_is_folder;

	t_SiteManagerItem *item1 = (t_SiteManagerItem *)lParam1;
	t_SiteManagerItem *item2 = (t_SiteManagerItem *)lParam2;

	ASSERT(item1);
	item1_is_folder = (item1->NodeType == FOLDER);
	ASSERT(item2);
	item2_is_folder = (item2->NodeType == FOLDER);

	// If lParam1 and lParam2 are either both folders or both not folders, just sort alphabetically
	if((item1_is_folder && item2_is_folder) || (!item1_is_folder && !item2_is_folder))
	{
		return (item1->name).CompareNoCase(item2->name);
	}
	else // They aren't the same type.  Put the folder before the non-folder.
	{
		if( !item1_is_folder )
			return 1;
		else // item2_is_folder == 0
			return -1;
	}
}

void CSiteManager::OnSitemanagerAdvanced()
{
	HTREEITEM item = m_cTree.GetSelectedItem();
	if (!item)
		return;

	t_SiteManagerItem *site = (t_SiteManagerItem *)m_cTree.GetItemData(item);
	if (!site || site->NodeType != SITE)
		return;

	CSiteManagerAdvancedDlg dlg(site);

	dlg.DoModal();
}

t_SiteManagerItem::t_SiteManagerItem()
{
	nPort = 21;
	nPasv = 0;
	nTimeZoneOffset = 0;
	nTimeZoneOffset2 = 0;
	bPassDontSave = FALSE;
	bFirewallBypass = FALSE;
	nUTF8 = 0;
}

void CSiteManager::OnSitemanagerCopy()
{
	if (!CheckValid())
		return;
	HTREEITEM from = m_cTree.GetSelectedItem();
	t_SiteManagerItem *pData = (t_SiteManagerItem *)m_cTree.GetItemData(from);
	if (!pData || pData->NodeType != SITE)
		return;

	HTREEITEM parent = m_cTree.GetParentItem(from);
	if (!parent)
		parent = m_cTree.GetRootItem();
	ASSERT(m_cTree.GetItemData(parent));
	if (((t_SiteManagerItem *)m_cTree.GetItemData(parent))->NodeType==SITE)
		return;

	ASSERT(m_cTree.GetItemData(parent) && ((t_SiteManagerItem *)m_cTree.GetItemData(parent))->NodeType==FOLDER);

	CString name = m_cTree.GetItemText(from);

	HTREEITEM child=m_cTree.GetChildItem(parent);
	int i=2;
	while (child)
	{
		CString name2=name;
		name2.MakeLower();
		CString childstr=m_cTree.GetItemText(child);
		childstr.MakeLower();
		if (childstr==name2)
		{
			child=m_cTree.GetChildItem(parent);
			name.Format(m_cTree.GetItemText(from) + _T(" %d"), i);
			i++;
			continue;
		}
		child=m_cTree.GetNextItem(child,TVGN_NEXT);
	}

	HTREEITEM item=m_cTree.InsertItem(name, 2, 2, parent, TVI_SORT);
	m_cTree.Expand(parent, TVE_EXPAND);
	t_SiteManagerItem *site=new t_SiteManagerItem;
	*site = *pData;
	m_cTree.SetItemData(item,(DWORD)site);

	// Re-sort with folders first if that option is selected.  This must come AFTER the SetItemData command above.
	if (COptions::GetOptionVal(OPTION_SORTSITEMANAGERFOLDERSFIRST))
	{
		TVSORTCB pMy_Sort;
		pMy_Sort.hParent = parent;
		pMy_Sort.lpfnCompare = SortFoldersFirst;
		pMy_Sort.lParam = (LPARAM) &m_cTree;
		m_cTree.SortChildrenCB(&pMy_Sort);
	}

	m_cTree.SelectItem(item);
	DisplaySite(site);
	m_cTree.EditLabel(item);
}

void CSiteManager::OnSitemanagerRename()
{
	HTREEITEM item=m_cTree.GetSelectedItem();
	if (!item)
		return;
	if (!m_cTree.GetParentItem(item))
		return;
	m_cTree.EditLabel(item);
	return;
}

t_SiteManagerItem * CSiteManager::GetSiteByName(CString name)
{
	name.TrimLeft(_T("\\"));
	name.TrimRight(_T("\\"));
	while (name.Replace(_T("\\\\"), _T("\\")));

	if (name == "")
		return 0;

	t_SiteManagerItem *site = 0;

	CMarkupSTL *pMarkup;
	if (COptions::LockXML(&pMarkup))
	{
		pMarkup->ResetPos();
		if (pMarkup->FindChildElem( _T("Sites") ))
		{
			pMarkup->IntoElem();
			site = GetSiteByNameXML(pMarkup, name);
			COptions::UnlockXML();
			if (!site)
				return 0;
		}
		else
			COptions::UnlockXML();
	}
	if (!site)
	{
		CString path = name;

		HKEY key;
		HKEY hRootKey = HKEY_CURRENT_USER;
		if (RegOpenKey(HKEY_CURRENT_USER, "Software\\FileZilla\\Site Manager\\" + path, &key) != ERROR_SUCCESS)
		{
			if (RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\FileZilla\\Site Manager\\"+path, &key) != ERROR_SUCCESS)
				return 0;

			hRootKey = HKEY_LOCAL_MACHINE;
		}

		site = GetData(hRootKey, path);
		if (!site)
			return 0;
	}

	BOOL bUseGSS = FALSE;
	if (COptions::GetOptionVal(OPTION_USEGSS) && site->bPassDontSave)
	{
		USES_CONVERSION;
		CString GssServers=COptions::GetOption(OPTION_GSSSERVERS);
		LPCSTR lpszAsciiHost=T2CA(site->Host);
		hostent *fullname=gethostbyname(lpszAsciiHost);
		CString host;
		if (fullname)
			host=fullname->h_name;
		else
			host=site->Host;
		host.MakeLower();
		int i;
		while ((i=GssServers.Find( _T(";") ))!=-1)
		{
			if (("."+GssServers.Left(i))==host.Right(GssServers.Left(i).GetLength()+1) || GssServers.Left(i)==host)
			{
				bUseGSS=TRUE;
				break;
			}
			GssServers=GssServers.Mid(i+1);
		}
	}

	if (!bUseGSS)
	{
		if (site->nLogonType && (site->bPassDontSave || _ttoi(COptions::GetOption(OPTION_RUNINSECUREMODE))) )
		{
			CEnterSomething dlg(IDS_INPUTDIALOGTITLE_INPUTPASSWORD, IDS_INPUTDIALOGTEXT_INPUTPASSWORD, '*');
			if (dlg.DoModal() != IDOK)
				return 0;
			site->Pass = dlg.m_String;
		}
	}

	return site;
}

t_SiteManagerItem * CSiteManager::GetSiteByNameXML(CMarkupSTL *pMarkup, CString name)
{
	name += "\\";
	int pos = name.Find('\\');
	while (pos != -1)
	{
		CString cur = name.Left(pos);
		name = name.Mid(pos + 1);

		bool bFound = false;
		while (pMarkup->FindChildElem(_T("Site")))
		{
			if (pMarkup->GetChildAttrib(_T("Name")) == cur)
			{
				bFound = true;
				break;
			}
		}
		if (!bFound)
			return 0;

		if (name != "")
			pMarkup->IntoElem();

		pos = name.Find(_T("\\"));
	};

	return GetDataXML(pMarkup);
}
