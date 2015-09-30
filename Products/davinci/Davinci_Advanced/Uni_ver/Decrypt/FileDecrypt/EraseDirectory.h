// EraseDirectory.h: interface for the CEraseDirectory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERASEDIRECTORY_H__B8CDE59D_8E47_4660_8FE7_9B9BEB3328DF__INCLUDED_)
#define AFX_ERASEDIRECTORY_H__B8CDE59D_8E47_4660_8FE7_9B9BEB3328DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEraseDirectory  
{

public:
	BOOL bIsDirectory(LPCTSTR szDirName);
	BOOL EraseDirectory(LPCTSTR szDirName);
	BOOL InitVariable();
	CEraseDirectory();
	virtual ~CEraseDirectory();

private:
	BOOL EraseLeaf(LPCTSTR szDirName);
};

#endif // !defined(AFX_ERASEDIRECTORY_H__B8CDE59D_8E47_4660_8FE7_9B9BEB3328DF__INCLUDED_)
