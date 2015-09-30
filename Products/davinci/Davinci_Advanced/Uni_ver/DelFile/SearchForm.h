#if !defined(AFX_SEARCHFORM_H__EFCD6AC2_95CF_4054_A9FC_9B5B51C28449__INCLUDED_)
#define AFX_SEARCHFORM_H__EFCD6AC2_95CF_4054_A9FC_9B5B51C28449__INCLUDED_

#include "StdAfx.h"

#define LAST_MODIFY_TIME   1
#define CREATE_TIME        2
#define LAST_ACCESS_TIME   3

#define MAXSIZE            1
#define MINSIZE            2

typedef struct
{
	HWND hParWnd;
	WCHAR szSearchPath[MAX_PATH];
	WCHAR szNameMatch[MAX_PATH];
	BOOL  bCheckDate;
	DWORD dwDateType;
	CTime TimeStart;
	CTime TimeEnd;
	BOOL  bCheckType;
	WCHAR  szFileExt[MAX_PATH];
	BOOL  bCheckSize;
	DWORD dwSizeType;
	DWORD dwSize;
}DMFINDSTRUCT,*PDMFINDSTRUCT;

/////////////////////////////////////////////////////////////////////////////
// CSearchForm form

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CSearchForm : public CDialog
{
public:
	CSearchForm(CWnd* pParent = NULL);   // standard constructor

// Form Data
public:
	//{{AFX_DATA(CSearchForm)
	enum { IDD = IDD_SEARCH_FORM };
	CComboBox	m_SearchPath;
	CSpinButtonCtrl	m_SpinSize;
	CComboBox	m_SizeType;
	CComboBox	m_FileType;
	CComboBox	m_DateType;
	BOOL	m_CheckDate;
	BOOL	m_CheckSize;
	BOOL	m_CheckType;
	CString	m_ContainChar;
	CTime	m_DateStart;
	CTime	m_DateEnd;
	CString	m_SearchName;
	DWORD	m_FileSize;
	//}}AFX_DATA

// Attributes
public:
	HANDLE m_hThreadHandle;
	DMFINDSTRUCT m_FindParam;

// Operations
public:
	void InitPathComobo();
	CString m_strSearchPath;
	void LoadTimeStamp(LPCTSTR szImageFile);
	char m_szTempDir[MAX_PATH];
	BOOL FreshTimeStamp(char *szImgFile);
	void EnableControl(BOOL bEnable);
	BOOL m_bSearch;
	BOOL CheckConflict();
	void InitFileType();
	void InitControl();
	static	DWORD WINAPI ThreadProc(PDMFINDSTRUCT pPara);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchForm)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSearchForm)
	afx_msg void OnCheckDate();
	afx_msg void OnCheckType();
	afx_msg void OnCheckSize();
	afx_msg void OnSearch();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHFORM_H__EFCD6AC2_95CF_4054_A9FC_9B5B51C28449__INCLUDED_)
