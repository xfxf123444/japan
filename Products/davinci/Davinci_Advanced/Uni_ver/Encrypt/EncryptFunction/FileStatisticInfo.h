// FileStatisticInfo.h: interface for the CFileStatisticInfo class.
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

#if !defined(AFX_FILESTATISTICINFO_H__A571896B_02B2_40D5_814B_EDE96C662DD4__INCLUDED_)
#define AFX_FILESTATISTICINFO_H__A571896B_02B2_40D5_814B_EDE96C662DD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileStatisticInfo  
{
public:
	CFileStatisticInfo();
	virtual ~CFileStatisticInfo();
    void CountTotalFileSize(CStringArray &SelectionArray);
    BOOL GetDirectoryInfo(LPCTSTR szSourceDirectory);
    BOOL GetFileInfo(LPCTSTR FilePath);

public:
    unsigned __int64 m_qwTotalFileSize;


    // used for progress bar
    // and if m_dwTotalFileCount is 1
    // this is a special kind of image file
    // which maybe created by ShellEncrypt and so on
    // it will be decrypted by ShellDecrypt to the same directory as 
    // the original image file
	DWORD     m_dwTotalFileCount; 

};

#endif // !defined(AFX_FILESTATISTICINFO_H__A571896B_02B2_40D5_814B_EDE96C662DD4__INCLUDED_)
