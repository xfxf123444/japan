// IntroPage.h : header file
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

#if !defined(AFX_WZSTEP1_H__99E7B192_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_)
#define AFX_WZSTEP1_H__99E7B192_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_

#include "stdafx.h"
/////////////////////////////////////////////////////////////////////////////
// CWZStep1 dialog

class CWZStep1 : public CPropertyPageEx
{
	DECLARE_DYNCREATE(CWZStep1)

// Construction
public:
	CWZStep1();
	~CWZStep1();

// Dialog Data
	//{{AFX_DATA(CWZStep1)
	enum { IDD = IDD_WZSTEP1 };
	CString	m_szSettingName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWZStep1)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWZStep1)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WZSTEP1_H__99E7B192_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_)
