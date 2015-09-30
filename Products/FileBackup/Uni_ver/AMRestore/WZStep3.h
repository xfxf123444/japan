// WZStep3.h : header file
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

#if !defined(AFX_WZSTEP3_H__99E7B193_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_)
#define AFX_WZSTEP3_H__99E7B193_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// Interior2 dialog

class CWZStep3 : public CPropertyPageEx
{
	DECLARE_DYNCREATE(CWZStep3)

// Construction
public:
	void InitControl();
	CWZStep3();
	~CWZStep3();

// Dialog Data
	//{{AFX_DATA(CWZStep3)
	enum { IDD = IDD_WZSTEP3 };
	int		m_PathType;
	CString	m_TargetPath;
	CString m_DefaultPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWZStep3)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWZStep3)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRefer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WZSTEP3_H__99E7B193_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_)
