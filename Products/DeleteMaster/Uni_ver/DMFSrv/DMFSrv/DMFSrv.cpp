// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1997  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   DMFSrv.c
//
//  FUNCTIONS:
//            ServiceStart(DWORD dwArgc, LPTSTR *lpszArgv);
//            ServiceStop( );
//
//  COMMENTS: The functions implemented in DMFSrv.c are
//            prototyped in service.h
//              


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <tchar.h>
#include "service.h"
#include "SecureDelete.h"

//FILE* fsrv;
// this event is signalled when the
// service should end
//
HANDLE  hServerStopEvent = NULL;
BOOL g_bCancelDelete = FALSE;

//
//  FUNCTION: ServiceStart
//
//  PURPOSE: Actual code of the service
//           that does the work.
//
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//	The service stops when hServerStopEvent is signalled
//
VOID ServiceStart (DWORD dwArgc, LPTSTR *lpszArgv)
{
    DWORD                   dwWait;
	SYSTEMTIME              SystemTime;

    ///////////////////////////////////////////////////
    //
    // Service initialization
    //

    // report the status to the service control manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_START_PENDING, // service state
        NO_ERROR,              // exit code
        3000))                 // wait hint
        goto cleanup;

    // create the event object. The control handler function signals
    // this event when it receives the "stop" control code.
    //
    hServerStopEvent = CreateEvent(
        NULL,    // no security attributes
        TRUE,    // manual reset event
        FALSE,   // not-signalled
        NULL);   // no name

    if ( hServerStopEvent == NULL)
        goto cleanup;

    // report the status to the service control manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_RUNNING,       // service state
        NO_ERROR,              // exit code
        0))                    // wait hint
        goto cleanup;

    ////////////////////////////////////////////////////////
    //
    // Service is now running, perform work until shutdown
    //
	//fsrv = fopen("c:\\fsrv.txt", "w");
	//fwrite("service starts2\n", strlen("service starts1\n"), 1, fsrv);
	//fflush(fsrv);
	//GetLocalTime(&SystemTime);
	//ExcuteLostSchedule(&SystemTime);

    while ( 1 )
    {
        dwWait = WaitForSingleObject(hServerStopEvent,60000);
        if ( dwWait == WAIT_OBJECT_0 )     // server stop signaled
			break;
		GetLocalTime(&SystemTime);
		ExcuteLostSchedule(&SystemTime);
		GetLocalTime(&SystemTime);
		ExcuteSchedule(&SystemTime);
    }
	//fclose(fsrv);
cleanup:

    if (hServerStopEvent)
        CloseHandle(hServerStopEvent);

}

//
//  FUNCTION: ServiceStop
//
//  PURPOSE: Stops the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    If a ServiceStop procedure is going to
//    take longer than 3 seconds to execute,
//    it should spawn a thread to execute the
//    stop code, and return.  Otherwise, the
//    ServiceControlManager will believe that
//    the service has stopped responding.
//    
VOID ServiceStop()
{
    if ( hServerStopEvent )
        SetEvent(hServerStopEvent);
}
