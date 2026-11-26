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

// FileExistsDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "FileExistsDlg.h"
#include "queuectrl.h"
#include "entersomething.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CFileExistsDlg::m_nAction=0;
BOOL CFileExistsDlg::m_bAlways=FALSE;


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFileExistsDlg 


CFileExistsDlg::CFileExistsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileExistsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileExistsDlg)
	m_CurAction = 0;
	m_CurAll = FALSE;
	//}}AFX_DATA_INIT
	m_DataFileTime1=0;
	m_DataFileTime2=0;
}

CFileExistsDlg::~CFileExistsDlg()
{
}


void CFileExistsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileExistsDlg)
	DDX_Control(pDX, IDC_FILEEXISTS_DATE2, m_cTime2);
	DDX_Control(pDX, IDC_FILEEXISTS_DATE1, m_cTime1);
	DDX_Control(pDX, IDC_FILEEXISTS_SIZE2, m_cSize2);
	DDX_Control(pDX, IDC_FILEEXISTS_SIZE1, m_cSize1);
	DDX_Control(pDX, IDC_FILEEXISTS_ICON2, m_cIcon2);
	DDX_Control(pDX, IDC_FILEEXISTS_ICON1, m_cIcon1);
	DDX_Control(pDX, IDC_FILEEXISTS_FILE2, m_cFile2);
	DDX_Control(pDX, IDC_FILEEXISTS_FILE1, m_cFile1);
	DDX_Radio(pDX, IDC_FILEEXISTS_RADIO1, m_CurAction);
	DDX_Check(pDX, IDC_FILEEXISTS_ALWAYS, m_CurAll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileExistsDlg, CDialog)
	//{{AFX_MSG_MAP(CFileExistsDlg)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CFileExistsDlg 

BOOL CFileExistsDlg::OnInitDialog() 
{
	USES_CONVERSION;
	CDialog::OnInitDialog();
	
	CString str;
	SYSTEMTIME timeDest;
	
	/*m_cFile1.GetWindowText(tmp);
	str.Format(tmp,m_DataFile1);
	m_cFile1.SetWindowText(str);*/
	m_cFile1.SetWindowText(FormatLongPath(IDC_FILEEXISTS_FILE1,m_DataFile1));

	SHFILEINFO fileinfo;
	memset(&fileinfo,0,sizeof(fileinfo));
	SHGetFileInfo(m_DataFile1,FILE_ATTRIBUTE_NORMAL,&fileinfo,sizeof(fileinfo),SHGFI_ICON|SHGFI_USEFILEATTRIBUTES);
	m_cIcon1.SetIcon(fileinfo.hIcon);

	m_cSize1.GetWindowText(str);
	if (m_DataFileSize1>-1)
	{
		char buffer[500];
		sprintf(buffer,"%I64d",m_DataFileSize1);
		str.Replace(_T("%d"),A2CT(buffer));
	}
	else
		str="Size unknown";
	m_cSize1.SetWindowText(str);

	if (m_DataFileTime1)
	{
		m_DataFileTime1->GetAsSystemTime( timeDest ) ;
		
		
		TCHAR text[200];
		if (!GetDateFormat(
			LOCALE_USER_DEFAULT,               // locale for which date is to be formatted
			DATE_SHORTDATE,             // flags specifying function options
			&timeDest,  // date to be formatted
			0,          // date format string
			text,          // buffer for storing formatted string
			200                // size of buffer
			))
			return TRUE;
		str=text;
			
		if (!GetTimeFormat(
			LOCALE_USER_DEFAULT,               // locale for which date is to be formatted
			TIME_NOSECONDS|TIME_FORCE24HOURFORMAT,             // flags specifying function options
			&timeDest,  // date to be formatted
			0,          // date format string
			text,          // buffer for storing formatted string
			200                // size of buffer
			))
			return TRUE;
		str+=" ";
		str+=text;
	}
	else
		str="Date/Time unknown";
	m_cTime1.SetWindowText(str);
	
	
	m_cFile2.SetWindowText(FormatLongPath(IDC_FILEEXISTS_FILE2,m_DataFile2));

	memset(&fileinfo,0,sizeof(fileinfo));
	SHGetFileInfo(m_DataFile2,FILE_ATTRIBUTE_NORMAL,&fileinfo,sizeof(fileinfo),SHGFI_ICON|SHGFI_USEFILEATTRIBUTES);
	m_cIcon2.SetIcon(fileinfo.hIcon);
	
	m_cSize2.GetWindowText(str);
	if (m_DataFileSize2>-1)
	{
		char buffer[500];
		sprintf(buffer,"%I64d",m_DataFileSize2);
		str.Replace(_T("%d"),A2CT(buffer));
	}
	else
		str="Size unknown";
	m_cSize2.SetWindowText(str);

	if (m_DataFileTime2)
	{
		m_DataFileTime2->GetAsSystemTime( timeDest ) ;
		
		TCHAR text[200];
		if (!GetDateFormat(
			LOCALE_USER_DEFAULT,               // locale for which date is to be formatted
			DATE_SHORTDATE,             // flags specifying function options
			&timeDest,  // date to be formatted
			0,          // date format string
			text,          // buffer for storing formatted string
			200                // size of buffer
			))
			return TRUE;
		str=text;
			
		if (!GetTimeFormat(
			LOCALE_USER_DEFAULT,               // locale for which date is to be formatted
			TIME_NOSECONDS|TIME_FORCE24HOURFORMAT,             // flags specifying function options
			&timeDest,  // date to be formatted
			0,          // date format string
			text,          // buffer for storing formatted string
			200                // size of buffer
			))
			return TRUE;
		str+=" ";
		str+=text;
	}
	else
		str="Date/Time unknown";
	m_cTime2.SetWindowText(str);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

CString CFileExistsDlg::FormatLongPath(UINT nID,CString oLongPath) const
{
	//Based on code by Scott D. Killen

	//will be passed instead of original
	CString oModifString( oLongPath );

	//will be used to get measurements
	CWnd *oWnd=GetDlgItem(nID);

	if( !oModifString.IsEmpty() && oWnd->GetSafeHwnd() )
	{
		CRect rect;
		oWnd->GetWindowRect(rect);
		int nMaxTextWidth=rect.Width()-4;
		
		CClientDC oClientDC(oWnd);

		//trying to determine the system metrix to create apropriate fonts for measurement
		NONCLIENTMETRICS NonClientMetrics;
		
		NonClientMetrics.cbSize = sizeof(NONCLIENTMETRICS);
		
		BOOL bSystemMetrics = SystemParametersInfo( SPI_GETNONCLIENTMETRICS, 
			NonClientMetrics.cbSize, 
			&NonClientMetrics,
			0 );

		if( bSystemMetrics )
		{
			CFont oMessageFont;//lets create the fonts same as the selected Message Font on the Display/Appearance tab

			if( oMessageFont.CreateFontIndirect(&NonClientMetrics.lfMessageFont) )
			{
				oClientDC.SelectObject( &oMessageFont );
			}
		}
		else
		{
			oClientDC.SelectStockObject( SYSTEM_FONT );	//it MUST NOT happen, but in case...
		}
		
		//measure the actual text width
		int nTextWidth = oClientDC.GetTextExtent( oModifString ).cx;

		//to check whether it's correct uncoment below and change directory few times...
		//oClientDC.SelectStockObject( BLACK_PEN );
		//oClientDC.Rectangle( 0, 0, nMargin, nMargin*5 );
		//oClientDC.Rectangle( nMaxTextWidth+nMargin, 0, oClientRect.Width(), nMargin*5 );
		//oClientDC.Rectangle( nMargin, 0, nMaxTextWidth+nMargin, nMargin );
		//oClientDC.TextOut( nMargin, 0, oModifString );
		
		//after all this measurements time to do the real job
		if( nTextWidth > nMaxTextWidth )
		{
			int nRootDirIndex, nLastDirIndex;

			//this is the testing line:
			//oModifString = "\\\\computer_name\\dir1\\subdir1" + oModifString.Right(oModifString.GetLength() - 2 );

			nRootDirIndex = oModifString.Find( '\\' );
			if (nRootDirIndex==-1)
				nRootDirIndex = oModifString.Find( '/' );
			nLastDirIndex = oModifString.ReverseFind( '\\' );
			if (nLastDirIndex==-1)
				nLastDirIndex = oModifString.ReverseFind( '/' );

			if( nRootDirIndex == 0 )	//we have to deal with the network 'drive', which would look like that: \\computer_name\dir1\subdir1
			{
				nRootDirIndex = oModifString.Find( '\\', nRootDirIndex+1 );
				if( nRootDirIndex != -1 )
				{
					nRootDirIndex = oModifString.Find( '\\', nRootDirIndex+1 );
				}
				else
					nRootDirIndex = oModifString.Find( '/', nRootDirIndex+1 );
			}

			if( nRootDirIndex != -1 && nLastDirIndex != -1 )
			{
				nRootDirIndex += 1;	//increase for the tactical reasons

				CString oDottedText( "..." );//this three dots will be used to indicate the cut part of the path

				CString oRootDirectory; 	//this can be cut as the last one
				CString oMidDirectoryPart;	//we will try to shorten this part first
				CString oLastDirectory; 	//and then, if still too long we'll cut this one
				
				oRootDirectory =	oModifString.Left( nRootDirIndex );
				oMidDirectoryPart =	oModifString.Mid( nRootDirIndex, nLastDirIndex - nRootDirIndex );
				oLastDirectory =	oModifString.Mid( nLastDirIndex );
				
				while( nTextWidth > nMaxTextWidth )
				{
					int nMidPartLenght = oMidDirectoryPart.GetLength();

					oModifString = oRootDirectory + oMidDirectoryPart + oDottedText + oLastDirectory;

					//measure the actual text width again
					nTextWidth = oClientDC.GetTextExtent( oModifString ).cx;
					
					if( nMidPartLenght > 0 )
					{
						//prepare for the next loop (if any)
						oMidDirectoryPart = oMidDirectoryPart.Left(oMidDirectoryPart.GetLength() - 1 );
					}
					else
					{
						int nLastDirectoryLenght = oLastDirectory.GetLength();
						
						if( nLastDirectoryLenght > 0 )
						{
							//prepare for the next loop (if any)
							oLastDirectory = oLastDirectory.Right(oLastDirectory.GetLength() - 1 );
						}
						else
						{
							//should not come here, what size of the fonts are you using?!
							//anyway, we will do different now, cutting from the end...
							int nRootDirectoryLenght = oRootDirectory.GetLength();

							if( nRootDirectoryLenght > 0 )
							{
								oRootDirectory = oRootDirectory.Left(oRootDirectory.GetLength() - 1 );
							}
							else
							{
								TRACE0( "Mayday, Mayday!!!\n" );
								oModifString = oLongPath;
								//something wrong, give me a...
								break;
							}
						}
					}
				}//end while
			}
		}
	}

	return oModifString;
}

int CFileExistsDlg::GetAction()
{
	return m_nAction;
}

BOOL CFileExistsDlg::Always()
{
	return m_bAlways;
}

void CFileExistsDlg::SetData(CString filename1, _int64 size1, CTime *time1, CString filename2, _int64 size2, CTime *time2)
{
	m_DataFile1 = filename1;
	m_DataFile2 = filename2;
	m_DataFileSize1 = size1;
	m_DataFileSize2 = size2;
	m_DataFileTime1 = time1;
	m_DataFileTime2 = time2;
}

void CFileExistsDlg::OnDestroy() 
{
	HICON icon=0;
	icon=m_cIcon1.GetIcon();
	if (icon)
		DestroyIcon(icon);
	icon=0;
	icon=m_cIcon2.GetIcon();
	if (icon)
		DestroyIcon(icon);

	CDialog::OnDestroy();
}

void CFileExistsDlg::OnClose() 
{
	m_nAction=FILEEXISTS_SKIP;
	
	CDialog::OnClose();
}

void CFileExistsDlg::OnOK() 
{
	UpdateData(TRUE);
	m_nAction=m_CurAction;
	if (m_CurAll)
		m_bAlways=TRUE;
	CDialog::OnOK();
}

void CFileExistsDlg::SetOverwriteSettings(int nAction)
{
	if (nAction==FILEEXISTS_ASK)
		m_bAlways=FALSE;
	else
	{
		m_bAlways=TRUE;
		m_nAction=nAction;
	}
}

BOOL CFileExistsDlg::ProcessRequest(COverwriteRequestData *pData, CFileZillaApi *pFileZillaApi, BOOL bNoDisplayDialog)
{
	ASSERT(pData->nRequestType == FZ_ASYNCREQUEST_OVERWRITE);
	
	if (!pFileZillaApi)
		return TRUE;

	int nAction = 0;
	if (!pData->pTransferFile->get && pData->pTransferFile->nUserData == 3)
	{
		nAction=0;
	}
	else
	{
		if (CFileExistsDlg::Always())
			nAction = GetAction();
		else
		{
			if (bNoDisplayDialog)
				return FALSE;

			CFileExistsDlg dlg;
			if (pData->size2 >= pData->size1)
				dlg.m_bShowResumeCtrl = FALSE;
			else
				dlg.m_bShowResumeCtrl = TRUE;
			
			dlg.SetData(pData->path1 + pData->FileName1, pData->size1, pData->time1, pData->path2 + pData->FileName2, pData->size2, pData->time2);
			
			::EndMenu();
			if (dlg.DoModal() == IDOK)
				nAction = dlg.GetAction();
			else
				nAction = FILEEXISTS_SKIP;
		}
		if (nAction == FILEEXISTS_RENAME)
		{
			if (bNoDisplayDialog)
				return FALSE;

			CEnterSomething dlg(IDS_INPUTDIALOGTITLE_RENAME, IDS_INPUTDIALOGTEXT_RENAME);
			::EndMenu();
			if (dlg.DoModal() == IDOK)
				pData->FileName1 = dlg.m_String;
			else
				nAction = FILEEXISTS_SKIP;
		}
	}
	if (pFileZillaApi->IsValid()) //Make sure data.pFileZillaApi still exists
		pFileZillaApi->SetAsyncRequestResult(nAction, pData);
	
	return TRUE;
}
