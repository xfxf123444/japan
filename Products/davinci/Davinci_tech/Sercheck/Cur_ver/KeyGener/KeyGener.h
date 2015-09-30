// KeyGener.h : main header file for the KEYGENER application
//

#if !defined(AFX_KEYGENER_H__9253CCFF_B8EE_46C1_8B5E_DD6F6C1A7CD3__INCLUDED_)
#define AFX_KEYGENER_H__9253CCFF_B8EE_46C1_8B5E_DD6F6C1A7CD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
BOOL SelectFile(char *szFileExt,char *szFileType,char *szFile);

/////////////////////////////////////////////////////////////////////////////
// CKeyGenerApp:
// See KeyGener.cpp for the implementation of this class
//

class CKeyGenerApp : public CWinApp
{
public:
	CKeyGenerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyGenerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CKeyGenerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYGENER_H__9253CCFF_B8EE_46C1_8B5E_DD6F6C1A7CD3__INCLUDED_)
