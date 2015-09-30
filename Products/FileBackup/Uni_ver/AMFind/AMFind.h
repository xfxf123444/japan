// AMFind.h : main header file for the AMFIND application
//

#if !defined(AFX_AMFIND_H__F290A08C_7E98_4AF7_BE0A_C203EB5A9CB8__INCLUDED_)
#define AFX_AMFIND_H__F290A08C_7E98_4AF7_BE0A_C203EB5A9CB8__INCLUDED_

#include "StdAfx.h"

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define LAST_MODIFY_TIME   1
#define CREATE_TIME        2
#define LAST_ACCESS_TIME   3

#define MAXSIZE            1
#define MINSIZE            2

typedef struct
{
	HWND hParWnd;
	CListCtrl *plistView;
	WCHAR szNameMatch[MAX_PATH];
	WCHAR szContainChar[MAX_PATH];
	DWORD dwTimeStamp;
	BOOL  bCheckDate;
	DWORD dwDateType;
	CTime TimeStart;
	CTime TimeEnd;
	BOOL  bCheckType;
	WCHAR  szFileExt[MAX_PATH];
	BOOL  bCheckSize;
	DWORD dwSizeType;
	DWORD dwSize;
}AMFINDSTRUCT,*PAMFINDSTRUCT;

BOOL GetFileTypeDesc(WCHAR *szFileExt,WCHAR *szDesc);
BOOL SelectFile(WCHAR *szFileExt,WCHAR *szFileType,WCHAR *szFile);
// BOOL SelectFolder(HWND hWnd,char *ach);

/////////////////////////////////////////////////////////////////////////////
// CAMFindApp:
// See AMFind.cpp for the implementation of this class
//

class CAMFindApp : public CWinApp
{
public:
	CAMFindApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAMFindApp)
	public:
	virtual BOOL InitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CAMFindApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMFIND_H__F290A08C_7E98_4AF7_BE0A_C203EB5A9CB8__INCLUDED_)
