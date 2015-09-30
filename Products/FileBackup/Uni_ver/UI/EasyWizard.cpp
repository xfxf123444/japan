// EasyWizard.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "EasyWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEasyWizard

IMPLEMENT_DYNAMIC(CEasyWizard, CPropertySheet)

CEasyWizard::CEasyWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CEasyWizard::CEasyWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CEasyWizard::~CEasyWizard()
{
}


BEGIN_MESSAGE_MAP(CEasyWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CEasyWizard)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEasyWizard message handlers
