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

// FtpListCtrl.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "FileZilla.h"
#include "FtpListCtrl.h"
#include "mainfrm.h"
#include "queueview.h"
#include "entersomething.h"
#include "FileAttributes.h"
#include "TransferAsDlg.h"
#include "commandqueue.h"
#include "FtpView.h"
#include "LocalView2.h"
#include "PathFunctions.h"
#include "FtpTreeView.h"
#include "FtpTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CFtpListCtrl

static CString dateformat;
BOOL CALLBACK EnumDateFormatsProc(LPTSTR lpDateFormatString)
{
	dateformat=lpDateFormatString;
	return FALSE;
}

CFtpListCtrl::CFtpListCtrl(CFtpView *pOwner)
{
	ASSERT(pOwner);
	m_pOwner = pOwner;

	m_nStyle=0;
	m_pDirectory=0;
	m_nHideColumns=0;
	for (int i=0; i<7; i++)
		m_Columns[i] = i;

	dateformat = _T("");
	EnumDateFormats(EnumDateFormatsProc, LOCALE_USER_DEFAULT, DATE_SHORTDATE);

	m_bUpdating = FALSE;

	m_nBatchAction = 0;

	m_nDragHilited = -1;
}

CFtpListCtrl::~CFtpListCtrl()
{
	if (m_pDirectory)
	{
		delete m_pDirectory;
		m_pDirectory=0;
	}
}


BEGIN_MESSAGE_MAP(CFtpListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CFtpListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_DROPFILES()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_COMMAND(ID_FTPCONTEXT_ADDTOQUEUE, OnFtpcontextAddtoqueue)
	ON_COMMAND(ID_FTPCONTEXT_ATTRIBUTES, OnFtpcontextAttributes)
	ON_COMMAND(ID_FTPCONTEXT_CREATEDIR, OnFtpcontextCreatedir)
	ON_COMMAND(ID_FTPCONTEXT_DELETE, OnFtpcontextDelete)
	ON_COMMAND(ID_FTPCONTEXT_DOWNLOAD, OnFtpcontextDownload)
	ON_COMMAND(ID_FTPCONTEXT_DOWNLOADAS, OnFtpcontextDownloadas)
	ON_COMMAND(ID_FTPCONTEXT_OPEN, OnFtpcontextOpen)
	ON_COMMAND(ID_FTPCONTEXT_RENAME, OnFtpcontextRename)
	ON_COMMAND(ID_FTPCONTEXT_VIEWEDIT, OnFtpcontextViewEdit)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CFtpListCtrl 

extern CFileZillaApp theApp;

/////////////////////////////////////////////////
BOOL CFtpListCtrl::GetSysImgList()
/////////////////////////////////////////////////
{
	USES_CONVERSION;

	CImageList sysImgList;
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
							 ((m_nStyle == LVS_ICON)?SHGFI_ICON:SHGFI_SMALLICON) );

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
							 ((m_nStyle == LVS_ICON)?SHGFI_ICON:SHGFI_SMALLICON) );

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

	sysImgList.Attach(hImageList);
	
	SetImageList( &sysImgList, (m_nStyle == LVS_ICON)?LVSIL_NORMAL:LVSIL_SMALL);
	sysImgList.Detach();
	
	/*
		Do a version check first because you only need to use this code on
		Windows NT version 4.0 or newer (2000/XP)
	*/ 
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof(vi);
	GetVersionEx(&vi);
	if (VER_PLATFORM_WIN32_WINDOWS == vi.dwPlatformId)	
		return TRUE;
	
	/*
	You need to create a temporary, empty .lnk file that you can use to
	pass to IShellIconOverlay::GetOverlayIndex. You could just enumerate
	down from the Start Menu folder to find an existing .lnk file, but
	there is a very remote chance that you will not find one. By creating
	your own, you know this code will always work.
	*/ 
	HRESULT			  hr;
	IShellFolder		*psfDesktop = NULL;
	IShellFolder		*psfTempDir = NULL;
	IMalloc			  *pMalloc = NULL;
	LPITEMIDLIST		pidlTempDir = NULL;
	LPITEMIDLIST		pidlTempFile = NULL;
	TCHAR				 szTempDir[MAX_PATH];
	TCHAR				 szTempFile[MAX_PATH] = TEXT("");
	TCHAR				 szFile[MAX_PATH];
	HANDLE				hFile;
	int					i;
	OLECHAR			  szOle[MAX_PATH];
	DWORD				 dwAttributes;
	DWORD				 dwEaten;
	IShellIconOverlay *psio = NULL;
	int					nIndex;

	// Get the desktop folder.
	hr = SHGetDesktopFolder(&psfDesktop);
	if(FAILED(hr))
		goto exit;

	// Get the shell's allocator.
	hr = SHGetMalloc(&pMalloc);
	if(FAILED(hr))
		goto exit;

	// Get the TEMP directory.
	if(!GetTempPath(MAX_PATH, szTempDir))
		{
		/*
		There might not be a TEMP directory. If this is the case, use the
		Windows directory.
		*/ 
		if(!GetWindowsDirectory(szTempDir, MAX_PATH))
			{
			hr = E_FAIL;
			goto exit;
			}
		}

	// Create a temporary .lnk file.
	if(szTempDir[lstrlen(szTempDir) - 1] != '\\')
		lstrcat(szTempDir, TEXT("\\"));
	for(i = 0, hFile = INVALID_HANDLE_VALUE;
		INVALID_HANDLE_VALUE == hFile;
		i++)
		{
		lstrcpy(szTempFile, szTempDir);
		wsprintf(szFile, TEXT("temp%d.lnk"), i);
		lstrcat(szTempFile, szFile);

		hFile = CreateFile(  szTempFile,
									GENERIC_WRITE,
									0,
									NULL,
									CREATE_NEW,
									FILE_ATTRIBUTE_NORMAL,
									NULL);

		// Do not try this more than 100 times.
		if(i > 100)
			{
			hr = E_FAIL;
			goto exit;
			}
		}

	// Close the file you just created.
	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;

	// Get the PIDL for the directory.
	//LocalToWideChar(szOle, szTempDir, MAX_PATH);
	wcscpy(szOle, T2OLE(szTempDir));
	hr = psfDesktop->ParseDisplayName(  NULL,
													NULL,
													szOle,
													&dwEaten,
													&pidlTempDir,
													&dwAttributes);
	if(FAILED(hr))
		goto exit;

	// Get the IShellFolder for the TEMP directory.
	hr = psfDesktop->BindToObject(	pidlTempDir,
												NULL,
												IID_IShellFolder,
												(LPVOID*)&psfTempDir);
	if(FAILED(hr))
		goto exit;

	/*
	Get the IShellIconOverlay interface for this folder. If this fails,
	it could indicate that you are running on a pre-Internet Explorer 4.0
	shell, which doesn't support this interface. If this is the case, the
	overlay icons are already in the system image list.
	*/ 
	hr = psfTempDir->QueryInterface(IID_IShellIconOverlay, (LPVOID*)&psio);
	if(FAILED(hr))
		goto exit;

	// Get the PIDL for the temporary .lnk file.
	//LocalToWideChar(szOle, szFile, MAX_PATH);
	wcscpy(szOle, T2OLE(szFile));

	hr = psfTempDir->ParseDisplayName(  NULL,
													NULL,
													szOle,
													&dwEaten,
													&pidlTempFile,
													&dwAttributes);
	if(FAILED(hr))
		goto exit;

	/*
	Get the overlay icon for the .lnk file. This causes the shell
	to put all of the standard overlay icons into your copy of the system
	image list.
	*/ 
	hr = psio->GetOverlayIndex(pidlTempFile, &nIndex);

	exit:
	// Delete the temporary file.
	DeleteFile(szTempFile);

	if(psio)
		psio->Release();

	if(INVALID_HANDLE_VALUE != hFile)
		CloseHandle(hFile);

	if(psfTempDir)
		psfTempDir->Release();

	if(pMalloc)
	{
		if(pidlTempFile)
			pMalloc->Free(pidlTempFile);

		if(pidlTempDir)
			pMalloc->Free(pidlTempDir);

		pMalloc->Release();
	}

	if(psfDesktop)
		psfDesktop->Release();

	return TRUE;
}

int CFtpListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	int widths[7]={150, 70, 80, 68, 54, 90, 90};
	if (COptions::GetOptionVal(OPTION_REMEMBERREMOTECOLUMNWIDTHS))
	{
		CString tmp=COptions::GetOption(OPTION_REMOTECOLUMNWIDTHS);
		int pos=-1;
		int i;
		for (i=0; i<6; i++)
		{
			int oldpos = pos + 1;
			pos = tmp.Find(_T(" "), oldpos);
			if (pos == -1)
				break;
			tmp.SetAt(pos, 0);
			int size = _ttoi(tmp.Mid(oldpos));
			if (size > 0)
				widths[i] = size;
		}
		if (i == 6)
		{
			int size = _ttoi(tmp.Mid(pos+1));
			if (size > 0)
				widths[i] = size;
		}
	}
	CString str;
	str.LoadString(IDS_HEADER_FILENAME);
	InsertColumn(0, str, LVCFMT_LEFT, widths[0]);
	str.LoadString(IDS_HEADER_FILESIZE);
	InsertColumn(1, str, LVCFMT_RIGHT, widths[1]);
	str.LoadString(IDS_HEADER_FILETYPE);
	InsertColumn(2, str, LVCFMT_LEFT, widths[2]);
	str.LoadString(IDS_HEADER_DATE);
	InsertColumn(3, str, LVCFMT_LEFT, widths[3]);
	str.LoadString(IDS_HEADER_TIME);
	InsertColumn(4, str, LVCFMT_LEFT, widths[4]);
	str.LoadString(IDS_HEADER_PERMISSIONS);
	InsertColumn(5, str, LVCFMT_LEFT, widths[5]);
	str.LoadString(IDS_HEADER_OWNERGROUP);
	InsertColumn(6, str, LVCFMT_LEFT, widths[6]);
	SetListStyle(0);
	
	m_SortImg.Create( 8, 8, ILC_MASK, 3, 3 );
	HICON Icon;
	Icon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_EMPTY));
	m_SortImg.Add(Icon);
	Icon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_UP));
	m_SortImg.Add(Icon);
	Icon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_DOWN));
	m_SortImg.Add(Icon);
	m_SortImg.SetBkColor(CLR_NONE);
	
	CHeaderCtrl *header=GetHeaderCtrl( );
	if (header)
		header->SetImageList(&m_SortImg);

	int nSort = COptions::GetOptionVal(OPTION_REMOTECOLUMNSORT);
	m_sortdir = (nSort >> 4) % 3;
	if (!m_sortdir)
		m_sortdir = 1;
	m_sortcolumn = (nSort >> 1) & 0x07;
	if (m_sortcolumn > 6)
		m_sortcolumn = 0;

	DragAcceptFiles(TRUE);
	m_nFolderDownloadStart = FALSE;
	
	SetExtendedStyle(LVS_EX_INFOTIP);
	SetCallbackMask(GetCallbackMask() | LVIS_OVERLAYMASK); 
	return 0;
}

void CFtpListCtrl::List(t_directory *list)
{
	//Don't update the statusbar until finished
	m_bUpdating = TRUE;

	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	
	std::list<CString> SelectedList;
	BOOL bDotsSelected=FALSE;
	CServerPath OldPath;
	if (m_pDirectory)
	{
		//Previous directory visible
		OldPath=m_pDirectory->path;
		BOOL bStoreSelectedFiles=0;

		//If the new and the old directory have the same path, remember the selected items
		if (list && list->path==m_pDirectory->path)
			bStoreSelectedFiles=TRUE;
		UINT nNoOfItems = GetItemCount( );
		for( UINT nListItem = 0; nListItem < nNoOfItems; nListItem++ )
		{
			//Unselect all selected items
			if( GetItemState( nListItem, LVIS_SELECTED )  )
			{
				SetItemState( nListItem, 0, LVIS_SELECTED);
				if (bStoreSelectedFiles && nListItem) //and remember them if nessesary
					SelectedList.push_back(m_pDirectory->direntry[m_IndexMapping[nListItem]].lName);
				else if (bStoreSelectedFiles && !nListItem)
					bDotsSelected=TRUE;
			}
		}
		delete m_pDirectory;
	}

	//Set new directory
	m_pDirectory=list;
	m_IndexMapping.clear();
	m_IconCache.clear();
	if (!list)
	{
		m_PendingDirs.clear();
		pMainFrame->m_pCommandQueue->SetLock(FALSE);
		SetItemCount(0);
		m_nBatchAction = 0;
		m_PathsToVisit.clear();
		m_PathsVisited.clear();
		m_bUpdating = FALSE;
		m_FilesToDelete.clear();
		m_FilesToDeletePaths.clear();
		UpdateStatusBar();
		return;
	}

	m_IndexMapping.push_back(m_pDirectory->num);
	m_IndexMapping.resize(list->num+1);
	for (int i=0;i<list->num;i++)
		m_IndexMapping[i+1]=i;

	SetItemCount(m_IndexMapping.size());
	
	SortList(m_sortcolumn,m_sortdir);

	//Restore item selection
	for (std::list<CString>::iterator iter=SelectedList.begin(); iter!=SelectedList.end(); iter++)
	{
		for( int nListItem = 1; nListItem < GetItemCount(); nListItem++ )
		{
			if (*iter==m_pDirectory->direntry[m_IndexMapping[nListItem]].lName)
			{
				SetItemState( nListItem, LVIS_SELECTED, LVIS_SELECTED);
				break;
			}
		}
	}
	if (bDotsSelected)
		SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED);

	m_bUpdating = FALSE;
	UpdateStatusBar();

	//Check if complex operation in progress
	if (m_nBatchAction && m_pDirectory->path!=OldPath)
	{
		//Check if current path is valid
		if (!m_pDirectory->path.IsSubdirOf(m_PathsVisited.front()))
		{ //Path is not a subdir of the startdir
			DoNextStep();
			return;
		}
		for (std::list<CServerPath>::iterator iter=m_PathsVisited.begin(); iter!=m_PathsVisited.end(); iter++)
			if (m_pDirectory->path==*iter)
			{ //Path already processed
				DoNextStep();
				return;
			}
		m_PathsVisited.push_back(m_pDirectory->path);

		CString subdir;
		CServerPath path=m_pDirectory->path;
		while(path.IsSubdirOf(m_PathsVisited.front()))
		{
			subdir=_T("\\")+path.GetLastSegment()+subdir;
			path=path.GetParent();
		}
		subdir.TrimLeft( _T("\\") );
		
		if (!m_pDirectory->num &&
			m_nBatchAction == 1)
		{
			//Create empty local directories
			CString pathToCreate = pMainFrame->GetLocalPane2()->GetLocalFolder()+_T("\\")+subdir;
			while(pathToCreate.Replace( _T("\\\\"), _T("\\") ));
			
			pathToCreate.TrimRight(_T("\\"));
			pathToCreate += _T("\\");
			CString path2;
			while (pathToCreate!="")
			{
				path2+=pathToCreate.Left(pathToCreate.Find( _T("\\") )+1);
				pathToCreate = pathToCreate.Mid(pathToCreate.Find( _T("\\") )+1);
				int res = CreateDirectory(path2, 0);
			}
		}

		for (int i=0; i<m_pDirectory->num; i++)
		{ //Process all dir entries
			if (m_pDirectory->direntry[i].dir)
			{ //Change directory

				//Don't change to parent and own dir
				if ((m_pDirectory->direntry[i].name==".")||(m_pDirectory->direntry[i].name==".."))
					continue;

				if (m_nBatchAction == 2 && m_pDirectory->direntry[i].bLink)
				{
					m_FilesToDelete.push_back(m_pDirectory->direntry[i].name);
					m_FilesToDeletePaths.push_back(m_pDirectory->path);
				}
				else
				{
					t_folder folder;
					folder.path = m_pDirectory->path;
					folder.subdir = m_pDirectory->direntry[i].name;
					m_PathsToVisit.push_front(folder);
				}
			}
			else
			{
				//Download/Delete file
				if (m_nBatchAction == 1)
					pMainFrame->AddQueueItem(TRUE, m_pDirectory->direntry[i].name, subdir, _T(""), m_pDirectory->path, m_nFolderDownloadStart, m_transferuser, m_transferpass);
				else if (m_nBatchAction == 2)
				{
					m_FilesToDelete.push_back(m_pDirectory->direntry[i].name);
					m_FilesToDeletePaths.push_back(m_pDirectory->path);
				}
				else
					ASSERT(FALSE);
			}
		}
		DoNextStep();
	}
}

void CFtpListCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (pResult)
		*pResult = 0;
	
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;
	POSITION selpos = GetFirstSelectedItemPosition();
	if (selpos)
	{
		int nItem = GetNextSelectedItem(selpos);
		int index=m_IndexMapping[nItem];
		if (index==m_pDirectory->num)
			ChangeDir(m_pDirectory->path, "..", FALSE, FZ_LIST_USECACHE|FZ_LIST_EXACT);		
		else if (m_pDirectory->direntry[index].dir)
		{
			CString name=m_pDirectory->direntry[index].name;
			ChangeDir(m_pDirectory->path, name, FALSE, FZ_LIST_USECACHE|FZ_LIST_EXACT);		
		}
		else
		{
			int nAction = COptions::GetOptionVal(OPTION_REMOTE_DOUBLECLICK_ACTION);
			if (nAction == 1)
			{
				pMainFrame->AddQueueItem(TRUE, m_pDirectory->direntry[index].name, _T(""), _T(""), m_pDirectory->path, TRUE, _T(""), _T(""), 2);	
				pMainFrame->TransferQueue(2);
			}
			else
			{
				pMainFrame->AddQueueItem(TRUE, m_pDirectory->direntry[index].name, "", "", m_pDirectory->path, !nAction);
				if (!nAction)
					pMainFrame->TransferQueue(2);
			}
		}
	}
}

void CFtpListCtrl::SortList(int item, int direction /*=0*/)
{	
	if (!m_pDirectory)
		return;
	if (item>3)
		return;
	UINT nID[7] = {IDS_HEADER_FILENAME, IDS_HEADER_FILESIZE, IDS_HEADER_FILETYPE, IDS_HEADER_DATE, IDS_HEADER_TIME, IDS_HEADER_PERMISSIONS, IDS_HEADER_OWNERGROUP};

	if (!direction)
	{
		if (item!=m_sortcolumn)
			m_sortdir=1;
		else
		{
			m_sortdir=(++m_sortdir%3);
			if (!m_sortdir)
				m_sortdir++;
		}
			
	}
	else if (direction != -1)
		m_sortdir = direction;

	if (item == -1)
		item = m_sortcolumn;

	CString headertext;
	headertext.LoadString(nID[m_Columns[m_sortcolumn]]);
	CHeaderCtrl *header=GetHeaderCtrl();
	HDITEM *hdi=new HDITEM;
	hdi->pszText=headertext.GetBuffer(0);
	hdi->cchTextMax=0;
	hdi->mask= HDI_TEXT | HDI_FORMAT;
	switch (m_Columns[m_sortcolumn])
	{
	case 1:
		hdi->fmt = HDF_RIGHT | HDF_STRING;
		break;
	default:
		hdi->fmt = HDF_LEFT | HDF_STRING;
	}
	hdi->mask= HDI_TEXT | HDI_IMAGE | HDI_FORMAT;
	hdi->iImage=0; // My ascending image list index
	header->SetItem( m_sortcolumn, hdi );

	headertext.ReleaseBuffer();
	headertext.LoadString(nID[m_Columns[item]]);
	
	hdi->pszText=headertext.GetBuffer(0);
	hdi->mask= HDI_TEXT | HDI_IMAGE | HDI_FORMAT;
	hdi->iImage= m_sortdir; // My ascending image list index
	hdi->fmt=((m_Columns[item]!=1)?HDF_LEFT:HDF_RIGHT) | HDF_IMAGE | HDF_STRING | HDF_BITMAP_ON_RIGHT;
	header->SetItem( item, hdi );
	delete hdi;
	headertext.ReleaseBuffer();
	m_sortcolumn=item;
	if (GetItemCount()<2)
		return;

	std::list<int> SelectedItemsList;
	
	int i;
	for (i=1; i<GetItemCount(); i++)
	{
		if (GetItemState( i, LVIS_SELECTED))
		{
			SelectedItemsList.push_back(m_IndexMapping[i]);
			SetItemState( i, 0, LVIS_SELECTED);
		}
	}


	if (!m_Columns[m_sortcolumn])
		QSortByName(1, GetItemCount()-1);
	else if (m_Columns[m_sortcolumn]==1)
		QSortBySize(1, GetItemCount()-1);
	else if (m_Columns[m_sortcolumn]==2)
	{	//Sort by filetype
		//Since this is a column that is filled while displaying,
		//we have to load the filetypes for every file

		std::vector<CString> typeArray;
		typeArray.resize(GetItemCount());
		typeArray[0] = _T("");
		for (int i = 1; i<GetItemCount(); i++)
		{
			typeArray[m_IndexMapping[i]] = GetType(m_pDirectory->direntry[m_IndexMapping[i]].lName, m_pDirectory->direntry[m_IndexMapping[i]].dir);
			typeArray[m_IndexMapping[i]].MakeLower();
		}
		QSortByType(typeArray, 1, GetItemCount()-1);
	}
	else if (m_Columns[m_sortcolumn]==3 || m_Columns[m_sortcolumn]==4)
		QSortByTime(1, GetItemCount()-1);

	for (i=1; i<GetItemCount(); i++)
	{
		int nIndex = m_IndexMapping[i];
		if (SelectedItemsList.empty())
			break;
		for (std::list<int>::iterator iter=SelectedItemsList.begin(); iter!=SelectedItemsList.end(); iter++)
			if (*iter==nIndex)
			{
				SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
				SelectedItemsList.erase(iter);
				break;
			}
	}

	RedrawItems(1, GetItemCount()-1);

	return;
}

void CFtpListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	SortList(pNMListView->iSubItem);
	
	*pResult = 0;
}

BOOL CFtpListCtrl::GetTransferfile(t_transferfile &transferfile)
{
	if (!m_pDirectory)
		return FALSE;
	transferfile.size=0;
	transferfile.server=m_pDirectory->server;
	for (int i=0;i<m_pDirectory->num;i++)
	{
		if (m_pDirectory->direntry[i].name==transferfile.remotefile)
		{
			transferfile.size=m_pDirectory->direntry[i].size;
			return TRUE;
		}
		
	}
	return TRUE;
}

void CFtpListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	m_transferuser=m_transferpass="";
	if (!GetItemCount() || !m_pDirectory)
		return;
	CMenu menu;
	menu.LoadMenu(IDR_FTPCONTEXTMENU);

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;
	//while (pWndPopupOwner->GetStyle() & WS_CHILD)
	//	pWndPopupOwner = pWndPopupOwner->GetParent();

	POSITION selpos=GetFirstSelectedItemPosition();

	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (!selpos || pMainFrame->m_pCommandQueue->IsLocked())
	{
		pPopup->EnableMenuItem(ID_FTPCONTEXT_DOWNLOADAS, MF_GRAYED);	
		pPopup->EnableMenuItem(ID_FTPCONTEXT_DOWNLOAD, MF_GRAYED);	
		pPopup->EnableMenuItem(ID_FTPCONTEXT_ADDTOQUEUE, MF_GRAYED);
		pPopup->EnableMenuItem(ID_FTPCONTEXT_DELETE, MF_GRAYED);	
		pPopup->EnableMenuItem(ID_FTPCONTEXT_RENAME, MF_GRAYED);	
		pPopup->EnableMenuItem(ID_FTPCONTEXT_OPEN, MF_GRAYED);
		pPopup->EnableMenuItem(ID_FTPCONTEXT_ATTRIBUTES, MF_GRAYED);
		pPopup->EnableMenuItem(ID_FTPCONTEXT_VIEWEDIT, MF_GRAYED);
		if (point.x==-1 || point.y==-1)
		{
			point.x=5;
			point.y=5;
			ClientToScreen(&point);
		}
	}
	else
	{
		int nItem = GetNextSelectedItem(selpos);

		if (point.x==-1 || point.y==-1)
		{
			CRect rect;
			GetItemRect(nItem,&rect,LVIR_LABEL);
			point.x=rect.left+5;
			point.y=rect.top+5;
			ClientToScreen(&point);
		}

		int index=m_IndexMapping[nItem];
		if (index==m_pDirectory->num) //.. item
		{
			pPopup->EnableMenuItem(ID_FTPCONTEXT_DOWNLOADAS, MF_GRAYED);	
			pPopup->EnableMenuItem(ID_FTPCONTEXT_DOWNLOAD, MF_GRAYED);	
			pPopup->EnableMenuItem(ID_FTPCONTEXT_ADDTOQUEUE, MF_GRAYED);
			pPopup->EnableMenuItem(ID_FTPCONTEXT_DELETE, MF_GRAYED);	
			pPopup->EnableMenuItem(ID_FTPCONTEXT_RENAME, MF_GRAYED);	
			pPopup->EnableMenuItem(ID_FTPCONTEXT_ATTRIBUTES, MF_GRAYED);
			pPopup->EnableMenuItem(ID_FTPCONTEXT_VIEWEDIT, MF_GRAYED);
		}
		else
		{
			if (m_pDirectory->direntry[index].dir)
			{
				pPopup->SetDefaultItem(ID_FTPCONTEXT_OPEN);
				pPopup->EnableMenuItem(ID_FTPCONTEXT_VIEWEDIT, MF_GRAYED);
			}
			else
			{
				int nAction = COptions::GetOptionVal(OPTION_REMOTE_DOUBLECLICK_ACTION);
				if (nAction == 1)
					pPopup->SetDefaultItem(ID_FTPCONTEXT_VIEWEDIT);
				else if (nAction == 2)
					pPopup->SetDefaultItem(ID_FTPCONTEXT_ADDTOQUEUE);
				else
					pPopup->SetDefaultItem(ID_FTPCONTEXT_DOWNLOAD);
				pPopup->EnableMenuItem(ID_FTPCONTEXT_OPEN, MF_GRAYED);
			}
		}

		nItem = GetNextSelectedItem(selpos);
		while (nItem!=-1)
		{
			int nAction = COptions::GetOptionVal(OPTION_REMOTE_DOUBLECLICK_ACTION);
			if (nAction == 2)
				pPopup->SetDefaultItem(ID_FTPCONTEXT_ADDTOQUEUE);
			else
				pPopup->SetDefaultItem(ID_FTPCONTEXT_DOWNLOAD);

			int index=m_IndexMapping[nItem];
			if (index==m_pDirectory->num ||
				m_pDirectory->direntry[index].dir) //.. item
				pPopup->EnableMenuItem(ID_FTPCONTEXT_VIEWEDIT, MF_GRAYED);
		
			pPopup->EnableMenuItem(ID_FTPCONTEXT_OPEN, MF_GRAYED);
			pPopup->EnableMenuItem(ID_FTPCONTEXT_RENAME, MF_GRAYED);
			nItem = GetNextSelectedItem(selpos);
		}
	}
		
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
}

void CFtpListCtrl::OnFtpcontextOpen() 
{
	if (!GetItemCount() || !m_pDirectory)
		return;

	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;
	if (m_nBatchAction || !m_PathsToVisit.empty() || !m_PathsVisited.empty())
		return;

	POSITION selpos = GetFirstSelectedItemPosition();
	if (!selpos)
		return;
	
	int nItem = GetNextSelectedItem(selpos);
	int index = m_IndexMapping[nItem];

	if (index == m_pDirectory->num)
		ChangeDir(m_pDirectory->path,"..",FALSE,FZ_LIST_USECACHE|FZ_LIST_EXACT);		
	else if (m_pDirectory->direntry[index].dir)
		ChangeDir(m_pDirectory->path,m_pDirectory->direntry[index].name,FALSE,FZ_LIST_USECACHE|FZ_LIST_EXACT);		
	else
	{
	/*	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
		pMainFrame->AddQueueItem(TRUE, m_pDirectory->direntry[index].name, _T(""), _T(""), m_pDirectory->path, TRUE, _T(""), _T(""), 1);
		pMainFrame->TransferQueue(2);*/
	}
}

void CFtpListCtrl::OnFtpcontextDownload() 
{
	if (!GetItemCount() || !m_pDirectory)
		return;

	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;
	if (m_nBatchAction || !m_PathsToVisit.empty() || !m_PathsVisited.empty())
		return;

	POSITION selpos=GetFirstSelectedItemPosition();
	if (pMainFrame->m_pCommandQueue->IsBusy())
	{
		while (selpos)
		{
			int nItem = GetNextSelectedItem(selpos);
			int index=m_IndexMapping[nItem];
			ASSERT(index!=m_pDirectory->num);
			if (m_pDirectory->direntry[index].dir)
			{
				MessageBeep(MB_ICONEXCLAMATION);
				return;
			}
		}
		selpos=GetFirstSelectedItemPosition();
	}
	while (selpos)
	{
		int nItem = GetNextSelectedItem(selpos);
		int index=m_IndexMapping[nItem];
		ASSERT(index!=m_pDirectory->num);
		if (m_pDirectory->direntry[index].dir)
		{
			t_folder folder;
			folder.path = m_pDirectory->path;
			folder.subdir = m_pDirectory->direntry[index].name;
			m_PathsToVisit.push_back(folder);
			pMainFrame->m_pCommandQueue->SetLock(TRUE);
		}
		else
			pMainFrame->AddQueueItem(TRUE,m_pDirectory->direntry[index].name,"","",m_pDirectory->path,TRUE,m_transferuser,m_transferpass);
	}
	if (!m_PathsToVisit.empty())
	{
		m_nFolderDownloadStart = TRUE;
		m_PathsVisited.push_front(m_pDirectory->path);
		m_nBatchAction = 1;
		DoNextStep();
	}
	else
		pMainFrame->TransferQueue(2);
}

void CFtpListCtrl::OnFtpcontextAddtoqueue() 
{
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;
	if (m_nBatchAction || !m_PathsToVisit.empty() || !m_PathsVisited.empty())
		return;

	POSITION selpos=GetFirstSelectedItemPosition();
	if (pMainFrame->m_pCommandQueue->IsBusy())
	{
		while (selpos)
		{
			int nItem = GetNextSelectedItem(selpos);
			int index=m_IndexMapping[nItem];
			ASSERT(index!=m_pDirectory->num);
			if (m_pDirectory->direntry[index].dir)
			{
				MessageBeep(MB_ICONEXCLAMATION);
				return;
			}
		}
		selpos = GetFirstSelectedItemPosition();
	}
	while (selpos)
	{
		int nItem = GetNextSelectedItem(selpos);
		int index=m_IndexMapping[nItem];
		ASSERT(index!=m_pDirectory->num);
		if (m_pDirectory->direntry[index].dir)
		{
			t_folder folder;
			folder.path = m_pDirectory->path;
			folder.subdir = m_pDirectory->direntry[index].name;
			m_PathsToVisit.push_back(folder);
			pMainFrame->m_pCommandQueue->SetLock(TRUE);
		}
		else
			pMainFrame->AddQueueItem(TRUE, m_pDirectory->direntry[index].name, "", "", m_pDirectory->path, FALSE, m_transferuser, m_transferpass);
	}
	if (!m_PathsToVisit.empty())
	{
		m_nFolderDownloadStart = FALSE;
		m_PathsVisited.push_front(m_pDirectory->path);
		m_nBatchAction = 1;
		DoNextStep();
	}
}

void CFtpListCtrl::OnFtpcontextDelete() 
{
	BOOL bDontAsk = FALSE;

	/* After displaying the confirmation dialog, jump back to the start of the function.
	 * This is required since a new/changed directory listing could have been displayed 
	 * while the dialog was open.
	 */
  ondelete_recheck:

	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;
	if (!m_pDirectory || m_nBatchAction || 
		!m_PathsToVisit.empty() || !m_PathsVisited.empty())
		return;

	POSITION selpos = GetFirstSelectedItemPosition();
	if (!selpos)
		return;
	
	BOOL dir = FALSE;
	BOOL file = FALSE;
	int count = 0;

	while (selpos)
	{
		int nItem = GetNextSelectedItem(selpos);
		int index = m_IndexMapping[nItem];
		if (index == m_pDirectory->num)
			continue;
		count++;
		if (m_pDirectory->direntry[index].dir)
			dir = TRUE;
		else
			file = TRUE;
	}

	if (count == 1)
	{
		if (pMainFrame->m_pCommandQueue->IsBusy())
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return;
		}
		POSITION selpos = GetFirstSelectedItemPosition();
		ASSERT(selpos);
		int nItem = GetNextSelectedItem(selpos);
		int index = m_IndexMapping[nItem];
		if (index == m_pDirectory->num)
		{
			ASSERT(selpos);
			nItem = GetNextSelectedItem(selpos);
			index = m_IndexMapping[nItem];
		}
		if (index < 0 || index >= m_pDirectory->num)
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return;
		}

		if (!bDontAsk)
		{
			CString str;
			str.Format(IDS_QUESTION_DELETEFILE, m_pDirectory->direntry[index].name);
			int res = AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION);
			if (res == IDYES)
			{
				bDontAsk = TRUE;

				// Jump back to the start of the function
				goto ondelete_recheck;
			}
		}
		else
		{
			if (m_pDirectory->direntry[index].dir && !m_pDirectory->direntry[index].bLink)
			{
				t_folder folder;
				folder.path = m_pDirectory->path;
				folder.subdir = m_pDirectory->direntry[index].name;
				m_PathsToVisit.push_back(folder);
				pMainFrame->m_pCommandQueue->SetLock(TRUE);
			}
			else
				pMainFrame->m_pCommandQueue->Delete(m_pDirectory->direntry[index].name, m_pDirectory->path);
		}		
	}
	else if (count>1)
	{
		if (pMainFrame->m_pCommandQueue->IsBusy())
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return;
		}

		if (!bDontAsk)
		{
			int res = IDNO;
			CString str;
			if (dir&&(!file))
			{
				str.Format(IDS_QUESTION_DELETEDIRS, count);
				res = AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION);
			}
			else if (dir&&file)
			{
				str.Format(IDS_QUESTION_DELETEFILESANDDIRS, count);
				res = AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION);
			}
			else
			{
				str.Format(IDS_QUESTION_DELETEFILES, count);
				res = AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION);
			}
			if (res == IDYES)
			{
				bDontAsk = TRUE;
				
				// Jump back to the start of the function
				goto ondelete_recheck;
			}
		}
		else
		{
			selpos = GetFirstSelectedItemPosition();
			ASSERT(selpos);
			while (selpos)
			{
				count++;
				int nItem = GetNextSelectedItem(selpos);
				int index = m_IndexMapping[nItem];
				if (index==m_pDirectory->num)
					continue;
				if (m_pDirectory->direntry[index].dir && !m_pDirectory->direntry[index].bLink)
				{
					t_folder folder;
					folder.path = m_pDirectory->path;
					folder.subdir = m_pDirectory->direntry[index].name;
					m_PathsToVisit.push_back(folder);
					pMainFrame->m_pCommandQueue->SetLock(TRUE);
				}
				else
					pMainFrame->m_pCommandQueue->Delete(m_pDirectory->direntry[index].name,m_pDirectory->path);
			}
		}
	}
	if (!m_PathsToVisit.empty())
	{
		m_PathsVisited.push_front(m_pDirectory->path);
		m_nBatchAction = 2;
		DoNextStep();
	}
}

void CFtpListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar==46)
		OnFtpcontextDelete();
	else if (nChar==VK_F2)
	{
		CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
		if (pMainFrame->m_pCommandQueue->IsLocked())
			return;

		// To Use F2 as hot Key to get EditCtrl on the ListView it must have 
		// the Style LVS_EDITLABELS
		ASSERT( GetStyle() & LVS_EDITLABELS );
		// don't do an Edit Label when the multiple Items are selected
		if( GetSelectedCount( ) == 1 )
		{
			UINT nListSelectedItem = GetSelectedItem();
			VERIFY( EditLabel( nListSelectedItem ) != NULL );
		}
		else
			CListCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
	}
		
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFtpListCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsBusy())
	{
		POSITION selpos = GetFirstSelectedItemPosition();
		while (selpos)
		{
			int nItem = GetNextSelectedItem(selpos);
			int index = m_IndexMapping[nItem];
			ASSERT(index!=m_pDirectory->num);
			if (m_pDirectory->direntry[index].dir)
				return;
		}
	}
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;

	POSITION selpos = GetFirstSelectedItemPosition();
	while (selpos)
	{
		int nItem = GetNextSelectedItem(selpos);
		if (!nItem)
			return;
	}
	EnsureVisible(((LPNMLISTVIEW)pNMHDR)->iItem, FALSE);
	
	//Let the main window handle the rest
	pMainFrame->OnBegindrag(this, ((LPNMLISTVIEW)pNMHDR)->ptAction);
}

void CFtpListCtrl::OnDragEnd(int target, CPoint point)
{
	DragLeave(NULL);

	m_transferuser = m_transferpass = "";
	if (target == 1)
		OnFtpcontextAddtoqueue();
	else if (!target)
		OnFtpcontextDownload();
	else if (target == 2 || target == 3)
	{
		if (!m_pDirectory)
			return;
		
		CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
		if (pMainFrame->m_pCommandQueue->IsLocked())
			return;
		if (m_nBatchAction || !m_PathsToVisit.empty() || !m_PathsVisited.empty())
			return;
		
		if (pMainFrame->m_pCommandQueue->IsBusy())
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return;
		}

		CServerPath path = m_pDirectory->path;
		CServerPath newPath = path;

		if (target == 2)
		{
			ScreenToClient(&point);
			int nHitItem = HitTest(point);
			if (nHitItem == -1)
				return;
			
			if (nHitItem > 0 && nHitItem < (int)m_IndexMapping.size() && !m_pDirectory->direntry[m_IndexMapping[nHitItem]].dir)
				return;
			
			POSITION selpos = GetFirstSelectedItemPosition();
			while (selpos)
			{
				int nItem = GetNextSelectedItem(selpos);
				if (nItem == nHitItem)
					return;
			}

			if (!nHitItem)
			{
				if (!path.HasParent())
					return;
				newPath = path.GetParent();
			}
			else
				newPath.AddSubdir(m_pDirectory->direntry[m_IndexMapping[nHitItem]].name);
		}
		else
		{
			newPath = reinterpret_cast<CFtpTreeCtrl *>(pMainFrame->GetFtpTreePane()->GetTreeCtrl())->GetDropTarget();
			if (newPath.IsEmpty())
				return;
		}

		POSITION selpos = GetFirstSelectedItemPosition();
		while (selpos)
		{
			int nItem = GetNextSelectedItem(selpos);
			if (nItem < 1 || nItem >= (int)m_IndexMapping.size())
				continue;
			int nIndex = m_IndexMapping[nItem];
			if (nIndex < 0 || nIndex >= m_pDirectory->num)
				continue;
			pMainFrame->m_pCommandQueue->Rename(m_pDirectory->direntry[nIndex].name, m_pDirectory->direntry[nIndex].name, path, newPath);
		}
	}
}

void CFtpListCtrl::OnDropFiles(HDROP hDropInfo) 
{
	//End of a file drag&drop operation
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (!GetItemCount())
	{
		DragFinish(hDropInfo);
		return;
	}
	int dropcount=DragQueryFile(hDropInfo,0xFFFFFFFF,0,0);
	for (int i=0;i<dropcount;i++)
	{
		int len=DragQueryFile(hDropInfo,i,0,0)+1;
		LPTSTR name=new TCHAR[len];
		DragQueryFile(hDropInfo,i,name,len);
		CFileStatus64 status;
		GetStatus64(name, status);
		CString name2=name;
		if (status.m_attribute&0x10)
		{
			CString filename=name;
			if (filename.ReverseFind('\\')!=-1)
				filename=filename.Right(filename.GetLength()-filename.ReverseFind('\\')-1);
			pMainFrame->UploadDir(name2 + _T("\\*.*"), filename + _T("\\"), TRUE);
		}
		else
		{
			ASSERT(name2.ReverseFind('\\')!=-1);
			CString filename=name2.Mid(name2.ReverseFind('\\')+1);
			CString path=name2.Left(name2.ReverseFind('\\'));
			pMainFrame->AddQueueItem(FALSE,filename,"",path,m_pDirectory->path,TRUE);
		}
		delete [] name;
	}
	pMainFrame->TransferQueue(2);
	DragFinish(hDropInfo);

}

void CFtpListCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult=TRUE;
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;
	if (pMainFrame->m_pCommandQueue->IsBusy())
		return;
	if (!pDispInfo->item.iItem)
		return;
	*pResult = 0;
	CEdit *pEdit=GetEditControl();
	if (pEdit)
		pEdit->LimitText( 255 );
	
}

void CFtpListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if (pDispInfo->item.pszText)
	{
		CString newname = pDispInfo->item.pszText;
		newname = newname.Left(255);
		if ((newname.Find('/')!=-1)||(newname.Find('\\')!=-1)||
			(newname.Find(':')!=-1)||(newname.Find('*')!=-1)||
			(newname.Find('?')!=-1)||(newname.Find('"')!=-1)||
			(newname.Find('<')!=-1)||(newname.Find('>')!=-1)||
			(newname.Find('|')!=-1))
		{
			AfxMessageBox(IDS_ERRORMSG_FILENAMEINVALID, MB_ICONEXCLAMATION);
			*pResult = FALSE;
			return;
		}
		CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
		pMainFrame->m_pCommandQueue->Rename(m_pDirectory->direntry[m_IndexMapping[pDispInfo->item.iItem]].name, newname, m_pDirectory->path);
	}
	*pResult = FALSE;
}

#define VK_A		65
BOOL CFtpListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// If edit control is visible in tree view control, sending a
	// WM_KEYDOWN message to the edit control will dismiss the edit
	// control.  When ENTER key was sent to the edit control, the parent
	// window of the tree view control is responsible for updating the
	// item's label in TVN_ENDLABELEDIT notification code.
	if ( pMsg->message == WM_KEYDOWN )
	{
		CEdit* edit = GetEditControl();
		if (edit)
		{
			if( GetKeyState( VK_CONTROL )&128 && pMsg->wParam == VK_A )
			{
				edit->SetSel(0, -1);
				return TRUE;
			}
			if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CONTROL || pMsg->wParam == VK_INSERT || pMsg->wParam == VK_SHIFT )
			{
				edit->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
		}
		else
		{
			if( GetKeyState( VK_CONTROL )&128 && pMsg->wParam == VK_A )
			{
				m_bUpdating = TRUE;
				if (GetItemCount())
					SetItemState(0,GetItemCount()==1?LVIS_SELECTED:0,LVIS_SELECTED);
				for (int i=1;i<GetItemCount();i++)
					SetItemState(i,LVIS_SELECTED,LVIS_SELECTED);
				m_bUpdating = FALSE;

				UpdateStatusBar();
				return TRUE;
			}
			else if (pMsg->wParam==VK_RETURN)
			{
				CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
				if (pMainFrame->m_pCommandQueue->IsLocked())
					return TRUE;
				if (m_nBatchAction || !m_PathsToVisit.empty() || !m_PathsVisited.empty())
					return TRUE;

				POSITION selpos = GetFirstSelectedItemPosition();
				BOOL bOpen=TRUE;
				int openindex=-1;
				if (selpos)
				{
					int nItem = GetNextSelectedItem(selpos);
					int index=openindex=m_IndexMapping[nItem];
					if (index!=m_pDirectory->num && !m_pDirectory->direntry[index].dir)
					{
						bOpen=FALSE;
						pMainFrame->AddQueueItem(TRUE,m_pDirectory->direntry[index].name,"","",m_pDirectory->path,TRUE);
						openindex=-1;
					}
				}
				else
					return TRUE;
				while (selpos)
				{
					bOpen=FALSE;
					int nItem = GetNextSelectedItem(selpos);
					int index=m_IndexMapping[nItem];
					if (index==m_pDirectory->num)
						continue;
					if (m_pDirectory->direntry[index].dir)
					{
						t_folder folder;
						folder.path = m_pDirectory->path;
						folder.subdir = m_pDirectory->direntry[index].name;
						m_PathsToVisit.push_back(folder);
						pMainFrame->m_pCommandQueue->SetLock(TRUE);
					}
					else
						pMainFrame->AddQueueItem(TRUE,m_pDirectory->direntry[index].name,"","",m_pDirectory->path,TRUE,m_transferuser,m_transferpass);
				}
				if (bOpen)
				{
					if (openindex==m_pDirectory->num)
						ChangeDir(m_pDirectory->path,"..",FALSE,FZ_LIST_USECACHE|FZ_LIST_EXACT);
					else if (m_pDirectory->direntry[openindex].dir)
						ChangeDir(m_pDirectory->path,m_pDirectory->direntry[openindex].name,FALSE,FZ_LIST_USECACHE|FZ_LIST_EXACT);
				}
				else
				{
					if (openindex!=-1 && m_pDirectory->direntry[openindex].dir)
					{
						t_folder folder;
						folder.path = m_pDirectory->path;
						folder.subdir = m_pDirectory->direntry[openindex].name;
						m_PathsToVisit.push_back(folder);
						pMainFrame->m_pCommandQueue->SetLock(TRUE);
					}
					if (!m_PathsToVisit.empty())
					{
						m_nFolderDownloadStart = TRUE;
						m_PathsVisited.push_front(m_pDirectory->path);
						m_nBatchAction = 1;
						DoNextStep();
					}
					else
						pMainFrame->TransferQueue(2);
				}
			}
			else if (pMsg->wParam == VK_BACK)
			{
				if (m_pDirectory)
					ChangeDir(m_pDirectory->path, "..", FALSE, FZ_LIST_USECACHE|FZ_LIST_EXACT);
				else
					MessageBeep(MB_ICONEXCLAMATION);
			}
		}
	}	
	return CListCtrl::PreTranslateMessage(pMsg);
}

void CFtpListCtrl::OnFtpcontextRename() 
{
	if (!GetItemCount() || !m_pDirectory)
		return;
	
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;
	if (m_nBatchAction || !m_PathsToVisit.empty() || !m_PathsVisited.empty())
		return;

	ASSERT(GetSelectedCount()==1);
	if (pMainFrame->m_pCommandQueue->IsBusy())
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}
	POSITION selpos = GetFirstSelectedItemPosition();
	int index = GetNextSelectedItem(selpos);
	EditLabel(index);
}


// this Function Returns the first Selected Item In the List
UINT CFtpListCtrl::GetSelectedItem()
{
	// this Function Valid Only when a Single Item Is Selected
	ASSERT( GetSelectedCount( ) == 1 );
	UINT nNoOfItems = GetItemCount( );
	UINT nListItem;
	for (nListItem = 0; nListItem < nNoOfItems; nListItem++ )
		if( GetItemState( nListItem, LVIS_SELECTED )  )
			break;
	ASSERT( nListItem < nNoOfItems );
	return nListItem;
}

void CFtpListCtrl::OnFtpcontextCreatedir() 
{
	if (!GetItemCount() || !m_pDirectory)
		return;

	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;
	if (m_nBatchAction || !m_PathsToVisit.empty() || !m_PathsVisited.empty())
		return;

	CEnterSomething dlg(IDS_INPUTDIALOGTITLE_CREATEDIR, IDS_INPUTDIALOGTEXT_CREATEDIR);
	if (pMainFrame->m_pCommandQueue->IsBusy())
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}
	CServerPath path = m_pDirectory->path;
	CServerPath tmpPath = path;
	tmpPath.AddSubdir( _T("\001\002\003\004\005") );
	CString tmp = tmpPath.GetPath();
	int pos = tmp.Find( _T("\001\002\003\004\005") );
	if (pos == -1)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}
	CString tmp2;
	tmp2.LoadString(IDS_SITEMANAGER_NEWFOLDER);
	tmp.Replace(_T("\001\002\003\004\005"), tmp2);
	dlg.m_String = tmp;
	dlg.m_nPreselectMin = pos;
	dlg.m_nPreselectMax = pos + tmp2.GetLength();
	if (dlg.DoModal() == IDOK)
	{
		if (!path.ChangePath(dlg.m_String))
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return;
		}
		else if (!path.HasParent())
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return;
		}
		CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
		if (!pMainFrame->m_pCommandQueue->MakeDir(path))
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return;
		}
	}
}

void CFtpListCtrl::ReloadHeader()
{
	ReloadHeaderItem(0,IDS_HEADER_FILENAME);
	int i=1;
	if (!(m_nHideColumns&1))
	{
		ReloadHeaderItem(i, IDS_HEADER_FILESIZE);
		i++;
	}
	if (!(m_nHideColumns&2))
	{
		ReloadHeaderItem(i, IDS_HEADER_FILETYPE);
		i++;
	}
	if (!(m_nHideColumns&4))
	{
		ReloadHeaderItem(i, IDS_HEADER_DATE);
		i++;
	}
	if (!(m_nHideColumns&8))
	{
		ReloadHeaderItem(i, IDS_HEADER_TIME);
		i++;
	}
	if (!(m_nHideColumns & 0x10))
	{
		ReloadHeaderItem(i, IDS_HEADER_PERMISSIONS);
		i++;
	}
	if (!(m_nHideColumns & 0x20))
	{
		ReloadHeaderItem(i, IDS_HEADER_OWNERGROUP);
	}
}

void CFtpListCtrl::ReloadHeaderItem(int nIndex, UINT nID)
{
	CHeaderCtrl *header=GetHeaderCtrl();
	TCHAR text[100];
	HDITEM item;
	memset(&item,0,sizeof(HDITEM));
	item.cchTextMax=100;
	item.mask=HDI_TEXT;
	item.pszText=text;
	header->GetItem(nIndex,&item);
	CString str;
	str.LoadString(nID);
	_tcscpy(text,str);
	header->SetItem(nIndex,&item);
}


void CFtpListCtrl::OnFtpcontextAttributes() 
{
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;
	if (pMainFrame->m_pCommandQueue->IsBusy())
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}
	POSITION selpos=GetFirstSelectedItemPosition();
	if (!selpos)
		return;
	int count=0;
	int permbuffer[9];
	memset(permbuffer,0,sizeof(int)*9);
	int index;
	while (selpos)
	{
		int nItem = GetNextSelectedItem(selpos);
		if (!nItem)
			continue;
		index=m_IndexMapping[nItem];
	
		CString permissions = m_pDirectory->direntry[index].permissionstr;
		permissions.MakeLower();

		if (permissions.GetLength() >= 10)
		{
			//Owner permissions
			if (permissions[1]=='r')
				permbuffer[0]++;
			if (permissions[2]=='w')
				permbuffer[1]++;
			if (permissions[3]=='x')
				permbuffer[2]++;
	
			//Group permissions
			if (permissions[4]=='r')
				permbuffer[3]++;
			if (permissions[5]=='w')
				permbuffer[4]++;
			if (permissions[6]=='x')
				permbuffer[5]++;

			//Public permissions
			if (permissions[7]=='r')
				permbuffer[6]++;
			if (permissions[8]=='w')
				permbuffer[7]++;
			if (permissions[9]=='x')
				permbuffer[8]++;
		}

		count++;
	}
	if (!count)
		return;

	CFileAttributes dlg;
	for (int i=0;i<9;i++)
	{
		if (permbuffer[i])
			if (permbuffer[i]!=count)
				permbuffer[i]=2;
			else
				permbuffer[i]=1;
	}
	memcpy(dlg.m_permbuffer, permbuffer, sizeof(int)*9);

	dlg.SetFileCount(count);
	
	if (count>1)
		dlg.m_text.LoadString(IDS_CHANGEDIALOG_MULTIPLEFILES);
	else
		dlg.m_text.Format(m_pDirectory->direntry[index].dir?IDS_CHANGEDIALOG_DIRECTORY:IDS_CHANGEDIALOG_FILE, m_pDirectory->direntry[index].name);
	if (dlg.DoModal()==IDOK)
	{
		selpos=GetFirstSelectedItemPosition();
		BOOL bChanged=FALSE;
		while(selpos)
		{
			int nItem=GetNextSelectedItem(selpos);
			if (!nItem)
				continue;
			int index=m_IndexMapping[nItem];
			CString permissions=m_pDirectory->direntry[index].permissionstr;
			permissions.MakeLower();
			int value=0;
			
			//Owner permissions
			if (dlg.m_permbuffer[0]==2)
			{
				if (permissions[1]=='r')
					value+=400;
			}
			else if (dlg.m_permbuffer[0]==1)
				value+=400;
			
			if (dlg.m_permbuffer[1]==2)
			{
				if (permissions[2]=='w')
					value+=200;
			}
			else if (dlg.m_permbuffer[1]==1)
				value+=200;

			if (dlg.m_permbuffer[2]==2)
			{
				if (permissions[3]=='x')
					value+=100;
			}
			else if (dlg.m_permbuffer[2]==1)
				value+=100;
			
			//Group permissions
			if (dlg.m_permbuffer[3]==2)
			{
				if (permissions[4]=='r')
					value+=40;
			}
			else if (dlg.m_permbuffer[3]==1)
				value+=40;
			
			if (dlg.m_permbuffer[4]==2)
			{
				if (permissions[5]=='w')
					value+=20;
			}
			else if (dlg.m_permbuffer[4]==1)
				value+=20;

			if (dlg.m_permbuffer[5]==2)
			{
				if (permissions[6]=='x')
					value+=10;
			}
			else if (dlg.m_permbuffer[5]==1)
				value+=10;
			
			//Public permissions
			if (dlg.m_permbuffer[6]==2)
			{
				if (permissions[7]=='r')
					value+=4;
			}
			else if (dlg.m_permbuffer[6]==1)
				value+=4;
			
			if (dlg.m_permbuffer[7]==2)
			{
				if (permissions[8]=='w')
					value+=2;
			}
			else if (dlg.m_permbuffer[7]==1)
				value+=2;

			if (dlg.m_permbuffer[8]==2)
			{
				if (permissions[9]=='x')
					value+=1;
			}
			else if (dlg.m_permbuffer[8]==1)
				value+=1;
			
			if (dlg.m_permbuffer[9])
				value += dlg.m_permbuffer[9] * 1000;

			if (!pMainFrame->m_pCommandQueue->Chmod(value, m_pDirectory->direntry[index].name, m_pDirectory->path, TRUE))
				MessageBeep(MB_ICONEXCLAMATION);
			else
				bChanged=TRUE;
		}
		if (bChanged)
		{
			if (m_pDirectory)
				pMainFrame->m_pCommandQueue->List(m_pDirectory->path, 0, TRUE);
			else
				pMainFrame->m_pCommandQueue->List(0, TRUE);
		}
	}
}	

void CFtpListCtrl::OnFtpcontextDownloadas() 
{
	if (!GetItemCount() || !m_pDirectory)
		return;

	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;
	if (m_nBatchAction || !m_PathsToVisit.empty() || !m_PathsVisited.empty())
		return;

	CTransferAsDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_transferuser = dlg.m_User;
		m_transferpass = dlg.m_Pass;
		if (dlg.m_bTransferNow)
			OnFtpcontextDownload();
		else
			OnFtpcontextAddtoqueue();
	}	
}

void CFtpListCtrl::SetListStyle(int nStyle)
{
	if (!nStyle)
		m_nStyle=LVS_REPORT;
	else if (nStyle==1)
		m_nStyle=LVS_LIST;
	else if (nStyle==2)
		m_nStyle=LVS_ICON;
	else if (nStyle==3)
		m_nStyle=LVS_SMALLICON;
	int remove=~m_nStyle & (LVS_REPORT|LVS_ICON|LVS_SMALLICON|LVS_LIST);
	ModifyStyle(remove, m_nStyle, SWP_NOZORDER);
	
	GetSysImgList();
	CHeaderCtrl *header = GetHeaderCtrl( );
	if (header)
		header->SetImageList(&m_SortImg);
	Arrange(LVA_DEFAULT);
	if (m_nStyle != LVS_REPORT)
		SortList(0,1);
}

void CFtpListCtrl::UpdateColumns(int nHideColumns)
{
	CString str;
	if (nHideColumns&1 && !(m_nHideColumns&1))
	{
		m_Columns[1] = m_Columns[2];
		m_Columns[2] = m_Columns[3];
		m_Columns[3] = m_Columns[4];
		m_Columns[4] = m_Columns[5];
		m_Columns[5] = m_Columns[6];
		m_Columns[6] = 0;
		
		DeleteColumn(1);
		if (m_sortcolumn == 1)
			SortList(0, 1);
		if (m_sortcolumn > 1)
			m_sortcolumn--;
	}
	else if (m_nHideColumns&1 && !(nHideColumns&1))
	{
		m_Columns[5] = m_Columns[4];
		m_Columns[4] = m_Columns[3];
		m_Columns[3] = m_Columns[2];
		m_Columns[2] = m_Columns[1];
		m_Columns[1] = 1;
		str.LoadString(IDS_HEADER_FILESIZE);
		InsertColumn(1, str, LVCFMT_RIGHT, 70);
		if (m_sortcolumn >= 1)
			m_sortcolumn++;
		RedrawItems(1, GetItemCount()-1);
	}
	if (nHideColumns&2 && !(m_nHideColumns&2))
	{
		for (int i=1; i<3; i++)
		{
			if (m_Columns[i] == 2)
			{
				int j = i;
				while (j < 6)
				{
					m_Columns[j] = m_Columns[j+1];
					j++;
				}
				m_Columns[j] = 5;
				DeleteColumn(i);		
				if (m_sortcolumn==i)
					SortList(0, 1);
				if (m_sortcolumn > i)
					m_sortcolumn--;
				break;
			}
		}
	}
	else if (m_nHideColumns&2 && !(nHideColumns&2))
	{
		for (int i=1; i<3; i++)
		{
			if (m_Columns[i] >= 3)
			{
				int j = 6;
				while (j != i)
				{
					m_Columns[j] = m_Columns[j-1];
					j--;
				}
				m_Columns[i] = 2;

				str.LoadString(IDS_HEADER_FILETYPE);
				InsertColumn(i, str, LVCFMT_LEFT, 80);
				if (m_sortcolumn >= i)
					m_sortcolumn++;
				RedrawItems(1, GetItemCount()-1);
				break;
			}
		}
	}
	if (nHideColumns&4 && !(m_nHideColumns&4))
	{
		for (int i=1; i<4; i++)
		{
			if (m_Columns[i] == 3)
			{
				int j = i;
				while (j < 6)
				{
					m_Columns[j] = m_Columns[j+1];
					j++;
				}
				m_Columns[j] = 6;
				DeleteColumn(i);		
				if (m_sortcolumn==i)
					SortList(0, 1);
				if (m_sortcolumn > i)
					m_sortcolumn--;
				break;
			}
		}
	}
	else if (m_nHideColumns&4 && !(nHideColumns&4))
	{
		for (int i=1; i<4; i++)
		{
			if (m_Columns[i] >= 4)
			{
				int j = 6;
				while (j != i)
				{
					m_Columns[j] = m_Columns[j-1];
					j--;
				}
				m_Columns[i] = 3;

				str.LoadString(IDS_HEADER_DATE);
				InsertColumn(i, str, LVCFMT_LEFT, 68);
				if (m_sortcolumn >= i)
					m_sortcolumn++;
				RedrawItems(1, GetItemCount()-1);
				break;
			}
		}
	}
	if (nHideColumns&8 && !(m_nHideColumns&8))
	{
		for (int i=1; i<5; i++)
		{
			if (m_Columns[i] == 4)
			{
				int j = i;
				while (j < 6)
				{
					m_Columns[j] = m_Columns[j+1];
					j++;
				}
				m_Columns[j] = 6;

				DeleteColumn(i);		
				if (m_sortcolumn == i)
					SortList(0, 1);
				if (m_sortcolumn > i)
					m_sortcolumn--;
				break;
			}
		}
	}
	else if (m_nHideColumns&8 && !(nHideColumns&8))
	{
		for (int i=1; i<5; i++)
		{
			if (m_Columns[i] >= 5)
			{
				int j = 6;
				while (j != i)
				{
					m_Columns[j] = m_Columns[j-1];
					j--;
				}
				m_Columns[i] = 4;

				str.LoadString(IDS_HEADER_TIME);
				InsertColumn(i, str, LVCFMT_LEFT, 54);
				if (m_sortcolumn >= i)
					m_sortcolumn++;
				RedrawItems(1, GetItemCount()-1);
				break;
			}
		}
	}
	if (nHideColumns&0x10 && !(m_nHideColumns&0x10))
	{
		for (int i=1; i<6; i++)
		{
			if (m_Columns[i] == 5)
			{
				int j = i;
				while (j < 6)
				{
					m_Columns[j] = m_Columns[j+1];
					j++;
				}
				m_Columns[j] = 6;

				DeleteColumn(i);		
				if (m_sortcolumn == i)
					SortList(0, 1);
				if (m_sortcolumn > i)
					m_sortcolumn--;
				break;
			}
		}
	}
	else if (m_nHideColumns&0x10 && !(nHideColumns&0x10))
	{
		for (int i=1; i<6; i++)
		{
			if (m_Columns[i] >= 6)
			{
				int j = 6;
				while (j != i)
				{
					m_Columns[j] = m_Columns[j-1];
					j--;
				}
				m_Columns[i] = 5;

				str.LoadString(IDS_HEADER_PERMISSIONS);
				InsertColumn(i, str, LVCFMT_LEFT, 90);
				if (m_sortcolumn >= i)
					m_sortcolumn++;
				RedrawItems(1, GetItemCount()-1);
				break;
			}
		}
	}
	if (nHideColumns&0x20 && !(m_nHideColumns&0x20))
	{
		for (int i=1; i<7; i++)
		{
			if (m_Columns[i] == 6)
			{
				DeleteColumn(i);		
				if (m_sortcolumn == i)
					SortList(0, 1);
				if (m_sortcolumn > i)
					m_sortcolumn--;
				break;
			}
		}
	}
	else if (m_nHideColumns&0x20 && !(nHideColumns&0x20))
	{
		for (int i=1; i<7; i++)
		{
			if (m_Columns[i] == 6)
			{
				str.LoadString(IDS_HEADER_OWNERGROUP);
				InsertColumn(i, str, LVCFMT_LEFT, 90);
				if (m_sortcolumn >= i)
					m_sortcolumn++;
				RedrawItems(1, GetItemCount()-1);
				break;
			}
		}
	}
	m_nHideColumns = nHideColumns;
}
void CFtpListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if (!pDispInfo)
		return;

	LV_ITEM* pItem= &(pDispInfo)->item;
	if (!pItem)
		return;

	if (pItem->iItem < 0 || pItem->iItem >= static_cast<int>(m_IndexMapping.size()))
		return;

	int index = m_IndexMapping[pItem->iItem];
	
	if (pItem->mask & LVIF_IMAGE && !m_Columns[pItem->iSubItem])
	{
		int iIcon;
		std::map<int, int>::iterator iter=m_IconCache.find(m_IndexMapping[pItem->iItem]);
		BOOL found=iter!=m_IconCache.end();
		if (found)
			iIcon = iter->second;
		if (!pItem->iItem)
		{
			if (!found)
			{
				SHFILEINFO shFinfo;
				if (!SHGetFileInfo(_T("{5261C4A9-92B3-4667-A128-3E87564EA628}"),
					FILE_ATTRIBUTE_DIRECTORY,
					&shFinfo,
					sizeof( shFinfo ),
					SHGFI_ICON |
					((m_nStyle != LVS_ICON)?SHGFI_SMALLICON:SHGFI_LARGEICON) |SHGFI_USEFILEATTRIBUTES))
					iIcon=-1;
				else
				{
					DestroyIcon(shFinfo.hIcon);
					iIcon = shFinfo.iIcon;
					m_IconCache[index] = iIcon;
				}
			}
		}
		else
		{
			if (!found)
			{
				
				SHFILEINFO shFinfo;
				memset(&shFinfo, 0, sizeof(shFinfo));
				CString name = m_pDirectory->direntry[index].dir ? "st" : m_pDirectory->direntry[index].name;
				name.Replace(':', '_');
		
				if (!SHGetFileInfo(name,
					m_pDirectory->direntry[index].dir?FILE_ATTRIBUTE_DIRECTORY:FILE_ATTRIBUTE_NORMAL,
					&shFinfo,
					sizeof(shFinfo),
					SHGFI_ICON | //(bLink?(SHGFI_LINKOVERLAY|SHGFI_OVERLAYINDEX):0) |
					((m_nStyle != LVS_ICON)?SHGFI_SMALLICON:SHGFI_LARGEICON) | (SHGFI_USEFILEATTRIBUTES) ) )
					iIcon = -1;
				else
				{
					// we only need the index from the system image list
					DestroyIcon(shFinfo.hIcon);
					iIcon = shFinfo.iIcon;
					m_IconCache[m_IndexMapping[pItem->iItem]] = iIcon;
				}
			}
		}

		pItem->iImage = iIcon;
	}
	if (pItem->mask & LVIF_STATE && !m_Columns[pItem->iSubItem] && pItem->iItem)
	{
		BOOL bLink = m_pDirectory->direntry[index].bLink;
		if (pItem->iItem && bLink)
			pItem->state = INDEXTOOVERLAYMASK(2);
	}
	if (pItem->mask & LVIF_TEXT)
	{
		if (!pItem->iItem)
		{
			if (!pItem->iSubItem)
				lstrcpy(pItem->pszText, _T("..") );
		}
		else
		{
			if (!m_Columns[pItem->iSubItem])
			{
				_tcsncpy(pItem->pszText, m_pDirectory->direntry[index].name, pItem->cchTextMax);
			}
			
			if (m_Columns[pItem->iSubItem]==1)
			{
				if (!m_pDirectory->direntry[index].dir)
				{
					_int64 size=m_pDirectory->direntry[index].size;
					if (size>-1)
					{
						int nFormat=COptions::GetOptionVal(OPTION_REMOTEFILESIZEFORMAT);
						if (!nFormat)
							if (size<1024)
								nFormat=1;
							else if (size<(1024*1024))
								nFormat=2;
							else
								nFormat=3;
						
						CString tmp, sizestr;
						switch (nFormat)
						{
						case 1:
							sizestr.Format(_T("%I64d"), size);
							break;
						case 2:
							tmp.LoadString(IDS_SIZE_KBS);
							sizestr.Format(_T("%I64d %s"), (size + 1023) / 1024, tmp);
							break;
						case 3:
							tmp.LoadString(IDS_SIZE_MBS);
							sizestr.Format(_T("%I64d %s"), (size + 1024 * 1024 - 1) / 1024 / 1024, tmp);
							break;
						default:
							ASSERT(FALSE);
						}
						_tcsncpy(pItem->pszText, sizestr, pItem->cchTextMax);
					}
					else 
						lstrcpy(pItem->pszText, _T("???"));
				}
			}
			else if (m_Columns[pItem->iSubItem] == 2)
			{
				CString type = GetType(m_pDirectory->direntry[index].lName, m_pDirectory->direntry[index].dir);
				_tcsncpy(pItem->pszText, type, pItem->cchTextMax);
			}
			else if (m_Columns[pItem->iSubItem]==3)
			{
				if (m_pDirectory->direntry[index].date.hasdate)
				{
					bool y=1;
					bool m=1;
					bool d=1;
					TCHAR date[100];
					memset(date,0,100*sizeof(TCHAR));
					for (int j=0;j<dateformat.GetLength();j++)
					{
						dateformat.MakeLower();
						if (dateformat[j]=='y')
						{
							if (y)
								_stprintf(date,_T("%s%d"),date,m_pDirectory->direntry[index].date.year);
							y=0;
						}
						else if (dateformat[j]=='m')
						{
							if (m)
								_stprintf(date,_T("%s%02d"),date,m_pDirectory->direntry[index].date.month);
							m=0;
						}
						else if (dateformat[j]=='d')
						{
							if (d)
								_stprintf(date,_T("%s%02d"),date,m_pDirectory->direntry[index].date.day);
							d=0;
						}
						else
						{
							date[_tcslen(date)+1]=0;
							date[_tcslen(date)]=dateformat[j];
						}
					}
					_tcsncpy(pItem->pszText, date, pItem->cchTextMax);
				}
				else
					lstrcpy(pItem->pszText, _T("???"));
			}
			else if (m_Columns[pItem->iSubItem] == 4)
			{
				if (m_pDirectory->direntry[index].date.hastime && m_pDirectory->direntry[index].date.hasdate)
				{
					CString str;
					str.Format(_T("%02d:%02d"), m_pDirectory->direntry[index].date.hour, m_pDirectory->direntry[index].date.minute);
					_tcsncpy(pItem->pszText, str, pItem->cchTextMax);
				}
			}
			else if (m_Columns[pItem->iSubItem] == 5)
			{
				_tcsncpy(pItem->pszText, m_pDirectory->direntry[index].permissionstr, pItem->cchTextMax);
			}
			else if (m_Columns[pItem->iSubItem] == 6)
			{
				_tcsncpy(pItem->pszText, m_pDirectory->direntry[index].ownergroup, pItem->cchTextMax);
			}
		}
		pItem->pszText[pItem->cchTextMax - 1] = 0;
	}
	
	*pResult = 0;
}

extern BOOL greater(const CString &str1, const CString &str2, BOOL isdir1, BOOL isdir2);
extern BOOL lesser(const CString &str1, const CString &str2, BOOL isdir1, BOOL isdir2);
extern BOOL lesserbysize(const __int64 &size1,const __int64 &size2, BOOL isdir1, BOOL isdir2, const CString &fn1, const CString &fn2);
extern BOOL greaterbysize(const __int64 &size1,const __int64 &size2, BOOL isdir1, BOOL isdir2, const CString &fn1, const CString &fn2);
extern BOOL lesserbytype(const CString &str1, const CString &str2, const BOOL isdir1, const BOOL isdir2, const CString &fn1, const CString &fn2);
extern BOOL greaterbytype(const CString &str1, const CString &str2, const BOOL isdir1, const BOOL isdir2, const CString &fn1, const CString &fn2);

void CFtpListCtrl::QSortByName(int begin, int end)
{
	int l=begin;
	int r=end;
	CString tmp;
	CString refname=m_pDirectory->direntry[m_IndexMapping[(l+r)/2]].lName;
	BOOL bRefIsDir=m_pDirectory->direntry[m_IndexMapping[(l+r)/2]].dir;
	do
    {
		if (m_sortdir&1)
		{
			while (lesser(m_pDirectory->direntry[m_IndexMapping[l]].lName,refname,m_pDirectory->direntry[m_IndexMapping[l]].dir,bRefIsDir) && l<end) l++;
			while (greater(m_pDirectory->direntry[m_IndexMapping[r]].lName,refname,m_pDirectory->direntry[m_IndexMapping[r]].dir,bRefIsDir) && r>begin) r--;
		}
		else
		{
			while (greater(m_pDirectory->direntry[m_IndexMapping[l]].lName,refname,m_pDirectory->direntry[m_IndexMapping[l]].dir,bRefIsDir) && l<end) l++;
			while (lesser(m_pDirectory->direntry[m_IndexMapping[r]].lName,refname,m_pDirectory->direntry[m_IndexMapping[r]].dir,bRefIsDir) && r>begin) r--;
		}
		if (l<=r)
		{
			int tmp=m_IndexMapping[l];
			m_IndexMapping[l]=m_IndexMapping[r];
			m_IndexMapping[r]=tmp;
			l++;
			r--;
		}
    } 
	while (l<=r);

  if (begin<r) QSortByName(begin,r);
  if (l<end) QSortByName(l,end);
}

void CFtpListCtrl::QSortBySize(int begin, int end)
{
	int l=begin;
	int r=end;
	CString tmp;
	CString refname=m_pDirectory->direntry[m_IndexMapping[(l+r)/2]].lName;
	BOOL bRefIsDir=m_pDirectory->direntry[m_IndexMapping[(l+r)/2]].dir;
	__int64 nRefSize=m_pDirectory->direntry[m_IndexMapping[(l+r)/2]].size;
	do
    {
		if (m_sortdir&1)
		{
			while (lesserbysize(m_pDirectory->direntry[m_IndexMapping[l]].size,nRefSize,m_pDirectory->direntry[m_IndexMapping[l]].dir,bRefIsDir,m_pDirectory->direntry[m_IndexMapping[l]].lName,refname) && l<end) l++;
			while (greaterbysize(m_pDirectory->direntry[m_IndexMapping[r]].size,nRefSize,m_pDirectory->direntry[m_IndexMapping[r]].dir,bRefIsDir,m_pDirectory->direntry[m_IndexMapping[r]].lName,refname) && r>begin) r--;
		}
		else
		{
			while (greaterbysize(m_pDirectory->direntry[m_IndexMapping[l]].size,nRefSize,m_pDirectory->direntry[m_IndexMapping[l]].dir,bRefIsDir,m_pDirectory->direntry[m_IndexMapping[l]].lName,refname) && l<end) l++;
			while (lesserbysize(m_pDirectory->direntry[m_IndexMapping[r]].size,nRefSize,m_pDirectory->direntry[m_IndexMapping[r]].dir,bRefIsDir,m_pDirectory->direntry[m_IndexMapping[r]].lName,refname) && r>begin) r--;
		}
		if (l<=r)
		{
			int tmp=m_IndexMapping[l];
			m_IndexMapping[l]=m_IndexMapping[r];
			m_IndexMapping[r]=tmp;
			l++;
			r--;
		}
    } 
	while (l<=r);

  if (begin<r) QSortBySize(begin,r);
  if (l<end) QSortBySize(l,end);
}

void CFtpListCtrl::QSortByType(const std::vector<CString> &typeArray, int begin, int end)
{
	int l = begin;
	int r = end;
	CString tmp;
	CString refname = m_pDirectory->direntry[m_IndexMapping[(l+r)/2]].lName;
	BOOL bRefIsDir = m_pDirectory->direntry[m_IndexMapping[(l+r)/2]].dir;
	CString refType = typeArray[m_IndexMapping[(l+r)/2]];
	do
    {
		if (m_sortdir&1)
		{
			while (lesserbytype(typeArray[m_IndexMapping[l]], refType,
								m_pDirectory->direntry[m_IndexMapping[l]].dir, bRefIsDir,
								m_pDirectory->direntry[m_IndexMapping[l]].lName, refname) 
				   && l<end)
				   l++;
			while (greaterbytype(typeArray[m_IndexMapping[r]], refType,
								 m_pDirectory->direntry[m_IndexMapping[r]].dir, bRefIsDir,
								 m_pDirectory->direntry[m_IndexMapping[r]].lName, refname)
				   && r>begin)
				   r--;
		}
		else
		{
			while (greaterbytype(typeArray[m_IndexMapping[l]], refType, m_pDirectory->direntry[m_IndexMapping[l]].dir,bRefIsDir,m_pDirectory->direntry[m_IndexMapping[l]].lName,refname) && l<end) l++;
			while (lesserbytype(typeArray[m_IndexMapping[r]], refType, m_pDirectory->direntry[m_IndexMapping[r]].dir,bRefIsDir,m_pDirectory->direntry[m_IndexMapping[r]].lName,refname) && r>begin) r--;
		}
		if (l<=r)
		{
			int tmp=m_IndexMapping[l];
			m_IndexMapping[l]=m_IndexMapping[r];
			m_IndexMapping[r]=tmp;
			l++;
			r--;
		}
    } 
	while (l<=r);

  if (begin<r) QSortByType(typeArray, begin,r);
  if (l<end) QSortByType(typeArray, l,end);
}

BOOL lesserbydate(const t_directory::t_direntry::t_date &date1,const t_directory::t_direntry::t_date &date2,BOOL isdir1,BOOL isdir2,CString fn1,CString fn2)
{
	if (isdir1 && !isdir2)
		return TRUE;
	if (!isdir1 && isdir2)
		return FALSE;
	if (!date1.hasdate && date2.hasdate)
		return TRUE;
	if (date1.hasdate && !date2.hasdate)
		return FALSE;
	if (!date1.hasdate && !date2.hasdate)
	{
		if (fn1<fn2)
			return TRUE;
		else
			return FALSE;
	}
	unsigned int time1=date1.year*31*12+date1.month*31+date1.day;
	unsigned int time2=date2.year*31*12+date2.month*31+date2.day;
	time1*=60*24;
	time2*=60*24;
	if (date1.hastime)
	{
		ASSERT(date1.hour<24 && date1.minute<60);
		time1+=date1.hour*60+date1.minute;
	}
	if (date2.hastime)
	{
		ASSERT(date2.hour<24 && date2.minute<60);
		time2+=date2.hour*60+date2.minute;
	}

	if (time1<time2)
		return TRUE;
	if (time1==time2)
	{
		if (fn1<fn2)
			return TRUE;
	}
	return FALSE;
}

BOOL greaterbydate(const t_directory::t_direntry::t_date &date1,const t_directory::t_direntry::t_date &date2,BOOL isdir1,BOOL isdir2,CString fn1,CString fn2)
{
	if (isdir1 && !isdir2)
		return FALSE;
	if (!isdir1 && isdir2)
		return TRUE;
	if (!date1.hasdate && date2.hasdate)
		return FALSE;
	if (date1.hasdate && !date2.hasdate)
		return TRUE;
	if (!date1.hasdate && !date2.hasdate)
	{
		if (fn1>fn2)
			return TRUE;
		else
			return FALSE;
	}

	unsigned int time1=date1.year*31*12+date1.month*31+date1.day;
	unsigned int time2=date2.year*31*12+date2.month*31+date2.day;
	time1*=60*24;
	time2*=60*24;
	if (date1.hastime)
		time1+=date1.hour*60+date1.minute;
	if (date2.hastime)
		time2+=date2.hour*60+date2.minute;
	if (time1>time2)
		return TRUE;
	if (time1==time2)
	{
		CString name1=fn1;
		CString name2=fn2;
		name1.MakeLower();
		name2.MakeLower();
		if (name1>name2)
			return TRUE;
	}
	return FALSE;
}


void CFtpListCtrl::QSortByTime(int begin, int end)
{
	int l=begin;
	int r=end;
	CString tmp;
	CString refname=m_pDirectory->direntry[m_IndexMapping[(l+r)/2]].lName;
	BOOL bRefIsDir=m_pDirectory->direntry[m_IndexMapping[(l+r)/2]].dir;
	t_directory::t_direntry::t_date nRefTime=m_pDirectory->direntry[m_IndexMapping[(l+r)/2]].date;
	do
    {
		if (m_sortdir&1)
		{
			while (lesserbydate(m_pDirectory->direntry[m_IndexMapping[l]].date,nRefTime,m_pDirectory->direntry[m_IndexMapping[l]].dir,bRefIsDir,m_pDirectory->direntry[m_IndexMapping[l]].lName,refname) && l<end) l++;
			while (greaterbydate(m_pDirectory->direntry[m_IndexMapping[r]].date,nRefTime,m_pDirectory->direntry[m_IndexMapping[r]].dir,bRefIsDir,m_pDirectory->direntry[m_IndexMapping[r]].lName,refname) && r>begin) r--;
		}
		else
		{
			while (greaterbydate(m_pDirectory->direntry[m_IndexMapping[l]].date,nRefTime,m_pDirectory->direntry[m_IndexMapping[l]].dir,bRefIsDir,m_pDirectory->direntry[m_IndexMapping[l]].lName,refname) && l<end) l++;
			while (lesserbydate(m_pDirectory->direntry[m_IndexMapping[r]].date,nRefTime,m_pDirectory->direntry[m_IndexMapping[r]].dir,bRefIsDir,m_pDirectory->direntry[m_IndexMapping[r]].lName,refname) && r>begin) r--;
		}
		if (l<=r)
		{
			int tmp=m_IndexMapping[l];
			m_IndexMapping[l]=m_IndexMapping[r];
			m_IndexMapping[r]=tmp;
			l++;
			r--;
		}
    } 
	while (l<=r);

  if (begin<r) QSortByTime(begin,r);
  if (l<end) QSortByTime(l,end);
}

void CFtpListCtrl::Refresh()
{
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;

	if (m_pDirectory)
	{
		if (!pMainFrame->m_pCommandQueue->List(m_pDirectory->path,0))
			MessageBeep(MB_ICONEXCLAMATION);
	}
	else
		if (!pMainFrame->m_pCommandQueue->List(0))
			MessageBeep(MB_ICONEXCLAMATION);
}

BOOL CFtpListCtrl::ChangeDir(const CServerPath &path, CString subdir, BOOL bAddToQueue /*=FALSE*/,int nListMode /*=FZ_LIST_USECACHE*/)
{
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (subdir!="")
	{
		if (!pMainFrame->m_pCommandQueue->List(path, subdir, nListMode, bAddToQueue))
			MessageBeep(MB_ICONEXCLAMATION);
		else
		{
			t_PendingDirs pendingDir;
			pendingDir.path = path;
			pendingDir.Subdir = subdir;
			m_PendingDirs.push_back(pendingDir);

			return TRUE;
		}
	}
	else
		if (!pMainFrame->m_pCommandQueue->List(path, nListMode, bAddToQueue))
			MessageBeep(MB_ICONEXCLAMATION);
		else
		{
			t_PendingDirs pendingDir;
			pendingDir.path = path;
			m_PendingDirs.push_back(pendingDir);

			return TRUE;
		}
	return FALSE;
	
}

BOOL CFtpListCtrl::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	if (message==WM_NOTIFY)
	{
		LPNMHDR  lpnmh = (LPNMHDR) lParam;
		
		if (lpnmh->code==LVN_ODFINDITEM)
		{
			NMLVFINDITEM* pFindItem = (NMLVFINDITEM*)lParam;
			LVFINDINFO pFindInfo=pFindItem->lvfi;
		
			CString part=pFindInfo.psz;
			part.MakeLower();
			int start=pFindItem->iStart;
			
			if (m_pDirectory)
				for (int i=start;i<(m_pDirectory->num+start);i++)
				{
					int index=m_IndexMapping[i%GetItemCount()];
					CString fn;
					if (index==m_pDirectory->num)
					{
						fn="..";
						fn=fn.Left(part.GetLength());
					}
					else
						fn=m_pDirectory->direntry[index].name.Left(part.GetLength());
					fn.MakeLower();
					if (fn==part)
					{
						*pLResult=i%GetItemCount();
						return TRUE;
					}
				}
			*pLResult=-1;
			return TRUE;
		}
	}
	return CListCtrl::OnChildNotify(message, wParam, lParam, pLResult);
}

CServerPath CFtpListCtrl::GetCurrentDirectory() const
{
	if (!m_pDirectory)
		return CServerPath();
	else
		return m_pDirectory->path;
}

BOOL CFtpListCtrl::HasUnsureEntries()
{
	if (!m_pDirectory)
		return FALSE;

	for (int i=0;i<m_pDirectory->num;i++)
		if (m_pDirectory->direntry[i].bUnsure || (m_pDirectory->direntry[i].size==-1 && !m_pDirectory->direntry[i].dir))
			return TRUE;

	return FALSE;
}

void CFtpListCtrl::OnPaint() 
{
	CListCtrl::OnPaint();

	//Show text when dirlist is empty
	if (m_pDirectory && !m_pDirectory->num)
	{
		CDC *pDC=GetDC(); // device context for painting

		//Load string to draw
		CString str;
		str.LoadString(IDS_REMOTELIST_EMPTY);

		//Select font
		CObject *pOld=pDC->SelectObject(GetFont());

		//Get rect of client area
		CRect rect;
		GetClientRect(rect);

		//Get rect of always existing ".." item
		CRect itemRect;
		GetSubItemRect(0,0,LVIR_BOUNDS,itemRect);

		//Draw text
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut(rect.Width()/2, //Horizontal center of client area
					itemRect.top+itemRect.Height()+2,
					str);
		
		//Cleanup
		pDC->SelectObject(pOld);
		ReleaseDC(pDC);
	}
}

void CFtpListCtrl::SaveColumnSizes()
{
	int nSize[7];
	nSize[0] = GetColumnWidth(0);
	int index = 1;
	nSize[1] = 70;
	nSize[2] = 80;
	nSize[3] = 68;
	nSize[4] = 54;
	nSize[5] = 90;
	nSize[6] = 90;

	if (!(m_nHideColumns&1))
	{
		nSize[1]=GetColumnWidth(index);
		index++;
	}
	
	if (!(m_nHideColumns&2))
	{
		nSize[2]=GetColumnWidth(index);
		index++;
	}
	
	if (!(m_nHideColumns&4))
	{
		nSize[3]=GetColumnWidth(index);
		index++;
	}

	if (!(m_nHideColumns&8))
	{
		nSize[4]=GetColumnWidth(index);
		index++;
	}

	if (!(m_nHideColumns&0x10))
	{
		nSize[5]=GetColumnWidth(index);
		index++;
	}

	if (!(m_nHideColumns&0x20))
	{
		nSize[6]=GetColumnWidth(index);
		index++;
	}

	CString str;
	str.Format(_T("%d %d %d %d %d %d %d"), nSize[0], nSize[1], nSize[2], nSize[3], nSize[4], nSize[5], nSize[6]);
	COptions::SetOption(OPTION_REMOTECOLUMNWIDTHS, str);
	
}

void CFtpListCtrl::OnFtpcontextViewEdit()
{
	if (!GetItemCount() || !m_pDirectory)
		return;

	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
	if (pMainFrame->m_pCommandQueue->IsLocked())
		return;
	if (m_nBatchAction || !m_PathsToVisit.empty() || !m_PathsVisited.empty())
		return;

	POSITION selpos=GetFirstSelectedItemPosition();
	if (!selpos)
		return;

	BOOL bAdded = FALSE;
	
	while (selpos)
	{
		int nItem = GetNextSelectedItem(selpos);
		if (!nItem)
			continue;
	
		int index=m_IndexMapping[nItem];

		if (m_pDirectory->direntry[index].dir)
			continue;
	
		pMainFrame->AddQueueItem(TRUE, m_pDirectory->direntry[index].name, _T(""), _T(""), m_pDirectory->path, TRUE, _T(""), _T(""), 2);	
		bAdded = TRUE;
	}

	if (bAdded)
		pMainFrame->TransferQueue(2);
}

void CFtpListCtrl::ListComplete(BOOL bSuccessful, CServerPath path, CString subdir /*=""*/)
{
	if (m_PendingDirs.empty())
		return;

	std::list<t_PendingDirs>::iterator iter;
	for (iter = m_PendingDirs.begin(); iter!=m_PendingDirs.end(); iter++)
	{
		if (path!=iter->path)
			continue;
		if (subdir.CompareNoCase(iter->Subdir))
			continue;
		break;
	}
	
	if (iter == m_PendingDirs.end())
		return;
	m_PendingDirs.erase(iter);

	if (!m_pDirectory)
	{
		m_PendingDirs.clear();
		return;
	}
	
	if (!bSuccessful)
	{
		for (int nItem=1; nItem<GetItemCount(); nItem++)
		{
			if (GetItemText(nItem, 0)==subdir)
			{
				int nIndex=m_IndexMapping[nItem];
				
				if (!m_pDirectory->direntry[nIndex].dir)
					break;
				
				if (m_pDirectory->direntry[nIndex].bLink)
				{
					m_pDirectory->direntry[nIndex].dir = false;
					m_IconCache.erase(nIndex);
					SortList(m_sortcolumn, m_sortdir);
					RedrawItems(nItem, nItem);
					
					//Check if user wanted to download/delete the directory and download/delete the file instead
					CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
					
					//Download file
					if (m_nBatchAction == 1)
					{
						CString name=subdir;
						subdir=_T("");
						while(path.IsSubdirOf(m_PathsVisited.front()))
						{
							subdir=_T("\\")+path.GetLastSegment()+subdir;
							path=path.GetParent();
						}
						subdir.TrimLeft( _T("\\") );
						pMainFrame->AddQueueItem(TRUE, name, subdir, _T(""), m_pDirectory->path, m_nFolderDownloadStart, m_transferuser, m_transferpass);
					}
					else if (m_nBatchAction == 2) // Delete file
					{
						m_FilesToDelete.push_front(subdir);
						m_FilesToDeletePaths.push_front(path);
					}
				}
				break;
			}
		}
	}

	if (m_nBatchAction)
		DoNextStep();
}

void CFtpListCtrl::FinishComplexOperation()
{
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());

	if (!m_nBatchAction)
		return;
	
	if (m_nBatchAction == 2)
	{
		//Delete all entries
		ChangeDir(m_PathsVisited.front(), "", TRUE,FZ_LIST_USECACHE | FZ_LIST_REALCHANGE);
		std::list<CServerPath>::iterator pathIter=m_FilesToDeletePaths.begin();
		for (std::list<CString>::iterator fileIter=m_FilesToDelete.begin(); fileIter!=m_FilesToDelete.end(); fileIter++, pathIter++)
			pMainFrame->m_pCommandQueue->Delete(*fileIter, *pathIter);
		m_FilesToDelete.clear();
		m_FilesToDeletePaths.clear();

		m_PathsVisited.pop_front();

		std::list<CString>::iterator deleteIter = m_FilesToDelete.begin();
		std::list<CServerPath>::iterator deletePathsIter = m_FilesToDeletePaths.begin();
		while (deleteIter != m_FilesToDelete.end())
		{
			pMainFrame->m_pCommandQueue->Delete(*deleteIter, *deletePathsIter);
			deleteIter++;
			deletePathsIter++;
		}

		for (std::list<CServerPath>::reverse_iterator iter=m_PathsVisited.rbegin(); iter!=m_PathsVisited.rend(); iter++)
			pMainFrame->m_pCommandQueue->RemoveDir(iter->GetLastSegment(), iter->GetParent());
		pMainFrame->m_pCommandQueue->SetLock(FALSE);
	}
	else
	{
		ChangeDir(m_PathsVisited.front(), "", TRUE);
		if (m_nBatchAction == 1 && m_nFolderDownloadStart)
			pMainFrame->TransferQueue(2);
			pMainFrame->m_pCommandQueue->SetLock(FALSE);
	}
	
	m_nBatchAction = 0;
	m_PathsVisited.clear();
	m_PendingDirs.clear();
	m_FilesToDelete.clear();
	m_FilesToDeletePaths.clear();
}

BOOL CFtpListCtrl::UpdateStatusBar()
{
	if (!m_pDirectory)
		return m_pOwner->SetStatusBarText(_T(""));

	CString str;
	POSITION selpos = GetFirstSelectedItemPosition();
	
	BOOL bUnsure = FALSE;
	int dircount = 0;
	int filecount = 0;
	_int64 size = 0;

	while (selpos)
	{
		int nItem = GetNextSelectedItem(selpos);
		if (!nItem)
			continue;

		int nIndex = m_IndexMapping[nItem];
		if (m_pDirectory->direntry[nIndex].dir)
				dircount++;
			else
			{
				filecount++;
				if (m_pDirectory->direntry[nIndex].size >= 0)
					size += m_pDirectory->direntry[nIndex].size;
				else
					bUnsure = TRUE;
			}
	}

	if (dircount || filecount)
	{
		if (!dircount)
			if (filecount == 1)
				if (!bUnsure)
					str.Format(IDS_DIRINFO_SELECTED_FILE, size);
				else
					str.Format(IDS_DIRINFO_SELECTED_FILEMIN, size);
			else
				if (!bUnsure)
					str.Format(IDS_DIRINFO_SELECTED_FILES, filecount, size);
				else
					str.Format(IDS_DIRINFO_SELECTED_FILESMIN, filecount, size);
		else if (!filecount)
			if (dircount == 1)
				str.LoadString(IDS_DIRINFO_SELECTED_DIR);
			else
				str.Format(IDS_DIRINFO_SELECTED_DIRS, dircount);
		else if (dircount == 1)
			if (filecount == 1)
				if (!bUnsure)
					str.Format(IDS_DIRINFO_SELECTED_DIRANDFILE, size);
				else
					str.Format(IDS_DIRINFO_SELECTED_DIRANDFILEMIN, size);
			else
				if (!bUnsure)
					str.Format(IDS_DIRINFO_SELECTED_DIRANDFILES, filecount, size);
				else
					str.Format(IDS_DIRINFO_SELECTED_DIRANDFILESMIN, filecount, size);
		else
			if (filecount == 1)
				if (!bUnsure)
					str.Format(IDS_DIRINFO_SELECTED_DIRSANDFILE, dircount, size);
				else
					str.Format(IDS_DIRINFO_SELECTED_DIRSANDFILEMIN, dircount, size);
			else
				if (!bUnsure)
					str.Format(IDS_DIRINFO_SELECTED_DIRSANDFILES, dircount, filecount, size);
				else
					str.Format(IDS_DIRINFO_SELECTED_DIRSANDFILESMIN, dircount, filecount, size);
		return m_pOwner->SetStatusBarText(str);
	}

	if (!m_pDirectory->num)
		str.LoadString(IDS_DIRINFO_EMPTY);
	else
	{
		for (int i=0; i<m_pDirectory->num; i++)
			if (m_pDirectory->direntry[i].dir)
				dircount++;
			else
			{
				filecount++;
				if (m_pDirectory->direntry[i].size >= 0)
					size += m_pDirectory->direntry[i].size;
				else
					bUnsure = TRUE;
			}
		if (!dircount)
			if (filecount == 1)
				if (!bUnsure)
					str.Format(IDS_DIRINFO_FILE, size);
				else
					str.Format(IDS_DIRINFO_FILEMIN, size);
			else
				if (!bUnsure)
					str.Format(IDS_DIRINFO_FILES, filecount, size);
				else
					str.Format(IDS_DIRINFO_FILESMIN, filecount, size);
		else if (!filecount)
			if (dircount == 1)
				str.LoadString(IDS_DIRINFO_DIR);
			else
				str.Format(IDS_DIRINFO_DIRS, dircount);
		else if (dircount == 1)
			if (filecount == 1)
				if (!bUnsure)
					str.Format(IDS_DIRINFO_DIRANDFILE, size);
				else
					str.Format(IDS_DIRINFO_DIRANDFILEMIN, size);
			else
				if (!bUnsure)
					str.Format(IDS_DIRINFO_DIRANDFILES, filecount, size);
				else
					str.Format(IDS_DIRINFO_DIRANDFILESMIN, filecount, size);
		else
			if (filecount == 1)
				if (!bUnsure)
					str.Format(IDS_DIRINFO_DIRSANDFILE, dircount, size);
				else
					str.Format(IDS_DIRINFO_DIRSANDFILEMIN, dircount, size);
			else
				if (!bUnsure)
					str.Format(IDS_DIRINFO_DIRSANDFILES, dircount, filecount, size);
				else
					str.Format(IDS_DIRINFO_DIRSANDFILESMIN, dircount, filecount, size);
	}
	
	return m_pOwner->SetStatusBarText(str);
}

void CFtpListCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	
	*pResult = 0;

	if (!m_bUpdating)
		UpdateStatusBar();
}

void CFtpListCtrl::OnDestroy() 
{
	if (COptions::GetOptionVal(OPTION_REMEMBERREMOTECOLUMNWIDTHS))
		SaveColumnSizes();

	if (COptions::GetOptionVal(OPTION_REMOTECOLUMNSORT))
	{
		int nSort = 1;
		nSort |= m_Columns[m_sortcolumn] << 1;
		nSort |= m_sortdir << 4;
		COptions::SetOption(OPTION_REMOTECOLUMNSORT, nSort);
	}

	CListCtrl::OnDestroy();
}

void CFtpListCtrl::DoNextStep()
{
	CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());

	if (!m_nBatchAction)
		return;
	if (pMainFrame->m_pCommandQueue->IsListInQueue())
		return;
	if (m_PathsToVisit.empty())
	{
		FinishComplexOperation();
		return;
	}

	while (!m_PathsToVisit.empty())
	{
		t_folder folder = m_PathsToVisit.front();
		if (ChangeDir(folder.path, folder.subdir, TRUE, FZ_LIST_USECACHE | ((m_nBatchAction == 2)?0:FZ_LIST_EXACT)))
		{
			m_PathsToVisit.pop_front();
			return;
		}
		
		m_PathsToVisit.pop_front();
	}

	if (m_PathsToVisit.empty())
	{
		FinishComplexOperation();
		return;
	}
}

static std::map<CString, CString> m_TypeCache;
CString CFtpListCtrl::GetType(CString lName, BOOL bIsDir)
{
	CString type;

	CString str = PathFindExtension(lName);
	str.MakeLower();
	std::map<CString, CString>::iterator iter = m_TypeCache.find(str);
//	if (iter != m_TypeCache.end())
	{
//		type = iter->second;
	}
//	else
	{
		SHFILEINFO shFinfo;
		CString path;
		path = "name" + str;
		memset(&shFinfo,0,sizeof(SHFILEINFO));
		if (SHGetFileInfo(path,
			bIsDir?FILE_ATTRIBUTE_DIRECTORY:FILE_ATTRIBUTE_NORMAL,
			&shFinfo,
			sizeof( shFinfo ),
			SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES))
		{
			type=shFinfo.szTypeName;
			if (type=="")
			{
				type=PathFindExtension(lName);
				if (type!="") 
					type=type.Mid(1);
				type.MakeUpper();
				if (type!="")
					type+="-file";
				else
					type="File";
			}
			else
			{
				if (!bIsDir && str != "")
					m_TypeCache[str] = type;
			}
		}
		else
		{
			type = PathFindExtension(lName);
			if (type != "") 
				type = type.Mid(1);
			type.MakeUpper();
			if (type != "")
				type += "-file";
			else
				type = "File";
		}
		m_TypeCache[m_pDirectory->path.GetSafePath()+lName] = type;		
	}
	return type;
}

void CFtpListCtrl::GetSortInfo(int &column, int &direction) const
{
	column = m_sortcolumn;
	direction = m_sortdir;
}

BOOL CFtpListCtrl::DragPosition(CImageList *pImageList, CWnd* pDragWnd, CPoint point)
{
	if (!m_pDirectory)
		return FALSE;
	ScreenToClient(&point);
	int index = HitTest(point);

	if (index > 0 && !m_pDirectory->direntry[m_IndexMapping[index]].dir)
		index = -1;
	if (pDragWnd == this)
	{
		POSITION selpos = GetFirstSelectedItemPosition();
		while (selpos)
		{
			int nItem = GetNextSelectedItem(selpos);
			if (nItem == index)
			{
				DragLeave(pImageList);
				return FALSE;
			}
		}
	}
	if (index != -1)
	{
		if (index != m_nDragHilited)
		{
			pImageList->DragShowNolock(false);
			if (m_nDragHilited != -1)
			{
				SetItemState(m_nDragHilited, 0, LVIS_DROPHILITED);
				RedrawItems(m_nDragHilited, m_nDragHilited);
			}
			m_nDragHilited = index;
			SetItemState(index, LVIS_DROPHILITED, LVIS_DROPHILITED);
			RedrawItems(index, index);
			UpdateWindow();
			pImageList->DragShowNolock(true);
		}
	}
	else
		DragLeave(pImageList);

	return TRUE;
}

void CFtpListCtrl::DragLeave(CImageList *pImageList)
{
	if (m_nDragHilited != -1)
	{
		if (pImageList)
			pImageList->DragShowNolock(false);
		SetItemState(m_nDragHilited, 0, LVIS_DROPHILITED);
		RedrawItems(m_nDragHilited, m_nDragHilited);
		UpdateWindow();
		if (pImageList)
			pImageList->DragShowNolock(true);
		m_nDragHilited = -1;
	}
}

CString CFtpListCtrl::GetDropTarget() const
{
	if (!m_pDirectory)
		return _T("");

	CServerPath path = GetCurrentDirectory();

	if (!m_nDragHilited)
	{
		if (!path.HasParent())
			return _T("");
		path = path.GetParent();
	}
	else if (m_nDragHilited != -1)
		path.AddSubdir(m_pDirectory->direntry[m_IndexMapping[m_nDragHilited]].name);
	
	return path.GetSafePath();
}