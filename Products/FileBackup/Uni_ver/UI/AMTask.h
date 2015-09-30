#if !defined(AFX_AMTASK_H__B5BE2651_98C5_42D2_9F50_8291C82B215F__INCLUDED_)
#define AFX_AMTASK_H__B5BE2651_98C5_42D2_9F50_8291C82B215F__INCLUDED_

#include "stdafx.h"
#include <afxdtctl.h>
#include "AM01.h"
#include "../AM01Struct/AM01Struct.h"

/////////////////////////////////////////////////////////////////////////////
// CAMTask dialog

class CAMTask : public CPropertyPage
{
	DECLARE_DYNCREATE(CAMTask)

// Construction
public:
	BOOL RefreshTaskInfo();
	BOOL GetUITaskInfo(SCHEDULE_DATA &ScheduleData);
	void InitTimeFormat();

	CTime m_TimeMonday;
	CTime m_TimeTuesday;
	CTime m_TimeWednesday;
	CTime m_TimeThursday;
	CTime m_TimeFriday;
	CTime m_TimeSaturday;
	CTime m_TimeSunday;

	void SaveControl();
	CAMTask();
	~CAMTask();

// Dialog Data
	//{{AFX_DATA(CAMTask)
	enum { IDD = IDD_AMTASK };
	CListCtrl	m_SettingList;
	CDateTimeCtrl	m_SundayCtrl;
	CDateTimeCtrl	m_SaturdayCtrl;
	CDateTimeCtrl	m_FridayCtrl;
	CDateTimeCtrl	m_ThursdayCtrl;
	CDateTimeCtrl	m_WednesdayCtrl;
	CDateTimeCtrl	m_TuesdayCtrl;
	CDateTimeCtrl	m_MondayCtrl;
	BOOL	m_bCheckMonday;
	BOOL	m_bCheckWednesday;
	BOOL	m_bCheckThursday;
	BOOL	m_bCheckFriday;
	BOOL	m_bCheckSaturday;
	BOOL	m_bCheckSunday;
	BOOL	m_bCheckTuesday;
	BOOL	m_bShutFriday;
	BOOL	m_bShutMonday;
	BOOL	m_bShutSaturday;
	BOOL	m_bShutSunday;
	BOOL	m_bShutThursday;
	BOOL	m_bShutTuesday;
	BOOL	m_bShutWednesday;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAMTask)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAMTask)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnButtonSaveSchedule();
	afx_msg void OnItemchangedListSettingName(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonAdvancedSchedule();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL SetUITaskInfo(SCHEDULE_DATA ScheduleData);
	CImageList  * m_pImageList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMTASK_H__B5BE2651_98C5_42D2_9F50_8291C82B215F__INCLUDED_)
