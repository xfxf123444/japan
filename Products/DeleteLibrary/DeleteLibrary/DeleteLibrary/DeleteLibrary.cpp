#include "DeleteLibrary.h"
#include <process.h>
#include <ctime>
#include <cmath>
#include <algorithm>
#include "DeleteLibraryHelper.h"

using namespace std;

#define BYTEINSEC 512
#define DM_DEL_SIZE (128 * BYTEINSEC)
#define  RENAME_STRING _T("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$")

DeleteLibrary::DeleteLibrary()
{
	m_pBuf = 0;
	m_method = DEL_METHOD_00;
	m_isDeletingInProgress = false;
	m_toStopCurrentTask = false;
	m_totalSize.QuadPart = 0;
	m_processedSize.QuadPart = 0;
	m_pLogPath = new CString;
	m_pUserName = new CString;
	m_pDeletePathSet = new std::vector<const TCHAR*>();
}


DeleteLibrary::~DeleteLibrary(void)
{
	Clear();
	delete m_pLogPath;
	delete m_pUserName;
	for (vector<const TCHAR*>::iterator it = m_pDeletePathSet->begin(); it != m_pDeletePathSet->end(); ++it)
	{
		delete[] (*it);
	}
	delete m_pDeletePathSet;
}

DeleteOperationResult DeleteLibrary::StartDeletingTask(const TCHAR* pDeletePathArray, int arrayCount)
{
	if (m_isDeletingInProgress)
	{
		return DEL_RESULT_OPERATION_CANNOT_BE_EXECUTED;
	}
	if (pDeletePathArray == 0 || arrayCount == 0)
	{
		return DEL_RESULT_INVALID_FILE_PATH;
	}
	for (vector<const TCHAR*>::iterator it = m_pDeletePathSet->begin(); it != m_pDeletePathSet->end(); ++it)
	{
		delete[] (*it);
	}
	m_pDeletePathSet->clear();
	for (int i = 0; i < arrayCount; ++i)
	{
		TCHAR* temp = new TCHAR[MAX_PATH];
		ZeroMemory(temp, sizeof(TCHAR) * MAX_PATH);
		_tcscpy(temp, pDeletePathArray + i * MAX_PATH);
		m_pDeletePathSet->push_back(temp);
	}
	m_bDeleteRecycledBin = false;
	unsigned int threadID;
	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0, &ThreadProc, this, 0, &threadID );
	if (hThread == INVALID_HANDLE_VALUE)
	{
		m_isDeletingInProgress = false;
		for (vector<const TCHAR*>::iterator it = m_pDeletePathSet->begin(); it != m_pDeletePathSet->end(); ++it)
		{
			delete[] (*it);
		}
		m_pDeletePathSet->clear();
		return DEL_RESULT_OPERATION_CANNOT_BE_EXECUTED;
	}
	else
	{
		CloseHandle( hThread );
		return DEL_RESULT_OK;
	}
}

DeleteOperationResult DeleteLibrary::StartDeletingRecycledBin()
{
	if (m_isDeletingInProgress)
	{
		return DEL_RESULT_OPERATION_CANNOT_BE_EXECUTED;
	}
	m_bDeleteRecycledBin = true;
	for (vector<const TCHAR*>::iterator it = m_pDeletePathSet->begin(); it != m_pDeletePathSet->end(); ++it)
	{
		delete[] (*it);
	}
	m_pDeletePathSet->clear();
	unsigned int threadID;
	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0, &ThreadProc, this, 0, &threadID );
	if (hThread == INVALID_HANDLE_VALUE)
	{
		m_isDeletingInProgress = false;
		return DEL_RESULT_OPERATION_CANNOT_BE_EXECUTED;
	}
	else
	{
		CloseHandle( hThread );
		return DEL_RESULT_OK;
	}
}

void DeleteLibrary::StopDeletingTask()
{
	if (m_isDeletingInProgress)
	{
		m_toStopCurrentTask = true;
	}
}

bool DeleteLibrary::IsDeletingInProgress()
{
	return m_isDeletingInProgress;
}

void DeleteLibrary::CalculateDeleteFileSize()
{
	m_totalSize.QuadPart = 0;
	if (m_bDeleteRecycledBin)
	{
		DeleteLibraryHelper::GetFileSizeInRecycled(&m_totalSize);
	}
	else
	{
		for (vector<const TCHAR*>::iterator it = m_pDeletePathSet->begin(); it != m_pDeletePathSet->end(); ++it)
		{
			ULARGE_INTEGER size;
			vector<CString> temp;
			if (DeleteLibraryHelper::GetFilePath(*it, temp, 0, true, &size, true))
			{
				m_totalSize.QuadPart += size.QuadPart;
			}
		}
	}
}

unsigned int WINAPI DeleteLibrary::ThreadProc( void* pParm )
{
	DeleteLibrary* pDeleteHandler = (DeleteLibrary*)pParm;
	pDeleteHandler->Init();
	if (!pDeleteHandler->m_bDeleteRecycledBin)
	{
		pDeleteHandler->DoDeleteMasterDel();
	}
	else
	{
		pDeleteHandler->DoDeleteMasterDelFileInRecycled();
	}
	pDeleteHandler->Clear();
	return 0;
}

bool DeleteLibrary::InitDelBuf(BYTE *pBuf,BYTE btFir,BYTE btSec,BYTE btThr,bool bRand,bool bOneByte)
{
	BYTE *pOneBuf;
	DWORD i,nRemain;

	if(bRand)
	{
		srand( (unsigned)time( NULL ) );
		btFir = (BYTE)rand();
	}
	if(bOneByte)
	{
		memset(pBuf,btFir,DM_DEL_SIZE);
	}
	else
	{
		pOneBuf = (BYTE*)malloc(BYTEINSEC);
		if (!pOneBuf)
		{
			return false;
		}
		for(i=0;i<BYTEINSEC;i++)
		{
			nRemain = i % 3;
			switch(nRemain)
			{
			case 0:
				*(pOneBuf+i) = btFir;
				break;
			case 1:
				*(pOneBuf+i) = btSec;
				break;
			case 2:
				*(pOneBuf+i) = btThr;
				break;
			}
		}
		DWORD cnt = DM_DEL_SIZE / BYTEINSEC;
		for(i = 0; i < cnt; i++)
		{
			memcpy(pBuf + BYTEINSEC * i, pOneBuf, BYTEINSEC);
		}
		free(pOneBuf);
	}
	return true;
}

DeleteOperationResult DeleteLibrary::SetDeleteMethod(DeleteMethod method)
{
	if (m_isDeletingInProgress)
	{
		return DEL_RESULT_OPERATION_CANNOT_BE_EXECUTED;
	}
	m_method = method;
	return DEL_RESULT_OK;
}

DeleteMethod DeleteLibrary::GetDeleteMethod()
{
	return m_method;
}

DeleteOperationResult DeleteLibrary::SetLogPath(const TCHAR* logPath)
{
	if (m_isDeletingInProgress)
	{
		return DEL_RESULT_OPERATION_CANNOT_BE_EXECUTED;
	}
	*m_pLogPath = logPath;
	return DEL_RESULT_OK;
}

const TCHAR* DeleteLibrary::GetLogPath()
{
	return *m_pLogPath;
}

void DeleteLibrary::FilterDuplicatedOrSubFolder()
{
	class SortClass
	{
	public:
		bool operator()(const TCHAR* l, const TCHAR* r) 
		{
			return _tcscmp(l, r) <= 0;
		}
	};
	sort(m_pDeletePathSet->begin(), m_pDeletePathSet->end(), SortClass());
	vector<const TCHAR*> totalPathSet = *m_pDeletePathSet;
	m_pDeletePathSet->clear();
	for (vector<const TCHAR*>::iterator it = totalPathSet.begin(); it != totalPathSet.end(); ++it)
	{
		CString path = *it;
		bool toAccept = true;
		bool bEqualDir = false;
		bool bSubDir = false;
		for (vector<const TCHAR*>::iterator it = m_pDeletePathSet->begin(); it != m_pDeletePathSet->end(); ++it)
		{
			CString acceptedPath = *it;
			if (DeleteLibraryHelper::CheckEqualOrSubDir(path, acceptedPath, bEqualDir, bSubDir))
			{
				toAccept = false;
				break;
			}
		}
		if (toAccept)
		{
			TCHAR* temp = new TCHAR[MAX_PATH];
			ZeroMemory(temp, sizeof(TCHAR) * MAX_PATH);
			_tcscpy(temp, path);
			m_pDeletePathSet->push_back(temp);
		}
	}
	for (vector<const TCHAR*>::iterator it = totalPathSet.begin(); it != totalPathSet.end(); ++it)
	{
		delete[] *it;
	}
}

void DeleteLibrary::Init()
{
	m_isDeletingInProgress = true;
	m_toStopCurrentTask = false;
	m_totalSize.QuadPart = 0;
	m_processedSize.QuadPart = 0;
	ULONG ulLen = MAX_PATH;
	TCHAR temp[MAX_PATH];
	ZeroMemory(temp, sizeof(temp));
	GetUserName(temp, &ulLen);
	*m_pUserName = temp;

	if (m_pBuf)
	{
		free(m_pBuf);
		m_pBuf = 0;
	}
	if (!m_bDeleteRecycledBin)
	{
		FilterDuplicatedOrSubFolder();
	} 
	CalculateDeleteFileSize();
}

void DeleteLibrary::Clear()
{
	free(m_pBuf);
	m_pBuf = 0;
	m_isDeletingInProgress = false;
}

void DeleteLibrary::WriteLog(const TCHAR *szFile, BOOL bSucc)
{
	if (m_pLogPath->IsEmpty())
	{
		return;
	}
	CTime  time;
	time = CTime::GetCurrentTime();
	CString strText;
	HANDLE  hFile;
	WORD    wUnicodeSign = 0;
	ULONG	ulEnter = 0x000a000d;
	ULONG	ulLen = MAX_PATH,dwWrtited;

	strText.Format(_T("Delete %s, User:%s, Method:%s, Time:%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d, Result:%s"), 
					szFile, *m_pUserName, GetDeleteMethodString(),
					time.GetYear(),time.GetMonth(),time.GetDay(),
					time.GetHour(),time.GetMinute(),time.GetSecond(),
					bSucc ? _T("Success") : _T("Failure"));

	hFile = CreateFile(*m_pLogPath, GENERIC_READ|GENERIC_WRITE,
					 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
					 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
					 NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	ReadFile(hFile,&wUnicodeSign,sizeof(WORD),&dwWrtited,NULL);
	if (wUnicodeSign != 0xfeff)
	{
		wUnicodeSign = 0xfeff;
		SetFilePointer(hFile,0,NULL,FILE_BEGIN);
		WriteFile(hFile,&wUnicodeSign,sizeof(WORD),&dwWrtited,NULL);
	}
	else
	{
		SetFilePointer(hFile,0,NULL,FILE_END);
	}
	WriteFile(hFile,strText,strText.GetLength()*sizeof(TCHAR),&dwWrtited,NULL);
	WriteFile(hFile,&ulEnter,sizeof(ULONG),&dwWrtited,NULL);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	return;
}

CString DeleteLibrary::GetDeleteMethodString()
{
	CString temp;
	switch (m_method)
	{
	case DEL_METHOD_00:
		temp = _T("00");
		break;
	case DEL_METHOD_FF:
		temp = _T("FF");
		break;
	case DEL_METHOD_RANDOM:
		temp = _T("RANDOM");
		break;
	case DEL_METHOD_NSA:
		temp = _T("NSA");
		break;
	case DEL_METHOD_OLD_NSA:
		temp = _T("OLD_NSA");
		break;
	case DEL_METHOD_DOD_STD:
		temp = _T("DOD_STD");
		break;
	case DEL_METHOD_NATO:
		temp = _T("NATO");
		break;
	case DEL_METHOD_GUTMANN:
		temp = _T("GUTMANN");
		break;
	default:
		temp = _T("UNKOWN");
		break;
	}
	return temp;
}

BOOL DeleteLibrary::DoDelFile(const TCHAR *pszDir,BOOL bDel)
{
	HANDLE		hFile;
	DWORD		i,j;
	DWORD		dwRWTimes;
	DWORD		dwRW;
	TCHAR		szFileName[MAX_PATH];
	TCHAR		*p;
	DWORD       dwFileSizeLo,dwFileSizeHigh;

	if (m_toStopCurrentTask)
	{
		return FALSE;
	}

	hFile = CreateFile(pszDir, 
		GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
		return FALSE;

	dwFileSizeLo = GetFileSize(hFile,&dwFileSizeHigh);

	i = dwFileSizeLo / DM_DEL_SIZE + dwFileSizeHigh * 0x4000;
	j = dwFileSizeLo % DM_DEL_SIZE; 

	for(dwRWTimes=0;dwRWTimes<i;dwRWTimes++)
	{
		WriteFile(hFile,m_pBuf,DM_DEL_SIZE,&dwRW,NULL);
		m_processedSize.QuadPart += DM_DEL_SIZE;
	}
	if(j)
	{
		WriteFile(hFile,m_pBuf,j,&dwRW,NULL);
		m_processedSize.QuadPart += j;
	}
	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	if(bDel) 
	{
		_tcscpy(szFileName,pszDir);
		p = _tcsrchr(szFileName,_T('\\'));
		p++;
		_tcscpy(p,RENAME_STRING);
		MoveFile(pszDir,szFileName);
		DeleteFile(szFileName);
		for(i=65;i<=90;i++)
		{
			*p = (TCHAR)i;
			hFile = CreateFile(szFileName, 
				GENERIC_READ|GENERIC_WRITE, 
				FILE_SHARE_READ |FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			CloseHandle(hFile);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (TCHAR)i;
			hFile = CreateFile(szFileName, 
				GENERIC_READ|GENERIC_WRITE, 
				FILE_SHARE_READ |FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			CloseHandle(hFile);
		}
		p--;
		_tcscpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (TCHAR)i;
			DeleteFile(szFileName);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (TCHAR)i;
			DeleteFile(szFileName);
		}
	}

	return TRUE;
}

BOOL DeleteLibrary::DelDirAndFiles(const TCHAR *pszDir,BOOL bDel)
{
	WIN32_FIND_DATA    FindData;
	HANDLE              hFind   ;
	DWORD				dwAttrs;
	int					i;
	TCHAR				*p;
	int					nDirEnd;
	TCHAR				szDirOrFileName[MAX_PATH];
	TCHAR				szDirName[MAX_PATH];
	CString             strText;

	if (m_toStopCurrentTask)
	{
		return FALSE;
	}

	hFind = FindFirstFile(pszDir,&FindData);
	if (INVALID_HANDLE_VALUE == hFind)	return FALSE;

	if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		FindClose(hFind);
		dwAttrs = GetFileAttributes(pszDir);
		SetFileAttributes(pszDir,FILE_ATTRIBUTE_NORMAL);
		if(!DoDelFile(pszDir,bDel))
		{
			ULARGE_INTEGER temp;
			temp.LowPart = FindData.nFileSizeLow;
			temp.HighPart = FindData.nFileSizeHigh;
			m_processedSize.QuadPart += temp.QuadPart;
			SetFileAttributes(pszDir,dwAttrs);
			if (bDel && !m_toStopCurrentTask)
			{
				WriteLog(pszDir, FALSE);
			}
			return FALSE;
		}
		if (bDel)
		{
			WriteLog(pszDir, TRUE);
		}
		return TRUE;
	}
	FindClose(hFind);

	_tcscpy( szDirOrFileName , pszDir) ;
	_tcscat( szDirOrFileName , _T("\\")) ;
	nDirEnd = static_cast<int>(_tcslen( szDirOrFileName));
	_tcscpy( &szDirOrFileName[nDirEnd] , _T("*.*"));

	hFind = FindFirstFile(szDirOrFileName ,&FindData);

	if ( INVALID_HANDLE_VALUE == hFind )
		return  FALSE ;

	do
	{
		if (_tcscmp(FindData.cFileName,_T(".")) && 
			_tcscmp(FindData.cFileName,_T("..")))
		{
			_tcscpy( &szDirOrFileName[nDirEnd] ,FindData.cFileName);

			if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
			{
				dwAttrs = GetFileAttributes(szDirOrFileName);
				SetFileAttributes(szDirOrFileName,FILE_ATTRIBUTE_NORMAL);
				if(!DoDelFile(szDirOrFileName,bDel))
				{
					ULARGE_INTEGER temp;
					temp.LowPart = FindData.nFileSizeLow;
					temp.HighPart = FindData.nFileSizeHigh;
					m_processedSize.QuadPart += temp.QuadPart;
					SetFileAttributes(szDirOrFileName,dwAttrs);
					if (bDel && !m_toStopCurrentTask)
					{
						WriteLog(szDirOrFileName, FALSE);
					}
				}
				else
				{
					if (bDel)
					{
						WriteLog(szDirOrFileName, TRUE);
					}
				}
			}
			else
			{
				if (!DelDirAndFiles(szDirOrFileName,bDel))
				{
					FindClose( hFind );
					return FALSE ;
				}
			}
		}
	} while( FindNextFile( hFind ,&FindData));
	FindClose( hFind);
	if(bDel)
	{
		RemoveDirectory(pszDir);

		_tcscpy(szDirName,pszDir);
		p = _tcsrchr(szDirName,_T('\\'));
		p++;
		_tcscpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (TCHAR)i;
			CreateDirectory(szDirName,NULL);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (TCHAR)i;
			CreateDirectory(szDirName,NULL);
		}
		p --;
		_tcscpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (TCHAR)i;
			RemoveDirectory(szDirName);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (TCHAR)i;
			RemoveDirectory(szDirName);
		}
	}
	return  TRUE;
}

BOOL DeleteLibrary::DoDeleteMasterDel()
{
	int			i;
	BOOL        bResult = TRUE;
	CString		csText,csCaption;

	m_pBuf = (BYTE*)malloc(DM_DEL_SIZE);
	if (!m_pBuf)
	{
		return FALSE;
	}

	switch(m_method)
	{
	case DEL_METHOD_00:
		InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
		bResult = DelAll(TRUE);
		break;
	case DEL_METHOD_FF:
		InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
		bResult = DelAll(TRUE);
		break;
	case DEL_METHOD_RANDOM:
		InitDelBuf(m_pBuf,0,0,0,TRUE,TRUE);
		bResult = DelAll(TRUE);
		break;
	case DEL_METHOD_NSA:
		m_totalSize.QuadPart = m_totalSize.QuadPart *3;
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
				InitDelBuf(m_pBuf,0,0,0,TRUE,TRUE);
				break;
			case 2:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			}
			if(i==2)
			{
				if(!DelAll(TRUE))
				{
					bResult = FALSE;
					break;
				}
			}
			else
			{
				if(!DelAll(FALSE))
				{
					bResult = FALSE;
					break;
				}
			}
		}
		break;
	case DEL_METHOD_OLD_NSA:
		m_totalSize.QuadPart = m_totalSize.QuadPart *4;
		for(i=0;i<4;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
				InitDelBuf(m_pBuf,0xFF,0,0,FALSE,TRUE);
				break;
			}
			if(i==3)
			{
				if(!DelAll(TRUE))
				{
					bResult = FALSE;
					break;
				}
			}
			else
			{
				if(!DelAll(FALSE))
				{
					bResult = FALSE;
					break;
				}
			}
		}
		break;
	case DEL_METHOD_DOD_STD:
		m_totalSize.QuadPart = m_totalSize.QuadPart *3;
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
				InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 2:
				InitDelBuf(m_pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(i==2)
			{
				if(!DelAll(TRUE))
				{
					bResult = FALSE;
					break;
				}
			}
			else
			{
				if(!DelAll(FALSE))
				{
					bResult = FALSE;
					break;
				}
			}
		}
		break;
	case DEL_METHOD_NATO:
		m_totalSize.QuadPart = m_totalSize.QuadPart *7;
		for(i=0;i<7;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
			case 4:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
			case 5:
				InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 6:
				InitDelBuf(m_pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(i==6)
			{
				if(!DelAll(TRUE))
				{
					bResult = FALSE;
					break;
				}
			}
			else
			{
				if(!DelAll(FALSE))
				{
					bResult = FALSE;
					break;
				}
			}
		}
		break;
	case DEL_METHOD_GUTMANN:
		m_totalSize.QuadPart = m_totalSize.QuadPart *35;
		for(i=0;i<35;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 31:
			case 32:
			case 33:
			case 34:
				InitDelBuf(m_pBuf,0,0,0,TRUE,TRUE);
				break;
			case 4:
				InitDelBuf(m_pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 5:
				InitDelBuf(m_pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 6:
			case 25:
				InitDelBuf(m_pBuf,0X92,0X49,0X24,FALSE,FALSE);
				break;
			case 7:
			case 26:
				InitDelBuf(m_pBuf,0X49,0X24,0X92,FALSE,FALSE);
				break;
			case 8:
			case 27:
				InitDelBuf(m_pBuf,0X24,0X92,0X49,FALSE,FALSE);
				break;
			case 9:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 10:
				InitDelBuf(m_pBuf,0X11,0,0,FALSE,TRUE);
				break;
			case 11:
				InitDelBuf(m_pBuf,0X22,0,0,FALSE,TRUE);
				break;
			case 12:
				InitDelBuf(m_pBuf,0X33,0,0,FALSE,TRUE);
				break;
			case 13:
				InitDelBuf(m_pBuf,0X44,0,0,FALSE,TRUE);
				break;
			case 14:
				InitDelBuf(m_pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 15:
				InitDelBuf(m_pBuf,0X66,0,0,FALSE,TRUE);
				break;
			case 16:
				InitDelBuf(m_pBuf,0x77,0,0,FALSE,TRUE);
				break;
			case 17:
				InitDelBuf(m_pBuf,0X88,0,0,FALSE,TRUE);
				break;
			case 18:
				InitDelBuf(m_pBuf,0X99,0,0,FALSE,TRUE);
				break;
			case 19:
				InitDelBuf(m_pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 20:
				InitDelBuf(m_pBuf,0XBB,0,0,FALSE,TRUE);
				break;
			case 21:
				InitDelBuf(m_pBuf,0XCC,0,0,FALSE,TRUE);
				break;
			case 22:
				InitDelBuf(m_pBuf,0XDD,0,0,FALSE,TRUE);
				break;
			case 23:
				InitDelBuf(m_pBuf,0XEE,0,0,FALSE,TRUE);
				break;
			case 24:
				InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 28:
				InitDelBuf(m_pBuf,0X6D,0XB6,0XDB,FALSE,FALSE);
				break;
			case 29:
				InitDelBuf(m_pBuf,0XB6,0XDB,0X6D,FALSE,FALSE);
				break;
			case 30:
				InitDelBuf(m_pBuf,0XDB,0X6D,0XB6,FALSE,FALSE);
				break;
			}
			if(i==34)
			{
				if(!DelAll(TRUE))				
				{
					bResult = FALSE;
					break;
				}
			}
			else
			{
				if(!DelAll(FALSE))				
				{
					bResult = FALSE;
					break;
				}
			}
		}
		break;
	default:
		bResult = FALSE;
	}
	free(m_pBuf);
	m_pBuf = 0;
	return bResult;
}

BOOL DeleteLibrary::DelAll(BOOL bDel)
{
	for (vector<const TCHAR*>::iterator it = m_pDeletePathSet->begin(); it != m_pDeletePathSet->end(); ++it)
	{
		CString path = *it;
		if(!DelDirAndFiles(path,  bDel))
		{
			return FALSE;
		}
		if (m_toStopCurrentTask)
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL DeleteLibrary::DoDeleteFileInRecycled(const TCHAR *pszDir,BOOL bDel)
{
	HANDLE hFile;
	TCHAR *p;
	DWORD i,j;
	DWORD dwRWTimes;
	DWORD dwRW;
	DWORD dwFileSizeLow,dwFileSizeHigh;
	TCHAR szFileName[MAX_PATH];

	if (m_toStopCurrentTask)
	{
		return FALSE;
	}

	hFile = CreateFile(pszDir,
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	dwFileSizeLow = GetFileSize(hFile,&dwFileSizeHigh);
	i = dwFileSizeLow / DM_DEL_SIZE + dwFileSizeHigh * 0x4000;
	j = dwFileSizeLow % DM_DEL_SIZE;

	for(dwRWTimes=0;dwRWTimes<i;dwRWTimes++)
	{
		WriteFile(hFile,m_pBuf,DM_DEL_SIZE,&dwRW,NULL);
		m_processedSize.QuadPart += DM_DEL_SIZE;
	}
	if(j)
	{
		WriteFile(hFile,m_pBuf,j,&dwRW,NULL);
		m_processedSize.QuadPart += j;
	}
	FlushFileBuffers(hFile);
	CloseHandle(hFile);

	if(bDel) 
	{
		_tcscpy(szFileName,pszDir);
		p = _tcsrchr(szFileName,_T('\\'));
		p++;
		_tcscpy(p,RENAME_STRING);
		MoveFile(pszDir,szFileName);
		DeleteFile(szFileName);
		for(i=65;i<=90;i++)
		{
			*p = (TCHAR)i;
			hFile = CreateFile(szFileName, 
				GENERIC_READ|GENERIC_WRITE, 
				FILE_SHARE_READ |FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			CloseHandle(hFile);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (TCHAR)i;
			hFile = CreateFile(szFileName, 
				GENERIC_READ|GENERIC_WRITE, 
				FILE_SHARE_READ |FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			CloseHandle(hFile);
		}
		p--;
		_tcscpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (TCHAR)i;
			DeleteFile(szFileName);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (TCHAR)i;
			DeleteFile(szFileName);
		}
	}

	return TRUE;
}

BOOL DeleteLibrary::DeleteFileInRecycled(const TCHAR *pszDir,BOOL bDel)
{
	WIN32_FIND_DATA FindData;
	HANDLE hFind;
	DWORD dwAttrs;
	TCHAR recycledFilePath[MAX_PATH];
	TCHAR findFileName[MAX_PATH];
	TCHAR szDirName[MAX_PATH];
	TCHAR szTempDirectory[MAX_PATH];
	TCHAR *p;
	int i;

	if (m_toStopCurrentTask)
	{
		return FALSE;
	}

#ifdef UNICODE
	swprintf(recycledFilePath,L"%s\\*.*",pszDir);
#else
	sprintf(recycledFilePath,"%s\\*.*",pszDir);
#endif // UNICODE

	hFind = FindFirstFile(recycledFilePath,&FindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}

	do
	{
		if ((_tcscmp(FindData.cFileName,_T(".")) != 0) && (_tcscmp(FindData.cFileName,_T(".."))) != 0)
		{
			_tcscpy(findFileName,FindData.cFileName);

			if (_tcscmp(findFileName,_T("desktop.ini")) && _tcscmp(findFileName,_T("INFO2")))
			{
				if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
#ifdef UNICODE
					swprintf(szTempDirectory,L"%s\\%s",pszDir,findFileName);
#else
					sprintf(szTempDirectory,"%s\\%s",pszDir,findFileName);
#endif // UNICODE
					
					dwAttrs = GetFileAttributes(szTempDirectory);
					SetFileAttributes(szTempDirectory,FILE_ATTRIBUTE_NORMAL);
					if (!DoDeleteFileInRecycled(szTempDirectory,bDel))
					{
						ULARGE_INTEGER temp;
						temp.LowPart = FindData.nFileSizeLow;
						temp.HighPart = FindData.nFileSizeHigh;
						m_processedSize.QuadPart += temp.QuadPart;
						SetFileAttributes(szTempDirectory,dwAttrs);

						if (bDel && !m_toStopCurrentTask)
						{
							WriteLog(pszDir, FALSE);
						}
					}
					else
					{
						if (bDel)
						{
							WriteLog(pszDir, TRUE);
						}
					}
				}
				else
				{
#ifdef UNICODE
					swprintf(szTempDirectory,L"%s\\%s",pszDir,findFileName);
#else
					sprintf(szTempDirectory,"%s\\%s",pszDir,findFileName);
#endif // UNICODE
					
					DeleteFileInRecycled(szTempDirectory,bDel);
				}
			}
		}
	}while(FindNextFile(hFind,&FindData));

	FindClose(hFind);

	SetFileAttributes(pszDir,FILE_ATTRIBUTE_NORMAL);

	if(bDel)
	{
		RemoveDirectory(pszDir);

		_tcscpy(szDirName,pszDir);
		p = _tcsrchr(szDirName,_T('\\'));
		p++;
		_tcscpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (TCHAR)i;
			CreateDirectory(szDirName,NULL);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (TCHAR)i;
			CreateDirectory(szDirName,NULL);
		}
		p--;
		_tcscpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (TCHAR)i;
			RemoveDirectory(szDirName);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (TCHAR)i;
			RemoveDirectory(szDirName);
		}
	}

	return TRUE;
}

BOOL DeleteLibrary::DelAllInRecycled(BOOL bDel)
{
	TCHAR driverLetters[MAX_PATH];
	TCHAR diskLetter[4] = _T("C:\\");
	TCHAR szRecyclePath[MAX_PATH];
	int n;
	int i,j,k=0;
	int nCurrentUserNum = 0;
	TCHAR  FileSystemNameBuffer[10];
	TCHAR  chAllSIDName[MAX_PATH][MAX_PATH];
	TCHAR  chTempPath[MAX_PATH];
	DWORD MaximumConponentLenght;
	DWORD FileSystemFlags;
	DWORD dwFileAttri;
	OSVERSIONINFO osvi;
	BOOL bOsVersionInfoEx = FALSE;
	BOOL bCurrentOsVersionVista;
	TCHAR chSubKeyName[MAX_PATH];
	UINT nIndex = 0;

	HKEY hKey;
	DWORD dwSubLen;
	DWORD dwType = REG_SZ;
	DWORD dwSize = MAX_PATH;
	LONG lRet;
	FILETIME ftLastWriteTime;

	TCHAR     achClass[MAX_PATH] = _T("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys=0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 

	GetLogicalDriveStrings(MAX_PATH,driverLetters);
	n = sizeof(driverLetters);
	ZeroMemory(&osvi,sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if ( !(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *) &osvi)) )
	{
		return FALSE;
	}

	if (osvi.dwMajorVersion >= 6)
	{
		bCurrentOsVersionVista = TRUE;
	}

	if (osvi.dwMajorVersion == 5)
	{
		bCurrentOsVersionVista = FALSE;
	}

	lRet = RegOpenKeyEx(HKEY_USERS,_T(""),0,KEY_READ,&hKey);

	if (lRet != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lRet = RegQueryInfoKey(
		hKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time

	if (lRet != ERROR_SUCCESS)
	{
		return FALSE;
	}

	for (nIndex = 0;nIndex < cSubKeys;nIndex++)
	{
		dwSubLen = MAX_PATH;
		lRet = RegEnumKeyEx(hKey,nIndex,
			chSubKeyName,
			&dwSubLen,
			NULL,
			NULL,
			NULL,
			&ftLastWriteTime);
		if (lRet != ERROR_SUCCESS)
		{
			return FALSE;
		}
		else
		{
			if (dwSubLen > 8 && (_tcsstr(chSubKeyName,_T("Classes")) == NULL))
			{
				_tcscpy(chAllSIDName[k],chSubKeyName);
				k++;
				nCurrentUserNum++;
			}
		}
	}

	RegCloseKey(hKey);

	for (i = 0;i < n;i += 4)
	{
		diskLetter[0] = driverLetters[i];

		if (GetDriveType(&driverLetters[i]) == DRIVE_FIXED)
		{
			GetVolumeInformation(&driverLetters[i],NULL,0,NULL,
				&MaximumConponentLenght,&FileSystemFlags,
				FileSystemNameBuffer,10);
			_tcscpy(szRecyclePath,diskLetter);

			if (bCurrentOsVersionVista)
			{
				if (!_tcscmp(FileSystemNameBuffer,_T("NTFS")))
				{
					_tcscat(szRecyclePath,_T("$Recycle.Bin\\"));

					for (j = 0;j < nCurrentUserNum;j++)
					{
						_tcscpy(chTempPath,szRecyclePath);
						_tcscat(chTempPath,chAllSIDName[j]);
						dwFileAttri = GetFileAttributes(chTempPath);
						DeleteFileInRecycled(chTempPath,bDel);
						SetFileAttributes(chTempPath,dwFileAttri);
						dwFileAttri = GetFileAttributes(szRecyclePath);
						SetFileAttributes(szRecyclePath,dwFileAttri);
						ZeroMemory(chTempPath,MAX_PATH*sizeof(TCHAR));

						if (m_toStopCurrentTask)
						{
							return FALSE;
						}
					}
				}
				else
				{
					_tcscat(szRecyclePath,_T("$Recycle.Bin"));
					dwFileAttri = GetFileAttributes(szRecyclePath);
					DeleteFileInRecycled(szRecyclePath,bDel);
					SetFileAttributes(szRecyclePath,dwFileAttri);

					if (m_toStopCurrentTask)
					{
						return FALSE;
					}
				}
			}
			else
			{
				if (!_tcscmp(FileSystemNameBuffer,_T("NTFS")))
				{
					_tcscat(szRecyclePath,_T("RECYCLER\\"));

					for (j = 0;j < nCurrentUserNum;j++)
					{
						_tcscpy(chTempPath,szRecyclePath);
						_tcscat(chTempPath,chAllSIDName[j]);
						dwFileAttri = GetFileAttributes(chTempPath);
						DeleteFileInRecycled(chTempPath,bDel);
						SetFileAttributes(chTempPath,dwFileAttri);
						ZeroMemory(chTempPath,MAX_PATH*sizeof(TCHAR));

						if (m_toStopCurrentTask)
						{
							return FALSE;
						}
					}
				}
				else
				{
					_tcscat(szRecyclePath,_T("Recycled"));
					dwFileAttri = GetFileAttributes(szRecyclePath);
					DeleteFileInRecycled(szRecyclePath,bDel);
					SetFileAttributes(szRecyclePath,dwFileAttri);

					if (m_toStopCurrentTask)
					{
						return FALSE;
					}
				}
			}
		}

	}

	return TRUE;
}

BOOL DeleteLibrary::DoDeleteMasterDelFileInRecycled()
{
	int			i;
	BOOL        bResult = TRUE;
	CString		csText,csCaption;

	m_pBuf = (BYTE*)malloc(DM_DEL_SIZE);
	if(!m_pBuf) 
	{
		return FALSE;
	}

	switch(m_method)
	{
	case DEL_METHOD_00:
		InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
		bResult = DelAllInRecycled(TRUE);
		break;
	case DEL_METHOD_FF:
		InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
		bResult = DelAllInRecycled(TRUE);
		break;
	case DEL_METHOD_RANDOM:
		InitDelBuf(m_pBuf,0,0,0,TRUE,TRUE);
		bResult = DelAllInRecycled(TRUE);
		break;
	case DEL_METHOD_NSA:
		m_totalSize.QuadPart = m_totalSize.QuadPart *3;
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
				InitDelBuf(m_pBuf,0,0,0,TRUE,TRUE);
				break;
			case 2:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			}
			if(i==2)
			{
				if(!DelAllInRecycled(TRUE))
				{
					bResult = FALSE;
					break;
				}
			}
			else
			{
				if(!DelAllInRecycled(FALSE))
				{
					bResult = FALSE;
					break;
				}
			}
		}
		break;
	case DEL_METHOD_OLD_NSA:
		m_totalSize.QuadPart = m_totalSize.QuadPart *4;
		for(i=0;i<4;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
				InitDelBuf(m_pBuf,0xFF,0,0,FALSE,TRUE);
				break;
			}
			if(i==3)
			{
				if(!DelAllInRecycled(TRUE))
				{
					bResult = FALSE;
					break;
				}
			}
			else
			{
				if(!DelAllInRecycled(FALSE))
				{
					bResult = FALSE;
					break;
				}
			}
		}
		break;
	case DEL_METHOD_DOD_STD:
		m_totalSize.QuadPart = m_totalSize.QuadPart *3;
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
				InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 2:
				InitDelBuf(m_pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(i==2)
			{
				if(!DelAllInRecycled(TRUE))
				{
					bResult = FALSE;
					break;
				}
			}
			else
			{
				if(!DelAllInRecycled(FALSE))
				{
					bResult = FALSE;
					break;
				}
			}
		}
		break;
	case DEL_METHOD_NATO:
		m_totalSize.QuadPart = m_totalSize.QuadPart *7;
		for(i=0;i<7;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
			case 4:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
			case 5:
				InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 6:
				InitDelBuf(m_pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(i==6)
			{
				if(!DelAllInRecycled(TRUE))
				{
					bResult = FALSE;
					break;
				}
			}
			else
			{
				if(!DelAllInRecycled(FALSE))
				{
					bResult = FALSE;
					break;
				}
			}
		}
		break;
	case DEL_METHOD_GUTMANN:
		m_totalSize.QuadPart = m_totalSize.QuadPart *35;
		for(i=0;i<35;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 31:
			case 32:
			case 33:
			case 34:
				InitDelBuf(m_pBuf,0,0,0,TRUE,TRUE);
				break;
			case 4:
				InitDelBuf(m_pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 5:
				InitDelBuf(m_pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 6:
			case 25:
				InitDelBuf(m_pBuf,0X92,0X49,0X24,FALSE,FALSE);
				break;
			case 7:
			case 26:
				InitDelBuf(m_pBuf,0X49,0X24,0X92,FALSE,FALSE);
				break;
			case 8:
			case 27:
				InitDelBuf(m_pBuf,0X24,0X92,0X49,FALSE,FALSE);
				break;
			case 9:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 10:
				InitDelBuf(m_pBuf,0X11,0,0,FALSE,TRUE);
				break;
			case 11:
				InitDelBuf(m_pBuf,0X22,0,0,FALSE,TRUE);
				break;
			case 12:
				InitDelBuf(m_pBuf,0X33,0,0,FALSE,TRUE);
				break;
			case 13:
				InitDelBuf(m_pBuf,0X44,0,0,FALSE,TRUE);
				break;
			case 14:
				InitDelBuf(m_pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 15:
				InitDelBuf(m_pBuf,0X66,0,0,FALSE,TRUE);
				break;
			case 16:
				InitDelBuf(m_pBuf,0x77,0,0,FALSE,TRUE);
				break;
			case 17:
				InitDelBuf(m_pBuf,0X88,0,0,FALSE,TRUE);
				break;
			case 18:
				InitDelBuf(m_pBuf,0X99,0,0,FALSE,TRUE);
				break;
			case 19:
				InitDelBuf(m_pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 20:
				InitDelBuf(m_pBuf,0XBB,0,0,FALSE,TRUE);
				break;
			case 21:
				InitDelBuf(m_pBuf,0XCC,0,0,FALSE,TRUE);
				break;
			case 22:
				InitDelBuf(m_pBuf,0XDD,0,0,FALSE,TRUE);
				break;
			case 23:
				InitDelBuf(m_pBuf,0XEE,0,0,FALSE,TRUE);
				break;
			case 24:
				InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 28:
				InitDelBuf(m_pBuf,0X6D,0XB6,0XDB,FALSE,FALSE);
				break;
			case 29:
				InitDelBuf(m_pBuf,0XB6,0XDB,0X6D,FALSE,FALSE);
				break;
			case 30:
				InitDelBuf(m_pBuf,0XDB,0X6D,0XB6,FALSE,FALSE);
				break;
			}
			if(i==34)
			{
				if(!DelAllInRecycled(TRUE))
				{
					bResult = FALSE;
					break;
				}
			}
			else
			{
				if(!DelAllInRecycled(FALSE))
				{
					bResult = FALSE;
					break;
				}
			}
		}
		break;
	default:
		bResult = FALSE;
	}
	free(m_pBuf);
	m_pBuf = 0;

	return bResult;
}

bool DeleteLibrary::CheckIsNormalPath(const TCHAR* path)
{
	TCHAR temp[MAX_PATH];
	ZeroMemory(temp, sizeof(temp));
	_tcscpy(temp, path);
	return DeleteLibraryHelper::IsNormalPath(temp);
}

ULARGE_INTEGER DeleteLibrary::GetTotalSizeOfProcessing()
{
	return m_totalSize;
}

ULARGE_INTEGER DeleteLibrary::GetSizeOfProcessed()
{
	return m_processedSize;
}
