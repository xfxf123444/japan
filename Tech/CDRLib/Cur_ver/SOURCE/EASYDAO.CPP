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

// External variables.

extern unsigned _stklen = 8192;   // Stack length.

extern UBYTE ActualSpeedTable[];

// Command line parameters.

static UBYTE cdwriter_adapter, cdwriter_id, cdwriter_lun;
static BOOL cdwriter_id_specified = FALSE;

static UWORD record_speed = SPEED_MAX;

static BOOL beep_flag = FALSE;
static BOOL eject_flag = FALSE;
static BOOL multisession_flag = FALSE;
static BOOL test_flag = FALSE;

// Function prototypes.

void ParseCommandLine(int argc, char *argv[]);
void PrintUsage(void);

//
// Print command line usage
//

void PrintUsage()
{
  printf ("Usage: EASYDAO [/ID=ha:id:lun] [/BEEP] [/EJECT]\n");
  printf ("         [/SESSION] [/SPEED=n] [/TEST]\n");
  printf ("/ID        - ASPI ID of CD recorder (default is first found)\n");
  printf ("/BEEP      - Issue an audible 'beep' when recording has finished\n");
  printf ("/EJECT     - Eject CD when recording has finished\n");
  printf ("/SESSION   - Enable session-at-once recording (default is disc-at-once)\n");
  printf ("/SPEED=n   - Recording speed (default is maximum)\n");
  printf ("/TEST      - Enable test recording mode\n");
}

//
// Parse DOS command line.
//

void ParseCommandLine(int argc, char *argv[])
{
  int i;
  char *argP, uparg[80];

  UWORD adapter, id, lun;

  // Parse each command line argument.

  for (i = 1; i < argc; i++)
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

      else if (! strcmp (uparg, "/BEEP"))
        beep_flag = TRUE;

      else if (! strcmp (uparg, "/EJECT"))
        eject_flag = TRUE;

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

    // otherwise, there are too many parameters...

    else
      {
      fprintf (stderr, "Too many parameters - %s\n", argP);
      exit (1);
      }
    }
}

//
// Main routine
//

main(int argc, char *argv[])
{
  UWORD display_speed;
  MSF msf;

  EASYDAO easydao;
  RECORDOPTIONS options;
  
  CDWriter *cdwriterP;

  // Enable exception handling.

  EXCEPTION_HANDLER_START

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
    if ((cdwriterP = (CDWriter *)ASPIAdapter::FindDeviceObject (ASPI_M_DEVTYPE_WORM)) == NULL) {
      fprintf (stderr,
        "\nError: Unable to find a known CD-Recorder device!\n");
      exit (1);
      }
    }

  // Display the CD recorder information.

  printf ("CD-Recorder device found...\n");
  printf ("  HA #%u - ASPI ID #%u - %-8s %-16s %-4s\n\n",
    cdwriterP->GetAdapter(), cdwriterP->GetId(),
    cdwriterP->GetVendorId(), cdwriterP->GetProductId(),
    cdwriterP->GetFirmwareLevel());

  // Make sure the device supports "disc-at-once" recording.

  if (! cdwriterP->IsFlagSet(CDWriter::cwfDiscAtOnce)) {
    fprintf (stderr,
      "Error: CD-Recorder does not support 'disc-at-once' recording!\n");
    exit (1);
    }

  // Initialize an "Easy DAO" descriptor

  MEMCLEAR (&easydao, sizeof(EASYDAO));

  easydao.titleP = "Christmas Songs";
  easydao.first_track_number = 1;
  easydao.track_count = 3;

  easydao.tracks[0].filnamP = "C:\\SILENT.WAV";
  easydao.tracks[0].titleP = "Silent Night";
  easydao.tracks[0].filetype = FILETYPE_WAVE_STEREO;
  easydao.tracks[0].datatype = DATATYPE_CDDA;
  easydao.tracks[0].blklen = SECTOR_CDDA_BLKLEN;
  easydao.tracks[0].pregap_blkcnt = 0;
  easydao.tracks[0].postgap_blkcnt = 0;
  easydao.tracks[0].copy_permitted = FALSE;
  easydao.tracks[0].four_channel = FALSE;
  easydao.tracks[0].pre_emphasis = FALSE;
  easydao.tracks[0].isrc_valid = FALSE;
  
  easydao.tracks[1].filnamP = "C:\\JINGLE.MP3";
  easydao.tracks[1].titleP = "Jingle Bells";
  easydao.tracks[1].filetype = FILETYPE_MP3;
  easydao.tracks[1].datatype = DATATYPE_CDDA;
  easydao.tracks[1].blklen = SECTOR_CDDA_BLKLEN;
  easydao.tracks[1].pregap_blkcnt = 150;
  easydao.tracks[1].postgap_blkcnt = 0;
  easydao.tracks[1].copy_permitted = FALSE;
  easydao.tracks[1].four_channel = FALSE;
  easydao.tracks[1].pre_emphasis = FALSE;
  easydao.tracks[1].isrc_valid = FALSE;
  
  easydao.tracks[2].filnamP = "C:\\RUDOLPH.AIF";
  easydao.tracks[2].titleP = "Rudolph The Red Nosed Reindeer";
  easydao.tracks[2].filetype = FILETYPE_AIFF_STEREO;
  easydao.tracks[2].datatype = DATATYPE_CDDA;
  easydao.tracks[2].blklen = SECTOR_CDDA_BLKLEN;
  easydao.tracks[2].pregap_blkcnt = 150;
  easydao.tracks[2].postgap_blkcnt = 0;
  easydao.tracks[2].copy_permitted = FALSE;
  easydao.tracks[2].four_channel = FALSE;
  easydao.tracks[2].pre_emphasis = FALSE;
  easydao.tracks[2].isrc_valid = FALSE;
  
  // Build a cuesheet.

  CueSheet *cuesheetP = new CueSheet(&easydao);

  // Make sure that the data will fit on an 80 minute CD.

  if (cuesheetP->disc_high_lba > CDROM_80MIN_BLKCNT)
    {
    MSF msf;

    ConvertLBA2MSF (cuesheetP->disc_high_lba, &msf);

    fprintf (stderr, "WARNING! Total disc time (%02u:%02u:%02u) exceeds 80 minutes!\n",
      msf.minutes, msf.seconds, msf.frames);
    }

  // Display some statistics.

  printf ("Disc Statistics...\n");
  printf ("  Number of files     : %03u\n", cuesheetP->file_count);
  printf ("  Number of tracks    : %03u\n", cuesheetP->track_count);
  printf ("  Number of cuepoints : %03u\n\n", cuesheetP->cuepoint_count - 2);

  // Determine the actual recording speed.

  record_speed = MIN (record_speed, cdwriterP->GetMaxWriteSpeed());
  display_speed = ActualSpeedTable[record_speed];

  // Print the recording statistics.

  ULONG write_blkcnt = cuesheetP->disc_high_lba + 20000;

  ULONG blocks_per_min = 60 * 75 * display_speed;
  UWORD record_min = CDIV (write_blkcnt, blocks_per_min);

  ConvertLBA2MSF (cuesheetP->disc_high_lba, &msf);
  printf ("Total disc time is %02u:%02u:%02u\n",
    msf.minutes, msf.seconds, msf.frames);

  printf ("Approximate recording time at %ux speed is %u minutes\n\n",
    display_speed, record_min);

  // Initialize the recording options.

  MEMCLEAR (&options, sizeof(RECORDOPTIONS));

  options.speed = record_speed;
  options.beep_flag = beep_flag;
  options.eject_flag = eject_flag;
  options.log_flag = TRUE;
  options.multisession_flag = multisession_flag;
  options.test_flag = test_flag;

  // Prompt to begin recording...

  if (test_flag) printf ("TEST recording mode is enabled!\n");
  printf ("Hit <ENTER> to begin recording (or CTRL/C to exit)...");
  getchar();
  printf ("\n");

  // Record the disc using disc-at-once recording.

  cdwriterP->RecordDiscAtOnce (cuesheetP, &options);

  // Success.

  printf ("\nCD successfully recorded!\n");

  // Free the cuesheet object.

  delete cuesheetP;

  // Shutdown the ASPI manager.

  ASPIAdapter::ShutdownManager();

  // End exception handling.

  EXCEPTION_HANDLER_EXIT

  return (0);
}

