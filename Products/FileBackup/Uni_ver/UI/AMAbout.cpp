// AMAbout.cpp : implementation file
//

#include "stdafx.h"
#include "AM01.h"
#include "AMAbout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAMAbout property page

IMPLEMENT_DYNCREATE(CAMAbout, CPropertyPage)

CAMAbout::CAMAbout() : CPropertyPage(CAMAbout::IDD)
{
	//{{AFX_DATA_INIT(CAMAbout)
	m_Information = _T("");
	//}}AFX_DATA_INIT
}

CAMAbout::~CAMAbout()
{
}

void CAMAbout::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAMAbout)
	DDX_Text(pDX, IDC_INFORMATION, m_Information);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAMAbout, CPropertyPage)
	//{{AFX_MSG_MAP(CAMAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAMAbout message handlers


BOOL CAMAbout::OnInitDialog() 
{
	CString csName,csCompany,csSerial;
	WCHAR    szName[MAX_PATH],szCompany[MAX_PATH],szSerial[MAX_PATH];
	HKEY	hKey;
	DWORD	dwSize,dwType;

	CPropertyPage::OnInitDialog();

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,AMSOFTWAREKEY,0,KEY_QUERY_VALUE,&hKey) == ERROR_SUCCESS)
	{
		csName.LoadString(IDS_ABOUT_NAME);
		csCompany.LoadString(IDS_ABOUT_COMPANY);
		csSerial.LoadString(IDS_ABOUT_SERIALNUM);


		dwSize = MAX_PATH * sizeof(WCHAR);

		if(RegQueryValueEx(hKey,L"Company",NULL,&dwType,(LPBYTE)szCompany,&dwSize) != ERROR_SUCCESS) 
		{
		    wcscpy(szCompany,L"Unknown");
		}

		dwSize = MAX_PATH * sizeof(WCHAR);

		if(RegQueryValueEx(hKey,L"Password",NULL,&dwType,(LPBYTE)szSerial,&dwSize) != ERROR_SUCCESS) 
		{
			wcscpy(szSerial,L"Unknown");
		}

		dwSize = MAX_PATH * sizeof(WCHAR);

		if(RegQueryValueEx(hKey,L"UserName",NULL,&dwType,(LPBYTE)szName,&dwSize) != ERROR_SUCCESS) 
		{
		    wcscpy(szName,L"Unknown");
		}

		RegCloseKey(hKey);

		m_Information.Format(L"%s %s\n%s %s\n%s %s",csName,szName,csCompany,szCompany,csSerial,szSerial);
		UpdateData(FALSE);
	}
	else
	{
		TRACE(L"\nRegOpenKeyEx error in CAMAbout::OnInitDialog()");
	}

	return TRUE;  
}


