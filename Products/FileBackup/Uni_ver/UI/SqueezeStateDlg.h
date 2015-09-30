#if !defined(AFX_SQUEEZESTATEDLG_H__C840F67C_9E6C_4C52_8C8C_B29A0B6E2786__INCLUDED_)
#define AFX_SQUEEZESTATEDLG_H__C840F67C_9E6C_4C52_8C8C_B29A0B6E2786__INCLUDED_

#include "stdafx.h"

#include "../AM01Struct/AM01Struct.h"

// SqueezeStateDlg.h : header file
//

typedef struct
{
  HWND hParentWnd;
  SQUEEZE_PARAMETER SqueezeParameter;
}SQUEEZE_THREAD_PARAM;

/////////////////////////////////////////////////////////////////////////////
// CSqueezeStateDlg dialog

class CSqueezeStateDlg : public CDialog
{
// Construction
public:
	void SetSqueezeParameter(SQUEEZE_PARAMETER SqueezeParameter);
	CSqueezeStateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSqueezeStateDlg)
	enum { IDD = IDD_SQUEEZE_STATE };
	CAnimateCtrl	m_SqueezeAnimate;
	CProgressCtrl	m_SqueezeProgress;
	CString	m_strCurrentFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSqueezeStateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSqueezeStateDlg)
	afx_msg void OnButtonCancle();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HANDLE m_hThreadHandle;
	int m_nTimer;
	SQUEEZE_THREAD_PARAM m_SqueezeThreadParam;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SQUEEZESTATEDLG_H__C840F67C_9E6C_4C52_8C8C_B29A0B6E2786__INCLUDED_)
