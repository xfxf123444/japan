/*=============================================================
ParManPrivate.cpp
WQ
=============================================================*/

#include "..\..\export\2000\ParMan.h"
#include "ParManPrivate.h"

DWORD QueryFormatProcess(HANDLE hChildStdoutRdDup,
						HANDLE hFormatProcess,
						HWND hWnd)
{
	int				nFinish = 0;
	DWORD			dwExitCode;

	for(;;)
	{
		Sleep(100);
		GetExitCodeProcess(hFormatProcess,&dwExitCode);
		if(dwExitCode == STILL_ACTIVE)
		{
			PostMessage(hWnd,WM_FORMAT_FLUSH_PROGRESS,100 * nFinish/(nFinish+100),0);
			nFinish++;
		}else
		{
			PostMessage(hWnd,WM_FORMAT_FLUSH_PROGRESS,100,0);
			break;
		}
	}
	return dwExitCode;
}

BOOL CreatePipes(HANDLE& hChildStdinRd,
				HANDLE& hChildStdinWr,
				HANDLE& hChildStdinWrDup,
				HANDLE& hChildStdoutRd, 
				HANDLE& hChildStdoutWr,
				HANDLE& hChildStdoutRdDup,
				HANDLE& hChildStderrRd, 
				HANDLE& hChildStderrWr,
				HANDLE& hChildStderrRdDup)
{
	SECURITY_ATTRIBUTES			sa;
	HANDLE						hCurProcess;

	hCurProcess = GetCurrentProcess();
	sa.nLength = sizeof(sa);
	//Can be inherit by child process
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	if(!(
		CreatePipe(&hChildStdinRd,&hChildStdinWr,&sa,0) &&
		CreatePipe(&hChildStdoutRd,&hChildStdoutWr,&sa,0) &&
		CreatePipe(&hChildStderrRd,&hChildStderrWr,&sa,0)	
		))
	{
		//fail
		ClosePipes(	hChildStdinRd,hChildStdinWr,
					hChildStdoutRd,hChildStdoutWr,
					hChildStderrRd,hChildStderrWr);
		return FALSE;
	}

	if	(
		DuplicateHandle(hCurProcess,hChildStdoutRd,
			hCurProcess,&hChildStdoutRdDup,0,FALSE,DUPLICATE_SAME_ACCESS) &&
		DuplicateHandle(hCurProcess,hChildStdinWr,
			hCurProcess,&hChildStdinWrDup,0,FALSE,DUPLICATE_SAME_ACCESS) &&
		DuplicateHandle(hCurProcess,hChildStderrRd,
			hCurProcess,&hChildStderrRdDup,0,FALSE,DUPLICATE_SAME_ACCESS)
		)
	{
		return TRUE;
	}else
	{
		ClosePipes(	hChildStdinRd,hChildStdinWr,
					hChildStdoutRd,hChildStdoutWr,
					hChildStderrRd,hChildStderrWr);
		return FALSE;
	}
}

void ClosePipes(HANDLE& hChildStdinRd, HANDLE& hChildStdinWr,
				HANDLE& hChildStdoutRd, HANDLE& hChildStdoutWr,
				HANDLE& hChildStderrRd, HANDLE& hChildStderrWr)
{
	CloseHandle(hChildStdinRd);
	CloseHandle(hChildStdinWr);
	CloseHandle(hChildStdoutRd);
	CloseHandle(hChildStdoutWr);
	CloseHandle(hChildStderrRd);
	CloseHandle(hChildStderrWr);
}