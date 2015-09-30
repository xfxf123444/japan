#if !defined(AFX_REMOVEFOLDER_H__231E4729_DF63_4B60_96F2_3C64B987150A__INCLUDED_)
#define AFX_REMOVEFOLDER_H__231E4729_DF63_4B60_96F2_3C64B987150A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RemoveFolder.h : header file
//
#include "..\ENCRYPTTOOLSTRUCT\EncryptToolStruct.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CRemoveFolder dialog

class CRemoveFolder : public CDialog
{
// Construction
public:
	MONITOR_ITEM m_MonitorItem;
	CRemoveFolder(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRemoveFolder)
	enum { IDD = IDD_REMOVE_OPTION };
	BOOL	m_bDecryptFile;
	CString	m_Directory;
	CString	m_Password;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoveFolder)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRemoveFolder)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOVEFOLDER_H__231E4729_DF63_4B60_96F2_3C64B987150A__INCLUDED_)
