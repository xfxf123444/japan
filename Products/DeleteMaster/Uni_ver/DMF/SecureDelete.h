// SecureDelete.h: interface for the CSecureDelete class.
//
//////////////////////////////////////////////////////////////////////

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
	BOOL DoDeleteMasterDel(WCHAR *szDirName,int nDelMethod);
	BOOL DeleteFileInRecycled(WCHAR *pszDir,BYTE *pBuf,BOOL bDel);
	BOOL DoDeleteFileInRecycled(WCHAR *pszDir,BYTE *pBuf, BOOL bDel);
	BOOL DelAllInRecycled(BYTE *pBuf,BOOL bDel);
	BOOL DoDeleteMasterDelFileInRecycled(int nDelMethod);
	CSecureDelete();
	virtual ~CSecureDelete();

private:
	BOOL IsDirectory(LPCTSTR szDirectoryName);
	BOOL DelAll(WCHAR *szDirName,BYTE *pBuf,BOOL bDel);
	BOOL InitDelBuf(BYTE *pBuf,BYTE btFir,BYTE btSec,BYTE btThr,BOOL bRand,BOOL bOneByte);
	BOOL DelDirAndFiles(WCHAR  *pszDir ,BYTE *pBuf, BOOL bDel,WCHAR  *pszSaveDir,BOOL bSave);
	BOOL GetDirFileSize(WCHAR  *pszDir);
	BOOL DoDelFile(WCHAR  *pszDir,BYTE *pBuf, BOOL bDel);

	BOOL InitDeleteMasterDel(WCHAR *szDirName);
	
	// recored the current file
	// if the user cancel the process, we will remove the current file
	// since it is not integrate

	const int DM_DEL_SIZE;
    const int BYTEINSEC;
    WCHAR RENAME_STRING[MAX_PATH];

	CString m_strCurrentFile; 
	unsigned __int64 m_qwDelSize;
	unsigned __int64 m_qwTotalSize;
	int m_nPercent;
	BOOL m_bCancelDelete;
	CString m_strOriginalDir; // do not delete the original dir
};

#endif // !defined(AFX_SECUREDELETE_H__0BCAF625_1376_4678_8761_E3BBA3268089__INCLUDED_)
