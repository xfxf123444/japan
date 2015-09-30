#if !defined(AFX_SEARCHFORM_H__EFCD6AC2_95CF_4054_A9FC_9B5B51C28449__INCLUDED_)
#define AFX_SEARCHFORM_H__EFCD6AC2_95CF_4054_A9FC_9B5B51C28449__INCLUDED_

#include "StdAfx.h"
class CAMFindDoc;

/////////////////////////////////////////////////////////////////////////////
// CSearchForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "..\AM01Struct\AM01Struct.h"

class CSearchForm : public CFormView
{
protected:
	CSearchForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSearchForm)

// Form Data
public:
	//{{AFX_DATA(CSearchForm)
	enum { IDD = IDD_SEARCH_FORM };
	CSpinButtonCtrl	m_SpinSize;
	CComboBox	m_SizeType;
	CComboBox	m_TimeStamp;
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
	void OnMyFileOpen();
	CAMFindDoc* GetDocument();
	HANDLE m_hThreadHandle;
	AMFINDSTRUCT m_FindParam;

// Operations
public:
	void LoadTimeStamp(LPCTSTR szImageFile);
	WCHAR m_szTempDir[MAX_PATH];
	BOOL FreshTimeStamp(WCHAR *szImgFile);
	void EnableControl(BOOL bEnable);
	BOOL m_bSearch;
	BOOL CheckConflict();
	void InitFileType();
	void InitControl();
	static	DWORD WINAPI ThreadProc(PAMFINDSTRUCT pPara);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchForm)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSearchForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CSearchForm)
	afx_msg void OnCheckDate();
	afx_msg void OnCheckType();
	afx_msg void OnCheckSize();
	afx_msg void OnSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AMFindView.cpp
inline CAMFindDoc* CSearchForm::GetDocument()
   { return (CAMFindDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHFORM_H__EFCD6AC2_95CF_4054_A9FC_9B5B51C28449__INCLUDED_)
