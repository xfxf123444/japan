// AM01DLL.h : main header file for the AM01DLL DLL
//

#if !defined(AFX_AM01DLL_H__21B043A2_B1B9_45B9_B93D_FC8E57D89BFC__INCLUDED_)
#define AFX_AM01DLL_H__21B043A2_B1B9_45B9_B93D_FC8E57D89BFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAM01DLLApp
// See AM01DLL.cpp for the implementation of this class
//

class CAM01DLLApp : public CWinApp
{
public:
	CAM01DLLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAM01DLLApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAM01DLLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AM01DLL_H__21B043A2_B1B9_45B9_B93D_FC8E57D89BFC__INCLUDED_)
