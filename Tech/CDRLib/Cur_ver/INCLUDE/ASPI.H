#if !defined(CDRLIB_ASPI_H)
#define CDRLIB_ASPI_H

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

// ASPI device types

#define ASPI_DEVTYPE_DISK       0x00
#define ASPI_DEVTYPE_WORM       0x04
#define ASPI_DEVTYPE_CDROM      0x05
#define ASPI_DEVTYPE_CHANGER    0x08
#define ASPI_DEVTYPE_UNDEFINED  0x1F

#define ASPI_M_DEVTYPE_DISK     (1 << ASPI_DEVTYPE_DISK)
#define ASPI_M_DEVTYPE_WORM     (1 << ASPI_DEVTYPE_WORM)
#define ASPI_M_DEVTYPE_CDROM    (1 << ASPI_DEVTYPE_CDROM)
#define ASPI_M_DEVTYPE_CHANGER  (1 << ASPI_DEVTYPE_CHANGER)
#define ASPI_M_DEVTYPE_ANY      0xFFFF

//
// ASPI Command Descriptor Block operation codes
//

#define ASPI_TEST_UNIT_READY        0x00
#define ASPI_REZERO_UNIT            0x01
#define ASPI_REQUEST_SENSE          0x03
#define ASPI_FORMAT_UNIT            0x04
#define ASPI_READ6                  0x08
#define ASPI_WRITE6                 0x0A
#define ASPI_SEEK6                  0x0B
#define ASPI_NO_OPERATION           0x0D

#define ASPI_INQUIRY                0x12
#define ASPI_MODE_SELECT6           0x15
#define ASPI_RESERVE                0x16
#define ASPI_RELEASE                0x17
#define ASPI_COPY                   0x18
#define ASPI_MODE_SENSE6            0x1A
#define ASPI_START_STOP_UNIT        0x1B
#define ASPI_RECEIVE_DIAGNOSTICS    0x1C
#define ASPI_SEND_DIAGNOSTICS       0x1D
#define ASPI_PREVENT_MEDIUM_REMOVAL 0x1E

#define ASPI_READ_FORMAT_CAPACITIES 0x23
#define ASPI_READ_CAPACITY          0x25
#define ASPI_READ10                 0x28
#define ASPI_WRITE10                0x2A
#define ASPI_SEEK10                 0x2B
#define ASPI_WRITE_AND_VERIFY       0x2E
#define ASPI_VERIFY10               0x2F

#define ASPI_SET_LIMITS10           0x33
#define ASPI_PREFETCH               0x34
#define ASPI_SYNCHRONIZE_CACHE      0x35
#define ASPI_WRITE_BUFFER           0x3B
#define ASPI_READ_BUFFER            0x3C

#define ASPI_READ_SUBCHANNEL        0x42
#define ASPI_READ_TOC               0x43
#define ASPI_READ_HEADER            0x44
#define ASPI_PLAY_AUDIO10           0x45
#define ASPI_GET_CONFIGURATION      0x46
#define ASPI_PLAY_AUDIO_MSF         0x47
#define ASPI_PLAY_TRACK_INDEX       0x48
#define ASPI_PLAY_TRACK_REL10       0x49
#define ASPI_PAUSE_RESUME           0x4B
#define ASPI_STOP_PLAY_SCAN         0x4E

#define ASPI_MODE_SELECT10          0x55
#define ASPI_RESERVE10              0x56
#define ASPI_RELEASE10              0x57
#define ASPI_MODE_SENSE10           0x5A

#define ASPI_READ_DISC_INFORMATION  0x51
#define ASPI_READ_TRACK_INFORMATION 0x52
#define ASPI_RESERVE_TRACK          0x53
#define ASPI_SEND_OPC_INFORMATION   0x54
#define ASPI_REPAIR_TRACK           0x58
#define ASPI_READ_MASTER_CUE        0x59
#define ASPI_CLOSE_TRACK_SESSION    0x5B
#define ASPI_READ_BUFFER_CAPACITY   0x5C
#define ASPI_SEND_CUE_SHEET         0x5D

#define ASPI_REPORT_LUNS            0xA0
#define ASPI_BLANK                  0xA1
#define ASPI_SEND_EVENT             0xA2
#define ASPI_SEND_KEY               0xA3
#define ASPI_REPORT_KEY             0xA4
#define ASPI_PLAY_AUDIO12           0xA5
#define ASPI_EXCHANGE_MEDIUM        0xA6
#define ASPI_SET_READ_AHEAD         0xA7
#define ASPI_READ12                 0xA8
#define ASPI_PLAY_TRACK_REL12       0xA9
#define ASPI_GET_PERFORMANCE        0xAC
#define ASPI_READ_DVD_STRUCTURE     0xAD

#define ASPI_SET_LIMITS12           0xB3
#define ASPI_SET_STREAMING          0xB6
#define ASPI_READ_CD_MSF            0xB9
#define ASPI_SCAN_CD                0xBA
#define ASPI_SET_CD_SPEED           0xBB
#define ASPI_PLAY_CD                0xBC
#define ASPI_MECHANISM_STATUS       0xBD
#define ASPI_READ_CD                0xBE
#define ASPI_SEND_DVD_STRUCTURE     0xBF

// These opcodes are vendor unique, but they are used by many CDROM drives.

#define ASPI_READ_CDDA              0xD8
#define ASPI_READ_CDDA_MSF          0xD9

// Mode select pages.

#define ASPI_ERRORRECOVERY_PAGECODE   0x01
#define ASPI_ERRORRECOVERY_PAGELEN    6

#define ASPI_POWERCONDITION_PAGECODE  0x1A
#define ASPI_POWERCONDITION_PAGELEN   10

#define ASPI_CDROMPARAMS_PAGECODE     0x0D
#define ASPI_CDROMPARAMS_PAGELEN      6

// ASPI Command Codes
//
//  00h           Host Adapter Inquiry
//  01h           Get Device Type
//  02h           Execute I/O Command
//  03h           Abort I/O Command
//  04h           Reset Device
//  05h           Set Host Adapter Parameters
//  06h           Get Disk Drive Information
//  07h-7Fh       Reserved for Future Expansion
//  80h-FFh       Reserved for Vendor Unique

#define ASPI_HOST_ADAPTER_INQUIRY   0x00
#define ASPI_GET_DEVICE_TYPE        0x01
#define ASPI_EXECUTE_IO             0x02
#define ASPI_ABORT_REQUEST          0x03
#define ASPI_RESET_DEVICE           0x04
#define ASPI_SET_ADAPTER_PARAMS     0x05
#define ASPI_GET_DISK_DRIVE_INFO    0x06

// ASPI request status codes

#define ASPI_PENDING                0x00    // SRB being processed
#define ASPI_SUCCESS                0x01    // SRB completed without error
#define ASPI_ABORT                  0x02    // SRB aborted
#define ASPI_ABORT_FAIL             0x03    // Unable to abort SRB
#define ASPI_ERROR                  0x04    // SRB completed with error
#define ASPI_INVALID_REQUEST        0x80    // Invalid ASPI command
#define ASPI_INVALID_ADAPTER        0x81    // Invalid host adapter number
#define ASPI_NO_DEVICE              0x82    // ASPI device not installed
#define ASPI_INVALID_SRB            0xE0    // Invalid request block.
#define ASPI_BUFFER_ALIGN           0xE1    // Buffer alignment error
#define ASPI_ILLEGAL_MODE           0xE2    // Illegal mode (Win32S)
#define ASPI_NO_HELPER_DRIVER       0xE3    // No ASPI helper driver
#define ASPI_FAILED_INIT            0xE4    // General initialization failure
#define ASPI_BUSY                   0xE5    // ASPI manager is busy
#define ASPI_BUFFER_TOO_BIG         0xE6    // Buffer size is too big
#define ASPI_MISMATCHED_COMPONENTS  0xE7    // Mismatched components
#define ASPI_NO_ADAPTERS            0xE8    // No host adapters on system
#define ASPI_INSUFFICIENT_RESOURCES 0xE9    // Insufficient resources

#define ADAPTER_SUCCESS             0x00
#define ADAPTER_TIMEOUT             0x09
#define ADAPTER_SRB_EXPIRED         0x0B
#define ADAPTER_MESSAGE_REJECT      0x0D
#define ADAPTER_BUS_RESET           0x0E
#define ADAPTER_PARITY_ERROR        0x0F
#define ADAPTER_SENSE_FAILED        0x10
#define ADAPTER_SELECTION_TIMEOUT   0x11
#define ADAPTER_DATA_OVERRUN        0x12
#define ADAPTER_BUS_FREE            0x13
#define ADAPTER_BUS_FAILURE         0x14

#define TARGET_NOSTATUS             0x00
#define TARGET_CHECK_STATUS         0x02
#define TARGET_BUSY                 0x08
#define TARGET_CONFLICT             0x18

// SENSE keys.

#define SENSE_NOSENSE               0x00
#define SENSE_RECOVERED_ERROR       0x01
#define SENSE_NOT_READY             0x02
#define SENSE_MEDIUM_ERROR          0x03
#define SENSE_HARDWARE_ERROR        0x04
#define SENSE_ILLEGAL_REQUEST       0x05
#define SENSE_UNIT_ATTENTION        0x06
#define SENSE_BLANK_CHECK           0x08
#define SENSE_DEVICE_BUSY           0x09
#define SENSE_TARGET_ABORT          0x0B
#define SENSE_VOLUME_OVERFLOW       0x0D

// ASPI request flags

#define ASPI_M_POST                 0x0001
#define ASPI_M_LINK                 0x0002
#define ASPI_M_RESIDUAL_COUNT       0x0004
#define ASPI_M_TARGET_TO_HOST       0x0008
#define ASPI_M_HOST_TO_TARGET       0x0010
#define ASPI_M_NO_TRANSFER          0x0018
#define ASPI_M_RESERVED1            0x0020
#define ASPI_M_EVENT_NOTIFY         0x0040
#define ASPI_M_RESERVED2            0x0080

#define ASPI_M_LONG_OPERATION       0x0100
#define ASPI_M_BUFFER_IS_ALIGNED    0x0200
#define ASPI_M_ALL_DEVICES          0x0400
#define ASPI_M_THIS_DEVICE_ONLY     0x0800
#define ASPI_M_FAILED_DEVICES_ONLY  0x1000
#define ASPI_M_MARK_FAILED_DEVICES  0x2000
#define ASPI_M_RESERVED3            0x4000
#define ASPI_M_RESERVED4            0x8000

// Error recovery flags.

#define ASPI_M_DCR    (1 << 0)
#define ASPI_M_DTE    (1 << 1)
#define ASPI_M_PER    (1 << 2)
#define ASPI_M_EEC    (1 << 3)
#define ASPI_M_RC     (1 << 4)
#define ASPI_M_TB     (1 << 5)

// BLANK command types.

#define BLANK_DISC_FULL                   0x00
#define BLANK_DISC_QUICK                  0x01
#define BLANK_TRACK                       0x02
#define BLANK_UNRESERVE_TRACK             0x03
#define BLANK_TRACK_TAIL                  0x04
#define BLANK_UNCLOSE_LAST_SESSION        0x05
#define BLANK_SESSION                     0x06
#define BLANK_RESERVED                    0x07

// Profiles for "GET CONFIGURATION" command

#define PROFILE_RESERVED                  0x00
#define PROFILE_DISK_NON_REMOVABLE        0x01
#define PROFILE_DISK_REMOVABLE            0x02
#define PROFILE_MO_ERASABLE               0x03
#define PROFILE_MO_WRITE_ONCE             0x04
#define PROFILE_MO_ADVANCE_STORAGE        0x05
#define PROFILE_CD_READONLY               0x08  // CD-ROM
#define PROFILE_CD_WRITABLE               0x09  // CD-R
#define PROFILE_CD_REWRITABLE             0x0A  // CD-RW
#define PROFILE_DVD_READONLY              0x10  // DVD-ROM
#define PROFILE_DVD_WRITABLE              0x11  // DVD-R
#define PROFILE_DVD_RAM                   0x12  // DVD-RAM
#define PROFILE_DVD_REWRITABLE_RESTRICTED 0x13  // DVD-RW
#define PROFILE_DVD_REWRITABLE_SEQUENTIAL 0x14  // DVD-RW
#define PROFILE_DVD_PLUS_REWRITABLE       0x1A  // DVD+RW
#define PROFILE_DDCD_READONLY             0x20  // DDCD-ROM
#define PROFILE_DDCD_WRITABLE             0x21  // DDCD-R
#define PROFILE_DDCD_REWRITABLE           0x22  // DDCD-RW

// List of known ASPI devices

#define GENERIC_DISK_SCSI       0x0000
#define GENERIC_DISK_ATAPI      0x0001
#define GENERIC_CDROM_SCSI      0x0002
#define GENERIC_CDROM_ATAPI     0x0003

#define CDRW_8X8X24             0x0100
#define CDRW_12X10X24           0x0101
#define CDRW_16X10X24           0x0102
#define CDRW_20X10X24           0x0103
#define CDRW_20X10X40           0x0104
#define CDRW_24X10X24           0x0105
#define CDRW_24X10X40           0x0106
#define CDRW_24X12X40           0x0107
#define CDRW_32X10X40           0x0108
#define CDRW_32X12X40           0x0109
#define CDRW_40X12X48           0x010A
#define CDRW_40X16X48           0x010B
#define CDRW_48X16X48           0x010C
#define CDRW_48X24X48           0x010D
#define CDRW_52X24X52           0x010E
#define CDRW_52X32X52           0x010F

#define ASUS_CRW1610            0x0200
#define ASUS_CRW2010            0x0201
#define ASUS_CRW2410            0x0202
#define ASUS_CRW3212            0x0203
#define ASUS_CRW4012            0x0204
#define ASUS_CRW4816            0x0205
#define ASUS_CRW5224            0x0206

#define HP_6020                 0x0300

#define JVC_XRW2001             0x0400
#define JVC_XRW2010             0x0401
#define JVC_XRW2020             0x0402
#define JVC_XRW2040             0x0403
#define JVC_XRW2080             0x0404
#define JVC_XRW4080             0x0405

#define KODAK_PCD600            0x0500
#define KODAK_TRANSPORTER       0x0501

#define LGE_4X2X24              0x0600
#define LGE_4X4X24              0x0601
#define LGE_4X4X32              0x0602
#define LGE_8X4X32              0x0603
#define LGE_12X8X32             0x0604
#define LGE_16X10X40            0x0605
#define LGE_24X10X40            0x0606
#define LGE_32X10X40            0x0607
#define LGE_40X12X40            0x0608
#define LGE_48X16X48            0x0609
#define LGE_48X24X48            0x060A
#define LGE_52X24X52            0x060B
#define LGE_GMA4020             0x061B
#define LGE_GSA4040             0x062B
#define LGE_GSA4081             0x063B
#define LGE_GSA4082             0x064B
#define LGE_GSA4120             0x065B
#define LGE_GSA4160             0x066B

#define MATSHITA_CW7501         0x0700
#define MATSHITA_CW7502         0x0701
#define MATSHITA_CW7503         0x0702
#define MATSHITA_CW7582         0x0703
#define MATSHITA_CW7585         0x0704
#define MATSHITA_CW8572         0x0705

#define MITSUMI_CR4801TE        0x0800
#define MITSUMI_CR4802TE        0x0801
#define MITSUMI_CR4804TE        0x0802
#define MITSUMI_CR4805TE        0x0803
#define MITSUMI_CR4808TE        0x0804
#define MITSUMI_CR4809TE        0x0805
#define MITSUMI_CR48XATE        0x0806
#define MITSUMI_CR48XCTE        0x0807
#define MITSUMI_CR48XFTE        0x0808

#define NEC_ND1000              0x0900
#define NEC_ND1100              0x0901
#define NEC_ND1300              0x0902
#define NEC_ND2100              0x0903
#define NEC_ND2500              0x0904
#define NEC_ND3500              0x0905
#define NEC_ND3520              0x0906
#define NEC_ND3530              0x0907
#define NEC_ND4550              0x0908

#define PHILIPS_CDD521          0x0A00
#define PHILIPS_CDD522          0x0A01
#define PHILIPS_CDD2000         0x0A02
#define PHILIPS_CDD2600         0x0A03
#define PHILIPS_CDD3600         0x0A04
#define PHILIPS_CDD3800         0x0A05
#define PHILIPS_CDD4200         0x0A06
#define PHILIPS_CDD4400         0x0A07
#define PHILIPS_CDD4600         0x0A08
#define PHILIPS_CDD4800         0x0A09
#define PHILIPS_CDRW1004        0x0A0A
#define PHILIPS_CDRW1208        0x0A0B
#define PHILIPS_CDRW1610        0x0A0C
#define PHILIPS_CDRW2010        0x0A0D
#define PHILIPS_CDRW2410        0x0A0E
#define PHILIPS_CDRW3210        0x0A0F
#define PHILIPS_CDRW4012        0x0A10
#define PHILIPS_CDRW4824        0x0A11
#define PHILIPS_CDRW5224        0x0A12

#define PIONEER_DWS114X         0x0B00
#define PIONEER_DVR103          0x0B01
#define PIONEER_DVR104          0x0B02
#define PIONEER_DVR105          0x0B03
#define PIONEER_DVR106          0x0B04
#define PIONEER_DVR107          0x0B05
#define PIONEER_DVR108          0x0B06
#define PIONEER_DVR109          0x0B07
#define PIONEER_DVR110          0x0B08
#define PIONEER_DVR111          0x0B09

#define PLEXTOR_PXR412          0x0C00
#define PLEXTOR_PXR820          0x0C01
#define PLEXTOR_PXW4220         0x0C02
#define PLEXTOR_PXW8220         0x0C03
#define PLEXTOR_PXW8432         0x0C04
#define PLEXTOR_PXW124          0x0C05
#define PLEXTOR_PXW1210         0x0C06
#define PLEXTOR_PXW1610         0x0C07
#define PLEXTOR_PXW2410         0x0C08
#define PLEXTOR_PXW4012         0x0C09
#define PLEXTOR_PXW4824         0x0C0A
#define PLEXTOR_PXW5224         0x0C0B
#define PLEXTOR_PXW5232         0x0C0C
#define PLEXTOR_PX320           0x0C0D
#define PLEXTOR_PXS88           0x0C0E
#define PLEXTOR_PX708           0x0C0F
#define PLEXTOR_PX712           0x0C10
#define PLEXTOR_PX716           0x0C11
#define PLEXTOR_PX750           0x0C12
#define PLEXTOR_PX755           0x0C13
#define PLEXTOR_PX760           0x0C14
#define PLEXTOR_PXB900          0x0C15

#define QSI_SBW241              0x0D00
#define QSI_DBW481              0x0D01
#define QSI_DBW521              0x0D02
#define QSI_SDW041              0x0D03
#define QSI_DDW041              0x0D04
#define QSI_DDW061              0x0D05
#define QSI_DDW082              0x0D06

#define RICOH_RS1420C           0x0E00
#define RICOH_MP6200S           0x0E01
#define RICOH_MP6200A           0x0E02
#define RICOH_MP7040            0x0E03
#define RICOH_MP7060            0x0E04
#define RICOH_MP7080            0x0E05
#define RICOH_MP7120            0x0E06
#define RICOH_MP7125            0x0E07
#define RICOH_MP7163            0x0E08
#define RICOH_MP7200            0x0E09
#define RICOH_MP7240            0x0E0A
#define RICOH_MP7320            0x0E0B
#define RICOH_MP7400            0x0E0C
#define RICOH_MP7480            0x0E0D
#define RICOH_MP7520            0x0E0E
#define RICOH_MP8040            0x0E0F
#define RICOH_MP8080            0x0E10
#define RICOH_MP8160            0x0E11
#define RICOH_MP5120            0x0E12
#define RICOH_MP5125            0x0E13
#define RICOH_MP5163            0x0E14
#define RICOH_MP5240            0x0E15

#define SANYO_CDR8020           0x0F00
#define SANYO_CDRW8220          0x0F01
#define SANYO_CDRW8432          0x0F02
#define SANYO_CDRW12432         0x0F03
#define SANYO_CDRW121032        0x0F04
#define SANYO_CDRW161040        0x0F05
#define SANYO_CDRW201040        0x0F06
#define SANYO_CDRW241040        0x0F07
#define SANYO_CDRW321040        0x0F08
#define SANYO_CDRW401248        0x0F09
#define SANYO_CDRW481648        0x0F0A
#define SANYO_CDRW522452        0x0F0B
#define SANYO_CRDBPDV2          0x0F0C
#define SANYO_CRDBPDV3          0x0F0D

#define SONY_CDW900E            0x1000
#define SONY_CDU920S            0x1001
#define SONY_CDU924S            0x1002
#define SONY_CDU926S            0x1003
#define SONY_CDU948S            0x1004
#define SONY_CRX100             0x1005
#define SONY_CRX110             0x1006
#define SONY_CRX120_2X          0x1007
#define SONY_CRX120_4X          0x1008
#define SONY_CRX130             0x1009
#define SONY_CRX140             0x100A
#define SONY_CRX145             0x100B
#define SONY_CRX160             0x100C
#define SONY_CRX168             0x100D
#define SONY_CRX85              0x100E
#define SONY_CRX175             0x100F
#define SONY_CRX185             0x1010
#define SONY_CRX195             0x1011
#define SONY_CRX225             0x1012
#define SONY_CRX230             0x1013
#define SONY_CRX300             0x1014
#define SONY_DRU500             0x1015
#define SONY_DRU510             0x1016
#define SONY_DRU530             0x1017
#define SONY_DRU540             0x1018
#define SONY_DRU710             0x1019
#define SONY_DWU50              0x101A
#define SONY_DWU54              0x101B
#define SONY_DWU55              0x101C
#define SONY_DWD56              0x101D
#define SONY_DRU720             0x101E
#define SONY_DRU800             0x101F

#define TEAC_CDR50S             0x1100
#define TEAC_CDR55S             0x1101
#define TEAC_CDR56S             0x1102
#define TEAC_CDR56S4            0x1103
#define TEAC_CDR58S             0x1104
#define TEAC_CDW54              0x1105
#define TEAC_CDW58              0x1106
#define TEAC_CDW512             0x1107
#define TEAC_CDW516             0x1108
#define TEAC_CDW524             0x1109
#define TEAC_CDW540             0x110A
#define TEAC_CDW548             0x110B
#define TEAC_CDW552             0x110C

#define TOSHIBA_SDR1002         0x1200
#define TOSHIBA_SDR1102         0x1201
#define TOSHIBA_SDR1202         0x1202
#define TOSHIBA_SDR5002         0x1203
#define TOSHIBA_SDR5112         0x1204
#define TOSHIBA_SDR6012         0x1205
#define TOSHIBA_SDR6112         0x1206
#define TOSHIBA_SDR5272         0x1207
#define TOSHIBA_SDR5372         0x1208

#define YAMAHA_CDR100           0x1300
#define YAMAHA_CDR102           0x1301
#define YAMAHA_CDR200           0x1302
#define YAMAHA_CDR400           0x1303
#define YAMAHA_CRW2260          0x1304
#define YAMAHA_CRW2216          0x1305
#define YAMAHA_CRW4260          0x1306
#define YAMAHA_CRW4416          0x1307
#define YAMAHA_CRW6416          0x1308
#define YAMAHA_CRW8424          0x1309
#define YAMAHA_CRW8824          0x130A
#define YAMAHA_CRW2100          0x130B
#define YAMAHA_CRW2200          0x130C
#define YAMAHA_CRW3200          0x130D
#define YAMAHA_CRWF1            0x130E
#define YAMAHA_CRW70            0x130F

#define BENQ_DW400              0x1400
#define BENQ_DW800              0x1401
#define BENQ_DW822              0x1402
#define BENQ_DW830              0x1403
#define BENQ_DW1600             0x1404
#define BENQ_DW1620             0x1405
#define BENQ_DW1640             0x1406

#define DVDRW_MINUSRW_2X        0x1500
#define DVDRW_PLUSRW_2X         0x1501
#define DVDRW_MINUSRW_4X        0x1502
#define DVDRW_PLUSRW_4X         0x1503
#define DVDRW_MINUSRW_8X        0x1504
#define DVDRW_PLUSRW_8X         0x1505
#define DVDRW_DUAL_4X           0x1506
#define DVDRW_DUAL_8X           0x1507
#define DVDRW_DUAL_12X          0x1508
#define DVDRW_DUAL_16X          0x1509
#define DVDRW_DUAL_DL_16X       0x150A

#define KENWOOD_CDROM_42X       0x2000
#define KENWOOD_CDROM_52X       0x2001
#define KENWOOD_CDROM_72X       0x2002
#define MATSHITA_CDROM_4X       0x2003
#define MATSHITA_CDROM_6X       0x2004
#define MATSHITA_CDROM_8X       0x2005
#define MATSHITA_CDROM_12X      0x2006
#define MATSHITA_CDROM_24X      0x2007
#define NEC_CDROM_3X            0x2008
#define NEC_CDROM_4X            0x2009
#define NEC_CDROM_6X            0x200A
#define NEC_CDROM_8X            0x200B
#define NEC_CDROM_16X           0x200C
#define NEC_CDROM_24X           0x200D
#define NEC_CDROM_32X           0x200E
#define NEC_CDROM_40X           0x200F
#define PIONEER_CDROM_4X        0x2010
#define PIONEER_CDROM_10X       0x2011
#define PIONEER_CDROM_12X       0x2012
#define PIONEER_CDROM_24X       0x2013
#define PIONEER_CDROM_32X       0x2014
#define PIONEER_CDROM_36X       0x2015
#define PIONEER_CDROM_40X       0x2016
#define PIONEER_DVD_32X         0x2017
#define PLEXTOR_CDROM_4X        0x2018
#define PLEXTOR_CDROM_6X        0x2019
#define PLEXTOR_CDROM_8X        0x201A
#define PLEXTOR_CDROM_12X       0x201B
#define PLEXTOR_CDROM_20X       0x201C
#define PLEXTOR_CDROM_32X       0x201D
#define PLEXTOR_CDROM_40X       0x201E
#define SONY_CDROM_2X           0x201F
#define SONY_CDROM_4X           0x2020
#define SONY_CDROM_12X          0x2021
#define SONY_CDROM_24X          0x2022
#define TEAC_CDROM_6X           0x2023
#define TEAC_CDROM_16X          0x2024
#define TEAC_CDROM_32X          0x2025
#define TOSHIBA_CDROM_2X        0x2026
#define TOSHIBA_CDROM_4X        0x2027
#define TOSHIBA_CDROM_6X        0x2028
#define TOSHIBA_CDROM_12X       0x2029
#define TOSHIBA_CDROM_14X       0x202A
#define TOSHIBA_CDROM_24X       0x202B
#define TOSHIBA_CDROM_32X       0x202C
#define TOSHIBA_CDROM_40X       0x202D
#define TOSHIBA_DVD_32X         0x202E

// Mode sense page parameters.

#define ALL_PAGES         0x3F

#define CURRENT_PARAMS    0x00
#define CHANGEABLE_PARAMS 0x01
#define DEFAULT_PARAMS    0x02
#define SAVED_PARAMS      0x03

// Device table entry.

struct DEVICEID {
  char id[18];
  UWORD model;
  };

// ASPI Sense Data

struct SENSE {
  BFLAG response_code : 7;
  BFLAG valid : 1;
  UBYTE segment_number;
  BFLAG key : 4;              // primary sense code
  BFLAG : 1;
  BFLAG ili_flag : 1;
  BFLAG eom_flag : 1;
  BFLAG fm_flag : 1;
  UBYTE info[4];              // device or command information
  UBYTE add_sense_len;        // additional sense data len
  UBYTE command_info[4];      // command specific information
  UBYTE asc;                  // additional sense code
  UBYTE ascq;                 // additional sense code qualifier
  UBYTE fruc;                 // field replacable unit code
  BFLAG : 7;
  BFLAG sksv : 1;
  UBYTE progess[2];           // progress information
  UBYTE reserved[14];
  };

// ASPI Request Block header.

struct SRBHEADER {
  UBYTE cmd;
  UBYTE volatile status;
  UBYTE adapter;
  UBYTE flags;
  ULONG reserved;
  };

//
// 32-bit ASPI Request Block definitions
//

#if defined(WIN32)
struct HAUNIQUEPARAMS {
  UWORD buffer_alignment_mask;
  BFLAG : 1;
  BFLAG residual_count_flag : 1;
  BFLAG : 6;
  UBYTE maximum_target_count;
  ULONG maximum_transfer_length;
  UBYTE reserved[8];
  };

struct SRBHAINQUIRY {
  SRBHEADER header;
  UBYTE adapter_count;
  UBYTE adapter_id;
  char manager_name[16];
  char adapter_name[16];
  HAUNIQUEPARAMS parameters;
  UWORD reserved;
  };

struct SRBDEVTYPE {
  SRBHEADER header;
  UBYTE target_id;
  UBYTE lun;
  UBYTE device_type;
  UBYTE device_letter;    // device letter (Nexitech driver only)
  };

struct SRBRESET {
  SRBHEADER header;
  UBYTE target_id;
  UBYTE lun;
  UBYTE reserved1[12];
  UBYTE adapter_status;
  UBYTE target_status;
  void *postproc;
  void *reserved2;
  UBYTE reserved3[32];
  };

struct SRBABORT {
  SRBHEADER header;
  void *srbP;
  };

struct SRBCOMMAND {
  SRBHEADER header;
  UBYTE target_id;
  UBYTE lun;
  UWORD reserved1;
  ULONG buflen;
  UBYTE *buf;
  UBYTE sense_len;
  UBYTE cdb_len;
  UBYTE adapter_status;
  UBYTE target_status;
  void *postproc;
  void *reserved2;
  UBYTE reserved3[16];
  UBYTE cdb[16];
  SENSE sense;
  };

#else

//
// 16-bit ASPI Request Block definitions
//

struct SRBHAINQUIRY {
  SRBHEADER header;
  UBYTE adapter_count;
  UBYTE adapter_id;
  UBYTE manager_name[16];
  UBYTE adapter_name[16];
  UBYTE parameters[16];
  UWORD reserved;
  };

struct SRBDEVTYPE {
  SRBHEADER header;
  UBYTE target_id;
  UBYTE lun;
  UBYTE device_type;
  };

struct SRBRESET {
  SRBHEADER header;
  UBYTE target_id;
  UBYTE lun;
  UBYTE reserved[14];
  UBYTE adapter_status;
  UBYTE target_status;
  unsigned short postproc_offset;
  unsigned short postproc_segment;
  UBYTE workspace[2];
  };

struct SRBABORT {
  SRBHEADER header;
  unsigned short srb_offset;
  unsigned short srb_segment;
  };

struct SRBCOMMAND {
  SRBHEADER header;
  UBYTE target_id;
  UBYTE lun;
  ULONG buflen;
  UBYTE sense_len;
  unsigned short buf_offset;
  unsigned short buf_segment;
  unsigned short srblink_offset;
  unsigned short srblink_segment;
  UBYTE cdb_len;
  UBYTE adapter_status;
  UBYTE target_status;
  unsigned short postproc_offset;
  unsigned short postproc_segment;
  UBYTE workspace[34];
  UBYTE cdb_sense_area[64];
  };
#endif

// Data returned from an ASPI "Host Adapter Inquiry" command

struct HAINQUIRYDATA {
  char manager_name[17];
  char adapter_name[17];
  UBYTE adapter_count;
  UBYTE adapter_number;
  UBYTE adapter_id;
  UBYTE maximum_target_count;
  UWORD buffer_alignment_mask;
  ULONG maximum_transfer_length;
  BOOL residual_count_flag;
  };

// Data returned from an ASPI "Inquiry" command.

struct INQUIRYDATA {
  BFLAG devtype : 5;
  BFLAG : 3;
  BFLAG devqual : 7;
  BFLAG rmb : 1;
  BFLAG ansi_ver : 3;
  BFLAG ecma_ver : 3;
  BFLAG iso_ver : 2;
  BFLAG response_fmt : 4;
  BFLAG : 3;
  BFLAG anec : 1;
  BFLAG additional_len : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 1;
  BFLAG cmd_que : 1;
  BFLAG cache : 1;
  BFLAG linked : 1;
  BFLAG sync : 1;
  BFLAG bus16 : 1;
  BFLAG bus32 : 1;
  BFLAG reladr : 1;
  char vendor_id[8];
  char product_id[16];
  char firmware_level[4];
  };

// Capacity information

struct CAPACITY {
  UBYTE lba[4];
  UBYTE blklen[4];
  };

// Mode select header

struct MSHEAD6 {
  UBYTE data_len;
  UBYTE medium_type;
  UBYTE application_code;
  UBYTE blkdsc_len;
  };

struct MSHEAD10 {
  UBYTE data_len[2];
  UBYTE medium_type;
  UBYTE application_code;
  UBYTE reserved1;
  UBYTE reserved2;
  UBYTE blkdsc_len[2];
  };

// Mode select block descriptor

struct MSBLKDSC {
  UBYTE density;
  UBYTE blocks[3];
  BFLAG : 8;
  UBYTE blklen[3];
  };

// ASPI Control Field

struct CONTROL {
  BFLAG link : 1;
  BFLAG flag : 1;
  BFLAG reserved : 4;
  BFLAG vendor : 2;
  };

// Generic CDB for 6 byte commands

struct GENCDB6 {
  UBYTE cmd;
  BFLAG flag0 : 1;
  BFLAG flag1 : 1;
  BFLAG flag2 : 1;
  BFLAG flag3 : 1;
  BFLAG flag4 : 1;
  BFLAG lun : 3;
  UBYTE adr[2];
  UBYTE buflen;
  CONTROL control;
  };

// Generic CDB for 10 byte commands

struct GENCDB10 {
  UBYTE cmd;
  BFLAG flag0 : 1;
  BFLAG flag1 : 1;
  BFLAG flag2 : 1;
  BFLAG flag3 : 1;
  BFLAG flag4 : 1;
  BFLAG lun : 3;
  UBYTE adr[4];
  BFLAG : 8;
  UBYTE buflen[2];
  CONTROL control;
  };

// Generic CDB for 12 byte commands

struct GENCDB12 {
  UBYTE cmd;
  BFLAG flag0 : 1;
  BFLAG flag1 : 1;
  BFLAG flag2 : 1;
  BFLAG flag3 : 1;
  BFLAG flag4 : 1;
  BFLAG lun : 3;
  UBYTE adr[4];
  UBYTE buflen[4];
  BFLAG : 8;
  CONTROL control;
  };

// "MODE SELECT" command

struct MODESELECT6 {
  UBYTE cmd;
  BFLAG sp : 1;
  BFLAG : 3;
  BFLAG pf : 1;
  BFLAG lun : 3;
  BFLAG : 8;
  BFLAG : 8;
  UBYTE param_len;
  CONTROL control;
  };

struct MODESELECT10 {
  UBYTE cmd;
  BFLAG sp : 1;
  BFLAG : 3;
  BFLAG pf : 1;
  BFLAG lun : 3;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  UBYTE param_len[2];
  CONTROL control;
  };

// "MODE SENSE" command

struct MODESENSE6 {
  UBYTE cmd;
  BFLAG : 5;
  BFLAG lun : 3;
  BFLAG pagecode : 6;
  BFLAG pcf : 2;
  BFLAG : 8;
  UBYTE param_len;
  CONTROL control;
  };

struct MODESENSE10 {
  UBYTE cmd;
  BFLAG : 5;
  BFLAG lun : 3;
  BFLAG pagecode : 6;
  BFLAG pcf : 2;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  UBYTE param_len[2];
  CONTROL control;
  };

// "READ CAPACITY" command

struct READCAPACITY {
  UBYTE cmd;
  BFLAG reladr : 1;
  BFLAG : 4;
  BFLAG lun : 3;
  UBYTE lba[4];
  BFLAG : 8;
  BFLAG : 8;
  BFLAG pmi : 1;
  BFLAG : 7;
  CONTROL control;
  };

// "START/STOP UNIT" command

struct STARTSTOPUNIT {
  UBYTE cmd;
  BFLAG immed : 1;
  BFLAG : 4;
  BFLAG lun : 3;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG start : 1;
  BFLAG eject : 1;
  BFLAG : 6;
  CONTROL control;
  };

// "PREVENT/ALLOW MEDIUM REMOVAL" command

struct MEDIUMREMOVAL {
  UBYTE cmd;
  BFLAG : 5;
  BFLAG lun : 3;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG prevent : 1;
  BFLAG persistent : 1;
  BFLAG : 6;
  CONTROL control;
  };

// "INQUIRY" command

struct INQUIRY {
  UBYTE cmd;
  BFLAG : 5;
  BFLAG lun : 3;
  BFLAG : 7;
  BFLAG evpd : 1;
  UBYTE vpd;
  UBYTE len;
  CONTROL control;
  };

// "READ" and "WRITE" commands (10 byte)

struct READWRITE {
  UBYTE cmd;
  BFLAG : 5;
  BFLAG lun : 3;
  UBYTE lba[4];
  BFLAG : 8;
  UBYTE blocks[2];
  CONTROL control;
  };

// "SYNCHRONIZE CACHE" command (10 byte)

struct SYNCHCACHE {
  UBYTE cmd;
  BFLAG reladr : 1;
  BFLAG immed : 1;
  BFLAG : 3;
  BFLAG lun : 3;
  UBYTE lba[4];
  BFLAG : 8;
  UBYTE blocks[2];
  CONTROL control;
  };

// "SET LIMITS" command (12 byte)

struct SETLIMITS {
  UBYTE cmd;
  BFLAG : 5;
  BFLAG lun : 3;
  UBYTE lba[4];
  UBYTE blocks[4];
  BFLAG : 8;
  CONTROL control;
  };

// "BLANK" command (A1h)

struct BLANK {
  UBYTE cmd;
  BFLAG type : 3;
  BFLAG : 1;
  BFLAG immed : 1;
  BFLAG : 3;
  UBYTE param[4];
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  CONTROL control;
  };

// "FORMAT UNIT" command (04h)

#define FORMAT_DESC_TYPE_RESERVED     0x00
#define FORMAT_DESC_TYPE_UNFORMATTED  0x01
#define FORMAT_DESC_TYPE_FORMATTED    0x02
#define FORMAT_DESC_TYPE_NOMEDIA      0x03

struct FORMATUNIT {
  UBYTE cmd;
  BFLAG code : 3;
  BFLAG cmplist : 1;
  BFLAG fmtdata : 1;
  BFLAG : 3;
  BFLAG : 8;
  UBYTE interleave_value[2];
  CONTROL control;
  };

struct FORMATLISTHEADER {
  UBYTE reserved;
  BFLAG vs : 1;
  BFLAG immed : 1;
  BFLAG tryout : 1;
  BFLAG ip : 1;
  BFLAG stpf : 1;
  BFLAG dcrt : 1;
  BFLAG dpry : 1;
  BFLAG fov : 1;
  UBYTE format_desc_len[2];
  };

struct FORMATCAPACITYDESC {
  UBYTE blocks[4];
  BFLAG type : 2;           // present in current/maximum capacity descriptor only
  BFLAG format_type : 6;    // present in formattable capacity descriptor only
  union {
    UBYTE block_length[3];  // present in current/maximum capacity descriptor only
    UBYTE param[3];         // present in formattable capacity descriptor only
    };
  };

struct FORMATDESC_CDRW {
  BFLAG : 6;
  BFLAG grow : 1;
  BFLAG session : 1;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  UBYTE blocks[4];
  };

struct FORMATPATTERNDESC {
  BFLAG : 5;
  BFLAG si : 1;
  BFLAG ip_modifier : 2;
  UBYTE pattern_type;
  UBYTE pattern_length[2];
  };

// "READ FORMAT CAPACITIES" command (23h)

struct READFORMATCAPACITIES {
  UBYTE cmd;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  UBYTE param_len[2];
  CONTROL control;
  };

struct CAPACITYLISTHEADER {
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  UBYTE capacity_desc_len;
  };

struct CAPACITYLIST {
  CAPACITYLISTHEADER header;
  FORMATCAPACITYDESC desc[31];
  };

// "GET CONFIGURATION" command (46h)

struct GETCONFIGURATION {
  UBYTE cmd;
  BFLAG rt : 2;
  BFLAG : 6;
  UBYTE starting_feature[2];
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  UBYTE param_len[2];
  CONTROL control;
  };

// "GET PERFORMANCE" command (ACh)

struct GETPERFORMANCE {
  UBYTE cmd;
  BFLAG datatype : 5;
  BFLAG : 3;
  UBYTE starting_lba[4];
  BFLAG : 8;
  BFLAG : 8;
  UBYTE max_desc_count[2];
  UBYTE desc_type;
  CONTROL control;
  };

struct WRITESPEEDDESC {
  BFLAG mrw :1;
  BFLAG exact :1;
  BFLAG rdd :1;
  BFLAG wrc :2;
  BFLAG : 3;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  UBYTE end_lba[4];
  UBYTE read_speed[4];
  UBYTE write_speed[4];
  };

// "Error Recovery" page (01h)

struct ERRORRECOVERY {
  BFLAG pagecode : 6;
  BFLAG : 2;
  UBYTE pagelen;
  UBYTE mode;
  UBYTE retry_count;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  };

// "Disconnect/Reconnect" page (02h)

struct DISCONNECT {
  BFLAG pagecode : 6;
  BFLAG : 2;
  UBYTE pagelen;
  UBYTE buffer_full_ratio;
  UBYTE buffer_empty_ratio;
  UBYTE reserved[12];
  };

// "CDROM Parameters" page (0Dh)

struct CDROMPARAMS {
  BFLAG pagecode : 6;
  BFLAG : 2;
  UBYTE pagelen;
  BFLAG : 8;
  BFLAG inactivity_multiplier : 4;
  BFLAG : 4;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  };

// "Power Condition" page (1Ah)

struct POWERCONDITION {
  BFLAG pagecode : 6;
  BFLAG : 1;
  BFLAG ps : 1;
  UBYTE pagelen;
  BFLAG : 8;
  BFLAG standby : 1;
  BFLAG idle : 1;
  BFLAG : 6;
  UBYTE idle_timeout[4];    // 100 milliseconds increments
  UBYTE standby_timeout[4]; // 100 milliseconds increments
  };

// ASPI device context block.

struct ADCB {
  UBYTE devtype;

  UBYTE adapter;
  UBYTE id;
  UBYTE lun;

  UWORD model;
  UWORD reserved;

  char vendor_id[9];
  char product_id[17];
  char firmware_level[5];

  char device_letter;
  };

//**********************************************************************
//
// ASPI device class
//
//**********************************************************************

class ASPIDevice {
  public:
    ASPIDevice(ADCB *adcbP);
    virtual ~ASPIDevice();

    // Get the inquiry data for a device.
    // Note: This is used when there is no device object yet.
    static int Inquiry(
      UBYTE adapter, UBYTE id, UBYTE lun,
      INQUIRYDATA *inquirydata, SENSE *senseP, BOOL sig_flag);

    // Check sense information.
    static BOOL CheckSense(SENSE *senseP, UBYTE key, UBYTE asc, UBYTE ascq);

    // Execute an ASPI command request.
    int ExecuteCommandRequest(
      UBYTE cmd, ULONG flags, void *cdbP, UBYTE cdblen, void *bufP = NULL, ULONG buflen = 0);

    // Execute a synchronous ASPI request.
    int ExecuteCommandRequestSync(
      UBYTE cmd, ULONG flags, void *cdbP, UBYTE cdblen, void *bufP, ULONG buflen);

    #if defined(WIN32)
    // Execute an asynchronous ASPI request.
    int ExecuteCommandRequestAsync(
      UBYTE cmd, ULONG flags, void *cdbP, UBYTE cdblen, void *bufP, ULONG buflen);

    // Wait for an ASPI request to complete.
    int WaitForRequestCompletion(ULONG timeout);
    #endif

    // Abort an ASPI command request.
    int AbortRequest();

    // Log an ASPI command request.
    void LogRequest(
      UBYTE cmd, ULONG flags, void *cdbP, UBYTE cdblen, void *bufP = NULL, ULONG buflen = 0);

    // Initialize a device chain (multi device support).
    int InitializeMultiDeviceChain();

    // Mark a device as failed (multi device support).
    void MarkFailed();
    void MarkFailed(SLONG code, ULONG count = 0, ...);
    void UnmarkFailed();

    // Force the OS to refresh a device's file system.
    BOOL RefreshFileSystem();

    // Functions that execute an ASPI command (jackets for actual calls).
    int FormatUnit(
      UBYTE format_code, void *format_list, UWORD format_list_len, BOOL sig_flag);
    int GetConfiguration(
      void *feature_list, UWORD feature_list_len,
      UBYTE request_type, UWORD starting_feature, BOOL sig_flag);
    int GetPerformance(
      void *desc_list, UWORD desc_list_len, UBYTE desc_type, UBYTE datatype,
      UWORD max_desc_count, ULONG starting_lba, BOOL sig_flag);
    int Inquiry(INQUIRYDATA *inquirydata, BOOL sig_flag);
    int ModeSelect6(
      void *param_list, UBYTE param_len, BOOL page_flag, BOOL save_flag, BOOL sig_flag);
    int ModeSelect10(
      void *param_list, UWORD param_len, BOOL page_flag, BOOL save_flag, BOOL sig_flag);
    int ModeSense6(
      void *param_list, UBYTE param_len, UBYTE pagecode,
      UBYTE type, BOOL block_desc_flag, BOOL sig_flag);
    int ModeSense10(
      void *param_list, UWORD param_len, UBYTE pagecode,
      UBYTE type, BOOL block_desc_flag, BOOL sig_flag);
    int ReadBlocks(
      void *bufP, UWORD blklen, UWORD blocks, SLONG lba, BOOL sig_flag);
    int PreventMediumRemoval(BOOL prevent_flag, BOOL sig_flag);
    int ReadCapacity(SLONG *lbaP, ULONG *blklenP, BOOL blank_flag, BOOL sig_flag);
    int ReadFormatCapacities(void *format_list, UWORD format_list_len, BOOL sig_flag);
    int Reset(BOOL sig_flag);
    int RezeroUnit(BOOL sig_flag);
    int SetErrorRecovery(UBYTE flags, UBYTE retry_count, BOOL sig_flag);
    int SetLimits(SLONG lba, ULONG blocks, BOOL sig_flag);
    int StartStopUnit(BOOL start_flag, BOOL loadeject_flag, BOOL immed_flag, BOOL sig_flag);
    int SynchronizeCache(BOOL immediate_flag, BOOL sig_flag);
    int TestUnitReady(BOOL sig_flag);
    int WaitForUnitReady(UBYTE timeout);
    int WriteBlocks(void *bufP, UWORD blklen, UWORD blocks, SLONG lba, BOOL sig_flag);

    // Virtual mode select functions.
    virtual int SetBlockDesc(UBYTE density, UWORD blklen, ULONG blocks)
      {return SetBlockDesc6 (density, blklen, blocks);}
    virtual int SetErrorRecovery(UBYTE mode, UBYTE retry_count)
      {return SetErrorRecovery6 (mode, retry_count);}
    virtual int SetInactivityMultiplier(UBYTE multiplier)
      {return SetInactivityMultiplier6 (multiplier);}

    // Check sense information.
    BOOL CheckSense(UBYTE key);
    BOOL CheckSense(UBYTE key, UBYTE asc);
    BOOL CheckSense(UBYTE key, UBYTE asc, UBYTE ascq);

    // Get functions.
    char *GetVendorId()             {return VendorId;}
    char *GetProductId()            {return ProductId;}
    char *GetFirmwareLevel()        {return FirmwareLevel;}
    
    UBYTE GetDeviceType()           {return DeviceType;}
    UWORD GetModel()                {return Model;}

    char GetDeviceLetter()          {return DeviceLetter;}

    UBYTE GetAdapter()              {return Adapter;}
    UBYTE GetId()                   {return Id;}
    UBYTE GetLogUnitNum()           {return LogUnitNum;}

    UBYTE GetAspiStatus()           {return SRBCommand.header.status;}
    UBYTE GetAdapterStatus()        {return SRBCommand.adapter_status;}
    UBYTE GetTargetStatus()         {return SRBCommand.target_status;}

    ULONG GetRequestCount()         {return RequestCount;}
    ULONG GetReserved()             {return Reserved;}

    int GetCompletionStatus()       {return CompletionStatus;}

    int GetFailedCompletionStatus() {return FailedCompletionStatus;}
    int GetFailedAspiStatus()       {return FailedAspiStatus;}
    int GetFailedAdapterStatus()    {return FailedAdapterStatus;}
    int GetFailedTargetStatus()     {return FailedTargetStatus;}
    int GetFailedSenseKey()         {return FailedSenseKey;}
    int GetFailedSenseASC()         {return FailedSenseASC;}
    int GetFailedSenseASCQ()        {return FailedSenseASCQ;}

    void GetSense(SENSE *senseP);
    UBYTE GetSenseKey();
    UBYTE GetSenseASC();
    UBYTE GetSenseASCQ();

  protected:
    // Functions that directly execute an ASPI command.
    int ExecuteBlank(UBYTE type, BOOL immed_flag, ULONG param);
    int ExecuteFormatUnit(UBYTE format_code, void *format_list, UWORD format_list_len);
    int ExecuteGetConfiguration(
      void *feature_list, UWORD feature_list_len, UBYTE request_type, UWORD starting_feature);
    int ExecuteGetPerformance(
      void *desc_list, UWORD desc_list_len, UBYTE desc_type, UBYTE datatype,
      UWORD max_desc_count, ULONG starting_lba);
    int ExecuteInquiry(INQUIRYDATA *inquirydata);
    int ExecuteModeSelect6(void *param_list, UBYTE param_len, BOOL page_flag, BOOL save_flag);
    int ExecuteModeSelect10(void *param_list, UWORD param_len, BOOL page_flag, BOOL save_flag);
    int ExecuteModeSense6(
      void *param_list, UBYTE param_len, UBYTE pagecode, UBYTE type, BOOL block_desc_flag);
    int ExecuteModeSense10(
      void *param_list, UWORD param_len, UBYTE pagecode, UBYTE type, BOOL block_desc_flag);
    int ExecutePreventMediumRemoval(BOOL prevent_flag);
    int ExecuteReadBlocks(
      void *bufP, UWORD blklen, UWORD blocks, SLONG lba, UBYTE vendor = 0);
    int ExecuteReadCapacity(SLONG *lbaP, ULONG *blklenP, BOOL blank_flag);
    int ExecuteReadFormatCapacities(void *format_list, UWORD format_list_len);
    int ExecuteReset();
    int ExecuteRezeroUnit();
    int ExecuteStartStopUnit(BOOL start_flag, BOOL loadeject_flag, BOOL immed_flag);
    int ExecuteSetLimits(SLONG lba, ULONG blocks);
    int ExecuteSynchronizeCache(BOOL immediate_flag);
    int ExecuteTestUnitReady();
    int ExecuteWriteBlocks(void *bufP, UWORD blklen, UWORD blocks, SLONG lba, UBYTE vendor = 0);
    int SetBlockDesc6(UBYTE density, UWORD blklen, ULONG blocks);
    int SetBlockDesc10(UBYTE density, UWORD blklen, ULONG blocks);
    int SetErrorRecovery6(UBYTE mode, UBYTE retry_count);
    int SetErrorRecovery10(UBYTE mode, UBYTE retry_count);
    int SetInactivityMultiplier6(UBYTE multiplier);
    int SetInactivityMultiplier10(UBYTE multiplier);

  private:
    int GenerateCompletionStatus();
    int ExecuteCommandRequestMultiple(
      UBYTE cmd, ULONG flags, void *cdbP, UBYTE cdblen, void *bufP, ULONG buflen);

  public:
    IOBuffer Buffer;                  // I/O Buffer

    ASPIDevice *pNextDevice;          // Next device in chain (multi device support)
    BOOL DeviceFailed;                // Device has failed (multi device support)
    MSGVEC ExceptionMsgVec;           // Exception message vector (multi device support)

  private:
    SRBCOMMAND SRBCommand;            // ASPI request block

    #if defined(WIN32)
    HANDLE ASPIEventHandle;           // ASPI completion event handle

    void *pUserBuffer;                // User buffer pointer
    ULONG UserBufferLength;           // User buffer length
    ULONG UserFlags;                  // User flags
    #endif

    char VendorId[9];                 // Vendor ID string
    char ProductId[17];               // Product ID string
    char FirmwareLevel[5];            // Firmware level revision string

    char DeviceLetter;                // Device letter (Nexitech driver only)

    UWORD Model;                      // Model type
    UBYTE Reserved1[2];

    UBYTE DeviceType;                 // Device type
    UBYTE Adapter;                    // Host Adapter Number
    UBYTE Id;                         // ASPI ID
    UBYTE LogUnitNum;                 // Logical Unit Number

    ULONG RequestCount;               // # of ASPI requests executed
    ULONG Reserved;                   // Reserved

    int CompletionStatus;             // Completion status from last command
    int FailedCompletionStatus;       // Completion status from last failed command

    UBYTE FailedAspiStatus;           // ASPI status from last failed command
    UBYTE FailedAdapterStatus;
    UBYTE FailedTargetStatus;
    UBYTE FailedSenseKey;
    UBYTE FailedSenseASC;
    UBYTE FailedSenseASCQ;
    UBYTE Reserved2[2];
};

//**********************************************************************
//
// ASPI adapter class
//
//**********************************************************************

#define ASPI_DRIVER_DEFAULT       0   // Load WNASPI32.DLL
#define ASPI_DRIVER_GOLDENHAWK    1   // Load WNASPI9X.DLL or WNASPINT.DLL

#define ASPI_DEF_MAX_BUFFER_SIZE  65000

class ASPIAdapter {
  public:
    // Platform ID
    static ULONG PlatformId;

    // Log file pointer
    static FILE *LogFile;

    // Adapter objects
    static ASPIAdapter *AdapterPtr[16];
    static UWORD AdapterCount;

    // Maximum I/O buffer size
    static UWORD MaxBufferSize;

    // Maximum device chain length (multi-device support).
    static int MaxDeviceChainLength;

    // Multi-device command request timeout (in milliseconds)
    static ULONG MultiRequestTimeout;

    // ASPI manager functions
    static BOOL ASPIAdapter::GetDeviceType(
      UBYTE adapter, UBYTE id, UBYTE lun, UBYTE *device_type, char *device_letter);
    static int HostAdapterInquiry(UBYTE adapter, HAINQUIRYDATA *hainquiry);
    static void SetMaxBufferSize(UWORD buffer_size) {MaxBufferSize = buffer_size;}
    static void SetMaxDeviceChainLength(int length) {MaxDeviceChainLength = length;}
    static void ShutdownManager();
    static int StartupManager(BOOL lun_flag, BOOL wide_flag, BOOL sig_flag);
    static int StartupManager(
      int driver_type, const TCHAR *driver_filnamP, UWORD max_buffer_size,
      BOOL lun_flag, BOOL wide_flag, BOOL logfile_flag, BOOL sig_flag);

    // Device object management functions
    static int GetDeviceObjects(UWORD devtype_mask, ASPIDevice **device_list, int list_size);

    static ASPIDevice *FindDeviceObject(UWORD devtype_mask);
    static ASPIDevice *FindDeviceObject(UWORD devtype_mask, const char *devname);
    static ASPIDevice *FindDeviceObject(UWORD devtype_mask, UBYTE adapter, UBYTE id, UBYTE lun);

    // Get functions
    ASPIDevice *GetDevicePtr(int index) {return DevicePtr[index];}
    UWORD GetDeviceCount()              {return DeviceCount;}
    char *GetManagerName()              {return ManagerName;}
    char *GetAdapterName()              {return AdapterName;}
    UBYTE GetAdapterNumber()            {return AdapterNumber;}
    UBYTE GetAspiId()                   {return AspiId;}
    UBYTE GetMaxTargetCount()           {return MaxTargetCount;}
    ULONG GetMaxTransferLength()        {return MaxTransferLength;}
    UWORD GetBufferAlignmentMask()      {return BufferAlignmentMask;}
    BOOL GetResidualCountFlag()         {return ResidualCountFlag;}

  private:
    // Initialize the ASPI manager
    static int InitializeManager(
      int driver_type, const TCHAR *driver_filnamP, UWORD max_buffer_size,
      BOOL lun_flag, BOOL wide_flag, BOOL logfile_flag);

    // Load the ASPI manager
    static int LoadManager(int driver_type, const TCHAR *driver_filnamP);

    // Adapter object management functions
    static void BuildAdapterObjects(BOOL lun_flag = FALSE, BOOL wide_flag = FALSE);
    static void FreeAdapterObjects();

    // Device object management functions
    void BuildDeviceObjects(BOOL lun_flag = FALSE, BOOL wide_flag = FALSE);
    void FreeDeviceObjects();
    ASPIDevice *CreateDeviceObject(UBYTE id, UBYTE lun, INQUIRYDATA *inquirydata);

  private:
    ASPIDevice *DevicePtr[64];  // ASPI device object pointers
    UWORD DeviceCount;          // ASPI device count
    UWORD Reserved1;

    char ManagerName[17];       // Manager name string
    char AdapterName[17];       // Adapter name string
    UWORD Reserved2;

    UBYTE AdapterNumber;        // Host Adapter Number
    UBYTE AspiId;               // ASPI ID
    UBYTE MaxTargetCount;       // Maximum number of targets (devices) supported
    UBYTE Reserved3;

    ULONG MaxTransferLength;    // Maximum transfer length
    UWORD BufferAlignmentMask;  // Buffer alignment mask
    UWORD Reserved4;

    BOOL ResidualCountFlag;     // Residual byte count supported?
  };


// Signal an ASPI command exception.

#define ASPIEXCEPTION(string)                                                                       \
  SignalException (E_CommandFailed, 4, status, 6,                                                   \
    _T(string), (ULONG)GetAdapter(), (ULONG)GetId(), (ULONG)GetLogUnitNum(),                        \
    (ULONG)GetFailedAspiStatus(), (ULONG)GetFailedAdapterStatus(), (ULONG)GetFailedTargetStatus(),  \
    (ULONG)GetFailedSenseKey(), (ULONG)GetFailedSenseASC(), (ULONG)GetFailedSenseASCQ())

// Move a 16-bit value from a CDB field.

inline UWORD Move16BitsFromCDB(UBYTE *cdbfld)
{
  UWORD uw = *(UWORD *)cdbfld;
  return FLIP16(uw);
}

// Move a 24-bit value from a CDB field.

inline ULONG Move24BitsFromCDB(UBYTE *cdbfld)
{
  UBYTE ul[4];

  ul[0] = cdbfld[2];
  ul[1] = cdbfld[1];
  ul[2] = cdbfld[0];
  ul[3] = 0;

  return *(ULONG *)ul;
}

// Move a 32-bit value from a CDB field.

inline ULONG Move32BitsFromCDB(UBYTE *cdbfld)
{
  ULONG ul = *(ULONG *)cdbfld;
  return FLIP32(ul);
}

// Move a 16-bit value to a CDB field.

inline void Move16BitsToCDB(UWORD value, UBYTE *cdbfld)
{
  UBYTE *bptr = (UBYTE *)&value;

  cdbfld[0] = bptr[1];
  cdbfld[1] = bptr[0];
}

// Move a 24-bit value to a CDB field.

inline void Move24BitsToCDB(ULONG value, UBYTE *cdbfld)
{
  UBYTE *bptr = (UBYTE *)&value;

  cdbfld[0] = bptr[2];
  cdbfld[1] = bptr[1];
  cdbfld[2] = bptr[0];
}

// Move a 32-bit value to a CDB field.

inline void Move32BitsToCDB(ULONG value, UBYTE *cdbfld)
{
  UBYTE *bptr = (UBYTE *)&value;

  cdbfld[0] = bptr[3];
  cdbfld[1] = bptr[2];
  cdbfld[2] = bptr[1];
  cdbfld[3] = bptr[0];
}

// Get the name string of an ASPI opcode

char *ASPIOpcodeName(UBYTE opcode);

// Restore previous packing alignment.

#pragma pack(pop)

#endif

