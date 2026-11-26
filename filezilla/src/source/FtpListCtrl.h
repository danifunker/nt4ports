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

#if !defined(AFX_FTPLISTCTRL_H__317FFBAF_01C2_4CBA_BCD1_398B97054C36__INCLUDED_)
#define AFX_FTPLISTCTRL_H__317FFBAF_01C2_4CBA_BCD1_398B97054C36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FtpListCtrl.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CFtpListCtrl 
class CFtpView;

class CFtpListCtrl : public CListCtrl
{
// Konstruktion
public:
	CFtpListCtrl(CFtpView *pOwner);

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CFtpListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	//}}AFX_VIRTUAL

// Implementierung
public:
	void ListComplete(BOOL bSuccessful, CServerPath path, CString subdir = _T(""));
	void SaveColumnSizes();
	BOOL HasUnsureEntries();
	CServerPath GetCurrentDirectory() const;
	void Refresh();
	void UpdateColumns(int nHideColumns);
	void SetListStyle(int nStyle);
	int m_nStyle;
	BOOL GetTransferfile(t_transferfile &transferfile);
	void List(t_directory *list);
	virtual ~CFtpListCtrl();
	void ReloadHeader();
	void SortList(int item, int direction = 0);
	void GetSortInfo(int &column, int &direction) const;

	// Drag&Drop
	BOOL DragPosition(CImageList *pImageList, CWnd* pDragWnd, CPoint point);
	void DragLeave(CImageList *pImageList);
	void OnDragEnd(int target, CPoint point);
	CString GetDropTarget() const;

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	CString GetType(CString lName, BOOL bIsDir);
	void DoNextStep();
	BOOL m_bUpdating;
	CFtpView *m_pOwner;
	BOOL UpdateStatusBar();
	void FinishComplexOperation();
	std::list<CString> m_FilesToDelete;
	std::list<CServerPath> m_FilesToDeletePaths;

	struct t_PendingDirs
	{
		CServerPath path;
		CString Subdir;
	};
	std::list<t_PendingDirs> m_PendingDirs;
	BOOL ChangeDir(const CServerPath &path,CString name, BOOL bAddToQueue=FALSE, int nListMode = FZ_LIST_USECACHE);
	void QSortByName(int begin, int end);
	void QSortBySize(int begin, int end);
	void QSortByType(const std::vector<CString> &typeArray, int begin, int end);
	void QSortByTime(int begin, int end);

	std::vector<int> m_IndexMapping;
	std::map<int, int> m_IconCache;

	int m_nHideColumns;
	CString m_transferuser;
	CString m_transferpass;
	void ReloadHeaderItem(int nIndex, UINT nID);
	t_directory *m_pDirectory;
	int m_sortcolumn;
	int m_sortdir;
	CImageList m_SortImg;
	UINT GetSelectedItem();
	int m_Columns[7];
	
	std::list<CServerPath> m_PathsVisited;
	struct t_folder
	{
		CServerPath path;
		CString subdir;
	};
	std::list<t_folder> m_PathsToVisit;
	int m_nBatchAction;

	BOOL m_nFolderDownloadStart;

	// Drag&Drop
	int m_nDragHilited;


	BOOL GetSysImgList();
	//{{AFX_MSG(CFtpListCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFtpcontextAddtoqueue();
	afx_msg void OnFtpcontextAttributes();
	afx_msg void OnFtpcontextCreatedir();
	afx_msg void OnFtpcontextDelete();
	afx_msg void OnFtpcontextDownload();
	afx_msg void OnFtpcontextDownloadas();
	afx_msg void OnFtpcontextOpen();
	afx_msg void OnFtpcontextRename();
	afx_msg void OnFtpcontextViewEdit();
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_FTPLISTCTRL_H__317FFBAF_01C2_4CBA_BCD1_398B97054C36__INCLUDED_
