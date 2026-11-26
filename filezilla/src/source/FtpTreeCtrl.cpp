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

#include "stdafx.h"
#include "filezilla.h"
#include "FtpTreeCtrl.h"
#include "mainfrm.h"
#include "commandqueue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// FtpTreeCtrl.cpp: Implementierungsdatei

/////////////////////////////////////////////////////////////////////////////
// CFtpTreeCtrl

CFtpTreeCtrl::CFtpTreeCtrl()
{
	m_bDontNotifySelchange = FALSE;
	m_pDirectory = 0;
	m_bChanged = FALSE;
	m_CollapsedItem = NULL;
	m_hDragHilited = NULL;
}

CFtpTreeCtrl::~CFtpTreeCtrl()
{
	m_SystemImageList.Detach();
	delete m_pDirectory;
	m_pDirectory=0;
}


BEGIN_MESSAGE_MAP(CFtpTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CFtpTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnSelchanging)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CFtpTreeCtrl 

void CFtpTreeCtrl::List(t_directory *pDirectory)
{
	DragLeave(NULL);

	m_bChanged = FALSE;
	m_bDontNotifySelchange = TRUE;
	
	if (!pDirectory)
	{
		delete m_pDirectory;
		m_pDirectory = 0;
		DeleteAllItems();
		m_bDontNotifySelchange = FALSE;
		return;
	}
	SetRedraw(FALSE);
	if (m_pDirectory != pDirectory)
	{
		delete m_pDirectory;
		m_pDirectory = new t_directory;
		*m_pDirectory = *pDirectory;
		if (pDirectory->server != m_Server)
		{
			m_Server = pDirectory->server;
			DeleteAllItems();
		}
	}

	if (!IsWindowEnabled())
	{
		m_bChanged = TRUE;
		SetRedraw(TRUE);
		return;
	}

	if (!GetRootItem() || GetItemText(GetRootItem())!=m_Server.host)
	{
		VERIFY(InsertItem(m_Server.host, m_nIconIndex, m_nIconIndex+1));
		if ( (m_Server.nServerType & FZ_SERVERTYPE_SUBMASK)==FZ_SERVERTYPE_SUB_FTP_VMS )
			SetItemState( GetRootItem(), INDEXTOOVERLAYMASK(2), TVIS_OVERLAYMASK );
		else
			SetItemState( GetRootItem(), INDEXTOOVERLAYMASK(1), TVIS_OVERLAYMASK );

		Expand(GetRootItem(), TVE_EXPAND);
	}

	CServerPath path = m_pDirectory->path;
	AddPath(path);
	SetRedraw(TRUE);
	m_bDontNotifySelchange = FALSE;
}

HTREEITEM CFtpTreeCtrl::AddPath(CServerPath path)
{
	std::list<CString> segmentList;
	while(path.HasParent())
	{
		segmentList.push_front(path.GetLastSegment());
		path=path.GetParent();
	}
	HTREEITEM parent = GetRootItem();
	for (std::list<CString>::iterator iter=segmentList.begin(); iter!=segmentList.end(); iter++)
	{
		CString Segment=*iter;
		HTREEITEM hItem=FindSegment(parent, Segment);
		if (!hItem)
		{
			hItem=InsertItem(Segment, m_nIconIndex, m_nIconIndex+1, parent);
			SetItemState( hItem, INDEXTOOVERLAYMASK(1), TVIS_OVERLAYMASK );
			SortChildren(parent);
		}
		Expand(parent, TVE_EXPAND);
		parent = hItem;
	}
	SetItemData(parent, 1);
	SetItemState( parent, INDEXTOOVERLAYMASK(0), TVIS_OVERLAYMASK );
	std::list<CString> SubdirList;
	for (int i=0;i<m_pDirectory->num;i++)
	{
		if (m_pDirectory->direntry[i].dir)
			SubdirList.push_back(m_pDirectory->direntry[i].name);
	}
	UpdateChildren(parent, SubdirList);
	if (parent != m_CollapsedItem)
		Expand(parent, TVE_EXPAND);	
	SelectItem(parent);
	return 0;
}

HTREEITEM CFtpTreeCtrl::FindSegment(HTREEITEM hParent, CString Segment)
{
	HTREEITEM hItem=GetChildItem(hParent);
	while (hItem)
	{
		if (GetItemText(hItem)==Segment)
			break;
		hItem=GetNextItem(hItem, TVGN_NEXT);
	}
	return hItem;
}

void CFtpTreeCtrl::UpdateChildren(HTREEITEM hParent, std::list<CString> &SubdirList)
{
	HTREEITEM hItem=GetChildItem(hParent);
	while (hItem)
	{
		CString ItemName=GetItemText(hItem);
		std::list<CString>::iterator iter;
		for (iter=SubdirList.begin(); iter!=SubdirList.end(); iter++)
			if (ItemName==*iter)
			{
				SubdirList.erase(iter);
				break;
			}
		if (iter==SubdirList.end())
		{
			HTREEITEM hNext=GetNextItem(hItem, TVGN_NEXT);
			DeleteItem(hItem);
			hItem=hNext;
		}
		else
			hItem=GetNextItem(hItem, TVGN_NEXT);
	}
	for (std::list<CString>::iterator iter=SubdirList.begin(); iter!=SubdirList.end(); iter++)
	{
		hItem=InsertItem(*iter, m_nIconIndex, m_nIconIndex+1, hParent);
		SetItemState(hItem, INDEXTOOVERLAYMASK(1), TVIS_OVERLAYMASK );
	}
	SubdirList.clear();
	SortChildren(hParent);
}

void CFtpTreeCtrl::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	if (!m_bDontNotifySelchange)
	{
		CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
		CServerPath path = GetPath(pNMTreeView->itemNew.hItem);
		if (path.IsEmpty())
		{
			MessageBeep(MB_ICONEXCLAMATION);
			*pResult=-1;
		}
		else if (!pMainFrame->m_pCommandQueue->List(path, FZ_LIST_USECACHE, FALSE))
		{
			MessageBeep(MB_ICONEXCLAMATION);
			*pResult=-1;
		}
	}
	
}

CServerPath CFtpTreeCtrl::GetPath(HTREEITEM hItem)
{
	if (!hItem)
		return CServerPath();

	CServerPath path(m_Server.nServerType);
	std::list<CString> SegmentList;
	while (hItem != GetRootItem())
	{
		SegmentList.push_front(GetItemText(hItem));
		hItem = GetParentItem(hItem);
	}
	if ( (m_Server.nServerType & FZ_SERVERTYPE_SUBMASK)!=FZ_SERVERTYPE_SUB_FTP_VMS )
		path.SetPath("/");
	for (std::list<CString>::iterator iter=SegmentList.begin(); iter!=SegmentList.end(); iter++)
		path.AddSubdir(*iter);
	return path;
}
	
int CFtpTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	GetSysImgList();
  
	m_nIconIndex=-1;
	SHFILEINFO shFinfo;
	if (SHGetFileInfo(_T("{5261C4A9-92B3-4667-A128-3E87564EA628}"),
		FILE_ATTRIBUTE_DIRECTORY,
		&shFinfo,
		sizeof( shFinfo ),
		SHGFI_ICON 
		| SHGFI_SMALLICON |SHGFI_USEFILEATTRIBUTES))
	{
		//shFinfo.hIcon;
//		m_nIconIndex=shFinfo.iIcon;
	}
	m_ImgList2.Create(&m_SystemImageList);
	m_SystemImageList.Detach();
	m_ImgList2.SetImageCount(0);
	CImageList img;
	img.Attach(ImageList_LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_FTPTREE_STATE), 16, 2, RGB(255, 0, 255)));
	
	m_nIconIndex=m_ImgList2.Add(shFinfo.hIcon);

	shFinfo;
	if (SHGetFileInfo(_T("{5261C4A9-92B3-4667-A128-3E87564EA628}"),
		FILE_ATTRIBUTE_DIRECTORY,
		&shFinfo,
		sizeof( shFinfo ),
		SHGFI_ICON | SHGFI_OPENICON
		| SHGFI_SMALLICON |SHGFI_USEFILEATTRIBUTES))
	{
	}
	m_ImgList2.Add(shFinfo.hIcon);

	int res=m_ImgList2.Add(img.ExtractIcon(0));
	res=m_ImgList2.Add(img.ExtractIcon(1));

	res=m_ImgList2.SetOverlayImage(2,1);
	res=m_ImgList2.SetOverlayImage(3,2);

	SetImageList(&m_ImgList2, TVSIL_NORMAL);
	return 0;
}

/////////////////////////////////////////////////
BOOL CFtpTreeCtrl::GetSysImgList()
/////////////////////////////////////////////////
{
	SHFILEINFO shFinfo;
	
	HIMAGELIST hImageList = 0;
	CString errorMessage;
	TCHAR filename[MAX_PATH + 10];

	if (GetModuleFileName(0, filename, MAX_PATH + 10))
	{
		hImageList = (HIMAGELIST)SHGetFileInfo(filename,
							 0,
							 &shFinfo,
							 sizeof( shFinfo ),
							 SHGFI_SYSICONINDEX |
							 SHGFI_SMALLICON);

		if (!hImageList)
		{
			int errorCode = GetLastError();
			TCHAR buffer[1000];
			memset(buffer, 0, 1000);
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, errorCode, 0, buffer, 999, 0);

			CString str;
			str.Format(_T("SHGetFileInfo failed with error %d: %s"), errorCode, buffer);
			errorMessage += str;
		}
	}
	else
	{
		int errorCode = GetLastError();
		TCHAR buffer[1000];
		memset(buffer, 0, 1000);
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, errorCode, 0, buffer, 999, 0);
		
		CString str;
		str.Format(_T("GetModuleFileName failed with error %d: %s"), errorCode, buffer);
		errorMessage += str;
	}
	if (!hImageList)
	{
		/*
		 * Fall back to C:\\
		 * Not bullerproof, but better than nothing
		 */		
		hImageList = (HIMAGELIST)SHGetFileInfo(_T("C:\\"),
							 0,
							 &shFinfo,
							 sizeof( shFinfo ),
							 SHGFI_SYSICONINDEX |
							 SHGFI_SMALLICON);

		if (!hImageList)
		{
			int errorCode = GetLastError();
			TCHAR buffer[1000];
			memset(buffer, 0, 1000);
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, errorCode, 0, buffer, 999, 0);

			CString str;
			str.Format(_T("SHGetFileInfo failed with error %d: %s"), errorCode, buffer);
			if (errorMessage != _T(""))
				errorMessage += _T("\n");
			errorMessage += str;
		}
	}

	if (!hImageList)
	{
		AfxMessageBox(errorMessage);
		return FALSE;
	}

	m_SystemImageList.Attach(hImageList);
	
	return TRUE;
}

void CFtpTreeCtrl::EnableTree(BOOL bEnable = TRUE )
{
	EnableWindow(bEnable);
	if (bEnable && m_bChanged)
		List(m_pDirectory);
}

void CFtpTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if (pNMTreeView->action == TVE_COLLAPSE)
	{
		m_CollapsedItem = pNMTreeView->itemNew.hItem;
	}
	else
	{
		m_CollapsedItem = 0;
	}
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	
	*pResult = 0;
}

void CFtpTreeCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *)pNMHDR;

	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsBusy())
		return;
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if (!hItem)
		return;

	CServerPath path = GetPath(hItem);
	if (!path.HasParent())
		return;

	EnsureVisible(hItem);

	m_hDragSource = hItem;
	
	//Let the main window handle the rest
	pMainFrame->OnBegindrag(this, pNMTreeView->ptDrag);
}

BOOL CFtpTreeCtrl::DragPosition(CImageList *pImageList, CWnd* pDragWnd, CPoint point)
{
	if (!m_pDirectory)
		return FALSE;

	ScreenToClient(&point);
	HTREEITEM hItem = HitTest(point);

	if (pDragWnd == this && hItem)
	{
		CServerPath dragPath = GetPath(m_hDragSource);
		CServerPath path = GetPath(hItem);
		
		if (path.IsSubdirOf(dragPath))
		{
			DragLeave(pImageList);
			return FALSE;
		}
	}
	if (hItem)
	{
		if (hItem != m_hDragHilited)
		{
			pImageList->DragShowNolock(false);
			if (m_hDragHilited)
			{
				SetItemState(m_hDragHilited, 0, LVIS_DROPHILITED);
			}
			m_hDragHilited = hItem;
			SetItemState(hItem, TVIS_DROPHILITED, TVIS_DROPHILITED);
			UpdateWindow();
			pImageList->DragShowNolock(true);
		}
	}
	else
		DragLeave(pImageList);

	return TRUE;
}

void CFtpTreeCtrl::DragLeave(CImageList *pImageList)
{
	if (!m_hDragHilited)
		return;

	if (pImageList)
		pImageList->DragShowNolock(false);
	SetItemState(m_hDragHilited, 0, TVIS_DROPHILITED);
	UpdateWindow();
	if (pImageList)
		pImageList->DragShowNolock(true);
	m_hDragHilited = NULL;
}

CServerPath CFtpTreeCtrl::GetDropTarget()
{
	if (!m_hDragHilited)
		return CServerPath();
	
	CServerPath path = GetPath(m_hDragHilited);
	if (!path.IsEmpty())
		SetItemState(m_hDragHilited, INDEXTOOVERLAYMASK(1), TVIS_OVERLAYMASK);
	return path;
}

void CFtpTreeCtrl::OnDragEnd(int target, CPoint point)
{
	if (!m_pDirectory)
		return;

	if (!m_hDragSource)
		return;

	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;
		
	if (pMainFrame->m_pCommandQueue->IsBusy())
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}

	if (!target)
	{
		ScreenToClient(&point);
		HTREEITEM hItem = HitTest(point);
		if (!hItem)
			return;

		CServerPath dragPath = GetPath(m_hDragSource);
		CServerPath path = GetPath(hItem);
		
		if (path.IsSubdirOf(dragPath))
			return;

		if (m_hDragSource == hItem)
			return;

		if (!dragPath.HasParent())
			return;

		if (dragPath.GetParent() == path)
			return;

		CString name = dragPath.GetLastSegment();
		dragPath = dragPath.GetParent();

		SetItemState(hItem, INDEXTOOVERLAYMASK(1), TVIS_OVERLAYMASK);
		SetItemState(m_hDragSource, INDEXTOOVERLAYMASK(1), TVIS_OVERLAYMASK);
		pMainFrame->m_pCommandQueue->Rename(name, name, dragPath, path);
	}
}