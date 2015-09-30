// NormalDelete.h: interface for the CNormalDelete class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NORMALDELETE_H__D2688A8A_A691_4470_A568_C373324B4B1E__INCLUDED_)
#define AFX_NORMALDELETE_H__D2688A8A_A691_4470_A568_C373324B4B1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNormalDelete  
{
public:
	
	BOOL SetCancelDelete();
	BOOL NormalDeleteDirectory(LPCTSTR szDirectory);
	BOOL InitNormalDeleteDirectory(LPCTSTR szDirectory);
	BOOL GetCurrentPercent(int *pnCurrentPercent);
	CNormalDelete();
	virtual ~CNormalDelete();

private:
	CString m_strOriginalDir;
	BOOL m_bCancelDelete;
	DWORD m_dwTotalFileCount;
	DWORD m_dwFinishedFileCount;
	BOOL GetFileCount(LPCTSTR szDirectoryName);
	BOOL DeleteDirectory(LPCTSTR szDirectoryName);
};

#endif // !defined(AFX_NORMALDELETE_H__D2688A8A_A691_4470_A568_C373324B4B1E__INCLUDED_)
