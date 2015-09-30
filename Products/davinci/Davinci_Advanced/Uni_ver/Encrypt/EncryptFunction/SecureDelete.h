// SecureDelete.h: interface for the CSecureDelete class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _FILE_ENCRYPT
#include "..\FileEncrypt\stdafx.h"
#endif

#ifdef _SHELL_ENCRYPT
#include "..\ShellEncrypt\stdafx.h"
#endif

#ifdef _YG_COMMAND_LINE
#include "..\..\FED\stdafx.h"
#endif

#ifdef _YG_FS_MON
#include "..\..\YGFSMon\stdafx.h"
#endif

#if !defined(AFX_SECUREDELETE_H__0BCAF625_1376_4678_8761_E3BBA3268089__INCLUDED_)
#define AFX_SECUREDELETE_H__0BCAF625_1376_4678_8761_E3BBA3268089__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSecureDelete  
{
public:
	void GetCurrentDeleteFile(CString &strCurrentDeleteFile);
	void SetCancelDelete();
	int  GetDeletePercent();
	BOOL DoDeleteMasterDel(char *szDirName,int nDelMethod);
	CSecureDelete();
	virtual ~CSecureDelete();

private:
	BOOL IsDirectory(LPCTSTR szDirectoryName);
	BOOL DelAll(char *szDirName,BYTE *pBuf,BOOL bDel);
	BOOL InitDelBuf(BYTE *pBuf,BYTE btFir,BYTE btSec,BYTE btThr,BOOL bRand,BOOL bOneByte);
	BOOL DelDirAndFiles(LPTSTR  pszDir ,BYTE *pBuf, BOOL bDel,LPTSTR  pszSaveDir,BOOL bSave);
	BOOL GetDirFileSize(LPCTSTR  pszDir);
	BOOL DoDelFile(LPTSTR  pszDir,BYTE *pBuf, DWORD dwFileSize,BOOL bDel);

	BOOL InitDeleteMasterDel(LPCTSTR szDirName);
	
	// recored the current file
	// if the user cancel the process, we will remove the current file
	// since it is not integrate

	const int DM_DEL_SIZE;
    const int BYTEINSEC;
    char RENAME_STRING[MAX_PATH];

	CString m_strCurrentFile; 
	unsigned __int64 m_qwDelSize;
	unsigned __int64 m_qwTotalSize;
	int m_nPercent;
	BOOL m_bCancelDelete;
	CString m_strOriginalDir; // do not delete the original dir
};

#endif // !defined(AFX_SECUREDELETE_H__0BCAF625_1376_4678_8761_E3BBA3268089__INCLUDED_)
