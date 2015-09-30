// YGFSMon.h : main header file for the MAGNARAM application
//
#include "stdafx.h"
#include "MainWnd.h"

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include <afxext.h>
#include <winioctl.h>
#include "resource.h"       // main symbols
#include "..\..\..\Davinci_tech\YGFSMon\cur_ver\export\ygfsmon.h"

BOOL YGFSMonCheckVersion();
BOOL YGFSMonStartWork();
BOOL YGFSMonStopWork();
BOOL YGFSMonClearState();
BOOL YGFSMonAddExcludeProcess(LPTSTR szProcess);
BOOL YGFSMonRemoveExcludeProcess(LPTSTR szProcess);
BOOL YGFSMonAddExcludePath(LPTSTR szPath);
BOOL YGFSMonRemoveExcludePath(LPTSTR szPath);
BOOL YGFSMonAddMonitorItem(PCONVERT_BUF pMonitorItem);
BOOL YGFSMonRemoveMonitorItem(PCONVERT_BUF pMonitorItem);
BOOL YGFSMonGetModifyFile(CONVERT_BUF *pModifyInfo);
BOOL YGFSMonGetMonitorDrive();

/////////////////////////////////////////////////////////////////////////////
// CYGFSMonApp:
// See magnaram.cpp for the implementation of this class
//

class CYGFSMonApp : public CWinApp
{
public:
	CYGFSMonApp();
	virtual ~CYGFSMonApp();
	void ExitApp();
protected:

	//{{AFX_VIRTUAL(CYGFSMonApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	
	//}}AFX_VIRTUAL
// Implementation
protected:
	//{{AFX_MSG(CYGFSMonApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL InitDavinciStatus();
	CMainWnd m_MainWnd;
};


/////////////////////////////////////////////////////////////////////////////
