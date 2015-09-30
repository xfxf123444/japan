#if !defined(AFX_CONFIRM_H__820C60FA_03E1_4A5E_A584_B93405D8C500__INCLUDED_)
#define AFX_CONFIRM_H__820C60FA_03E1_4A5E_A584_B93405D8C500__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CConfirm dialog

class CConfirm : public CDialog
{
// Construction
public:
	CConfirm(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfirm)
	enum { IDD = IDD_DELETE_CONFIRM };
	CString	m_szSelTar;
	CString	m_method;
	//}}AFX_DATA
	BOOL		m_bDisk;
	int			m_nDisk;
	TCHAR		m_DriveLetter;
	DWORD		m_dwMinSec,m_dwMaxSec;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirm)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIRM_H__820C60FA_03E1_4A5E_A584_B93405D8C500__INCLUDED_)
