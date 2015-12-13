// DeleteMaster.h : main header file for the DELETEMASTER application
//

#if !defined(AFX_DELETEMASTER_H__FE637539_AC7D_48D0_AA17_9317B1504BCE__INCLUDED_)
#define AFX_DELETEMASTER_H__FE637539_AC7D_48D0_AA17_9317B1504BCE__INCLUDED_

#include "StdAfx.h"
#include "resource.h"       // main symbols


//#define WIPE_ONLY

BOOL IsUserAdmin(VOID) ;
/////////////////////////////////////////////////////////////////////////////
// CDeleteMasterApp:
// See DeleteMaster.cpp for the implementation of this class
//

class CDeleteMasterApp : public CWinApp
{
public:
	CDeleteMasterApp();

	BOOL RegClass();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteMasterApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDeleteMasterApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETEMASTER_H__FE637539_AC7D_48D0_AA17_9317B1504BCE__INCLUDED_)
