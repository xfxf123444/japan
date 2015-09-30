#if !defined(AFX_SCHEDULEWIZARD_H__31FED081_3BC9_45CA_B28C_E2A85FEF3A27__INCLUDED_)
#define AFX_SCHEDULEWIZARD_H__31FED081_3BC9_45CA_B28C_E2A85FEF3A27__INCLUDED_

#include "stdafx.h"
#include "..\AM01STRUCT\AM01Struct.h"	// Added by ClassView


/////////////////////////////////////////////////////////////////////////////
// CScheduleWizard

class CScheduleWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CScheduleWizard)

// Construction
public:
	CScheduleWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CScheduleWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScheduleWizard)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScheduleWizard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CScheduleWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULEWIZARD_H__31FED081_3BC9_45CA_B28C_E2A85FEF3A27__INCLUDED_)
