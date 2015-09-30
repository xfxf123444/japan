#if !defined(AFX_EASYWIZARD_H__3ECA279E_0FE1_4E39_8C0A_9F4FD2ACCAE6__INCLUDED_)
#define AFX_EASYWIZARD_H__3ECA279E_0FE1_4E39_8C0A_9F4FD2ACCAE6__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CEasyWizard

class CEasyWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CEasyWizard)

// Construction
public:
	CEasyWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CEasyWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEasyWizard)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEasyWizard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEasyWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EASYWIZARD_H__3ECA279E_0FE1_4E39_8C0A_9F4FD2ACCAE6__INCLUDED_)
