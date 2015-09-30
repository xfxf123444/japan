#if !defined(AFX_DISKINFO_H__21703A63_9B67_4503_845B_FF0ADC1E624A__INCLUDED_)
#define AFX_DISKINFO_H__21703A63_9B67_4503_845B_FF0ADC1E624A__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CDiskInfo dialog

class CDiskInfo : public CDialog
{
// Construction
public:
	CDiskInfo(CWnd* pParent = NULL);   // standard constructor

	BYTE					m_btDisk;
// Dialog Data
	//{{AFX_DATA(CDiskInfo)
	enum { IDD = IDD_HARD_DISK_INFORMATION };
	CString	m_ModuleNumber;
	CString	m_FirmWare;
	CString	m_SerNumber;
	CString	m_Capacity;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiskInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDiskInfo)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISKINFO_H__21703A63_9B67_4503_845B_FF0ADC1E624A__INCLUDED_)
