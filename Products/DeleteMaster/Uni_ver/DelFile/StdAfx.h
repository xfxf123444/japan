// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__6B81B1B9_91D1_4F3E_82F4_72717009CA25__INCLUDED_)
#define AFX_STDAFX_H__6B81B1B9_91D1_4F3E_82F4_72717009CA25__INCLUDED_

#ifndef WINVER				
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501	
#endif						

#ifndef _WIN32_WINDOWS		
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE			
#define _WIN32_IE 0x0600
#endif

#define WM_REFRESH_ITEM  WM_USER + 201
#define WM_UPDATE_VIEW   WM_USER + 202

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define	DM_DEL_SIZE			65536
#define BYTEINSEC			512

#define RENAME_STRING		L"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"

#pragma warning(disable: 4996)

#endif // !defined(AFX_STDAFX_H__6B81B1B9_91D1_4F3E_82F4_72717009CA25__INCLUDED_)
