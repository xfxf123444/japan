#if !defined(AFX_DELPROC_H__48F38550_450D_4496_A6A9_22BD3EF60354__INCLUDED_)
#define AFX_DELPROC_H__48F38550_450D_4496_A6A9_22BD3EF60354__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CDelProc dialog

class CDelProc : public CDialog
{
// Construction
public:
	BOOL m_bDeleteObjMark;
	CDelProc(CWnd* pParent = NULL);   // standard constructor
	BOOL DoDeleteFileInRecycled(WCHAR *pszDir,BOOL bDel);
	BOOL DeleteFileInRecycled(WCHAR *pszDir,BOOL bDel);
	BOOL DelAllInRecycled(BOOL bDel);
	BOOL DoDeleteMasterDelFileInRecycled();

// Dialog Data
	//{{AFX_DATA(CDelProc)
	enum { IDD = IDD_DEL_PROGRESS };
	CStatic	m_Status;
	CStatic	m_BackupFile;
	CButton	m_CancelBut;
	CButton	m_OkBut;
	CProgressCtrl	m_Progress;
	//}}AFX_DATA
	HANDLE				m_Thread;
	BYTE				*m_pBuf;

	BOOL	DoDelFile(WCHAR  *pszDir,BOOL bDel);
	BOOL	DelDirAndFiles(WCHAR  *pszDir,BOOL bDel,WCHAR  *pszSaveDir,BOOL bSave);
	BOOL	DoDeleteMasterDel();
	BOOL	DelAll(BOOL bDel);
	static	DWORD WINAPI ThreadProc(CDelProc* dlg);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelProc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDelProc)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELPROC_H__48F38550_450D_4496_A6A9_22BD3EF60354__INCLUDED_)
