// DragListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "filedecrypt.h"
#include "DragListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDragListCtrl

CDragListCtrl::CDragListCtrl()
{
}

CDragListCtrl::~CDragListCtrl()
{
}


BEGIN_MESSAGE_MAP(CDragListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CDragListCtrl)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDragListCtrl message handlers

void CDragListCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
