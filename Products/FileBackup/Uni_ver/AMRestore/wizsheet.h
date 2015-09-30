// WizSheet.h : header file
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

#if !defined(AFX_WZSHEET_H__99E7B18F_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_)
#define AFX_WZSHEET_H__99E7B18F_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_

#include "stdafx.h"

#include "WZStep1.h"
#include "WZStep2.h"
#include "WZStep3.h"
#include "WZStep4.h"

/////////////////////////////////////////////////////////////////////////////
// CWizSheet

class CWizSheet : public CPropertySheetEx
{
	DECLARE_DYNAMIC(CWizSheet)

// Construction
public:
	CWizSheet(UINT nIDCaption, CWnd* pParentWnd = NULL,
			UINT iSelectPage = 0, HBITMAP hbmWatermark = NULL,
			HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);
	CWizSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL,
			UINT iSelectPage = 0, HBITMAP hbmWatermark = NULL,
			HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);

// Attributes
public:
	CWZStep1 m_WZStep1;
	CWZStep2 m_WZStep2;
	CWZStep3 m_WZStep3;
	CWZStep4 m_WZStep4;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWizSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWizSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWizSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WZSHEET_H__99E7B18F_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_)
