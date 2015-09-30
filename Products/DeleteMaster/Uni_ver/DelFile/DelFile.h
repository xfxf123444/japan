// DelFile.h : main header file for the DELFILE application
//

#if !defined(AFX_DELFILE_H__7E533E85_D47E_450B_BF26_0D4BF9A0C500__INCLUDED_)
#define AFX_DELFILE_H__7E533E85_D47E_450B_BF26_0D4BF9A0C500__INCLUDED_

#include "StdAfx.h"
#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDelFileApp:
// See DelFile.cpp for the implementation of this class
//

class CDelFileApp : public CWinApp
{
public:
	CDelFileApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelFileApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDelFileApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELFILE_H__7E533E85_D47E_450B_BF26_0D4BF9A0C500__INCLUDED_)
