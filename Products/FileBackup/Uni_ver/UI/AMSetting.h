#ifndef AM_SETTING_HEAD_FILE_H
#define AM_SETTING_HEAD_FILE_H
// AMSetting.h : header file
//
#include "stdafx.h"
#include "EasyWizard.h"
#include "EasyPage1.h"
#include "EasyPage2.h"
#include "EasyPage3.h"
#include "..\AM01STRUCT\AM01Struct.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CAMSetting

class CAMSetting : public CPropertyPage
{
	DECLARE_DYNAMIC(CAMSetting)

// Construction
public:
	CAMSetting();	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAMSetting)
	enum { IDD = IDD_AMSETTING };
	CListCtrl	m_SettingList;
	CString	m_Comment;
	CString	m_CurrentSetting;
	//}}AFX_DATA
// Attributes

protected:

   CEasyWizard*      m_pEasyWizard;
   CEasyPage1*       m_pEasyPage1;
   CEasyPage2*       m_pEasyPage2;
   CEasyPage3*       m_pEasyPage3;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAMSetting)
	public:
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

// Implementation
public:
	AM01_PROG_SETTING m_CurSetting;
	BOOL RefreshSetting();
	CImageList * m_pImageList;

	
	virtual ~CAMSetting();
	BOOL OnInitDialog();
	void DoDataExchange(CDataExchange* pDX);

	// Generated message map functions
protected:
	//{{AFX_MSG(CAMSetting)
	afx_msg void OnSettingWiz();
	afx_msg void OnRemove();
	afx_msg void OnExecute();
	afx_msg void OnClose();
	afx_msg void OnButtonViewSetting();
	afx_msg void OnDestroy();
	afx_msg void OnButtonEasyBackup();
	afx_msg void OnSetCurrent();
	afx_msg void OnButtonSnapshot();
	afx_msg void OnItemchangedSettingList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL CheckSettingName( WCHAR *szSettingName);
};

/////////////////////////////////////////////////////////////////////////////
#endif
