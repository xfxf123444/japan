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

static char *image_filnam = NULL;

static UWORD copy_count = 1;
static UWORD record_speed = SPEED_MAX;

static UBYTE cdwriter_adapter, cdwriter_id, cdwriter_lun;
static BOOL cdwriter_id_specified = FALSE;

static BOOL beep_flag = FALSE;
static BOOL close_session_flag = TRUE;
static BOOL confirm_flag = TRUE;
static BOOL eject_flag = FALSE;
static BOOL log_flag = TRUE;
static BOOL multisession_flag = FALSE;
static BOOL postgap_flag = FALSE;
static BOOL test_flag = FALSE;
static BOOL underrun_protect_flag = FALSE;

//
// Print command line usage
//

void PrintUsage()
{
  printf ("FILE2CD.EXE - Version %s (%s)\n", PRODUCT_VERSION, PRODUCT_RELEASE_DATE);
  printf ("%s\n\n", COPYRIGHT_MESSAGE);
  printf ("Usage: FILE2CD imagefile [/ID=ha:id:lun] [/BATCH] [/BEEP] [/COPIES=n]\n");
  printf ("         [/EJECT] [/NOCONFIRM] [/NOFINALIZE] [/NOUNDERRUN] [/POSTGAP]\n");
  printf ("         [/SESSION] [/SPEED=n] [/TEST]\n");
  printf ("imagefile   - Image data filename\n");
  printf ("/ID         - ASPI ID of CD recorder (default is first found)\n");
  printf ("/BATCH      - Disable all messages and confirmation prompts\n");
  printf ("/BEEP       - Issue an audible 'beep' when recording has finished\n");
  printf ("/COPIES=n   - Number of discs to be recorded (default is 1)\n");
  printf ("/EJECT      - Eject CD when recording has finished\n");
  printf ("/NOCONFIRM  - Disable all confirmation prompts\n");
  printf ("/NOFINALIZE - Disable session/disc finalization\n");
  printf ("/NOUNDERRUN - Enable buffer underrun protection (if supported by recorder)\n");
  printf ("/POSTGAP    - Enable writing of track postgap\n");
  printf ("/SESSION    - Open new session after finalizing current session\n");
  printf ("/SPEED=n    - Recording speed (default is maximum supported)\n");
  printf ("/TEST       - Enable test recording mode\n");
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

  UWORD adapter, id, lun;
  char *argP, uparg[80];

  // Print usage?

  if (argc == 1) {
    PrintUsage ();
    exit (0);
    }

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

      else if (! strncmp (uparg, "/ID", 3))
        {
        if (sscanf (uparg, "/ID=%hu:%hu:%hu", &adapter, &id, &lun) == 3)
          {
          cdwriter_adapter = adapter;
          cdwriter_id = id;
          cdwriter_lun = lun;
          cdwriter_id_specified = TRUE;
          }
        else {
          fprintf (stderr, "Error: Illegal /ID format!\n");
          exit (1);
          }
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

      else if (! strcmp (uparg, "/POSTGAP"))
        postgap_flag = TRUE;

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

      else {
        fprintf (stderr, "Invalid switch - %s\n", argP);
        exit (1);
        }
      }

    else
      {
      if (image_filnam == NULL)
        image_filnam = argP;
      else {
        fprintf (stderr, "Too many parameters - %s\n", argP);
        exit (1);
        }
      }
    }

  // Check for required filename.

  if (image_filnam == NULL) {
    fprintf (stderr, "Error: IMAGE filename must be specified!\n");
    exit (1);
    }
}

//
// Main entry point.
//

main(int argc, char *argv[])
{
  FILEHANDLE image_file;
  RECORDOPTIONS options;
  UWORD display_speed;
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
    printf ("  HA #%u - ASPI ID #%u - %-8s %-16s %-4s\n",
      cdwriterP->GetAdapter(), cdwriterP->GetId(),
      cdwriterP->GetVendorId(), cdwriterP->GetProductId(),
      cdwriterP->GetFirmwareLevel());
    }

  // Open the CDROM image file.

  if ((image_file = FileOpenRead (image_filnam)) == NULL) {
    fprintf (stderr, "\nError opening image file \"%s\"\n", image_filnam);
    exit (1);
    }

  // Calculate the number of CDROM blocks in the file.

  ULONG datalen = FileGetLength (image_file);
  ULONG data_blkcnt = datalen / SECTOR_MODE1_BLKLEN;

  // Validate the image file length and block count.

  if ((data_blkcnt * SECTOR_MODE1_BLKLEN) != datalen) {
    fprintf (stderr,
      "\nError: Image file length is not a multiple of the CDROM sector size!\n");
    exit (1);
    }

  if (data_blkcnt > CDROM_80MIN_BLKCNT)
    fprintf (stderr, "\nWarning: Image file contains more than 80 minutes of data!\n");

  // Close the file.

  FileClose (image_file);

  // Determine the actual recording speed.

  record_speed = MIN (record_speed, cdwriterP->GetMaxWriteSpeed());
  display_speed = ActualSpeedTable[record_speed];

  // Calculate the approximate time to copy.

  if (log_flag)
    {
    ULONG blocks_per_min = 75 * 60 * display_speed;
    UWORD minutes = CDIV (data_blkcnt, blocks_per_min);

    printf ("\nFile contains %luMb of data (%lu blocks)\n",
      CDIV (datalen, 0x100000), data_blkcnt);

    printf ("Approximate recording time at %ux speed is %u minutes\n",
      display_speed, minutes);
    }

  // Initialize the recording options.

  MEMCLEAR (&options, sizeof(RECORDOPTIONS));

  options.speed = record_speed;
  options.disc_datatype = DATATYPE_CDROM;
  options.postgap_flag = postgap_flag;
  options.close_session_flag = close_session_flag;
  options.multisession_flag = multisession_flag;
  options.test_flag = test_flag;
  options.underrun_protect_flag = underrun_protect_flag;
  options.beep_flag = beep_flag;
  options.eject_flag = eject_flag;
  options.log_flag = log_flag;

  // Make the copies...

  for (UWORD c = 0; c < copy_count; c++)
    {
    // Prompt to begin?

    if (confirm_flag || (copy_count > 1))
      {
      printf ("\n");
      if (test_flag) printf ("TEST recording mode is enabled!\n");
      printf ("Hit <ENTER> to begin recording (or CTRL/C to exit)...");
      getchar();
      }

    if (log_flag) printf ("\n");

    // Record the image file using track-at-once recording.

    cdwriterP->RecordTrackAtOnce (
      image_filnam, FILETYPE_BINARY_LSB, DATATYPE_CDROM, SECTOR_MODE1_BLKLEN, &options);

    // Success.

    if (log_flag) printf ("\nCD successfully recorded!\n");
    }

  // Shutdown the ASPI manager.

  ASPIAdapter::ShutdownManager();

  // End exception handling.

  EXCEPTION_HANDLER_EXIT

  return (0);
}
