// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__94F6631B_351B_46DA_B773_777ED362445E__INCLUDED_)
#define AFX_STDAFX_H__94F6631B_351B_46DA_B773_777ED362445E__INCLUDED_

#ifndef WINVER				
#define WINVER 0x0502
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>

#include "tchar.h"

#pragma warning(disable: 4996)

#endif // !defined(AFX_STDAFX_H__94F6631B_351B_46DA_B773_777ED362445E__INCLUDED_)
