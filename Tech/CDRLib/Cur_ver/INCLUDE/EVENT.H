#if !defined(CDRLIB_EVENT_H)
#define CDRLIB_EVENT_H

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

// Event callback codes.

#define EVENT_ANALYZE_DISC_START        0
#define EVENT_ANALYZE_DISC_END          1
#define EVENT_CLOSE_SESSION_START       2
#define EVENT_CLOSE_SESSION_END         3
#define EVENT_ERASE_DISC_START          4
#define EVENT_ERASE_DISC_END            5
#define EVENT_FORMAT_DISC_START         6
#define EVENT_FORMAT_DISC_END           7
#define EVENT_LONG_OPERATION_PROGRESS   8
#define EVENT_EXCHANGE_DISC_START       9
#define EVENT_EXCHANGE_DISC_END         10
#define EVENT_EXTRACT_AUDIO_START       11
#define EVENT_EXTRACT_AUDIO_PROGRESS    12
#define EVENT_EXTRACT_AUDIO_END         13
#define EVENT_EXTRACT_DATA_START        14
#define EVENT_EXTRACT_DATA_PROGRESS     15
#define EVENT_EXTRACT_DATA_END          16
#define EVENT_ANALYZE_SUBCODE_START     17
#define EVENT_ANALYZE_SUBCODE_PROGRESS  18
#define EVENT_ANALYZE_SUBCODE_END       19
#define EVENT_RECORD_GAP_START          20
#define EVENT_RECORD_GAP_PROGRESS       21
#define EVENT_RECORD_GAP_END            22
#define EVENT_RECORD_USERDATA_START     23
#define EVENT_RECORD_USERDATA_END       24
#define EVENT_RECORD_TRACK_START        25
#define EVENT_RECORD_TRACK_PROGRESS     26
#define EVENT_RECORD_TRACK_END          27
#define EVENT_RECORD_LEADIN_START       28
#define EVENT_RECORD_LEADIN_END         29
#define EVENT_RECORD_LEADOUT_START      30
#define EVENT_RECORD_LEADOUT_END        31
#define EVENT_BUILD_CDFS_START          32
#define EVENT_BUILD_CDFS_PROGRESS       33
#define EVENT_BUILD_CDFS_END            34
#define EVENT_VERIFY_DISC_START         35
#define EVENT_VERIFY_DISC_PROGRESS      36
#define EVENT_VERIFY_DISC_END           37
#define EVENT_PROCESS_FILE_START        38
#define EVENT_PROCESS_FILE_END          39
#define EVENT_IMPORT_SESSION_START      40
#define EVENT_IMPORT_SESSION_PROGRESS   41
#define EVENT_IMPORT_SESSION_END        42
#define EVENT_COMPILE_CUESHEET_START    43
#define EVENT_COMPILE_CUESHEET_END      44
#define EVENT_OVERWRITE_FILE_PROMPT     45
#define EVENT_DEBUG_VALUE               46
#define EVENT_DEBUG_STRING              47

#define EVENT_GAPTYPE_PREGAP            0
#define EVENT_GAPTYPE_POSTGAP           1
#define EVENT_GAPTYPE_LEADIN            2
#define EVENT_GAPTYPE_LEADOUT           3

struct CDREVENT {
  ULONG code;
  ULONG paramcount;
  ULONG param[8];
  };

// Event handling prototypes.

typedef BOOL (EVENTCALLBACK)(void *param, CDREVENT *event);

void EventRegisterCallback(EVENTCALLBACK *callback, void *param = NULL);
void EventUnregisterCallback(void);

BOOL EventExecuteCallback(ULONG code, ULONG paramcount = 0, ...);

BOOL ConsoleEventCallback(void *param, CDREVENT *event);

// Thread abort prototypes (32-bit only).

#if defined(WIN32)
void RegisterAbortEvent(void *event);
void UnregisterAbortEvent();
BOOL CheckAbortEvent();
#endif

// Restore previous packing alignment.

#pragma pack(pop)

#endif
