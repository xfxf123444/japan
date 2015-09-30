// FDimg.h : main header file for the FDIMG application
//

#if !defined(AFX_FDIMG_H__DEC080F7_F401_47EC_9464_5B585270324D__INCLUDED_)
#define AFX_FDIMG_H__DEC080F7_F401_47EC_9464_5B585270324D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFDimgApp:
// See FDimg.cpp for the implementation of this class
//

class CFDimgApp : public CWinApp
{
public:
	CFDimgApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFDimgApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFDimgApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FDIMG_H__DEC080F7_F401_47EC_9464_5B585270324D__INCLUDED_)
