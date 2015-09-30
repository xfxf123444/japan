// 13-sep-04 (sjk) UNICODE/MBCS image and cdtext filenames.

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

#include <conio.h>

#include "cdrlib.h"
#include "prodver.h"
#include "protect.h"

// External variables.

extern unsigned _stklen = 16384;

// Protection information.

extern CINFO gControlFileInfo;    // Control file information.
extern BOOL gbAppUnlocked;        // Application unlocked?

// Command line parameters.

static char *base_filnamP = NULL;

static UBYTE cdreader_adapter, cdreader_id, cdreader_lun;
static BOOL cdreader_id_specified = FALSE;

static UWORD read_speed = SPEED_MAX;

static UBYTE jitter_mode = JITTER_MODE_AUTO;
static UBYTE error_mode = READERROR_MODE_ABORT;
static UBYTE scan_subcode_mode = SCAN_SUBCODE_AUTO;

static BOOL beep_flag = FALSE;
static BOOL cdg_flag = FALSE;
static BOOL cdtext_flag = FALSE;
static BOOL confirm_flag = TRUE;
static BOOL cooked_flag = FALSE;
static BOOL cueonly_flag = FALSE;
static BOOL log_flag = TRUE;
static BOOL mcn_isrc_flag = FALSE;

static UBYTE speedtable[] = {
  SPEED_1X,  SPEED_2X,  SPEED_2X,  SPEED_4X,  SPEED_4X,  SPEED_6X, SPEED_6X,
  SPEED_8X,  SPEED_8X,  SPEED_10X, SPEED_10X, SPEED_12X, SPEED_12X,
  SPEED_14X, SPEED_14X, SPEED_16X, SPEED_16X, SPEED_18X, SPEED_18X,
  SPEED_20X, SPEED_20X, SPEED_20X, SPEED_20X, SPEED_24X, SPEED_24X,
  SPEED_24X, SPEED_24X, SPEED_24X, SPEED_24X, SPEED_24X, SPEED_24X,
  SPEED_32X, SPEED_32X, SPEED_32X, SPEED_32X, SPEED_32X, SPEED_32X,
  SPEED_32X, SPEED_32X, SPEED_40X, SPEED_40X, SPEED_40X, SPEED_40X,
  SPEED_40X, SPEED_40X, SPEED_40X, SPEED_40X, SPEED_48X
  };

// Function prototypes.

void PrintUsage(void);
void ParseCommandLine(int argc, char *argv[]);

//
// Print command line usage
//

void PrintUsage()
{
  printf ("SNAPSHOT.EXE - Version %s (%s)\n", PRODUCT_VERSION, PRODUCT_RELEASE_DATE);
  printf ("%s\n\n", COPYRIGHT_MESSAGE);
  printf ("Usage: SNAPSHOT <filename> [/ID=ha:id:lun] [/BATCH] [/BEEP] [/CDG]\n");
  printf ("         [/CDTEXT] [/COOKED] [/CUEONLY] [/IGNORE] [/ISRC] [/JC] [/NOJC]\n");
  printf ("         [/NOCONFIRM] [/SPEED=n] [/SUBCODE=option] [/THRESH]\n");

  printf ("<hit any key to continue>\r");
  getch();

  printf ("filename   - Output filename (used to name image/cuesheet/cdtext files)\n");
  printf ("/ID        - ASPI ID of CD reader (default is first found)\n");
  printf ("/BATCH     - Disable all messages and confirmation prompts\n");
  printf ("/BEEP      - Issue an audible 'beep' when copying has completed\n");
  printf ("/CDG       - Enable reading of CD+G discs\n");
  printf ("/CDTEXT    - Enable reading of CD-TEXT discs\n");
  printf ("/COOKED    - Read cooked data (default is raw sectors)\n");
  printf ("/CUEONLY   - Generate cuesheet file only\n");
  printf ("/IGNORE    - Ignore all reading errors\n");
  printf ("/ISRC      - Read the track ISRCs and disc MCN\n");
  printf ("/JC        - Enable audio 'jitter correction' (default is automatic)\n");
  printf ("/NOJC      - Disable audio 'jitter correction' (default is automatic)\n");
  printf ("/NOCONFIRM - Disable all confirmation prompts\n");
  printf ("/SPEED     - Audio reading speed (default is maximum)\n");
  printf ("/SUBCODE   - Audio subcode scanning option...\n");
  printf ("               AUTO    - Automatically select best mode for device (default)\n");
  printf ("               DISABLE - Disable subcode scan\n");
  printf ("               QUICK   - Scan subcode for pregaps only\n");
  printf ("               FULL    - Scan subcode for all subindexes\n");
  printf ("               FIXED   - Encode a two second pregap on all audio tracks\n");
}

//
// Parse the command line
//

void ParseCommandLine(int argc, char *argv[])
{
  extern BOOL env_cdreader_flag;
  extern UBYTE env_cdreader_adapter;
  extern UBYTE env_cdreader_id;
  extern UBYTE env_cdreader_lun;

  UWORD adapter, id, lun;
  char *argP, uparg[80];

  // Print usage?

  if (argc == 1) {
    PrintUsage ();
    exit (0);
    }

  // Load the environment variables.

  if (env_cdreader_flag)
    {
    cdreader_id_specified = TRUE;
    cdreader_adapter = env_cdreader_adapter;
    cdreader_id = env_cdreader_id;
    cdreader_lun = env_cdreader_lun;
    }

  // Parse each command line argument.

  for (int i = 1; i < argc; i++)
    {
    argP = argv[i];

    StringCopy (uparg, argP);
    strupr (uparg);

    // Is this a switch?

    if (uparg[0] == '/')
      {
      if (! strcmp (uparg, "/?")) {
        PrintUsage ();
        exit (0);
        }

      else if (! strcmp (uparg, "/BATCH")) {
        log_flag = FALSE;
        confirm_flag = FALSE;
        }

      else if (! strcmp (uparg, "/BEEP"))
        beep_flag = TRUE;

      else if (! strcmp (uparg, "/CDG"))
        cdg_flag = TRUE;

      else if (! strcmp (uparg, "/CDTEXT"))
        cdtext_flag = TRUE;

      else if (! strcmp (uparg, "/COOKED"))
        cooked_flag = TRUE;

      else if (! strcmp (uparg, "/CUEONLY"))
        cueonly_flag = TRUE;

      else if (! strcmp (uparg, "/IGNORE"))
        error_mode = READERROR_MODE_REPLACE;

      else if (! strcmp (uparg, "/ISRC"))
        mcn_isrc_flag = TRUE;

      else if (! strcmp (uparg, "/JC"))
        jitter_mode = JITTER_MODE_ENABLE;

      else if (! strcmp (uparg, "/NOJC"))
        jitter_mode = JITTER_MODE_DISABLE;

      else if (! strcmp (uparg, "/NOCONFIRM"))
        confirm_flag = FALSE;

      else if (! strncmp (uparg, "/SUBCODE", 8))
        {
        char subcode_str[80];

        if (sscanf (uparg, "/SUBCODE=%s", &subcode_str) != 1) {
          fprintf (stderr, "Error: Illegal /SUBCODE format!\n");
          exit (1);
          }

        if (! stricmp(subcode_str, "AUTO"))
          scan_subcode_mode = SCAN_SUBCODE_AUTO;
        else if (! stricmp(subcode_str, "DISABLE"))
          scan_subcode_mode = SCAN_SUBCODE_DISABLE;
        else if (! stricmp(subcode_str, "QUICK"))
          scan_subcode_mode = SCAN_SUBCODE_QUICK;
        else if (! stricmp(subcode_str, "FULL"))
          scan_subcode_mode = SCAN_SUBCODE_FULL;
        else if (! stricmp(subcode_str, "FIXED"))
          scan_subcode_mode = SCAN_SUBCODE_FIXED;
        else
          {
          fprintf (stderr, "Illegal /SUBCODE option \"%s\" specified!\n",
            subcode_str);
          exit (1);
          }
        }

      else if (! strncmp (uparg, "/SPEED", 6))
        {
        UWORD speed;

        if (sscanf (uparg, "/SPEED=%hu", &speed) != 1) {
          fprintf (stderr, "Error: Illegal /SPEED format!\n");
          exit (1);
          }

        if ((speed < 1) || (speed > 48)) {
          fprintf (stderr, "Error: Illegal reading speed specified!\n");
          exit (1);
          }

        read_speed = speedtable[speed - 1]; // lookup the actual speed constant.
        }

      else if (! strncmp (uparg, "/ID", 3))
        {
        if (sscanf (uparg, "/ID=%hu:%hu:%hu", &adapter, &id, &lun) == 3)
          {
          cdreader_adapter = adapter;
          cdreader_id = id;
          cdreader_lun = lun;
          cdreader_id_specified = TRUE;
          }
        else {
          fprintf (stderr, "Error: Illegal /ID format!\n");
          exit (1);
          }
        }

      else {
        fprintf (stderr, "Invalid switch - %s\n", argP);
        exit (1);
        }
      }

    // otherwise, this is probably the filename parameter...

    else
      {
      if (base_filnamP == NULL)
        base_filnamP = argP;
      else {
        fprintf (stderr, "Too many parameters - %s\n", argP);
        exit (1);
        }
      }
    }

  // Make sure that the required parameters were specified.

  if (base_filnamP == NULL) {
    fprintf (stderr, "Error: filename parameter must be specified!\n");
    exit (1);
    }
}

//
// Main entry point
//

main(int argc, char *argv[])
{
  CDReader *cdreaderP;
  FILEHANDLE image_file;

  UWORD audio_blklen, mode1_blklen, mode2_blklen;

  TCHAR image_filnam[_MAX_PATH + 1];
  TCHAR cue_filnam[_MAX_PATH + 1];
  TCHAR cdtext_filnam[_MAX_PATH + 1];

  TCHAR *image_filnamP = NULL, *cue_filnamP = NULL, *cdtext_filnamP = NULL;

  // Enable exception handling.

  EXCEPTION_HANDLER_START

  // Initialize and check the protection.

  ProtInitialize (NULL, FALSE);

  // Get the environment variables.

  GetEnvironmentVariables();

  // Parse the command line arguments.

  ParseCommandLine (argc, argv);

  // Register the event callback function.

  EventRegisterCallback (ConsoleEventCallback);

  // Startup the ASPI manager.

  ASPIAdapter::StartupManager (FALSE, FALSE, TRUE);

  // Find a CDROM device...

  if (cdreader_id_specified)
    {
    if ((cdreaderP = (CDReader *)ASPIAdapter::FindDeviceObject (
          ASPI_M_DEVTYPE_CDROM | ASPI_M_DEVTYPE_WORM,
          cdreader_adapter, cdreader_id, cdreader_lun)) == NULL)
      {
      fprintf (stderr,
        "\nError: Specified device (%u:%u:%u) is not a CDROM!\n",
        cdreader_adapter, cdreader_id, cdreader_lun);
      exit (1);
      }
    }
  else
    {
    if ((cdreaderP = (CDReader *)ASPIAdapter::FindDeviceObject (
          ASPI_M_DEVTYPE_CDROM | ASPI_M_DEVTYPE_WORM)) == NULL) {
      fprintf (stderr,
        "\nError: Unable to find a known CDROM device!\n");
      exit (1);
      }
    }

  if (log_flag)
    {
    printf ("CDROM device found...\n");
    printf ("  HA #%u - ASPI ID #%u - %-8s %-16s %-4s\n\n",
      cdreaderP->GetAdapter(), cdreaderP->GetId(),
      cdreaderP->GetVendorId(), cdreaderP->GetProductId(),
      cdreaderP->GetFirmwareLevel());
    }

  if (cdreaderP->GetModel() == GENERIC_CDROM_SCSI) {
    fprintf (stderr, "Warning: CDROM device model is unknown to this program!\n");
    fprintf (stderr, "The generic CDROM device driver will be used...\n\n");
    }

  // If necessary, check that this device supports the reading of SUBCODEs.

  if ((scan_subcode_mode == SCAN_SUBCODE_QUICK) ||
      (scan_subcode_mode == SCAN_SUBCODE_FULL))
    {
    if (! cdreaderP->IsFlagSet(CDReader::crfReadSUBQ))
      {
      fprintf (stderr,
        "Error: CDROM device does not support the reading of subcodes!\n");
      exit (1);
      }
    }

  // If necessary, check that this device supports the reading of CD+G discs.

  if (cdg_flag && (! cdreaderP->IsFlagSet(CDReader::crfReadCDG))) {
    fprintf (stderr,
      "Error: CDROM device does not support the reading of CD+G discs!\n");
    exit (1);
    }

  // Analyze the disc layout.

  auto_ptr<DiscLayout> disclayoutP(new DiscLayout (
    cdreaderP, read_speed, scan_subcode_mode,
    mcn_isrc_flag, cdtext_flag, log_flag));

  if (log_flag)
    {
    printf ("\nDisc Statistics:\n");
    printf ("  Audio track count      - %u\n", disclayoutP->m_nAudioTrackCount);
    printf ("  Mode1 data track count - %u\n", disclayoutP->m_nMode1TrackCount);
    printf ("  Mode2 data track count - %u\n", disclayoutP->m_nMode2TrackCount);
    }

  // Is the disc too long?

  if (disclayoutP->m_nLastLBA > CDROM_80MIN_BLKCNT)
    fprintf (stderr, "\nWARNING: Disc is longer than 80 minutes!\n");

  // Check the disc layout.

  if (! disclayoutP->Check())
    {
    fprintf (stderr, "\nWARNING: Disc contains one or more tracks that are shorter than \n");
    fprintf (stderr, "four seconds. This is a violation of the CDROM specification.\n");
    }

  // Determine the block lengths.

  audio_blklen = (cdg_flag ? SECTOR_CDDA_SUBPW_BLKLEN : SECTOR_CDDA_BLKLEN);

  if (cooked_flag)
    {mode1_blklen = SECTOR_MODE1_BLKLEN; mode2_blklen = SECTOR_MODE2_BLKLEN;}
  else
    {mode1_blklen = SECTOR_RAW_BLKLEN; mode2_blklen = SECTOR_RAW_BLKLEN;}

  // Set the block length for each track.

  DiscInfo *discinfoP = disclayoutP->m_pDiscInfo;

  for (int i = 0; i < discinfoP->m_nTrackCount; i++)
    {
    TRACKINFO *trackinfoP = &discinfoP->m_TrackInfo[i];

    switch (trackinfoP->datatype)
      {
      case DATATYPE_CDDA:
        trackinfoP->blklen = audio_blklen; break;
      case DATATYPE_CDROM:
        trackinfoP->blklen = mode1_blklen; break;
      case DATATYPE_CDROMXA:
      case DATATYPE_CDI:
        trackinfoP->blklen = mode2_blklen; break;
      }
    }
  
  // Build the cuesheet filename.

  StringCopy (cue_filnam, base_filnamP);
  FileAddExtension (cue_filnam, ".cue", (! cueonly_flag));
  cue_filnamP = cue_filnam;

  // If we're not just writing a cuesheet, then build the image and CD-TEXT filenames.

  if (! cueonly_flag)
    {
    StringCopy (image_filnam, base_filnamP);
    FileAddExtension (image_filnam, ".bin", FALSE);
    image_filnamP = image_filnam;

    if (cdtext_flag && (discinfoP->m_pCDTextPackVec != NULL))
      {
      StringCopy (cdtext_filnam, base_filnamP);
      FileAddExtension (cdtext_filnam, ".cdt", TRUE);
      cdtext_filnamP = cdtext_filnam;
      }

    // Display image file stats?

    if (log_flag)
      {
      // Compute the amount of disc space required.
      ULONG blkcnt = disclayoutP->m_nLastLBA + 1;

      printf ("\nImage file will require approximately %luMb of disk space.\n",
        CDIV (blkcnt * SECTOR_RAW_BLKLEN, 0x100000));
      }

    // Prompt to begin copy?

    if (confirm_flag)
      {
      printf ("\nHit <ENTER> to copy disc (or CTRL/C to exit)...");
      getchar ();
      printf ("\n");
      }
    }

  // Generate the CUE SHEET file.

  disclayoutP->GenerateCuesheetFile (cue_filnamP, image_filnamP, cdtext_filnamP);

  // Generate the CD-TEXT data file?

  if (cdtext_filnamP != NULL)
    {
    FILEHANDLE handle;
    ULONG null = 0;

    if ((handle = FileCreate (cdtext_filnamP)) == NULL)
      SignalException (E_CreateFile, 1, 0, 0, HeapString(cdtext_filnamP));

    FileWrite (
      handle, discinfoP->m_pCDTextPackVec, discinfoP->m_nCDTextPackCount * sizeof(CDTEXTPACK));

    FileWrite (handle, &null, 1);

    FileClose (handle);
    }

  // Generate the image file?

  if (image_filnamP != NULL)
    {
    // Make sure the device is ready.

    cdreaderP->LoadDisc (TRUE);

    // Create the output image file.

    if ((image_file = FileCreate (image_filnamP)) == NULL) {
      fprintf (stderr, "\nError creating image file \"%s\"\n", image_filnamP);
      exit (1);
      }

    // Loop through the "copy instructions"...

    for (int i = 0; i < disclayoutP->m_nCopyInstrCount; i++)
      {
      COPYINSTR *copyinstrP = &disclayoutP->m_CopyInstr[i];

      // Set the block length.

      UWORD blklen = ((copyinstrP->datatype == DATATYPE_CDDA) ? audio_blklen :
        ((copyinstrP->datatype == DATATYPE_CDROM) ? mode1_blklen : mode2_blklen));

      // Select operation...

      switch (copyinstrP->opcode)
        {
        // Read a range of audio/data sectors.

        case CI_READ_SECTORS:
          {
          if (copyinstrP->datatype == DATATYPE_CDDA)
            {
            // Set the reading speed.
            cdreaderP->SetSpindleSpeed (read_speed, FALSE, FALSE);

            // Extract the audio sectors.
            cdreaderP->ExtractSectorsToFile (
              image_file, DATATYPE_CDDA, blklen, copyinstrP->lba, copyinstrP->blkcnt,
              jitter_mode, 0, error_mode, FALSE, log_flag);
            }
          else
            {
            // Set the reading speed to maximum.
            cdreaderP->SetSpindleSpeed (SPEED_MAX, TRUE, FALSE);

            // Extract the data sectors.
            cdreaderP->ExtractSectorsToFile (
              image_file, copyinstrP->datatype, blklen, copyinstrP->lba, copyinstrP->blkcnt,
              JITTER_MODE_DISABLE, 0, error_mode, FALSE, log_flag);
            }

          break;
          }

        // Generate replacements for unreadable blocks at the end of a track.

        case CI_GENERATE_SECTORS:
          {
          WriteEmptySectors (
            image_file, copyinstrP->datatype, blklen, copyinstrP->lba, copyinstrP->blkcnt);
          break;
          }

        // Ignore PREGAP instructions.

        case CI_GENERATE_PREGAP:
        case CI_GENERATE_PREGAP2:
          break;

        default: SignalException (E_BugCheck);
        }
      }

    // Reset the reading speed.

    cdreaderP->SetSpindleSpeed (SPEED_MAX, FALSE, FALSE);

    // Close the image file.

    FileClose (image_file);

    // Success!

    if (log_flag) printf ("\nCopy completed successfully!\n");

    // Notify user of completion?

    if (beep_flag) BeepUser();
    }

  // Shutdown the ASPI manager.

  ASPIAdapter::ShutdownManager();

  // End exception handling.

  EXCEPTION_HANDLER_EXIT

  return (0);
}

