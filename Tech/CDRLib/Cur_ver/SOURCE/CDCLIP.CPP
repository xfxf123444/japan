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
#include "protect.h"
#include "prodver.h"

// External variables.

extern unsigned _stklen = 8192;

// Protection information.

extern CINFO gControlFileInfo;      // Control file information.
extern BOOL gbAppUnlocked;          // Application unlocked?

// Command line parameters.

static char *outfilnam = NULL;

static UBYTE filetype = FILETYPE_UNKNOWN;

static SLONG start_lba = 0;
static SLONG end_lba = 0x7FFFFFFF;

static UWORD read_speed = SPEED_MAX;
static UWORD track_number = 0;

static UBYTE cdreader_adapter, cdreader_id, cdreader_lun;
static BOOL cdreader_id_specified = FALSE;

static UWORD data_blklen = SECTOR_RAW_BLKLEN;

static UBYTE jitter_mode = JITTER_MODE_AUTO;
static UBYTE error_mode = READERROR_MODE_ABORT;

static BOOL cdg_flag = FALSE;
static BOOL data_flag = FALSE;
static BOOL log_flag = TRUE;
static BOOL subq_flag = FALSE;

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

static void PrintUsage(void);
static void ParseCommandLine(int argc, char *argv[]);
static void ExtractCallback(void *handle, void *bufP, UWORD blklen, UWORD blocks);

//
// Print command line usage
//

static void PrintUsage()
{
  printf ("CDCLIP.EXE - Version %s (%s)\n", PRODUCT_VERSION, PRODUCT_RELEASE_DATE);
  printf ("%s\n\n", COPYRIGHT_MESSAGE);
  printf ("Usage: CDCLIP <imagefile> [/START=<start>] [/END=<end>] [/TRACK=n]\n");
  printf ("         [/ID=ha:id:lun] [/BATCH] [/CDG] [/DATA] [/JC] [/NOJC] [/IGNORE]\n");
  printf ("         [/SPEED=n] [/AIF] [/BIN] [/MOT] [/WAV]\n");
  printf ("imagefile - Output image data file\n");
  printf ("/START    - Starting MSF/LBA (default is start of disc)\n");
  printf ("/END      - Ending MSF/LBA (default is end of disc)\n");
  printf ("/TRACK    - Copy specified track number (1 to 99)\n");
  printf ("/ID       - ASPI ID of CDROM drive (default is first found)\n");
  printf ("/BATCH    - Disable all logging messages\n");
  printf ("/CDG      - Enable reading of CD+G subcodes\n");
  printf ("/DATA     - Enable reading of raw (2352 byte) data sectors\n");
  printf ("/JC       - Enable 'jitter' correction (audio only)\n");
  printf ("/NOJC     - Disable 'jitter' correction (audio only)\n");
  printf ("/IGNORE   - Ignore all reading errors\n");
  printf ("/SPEED=n  - Audio reading speed (default is maximum)\n");
  printf ("/AIF      - Write output file in AIFF format (audio only)\n");
  printf ("/BIN      - Write output file in Intel binary format (default for data)\n");
  printf ("/MOT      - Write output file in Motorola binary format\n");
  printf ("/WAV      - Write output file in WAVE format (default for audio)\n");
  printf ("e.g. CDCLIP /START=10:14:36 /END=13:55:11 /WAV TEST.WAV\n");
  printf ("e.g. CDCLIP /START=106232 /END=124443 /BIN TEST.BIN\n");
  printf ("e.g. CDCLIP /TRACK=5 /ID=0:3:0 /SPEED=4 TRACK5.WAV\n");
}

//
// Parse command line arguments
//

static void ParseCommandLine(int argc, char *argv[])
{
  extern BOOL env_cdreader_flag;
  extern UBYTE env_cdreader_adapter;
  extern UBYTE env_cdreader_id;
  extern UBYTE env_cdreader_lun;

  int i;
  char *argP, uparg[80];

  SLONG lba;
  UWORD min, sec, frm, adapter, id, lun;

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

      else if (! strcmp (uparg, "/BATCH"))
        log_flag = FALSE;

      else if (! strcmp (uparg, "/CDG"))
        cdg_flag = TRUE;

      else if (! strcmp (uparg, "/SUBQ"))
        subq_flag = TRUE;

      else if (! strcmp (uparg, "/DATA"))
        data_flag = TRUE;

      else if (! strcmp (uparg, "/JC"))
        jitter_mode = JITTER_MODE_ENABLE;

      else if (! strcmp (uparg, "/NOJC"))
        jitter_mode = JITTER_MODE_DISABLE;

      else if (! strcmp (uparg, "/IGNORE"))
        error_mode = READERROR_MODE_REPLACE;

      else if (! strcmp (uparg, "/AIF"))
        filetype = FILETYPE_AIFF_STEREO;

      else if (! strcmp (uparg, "/BIN"))
        filetype = FILETYPE_BINARY_LSB;

      else if (! strcmp (uparg, "/MOT"))
        filetype = FILETYPE_BINARY_MSB;

      else if (! strcmp (uparg, "/WAV"))
        filetype = FILETYPE_WAVE_STEREO;

      else if (! strncmp (uparg, "/BLKLEN", 6))
        {
        if (sscanf (uparg, "/BLKLEN=%hu", &data_blklen) != 1) {
          fprintf (stderr, "Error: Illegal /BLKLEN format!\n");
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

      else if (! strncmp (uparg, "/TRACK", 6))
        {
        if (sscanf (uparg, "/TRACK=%hu", &track_number) != 1) {
          fprintf (stderr, "Error: Illegal /TRACK value format!\n");
          exit (1);
          }

        if ((track_number < 1) || (track_number > 99)) {
          fprintf (stderr, "Error: Illegal /TRACK number specified!\n");
          exit (1);
          }
        }

      else if (! strncmp (uparg, "/START", 6))
        {
        if (sscanf (uparg, "/START=%hu:%hu:%hu", &min, &sec, &frm) == 3)
          {
          if ((min > 89) || (sec > 59) || (frm > 74) ||
              ((min == 0) && (sec < 2))) {
            fprintf (stderr, "Error: Illegal MM:SS:FF value specified\n");
            exit (1);
            }
          start_lba = ConvertMSF2LBA (min, sec, frm);
          }
        else if (sscanf (uparg, "/START=%lu", &lba) == 1)
          start_lba = lba;
        else {
          fprintf (stderr, "Error: Illegal /START value format!\n");
          exit (1);
          }
        }

      else if (! strncmp (uparg, "/END", 4))
        {
        if (sscanf (uparg, "/END=%hu:%hu:%hu", &min, &sec, &frm) == 3)
          {
          if ((min > 89) || (sec > 59) || (frm > 74) ||
              ((min == 0) && (sec < 2))) {
            fprintf (stderr, "Error: Illegal MM:SS:FF value specified!\n");
            exit (1);
            }
          end_lba = ConvertMSF2LBA (min, sec, frm);
          }
        else if (sscanf (uparg, "/END=%lu", &lba) == 1)
          end_lba = lba;
        else {
          fprintf (stderr, "Error: Illegal /END value format!\n");
          exit (1);
          }
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
          fprintf (stderr, "Error: Illegal /ID value format!\n");
          exit (1);
          }
        }

      else {
        fprintf (stderr, "Invalid switch - %s\n", argP);
        exit (1);
        }
      }

    // otherwise, it's probably the output filename.

    else
      {
      if (outfilnam == NULL)
        outfilnam = argP;
      else {
        fprintf (stderr, "Too many parameters - %s\n", argP);
        exit (1);
        }
      }
    }

  // Check for required filename.

  if (outfilnam == NULL) {
    fprintf (stderr, "Error: Output filename must be specified!\n");
    exit (1);
    }

  // Check for required /END value.

//  if (end_lba == 0x7FFFFFFF) {
//    fprintf (stderr, "Error: /END value must be specified\n");
//    exit (1);
//    }

  // Check /START and /END values.

  if (start_lba > end_lba) {
    fprintf (stderr, "Error: /START value is greater than /END value!\n");
    exit (1);
    }

  // Check for illegal switch combinations.

  if (data_flag && cdg_flag) {
    fprintf (stderr, "Error: Cannot use both /DATA and /CDG options\n");
    exit (1);
    }

  if (data_flag && (filetype == FILETYPE_AIFF_STEREO)) {
    fprintf (stderr, "Error: Cannot use both /DATA and /AIF options\n");
    exit (1);
    }

  if (data_flag && (filetype == FILETYPE_WAVE_STEREO)) {
    fprintf (stderr, "Error: Cannot use both /DATA and /WAV options\n");
    exit (1);
    }
}

//
// CDReader::ExtractSectors callback function
//

static void ExtractCallback(void *handle, void *bufP, UWORD blklen, UWORD blocks)
{
  FILEHANDLE filehandle = (FILEHANDLE)handle;

  if (filehandle != NULL)
    {
    // Compute the buffer length.
    ULONG buflen = (ULONG)blklen * (ULONG)blocks;

    // Write the sectors to the output file.
    if (! FileWrite (filehandle, bufP, buflen)) SignalException (E_WriteFile);
    }
}

//
// Main entry point
//

main(int argc, char *argv[])
{
  SLONG loc_start_lba, loc_end_lba;

  CDReader *cdreaderP;
  FILEHANDLE outfile;

  // Enable exception handling.

  EXCEPTION_HANDLER_START

  // Initialize and check the protection.

  ProtInitialize (NULL, FALSE);

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

    if (cdreaderP->GetModel() == GENERIC_CDROM_SCSI)
      {
      printf ("Warning: CDROM device model is unknown to this program!\n");
      printf ("The generic CDROM device driver will be used...\n\n");
      }
    }

  // Make sure the device supports CDDA transfer.

  if (! cdreaderP->IsFlagSet(CDReader::crfReadCDDA)) {
    fprintf (stderr,
      "Error: CDROM drive does not support the reading of CDDA (audio)!\n");
    exit (1);
    }

  // Make sure the device supports CD+G transfer.

  if (cdg_flag && (! cdreaderP->IsFlagSet(CDReader::crfReadCDG))) {
    fprintf (stderr,
      "Error: CDROM drive does not support the reading of CD+G subcodes!\n");
    exit (1);
    }

  // Make sure that the device is ready.

  cdreaderP->LoadDisc (TRUE);

  // If a track number was specified, find the starting and ending
  // LBAs for this track.

  if (track_number)
    {
    UWORD track_index;
    BOOL track_found = FALSE;

    DiscInfo *discinfoP = new DiscInfo (cdreaderP);

    for (track_index = 0; track_index < discinfoP->m_nTrackCount; track_index++)
      {
      UBYTE track = discinfoP->m_TrackInfo[track_index].track;
      if (track == track_number) {track_found = TRUE; break;}
      }

    if (! track_found) {
      fprintf (stderr, "Error: Specified track number (%u) does not exist!\n",
        track_number);
      exit (1);
      }

    loc_start_lba = discinfoP->m_TrackInfo[track_index].lba;
    loc_end_lba = discinfoP->m_TrackInfo[track_index + 1].lba - 1;

    delete discinfoP;
    }
  else
    {
    loc_start_lba = start_lba;

    // If an ending LBA was not specified, try to find the last readable
    // CDDA sector on the disc.

    if (end_lba == 0x7FFFFFFF)
      loc_end_lba = cdreaderP->FindLastAudioSector();
    else
      loc_end_lba = end_lba;
    }

  // Compute the amount of data to be read.

  if (cdg_flag) data_blklen = SECTOR_CDDA_SUBPW_BLKLEN;
  if (subq_flag) data_blklen = SECTOR_CDDA_SUBQ_BLKLEN;

  ULONG blkcnt = (loc_end_lba - loc_start_lba) + 1;
  ULONG datalen = blkcnt * data_blklen;

  // If a filetype wasn't specified...

  if (filetype == FILETYPE_UNKNOWN)
    {
    if (data_flag || cdg_flag || subq_flag)
      filetype = FILETYPE_BINARY_LSB;
    else
      filetype = FILETYPE_WAVE_STEREO;
    }

  // Create the output file.

  if ((outfile = FileCreate (outfilnam)) == NULL) {
    fprintf (stderr, "Error creating output file \"%s\"\n", outfilnam);
    exit (1);
    }

  // If necessary, write the audio file header.

  if (filetype == FILETYPE_WAVE_STEREO)
    {
    WAVEHEADER waveheader;
    InitFileHeaderWAVE (&waveheader, datalen);
    FileWrite (outfile, &waveheader, sizeof(WAVEHEADER));
    }
  else if (filetype == FILETYPE_AIFF_STEREO)
    {
    AIFFHEADER aiffheader;
    InitFileHeaderAIFF (&aiffheader, datalen);
    FileWrite (outfile, &aiffheader, sizeof(AIFFHEADER));
    }

  // Set the reading speed.

  cdreaderP->SetSpindleSpeed (read_speed, data_flag, FALSE);
  
  // Extract the sectors from the disc into the output file.

  UBYTE datatype = (data_flag ? DATATYPE_CDROM : DATATYPE_CDDA);
  BOOL flip_flag = ((filetype == FILETYPE_AIFF_STEREO) || (filetype == FILETYPE_BINARY_MSB));

  cdreaderP->ExtractSectors (
    datatype, data_blklen, loc_start_lba, blkcnt, ExtractCallback, outfile,
    jitter_mode, 0, error_mode, flip_flag, FALSE, log_flag);

  // Close the output file.

  FileClose (outfile);

  // Shutdown the ASPI manager.

  ASPIAdapter::ShutdownManager();

  // End exception handling.

  EXCEPTION_HANDLER_EXIT

  return (0);
}

