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

static UBYTE disk_adapter, disk_id, disk_lun;
static BOOL disk_id_specified = FALSE;

static UBYTE cdwriter_adapter, cdwriter_id, cdwriter_lun;
static BOOL cdwriter_id_specified = FALSE;

static UWORD record_speed = SPEED_MAX;

static BOOL beep_flag = FALSE;
static BOOL close_session_flag = TRUE;
static BOOL confirm_flag = TRUE;
static BOOL eject_flag = FALSE;
static BOOL log_flag = TRUE;
static BOOL multisession_flag = FALSE;
static BOOL verify_flag = FALSE;
static BOOL test_flag = FALSE;

//
// Print command line usage
//

void PrintUsage()
{
  printf ("DISK2CD.EXE - Version %s (%s)\n", PRODUCT_VERSION, PRODUCT_RELEASE_DATE);
  printf ("%s\n\n", COPYRIGHT_MESSAGE);
  printf ("Usage: DISK2CD [/SOURCE=ha:id:lun] [/TARGET=ha:id:lun]\n");
  printf ("         [/BATCH] [/BEEP] [/EJECT] [/NOCONFIRM] [/NOFINALIZE]\n");
  printf ("         [/SESSION] [/SPEED=n] [/TEST] [/VERIFY]\n");
  printf ("/SOURCE     - ASPI ID of harddisk (default is first found)\n");
  printf ("/TARGET     - ASPI ID of CDROM recorder (default is first found)\n");
  printf ("/BATCH      - Disable all logging messages and confirmation prompts\n");
  printf ("/BEEP       - Issue an audible 'beep' when recording has finished\n");
  printf ("/EJECT      - Eject CD when recording has finished\n");
  printf ("/NOCONFIRM  - Disable all confirmation prompts\n");
  printf ("/NOFINALIZE - Disable session/disc finalization\n");
  printf ("/SESSION    - Open new session after finalizing current session\n");
  printf ("/SPEED=n    - Recording speed (default = 2x)\n");
  printf ("/TEST       - Enable test recording mode\n");
  printf ("/VERIFY     - Verify disk for readability before recording\n");
}

//
// Parse command line arguments
//

void ParseCommandLine(int argc, char *argv[])
{
  extern BOOL env_cdwriter_flag;
  extern UBYTE env_cdwriter_adapter;
  extern UBYTE env_cdwriter_id;
  extern UBYTE env_cdwriter_lun;

  char *argP, uparg[80];
  UWORD adapter, id, lun;

  // Load the environment variables.

  if (env_cdwriter_flag)
    {
    cdwriter_id_specified = TRUE;
    cdwriter_adapter = env_cdwriter_adapter;
    cdwriter_id = env_cdwriter_id;
    cdwriter_lun = env_cdwriter_lun;
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
        confirm_flag = FALSE;
        log_flag = FALSE;
        }

      else if (! strcmp (uparg, "/BEEP"))
        beep_flag = TRUE;

      else if (! strcmp (uparg, "/EJECT"))
        eject_flag = TRUE;

      else if (! strcmp (uparg, "/NOCONFIRM"))
        confirm_flag = FALSE;

      else if (! strcmp (uparg, "/NOFINALIZE"))
        close_session_flag = FALSE;

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
          disk_adapter = adapter;
          disk_id = id;
          disk_lun = lun;
          disk_id_specified = TRUE;
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
  SLONG last_lba;
  ULONG disk_blkcnt, blocks_written;
  UWORD display_speed, percent, cur_percent;

  ASPIDevice *diskdeviceP;
  CDWriter *cdwriterP;

  TAOCB taocb;

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

  // Find a disk device...

  if (disk_id_specified)
    {
    if ((diskdeviceP = (ASPIDevice *)ASPIAdapter::FindDeviceObject (
          ASPI_M_DEVTYPE_DISK,
          disk_adapter, disk_id, disk_lun)) == NULL)
      {
      fprintf (stderr,
        "\nError: Specified device (%u:%u:%u) is not a disk!\n",
        disk_adapter, disk_id, disk_lun);
      exit (1);
      }
    }
  else
    {
    if ((diskdeviceP = (ASPIDevice *)ASPIAdapter::FindDeviceObject (ASPI_M_DEVTYPE_DISK)) == NULL) {
      fprintf (stderr,
        "\nError: Unable to find a SCSI disk device on the system!\n");
      exit (1);
      }
    }

  if (log_flag)
    {
    printf ("Disk device found...\n");
    printf ("  HA #%u - ASPI ID #%u - %-8s %-16s %-4s\n",
      diskdeviceP->GetAdapter(), diskdeviceP->GetId(),
      diskdeviceP->GetVendorId(), diskdeviceP->GetProductId(),
      diskdeviceP->GetFirmwareLevel());
    }

  // Find a CD-R device...

  if (cdwriter_id_specified)
    {
    if ((cdwriterP = (CDWriter *)ASPIAdapter::FindDeviceObject (
          ASPI_M_DEVTYPE_WORM,
          cdwriter_adapter, cdwriter_id, cdwriter_lun)) == NULL)
      {
      fprintf (stderr,
        "\nError: Specified device (%u:%u:%u) is not a CD-Recorder or is unknown!\n",
        cdwriter_adapter, cdwriter_id, cdwriter_lun);
      exit (1);
      }
    }
  else
    {
    if ((cdwriterP = (CDWriter *)ASPIAdapter::FindDeviceObject (ASPI_M_DEVTYPE_WORM)) == NULL) {
      fprintf (stderr,
        "\nError: Unable to find a known CD-Recorder device!\n");
      exit (1);
      }
    }

  if (log_flag)
    {
    printf ("CD-Recorder device found...\n");
    printf ("  HA #%u - ASPI ID #%u - %-8s %-16s %-4s\n\n",
      cdwriterP->GetAdapter(), cdwriterP->GetId(),
      cdwriterP->GetVendorId(), cdwriterP->GetProductId(),
      cdwriterP->GetFirmwareLevel());
    }

  // Make sure the DISK is ready.

  diskdeviceP->TestUnitReady (TRUE);

  // Determine the number of disk blocks to copy.

  diskdeviceP->ReadCapacity (&last_lba, NULL, FALSE, TRUE);
  disk_blkcnt = last_lba + 1;

  // Determine the actual recording speed.

  record_speed = MIN (record_speed, cdwriterP->GetMaxWriteSpeed());
  display_speed = ActualSpeedTable[record_speed];

  // Display the disk capacity.

  if (log_flag)
    printf ("Disk contains %luMb of data (%lu blocks).\n",
      CDIV (disk_blkcnt * SECTOR_DISK_BLKLEN, 0x100000), disk_blkcnt);

  // Check for a disk that is larger than the capacity of a 74min CDROM.

  if (disk_blkcnt > 1330000)
    {
    // Output a warning message.

    printf ("\nWARNING: Disk size exceeds the capacity of a CDROM (650Mb)!\n",
      CDIV (disk_blkcnt * SECTOR_DISK_BLKLEN, 0x100000));

    if (! confirm_flag) exit(1);

    printf ("If you choose to continue, only the first 650Mb will be copied.\n");
    printf ("This may result in an unreadable CDROM!!\n\n");

    // Limit the number of blocks to copy.

    disk_blkcnt = 1330000;
    }

  // Display the approximate recording time.

  if (log_flag)
    {
    ULONG blocks_per_min = (75 * 60 * display_speed) * 4;
    UWORD minutes = CDIV (disk_blkcnt, blocks_per_min);

    printf ("Approximate recording time at %ux speed is %u minutes.\n\n",
      display_speed, minutes);
    }

  // Allocate an I/O buffer (automatically deallocated).

  IOBuffer iobuffer(sizeof(IOBUF));
  IOBUF *iobufP = (IOBUF *)iobuffer.GetHandle();

  // Calculate the number of blocks per I/O.

  UWORD blocks_per_io = (ASPIAdapter::MaxBufferSize / SECTOR_MODE1_BLKLEN) * 4;

  // If requested, verify the DISK for readability.

  if (verify_flag)
    {
    if (confirm_flag)
      {
      printf ("Hit <ENTER> to verify DISK readability...");
      getchar ();
      printf ("\n");
      }

    if (log_flag) {
      printf ("Verifying source disk...\n");
      printf (" 0%% completed.\r");
      }

    blocks_written = 0;
    cur_percent = 0;

    for (SLONG lba = 0; lba < disk_blkcnt; lba += blocks_per_io)
      {
      UWORD blocks = MIN (blocks_per_io, disk_blkcnt - lba);

      // Read the data.

      diskdeviceP->ReadBlocks (iobufP->data, SECTOR_DISK_BLKLEN, blocks, lba, TRUE);

      // Increment the total number of blocks written.

      blocks_written += blocks;

      // Percent complete status change?

      if (log_flag)
        {
        percent = (blocks_written * 100) / disk_blkcnt;

        if (percent > cur_percent) {
          printf (" %u%% completed.\r", percent);
          cur_percent = percent;
          }
        }
      }

    if (log_flag) printf ("Verification completed.\n\n");
    }

  // Prompt to begin...

  if (confirm_flag)
    {
    if (test_flag) printf ("TEST recording mode is enabled!\n");
    printf ("Hit <ENTER> to begin recording (or CTRL/C to exit)...");
    getchar ();
    printf ("\n");
    }

  // Initialize the TAO context block.

  MEMCLEAR (&taocb, sizeof(TAOCB));

  TRACKINFO *trackinfoP = &taocb.trackinfo;

  trackinfoP->track = 0;
  trackinfoP->datatype = DATATYPE_CDROM;
  trackinfoP->blklen = SECTOR_MODE1_BLKLEN;
  trackinfoP->data_blkcnt = CDIV (disk_blkcnt, 4);
  trackinfoP->data_flag = TRUE;

  taocb.recordoptions.speed = record_speed;
  taocb.recordoptions.disc_datatype = DATATYPE_CDROM;
  taocb.recordoptions.close_session_flag = close_session_flag;
  taocb.recordoptions.multisession_flag = multisession_flag;
  taocb.recordoptions.test_flag = test_flag;
  taocb.recordoptions.beep_flag = beep_flag;
  taocb.recordoptions.eject_flag = eject_flag;
  taocb.recordoptions.log_flag = log_flag;

  // Start track-at-once recording.

  cdwriterP->StartTrackAtOnce (&taocb, TRUE);

  // Copy the DISK to the CD-R.

  blocks_written = 0;
  cur_percent = 0;

  if (log_flag) {
    printf ("Copying disk to recorder...\n");
    printf (" 0%% completed.\r");
    }

  for (SLONG lba = 0; lba <= last_lba; lba += blocks_per_io)
    {
    ULONG source_blocks = MIN (blocks_per_io, disk_blkcnt - lba);
    ULONG target_blocks = CDIV (source_blocks, 4);

    // Read some disk blocks and write them to the CD-R.

    diskdeviceP->ReadBlocks (
      iobufP->data, SECTOR_DISK_BLKLEN, source_blocks, lba, TRUE);

    cdwriterP->WriteDisc (
      iobufP->data, SECTOR_MODE1_BLKLEN, target_blocks, 0, TRUE);

    // Increment the number of blocks written.

    blocks_written += source_blocks;

    // Log completion status?

    if (log_flag)
      {
      percent = (blocks_written * 100) / disk_blkcnt;

      if (percent > cur_percent) {
        printf (" %u%% completed.\r", percent);
        cur_percent = percent;
        }
      }
    }

  if (log_flag) printf ("Copy completed.             \n");

  // Write 150 sectors of postgap.

  if (log_flag) printf ("Writing track postgap (150 blocks)...\n");
  cdwriterP->RecordGap (GAPTYPE_POSTGAP, DATATYPE_CDROM, SECTOR_MODE1_BLKLEN, 150, FALSE);

  // Finish track-at-once recording.

  cdwriterP->FinishTrackAtOnce (&taocb, TRUE);

  // Success.

  printf ("\nCD successfully recorded!\n");

  // Shutdown the ASPI manager.

  ASPIAdapter::ShutdownManager();

  // End exception handling.

  EXCEPTION_HANDLER_EXIT

  return (0);
}

