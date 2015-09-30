#ifndef YGSCAPI_H
#define YGSCAPI_H
typedef struct
{
	char szComp[10];
	DWORD dwProduct;
	DWORD dwNumber;
}YGCSSTRU,*PYGCSSTRU;

BOOL YGSCGenerateSerial(char *szSerial,PYGCSSTRU pGenSerInfo);
BOOL YGSCCheckSerial(char *szCompany,char *szProduct,char *szNumber,char *szSerial);
BOOL JudgeWinXP();
BOOL JudgeWin2K();

// 2004.09.14 added begin
BOOL InstallXPDriver();
// 2004.09.14 added end

// 2004.09.13 added begin
const char YG_FILE_MONITOR_INF[MAX_PATH]="YGFSMon.inf";
const char YG_FILE_MONITOR_DRIVER[MAX_PATH]="YGFSMon.sys";
// 2004.09.13 added end

#endif