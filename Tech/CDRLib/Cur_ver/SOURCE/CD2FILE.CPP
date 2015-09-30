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

extern unsigned _stklen = 8192;

// Function prototypes.

void PrintUsage(void);
void ParseCommandLine(int argc, char *argv[]);

// Command line parameters.

static char *image_filnam = NULL;

static UBYTE cdreader_adapter = 0, cdreader_id = 0, cdreader_lun = 0;
static cdreader_id_specified = FALSE;

static UBYTE datatype = DATATYPE_CDROM;

static BOOL confirm_flag = TRUE;
static BOOL crc_flag = FALSE;
static BOOL log_flag = TRUE;
//
// Print command line usage
//

void PrintUsage()
{
  printf ("CD2FILE.EXE - Version %s (%s)\n", PRODUCT_VERSION, PRODUCT_RELEASE_DATE);
  printf ("%s\n\n", COPYRIGHT_MESSAGE);
  printf ("Usage: CD2FILE <imagefile> [/ID=ha:id:lun] [/NOCONFIRM]\n");
  printf ("imagefile  - Output image filename\n");
  printf ("/ID        - ASPI ID of CDROM device (default is first found)\n");
  printf ("/BATCH     - Disable all messages and confirmation prompts\n");
//  printf ("/CRC       - Compute CRC of image file data\n");
  printf ("/NOCONFIRM - Disable all confirmation prompts\n");
}

//
// Parse command line arguments
//

void ParseCommandLine(int argc, char *argv[])
{
  extern BOOL env_cdreader_flag;
  extern UBYTE env_cdreader_adapter;
  extern UBYTE env_cdreader_id;
  extern UBYTE env_cdreader_lun;

  int i;
  char *argP, uparg[80];

  UWORD adapter, id, lun;

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
          fprintf (stderr, "Error: Illegal /ID format\n");
          exit (1);
          }
        }

      else if (! strcmp (uparg, "/BATCH")) {
        log_flag = FALSE;
        confirm_flag = FALSE;
        }

      else if (! strcmp (uparg, "/CRC"))
        crc_flag = TRUE;

      else if (! strcmp (uparg, "/NOCONFIRM"))
        confirm_flag = FALSE;

      else {
        fprintf (stderr, "Invalid switch - %s\n", argP);
        exit (1);
        }
      }

    // otherwise, this is probably the filename parameter...

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

  // Make sure that the required filename was specified.

  if (image_filnam == NULL) {
    fprintf (stderr, "Error: IMAGE DATA filename must be specified\n");
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
    printf ("  HA #%u - ASPI ID #%u - %-8s %-16s %-4s\n",
      cdreaderP->GetAdapter(), cdreaderP->GetId(),
      cdreaderP->GetVendorId(), cdreaderP->GetProductId(),
      cdreaderP->GetFirmwareLevel());
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

  // Determine the # of blocks on the CDROM.

  ULONG blkcnt = cdreaderP->FindLastDataSector() + 1;
  UWORD blklen = SECTOR_MODE1_BLKLEN;

  if (blkcnt > CDROM_80MIN_BLKCNT)
    printf ("\nWARNING: Disc is longer than 80 minutes!\n"); 

  if (log_flag)
    printf ("\nCDROM contains %luMb of data (%lu blocks)\n",
      CDIV (blkcnt * blklen, 0x100000), blkcnt);

  // Calculate the number of blocks per I/O.

  UWORD blocks_per_io = ASPIAdapter::MaxBufferSize / blklen;

  // Prompt to begin...

  if (confirm_flag) {
    printf ("\nHit <ENTER> to begin copy (or CTRL/C to exit)...");
    getchar ();
    }

  // Create the image file.

  if ((image_file = FileCreate(image_filnam)) == NULL) {
    fprintf (stderr, "\nError creating image file \"%s\"\n", image_filnam);
    exit (1);
    }

  // Allocate an I/O buffer (automatically deallocated).

  IOBuffer iobuffer(sizeof(IOBUF));
  IOBUF *iobufP = (IOBUF *)iobuffer.GetHandle();

  // Enable CRC calculation?

  if (crc_flag)
    {
    cdreaderP->SetFlag (CDReader::crfComputeCRC);
    cdreaderP->SetCRC (0xFFFFFFFF);
    }

  // Set the reading mode.

  cdreaderP->SetBlockLength (DATATYPE_CDROM, blklen, TRUE);

  // Copy the CDROM to the image file.

  ULONG blocks_written = 0;
  UWORD cur_percent = 0;

  if (log_flag) {
    printf ("\nCopying source disc to image file...\n");
    printf (" 0%% completed.\r");
    }

  for (SLONG lba = 0; lba < blkcnt; lba += blocks_per_io)
    {
    UWORD blocks = MIN (blocks_per_io, blkcnt - lba);

    // Read the CDROM.

    cdreaderP->ReadDiscData (iobufP->data, blklen, blocks, lba, TRUE);

    // Write the output file.

    if (! FileWrite (image_file, iobufP->data, blocks * blklen)) {
      fprintf (stderr, "\nUnexpected error writing image file (disk full?)!\n");
      exit (1);
      }

    // Increment the number of blocks written.

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

  if (log_flag) printf ("Copy completed!    \n");

  // Close the image file.

  FileClose (image_file);

  // Finish the CRC calculation?

  if (crc_flag)
    {
    cdreaderP->SetCRC (cdreaderP->GetCRC() ^ 0xFFFFFFFF);
    cdreaderP->ClearFlag (CDReader::crfComputeCRC);
    printf ("\nImage file CRC = %08lX\n", cdreaderP->GetCRC());
    }

  // Shutdown the ASPI manager.

  ASPIAdapter::ShutdownManager();

  // End exception handling.

  EXCEPTION_HANDLER_EXIT

  return (0);
}

