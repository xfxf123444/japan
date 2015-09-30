#ifndef YG_FS_MON_SETUP_H
#define YG_FS_MON_SETUP_H
// Setup.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CSetup

class CSetup : public CPropertySheet
{
	DECLARE_DYNAMIC(CSetup)

// Construction
public:
	CSetup(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSetup(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDetails)
	public:  
	virtual BOOL OnInitDialog();  
	//}}AFX_VIRTUAL
// Implementation
public:
	virtual ~CSetup();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSetup)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
#endif