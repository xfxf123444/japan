// SerCheck.h : main header file for the SERCHECK DLL
//

#if !defined(AFX_SERCHECK_H__30BF4ACF_7DB2_4641_A62D_DC2AA509DDDB__INCLUDED_)
#define AFX_SERCHECK_H__30BF4ACF_7DB2_4641_A62D_DC2AA509DDDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
BOOL NumToText(DWORD dwNum,char *szText);


/////////////////////////////////////////////////////////////////////////////
// CSerCheckApp
// See SerCheck.cpp for the implementation of this class
//

class CSerCheckApp : public CWinApp
{
public:
	CSerCheckApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerCheckApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSerCheckApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERCHECK_H__30BF4ACF_7DB2_4641_A62D_DC2AA509DDDB__INCLUDED_)
