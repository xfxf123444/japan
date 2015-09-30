#if !defined(AFX_ENCRYPTWIZ_1_H__2896BC90_24A4_496E_9AB7_784F616FE865__INCLUDED_)
#define AFX_ENCRYPTWIZ_1_H__2896BC90_24A4_496E_9AB7_784F616FE865__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EncryptWiz_1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEncryptWiz_1 dialog

class CEncryptWiz_1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CEncryptWiz_1)

// Construction
public:
	BOOL AddOneItem(CListCtrl &SourceList,LPCTSTR szFileName);
	CEncryptWiz_1();
	~CEncryptWiz_1();

// Dialog Data
	//{{AFX_DATA(CEncryptWiz_1)
	enum { IDD = IDD_ENCRYPT_WIZ_1 };
	CListCtrl	m_Source;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEncryptWiz_1)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEncryptWiz_1)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddDirectory();
	afx_msg void OnAddFile();
	afx_msg void OnRemove();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENCRYPTWIZ_1_H__2896BC90_24A4_496E_9AB7_784F616FE865__INCLUDED_)
