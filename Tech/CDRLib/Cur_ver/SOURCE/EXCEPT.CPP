#if defined(WIN32)
#include <windows.h>
#include <tchar.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <io.h>
#include <dos.h>
#include <malloc.h>
#include <string.h>

#pragma hdrstop

#include "datatype.h"
#include "unicode.h"
#include "misc.h"
#include "except.h"

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

//
// Exception message table.
//

#if defined(_GERMAN)
EXCEPTMSG exceptmsg_table[] = {
  {E_Success,                 0, _T("Vorgang abgeschlossen!")},
  {E_Failure,                 0, _T("Fehler!")},
  {E_UserAbort,               0, _T("Der Vorgang wurde vom Benutzer abgebrochen")},

  {E_AspiInit,                0, _T("Es konnte kein ASPI-Manager initialisiert werden.\nSiehe http://www.aspi-treiber.de/")},
  {E_NoAspiManager,           0, _T("Es konnte kein ASPI-Manager gefunden werden!")},
  {E_NoAspiProcedure,         0, _T("Ausführung des ASPI-Befehls wurde abgelehnt!")},
  {E_NoAspiSupportInfo,       0, _T("Es konnten keine ASPI-Informationen gefunden werden!")},
  {E_AspiAbort,               0, _T("ASPI-Zugriff abgebrochen (%02lXh %02lXh %02lXh)")},
  {E_AspiAbortFail,           0, _T("ASPI-Abbruch fehlgeschlagen")},
  {E_AspiInvalidRequest,      0, _T("Ungültiger ASPI-Befehl")},
  {E_AspiInvalidAdapter,      0, _T("Ungültige Nummer des SCSI-Controllers!")},
  {E_AspiNoDevice,            0, _T("Das gewählte Laufwerk existiert nicht!")},
  {E_AspiInvalidSRB,          0, _T("Ungültiger SCSI-Befehlsblock")},
  {E_AspiBufferAlign,         0, _T("Ungültige Zuordnung des Puffers")},
  {E_AspiIllegalMode,         0, _T("Illegaler Modus (Win32S-Operation nicht erlaubt)")},
  {E_AspiNoHelperDriver,      0, _T("Es konnte kein ASPI-Helper-Treiber gefunden werden")},
  {E_AspiFailedInit,          0, _T("ASPI-Initialisierung fehlgeschlagen")},
  {E_AspiBusy,                0, _T("Der ASPI-Manager wird bereits von einem anderen Programm benutzt")},
  {E_AspiBufferTooBig,        0, _T("Der Puffer des Controllers ist zu groß!")},
  {E_AspiMismatchedComp,      0, _T("Ungültige ASPI-Komponenten gefunden...")},
  {E_AspiNoAdapters,          0, _T("Es konnte kein ASPI-Controller gefunden werden")},
  {E_AspiNoResources,         0, _T("Der ASPI-Manager konnte aufgrund von Speichermangel nicht initialisiert werden!")},
  {E_AspiUnknown,             0, _T("Unvorhergesehener ASPI-Fehler (%02lXh %02lXh %02lXh)")},
  {E_AspiTimeout,             0, _T("ASPI-Zugriff abgebrochen (Time out)!")},
  {E_AspiRequestFailed,       0, _T("ASPI-Zugriff konnte nicht beendet werden!")},

  {E_AdapterTimeout,          0, _T("Time Out")},
  {E_AdapterExpired,          0, _T("ASPI Request Expired")},
  {E_AdapterMessageReject,    0, _T("Message Reject Received")},
  {E_AdapterBusReset,         0, _T("ASPI Bus Reset!")},
  {E_AdapterParityError,      0, _T("ASPI Parity Fehler")},
  {E_AdapterSenseFailed,      0, _T("SENSE Request fehlerhaft")},
  {E_AdapterSelectTimeout,    0, _T("Geräteauswahl Time Out")},
  {E_AdapterDataOverrun,      0, _T("Daten Overrun/Underrun")},
  {E_AdapterBusFree,          0, _T("Kein unterstütztes Gerät gefunden!")},
  {E_AdapterBusFailure,       0, _T("Fehler am SCSI-Bus gefunden!")},
  {E_AdapterUnknown,          0, _T("Unerwarteter Fehler des SCSI-Controllers (%02lXh %02lXh %02lXh)!")},

  {E_TargetBusy,              0, _T("Zielgerät ist nicht bereit!")},
  {E_TargetConflict,          0, _T("Zielgerät konnte nicht reserviert werden!")},
  {E_TargetUnknown,           0, _T("Unerwarteter Fehler des Zielgerätes (%02lXh %02lXh %02lXh)!")},

  {E_Recovered,               0, _T("I/O Completed with Recovery")},
  {E_DeviceNotReady,          0, _T("Gerät ist nicht bereit (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_BadMedium,               0, _T("CD-Rohling defekt oder ASPI-Treiber inkompatibel. (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_Hardware,                0, _T("Nicht reparierbarer Hardware-Fehler (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_IllegalRequest,          0, _T("Ungültige Befehlsanforderung (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_UnitAttention,           0, _T("Unit Attention! (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_BlankCheck,              0, _T("Leeres Medium gefunden (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_DeviceBusy,              0, _T("Gerät wird schon benutzt! (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_TargetAbort,             0, _T("Gerät hat Befehl abgebrochen! (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_VolumeOverflow,          0, _T("Überlauf des Volume (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_EndOfMedium,             0, _T("Ende des Mediums gefunden (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_SenseUnknown,            0, _T("Unerwarteter SENSE-Fehler!")},

  {E_Generic,                 1, _T("%s")},
  {E_BugCheck,                0, _T("Interner Softwarefehler!")},
  {E_NoMemory,                0, _T("Zugriff auf Speicherzuordnung fehlgeschlagen!")},
  {E_NotImplemented,          0, _T("Dieses Feature wurde noch nicht implementiert!")},
  {E_CommandFailed,           4, _T("'%s' Befehl an Gerät %lu:%lu:%lu fehlgeschlagen")},
  {E_ReadAudio,               5, _T("AUDIO-Sektoren von %lu bis %lu an Gerät %lu:%lu:%lu konnten nicht gelesen werden!")},
  {E_ReadData,                5, _T("DATEN-Sektoren von %lu bis %lu an Gerät %lu:%lu:%lu konnten nicht gelesen werden!")},
  {E_OpenFile,                1, _T("Datei \"%s\" konnte nicht geöffnet werden!")},
  {E_CreateFile,              1, _T("Datei \"%s\" konnte nicht geschrieben werden!")},
  {E_FileNotFound,            0, _T("Datei konnte nicht gefunden oder geöffnet werden")},
  {E_FileIsEmpty,             0, _T("Datei ist leer...")},
  {E_ReadFile,                0, _T("Unerwarteter Fehler während des Lesens der aktuellen Datei (Defekte CD/Festplatte oder Datei zu Ende?)")},
  {E_WriteFile,               0, _T("Unerwarteter Fehler während des Schreibens der aktuellen Datei (Festplatte voll?)")},
  {E_EndOfFile,               1, _T("Unerwartetes Ende der Datei \"%s\"")},
  {E_FindLastSector,          0, _T("Der letzte lesbare Sektor der CD konnte nicht gefunden werden...")},
  {E_ParseCueSheet,           1, _T("Ungültiger Befehl im Cuesheet in Zeile %lu")},
  {E_BadWaveFile,             0, _T("WAVE-Datei ist ungültig oder nicht CD-ROM kompatibel!")},
  {E_BadAiffFile,             0, _T("AIFF-Datei ist ungültig oder nicht CD-ROM kompatibel!")},
  {E_BadMP3File,              0, _T("MP3-Datei ist ungültig oder nicht CD-ROM kompatibel!")},
  {E_JitterFailure,           0, _T("Unerwarteter Fehler innerhalb der Jitter-Korrektur!")},
  {E_AnalyzeDisc,             0, _T("Das CD-Layout konnte von CDRWIN nicht analysiert werden!")},
  {E_NoMultisession,          0, _T("Die CD enthält mehr als eine Session...")},
  {E_BadTrackType,            0, _T("Es befindet sich ein illegaler Track-Modus auf der CD!")},
  {E_TooManyFiles,            0, _T("Es sind zuviele Dateien im Quellverzeichnis!")},
  {E_TooManyDirs,             0, _T("Es wurden zuviele Verzeichnisse angegeben!")},
  {E_BadPathname,             0, _T("Der des Quellverzeichnisses ist ungültig!")},
  {E_MaxFileSort,             0, _T("Es sind zu viele Dateien/Verzeichnisse zu sortieren!")},
  {E_CDFSImageTooLong,        0, _T("Die ISO9660-Datei ist zu groß für den CD-Rohling!")},
  {E_BadFileLength,           0, _T("Die Datei-Länge ist nicht kompatibel zur Größe der CD-ROM Sektoren!")},
  {E_BadBootFile,             0, _T("Länge des Boot-Images ist zum Emulations-Typ nicht kompatibel!")},
  {E_NoCacheMemory,           0, _T("Der Cache hat nicht genügend Speicher zur Verfügung!")},
  {E_CreateFileCache,         0, _T("Unerwarteter Fehler während der Erstellung des File Cache!")},
  {E_CreateDiscCache,         0, _T("Unerwarteter Fehler während der Erstellung des Disc Cache!")},
  {E_ReadFileCache,           0, _T("Unerwarteter Fehler beim lesen des File Cache")},
  {E_ReadDiscCache,           0, _T("Unerwarteter Fehler beim lesen des Disc Cache")},
  {E_CacheTimeout,            0, _T("Der Cache von CDRWIN ist leer")},
  {E_CacheReadError,          0, _T("File/Disc Lesefehler!")},
  {E_CacheEndOfData,          0, _T("Nicht genügend Daten im Cache")},
  {E_VerifyData,              0, _T("Überprüfung der Daten von Sektor %lu bis %lu fehlgeschlagen!")},
  {E_ProgramAbort,            0, _T("Programm abgebrochen - Bitte wenden Sie sich an info@goldenhawk.de!")},
  {E_BadBlockLength,          0, _T("Fehlerkontrolle - Ungültige Blocklänge angegeben")},
  {E_BadDataType,             0, _T("Fehlerkontrolle - Ungültiger Datentyp angegeben")},
  {E_ReadVolDesc,             0, _T("Unerwarteter Fehler beim Lesen der Datenträgerbezeichnung")},
  {E_BadVolDesc,              0, _T("Ungültige Datenträgerbezeichnung")},
  {E_NoPrimaryDesc,           0, _T("Es konnte keine gültige Datenträgerbezeichnung gefunden werden")},
  {E_ImportDirectory,         0, _T("Unerwarteter Fehler während des Importierens der Verzeichnisstruktur")},
  {E_FilenameTooLong,         0, _T("Ein oder mehrere Dateinamen sind länger als 64 Zeichen!")},
  {E_ReadClientData,          0, _T("Unerwarteter Fehler während des Lesens vom Client.")},
  {E_BadPregapLength,         0, _T("Illegale Track- oder Pregap-Länge")},
  {E_CantFindAudioEnd,        0, _T("Das Ende des AUDIO Tracks konnt nicht localisiert werden")},
  {E_CantFindMode1End,        0, _T("Das Ende des MODE1 Daten Tracks konnte nicht lokalisiert werden")},
  {E_CantFindMode2End,        0, _T("Das Ende des MODE2 Daten Tracks konnte nicht lokalisiert werden")},

  {E_BadKeyMatch,             0, _T("Die Unlock Keys wurden fehlerhaft eingegeben")},
  {E_BadUnlockInfo,           0, _T("Die Unlock-Informationen wurden fehlerhaft eingegeben")},
  {E_BadAuthData,             0, _T("Die Authorisierungs-Datei cdrwin.dat ist defekt oder ungültig")},
  {E_ExpiredKey,              0, _T("Ihre Version von CDRWIN können Sie nicht (mehr) kostenlos aktualisieren. Updates gibt es auf www.goldenhawk.de.")},
  {E_VersionTooOld,           0, _T("Bitte installieren Sie eine aktuellere Version von http://www.goldenhawk.de/.")},
  {E_InitControlFile,         0, _T("Die Authorisierungs-Datei konnte nicht initialisiert werden.")},
  {E_UpdateControlFile,       0, _T("Die Authorisierungs-Datei konnte nicht aktualisiert werden!")},
  {E_SoftwareLocked,          0, _T("Die Software ist nicht freigeschaltet und auch keine Demoversion!")},
  {E_NoDiscAtOnce,            0, _T("Der gewählte CD-Recorder unterstützt kein Disc-At-Once!")},
  {E_NoTransporter,           0, _T("Es konnte kein Disc-Transporter gefunden werden!")},
  {E_EjectDiscFailed,         0, _T("Die CD konnte nicht ausgeworfen werden.")},
  {E_LoadDiscFailed,          0, _T("Es konnte keine CD geladen werden.")},
  {E_TransError,              0, _T("Disc Transporter Befehl fehlgeschlagen")},
  {E_TransDoorOpen,           0, _T("Die Klappe des Disc Transporters ist offen.")},
  {E_TransNoDisc,             0, _T("Das Magazin des Disc Transporters ist leer.")},
  {E_TransInvalid,            0, _T("Disc Transporter Befehl ist ungültig")},
  {E_TransBusy,               0, _T("Disc Transporter ist nicht bereit.")},
  {E_TransNoHolder,           0, _T("Das Magazin des Disc Transporters fehlt.")},
  {E_TransNoBulk,             0, _T("Die Spindel des Disc Transporters fehlt.")},
  {E_TransTimeout,            0, _T("Der Disc Transporter reagiert nicht (Time out?)")},
  {E_TransNoPort,             0, _T("Der Disc Transporter Communication Port meldet einen Fehler.")},

  {E_DiscNotSupported,        0, _T("Disc type is not supported")},
  {E_DiscNotWritable,         0, _T("Auf die eingelegt CD kann nicht (mehr) geschrieben werden!")},
  {E_DiscNotRewritable,       0, _T("Auf die eingelegt CD kann nicht (mehr) geschrieben werden!")},
  {E_SetBlockLength,          0, _T("Die Blocklänge konnte nicht eingestellt werden!")},
  {E_ReadDiscInfo,            0, _T("Die CD-Information konnte nicht gelesen werden!")},
  {E_ReadTrackInfo,           0, _T("Die Track-Informationen konnten nicht gelesen werden!")},
  {E_NextWritableAddress,     0, _T("Der nächste beschreibbare Adresse konnte nicht gefunden werden!")},
  {E_SetWriteMode,            0, _T("Der Schreib-Modus konnte nicht eingestellt werden!")},
  {E_SetTrackParameters,      0, _T("Die Spur konnte nicht eingestellt werden!")},
  {E_SetDiscParameters,       0, _T("Die CD-Parameter konnten nicht eingestellt werden!")},
  {E_SetRecordSpeed,          0, _T("Die Schreibgeschwindigkeit konnte nicht eingestellt werden!")},
  {E_FlushCache,              0, _T("Der Cache konnte nicht auf die CD geschrieben werden!")},
  {E_SetCatalogNumber,        0, _T("Die MCN (Media Catalog Nummer) konnte nicht eingestellt werden!")},
  {E_LaserCalibration,        0, _T("Der Laser konnte nicht kalibriert werden!")},
  {E_CueSheetRejected,        0, _T("Das Cuesheet ist vom CD-Recorder nicht akzeptiert worden!")},
  {E_BadBlockLength,          0, _T("Das Cuesheet beeinhaltet eine ungültige Blocklänge!")},
  {E_BadTwoPartPregap,        0, _T("Der Pregap besitzt eine ungültige Länge.")},
  {E_TooMuchText,             0, _T("Es wurden zuviel CD-TEXT Informationen eingegeben!")},
  {E_BadCDTextFile,           0, _T("Die CD-TEXT Datei ist ungültig oder defekt!")},
  {E_DifferentDisc,           0, _T("Disc is incompatible with discs in other recorders")},

  {E_BadCuesheetSyntax,       0, _T("Die Syntax im Cuesheet ist fehlerhaft")},
  {E_MultiplePerDisc,         0, _T("Befehl kann nur einmal pro CD ausgeführt werden")},
  {E_MultiplePerTrack,        0, _T("Befehl kann nur einmal pro Track ausgeführt werden")},
  {E_BadCommandPlacement,     0, _T("Illegaler Befehl angegeben")},
  {E_BadFiletype,             0, _T("Illegaler Dateityp spezifiziert")},
  {E_BadSizeMultiple,         0, _T("Datei ist passt nicht zur angegebenen Blockgröße")},
  {E_BadCatalogNumber,        0, _T("Die CATALOG-Nummer hat eine falsche Länge oder enthält illegale Zeichen")},
  {E_BadISRC,                 0, _T("Die ISRC-Nummer hat eine falsche Länge oder enthält illegale Zeichen")},
  {E_BadMSF,                  0, _T("Illegale MM:SS:FF Zeitangabe!")},
  {E_NoFileCommand,           0, _T("Es wurde kein FILE Befehl angegeben.")},
  {E_NoTrackCommand,          0, _T("Es wurde kein TRACK Befehl angegeben")},
  {E_BadTrackNumber,          0, _T("Illegale TRACK-Nummer angegeben")},
  {E_TracksNotSequential,     0, _T("TRACK-Nummern sind nicht in sequentieller Reihenfolge")},
  {E_BadIndexNumber,          0, _T("Illegale INDEX-Nummer angegeben!")},
  {E_IndexesNotSequential,    0, _T("INDEX-Nummern sind nicht in sequentieller Reihenfolge")},
  {E_IndexTimesNotAscending,  0, _T("INDEX-Zeiten sind nicht in aufsteigender Reihenfolge")},
  {E_IndexBeyondEOF,          0, _T("Eine INDEX-Angabe ist liegt außerhalb der Image-Files ")},
  {E_BadFirstIndex,           0, _T("Der erste INDEX muß 0 oder 1 sein!")},
  {E_BadFirstIndexMSF,        0, _T("Der erste INDEX startet nicht bei 00:00:00")},
  {E_NoTracksDefined,         0, _T("Es wurden keine TRACKS spezifiziert")},
  {E_NoIndexOne,              0, _T("Es wurde kein Start-INDEX für den vorhergehenden Track angegeben")},
  {E_TooManyCuepoints,        0, _T("Die maximale Anzahl von Indizes wurde überschritten")},
  {E_CantReadCDText,          0, _T("CD-TEXT-Datei konnte nicht gelesen werden")},
  {E_TooMuchCDText,           0, _T("CD-TEXT-Datei enthält zuviele Einträge")},
  {E_NoCuesheetTracks,        0, _T("Kein(e) Track(s) im Cuesheet angegeben!")},
  {E_NoTracksLastFile,        0, _T("Kein(e) Track(s) für die letzte Datei im Cuesheet angegeben")},
  {E_NoIndexesLastTrack,      0, _T("Kein Start-INDEX für den letzten Track im Cuesheet angegeben")}
  };
#else
EXCEPTMSG exceptmsg_table[] = {
  {E_Success,                 0, _T("Success!")},
  {E_Failure,                 0, _T("Failure!")},
  {E_UserAbort,               0, _T("User aborted operation!")},

  {E_AspiInit,                0, _T("Unable to initialize the ASPI manager")},
  {E_NoAspiManager,           0, _T("ASPI manager was not found on system")},
  {E_NoAspiProcedure,         0, _T("ASPI procedure entry point was not found")},
  {E_NoAspiSupportInfo,       0, _T("ASPI support information was not found")},
  {E_AspiAbort,               0, _T("ASPI request aborted (%02lXh %02lXh %02lXh)")},
  {E_AspiAbortFail,           0, _T("ASPI abort request failed")},
  {E_AspiInvalidRequest,      0, _T("Invalid ASPI request")},
  {E_AspiInvalidAdapter,      0, _T("Invalid host adapter number")},
  {E_AspiNoDevice,            0, _T("Device does not exist")},
  {E_AspiInvalidSRB,          0, _T("Invalid ASPI request block")},
  {E_AspiBufferAlign,         0, _T("Invalid buffer alignment")},
  {E_AspiIllegalMode,         0, _T("Illegal mode (Win32S operation not allowed)")},
  {E_AspiNoHelperDriver,      0, _T("ASPI helper driver was not found")},
  {E_AspiFailedInit,          0, _T("ASPI initialization failure")},
  {E_AspiBusy,                0, _T("ASPI manager is busy")},
  {E_AspiBufferTooBig,        0, _T("ASPI buffer is too big")},
  {E_AspiMismatchedComp,      0, _T("Mismatched ASPI components")},
  {E_AspiNoAdapters,          0, _T("No ASPI host adapters on system")},
  {E_AspiNoResources,         0, _T("Insufficient resources to initialize ASPI manager")},
  {E_AspiUnknown,             0, _T("Unexpected ASPI error (%02lXh %02lXh %02lXh)")},
  {E_AspiTimeout,             0, _T("ASPI request timed out")},
  {E_AspiRequestFailed,       0, _T("ASPI request failed to complete successfully")},
  {E_AspiSendRequestFailed,   0, _T("ASPI request failed to queue successfully")},

  {E_AdapterTimeout,          0, _T("Transaction timed out")},
  {E_AdapterExpired,          0, _T("ASPI request expired")},
  {E_AdapterMessageReject,    0, _T("Message reject received")},
  {E_AdapterBusReset,         0, _T("Bus reset detected")},
  {E_AdapterParityError,      0, _T("Parity error detected")},
  {E_AdapterSenseFailed,      0, _T("SENSE request failed")},
  {E_AdapterSelectTimeout,    0, _T("Device selection timed out")},
  {E_AdapterDataOverrun,      0, _T("Data overrun/underrun detected")},
  {E_AdapterBusFree,          0, _T("Bus free detected")},
  {E_AdapterBusFailure,       0, _T("Bus failure detected")},
  {E_AdapterUnknown,          0, _T("Unexpected ADAPTER error (%02lXh %02lXh %02lXh)")},

  {E_TargetBusy,              0, _T("Target is busy")},
  {E_TargetConflict,          0, _T("Target reservation conflict")},
  {E_TargetUnknown,           0, _T("Unexpected TARGET error (%02lXh %02lXh %02lXh)")},

  {E_Recovered,               0, _T("I/O completed with recovery")},
  {E_DeviceNotReady,          0, _T("Device contains no disc or is not ready (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_BadMedium,               0, _T("Read/Write error or bad medium detected (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_Hardware,                0, _T("Unrecoverable hardware error (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_IllegalRequest,          0, _T("Illegal command request (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_UnitAttention,           0, _T("Unit attention (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_BlankCheck,              0, _T("Blank medium detected (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_DeviceBusy,              0, _T("Device is busy (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_TargetAbort,             0, _T("Device aborted command (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_VolumeOverflow,          0, _T("Volume overflow (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_EndOfMedium,             0, _T("End of medium detected (%02lXh %02lXh %02lXh %02lXh %02lXh %02lXh)")},
  {E_SenseUnknown,            0, _T("Unexpected SENSE error")},

  {E_Generic,                 1, _T("%s")},
  {E_BugCheck,                0, _T("Internal bugcheck detected")},
  {E_NoMemory,                0, _T("Memory allocation request failed")},
  {E_NotImplemented,          0, _T("Feature is not implemented yet")},
  {E_CommandFailed,           4, _T("'%s' command failed on device %lu:%lu:%lu")},
  {E_ReadAudio,               5, _T("Unable to read AUDIO sectors %lu to %lu on device %lu:%lu:%lu")},
  {E_ReadData,                5, _T("Unable to read DATA sectors %lu to %lu on device %lu:%lu:%lu")},
  {E_OpenFile,                1, _T("Unable to open file \"%s\"")},
  {E_CreateFile,              1, _T("Unable to create file \"%s\"")},
  {E_ReadFile,                0, _T("Unexpected failure reading file (end-of-file?)")},
  {E_FileNotFound,            0, _T("Unable to find or open file")},
  {E_FileIsEmpty,             0, _T("File contains no data")},
  {E_WriteFile,               0, _T("Unexpected failure writing file (disk full?)")},
  {E_EndOfFile,               1, _T("Unexpected end-of-file reading \"%s\"")},
  {E_FindLastSector,          0, _T("Unable to find last readable sector")},
  {E_ParseCueSheet,           1, _T("Invalid CUE SHEET command at line %lu")},
  {E_BadWaveFile,             0, _T("WAVE file is invalid or not CDROM compatible")},
  {E_BadAiffFile,             0, _T("AIFF file is invalid or not CDROM compatible")},
  {E_BadMP3File,              0, _T("MP3 file is invalid or not CDROM compatible")},
  {E_JitterFailure,           0, _T("Unexpected jitter correction failure")},
  {E_AnalyzeDisc,             0, _T("Unable to analyze disc layout")},
  {E_NoMultisession,          0, _T("Discs containing more than one session are not supported")},
  {E_BadTrackType,            0, _T("Illegal track type encountered")},
  {E_TooManyFiles,            0, _T("Too many files in directory")},
  {E_TooManyDirs,             0, _T("Too many directories to process")},
  {E_BadPathname,             0, _T("Pathname \"%s\" is invalid or does not exist")},
  {E_MaxFileSort,             0, _T("Too many files/directories to sort")},
  {E_CDFSImageTooLong,        0, _T("CDFS image is too large to fit on disc")},
  {E_BadFileLength,           0, _T("File length is not a multiple of the CDROM sector size")},
  {E_BadBootFile,             0, _T("Boot image file length is not compatible with emulation type")},
  {E_NoCacheMemory,           0, _T("Not enough memory available for cache")},
  {E_CreateFileCache,         0, _T("Unexpected failure creating file cache")},
  {E_CreateDiscCache,         0, _T("Unexpected failure creating disc cache")},
  {E_ReadFileCache,           0, _T("Unexpected failure reading file cache")},
  {E_ReadDiscCache,           0, _T("Unexpected failure reading disc cache")},
  {E_CacheTimeout,            0, _T("Request timeout encountered (cache is empty)")},
  {E_CacheReadError,          0, _T("File/Disc read error encountered")},
  {E_CacheEndOfData,          0, _T("End-of-data encountered (not enough data in cache)")},
  {E_VerifyData,              0, _T("Data verification failure at sectors %lu to %lu")},
  {E_ProgramAbort,            0, _T("Program aborted - Internal error detected")},
  {E_BadBlockLength,          0, _T("Bugcheck - Invalid block length specified")},
  {E_BadDataType,             0, _T("Bugcheck - Invalid datatype specified")},
  {E_ReadVolDesc,             0, _T("Unexpected error reading volume descriptor")},
  {E_BadVolDesc,              0, _T("Illegal volume descriptor encountered")},
  {E_NoPrimaryDesc,           0, _T("No primary volume descriptor was found")},
  {E_ImportDirectory,         0, _T("Unexpected error importing directory structure")},
  {E_FilenameTooLong,         0, _T("Filename is too long for ISO9660 directory record...")},
  {E_ReadClientData,          0, _T("Unexpected failure reading data from client")},
  {E_BadPregapLength,         0, _T("Illegal track or pregap length")},
  {E_CantFindAudioEnd,        0, _T("Unable to locate end of AUDIO track")},
  {E_CantFindMode1End,        0, _T("Unable to locate end of MODE1 data track")},
  {E_CantFindMode2End,        0, _T("Unable to locate end of MODE2 data track")},
  {E_ImageFileTooBig,         0, _T("Image files larger than 4 gigabytes are not supported!")},

  {E_BadKeyMatch,             0, _T("Unlock keys have been entered incorrectly")},
  {E_BadUnlockInfo,           0, _T("Unlock information has been entered incorrectly")},
  {E_BadAuthData,             0, _T("Authorization file data is corrupt/invalid")},
  {E_ExpiredKey,              0, _T("Your unlock keys have expired (more than 18 months old)")},
  {E_VersionTooOld,           0, _T("Software version is out of date (please upgrade)")},
  {E_InitControlFile,         0, _T("Unable to initialize authorization file")},
  {E_UpdateControlFile,       0, _T("Unable to update authorization file")},
  {E_SoftwareLocked,          0, _T("Software is locked and has no demo mode!")},
  {E_NoDiscAtOnce,            0, _T("Recorder does not support disc-at-once recording")},
  {E_NoTransporter,           0, _T("No disc transporter device was found on SCSI bus")},
  {E_EjectDiscFailed,         0, _T("Eject disc command failed")},
  {E_LoadDiscFailed,          0, _T("Load disc command failed")},
  {E_TransError,              0, _T("Disc transporter command failed")},
  {E_TransDoorOpen,           0, _T("Disc transporter door is open")},
  {E_TransNoDisc,             0, _T("Disc transporter input holder is empty")},
  {E_TransInvalid,            0, _T("Disc transporter command is invalid")},
  {E_TransBusy,               0, _T("Disc transporter device is busy")},
  {E_TransNoHolder,           0, _T("Disc transporter input holder is missing")},
  {E_TransNoBulk,             0, _T("Disc transporter output spindle is missing")},
  {E_TransTimeout,            0, _T("Disc transporter device is not responding")},
  {E_TransNoPort,             0, _T("Unable to open disc transporter communications port")},

  {E_DiscNotSupported,        0, _T("Disc type is not supported")},
  {E_DiscNotWritable,         0, _T("Disc is not writable")},
  {E_DiscNotRewritable,       0, _T("Disc is not re-writable")},
  {E_SetBlockLength,          0, _T("Unable to set block length")},
  {E_ReadDiscInfo,            0, _T("Unable to read disc information")},
  {E_ReadTrackInfo,           0, _T("Unable to read track information")},
  {E_NextWritableAddress,     0, _T("Unable to determine next writable address")},
  {E_SetWriteMode,            0, _T("Unable to set writing mode")},
  {E_SetTrackParameters,      0, _T("Unable to set track parameters")},
  {E_SetDiscParameters,       0, _T("Unable to set disc parameters")},
  {E_SetRecordSpeed,          0, _T("Unable to set recording speed")},
  {E_FlushCache,              0, _T("Unable to flush cache to disc")},
  {E_SetCatalogNumber,        0, _T("Unable to set media catalog number")},
  {E_LaserCalibration,        0, _T("Unable to calibrate recording laser")},
  {E_CueSheetRejected,        0, _T("Cuesheet was rejected by the CDROM recorder")},
  {E_BadBlockLength,          0, _T("Cuesheet contains an illegal block length")},
  {E_BadTwoPartPregap,        0, _T("Illegal two-part PREGAP length specified (too small)")},
  {E_TooMuchText,             0, _T("Too much CD-TEXT information has been specified")},
  {E_BadCDTextFile,           0, _T("CD-TEXT file is invalid or corrupt")},
  {E_DifferentDisc,           0, _T("Disc is incompatible with discs in other recorders")},
  {E_ReserveTrack,            0, _T("Unable to reserve track")},

  {E_BadCuesheetSyntax,       0, _T("Illegal cuesheet command syntax")},
  {E_MultiplePerDisc,         0, _T("Command can not be specified more than once per disc")},
  {E_MultiplePerTrack,        0, _T("Command can not be specified more than once per track")},
  {E_BadCommandPlacement,     0, _T("Illegal command placement")},
  {E_BadFiletype,             0, _T("Illegal filetype specified")},
  {E_BadSizeMultiple,         0, _T("Previous file is not a multiple of the specified block size")},
  {E_BadCatalogNumber,        0, _T("CATALOG number is wrong length or contains illegal characters")},
  {E_BadISRC,                 0, _T("ISRC is wrong length or contains illegal characters")},
  {E_BadMSF,                  0, _T("Illegal MM:SS:FF time value specified!")},
  {E_NoFileCommand,           0, _T("No FILE command has been specified yet")},
  {E_NoTrackCommand,          0, _T("No TRACK command has been specified yet")},
  {E_BadTrackNumber,          0, _T("Illegal TRACK number specified")},
  {E_TracksNotSequential,     0, _T("TRACK numbers are not in sequential order")},
  {E_BadIndexNumber,          0, _T("Illegal INDEX number specified")},
  {E_IndexesNotSequential,    0, _T("INDEX numbers are not in sequential order")},
  {E_IndexTimesNotAscending,  0, _T("INDEX times are not in ascending order")},
  {E_IndexBeyondEOF,          0, _T("INDEX time is beyond the end-of-file")},
  {E_BadFirstIndex,           0, _T("First INDEX must be 0 or 1")},
  {E_BadFirstIndexMSF,        0, _T("First INDEX of file does not start at 00:00:00")},
  {E_NoTracksDefined,         0, _T("No TRACKS were defined for the previous file")},
  {E_NoIndexOne,              0, _T("No starting INDEX was defined for the previous track")},
  {E_TooManyCuepoints,        0, _T("The maximum number of cuepoints has been exceeded")},
  {E_CantReadCDText,          0, _T("Unable to read CD-TEXT file")},
  {E_TooMuchCDText,           0, _T("CD-TEXT file contains too many entries")},
  {E_NoCuesheetTracks,        0, _T("No tracks were defined in the cuesheet")},
  {E_NoTracksLastFile,        0, _T("No tracks were defined for the last file in the cuesheet")},
  {E_NoIndexesLastTrack,      0, _T("No starting INDEX was defined for the last track in the cuesheet")},

  {E_ISO9660BugCheck,         0, _T("ISO9660 Bugcheck (Blocks Expected = %u  Blocks Written = %u")}
  };
#endif

//
// Signal an exception
//

void SignalException (
  SLONG code, ULONG count, SLONG subcode, ULONG subcount, ...)
{
  ULONG i;
  va_list ap;

  MSGVEC *msgvecP = new MSGVEC;

  // Initialize the exception codes and argument counts.

  MEMCLEAR (msgvecP, sizeof(MSGVEC));

  msgvecP->code = code;
  msgvecP->count = count;

  msgvecP->subcode = subcode;
  msgvecP->subcount = subcount;

  // Initialize the arguments.

  va_start (ap, subcount);

  for (i = 0; i < count; i++)
    msgvecP->param[i] = va_arg(ap, ULONG);

  for (i = 0; i < subcount; i++)
    msgvecP->subparam[i] = va_arg(ap, ULONG);

  va_end (ap);

  // Throw the exception.

  throw (msgvecP);
}

//
// Print an exception
//

void ConsolePrintException(MSGVEC *msgvecP)
{
  TCHAR format[80];
  TCHAR *msgtextP;
    
  // Print the primary exception message.

  msgtextP = GetMessageText(msgvecP->code);

  StrPrintf (format, msgtextP,
    msgvecP->param[0], msgvecP->param[1], msgvecP->param[2],
    msgvecP->param[3], msgvecP->param[4], msgvecP->param[5],
    msgvecP->param[6], msgvecP->param[7]);

  FPrintf (stderr, _T("\nError: %s\n"), format);

  // Print the (optional) secondary exception message.

  if (msgvecP->subcode != E_Success)
    {
    msgtextP = GetMessageText(msgvecP->subcode);

    StrPrintf (format, msgtextP,
      msgvecP->subparam[0], msgvecP->subparam[1], msgvecP->subparam[2],
      msgvecP->subparam[3], msgvecP->subparam[4], msgvecP->subparam[5],
      msgvecP->subparam[6], msgvecP->subparam[7]);

    FPrintf (stderr, _T("%s\n"), format);
    }
}

//
// Print a warning message
//

void ConsolePrintWarning(MSGVEC *msgvecP)
{
  TCHAR format[80];
  TCHAR *msgtextP;

  // Print the primary warning message.

  msgtextP = GetMessageText(msgvecP->code);

  StrPrintf (format, msgtextP,
    msgvecP->param[0], msgvecP->param[1], msgvecP->param[2],
    msgvecP->param[3], msgvecP->param[4], msgvecP->param[5],
    msgvecP->param[6], msgvecP->param[7]);

  FPrintf (stderr, _T("\nWarning: %s\n"), format);

  // Print the (optional) secondary warning message.

  if (msgvecP->subcode)
    {
    msgtextP = GetMessageText(msgvecP->subcode);

    StrPrintf (format, msgtextP,
      msgvecP->subparam[0], msgvecP->subparam[1], msgvecP->subparam[2],
      msgvecP->subparam[3], msgvecP->subparam[4], msgvecP->subparam[5],
      msgvecP->subparam[6], msgvecP->subparam[7]);

    FPrintf (stderr, _T("%s\n"), format);
    }
}

//
// Get a pointer to an exception message block. 
//

EXCEPTMSG *GetMessagePtr(SLONG code)
{
  extern EXCEPTMSG exceptmsg_table[];

  for (int i = 0; i < 200; i++)
    {
    EXCEPTMSG *msgP = &exceptmsg_table[i];
    if (msgP->code == code) return msgP;
    }
  
  return NULL;    // shouldn't get here
}

//
// Get the text of an exception message.
//

TCHAR *GetMessageText(SLONG code)
{
  EXCEPTMSG *msgP = GetMessagePtr(code);
  return ((msgP == NULL) ? _T("") : msgP->text);
}

// Restore previous packing alignment.

#pragma pack(pop)
