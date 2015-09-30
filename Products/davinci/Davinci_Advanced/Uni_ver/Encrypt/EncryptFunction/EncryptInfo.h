// EncryptInfo.h: interface for the CEncryptInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENCRYPTINFO_H__22413EA7_8C46_4126_B240_878F10A98906__INCLUDED_)
#define AFX_ENCRYPTINFO_H__22413EA7_8C46_4126_B240_878F10A98906__INCLUDED_

#include "..\..\ENCRYPTTOOLSTRUCT\EncryptToolStruct.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEncryptInfo  
{
public:
	CEncryptInfo();
	virtual ~CEncryptInfo();

	CStringArray m_strSourceArray;
	CString m_strTarget;
	CString m_strPassword;
	DWORD   m_dwCompressLevel;
	ENCRYPTOPTION m_EncryptOption;
};

#endif // !defined(AFX_ENCRYPTINFO_H__22413EA7_8C46_4126_B240_878F10A98906__INCLUDED_)
