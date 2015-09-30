#if !defined(CDRLIB_EXCEPT_H)
#define CDRLIB_EXCEPT_H

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

// Error codes

#define E_Failure                 0     // Failure status
#define E_Success                 1     // Success status

#define E_AspiInit                -1    // Unable to initialize ASPI manager
#define E_NoAspiManager           -2    // ASPI manager was not found on system
#define E_NoAspiProcedure         -3    // ASPI procedure entry point was not found
#define E_NoAspiSupportInfo       -4    // ASPI support information was not found
#define E_AspiAbort               -5    // ASPI request aborted
#define E_AspiAbortFail           -6    // ASPI abort request failed
#define E_AspiInvalidRequest      -7    // Invalid ASPI request
#define E_AspiInvalidAdapter      -8    // Invalid host adapter number
#define E_AspiNoDevice            -9    // Device is not installed
#define E_AspiInvalidSRB          -10   // Invalid SRB
#define E_AspiBufferAlign         -11   // Bad buffer alignment
#define E_AspiIllegalMode         -12   // Illegal mode (Win32S)
#define E_AspiNoHelperDriver      -13   // No ASPI helper driver
#define E_AspiFailedInit          -14   // General initialization failure
#define E_AspiBusy                -15   // ASPI manager is busy
#define E_AspiBufferTooBig        -16   // Buffer is too big
#define E_AspiMismatchedComp      -17   // Mismatched components
#define E_AspiNoAdapters          -18   // No host adapters on system
#define E_AspiNoResources         -19   // Insufficient resources
#define E_AspiUnknown             -20   // Unknown ASPI error
#define E_AspiTimeout             -21   // ASPI request timed out
#define E_AspiRequestFailed       -22   // ASPI request failed to complete successfully
#define E_AspiSendRequestFailed   -23   // ASPI request failed to queue successfully

#define E_AdapterTimeout          -24   // Transaction timeout
#define E_AdapterExpired          -25   // SRB expired
#define E_AdapterMessageReject    -26   // Message reject
#define E_AdapterBusReset         -27   // Bus reset was detected
#define E_AdapterParityError      -28   // Parity error was detected
#define E_AdapterSenseFailed      -29   // Auto request sense failed
#define E_AdapterSelectTimeout    -30   // Selection timeout
#define E_AdapterDataOverrun      -31   // Data overrun/underrun
#define E_AdapterBusFree          -32   // Unexpected bus free
#define E_AdapterBusFailure       -33   // Target bus phase sequence failure
#define E_AdapterUnknown          -34   // Unexpected adapter error

#define E_TargetBusy              -35   // Target is busy
#define E_TargetConflict          -36   // Target reservation conflict
#define E_TargetUnknown           -37   // Unexpected target error

#define E_Recovered               -38   // Recovered error
#define E_DeviceNotReady          -39   // Device is not ready
#define E_BadMedium               -40   // Bad Medium
#define E_Hardware                -41   // Non-recoverable hardware error
#define E_IllegalRequest          -42   // Illegal Request
#define E_UnitAttention           -43   // Unit Attention
#define E_BlankCheck              -44   // Encountered non-blank data
#define E_DeviceBusy              -45   // Device is busy
#define E_TargetAbort             -46   // Target aborted the command
#define E_VolumeOverflow          -47   // Volume overflow
#define E_EndOfMedium             -48   // End-of-Medium detected
#define E_SenseUnknown            -49   // Unexpected sense error

#define E_UserAbort               -100  // User aborted operation
#define E_Generic                 -101  // Generic error message text
#define E_BugCheck                -102  // Internal bugcheck error
#define E_NoMemory                -103  // Memory allocation error
#define E_NotImplemented          -104  // Feature is not implemented yet
#define E_CommandFailed           -105  // ASPI command failed
#define E_ReadAudio               -106  // Read audio error
#define E_ReadData                -107  // Read data error
#define E_OpenFile                -108  // File open error
#define E_CreateFile              -109  // File create error
#define E_FileNotFound            -110  // File not found
#define E_FileIsEmpty             -111  // File contains no data
#define E_ReadFile                -112  // File read error
#define E_WriteFile               -113  // File write error
#define E_EndOfFile               -114  // End-of-file error
#define E_FindLastSector          -115  // Unable to find last sector
#define E_ParseCueSheet           -116  // Cue Sheet parsing error
#define E_BadWaveFile             -117  // WAVE file is invalid or not CDROM compatible
#define E_BadAiffFile             -118  // AIFF file is invalid or not CDROM compatible
#define E_BadMP3File              -119  // MP3 file is invalid or not CDROM compatible
#define E_JitterFailure           -110  // Jitter correction failure
#define E_AnalyzeDisc             -121  // Analyze disc error
#define E_NoMultisession          -122  // Disc contains more than one session
#define E_BadTrackType            -123  // Illegal track type encountered
#define E_TooManyFiles            -124  // Too many files in directory
#define E_TooManyDirs             -125  // Too many directories to process
#define E_BadPathname             -126  // Invalid directory name specified
#define E_MaxFileSort             -127  // Too many file/directories to sort
#define E_CDFSImageTooLong        -128  // CDFS image is too large to fit on disc
#define E_BadFileLength           -129  // File length is not a multiple of the sector size
#define E_BadBootFile             -130  // Boot image file length is bad
#define E_NoCacheMemory           -131  // Not enough memory available for cache
#define E_CreateFileCache         -132  // Unexpected failure creating file cache
#define E_CreateDiscCache         -133  // Unexpected failure creating disc cache
#define E_CreateTreeCache         -134  // Unexpected failure creating directory tree cache
#define E_ReadFileCache           -135  // Unexpected failure reading file cache
#define E_ReadDiscCache           -136  // Unexpected failure reading disc cache
#define E_ReadTreeCache           -137  // Unexpected failure reading directory tree cache
#define E_CacheTimeout            -138  // Read request timeout encountered (cache is empty)
#define E_CacheReadError          -139  // File/Disc read error encountered
#define E_CacheEndOfData          -140  // End-of-data encountered (not enough data in cache)
#define E_VerifyData              -141  // Data verification failure at sectors %lu to %lu
#define E_ProgramAbort            -142  // Program aborted - Internal bugcheck detected
#define E_BadBlockLength          -143  // Bugcheck - Invalid block length specified
#define E_BadDataType             -144  // Bugcheck - Invalid datatype specified
#define E_ReadVolDesc             -145  // Unexpected error reading volume descriptor
#define E_BadVolDesc              -146  // Illegal volume descriptor encountered
#define E_NoPrimaryDesc           -147  // No primary volume descriptor was found
#define E_ImportDirectory         -148  // Unexpected error importing directory structure
#define E_FilenameTooLong         -149  // One of more filenames is greater than 64 characters long
#define E_ReadClientData          -150  // Unexpected failure reading data from client
#define E_BadPregapLength         -151  // Illegal track or pregap length
#define E_CantFindAudioEnd        -152  // Unable to locate end of AUDIO track
#define E_CantFindMode1End        -153  // Unable to locate end of MODE1 data track
#define E_CantFindMode2End        -154  // Unable to locate end of MODE2 data track
#define E_ImageFileTooBig         -155  // Image files larger than 4 gigabytes are not supported!

#define E_BadKeyMatch             -200  // Bad primary/secondary key match
#define E_BadUnlockInfo           -201  // Bad unlock strings and/or keys
#define E_BadAuthData             -202  // Bad authorization file data
#define E_ExpiredKey              -203  // Unlock keys have expired
#define E_VersionTooOld           -204  // Software version is out of date (please upgrade)
#define E_InitControlFile         -205  // Unable to initialize control file
#define E_UpdateControlFile       -206  // Unable to update control file
#define E_SoftwareLocked          -207  // Software is locked and has no demo mode
#define E_NoDiscAtOnce            -208  // Recorder does not support DAO recording
#define E_NoTransporter           -209  // No disc transporter was found on the bus
#define E_EjectDiscFailed         -210  // Eject disc command failed
#define E_LoadDiscFailed          -211  // Load disc command failed
#define E_TransError              -212  // Transporter command failed
#define E_TransDoorOpen           -213  // Transporter door is open
#define E_TransNoDisc             -214  // Transporter input disc holder is empty
#define E_TransInvalid            -215  // Transporter command is invalid
#define E_TransBusy               -216  // Transporter device is busy
#define E_TransNoHolder           -217  // Transporter input disc holder is missing
#define E_TransNoBulk             -218  // Transporter output disc spindle is missing
#define E_TransTimeout            -219  // Transporter device is not responding or timed out
#define E_TransNoPort             -220  // Unable to open disc transporter communications port

#define E_DiscNotSupported        -300  // Disc type is not supported
#define E_DiscNotWritable         -301  // Disc is not writable
#define E_DiscNotRewritable       -302  // Disc is not re-writable
#define E_SetBlockLength          -303  // Unable to set block length
#define E_ReadDiscInfo            -304  // Unable to read disc information
#define E_ReadTrackInfo           -305  // Unable to read track information
#define E_NextWritableAddress     -306  // Unable to determine next writable address
#define E_SetWriteMode            -307  // Unable to set writing mode
#define E_SetTrackParameters      -308  // Unable to set track-at-once parameters
#define E_SetDiscParameters       -309  // Unable to set disc-at-once parameters
#define E_SetRecordSpeed          -310  // Unable to set recording speed
#define E_FlushCache              -311  // Unable to flush CD recorder cache to disc
#define E_SetCatalogNumber        -312  // Unable to set media catalog number
#define E_LaserCalibration        -313  // Unable to calibrate recording laser
#define E_CueSheetRejected        -314  // Cuesheet is invalid for the specified CD recorder
#define E_BadTwoPartPregap        -315  // Illegal two-part PREGAP length specified (too small)
#define E_TooMuchText             -316  // Too much CD-TEXT information has been specified
#define E_BadCDTextFile           -317  // CD-TEXT file is invalid or corrupt
#define E_DifferentDisc           -318  // Disc is incompatible with discs in other recorders
#define E_ReserveTrack            -319  // Unable to reserve track

#define E_BadCuesheetSyntax       -400  // Illegal cuesheet command syntax
#define E_MultiplePerDisc         -401  // Command can not be specified more than once per disc
#define E_MultiplePerTrack        -402  // Command can not be specified more than once per track
#define E_BadCommandPlacement     -403  // Illegal command placement
#define E_BadFiletype             -404  // Illegal filetype specified
#define E_BadSizeMultiple         -405  // Previous file is not a multiple of the specified block size
#define E_BadCatalogNumber        -406  // CATALOG number is wrong length or contains illegal characters
#define E_BadISRC                 -407  // ISRC is wrong length or contains illegal characters
#define E_BadMSF                  -408  // Illegal MM:SS:FF time value specified!
#define E_NoFileCommand           -409  // No FILE command has been specified yet
#define E_NoTrackCommand          -410  // No TRACK command has been specified yet
#define E_BadTrackNumber          -411  // Illegal TRACK number specified
#define E_TracksNotSequential     -412  // TRACK numbers are not in sequential order
#define E_BadIndexNumber          -413  // Illegal INDEX number specified
#define E_IndexesNotSequential    -414  // INDEX numbers are not in sequential order
#define E_IndexTimesNotAscending  -415  // INDEX times are not in ascending order
#define E_IndexBeyondEOF          -416  // INDEX time is beyond the end-of-file
#define E_BadFirstIndex           -417  // First INDEX must be -400 or 1
#define E_BadFirstIndexMSF        -418  // First INDEX of file does not start at 00:00:00
#define E_NoTracksDefined         -419  // No TRACKS were defined for the previous file
#define E_NoIndexOne              -420  // No starting INDEX was defined for the previous track
#define E_TooManyCuepoints        -421  // The maximum number of cuepoints has been exceeded
#define E_CantReadCDText          -422  // Unable to read CD-TEXT file
#define E_TooMuchCDText           -423  // CD-TEXT file contains too many entries
#define E_NoCuesheetTracks        -424  // No tracks were defined in the cuesheet
#define E_NoTracksLastFile        -425  // No tracks were defined for the last file in the cuesheet
#define E_NoIndexesLastTrack      -426  // No starting INDEX was defined for the last track in the cuesheet

#define E_ISO9660BugCheck         -500

// Message vector

struct MSGVEC {
  SLONG code;
  ULONG count;
  SLONG subcode;
  ULONG subcount;
  ULONG param[8];
  ULONG subparam[8];
  };

// Exception message.

struct EXCEPTMSG {
  SLONG code;
  ULONG argcount;
  TCHAR *text;
  };

// Exception handling macros.

#define EXCEPTION_HANDLER_START try {

#define EXCEPTION_HANDLER_CONTINUE          \
  } catch (MSGVEC *msgvecP)                 \
      {                                     \
      ConsolePrintException(msgvecP);       \
      delete msgvecP;                       \
      }

#define EXCEPTION_HANDLER_RETURN            \
  } catch (MSGVEC *msgvecP)                 \
      {                                     \
      ConsolePrintException(msgvecP);       \
      delete msgvecP;                       \
      return;                               \
      }

#define EXCEPTION_HANDLER_RETVAL(value)     \
  } catch (MSGVEC *msgvecP)                 \
      {                                     \
      ConsolePrintException(msgvecP);       \
      delete msgvecP;                       \
      return(value);                        \
      }

#define EXCEPTION_HANDLER_EXIT              \
  } catch (MSGVEC *msgvecP)                 \
      {                                     \
      SLONG code = msgvecP->code;           \
      ConsolePrintException(msgvecP);       \
      delete msgvecP;                       \
      exit(code);                           \
      }

// Function prototypes.

void SignalException(
  SLONG code, ULONG count = 0, SLONG subcode = E_Success, ULONG subcount = 0, ...);

void ConsolePrintException(MSGVEC *msgvecP);
void ConsolePrintWarning(MSGVEC *msgvecP);

EXCEPTMSG *GetMessagePtr(SLONG code);
TCHAR *GetMessageText(SLONG code);

// Restore previous packing alignment.

#pragma pack(pop)

#endif
