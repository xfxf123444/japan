// AMRestor.h : main header file for the AMRestore application
//
#include "stdafx.h"

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include <afxext.h>
#include "resource.h"       // main symbols
#include    <winioctl.h>

/////////////////////////////////////////////////////////////////////////////
// CAMRestorApp:
// See magnaram.cpp for the implementation of this class
//

class CAMRestorApp : public CWinApp
{
public:
	CAMRestorApp();
	virtual ~CAMRestorApp();
	void ExitApp();

protected:

	//{{AFX_VIRTUAL(CYGSetupApp)
	public:
	virtual BOOL InitInstance();
	//virtual int ExitInstance();
	//}}AFX_VIRTUAL
// Implementation
protected:
	//{{AFX_MSG(CAMRestorApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
};


/////////////////////////////////////////////////////////////////////////////
