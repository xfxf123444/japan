#if !defined(AFX_MAKEFD_H__9630CA18_8266_482B_9B2E_8D6E46A3D82E__INCLUDED_)
#define AFX_MAKEFD_H__9630CA18_8266_482B_9B2E_8D6E46A3D82E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MakeFD.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMakeFD dialog

class CMakeFD : public CDialog
{
// Construction
public:
	CMakeFD(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMakeFD)
	enum { IDD = IDD_MAKE_PROCESS };
	CStatic	m_MakeStatus;
	CButton	m_OkBut;
	CButton	m_CancelBut;
	CProgressCtrl	m_MakeProgress;
	//}}AFX_DATA
	HANDLE				m_Thread;
	char				m_szFloppy[3];
	BOOL				m_bContinue;

	BOOL    MakeFloppyDisk();
	static	DWORD WINAPI ThreadProc(CMakeFD* dlg);
	virtual DWORD ProgressProc();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMakeFD)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMakeFD)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAKEFD_H__9630CA18_8266_482B_9B2E_8D6E46A3D82E__INCLUDED_)
