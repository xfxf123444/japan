#if !defined(AFX_SQUEEZEIMAGE_H__10E9FCDB_3191_462C_B4C7_1FFB0725BDE6__INCLUDED_)
#define AFX_SQUEEZEIMAGE_H__10E9FCDB_3191_462C_B4C7_1FFB0725BDE6__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CSqueezeImage dialog

class CSqueezeImage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSqueezeImage)

// Construction
public:
	CSqueezeImage();
	~CSqueezeImage();

// Dialog Data
	//{{AFX_DATA(CSqueezeImage)
	enum { IDD = IDD_SQUEEZE_IMAGE };
	CString	m_strSourceImage;
	CString	m_strTargetImage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSqueezeImage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSqueezeImage)
	afx_msg void OnButtonReferSource();
	afx_msg void OnButtonReferTarget();
	afx_msg void OnButtonSqueezeImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL CheckOverwrite(LPCTSTR szTargetImage);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SQUEEZEIMAGE_H__10E9FCDB_3191_462C_B4C7_1FFB0725BDE6__INCLUDED_)
