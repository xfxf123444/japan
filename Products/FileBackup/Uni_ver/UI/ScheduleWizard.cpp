// ScheduleWizard.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "ScheduleWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CScheduleWizard

IMPLEMENT_DYNAMIC(CScheduleWizard, CPropertySheet)

CScheduleWizard::CScheduleWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CScheduleWizard::CScheduleWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CScheduleWizard::~CScheduleWizard()
{
}


BEGIN_MESSAGE_MAP(CScheduleWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CScheduleWizard)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScheduleWizard message handlers
