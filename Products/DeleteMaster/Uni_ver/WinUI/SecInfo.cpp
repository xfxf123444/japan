// SecInfo.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "SecInfo.h"
#include "Fun.h"


//extern  CFont				g_Font;
extern  BYTE				g_btSecBuf[BYTEINSEC];

/////////////////////////////////////////////////////////////////////////////
// CSecInfo

CSecInfo::CSecInfo(CWnd	*pSecViewWnd,CRect rect)
{
//	rect.top	+= 20;
	rect.left	+= 10;
	rect.right	-= 10;
//	rect.bottom = rect.bottom * 5 / 6;
	CreateEx(WS_EX_CLIENTEDGE,
			 DMSecWinClass,
			 _T("SecInfo"),
			 WS_CHILD| WS_VISIBLE|WS_VSCROLL ,
			 rect,pSecViewWnd,0,NULL);
}

CSecInfo::~CSecInfo()
{
}


BEGIN_MESSAGE_MAP(CSecInfo, CWnd)
	//{{AFX_MSG_MAP(CSecInfo)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSecInfo message handlers

int CSecInfo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CRect		rect;
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	GetClientRect(&rect);
	m_nPage  = rect.bottom/12;
	m_nCurLine  = 0;
	SetScrollRange( SB_VERT,0, ((32*12) - rect.bottom)/12 +1);
	return 0;
}

void CSecInfo::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	int			i,j;
	int			nBufOffset = m_nCurLine*16;
	char		szitem[80];
	char		szch;

	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = -12;
	lf.lfWidth  = 7;
	lf.lfPitchAndFamily = FIXED_PITCH;
	_tcscpy(lf.lfFaceName, _T("Roman"));
	VERIFY(font.CreateFontIndirect(&lf));
	CFont* def_font = dc.SelectObject(&font);
	
	//COLORREF color;
	dc.SetBkMode(TRANSPARENT);
	//dc.SelectObject (&g_Font);

 	for(i=m_nCurLine;i<32;i++)
 	{
		sprintf(szitem, SECTORINFO, nBufOffset,
				g_btSecBuf[nBufOffset],g_btSecBuf[nBufOffset+1],
 				g_btSecBuf[nBufOffset+2],g_btSecBuf[nBufOffset+3],
 				g_btSecBuf[nBufOffset+4],g_btSecBuf[nBufOffset+5],
 				g_btSecBuf[nBufOffset+6],g_btSecBuf[nBufOffset+7],
 				g_btSecBuf[nBufOffset+8],g_btSecBuf[nBufOffset+9],
 				g_btSecBuf[nBufOffset+10],g_btSecBuf[nBufOffset+11],
 				g_btSecBuf[nBufOffset+12],g_btSecBuf[nBufOffset+13],
 				g_btSecBuf[nBufOffset+14],g_btSecBuf[nBufOffset+15]);
 		for(j=0;j<16;j++)
 		{
 			szch = g_btSecBuf[nBufOffset+j];
 			if(	
 				
 				szch == 0x0a || 
 				szch == 0x0d ||
				szch == 0x08 ||
				szch == 0x07 ||
				szch == 0x1a
 				
			  )
 			{
 				//szch = szch;
 				szch = 0x2e;
 			}
 			if(szch == 0) szch = 0x2e;
 			sprintf(szitem,"%s%c",szitem,szch);
 		}
		TCHAR temp[MAX_PATH];
		ZeroMemory(temp, MAX_PATH * sizeof(TCHAR));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szitem, 80, temp, MAX_PATH);
 		//dc.TextOut (0,(i-m_nCurLine)*12,szitem);
		dc.TextOut (0,(i-m_nCurLine)*12,temp);
 		nBufOffset += 16;
 	}
	dc.SelectObject(def_font);

	// Done with the font. Delete the font object.
	font.DeleteObject();
	// Do not call CWnd::OnPaint() for painting messages
}

void CSecInfo::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	SCROLLINFO      si;

	GetScrollInfo( SB_VERT, &si, SIF_ALL);

	switch(nSBCode)
	{
	case SB_LINEUP:
		if(m_nCurLine != si.nMin)
		{
			m_nCurLine --;
			SetScrollPos( SB_VERT, m_nCurLine);
			ScrollWindow(0,12);
		}
		break;
	case SB_LINEDOWN:
		if(m_nCurLine != si.nMax)
		{
			m_nCurLine ++;
			SetScrollPos( SB_VERT, m_nCurLine);
			ScrollWindow(0,-12);
		}
		break;
	case SB_PAGEUP:
		if(m_nCurLine != si.nMin)
		{
			m_nCurLine -= m_nPage;
			if(m_nCurLine < si.nMin)
				m_nCurLine = si.nMin;
			SetScrollPos( SB_VERT, m_nCurLine);
			ScrollWindow(0,12*si.nMax);
		}
		break;
	case SB_PAGEDOWN:
		if(m_nCurLine != si.nMax)
		{
			m_nCurLine += m_nPage;
			if(m_nCurLine > si.nMax)
				m_nCurLine = si.nMax;
			SetScrollPos( SB_VERT, m_nCurLine);
			ScrollWindow(0,-12*m_nCurLine);
		}
		break;
	case SB_ENDSCROLL:
		//nPos += 12;
		break;
	case SB_BOTTOM:
		//nPos += 12;
		break;
	case SB_TOP:
		//nPos += 12;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		SetScrollPos( SB_VERT, nPos);
		ScrollWindow(0,12*(m_nCurLine-nPos));
		m_nCurLine = nPos;
		break;
	}
	//ScrollWindow(0,-12);
	UpdateWindow();
	//SetScrollPos( SB_VERT, nPos+1);
	//CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}
