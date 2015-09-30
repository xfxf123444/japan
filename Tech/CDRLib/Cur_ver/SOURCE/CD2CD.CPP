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

#include "cdrlib.h"
#include "prodver.h"

// External variables.

extern unsigned _stklen = 8192;

extern UBYTE ActualSpeedTable[];

// Function prototypes.

void PrintUsage(void);
void ParseCommandLine(int argc, char *argv[]);

// Command line parameters.

static UBYTE cdreader_adapter, cdreader_id, cdreader_lun;
static BOOL cdreader_id_specified = FALSE;

static UBYTE cdwriter_adapter, cdwriter_id, cdwriter_lun;
static BOOL cdwriter_id_specified = FALSE;

static UWORD copy_count = 1;
static UWORD record_speed = SPEED_MAX;

static BOOL beep_flag = FALSE;
static BOOL close_session_flag = TRUE;
static BOOL confirm_flag = TRUE;
static BOOL eject_flag = FALSE;
static BOOL log_flag = TRUE;
static BOOL multisession_flag = FALSE;
static BOOL verify_flag = FALSE;
static BOOL test_flag = FALSE;
static BOOL underrun_protect_flag = FALSE;

//
// Print command line usage
//

void PrintUsage()
{
  printf ("CD2CD.EXE - Version %s (%s)\n", PRODUCT_VERSION, PRODUCT_RELEASE_DATE);
  printf ("%s\n\n", COPYRIGHT_MESSAGE);
  printf ("Usage: CD2CD [/SOURCE=ha:id:lun] [/TARGET=ha:id:lun]\n");
  printf ("         [/BATCH] [/BEEP] [/COPIES=n] [/EJECT] [/NOCONFIRM] [/NOFINALIZE]\n");
  printf ("         [/NOUNDERRUN]  [/SESSION] [/SPEED=n] [/TEST] [/VERIFY]\n");
  printf ("/SOURCE     - ASPI ID of CDROM reader (default is first found)\n");
  printf ("/TARGET     - ASPI ID of CDROM recorder (default is first found)\n");
  printf ("/BATCH      - Disable all messages and confirmation prompts\n");
  printf ("/BEEP       - Issue an audible 'beep' when recording has finished\n");
  printf ("/COPIES=n   - Number of discs to be recorded (default is 1)\n");
  printf ("/EJECT      - Eject CD when recording has finished\n");
  printf ("/NOCONFIRM  - Disable all confirmation prompts\n");
  printf ("/NOFINALIZE - Disable session/disc finalization\n");
  printf ("/NOUNDERRUN - Enable buffer underrun protection (if supported by recorder)\n");
  printf ("/SESSION    - Open new session after finalizing current session\n");
  printf ("/SPEED=n    - Recording speed (default is maximum)\n");
  printf ("/TEST       - Enable test recording mode\n");
  printf ("/VERIFY     - Verify CDROM for readability before recording\n");
}

//
// Parse command line arguments
//

void ParseCommandLine(int argc, char *argv[])
{
  extern BOOL env_cdreader_flag, env_cdwriter_flag;
  extern UBYTE env_cdreader_adapter, env_cdwriter_adapter;
  extern UBYTE env_cdreader_id, env_cdwriter_id;
  extern UBYTE env_cdreader_lun, env_cdwriter_lun;

  int i;
  char *argP, uparg[80];

  UWORD adapter, id, lun;

  // Load the environment variables.

  if (env_cdreader_flag)
    {
    cdreader_id_specified = TRUE;
    cdreader_adapter = env_cdreader_adapter;
    cdreader_id = env_cdreader_id;
    cdreader_lun = env_cdreader_lun;
    }

  if (env_cdwriter_flag)
    {
    cdwriter_id_specified = TRUE;
    cdwriter_adapter = env_cdwriter_adapter;
    cdwriter_id = env_cdwriter_id;
    cdwriter_lun = env_cdwriter_lun;
    }

  // Parse each command line argument.

  for (i = 1; i < argc; i++)
    {
    argP = argv[i];

    StringCopy (uparg, argP);
    strupr (uparg);

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

      else if (! strncmp (uparg, "/COPIES", 7))
        {
        if (sscanf (uparg, "/COPIES=%hu", &copy_count) != 1) {
          fprintf (stderr, "Error: Illegal /COPIES format!\n");
          exit (1);
          }

        if (copy_count == 0) {
          fprintf (stderr, "Error: Illegal copy count specified!\n");
          exit (1);
          }
        }

      else if (! strcmp (uparg, "/EJECT"))
        eject_flag = TRUE;

      else if (! strcmp (uparg, "/NOCONFIRM"))
        confirm_flag = FALSE;

      else if (! strcmp (uparg, "/NOFINALIZE"))
        close_session_flag = FALSE;

      else if (! strcmp (uparg, "/NOUNDERRUN"))
        underrun_protect_flag = TRUE;

      else if (! strcmp (uparg, "/SESSION"))
        multisession_flag = TRUE;

      else if (! strncmp (uparg, "/SPEED", 6))
        {
        UWORD speed;

        if (sscanf (uparg, "/SPEED=%hu", &speed) != 1) {
          fprintf (stderr, "Error: Illegal /SPEED format!\n");
          exit (1);
          }

        switch (speed)
          {
          case 1: record_speed = SPEED_1X; break;
          case 2: record_speed = SPEED_2X; break;
          case 4: record_speed = SPEED_4X; break;
          case 6: record_speed = SPEED_6X; break;
          case 8: record_speed = SPEED_8X; break;
          case 10: record_speed = SPEED_10X; break;
          case 12: record_speed = SPEED_12X; break;
          case 16: record_speed = SPEED_16X; break;
          case 20: record_speed = SPEED_20X; break;
          case 24: record_speed = SPEED_24X; break;
          case 32: record_speed = SPEED_32X; break;
          case 40: record_speed = SPEED_40X; break;
          case 44: record_speed = SPEED_44X; break;
          case 48: record_speed = SPEED_48X; break;
          case 52: record_speed = SPEED_52X; break;
          default:
            {
            fprintf (stderr, "Error: Illegal recording speed specified!\n");
            exit (1);
            }
          }
        }

      else if (! strcmp (uparg, "/TEST"))
        test_flag = TRUE;

      else if (! strcmp (uparg, "/VERIFY"))
        verify_flag = TRUE;

      else if (! strncmp (uparg, "/SOURCE", 7))
        {
        if (sscanf (uparg, "/SOURCE=%hu:%hu:%hu", &adapter, &id, &lun) == 3)
          {
          cdreader_adapter = adapter;
          cdreader_id = id;
          cdreader_lun = lun;
          cdreader_id_specified = TRUE;
          }
        else {
          fprintf (stderr, "Error: Illegal /SOURCE format!\n");
          exit (1);
          }
        }

      else if (! strncmp (uparg, "/TARGET", 7))
        {
        if (sscanf (uparg, "/TARGET=%hu:%hu:%hu", &adapter, &id, &lun) == 3)
          {
          cdwriter_adapter = adapter;
          cdwriter_id = id;
          cdwriter_lun = lun;
          cdwriter_id_specified = TRUE;
          }
        else {
          fprintf (stderr, "Error: Illegal /TARGET format!\n");
          exit (1);
          }
        }

      else {
        fprintf (stderr, "Invalid switch - %s\n", argP);
        exit (1);
        }
      }

    else {
      fprintf (stderr, "Too many parameters - %s\n", argP);
      exit (1);
      }
    }
}

//
// Main entry point
//

main(int argc, char *argv[])
{
  UWORD display_speed;
  
  TAOCB taocb;

  CDReader *cdreaderP;
  CDWriter *cdwriterP;

  // Enable exception handling.

  EXCEPTION_HANDLER_START

  // Get the environment variables.

  GetEnvironmentVariables ();

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
          (ASPI_M_DEVTYPE_CDROM | ASPI_M_DEVTYPE_WORM),
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
    if ((cdreaderP = (CDReader *)ASPIAdapter::FindDeviceObject(
          ASPI_M_DEVTYPE_CDROM | ASPI_M_DEVTYPE_WORM)) == NULL)
      {
      fprintf (stderr,
        "\nError: Unable to find a known CDROM device!\n");
      exit (1);
      }
    }

  if (log_flag)
    {
    printf ("CDROM device found...\n");
    printf ("  HA #%u - ASPI ID #%u - %-8s %-16s %-4s\n",
      cdreaderP->GetAdapter(), cdreaderP->GetId(),
      cdreaderP->GetVendorId(), cdreaderP->GetProductId(),
      cdreaderP->GetFirmwareLevel());
    }

  // Find a CD-R device...

  if (cdwriter_id_specified)
    {
    if ((cdwriterP = (CDWriter *)ASPIAdapter::FindDeviceObject (
          ASPI_M_DEVTYPE_WORM, cdwriter_adapter, cdwriter_id, cdwriter_lun)) == NULL)
      {
      fprintf (stderr,
        "\nError: Specified device (%u:%u:%u) is not a CD-Recorder or is unknown!\n",
        cdwriter_adapter, cdwriter_id, cdwriter_lun);
      exit (1);
      }
    }
  else
    {
    if ((cdwriterP = (CDWriter *)ASPIAdapter::FindDeviceObject(ASPI_M_DEVTYPE_WORM)) == NULL)
      {
      fprintf (stderr,
        "\nError: Unable to find a known CD-Recorder device!\n");
      exit (1);
      }
    }

  if (log_flag)
    {
    printf ("CD-Recorder device found...\n");
    printf ("  HA #%u - ASPI ID #%u - %-8s %-16s %-4s\n",
      cdwriterP->GetAdapter(), cdwriterP->GetId(),
      cdwriterP->GetVendorId(), cdwriterP->GetProductId(),
      cdwriterP->GetFirmwareLevel());
    }

  // Make sure the device is ready.

  cdreaderP->LoadDisc (TRUE);

  // Make sure the disc has only one track.

  DiscInfo *discinfoP = new DiscInfo(cdreaderP);
  int track_count = discinfoP->m_nTrackCount;
  delete discinfoP;

  if (track_count > 1) {
    fprintf (stderr, "\nError: Unable to copy CDs with more than one track!\n");
    exit (1);
    }

  // Allocate an I/O buffer (automatically deallocated).

  IOBuffer iobuffer(sizeof(IOBUF));
  IOBUF *iobufP = (IOBUF *)iobuffer.GetHandle();

  // Determine the number of blocks to copy.

  ULONG blkcnt = cdreaderP->FindLastDataSector() + 1;
  UWORD blklen = SECTOR_MODE1_BLKLEN;

  if (blkcnt > CDROM_80MIN_BLKCNT)
    printf ("\nWARNING: Disc is longer than 80 minutes!\n");

  // Calculate the number of blocks per I/O.

  UWORD blocks_per_io = ASPIAdapter::MaxBufferSize / blklen;

  // Determine the actual recording speed.

  record_speed = MIN (record_speed, cdwriterP->GetMaxWriteSpeed());
  display_speed = ActualSpeedTable[record_speed];

  // Calculate the approximate time to copy.

  if (log_flag)
    {
    ULONG blocks_per_min = 75 * 60 * display_speed;
    UWORD minutes = CDIV (blkcnt, blocks_per_min);

    printf ("\nCDROM contains %luMb of data (%lu blocks)\n",
      CDIV (blkcnt * blklen, 0x100000), blkcnt);

    printf ("Approximate recording time at %ux speed is %u minutes\n",
      display_speed, minutes);
    }

  // If requested, verify the CDROM for readability.

  if (verify_flag)
    {
    // Prompt to begin?

    if (confirm_flag) {
      printf ("\nHit <ENTER> to verify source disc (or CTRL/C to exit)...");
      getchar ();
      }

    // Set the reading mode.

    cdreaderP->SetBlockLength (DATATYPE_CDROM, SECTOR_MODE1_BLKLEN, TRUE);

    // Verify all blocks.

    if (log_flag) {
      printf ("\nVerifying source disc...\n");
      printf (" 0%% completed.\r");
      }

    ULONG blocks_read = 0;
    UWORD cur_percent = 0;

    for (SLONG lba = 0; lba < blkcnt; lba += blocks_per_io)
      {
      UWORD blocks = MIN (blocks_per_io, blkcnt - lba);

      // Read the source disc.

      cdreaderP->ReadDiscData (iobufP->data, blklen, blocks, lba, TRUE);

      // Increment the total number of blocks verified.

      blocks_read += blocks;

      // Log completion status?

      if (log_flag)
        {
        UWORD percent = (blocks_read * 100) / blkcnt;

        if (percent > cur_percent) {
          printf (" %u%% completed.\r", percent);
          cur_percent = percent;
          }
        }
      }

    if (log_flag) printf ("Verification completed.\n");
    }

  // Initialize the track-at-once context block.

  MEMCLEAR (&taocb, sizeof(TAOCB));

  TRACKINFO *trackinfoP = &taocb.trackinfo;
  
  taocb.recordoptions.speed = record_speed;
  taocb.recordoptions.disc_datatype = DATATYPE_CDROM;
  taocb.recordoptions.close_session_flag = close_session_flag;
  taocb.recordoptions.multisession_flag = multisession_flag;
  taocb.recordoptions.test_flag = test_flag;
  taocb.recordoptions.underrun_protect_flag = underrun_protect_flag;
  taocb.recordoptions.beep_flag = beep_flag;
  taocb.recordoptions.eject_flag = eject_flag;
  taocb.recordoptions.log_flag = log_flag;

  trackinfoP->track = 0;
  trackinfoP->datatype = DATATYPE_CDROM;
  trackinfoP->blklen = SECTOR_MODE1_BLKLEN;
  trackinfoP->data_blkcnt = blkcnt;
  trackinfoP->data_flag = TRUE;

  // Make the copies...

  for (UWORD c = 0; c < copy_count; c++)
    {
    // Prompt to begin...

    if (confirm_flag || (copy_count > 1))
      {
      printf ("\n");
      if (test_flag) printf ("TEST recording mode is enabled!\n");
      printf ("Hit <ENTER> to begin recording (or CTRL/C to exit)...");
      getchar();
      }

    // Set the reading mode.

    cdreaderP->SetBlockLength (DATATYPE_CDROM, SECTOR_MODE1_BLKLEN, TRUE);

    // Start track-at-once recording.

    cdwriterP->StartTrackAtOnce (&taocb, TRUE);

    // Copy the CDROM to the CD-R.

    ULONG blocks_written = 0;
    UWORD cur_percent = 0;

    if (log_flag) {
      printf ("\nCopying source disc to recorder...\n");
      printf (" 0%% completed.\r");
      }

    for (SLONG lba = 0; lba < blkcnt; lba += blocks_per_io)
      {
      UWORD blocks = MIN (blocks_per_io, blkcnt - lba);
  
      // Copy some blocks from the source disc to the CD-R.

      cdreaderP->ReadDiscData (iobufP->data, blklen, blocks, lba, TRUE);
      cdwriterP->WriteDisc (iobufP->data, blklen, blocks, 0, TRUE);

      // Increment the total number of blocks written.

      blocks_written += blocks;

      // Log completion status?

      if (log_flag)
        {
        UWORD percent = (blocks_written * 100) / blkcnt;

        if (percent > cur_percent) {
          printf (" %u%% completed.\r", percent);
          cur_percent = percent;
          }
        }
      }

    if (log_flag) printf ("Copy completed.             \n");

    // Finish track-at-once recording.

    cdwriterP->FinishTrackAtOnce (&taocb, TRUE);

    // Success.

    if (log_flag) printf ("\nCD successfully recorded!\n");
    }

  // Shutdown the ASPI manager.

  ASPIAdapter::ShutdownManager();

  // End exception handling.

  EXCEPTION_HANDLER_EXIT

  return (0);
}
