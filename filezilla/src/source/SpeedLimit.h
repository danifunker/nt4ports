// SpeedLimit.h: interface for the CSpeedLimit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPEEDLIMIT_H__D4CEBB35_CE08_4438_9A42_4F565DE84AE4__INCLUDED_)
#define AFX_SPEEDLIMIT_H__D4CEBB35_CE08_4438_9A42_4F565DE84AE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpeedLimit : public CObject  
{
public:
	bool IsItActive( CTime &time);
	CSpeedLimit * GetCopy();
	CString GetSpeedLimitString();
	static CSpeedLimit * ParseSpeedLimit( CString str);
	CString GetListBoxString();
	CSpeedLimit();
	virtual ~CSpeedLimit();

	BOOL	m_DateCheck;
	CTime	m_Date;
	BOOL	m_FromCheck;
	CTime	m_FromTime;
	BOOL	m_ToCheck;
	CTime	m_ToTime;
	int		m_Speed;
	BOOL	m_Day[ 7];
};

typedef std::vector<CSpeedLimit *> SPEEDLIMITSLIST;

#endif // !defined(AFX_SPEEDLIMIT_H__D4CEBB35_CE08_4438_9A42_4F565DE84AE4__INCLUDED_)
