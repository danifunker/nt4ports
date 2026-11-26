#include "stdafx.h"
#include "version.h"
#include "pathfunctions.h"

CString GetProgramFile()
{
	CString	RtnVal;
	TCHAR	FileName[MAX_PATH];
	GetModuleFileName(0, FileName, MAX_PATH);
	RtnVal = FileName;
	return RtnVal;
}

CString GetVersionString()
{
	//Fill the version info
	CString fullpath=GetProgramFile();

	TCHAR *str=new TCHAR[fullpath.GetLength()+1];
	_tcscpy(str,fullpath);
	DWORD tmp=0;
	DWORD len=GetFileVersionInfoSize(str, &tmp);
	if (!len)
		return "FileZilla";
	LPVOID pBlock=new char[len];
	GetFileVersionInfo(str,0,len,pBlock);
	LPVOID ptr;
	UINT ptrlen;

	CString ProductName;
	//Retreive the product name
	
	TCHAR SubBlock[50];
			
	// Structure used to store enumerated languages and code pages.
	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;

	UINT cbTranslate;
			
	// Read the list of languages and code pages.
	if (VerQueryValue(pBlock, 
				TEXT("\\VarFileInfo\\Translation"),
				(LPVOID*)&lpTranslate,
				&cbTranslate))
	{
		// Read the file description for each language and code page.
	
		_stprintf( SubBlock, 
	           _T("\\StringFileInfo\\%04x%04x\\ProductName"),
	           lpTranslate[0].wLanguage,
	           lpTranslate[0].wCodePage);
		// Retrieve file description for language and code page "0". 
		if (VerQueryValue(pBlock, 
				SubBlock, 
				&ptr, 
					&ptrlen))
		{
			ProductName=(TCHAR*)ptr;
		}
	}
	CString version;
	//Format the versionstring
	if (VerQueryValue(pBlock, _T("\\"), &ptr, &ptrlen))
	{
		VS_FIXEDFILEINFO *fi=(VS_FIXEDFILEINFO*)ptr;
		unsigned __int64 curver=(((__int64)fi->dwFileVersionMS)<<32)+fi->dwFileVersionLS;
		
		
		if (fi->dwFileVersionMS>>16)
		{
			//v1.00+
			if ((fi->dwFileVersionLS&0xFFFF)%100)
			{ //betas
				if (fi->dwFileVersionLS>>16)
					version.Format(_T("%s version %d.%d.%d beta %d"), ProductName, fi->dwFileVersionMS>>16, fi->dwFileVersionMS&0xFFFF, fi->dwFileVersionLS>>16, (fi->dwFileVersionLS&0xFFFF)%100);
				else
					version.Format(_T("%s version %d.%d beta %d"), ProductName, fi->dwFileVersionMS>>16, fi->dwFileVersionMS&0xFFFF, (fi->dwFileVersionLS&0xFFFF)%100);
			}
			else
			{ //final versions
				if ((fi->dwFileVersionLS&0xFFFF)/100)
					if (fi->dwFileVersionLS>>16) //minor releases
						version.Format(_T("%s version %d.%d.%d%c"), ProductName, fi->dwFileVersionMS>>16, fi->dwFileVersionMS&0xFFFF, fi->dwFileVersionLS>>16, 'a'+(fi->dwFileVersionLS&0xFFFF)/100-1);
					else
						version.Format(_T("%s version %d.%d%c"), ProductName, fi->dwFileVersionMS>>16, fi->dwFileVersionMS&0xFFFF, 'a'+(fi->dwFileVersionLS&0xFFFF)/100-1);
				else
					if (fi->dwFileVersionLS>>16)
						version.Format(_T("%s version %d.%d.%d"), ProductName, fi->dwFileVersionMS>>16, fi->dwFileVersionMS&0xFFFF, fi->dwFileVersionLS>>16);
					else
						version.Format(_T("%s version %d.%d"), ProductName, fi->dwFileVersionMS>>16, fi->dwFileVersionMS&0xFFFF);
			}
		}
		else
		{
			//beta versions
			if (fi->dwFileVersionLS&0xFFFF)
				//beta
				version.Format(_T("%s Beta %d.%d beta %d"), ProductName, fi->dwFileVersionMS&0xFFFF,fi->dwFileVersionLS>>16,fi->dwFileVersionLS&0xFFFF);
			else
				//final version
				version.Format(_T("%s Beta %d.%d"), ProductName, fi->dwFileVersionMS&0xFFFF,fi->dwFileVersionLS>>16);
		}
			
	}

	delete [] str;
	delete [] pBlock;
	return version;
}