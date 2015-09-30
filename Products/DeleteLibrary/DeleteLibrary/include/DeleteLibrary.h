#ifndef   DELETE_LIBRARY_INCLUDED    
#define   DELETE_LIBRARY_INCLUDED 

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <atlstr.h>
#include <atltime.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <vector>

#ifdef DELETELIBRARY_EXPORTS
#define DELETELIBRARY_API __declspec(dllexport)
#else
#define DELETELIBRARY_API __declspec(dllimport)
#endif

// delete method
enum DeleteMethod
{
	DEL_METHOD_00 = 0,	// erase file with all digit 0
	DEL_METHOD_FF,	// erase file with all digit 1
	DEL_METHOD_RANDOM,	// erase file with random value
	DEL_METHOD_NSA,		// erase file with NSA Standard 
	DEL_METHOD_OLD_NSA,	// erase file with old NSA Standard 
	DEL_METHOD_DOD_STD, // erase file with DOD Standard 
	DEL_METHOD_NATO,// erase file with NATO Standard 
	DEL_METHOD_GUTMANN // erase file with gutmann algorithm 
};

// operation return value
enum DeleteOperationResult
{
	DEL_RESULT_OK = 0,	//	Operation executes OK.
	DEL_RESULT_OPERATION_CANNOT_BE_EXECUTED,	// Operation cannot be executed.
	DEL_RESULT_INVALID_FILE_PATH,				// Inputing file path is invalid
	DEL_RESULT_INVALID_PARAMETER,				// Inputing parameter is invalid
	DEL_RESULT_UNKNOWN_ERROR					// Unknown unexpected error occurred
};

class DELETELIBRARY_API DeleteLibrary
{
public:
	DeleteLibrary();
	~DeleteLibrary();

	// set delete method.
	// If deleting is in progress, method cannot be set
	DeleteOperationResult SetDeleteMethod(DeleteMethod method);
	// get delete method
	DeleteMethod GetDeleteMethod();
	// set log path.
	// If deleting is in progress, log path cannot be set
	// When deleting some file failed, log will be output to the log path file.
	// Log format: "Delete (file path), User:(user name), Method:(method), Time:(execution time), Result:(failure)"
	// If log path is not be set or set with empty string, log will not be output
	// logPath will be deep copied immediately after entering function.
	DeleteOperationResult SetLogPath(const TCHAR* logPath);
	// get log path
	// the life span of the returned const TCHAR* lasts until when the next time SetLogPath function is called or DeleteLibrary is destoryed.
	const TCHAR* GetLogPath();
	// get total size of all file to delete.
	// NOTE: This value is timed by erase times of each delete method
	// For example, delete a file of size 1000 with method NSA, the value = 1000 * 3 = 3000
	//  DEL_METHOD_00 = 1
	//	DEL_METHOD_FF = 1
	//	DEL_METHOD_RANDOM = 1
	//	DEL_METHOD_NSA = 3 
	//	DEL_METHOD_OLD_NSA = 4 
	//	DEL_METHOD_DOD_STD = 5 
	//	DEL_METHOD_NATO = 7
	//	DEL_METHOD_GUTMANN = 35 
	ULARGE_INTEGER GetTotalSizeOfProcessing();
	// get total size of all file to delete.
	// NOTE: This value is timed by erase times of each delete method
	ULARGE_INTEGER GetSizeOfProcessed();
	// is deleting in progress
	bool IsDeletingInProgress();

	// start deleting all files and folders
	// "pDeletePathArray" may include several files' and folders' path, no matter duplicated or overlapped
	// "arrayCount" indicates how many path element in pDeletePathArray
	// if arrayCount equals to 1, pDeletePathArray will be treated as a normal TCHAR* path which ends with '\0'
	// if arrayCount is larger than 1, the count of TCHAR in pDeletePathArray is supposed to be MAX_PATH * arrayCount.
	// in that case, every MAX_PATH TCHAR contains a file or folder path which ends with '\0'.
	// pDeletePathArray will be deep copied immediately after entering function. 
	DeleteOperationResult StartDeletingTask(const TCHAR* pDeletePathArray, int arrayCount);
	// start deleting all files and folders in RecycledBin
	// user application should call windows API SHEmptyRecycleBin to make RecycleBin update its status after deleting finished.
	DeleteOperationResult StartDeletingRecycledBin();
	// stop current deleting task
	void StopDeletingTask();
	// check input file path is a normal path
	// The normal path is path not a "C:\" or "windows directory" or "program files" or "program files(x86)" 
	// or other users' directory or current application's directory
	// Deleting a none-normal path may cause critical error
	// path will be deep copied immediately after entering function. 
	static bool CheckIsNormalPath(const TCHAR* path); 

private:
	// filter duplicated or overlapped files' and folders' path
	void FilterDuplicatedOrSubFolder();
	// collect all files and folders to delete
	void CalculateDeleteFileSize();

	// thread
	static unsigned int WINAPI ThreadProc(void* pParm);

	// init parameter
	void Init();
	// clear parameter
	void Clear();
	// create write buffer
	bool InitDelBuf(BYTE* pBuf,BYTE btFir,BYTE btSec,BYTE btThr,bool bRand,bool bOneByte);

	// delete function for deleting files and folders
	BOOL DoDelFile(const TCHAR* pszDir,BOOL bDel);
	BOOL DelDirAndFiles(const TCHAR* pszDir,BOOL bDel);
	BOOL DoDeleteMasterDel();
	BOOL DelAll(BOOL bDel);

	// delete function for deleting RecycledBin
	BOOL DoDeleteFileInRecycled(const TCHAR* pszDir,BOOL bDel);
	BOOL DeleteFileInRecycled(const TCHAR* pszDir,BOOL bDel);
	BOOL DelAllInRecycled(BOOL bDel);
	BOOL DoDeleteMasterDelFileInRecycled();

	// output log
	void WriteLog(const TCHAR* szFile, BOOL bSucc);
	// convert delete method to string
	CString GetDeleteMethodString();
	// delete method
	DeleteMethod m_method;
	// log output path
	CString* m_pLogPath;
	// current user name
	CString* m_pUserName;
	// files' and folders' path to delete
	std::vector<const TCHAR*>* m_pDeletePathSet;
	// delete buffer
	BYTE* m_pBuf;
	// is in deleting
	bool m_isDeletingInProgress;
	// to stop current deleting task 
	bool m_toStopCurrentTask;
	// whether delete files in RecycledBin
	bool m_bDeleteRecycledBin;
	// total size of all file to delete, times by erase time
	ULARGE_INTEGER m_totalSize;
	// processed size of deleted file, times by erase time
	ULARGE_INTEGER m_processedSize;
};

#endif