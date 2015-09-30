// SqueezeImage.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "SqueezeImage.h"
#include "Function.h"
#include "SqueezeStateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSqueezeImage property page

IMPLEMENT_DYNCREATE(CSqueezeImage, CPropertyPage)

CSqueezeImage::CSqueezeImage() : CPropertyPage(CSqueezeImage::IDD)
{
	//{{AFX_DATA_INIT(CSqueezeImage)
	m_strSourceImage = _T("");
	m_strTargetImage = _T("");
	//}}AFX_DATA_INIT
}

CSqueezeImage::~CSqueezeImage()
{
}

void CSqueezeImage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSqueezeImage)
	DDX_Text(pDX, IDC_EDIT_SOURCE_IMAGE, m_strSourceImage);
	DDX_Text(pDX, IDC_EDIT_TARGET_IMAGE, m_strTargetImage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSqueezeImage, CPropertyPage)
	//{{AFX_MSG_MAP(CSqueezeImage)
	ON_BN_CLICKED(IDC_BUTTON_REFER_SOURCE, OnButtonReferSource)
	ON_BN_CLICKED(IDC_BUTTON_REFER_TARGET, OnButtonReferTarget)
	ON_BN_CLICKED(IDC_BUTTON_SQUEEZE_IMAGE, OnButtonSqueezeImage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSqueezeImage message handlers

void CSqueezeImage::OnButtonReferSource() 
{
	// TODO: Add your control notification handler code here
	WCHAR szSourceImage[MAX_PATH];
	if (SelectFile(L"AMG",L"AM01 Image File",szSourceImage))
	{
		m_strSourceImage = szSourceImage;
		UpdateData(FALSE);
	}	
}

void CSqueezeImage::OnButtonReferTarget() 
{
	// TODO: Add your control notification handler code here
	WCHAR szTargetImage[MAX_PATH];
	if (NewSelectFile(L"AMG",L"AM01 Image File",szTargetImage))
	{
		m_strTargetImage = szTargetImage;
		UpdateData(FALSE);
	}	
	
}

void CSqueezeImage::OnButtonSqueezeImage() 
{
	// TODO: Add your control notification handler code here

	SQUEEZE_PARAMETER SqueezeParameter;

	UpdateData(TRUE);

	DWORD dwErrorCode;

	if ( FALSE == CheckLastImage((LPCTSTR)m_strSourceImage,&dwErrorCode))
	{
		if(NOT_LAST_IMAGE == dwErrorCode )
		{
			AfxMessageBox(IDS_NOT_LAST_IMAGE,MB_OK|MB_ICONINFORMATION);
		}
		return;
	}

	// the function IsValidAMGPath will create the directory if needed
	
	// if( FALSE == IsValidAMGPath((LPCTSTR)m_strTargetImage))
	if( FALSE == IsValidAMGPath(m_strTargetImage.GetBuffer(0)))
	{
        TRACE(L"\nInvalid AMG Path");
		m_strTargetImage.ReleaseBuffer();
		return;
	}
	m_strTargetImage.ReleaseBuffer();

	if ( FALSE == CheckOverwrite((LPCTSTR)m_strTargetImage))
	{
		TRACE(L"\nNot overwrite the original file.");
		return;
	}

	wcsncpy(SqueezeParameter.szSourceImage ,(LPCTSTR)m_strSourceImage,MAX_PATH-1);
	wcsncpy(SqueezeParameter.szTargetImage ,(LPCTSTR)m_strTargetImage,MAX_PATH-1);

	CSqueezeStateDlg SqueezeStateDlg;

	SqueezeStateDlg.SetSqueezeParameter(SqueezeParameter);
	SqueezeStateDlg.DoModal();

	/*
	if( FALSE == SqueezeImage( (LPCTSTR)m_strSourceImage,(LPCTSTR)m_strTargetImage) )
	{
		TRACE(L"\nSqueezeImage Error.");
	}
	*/

}

// if the file is not exist, return TRUE
// if the file exist and the user choose overwrite, return TRUE

BOOL CSqueezeImage::CheckOverwrite(LPCTSTR szTargetImage)
{
	DWORD dwFileAttribute;
	
	dwFileAttribute = GetFileAttributes(szTargetImage);
	
	if( -1 != dwFileAttribute )
	{
		if ( 0 != (FILE_ATTRIBUTE_DIRECTORY & dwFileAttribute ) )
		{
			TRACE(L"\nTarget is a directory.");
			return FALSE;
		}

		CString strMessage;
		strMessage = strMessage + szTargetImage + L" already exist, overwrite it ?";

		if( IDYES != AfxMessageBox( (LPCTSTR)strMessage,MB_YESNOCANCEL|MB_ICONINFORMATION,NULL) )
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	return TRUE;
}

