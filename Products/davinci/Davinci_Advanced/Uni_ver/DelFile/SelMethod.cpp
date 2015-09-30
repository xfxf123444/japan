// SelMethod.cpp : implementation file
//

#include "stdafx.h"
#include "DelFile.h"
#include "SelMethod.h"
#include "Fun.h"

extern	int			g_nDelMethod;
extern  int			g_nSelItem;
extern  WCHAR		g_szDir[MAX_PATH][MAX_PATH];
extern  BOOL        g_bBeepOff;
extern	BOOL		g_bDeleteObjMark;

/////////////////////////////////////////////////////////////////////////////
// CSelMethod dialog


CSelMethod::CSelMethod(CWnd* pParent /*=NULL*/)
	: CDialog(CSelMethod::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelMethod)
	m_csDelString = _T("");
	m_bBeepOff = FALSE;
	//}}AFX_DATA_INIT
}


void CSelMethod::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelMethod)
	DDX_Control(pDX, IDC_DELETE_METHOD, m_SelMethod);
	DDX_Text(pDX, IDC_DEL_STRING, m_csDelString);
	DDX_Check(pDX, IDC_BEEP_OFF, m_bBeepOff);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelMethod, CDialog)
	//{{AFX_MSG_MAP(CSelMethod)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelMethod message handlers

BOOL CSelMethod::OnInitDialog() 
{
	CDialog::OnInitDialog();
	WCHAR	*p;
	
	AddDelMethod(&m_SelMethod,3);

	if (g_bDeleteObjMark)
	{
		UpdateData(FALSE);
		return TRUE;
	}

	if(g_nSelItem >1)
	{
		m_csDelString.Format(IDS_DELETE_CONFIRM_ITEMS,g_nSelItem);
	}
	else
	{
		p = wcsrchr(g_szDir[0],L'\\');
		if(p)
		{
			p++;
			m_csDelString.Format(IDS_DELETE_CONFIRM_ITEM,p);
		}
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CSelMethod::OnOK() 
{
	int nSel;
	// TODO: Add extra validation here
	UpdateData(TRUE);
	g_bBeepOff = m_bBeepOff;
	nSel = m_SelMethod.GetCurSel ();
	g_nDelMethod = m_SelMethod.GetItemData(nSel);
	CDialog::OnOK();
}
