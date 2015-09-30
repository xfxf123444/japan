// FileEncrypt.h : main header file for the FILEENCRYPT application
//

#if !defined(AFX_FILEENCRYPT_H__3CBFA299_8A84_41CB_AB9A_454485EEBE59__INCLUDED_)
#define AFX_FILEENCRYPT_H__3CBFA299_8A84_41CB_AB9A_454485EEBE59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFileEncryptApp:
// See FileEncrypt.cpp for the implementation of this class
//

class CFileEncryptApp : public CWinApp
{
public:
	CFileEncryptApp();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileEncryptApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFileEncryptApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEENCRYPT_H__3CBFA299_8A84_41CB_AB9A_454485EEBE59__INCLUDED_)
