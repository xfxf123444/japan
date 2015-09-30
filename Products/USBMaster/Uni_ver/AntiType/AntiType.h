// AntiType.h : main header file for the ANTITYPE application
//

#if !defined(AFX_ANTITYPE_H__02C7E4D5_279D_4B73_B5FF_F743E507652F__INCLUDED_)
#define AFX_ANTITYPE_H__02C7E4D5_279D_4B73_B5FF_F743E507652F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "..\..\..\..\Tech\ParInfo\CUR_VER\EXPORT\2000\ParInfo.h"
#include "..\..\..\..\Tech\ParMan\CUR_VER\EXPORT\2000\ParMan.h"
#include "..\..\..\..\Tech\Lib\crypto\Cur_ver\Export\Crypto.h"
#include "..\..\..\..\Tech\FileLoc\Cur_Ver\Export\2000\YGFLoc.h"
#include "..\..\..\..\Tech\FileLoc\Cur_Ver\Export\2000\Fileloc.h"
#include "..\..\..\..\Tech\YGCryDisk\Cur_ver\Export\2000\YGCryDisk.h"

BOOL NormalKeyToDESKey(char *szKey,UCHAR *ucDesKey);

/////////////////////////////////////////////////////////////////////////////
// CAntiTypeApp:
// See AntiType.cpp for the implementation of this class
//

class CAntiTypeApp : public CWinApp
{
public:
	CAntiTypeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAntiTypeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAntiTypeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANTITYPE_H__02C7E4D5_279D_4B73_B5FF_F743E507652F__INCLUDED_)
