/*
 * Interrupt.h
 */

#ifndef _INTERRUPT_
#define _INTERRUPT_
       
#include "comm.h"
       
#define ATA_MASTER	0
#define ATA_SLAVE	1

#define PRIMARY_IDE_IO_BASE 	0x1F0
#define SECONDARY_IDE_IO_BASE 	0x170

#define ERR_SUCCESS						0
#define ERR_IOM_COMMAND_NOT_SUPPORTED   1

typedef struct                                                	
{
	WORD		wIoBase;
	int			nDevice;
} ATA_DEVICE_TYPE, far * LPATA_DEVICE_TYPE, near * NPATA_DEVICE_TYPE;

typedef struct                                                	
{
	unsigned char	Data;
	unsigned char	Error_PreComp;
	unsigned char	SctorCnt;
	unsigned char	SectorNum;
	unsigned char	CylinderLSB;
	unsigned char	CylinderMSB;
	unsigned char	Head;
	unsigned char	Command_Status;
} ATA_TASK_FILE, far * LPATA_TASK_FILE, near * NPATA_TASK_FILE;

typedef struct {
        // word 0.
        unsigned short Reserved_0:1;
        unsigned short Retired_0:1;
        unsigned short Response_incomplete:1;
        unsigned short Retired_1:3;
        unsigned short Obsolete_0:1;
        unsigned short RemovableMediaDevice:1;
        unsigned short Retired_2:7;
        unsigned short AtaDevice:1; 
        // word 1
        unsigned short Obselete_1;
        // word 2
        unsigned short SpecificConfiguration;
        // word 3 - 6
        unsigned short ObsoleteOrRetired_0[4];
        // word 7 - 8
        unsigned short ReservedForCompactFlash[2];
        // word 9
        unsigned short Retired_3;
        // word 10 - 19
        char SerialNumber[20];
        // word 20 - 22
        unsigned short ObsoleteOrRetired_1[3];
        // word 23 - 26
        char FirmwareRevision[8];
        // word 27 - 46
        char ModelNumber[40];
        // word 47
        unsigned char Reserved_1; // always 80
        unsigned char MaxTransferSectorsPerInt;
        // word 48
        unsigned short Reserved_2;
        // word 49 
        unsigned char Retired_4;
        unsigned char DmaSupport:1;
        unsigned char LbaSupport:1;
        unsigned char IordyDisable:1;
        unsigned char IordySupport:1;
		unsigned char ReservedForIdentifyPacket_0:1;
		unsigned char StandbyTimerSupport:1;
		unsigned char ReservedForIdentifyPacket_1:2;
        // word 50
        unsigned short StandbyTimerMinimum:1;
        unsigned short ReservedOrObsolete_0:15;
        // word 51 - 52
        unsigned short Obsolete_2[2];
        // word 53
        unsigned short Obsolete_3:1;
        unsigned short Word70Valid:1;
        unsigned short Word88Valid:1;
        unsigned short Reserved_3:13;
        // word 54 - 58
        unsigned short Obsolete_4[5];
        // word 59
        unsigned char MaxTransferSecotorsMultiPerInt;
        unsigned char MultiSectorSettingValid:1;
        unsigned char Reserved_4:7;
        // word 60 - 61
        unsigned long TotalNumberUserAddressableSectors;
        // word 62
        unsigned short Obsolete_5;
        // word 63
        unsigned short MultiWordDmaMode0Support:1;
        unsigned short MultiWordDmaMode1Support:1;
        unsigned short MultiWordDmaMode2Support:1;
		unsigned short Reserved_4b:5;
        unsigned short MultiWordDmaMode0Selected:1;
        unsigned short MultiWordDmaMode1Selected:1;
        unsigned short MultiWordDmaMode2Selected:1;
        unsigned short Reserved_5:5;
        // word 64
        unsigned char PioModeSupport;
        unsigned char Reserved_6;
        // word 65
        unsigned short MinMwDmaTransferCycle; // in ns
        // word 66
        unsigned short RecommendedMwDmaTransferCycle; // in ns
        // word 67
        unsigned short MinPioTransferCycleWithoutFlowCtrl;
        // word 68
        unsigned short MinPioTransferCycleWithIordy;
        // word 69 - 70
        unsigned short Reserved_7[2];
        // word 71 - 74
        unsigned short ReservedForIdentifyPacketDevice_2[4];
        // word 75
        unsigned short MaxQueueDepthMinusOne : 5;
        unsigned short Reserved_8:11;
        // word 76 - 79
        unsigned short Reserved_9[4];
        // word 80
        unsigned short Reserved_10:3;
        unsigned short SupportAta3:1;
        unsigned short SupportAta4:1;
        unsigned short SupportAta5:1;
        unsigned short SupportAta6:1;
        unsigned short SupportAta7:1;
        unsigned short SupportAta8:1;
        unsigned short SupportAta9:1;
        unsigned short SupportAta10:1;
        unsigned short SupportAta11:1;
        unsigned short SupportAta12:1;
        unsigned short SupportAta13:1;
        unsigned short SupportAta14:1;
        unsigned short Reserved_11:1;
        // word 81
        unsigned short MinorVersion;
        // word 82
        unsigned short SmartSupport:1;
        unsigned short SecuritySupport:1;
        unsigned short RemovableMediaSupport:1;
        unsigned short PowerManagementSupport:1;
        unsigned short PacketCommandSupport:1;
        unsigned short WriteCacheSupport:1;
        unsigned short LookaheadSupport:1;
        unsigned short ReleaseInterruptSupport:1;
        unsigned short ServiceInterruptSupport:1;
        unsigned short DeviceResetCommandSupport:1;
        unsigned short HpaSupport:1;
        unsigned short Obsolete_6:1;
        unsigned short WriteBufferSupport:1;
        unsigned short ReadBufferSupport:1;
        unsigned short NopSupport:1;
        unsigned short Obsolete_7:1;
        // word 83
        unsigned short DownloadMicrocodeSupport:1;
        unsigned short ReadWriteDmaQueueSupport:1;
        unsigned short CfaSupport:1;
        unsigned short AdvancePowerManagementSupport:1;
        unsigned short RemovableMediaStatusNotifySupport:1;
        unsigned short PowerUpInStandbySupport:1;
        unsigned short SetFeatureRequiredSpinUpAfterPowerUp:1;
        unsigned short AddressOffsetReservedAreaBoot:1;
        unsigned short SetmaxSecurityExtensionSupport:1;
        unsigned short AutomaticAcousticManagementSupport:1;
        unsigned short Address48BitsSupport:1;
        unsigned short DeviceConfigOverlaySupport:1;
        unsigned short FlushCacheSupport:1;
        unsigned short FlushCacheExtSupport:1;
        unsigned short Reserved_12:2;
        // word 84
        unsigned short SmartErrorLogSupport:1;
        unsigned short SmartSelftestSupport:1;
        unsigned short MediaSerialNumberSupport:1;
        unsigned short MediaCardPassThroughSupport:1;
        unsigned short Reserved_13:1;
        unsigned short GeneralPurposeLogSupport:1;
        unsigned short Reserved_14:10;
        // word 85
        unsigned short SmartEnabled:1;
        unsigned short SecurityEnabled:1;
        unsigned short RemovableMediaEnabled:1;
        unsigned short PowerManagementEnabled:1;
        unsigned short PacketCommandEnabled:1;
        unsigned short WriteCacheEnabled:1;
        unsigned short LookaheadEnabled:1;
        unsigned short ReleaseIntEnabled:1;
        unsigned short ServiceIntEnabled:1;
        unsigned short DeviceResetEnabled:1;
        unsigned short HpaEnabled:1;
        unsigned short Obsolete_8:1;
        unsigned short WriteBufferCommandEnabled:1;
        unsigned short ReadBufferCommandEnabled:1;
        unsigned short NopCommandEnabled:1;
        unsigned short Obsolete_9:1;
        // word 86
        unsigned short DownloadMicrocodeEnabled:1;
        unsigned short ReadWriteDmaQueueEnabled:1;
        unsigned short CfaEnabled:1;
        unsigned short AdvancePowerManagementEnabled:1;
        unsigned short RemovableMediaStatusNotifyEnabled:1;
        unsigned short PowerUpInStandbyEnabled:1;
		unsigned short SetFeatureSpinUpAfterPowerUpEnabled:1;
        unsigned short AddressOffsetReservedAreaBootEnabled:1;
        unsigned short SetmaxSecurityExtensionEnabled:1;
        unsigned short AutomaticAcousticManagementEnabled:1;
        unsigned short Address48BitsEnabled:1;
        unsigned short DeviceConfigOverlayEnabled:1;
        unsigned short FlushCacheEnabled:1;
        unsigned short FlushCacheExtEnabled:1;
        unsigned short Reserved_15:2;
        // word 87
        unsigned short SmartErrorLogDefault:1;
        unsigned short SmartSelftestDefault:1;
        unsigned short MediaSerialNumberDefault:1;
        unsigned short MediaCardPassThroughDefault:1;
        unsigned short Reserved_16:1;
        unsigned short GeneralPurposeLogDefault:1;
        unsigned short Reserved_17:10;
        // word 88
        unsigned short UltraDmaMode0Support:1;
        unsigned short UltraDmaMode1Support:1;
        unsigned short UltraDmaMode2Support:1;
        unsigned short UltraDmaMode3Support:1;
        unsigned short UltraDmaMode4Support:1;
        unsigned short UltraDmaMode5Support:1;
        unsigned short UltraDmaMode6Support:1;
        unsigned short UltraDmaMode7Support:1;
        unsigned short UltraDmaMode0Selected:1;
        unsigned short UltraDmaMode1Selected:1;
        unsigned short UltraDmaMode2Selected:1;
        unsigned short UltraDmaMode3Selected:1;
        unsigned short UltraDmaMode4Selected:1;
        unsigned short UltraDmaMode5Selected:1;
        unsigned short UltraDmaMode6Selected:1;
        unsigned short UltraDmaMode7Selected:1;
        // word 89
        unsigned short TimeForSecurityEraseCompletion;
        // word 90
        unsigned short TimeForEnhanceSecurityEraseCompletion;
        // word 91
        unsigned short CurrentAdvancePowerManagementValue;
        // word 92
        unsigned short MasterPasswordRevisionCode;
        // word 93
        unsigned short HardwareResetResult;
        // word 94
        unsigned char CurrentAcousticManagementValue;
        unsigned char RecommendedAcousticManagementValue;
        // word 95 - 99
        unsigned short Reserved_18[5];
        // word 100 - 103
        unsigned long MaxUserLbaLsbFor48Bits;
        unsigned long MaxUserLbaMsbFor48Bits;
        // word 104 - 126
        unsigned short Reserved_19[23];
        // word 127
        unsigned short RemovableMediaNotificationSupportLevel:2;
        unsigned short Reserved_20:14;
        // word 128
		unsigned short StatusSecuritySupport:1;
		unsigned short StatusSecurityEnabled:1;
        unsigned short SecurityLocked:1;
        unsigned short SecurityFrozen:1;
        unsigned short SecurityCountExpired:1;
        unsigned short EnhanceSecurityEraseSupport:1;
        unsigned short Reserved_21:2;
        unsigned short SecurityLevel:1;
        unsigned short Reserved_22:7;
        // word 129 - 159
        unsigned short VendorSpecific[31];
        // word 160
        unsigned short CfaPowerMode;
        // word 161 - 175
        unsigned short Reserved_23[15];
        // word 176 - 205
        unsigned short CurrentMediaSerial[30];
        // word 206 - 254
        unsigned short Reserved_24[49];
        // word 255
        unsigned char Signature;
        unsigned char Checksum;
}
ATA_IDENTIFY_INFO, far * LPATA_IDENTIFY_INFO, near * NPATA_IDENTIFY_INFO;

void _interrupt TimerHandler(void);
void _interrupt HarddiskIntHandlerPri(void);
void _interrupt HarddiskIntHandlerSec(void);

void HardwareWait(unsigned long Ticks);
unsigned long GetTimerTick();

unsigned long GetVect(int nInt);
void SetVect(int nInt, unsigned long p);


void identify(WORD wIoBase,int nDevice);
int  IsThereAnError(WORD wIoBase);
int  IsDataReady(WORD wIoBase);
int  IsDriveReady(WORD wIoBase);
int  WaitUntilNotBusyOrTimeout(WORD wIoBase,int nTicks);
int  ATARead(WORD wIoBase, int nDevice, DWORD dwAddr, DWORD dwSectors, LPBYTE lpBuf);

#endif
