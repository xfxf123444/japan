// WZStep4.h : header file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#if !defined(AFX_WZSTEP4_H__99E7B195_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_)
#define AFX_WZSTEP4_H__99E7B195_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_

#include "stdafx.h"

#include "NewRestoreTreeCtrl.h"

#include "..\AM01Struct\AM01Struct.h"

class CDialogThread : public CWinThread
{
	DECLARE_DYNCREATE(CDialogThread)
	CDialogThread() {};
	virtual BOOL InitInstance();
};


/////////////////////////////////////////////////////////////////////////////
// CWZStep4 dialog

class CWZStep4 : public CPropertyPageEx
{
	DECLARE_DYNCREATE(CWZStep4)

// Construction
public:
	BOOL GetUIInfo();

	CImageList m_ImageList;
	void ShowStampInfo();
	void ClearAllColumn();
	void ClearFile();
	void InitDialogItem();
	void LoadTree();
	void SetPartRestoreColumn();
	void SetEntireRestoreColumn();
	int GetSelectedItem();

	void ShowItemInfo(DWORD dwPathID);
	CString GetItemPath();


	void LoadTimeStamp(LPCTSTR szImageFile);
	CWZStep4();
	~CWZStep4();

// Dialog Data
	//{{AFX_DATA(CWZStep4)
	enum { IDD = IDD_WZSTEP4 };
	CNewRestoreTreeCtrl	m_DirTree;
	CListCtrl	m_TimeStamp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWZStep4)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWZStep4)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WZSTEP4_H__99E7B195_4A5C_11D1_BF2C_00C04FC99F83__INCLUDED_)
