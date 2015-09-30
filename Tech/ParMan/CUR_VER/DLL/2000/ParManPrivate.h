/*=============================================================
ParManPrivate.h
WQ
=============================================================*/
#ifndef PARMAN_PRIVATE_H
#define PARMAN_PRIVATE_H

#pragma once

#include <windows.h>
#include "DiskMan.h"

/*----------------------------------------------------------------------
QueryFormatProcess, CreatePipes, ClosePipes
Purpose:
	For format use
----------------------------------------------------------------------*/
DWORD QueryFormatProcess(	HANDLE hChildStdoutRdDup,
							HANDLE hFormatProcess,
							HWND hWnd);

BOOL CreatePipes(HANDLE& hChildStdinRd, 
			HANDLE& hChildStdinWr,
			HANDLE& hChildStdinWrDup,
			HANDLE& hChildStdoutRd,
			HANDLE& hChildStdoutWr,
			HANDLE& hChildStdoutRdDup,
			HANDLE& hChildStderrRd,
			HANDLE& hChildStderrWr,
			HANDLE& hChildStderrRdDup);

void ClosePipes(HANDLE& hChildStdinRd, HANDLE& hChildStdinWr,
				HANDLE& hChildStdoutRd, HANDLE& hChildStdoutWr,
				HANDLE& hChildStderrRd, HANDLE& hChildStderrWr);

#endif//PARMAN_PRIVATE_H