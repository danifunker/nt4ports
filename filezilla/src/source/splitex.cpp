////////////////////////////////////////////////////////////////////////////
//
// splitex.cpp
// Based upon code from Oleg G. Galkin
// Modified to handle multiple hidden rows

#include "stdafx.h"
#include "splitex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
//
// CSplitterWndEx

CSplitterWndEx::CSplitterWndEx()
{
	m_arr = NULL;
	m_colarr = NULL;
}

CSplitterWndEx::~CSplitterWndEx()
{
	delete [] m_arr;
	delete [] m_colarr;
}

int CSplitterWndEx::Id_short(int row, int col)
{
	return AFX_IDW_PANE_FIRST + row * 16 + col;
}

void CSplitterWndEx::ShowRow(int r)
{
    ASSERT_VALID(this);
    ASSERT(m_nRows < m_nMaxRows);

	ASSERT(m_arr);
	ASSERT(r < m_length);
	ASSERT(m_arr[r] >= m_nRows);
	ASSERT(m_arr[r] < m_length);

	int rowNew = r;
	int cyNew = m_pRowInfo[m_arr[r]].nCurSize;
	int cyIdealNew = m_pRowInfo[m_arr[r]].nIdealSize;
	
	int new_val = 0;

	for (int i = rowNew - 1; i >= 0; i--)
		if (m_arr[i] < m_nRows)	// not hidden
		{
			new_val = m_arr[i] + 1;
			break;
		}

	int old_val = m_arr[rowNew];

    m_nRows++;  // add a row

    // fill the hided row
    int row;
    for (int col = 0; col < m_nCols; col++)
    {
		CWnd* pPaneShow = GetDlgItem(
			Id_short(old_val, col));
        ASSERT(pPaneShow != NULL);
        pPaneShow->ShowWindow(SW_SHOWNA);

		for (row = m_length - 1; row >= 0; row--)
        {
			if ((m_arr[row] >= new_val) &&
				(m_arr[row] < old_val))
			{
				CWnd* pPane = CSplitterWnd::GetPane(m_arr[row], col);
				ASSERT(pPane != NULL);
				pPane->SetDlgCtrlID(Id_short(m_arr[row] + 1, col));
			}
        }
		pPaneShow->SetDlgCtrlID(Id_short(new_val, col));
    }

	for (row = 0; row < m_length; row++)
		if ((m_arr[row] >= new_val) &&
			(m_arr[row] < old_val))
			m_arr[row]++;

	m_arr[rowNew] = new_val;

    //new panes have been created -- recalculate layout
	for (row = new_val + 1; row < m_length; row++)
	{
		if (m_arr[row]<m_nRows)
		{
			m_pRowInfo[m_arr[row]].nIdealSize = m_pRowInfo[m_arr[row-1]].nCurSize;
			m_pRowInfo[m_arr[row]].nCurSize = m_pRowInfo[m_arr[row-1]].nCurSize;
		}
	}
	if (cyNew>=0x10000)
	{
		int rowToResize=(cyNew>>16)-1;
		cyNew%=0x10000;
		cyIdealNew%=0x10000;
		m_pRowInfo[m_arr[rowToResize]].nCurSize-=cyNew+m_cxSplitter;
		m_pRowInfo[m_arr[rowToResize]].nIdealSize=m_pRowInfo[m_arr[rowToResize]].nCurSize;//-=cyIdealNew+m_cxSplitter;
	}

	m_pRowInfo[new_val].nIdealSize = cyNew;
	m_pRowInfo[new_val].nCurSize = cyNew;
    RecalcLayout();
}

void CSplitterWndEx::ShowCol(int c)
{
    ASSERT_VALID(this);
    ASSERT(m_nCols < m_nMaxCols);

	ASSERT(m_colarr);
	ASSERT(c < m_collength);
	ASSERT(m_colarr[c] >= m_nRows);
	ASSERT(m_colarr[c] < m_collength);

	int colNew = c;
	int cxNew = m_pColInfo[m_colarr[c]].nCurSize;
	int cxIdealNew = m_pColInfo[m_colarr[c]].nIdealSize;
	
	int new_val = 0;

	for (int i = colNew - 1; i >= 0; i--)
		if (m_colarr[i] < m_nCols)	// not hidden
		{
			new_val = m_colarr[i] + 1;
			break;
		}

	int old_val = m_colarr[colNew];

    m_nCols++;  // add a row

    // fill the hided row
    int col;
    for (int row = 0; row < m_nRows; row++)
    {
		CWnd* pPaneShow = GetDlgItem(
			Id_short(row, old_val));
        ASSERT(pPaneShow != NULL);
        pPaneShow->ShowWindow(SW_SHOWNA);

		for (col = m_collength - 1; col >= 0; col--)
        {
			if ((m_colarr[col] >= new_val) &&
				(m_colarr[col] < old_val))
			{
				CWnd* pPane = CSplitterWnd::GetPane(row, m_colarr[col]);
				ASSERT(pPane != NULL);
				pPane->SetDlgCtrlID(Id_short(row, m_colarr[col] + 1));
			}
        }
		pPaneShow->SetDlgCtrlID(Id_short(row, new_val));
    }

	for (col = 0; col < m_collength; col++)
		if ((m_colarr[col] >= new_val) &&
			(m_colarr[col] < old_val))
			m_colarr[col]++;

	m_colarr[colNew] = new_val;

    //new panes have been created -- recalculate layout
	for (col = new_val + 1; col < m_collength; col++)
	{
		if (m_colarr[col]<m_nCols)
		{
			m_pColInfo[m_colarr[col]].nIdealSize = m_pColInfo[m_colarr[col-1]].nCurSize;
			m_pColInfo[m_colarr[col]].nCurSize = m_pColInfo[m_colarr[col-1]].nCurSize;
		}
	}
	if (cxNew>=0x10000)
	{
		int colToResize=(cxNew>>16)-1;
		cxNew%=0x10000;
		cxIdealNew%=0x10000;
		m_pColInfo[m_colarr[colToResize]].nCurSize-=cxNew+m_cySplitter;
		m_pColInfo[m_colarr[colToResize]].nIdealSize=m_pColInfo[m_colarr[colToResize]].nCurSize;//-=cxIdealNew+m_cySplitter;
	}

	m_pColInfo[new_val].nIdealSize = cxNew;
	m_pColInfo[new_val].nCurSize = cxNew;
    RecalcLayout();
}

void CSplitterWndEx::HideRow(int rowHide,int rowToResize)
{
    ASSERT_VALID(this);
    ASSERT(m_nRows > 1);

	if (m_arr)
		ASSERT(m_arr[rowHide] < m_nRows);

    // if the row has an active window -- change it
    int rowActive, colActive;

	if (!m_arr)
	{
		m_arr = new int[m_nRows];
		for (int i = 0; i < m_nRows; i++)
			m_arr[i] = i;
		m_length = m_nRows;
	}

	if (GetActivePane(&rowActive, &colActive) != NULL &&
        rowActive == rowHide) //colActive == rowHide)
    {
        if (++rowActive >= m_nRows)
			rowActive = 0;
        //SetActivePane(rowActive, colActive);

		SetActivePane(rowActive, colActive);
    }

    // hide all row panes
    for (int col = 0; col < m_nCols; col++)
    {
        CWnd* pPaneHide = CSplitterWnd::GetPane(m_arr[rowHide], col);
        ASSERT(pPaneHide != NULL);
	    pPaneHide->ShowWindow(SW_HIDE);

		for (int row = rowHide + 1; row < m_length; row++)
        {
			if (m_arr[row] < m_nRows )
			{
				CWnd* pPane = CSplitterWnd::GetPane(m_arr[row], col);
				ASSERT(pPane != NULL);
				pPane->SetDlgCtrlID(Id_short(row-1, col));
				m_arr[row]--;
			}
        }
        pPaneHide->SetDlgCtrlID(
			Id_short(m_nRows -1 , col));
    }

	int oldsize=m_pRowInfo[m_arr[rowHide]].nCurSize;
	int oldidealsize=m_pRowInfo[m_arr[rowHide]].nIdealSize;
	for (int row=rowHide;row<(m_length-1);row++)
	{
		if (m_arr[row+1] < m_nRows )
		{
			m_pRowInfo[m_arr[row]].nCurSize=m_pRowInfo[m_arr[row+1]].nCurSize;
			m_pRowInfo[m_arr[row]].nIdealSize=m_pRowInfo[m_arr[row+1]].nCurSize;		
		}
	}
	if (rowToResize!=-1)
	{
		m_pRowInfo[m_arr[rowToResize]].nCurSize+=oldsize+m_cySplitter;
		m_pRowInfo[m_arr[rowToResize]].nIdealSize+=oldsize+m_cySplitter;
		oldsize+=0x10000*(rowToResize+1);
		oldidealsize+=0x10000*(rowToResize+1);
	}

	m_pRowInfo[m_nRows - 1].nCurSize =oldsize;
	m_pRowInfo[m_nRows - 1].nIdealSize =oldsize;
	
	m_arr[rowHide] = m_nRows-1;
	

    m_nRows--;
	RecalcLayout();
}

void CSplitterWndEx::HideCol(int colHide, int colToResize)
{
    ASSERT_VALID(this);
    ASSERT(m_nCols > 1);

	if (m_colarr)
		ASSERT(m_colarr[colHide] < m_nCols);

    // if the col has an active window -- change it
    int colActive, rowActive;

	if (!m_colarr)
	{
		m_colarr = new int[m_nCols];
		for (int i = 0; i < m_nCols; i++)
			m_colarr[i] = i;
		m_collength = m_nCols;
	}

	if (GetActivePane(&rowActive, &colActive) != NULL &&
        colActive == colHide) //rowActive == colHide)
    {
        if (++colActive >= m_nCols)
			colActive = 0;
        //SetActivePane(colActive, rowActive);

		SetActivePane(rowActive, colActive);
    }

    // hide all col panes
    for (int row = 0; row < m_nRows; row++)
    {
        CWnd* pPaneHide = CSplitterWnd::GetPane(row, m_colarr[colHide]);
        ASSERT(pPaneHide != NULL);
	    pPaneHide->ShowWindow(SW_HIDE);

		for (int col = colHide + 1; col < m_collength; col++)
        {
			if (m_colarr[col] < m_nCols )
			{
				CWnd* pPane = CSplitterWnd::GetPane(row, m_colarr[col]);
				ASSERT(pPane != NULL);
				pPane->SetDlgCtrlID(Id_short(row, col-1));
				m_colarr[col]--;
			}
        }
        pPaneHide->SetDlgCtrlID(
			Id_short(row, m_nCols - 1));
    }

	int oldsize = m_pColInfo[m_colarr[colHide]].nCurSize;
	int oldidealsize = m_pColInfo[m_colarr[colHide]].nIdealSize;
	for (int col=colHide;col<(m_collength-1);col++)
	{
		if (m_colarr[col+1] < m_nCols )
		{
			m_pColInfo[m_colarr[col]].nCurSize=m_pColInfo[m_colarr[col+1]].nCurSize;
			m_pColInfo[m_colarr[col]].nIdealSize=m_pColInfo[m_colarr[col+1]].nCurSize;		
		}
	}
	if (colToResize!=-1)
	{
		m_pColInfo[m_colarr[colToResize]].nCurSize+=oldsize+m_cxSplitter;
		m_pColInfo[m_colarr[colToResize]].nIdealSize+=oldsize+m_cxSplitter;
		oldsize+=0x10000*(colToResize+1);
		oldidealsize+=0x10000*(colToResize+1);
	}

	m_pColInfo[m_nCols - 1].nCurSize =oldsize;
	m_pColInfo[m_nCols - 1].nIdealSize =oldsize;
	
	m_colarr[colHide] = m_nCols-1;
	

    m_nCols--;
	RecalcLayout();
}

BEGIN_MESSAGE_MAP(CSplitterWndEx, CSplitterWnd)
//{{AFX_MSG_MAP(CSplitterWndEx)
  // NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CWnd* CSplitterWndEx::GetPane(int row, int col)
{
	if (!m_arr)
	{
		if (!m_colarr)
			return CSplitterWnd::GetPane(row, col);
		else
		{
			ASSERT_VALID(this);

			CWnd* pView = GetDlgItem(IdFromRowCol(row, m_colarr[col]));
			ASSERT(pView != NULL);  // panes can be a CWnd, but are usually CViews
			return pView;
		}
	}
	else
	{
		CWnd* pView;

		ASSERT_VALID(this);
	
		if (!m_colarr)
			pView = GetDlgItem(IdFromRowCol(m_arr[row], col));
		else
			pView = GetDlgItem(IdFromRowCol(m_arr[row], m_colarr[col]));
		ASSERT(pView != NULL);  // panes can be a CWnd, but are usually CViews
		
		return pView;
	}
}

int CSplitterWndEx::IdFromRowCol(int row, int col) const
{
	ASSERT_VALID(this);
	ASSERT(row >= 0);
	ASSERT(row < (m_arr?m_length:m_nRows));
	ASSERT(col >= 0);
	ASSERT(col < (m_colarr?m_collength:m_nCols));

	return AFX_IDW_PANE_FIRST + row * 16 + col;
}

BOOL CSplitterWndEx::IsChildPane(CWnd* pWnd, int* pRow, int* pCol)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pWnd);

	UINT nID = ::GetDlgCtrlID(pWnd->m_hWnd);
	if (IsChild(pWnd) && nID >= AFX_IDW_PANE_FIRST && nID <= AFX_IDW_PANE_LAST)
	{
		if (pWnd->GetParent()!=this)
			return FALSE;
		if (pRow != NULL)
			*pRow = (nID - AFX_IDW_PANE_FIRST) / 16;
		if (pCol != NULL)
			*pCol = (nID - AFX_IDW_PANE_FIRST) % 16;
		ASSERT(pRow == NULL || *pRow < (m_arr?m_length:m_nRows));
		ASSERT(pCol == NULL || *pCol < (m_colarr?m_collength:m_nCols));
		return TRUE;
	}
	else
	{
		if (pRow != NULL)
			*pRow = -1;
		if (pCol != NULL)
			*pCol = -1;
		return FALSE;
	}
}

CWnd* CSplitterWndEx::GetActivePane(int* pRow, int* pCol)
	// return active view, NULL when no active view
{
	ASSERT_VALID(this);

	// attempt to use active view of frame window
	CWnd* pView = NULL;
	CFrameWnd* pFrameWnd = GetParentFrame();
	ASSERT_VALID(pFrameWnd);
	pView = pFrameWnd->GetActiveView();

	// failing that, use the current focus
	if (pView == NULL)
		pView = GetFocus();

	// make sure the pane is a child pane of the splitter
	if (pView != NULL && !IsChildPane(pView, pRow, pCol))
		pView = NULL;

	return pView;
}

BOOL CSplitterWndEx::IsRowHidden(int row)
{
	return m_arr[row]>=m_nRows;
}

BOOL CSplitterWndEx::IsColHidden(int col)
{
	return m_arr[col]>=m_nCols;
}

void CSplitterWndEx::GetRowInfoEx(int row, int &cyCur, int &cyMin)
{
	if (!m_arr)
		GetRowInfo(row,cyCur,cyMin);
	else
	{
		if (m_pRowInfo[m_arr[row]].nCurSize>0x10000)
			cyCur=m_pRowInfo[m_arr[row]].nCurSize/0x10000;
		else
			cyCur=m_pRowInfo[m_arr[row]].nCurSize%0x10000;
		cyMin=0;
	}
}

void CSplitterWndEx::GetColumnInfoEx(int col, int &cxCur, int &cxMin)
{
	if (!m_colarr)
		GetColumnInfo(col, cxCur, cxMin);
	else
	{
		if (m_pColInfo[m_colarr[col]].nCurSize > 0x10000)
			cxCur = m_pColInfo[m_colarr[col]].nCurSize/0x10000;
		else
			cxCur = m_pColInfo[m_colarr[col]].nCurSize%0x10000;
		cxMin=0;
	}
}

