#if !defined(AFX_AMABOUT_H__28111781_410F_11D3_A638_5254AB10413A__INCLUDED_)
#define AFX_AMABOUT_H__28111781_410F_11D3_A638_5254AB10413A__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CAMAbout dialog

class CAMAbout : public CPropertyPage
{
	DECLARE_DYNCREATE(CAMAbout)

// Construction
public:
	CAMAbout();
	~CAMAbout();
// Dialog Data
	//{{AFX_DATA(CAMAbout)
	enum { IDD = IDD_AMABOUT };
	CString	m_Information;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAMAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAMAbout)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMABOUT_H__28111781_410F_11D3_A638_5254AB10413A__INCLUDED_)
