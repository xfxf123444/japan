// WZStep4.h : header file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#if !defined(AFX_WZSTEP4_H__99E7B195_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_)
#define AFX_WZSTEP4_H__99E7B195_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CWZStep4 dialog

class CWZStep4 : public CPropertyPageEx
{
	DECLARE_DYNCREATE(CWZStep4)

// Construction
public:
	CWZStep4();
	~CWZStep4();

// Dialog Data
	//{{AFX_DATA(CWZStep4)
	enum { IDD = IDD_WZSTEP4 };
	CString	m_TargetPath;
	CString	m_Comment;
	BOOL	m_bLogFail;
	DWORD	m_dwMinutes;
	BOOL	m_bRetry;
	BOOL	m_bAutoSpawn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWZStep4)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWZStep4)
	afx_msg void OnRefer();
	virtual BOOL OnInitDialog();
	afx_msg void OnEncrypt();
	afx_msg void OnRetry();
	afx_msg void OnLogFail();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_strPassword;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WZSTEP4_H__99E7B195_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_)
