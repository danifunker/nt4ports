// SpeedLimit.cpp: implementation of the CSpeedLimit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "filezilla.h"
#include "SpeedLimit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpeedLimit::CSpeedLimit()
{
	for ( int i = 0; i < 7; i++)
		m_Day[ i] = FALSE;

	m_Speed = 8;
	m_ToCheck = FALSE;
	m_DateCheck = FALSE;
	m_FromCheck = FALSE;
}

CSpeedLimit::~CSpeedLimit()
{

}

CString CSpeedLimit::GetListBoxString()
{
	CString str;

	str.Format( _T( "%dkB/s"), m_Speed);

	CString help;

	if ( m_DateCheck)
	{
		help = m_Date.Format( _T( "%x"));
	}

	if ( m_FromCheck)
	{
		if ( help.GetLength() > 0)
			help += _T( "; ");

		help += _T( "F:") + m_FromTime.Format( _T( "%X"));
	}

	if ( m_ToCheck)
	{
		if ( help.GetLength() > 0)
			help += _T( "; ");

		help += _T( "T:") + m_ToTime.Format( _T( "%X"));
	}

	for ( int i = 0; i < 7; i++)
	{
		if ( !m_Day[ i])
		{
			if ( help.GetLength() > 0)
				help += _T( "; ");

			bool was = false;
			for ( int j = 0; j < 7; j++)
			{
				if ( m_Day[ j])
				{
					if ( was)
						help += _T( ", ");

					was = true;

					CTime time( 2001, 1, j + 1, 0, 0, 0);

					help += time.Format( _T( "%a"));
				}
			}

			break;
		}
	}

	return str + _T( " [") + help + _T( "]");
}

CSpeedLimit * CSpeedLimit::ParseSpeedLimit(CString str)
{
	CSpeedLimit *res = new CSpeedLimit();

	while ( str.GetLength() > 0)
	{
		int i = str.Find( ';');
		CString parse;

		if ( i < 0)
		{
			parse = str;
			str.Empty();
		}
		else
		{
			parse = str.Left( i);
			str = str.Mid( i + 1);
		}

		if ( parse.GetLength() > 0)
		{
			if ( parse[ 0] == 'S')
			{
				res->m_Speed = _ttoi( parse.Mid( 1));
				continue;
			}

			if ( parse[ 0] == 'D')
			{
				for ( int i = 1; i < parse.GetLength(); i++)
				{
					if ( parse[ i] == '1')
						res->m_Day[ 0] = TRUE;
					if ( parse[ i] == '2')
						res->m_Day[ 1] = TRUE;
					if ( parse[ i] == '3')
						res->m_Day[ 2] = TRUE;
					if ( parse[ i] == '4')
						res->m_Day[ 3] = TRUE;
					if ( parse[ i] == '5')
						res->m_Day[ 4] = TRUE;
					if ( parse[ i] == '6')
						res->m_Day[ 5] = TRUE;
					if ( parse[ i] == '7')
						res->m_Day[ 6] = TRUE;
				}

				continue;
			}

			if ( parse[ 0] == 'F')
			{
				if ( parse.GetLength() != 7)
				{
					delete res;
					return NULL;
				}

				res->m_FromCheck = TRUE;

				res->m_FromTime = CTime( 2001, 1, 1, _ttoi( parse.Mid( 1, 2)), _ttoi( parse.Mid( 3, 2)), _ttoi( parse.Mid( 5, 2)));

				continue;
			}

			if ( parse[ 0] == 'T')
			{
				if ( parse.GetLength() != 7)
				{
					delete res;
					return NULL;
				}

				res->m_ToCheck = TRUE;

				res->m_ToTime = CTime( 2001, 1, 1, _ttoi( parse.Mid( 1, 2)), _ttoi( parse.Mid( 3, 2)), _ttoi( parse.Mid( 5, 2)));

				continue;
			}

			if ( parse[ 0] == 'A')
			{
				if ( parse.GetLength() != 9)
				{
					delete res;
					return NULL;
				}

				res->m_DateCheck = TRUE;

				res->m_Date = CTime( _ttoi( parse.Mid( 5, 4)), _ttoi( parse.Mid( 3, 2)), _ttoi( parse.Mid( 1, 2)), 0, 0, 0);

				continue;
			}
		}
	}

	return res;
}

CString CSpeedLimit::GetSpeedLimitString()
{
	CString str;
	CString help;

	for ( int i = 0; i < 7; i++)
	{
		if ( m_Day[ i])
		{
			help.Format( _T( "%d"), i + 1);
			str = str + help;
		}
	}

	if ( str.GetLength() > 0)
		str = _T( "D") + str + _T( ";");

	help.Format( _T( "S%d;"), m_Speed);

	str = help + str;

	if ( m_FromCheck)
	{
		help.Format( _T( "F%02d%02d%02d;"), m_FromTime.GetHour(), m_FromTime.GetMinute(), m_FromTime.GetSecond());
		str += help;
	}

	if ( m_ToCheck)
	{
		help.Format( _T( "T%02d%02d%02d;"), m_ToTime.GetHour(), m_ToTime.GetMinute(), m_ToTime.GetSecond());
		str += help;
	}

	if ( m_DateCheck)
	{
		help.Format( _T( "A%02d%02d%04d;"), m_Date.GetDay(), m_Date.GetMonth(), m_Date.GetYear());
		str += help;
	}

	return str;
}

CSpeedLimit * CSpeedLimit::GetCopy()
{
	CSpeedLimit *res = new CSpeedLimit();

	res->m_DateCheck = m_DateCheck;
	res->m_Date = m_Date;
	res->m_FromCheck = m_FromCheck;
	res->m_FromTime = m_FromTime;
	res->m_ToCheck = m_ToCheck;
	res->m_ToTime = m_ToTime;
	res->m_Speed = m_Speed;

	for ( int i = 0; i < 7; i++)
		res->m_Day[ i] = m_Day[ i];

	return res;
}

bool CSpeedLimit::IsItActive(CTime &time)
{
	if ( m_DateCheck)
	{
		if ( ( m_Date.GetYear() != time.GetYear()) ||
			( m_Date.GetMonth() != time.GetMonth()) ||
			( m_Date.GetDay() != time.GetDay()))
			return false;
	}
	else
	{
		int i = CTime::GetCurrentTime().GetDayOfWeek() - 2;

		while ( i < 0) i += 7;
		while ( i >= 7) i -= 7;

		if ( !m_Day[ i])
			return false;
	}

	if (m_FromCheck && m_ToCheck)
	{
		CTimeSpan span = m_ToTime - m_FromTime;
		if (m_ToTime < m_FromTime)
			span += CTimeSpan(1, 0, 0, 0);
		CTimeSpan ref = time - m_FromTime;
		if (time < m_FromTime)
			ref += CTimeSpan(1, 0, 0, 0);
		if (span < ref)
			return false;
	}
	else
	{
		if ( m_ToCheck)
		{
			if ( m_ToTime.GetHour() < time.GetHour())
				return false;

			if ( m_ToTime.GetHour() == time.GetHour())
			{
				if ( m_ToTime.GetMinute() < time.GetMinute())
					return false;

				if ( m_ToTime.GetMinute() == time.GetMinute())
				{
					if ( m_ToTime.GetSecond() < time.GetSecond())
						return false;
				}
			}
		}

		if (m_FromCheck)
		{
			if ( m_FromTime.GetHour() > time.GetHour())
				return false;

			if ( m_FromTime.GetHour() == time.GetHour())
			{
				if ( m_FromTime.GetMinute() > time.GetMinute())
					return false;

				if ( m_FromTime.GetMinute() == time.GetMinute())
				{
					if ( m_FromTime.GetSecond() > time.GetSecond())
						return false;
				}
			}
		}
	}

	return true;
}
