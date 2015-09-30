// AM01.h : main header file for the AM01 application
//
#ifndef AM01_APPLICATION_HEAD_FILE_H
#define AM01_APPLICATION_HEAD_FILE_H
#include "stdafx.h"

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include <afxext.h>
#include "resource.h"       // main symbols
#include    <winioctl.h>
#include "MainWnd.h"	// Added by ClassView


BOOL GetCurrentSetting(AM01_PROG_SETTING *pSetting);
BOOL SetCurrentSetting(WCHAR *szSetting);

BOOL SelectFile(WCHAR *szFileExt,WCHAR *szFileType,WCHAR *szFile);
BOOL GetBecky2DataPath(WCHAR *szPath);

BOOL GetOutlookDataPath(WCHAR *szPath);
BOOL ShutdownSystem();

BOOL IsValidAMGPath(WCHAR *szPath);

BOOL IsValidRMIMGPath(WCHAR *szPath);
BOOL CheckConflict(WCHAR *szSource,WCHAR *szTargetImg,BOOL bImage);

#define BECKY2DATAKEY         L"Software\\RimArts\\B2\\Settings"
#define AMSOFTWAREKEY         L"Software\\FrontLine\\FileBackup\\6.00"


/////////////////////////////////////////////////////////////////////////////
// CAM01App:
// See magnaram.cpp for the implementation of this class
//

class CAM01App : public CWinApp
{
public:
CAM01App();

	void CloseSplash();
protected:

	//{{AFX_VIRTUAL(CYGSetupApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
// Implementation
protected:
	//{{AFX_MSG(CAM01App)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
private:
	CMainWnd *m_pSaveMainWnd;
};


/////////////////////////////////////////////////////////////////////////////
#endif
