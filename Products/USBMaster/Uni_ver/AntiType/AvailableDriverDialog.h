#if !defined(AFX_AVAILABLEDRIVERDIALOG_H__5EF68379_F6F9_46A4_89C6_66784BEEE7EA__INCLUDED_)
#define AFX_AVAILABLEDRIVERDIALOG_H__5EF68379_F6F9_46A4_89C6_66784BEEE7EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AvailableDriverDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAvailableDriverDialog dialog

extern CString driverName;
extern CString mirrorDriverName;

typedef struct
{
	BYTE password[32];
	BYTE keyLen;
	BYTE defaultName;
	BYTE reserved[30];
}SAVE_INFO,*PSAVE_INFO;

class CAvailableDriverDialog : public CDialog
{
// Construction
public:
	CAvailableDriverDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAvailableDriverDialog)
	enum { IDD = IDD_AVAILABLE_DRIVER_DIALOG };
	CListCtrl	m_availableDriverList;
	CString	m_mountCheckPassword;
	BOOL	m_checkSetDefaultDriver;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvailableDriverDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAvailableDriverDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVAILABLEDRIVERDIALOG_H__5EF68379_F6F9_46A4_89C6_66784BEEE7EA__INCLUDED_)
