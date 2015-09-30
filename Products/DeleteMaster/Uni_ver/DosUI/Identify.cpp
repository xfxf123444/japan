#include "comm.h"
#include "main.h"
#include <conio.h>
#include "intrrpt.h"

extern 	_sysinfo					g_SysInfo;     
extern	YG_PARTITION_INFO			*g_pCurFixDiskInfo;
extern	_deletediskinfo				g_deldiskinfo;
extern	_deleteparinfo              g_delparinfo;
extern	_diskviewinfo				g_diskviewinfo;
extern  BYTE						g_btCurFun;
extern	int							g_nMethod;
extern  ATA_DEVICE_TYPE				g_DevType[4];

extern void (_interrupt far *PrevTimerHandler)();
extern void (_interrupt far *PrevIntHandlerPri)();
extern void (_interrupt far *PrevIntHandlerSec)();

ATA_IDENTIFY_INFO 	AtaIdentifyInfo;
//unsigned short wIoBase = PRIMARY_IDE_IO_BASE;
//int nDevice = ATA_MASTER;
char showidentifywin()
{
	char			ch;
	int				nWidth,nselect;
	char			*str[] = {"[O]k"};
	static point	ptok;
	char			szDiskInfo[80];

	_setvisualpage(PAGE_IDENTIFY);  
	_setactivepage(PAGE_IDENTIFY);  
    
    drawbox(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);

	_settextposition(3, 34);          
	_outtext("Disk Indentify");
    drawbox(2, 30, 4, 50);


	nWidth = strlen(str[0]);
	ptok.row = 23;
	ptok.col = g_SysInfo.nmaxcols - 2 - nWidth;	           
	_settextposition(ptok.row, ptok.col);
	_outtext(str[0]);  
	drawbox(ptok.row - 1, ptok.col - 1, ptok.row + 1, ptok.col + nWidth);

	PrevIntHandlerPri = _dos_getvect(0x76); 
	PrevIntHandlerSec = _dos_getvect(0x77);
	PrevTimerHandler = _dos_getvect(0x08);
	_dos_setvect(0x76, HarddiskIntHandlerPri);
	_dos_setvect(0x77, HarddiskIntHandlerSec);
	_dos_setvect(0x08, TimerHandler);
	
	identify(g_DevType[g_deldiskinfo.nSelDisk-DISKBASE].wIoBase,g_DevType[g_deldiskinfo.nSelDisk-DISKBASE].nDevice);
	
	_dos_setvect(0x76, PrevIntHandlerPri);
	_dos_setvect(0x77, PrevIntHandlerSec);
	_dos_setvect(0x08, PrevTimerHandler);
	// print ATA information.
	
	sprintf(szDiskInfo,"Vender Name  : %s\n", AtaIdentifyInfo.ModelNumber);
	_settextposition(7,22);
	_outtext(szDiskInfo);
	sprintf(szDiskInfo,"Firmware Rer : %s\n", AtaIdentifyInfo.FirmwareRevision);
	_settextposition(8,22);
	_outtext(szDiskInfo);
	sprintf(szDiskInfo,"Serial Number: %s\n", AtaIdentifyInfo.SerialNumber);
	_settextposition(9,22);
	_outtext(szDiskInfo);
	sprintf(szDiskInfo,"Capicity     : %lu sectors, %lu MB\n", 
			AtaIdentifyInfo.TotalNumberUserAddressableSectors, 
			AtaIdentifyInfo.TotalNumberUserAddressableSectors / 2048);
	_settextposition(10,22);
	_outtext(szDiskInfo);
    

	//confirmshowselected();
	nselect = 0;
	selectstr(str[0], ptok.row, ptok.col, SELCOLOR);
	while(1)
	{                                               
	  	ch = _getch();
		switch(ch)
		{
		case 'O':
		case 'o':
			_clearscreen(_GCLEARSCREEN); 
			return 'o';
		case 'C':
		case 'c':
		case ESC :
			_clearscreen(_GCLEARSCREEN); 
			return 'o';
		case CR:
			_clearscreen(_GCLEARSCREEN);
			return 'o';
		}
	}
}

int WaitUntilNotBusyOrTimeout(WORD wIoBase,int nTicks)
{
	NPATA_TASK_FILE	npTaskFile;
	unsigned char		btStatus;
	int				nTickCount = 0;
	
	npTaskFile = (NPATA_TASK_FILE)wIoBase;
	do
	{
		btStatus = inp((unsigned short)&npTaskFile->Command_Status);

		if (btStatus & 0x80)
		{
			HardwareWait(1);
			nTickCount++;
		}
		
	} while ((btStatus & 0x80) && nTickCount < nTicks); // Check the BSY bit, 1 means busy 

	if (nTickCount >= nTicks)
		return ERR_IOM_COMMAND_NOT_SUPPORTED;

	return ERR_SUCCESS;
}

int IsDriveReady(WORD wIoBase)
{
	NPATA_TASK_FILE		npTaskFile;
	unsigned char		btStatus;
	
	npTaskFile = (NPATA_TASK_FILE)wIoBase;
	btStatus = inp((unsigned short)&npTaskFile->Command_Status);
	return (btStatus & 0x40); // Check the RDY bit, 1 means the drive is ready
}

int IsDataReady(WORD wIoBase)
{
	NPATA_TASK_FILE	npTaskFile;
	unsigned char		btStatus;
	
	npTaskFile = (NPATA_TASK_FILE)wIoBase;
	btStatus = inp((unsigned short)&npTaskFile->Command_Status);
	return (btStatus & 0x08); // Check the DRQ bit, 1 means data available
}
                    
int IsThereAnError(WORD wIoBase)
{
	NPATA_TASK_FILE	npTaskFile;
	unsigned char		btStatus;
	
	npTaskFile = (NPATA_TASK_FILE)wIoBase;
	btStatus = inp((unsigned short)&npTaskFile->Command_Status);
	return (btStatus & 0x01); // Check the ERR bit, 1 means there is an error
}
                    
void identify(WORD wIoBase,int nDevice)
{
	NPATA_TASK_FILE 	npTaskFile;
	unsigned short 		*data;
	int					i;
	int					result;

	memset(&AtaIdentifyInfo, 0, sizeof(AtaIdentifyInfo));
	data = (unsigned short *)&AtaIdentifyInfo;

	npTaskFile = (NPATA_TASK_FILE)wIoBase;

	// Setup Head/Dev register required by the command
	// 10100000
	outp((unsigned short)&npTaskFile->Head, (unsigned char)(0xA0 | (nDevice << 4)));

	// Wait for 2 ticks before reading the status register
	HardwareWait(2);

	result = WaitUntilNotBusyOrTimeout(wIoBase,1);
	if (result != 0)
	{
		_settextposition(5, 4);
		_outtext("command timeout.");  
		return;
	}

	if (IsDriveReady(wIoBase) == 0)
	{
		_settextposition(5, 4);
		_outtext("command not support.");  
		return;
	}

	// Write the command code for IDENTIFY DEVICE
	outp((unsigned short)&npTaskFile->Command_Status,0xEC);
	HardwareWait(2);
	result= WaitUntilNotBusyOrTimeout(wIoBase,1);
	if (result != 0)
	{
		_settextposition(5, 4);
		_outtext("command timeout.");  
		return;
	}

	if (IsThereAnError(wIoBase) == 0)
	{
		if (IsDataReady(wIoBase) != 0)
		{
			for (i = 0 ; i < 256 ; i++)
			{
				*data = inpw((unsigned short)&npTaskFile->Data);
				data++;
			}

			// swap name.
			for (i = 0 ; i < 40 ; i += 2)
			{
				result = AtaIdentifyInfo.ModelNumber[i];
				AtaIdentifyInfo.ModelNumber[i] = AtaIdentifyInfo.ModelNumber[i+1];
				AtaIdentifyInfo.ModelNumber[i+1] = (char)result;
			}
			AtaIdentifyInfo.ModelNumber[39] = 0;

			// swap serial number.
			for (i = 0 ; i < 20 ; i += 2)
			{
				result = AtaIdentifyInfo.SerialNumber[i];
				AtaIdentifyInfo.SerialNumber[i] = AtaIdentifyInfo.SerialNumber[i + 1];
				AtaIdentifyInfo.SerialNumber[i + 1] = (char)result;
			}
			AtaIdentifyInfo.SerialNumber[19] = 0;

			// swap firmware version
			for (i = 0 ; i < 8 ; i += 2)
			{
				result = AtaIdentifyInfo.FirmwareRevision[i];
				AtaIdentifyInfo.FirmwareRevision[i] = AtaIdentifyInfo.FirmwareRevision[i + 1];
				AtaIdentifyInfo.FirmwareRevision[i + 1] = (char)result;
			}
			AtaIdentifyInfo.FirmwareRevision[7] = 0;
		}
		else
		{
			_settextposition(5, 4);
			_outtext("command not support.");  
			return;
		}
	}
}

// Read sectors using ATA command
int ATARead(WORD wIoBase, int nDevice, DWORD dwAddr, DWORD dwSectors, LPBYTE lpBuf)
{
	NPATA_TASK_FILE		npTaskFile;
	BYTE				btDeviceHead;
	LPWORD				lpData;
	int					i;
	
	npTaskFile = (NPATA_TASK_FILE)wIoBase;

	// Wait for the busy bit to clear
	if(ERR_SUCCESS != WaitUntilNotBusyOrTimeout(wIoBase,2))
		return ERR_IOM_COMMAND_NOT_SUPPORTED;	
	
	// Setup Head/Dev register required by the command
	// 11100000, the bit 6 indicates LBA read
	btDeviceHead = 0xE0;
	// Set the least significant 4 bits to be LBA address's high bits
	btDeviceHead |= (HIBYTE(HIWORD(dwAddr)) & 0x0F) | (nDevice << 4);
	outp((WORD)&npTaskFile->Head,btDeviceHead);
	
	// Wait for 2 ticks before reading the status register
	HardwareWait(2);
	if(ERR_SUCCESS != WaitUntilNotBusyOrTimeout(wIoBase,1))
		return ERR_IOM_COMMAND_NOT_SUPPORTED;	
	if (!IsDriveReady(wIoBase))
		return ERR_IOM_COMMAND_NOT_SUPPORTED;
	
	// Setup other parameter registers 
	// Set the number of sectors to read
	outp((WORD)&npTaskFile->SctorCnt,LOBYTE(LOWORD(dwSectors)));
	// Set the first byte of LBA address
	outp((WORD)&npTaskFile->SectorNum,LOBYTE(LOWORD(dwAddr)));
	// Set the second byte of LBA address
	outp((WORD)&npTaskFile->CylinderLSB,HIBYTE(LOWORD(dwAddr)));
	// Set the third byte of LBA address
	outp((WORD)&npTaskFile->CylinderMSB,LOBYTE(HIWORD(dwAddr)));
	
	// Issue command READ SECTORS
	outp((WORD)&npTaskFile->Command_Status,0x20);
	HardwareWait(2);
	if(ERR_SUCCESS != WaitUntilNotBusyOrTimeout(wIoBase,1))
		return ERR_IOM_COMMAND_NOT_SUPPORTED;	
	if (!IsThereAnError(wIoBase))
	{
		if (IsDataReady(wIoBase))
		{
			// No error 
			// Retrieve data  
			lpData = (LPWORD)lpBuf;
			for (i = 0; i < (int)dwSectors*SECTOR_SIZE/2; i++)
			{
				*lpData = inpw((WORD)&npTaskFile->Data);
				lpData++;
			}
		}
	}
	return ERR_SUCCESS;
}
