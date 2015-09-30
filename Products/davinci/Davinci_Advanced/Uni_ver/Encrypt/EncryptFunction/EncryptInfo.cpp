// EncryptInfo.cpp: implementation of the CEncryptInfo class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _FILE_ENCRYPT
#include "..\FileEncrypt\stdafx.h"
#endif

#ifdef _SHELL_ENCRYPT
#include "..\ShellEncrypt\stdafx.h"
#endif

#ifdef _YG_FS_MON
#include "..\..\YGFSMon\stdafx.h"
#endif

#ifdef _YG_COMMAND_LINE
#include "..\..\FED\stdafx.h"
#endif

#include "EncryptInfo.h"
#include "..\..\EncryptToolStruct\EncryptToolStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEncryptInfo::CEncryptInfo()
{
	m_dwCompressLevel = 0;
	m_strSourceArray.RemoveAll();
	m_strTarget.Empty();
	m_strPassword.Empty();
	memset(&m_EncryptOption,0,sizeof(ENCRYPTOPTION));
	m_EncryptOption.LimitTime = CTime::GetCurrentTime() + CTimeSpan(365,0,0,0);
	m_EncryptOption.BaseTime = CTime::GetCurrentTime();
	m_EncryptOption.ulMaxCount = 1;
	m_EncryptOption.ulErrorLimit = 3;
//	m_EncryptOption.bErrorLimition = FALSE;
}

CEncryptInfo::~CEncryptInfo()
{
}

