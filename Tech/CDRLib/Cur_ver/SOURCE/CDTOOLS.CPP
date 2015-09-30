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

extern unsigned _stklen = 8192;

#define CDTOOLS_COMMAND_UNKNOWN     0
#define CDTOOLS_COMMAND_DEVICES     1
#define CDTOOLS_COMMAND_EJECT       2
#define CDTOOLS_COMMAND_ERASE       3
#define CDTOOLS_COMMAND_FORMAT      4
#define CDTOOLS_COMMAND_FINALIZE    5
#define CDTOOLS_COMMAND_LOAD        6
#define CDTOOLS_COMMAND_TOC         7
#define CDTOOLS_COMMAND_DISCINFO    8

// Function prototypes.

void PrintUsage(void);
void ParseCommandLine(int argc, char *argv[]);

// Command line parameters.

static UBYTE cdrom_adapter, cdrom_id, cdrom_lun;
static BOOL cdrom_id_specified = FALSE;

static int command = CDTOOLS_COMMAND_UNKNOWN;

// Common options.

static BOOL confirm_flag = TRUE;
static BOOL log_flag = TRUE;
static BOOL test_flag = FALSE;
static UWORD record_speed = SPEED_MAX;

// Erase options.

static BOOL full_flag = FALSE;

// Finalize options.

static UBYTE finalize_datatype = DATATYPE_CDROM;
static BOOL multisession_flag = FALSE;

// Format options.

static ULONG format_blkcnt = 0xFFFFFFFF;
static ULONG format_packet_size = CDRW_DEF_PACKET_SIZE;

// TOC options.

static BOOL mcn_isrc_flag = FALSE;
static BOOL mode_flag = FALSE;

//
// Print command line usage
//

void PrintUsage()
{
  printf ("CDTOOLS.EXE - Version %s (%s)\n", PRODUCT_VERSION, PRODUCT_RELEASE_DATE);
  printf ("%s\n\n", COPYRIGHT_MESSAGE);
  printf ("Usage: CDTOOLS [/DEVICES] [/DISCINFO] [/EJECT] [/ERASE] [/FINALIZE]\n");
  printf ("         [/FORMAT] [/LOAD] [/TOC] [/BATCH] [/ID=ha:id:lun] [/NOCONFIRM]\n");
  printf ("         [/SPEED=n] [options...]\n");
  printf ("One of the following commands must be specified...\n");
  printf ("  /DEVICES   - Display a list of installed CDROM devices\n");
  printf ("  /DISCINFO  - Display the status of a disc\n");
  printf ("  /EJECT     - Eject a disc\n");
  printf ("  /ERASE     - Erase a re-writiable disc\n");
  printf ("  /FINALIZE  - Finalize session/disc\n");
  printf ("  /FORMAT    - Format a re-writable disc\n");
  printf ("  /LOAD      - Load a disc\n");
  printf ("  /TOC       - Display the table-of-contents of a disc\n");

  printf ("<hit any key to continue>\r");
  getch();

  printf ("Common Options...\n");
  printf ("  /BATCH     - Disable all messages and confirmation prompts\n");
  printf ("  /ID        - ID of CDROM device (default is first found)\n");
  printf ("  /NOCONFIRM - Disable all confirmation prompts\n");
  printf ("  /SPEED=n   - Recording speed (default is maximum)\n");
  printf ("Erase Options...\n");
  printf ("  /FULL      - Full erase\n");
  printf ("  /QUICK     - Quick erase (default)\n");
  printf ("Finalize Options...\n");
  printf ("  /CDDA      - Close session/disc as CDDA\n");
  printf ("  /CDROM     - Close session/disc as CDROM (default)\n");
  printf ("  /CDROMXA   - Close session/disc as CDROM-XA\n");
  printf ("  /SESSION   - Open new session after closing current one\n");
  printf ("Format Options...\n");
  printf ("  /BLOCKS=n  - Number of blocks to format (default is the entire disc)\n");
  printf ("  /PACKET=n  - Number of blocks per packet (default is 32 for CD-RW)\n");
  printf ("TOC Options...\n");
  printf ("  /ISRC      - Display the track ISRCs and disc MCN\n");
  printf ("  /MODE      - Display the sector mode of all data tracks\n");
}

//
// Parse command line arguments.
//

void ParseCommandLine(int argc, char *argv[])
{
  int i;
  char *argP, uparg[80];

  UWORD adapter, id, lun;

  // Print command usage?

  if (argc < 2)
    {
    PrintUsage();
    exit(0);
    }

  // Get the primary command option...

  StringCopy (uparg, argv[1]);
  strupr (uparg);

  if (! strcmp (uparg, "/DEVICES"))
    command = CDTOOLS_COMMAND_DEVICES;

  else if (! strcmp (uparg, "/DISCINFO"))
    command = CDTOOLS_COMMAND_DISCINFO;

  else if (! strcmp (uparg, "/EJECT"))
    command = CDTOOLS_COMMAND_EJECT;

  else if (! strcmp (uparg, "/ERASE"))
    command = CDTOOLS_COMMAND_ERASE;

  else if (! strcmp (uparg, "/FINALIZE"))
    command = CDTOOLS_COMMAND_FINALIZE;

  else if (! strcmp (uparg, "/FORMAT"))
    command = CDTOOLS_COMMAND_FORMAT;

  else if (! strcmp (uparg, "/LOAD"))
    command = CDTOOLS_COMMAND_LOAD;

  else if (! strcmp (uparg, "/TOC"))
    command = CDTOOLS_COMMAND_TOC;

  else {
    fprintf (stderr, "Illegal command specified - %s\n", argv[1]);
    exit (1);
    }

  // Parse the other options...

  for (i = 2; i < argc; i++)
    {
    argP = argv[i];

    StringCopy (uparg, argP);
    strupr (uparg);

    // Is this a switch?

    if (uparg[0] == '/')
      {
      // Common options...

      if (! strcmp (uparg, "/BATCH")) {
        log_flag = FALSE;
        confirm_flag = FALSE;
        }

      else if (! strcmp (uparg, "/NOCONFIRM"))
        confirm_flag = FALSE;

      else if (! strcmp (uparg, "/TEST"))
        test_flag = TRUE;

      else if (! strncmp (uparg, "/ID", 3))
        {
        if (sscanf (uparg, "/ID=%hu:%hu:%hu", &adapter, &id, &lun) == 3)
          {
          cdrom_adapter = adapter;
          cdrom_id = id;
          cdrom_lun = lun;
          cdrom_id_specified = TRUE;
          }
        else {
          fprintf (stderr, "Error: Illegal /ID format!\n");
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

      // Erase options...

      else if (! strcmp (uparg, "/FULL"))
        full_flag = TRUE;

      else if (! strcmp (uparg, "/QUICK"))
        full_flag = FALSE;

      // Finalize options...

      else if (! strcmp (uparg, "/CDDA"))
        finalize_datatype = DATATYPE_CDDA;

      else if (! strcmp (uparg, "/CDROM"))
        finalize_datatype = DATATYPE_CDROM;

      else if (! strcmp (uparg, "/CDROMXA"))
        finalize_datatype = DATATYPE_CDROMXA;

      else if (! strcmp (uparg, "/SESSION"))
        multisession_flag = TRUE;

      // Format options...

      else if (! strncmp (uparg, "/BLOCKS", 7))
        {
        ULONG blkcnt;

        if (sscanf (uparg, "/BLOCKS=%lu", &blkcnt) != 1) {
          fprintf (stderr, "Error: Illegal /BLOCKS format!\n");
          exit (1);
          }

        format_blkcnt = blkcnt;
        }

      else if (! strncmp (uparg, "/PACKET", 7))
        {
        ULONG packet_size;

        if (sscanf (uparg, "/PACKET=%lu", &packet_size) != 1) {
          fprintf (stderr, "Error: Illegal /PACKET format!\n");
          exit (1);
          }

        format_packet_size = packet_size;
        }

      // TOC options...

      else if (! strcmp (uparg, "/ISRC"))
        mcn_isrc_flag = TRUE;

      else if (! strcmp (uparg, "/MODE"))
        mode_flag = TRUE;

      // Invalid option specified...

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
  CDReader *cdreaderP;
  CDWriter *cdwriterP;

  CDReader *devlist[64];
  int device_count = 0;

  // Enable exception handling.

  EXCEPTION_HANDLER_START

  // Parse the command line arguments.

  ParseCommandLine (argc, argv);

  // Register event callback function.

  EventRegisterCallback (ConsoleEventCallback);

  // Startup the ASPI manager.

  ASPIAdapter::StartupManager (FALSE, FALSE, TRUE);

  switch (command)
    {
    // Commands that need a list of all CDROM devices...

    case CDTOOLS_COMMAND_DEVICES:
      {
      device_count = CDReader::GetDeviceObjects (devlist, 64);
      break;
      }

    // Commands that need a CD Reader...

    case CDTOOLS_COMMAND_EJECT:
    case CDTOOLS_COMMAND_LOAD:
    case CDTOOLS_COMMAND_TOC:
      {
      // Find a CDROM reader device...

      if (cdrom_id_specified)
        {
        if ((cdreaderP = (CDReader *)ASPIAdapter::FindDeviceObject (
              ASPI_M_DEVTYPE_CDROM | ASPI_M_DEVTYPE_WORM,
              cdrom_adapter, cdrom_id, cdrom_lun)) == NULL)
          {
          fprintf (stderr,
            "\nError: Specified device (%u:%u:%u) is not a CDROM!\n",
            cdrom_adapter, cdrom_id, cdrom_lun);
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

      break;
      }

    // Commands that need a CD Writer...

    case CDTOOLS_COMMAND_DISCINFO:
    case CDTOOLS_COMMAND_ERASE:
    case CDTOOLS_COMMAND_FINALIZE:
    case CDTOOLS_COMMAND_FORMAT:
      {
      // Find a CDROM writer device...

      if (cdrom_id_specified)
        {
        if ((cdwriterP = (CDWriter *)ASPIAdapter::FindDeviceObject (
              ASPI_M_DEVTYPE_WORM, cdrom_adapter, cdrom_id, cdrom_lun)) == NULL)
          {
          fprintf (stderr,
            "\nError: Specified device (%u:%u:%u) is not a CD-Recorder or is unknown!\n",
            cdrom_adapter, cdrom_id, cdrom_lun);
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

      break;
      }
    }

  //
  // Execute the command
  //

  switch (command)
    {
    // Eject a disc.

    case CDTOOLS_COMMAND_EJECT:
      cdreaderP->EjectDisc(TRUE);
      break;

    // Load a disc.

    case CDTOOLS_COMMAND_LOAD:
      cdreaderP->LoadDisc(TRUE);
      break;

    // Display a list of all CDROM devices.

    case CDTOOLS_COMMAND_DEVICES:
      {
      if (device_count == 0)
        printf ("No CDROM devices were found!\n");
      else
        {
        printf ("Installed Device Adapters...\n");

        for (int a = 0; a < ASPIAdapter::AdapterCount; a++)
          {
          ASPIAdapter *adapterP = ASPIAdapter::AdapterPtr[a];

          if (adapterP != NULL)
            {
            printf ("  %u:%u %hs\n",
              adapterP->GetAdapterNumber(), adapterP->GetAspiId(), adapterP->GetAdapterName());
            }
          }

        printf ("\n");

        printf ("Installed CDROM/DVD Devices...\n");

        for (int d = 0; d < device_count; d++)
          {
          CDReader *cdreaderP = devlist[d];
          
          printf ("  %u:%u:%u %-8s %-16s %-4s\n",
            cdreaderP->GetAdapter(), cdreaderP->GetId(), cdreaderP->GetLogUnitNum(),
            cdreaderP->GetVendorId(), cdreaderP->GetProductId(),
            cdreaderP->GetFirmwareLevel());
          }
        }

      break;
      }

    // Display a disc's table-of-contents.

    case CDTOOLS_COMMAND_TOC:
      {
      SLONG lba;
      MSF msf;

      // Make sure that the device is ready.

      cdreaderP->LoadDisc (TRUE);

      // Construct a DISCINFO object.

      DiscInfo *discinfoP = new DiscInfo (cdreaderP, mode_flag, mcn_isrc_flag, FALSE);

      // Dump the MCN?

      if (mcn_isrc_flag)
        {
        printf ("Media Catalog Number: ");

        if (discinfoP->m_bValidMCN)
          printf ("%.13s\n", &discinfoP->m_MCN);
        else
          printf ("Not Available\n");
        }

      // Display the TOC.

      printf ("                                  Copy       Pre      Four\n");
      printf ("Trk  Type   Min:Sec:Frm   LBA   Permitted  Emphasis  Channel      ISRC\n");
      printf ("---  ----   -----------   ---   ---------  --------  -------      ----\n");

      for (int i = 0; i < discinfoP->m_nTrackCount; i++)
        {
        TRACKINFO *trackinfoP = &discinfoP->m_TrackInfo[i];
        char *typestr;

        // Prompt to continue?

        if (confirm_flag && (i != 0) && ((i % 12) == 0)) {
          printf ("<hit any key to continue>\r");
          getch();
          }

        // Convert the LBA to MSF.

        lba = trackinfoP->lba;
        ConvertLBA2MSF (lba, &msf);

        // Get the track type.

        switch (trackinfoP->tracktype)
          {
          case TRACKTYPE_CDDA: typestr = "Audio"; break;
          case TRACKTYPE_DATA : typestr = "Data";  break;
          case TRACKTYPE_MODE0: typestr = "Mode0"; break;
          case TRACKTYPE_MODE1: typestr = "Mode1"; break;
          case TRACKTYPE_MODE2: typestr = "Mode2"; break;
          case TRACKTYPE_CDI  : typestr = "CD-I";  break;
          }

        // Display the track information.

        printf ("%02u   %-5s   %02u:%02u:%02u    %06lu    %3s       %3s       %3s     ",
          trackinfoP->track, typestr, msf.minutes, msf.seconds, msf.frames, lba,
          (trackinfoP->copy_permitted ? "Yes" : "No"),
          (trackinfoP->pre_emphasis   ? "Yes" : "No"),
          (trackinfoP->four_channel   ? "Yes" : "No"));

        // Display the ISRC.

        if (mcn_isrc_flag && trackinfoP->isrc_valid)
          printf ("%.12s\n", &trackinfoP->isrc);
        else
          printf ("Not Available\n");
        }

      // Display the leadout track time.

      lba = discinfoP->m_nLeadoutLBA;
      ConvertLBA2MSF (lba, &msf);

      printf ("\nLeadout: %02u:%02u:%02u (LBA %06lu)\n",
        msf.minutes, msf.seconds, msf.frames, lba);

      // Free the DISCINFO object.

      delete discinfoP;

      break;
      }

    // Erase a disc (CD-RW).

    case CDTOOLS_COMMAND_ERASE:
      {
      // Make sure that this is a re-writable drive.
      if ((cdwriterP->GetWriteDiscTypeMask() & DISCTYPE_M_CDRW) == 0)
        {
        fprintf (stderr,
          "Error: CD-Recorder device does not support REWRITABLE discs!\n");
        exit (1);
        }

      // Prompt to begin...
      if (confirm_flag)
        {
        printf ("WARNING!! All data on the disc will be destroyed!!\n\n");
        printf ("Hit <ENTER> to ERASE the disc (or CTRL/C to exit)...");
        getchar ();
        }

      // Make sure that the device is ready.
      cdwriterP->LoadDisc (TRUE);

      // Set the recording parameters.
      cdwriterP->SetWriteParameters (record_speed, FALSE);

      // Erase the disc.
      cdwriterP->EraseDisc (full_flag, log_flag, TRUE);

      // Success!
      if (log_flag) printf ("Disc successfully erased!\n");

      break;
      }

    // Finalize a disc.

    case CDTOOLS_COMMAND_FINALIZE:
      {
      // Check that the CD-R model is supported.

      if ((cdwriterP->GetModel() == JVC_XRW2001) ||
          (cdwriterP->GetModel() == JVC_XRW2010) ||
          (cdwriterP->GetModel() == JVC_XRW2020) ||
          (cdwriterP->GetModel() == JVC_XRW2040))
        {
        fprintf (stderr,
          "Error: CD-Recorder model is not supported by this program!\n");
        exit (1);
        }

      // Prompt to begin...
      if (confirm_flag) {
        printf ("Hit <ENTER> to FINALIZE the disc (or CTRL/C to exit)...");
        getchar ();
        }

      // Make sure that the device is ready.
      cdwriterP->LoadDisc (TRUE);

      // Set the recording parameters.
      cdwriterP->SetWriteParameters (record_speed, test_flag, TRUE);

      // Close the session (finalize).
      cdwriterP->CloseSession (finalize_datatype, multisession_flag, log_flag, TRUE);

      // Success!
      if (log_flag) printf ("Disc successfully finalized!\n");

      break;
      }

    // Format a disc.

    case CDTOOLS_COMMAND_FORMAT:
      {
      // Make sure that this is a re-writable drive.
      if ((cdwriterP->GetWriteDiscTypeMask() & DISCTYPE_M_CDRW) == 0)
        {
        fprintf (stderr,
          "Error: CD-Recorder device does not support REWRITABLE discs!\n");
        exit (1);
        }

      // Prompt to begin...
      if (confirm_flag)
        {
        printf ("WARNING!! Data on the disc will be destroyed!!\n\n");
        printf ("Hit <ENTER> to FORMAT the disc (or CTRL/C to exit)...");
        getchar ();
        }

      // Make sure that the device is ready.
      cdwriterP->LoadDisc (TRUE);

      // Set the recording parameters.
      cdwriterP->SetWriteParameters (record_speed, FALSE);

      // Format the disc.
      cdwriterP->FormatDisc (format_blkcnt, format_packet_size, log_flag, TRUE);

      // Success!
      if (log_flag) printf ("Disc successfully formatted!\n");

      break;
      }

    // Disc information.

    case CDTOOLS_COMMAND_DISCINFO:
      {
      int status;

      int write_status;
      int disc_type;
      MSF msf;
      SLONG lba;
      ULONG freespace;
      ULONG blocks;

      // Make sure that the device is ready.

      cdwriterP->LoadDisc (TRUE);

      // Get the disc type.

      cdwriterP->GetDiscType (&disc_type);

      printf ("Disc Type              = ");

      switch (disc_type)
        {
        case DISCTYPE_CDROM:    printf ("CDROM\n"); break;
        case DISCTYPE_CDR:      printf ("CD-R\n"); break;
        case DISCTYPE_CDRW:     printf ("CD-RW\n"); break;
        case DISCTYPE_CDROM_DD: printf ("CDROM/DD\n"); break;
        case DISCTYPE_CDR_DD:   printf ("CD-R/DD\n"); break;
        case DISCTYPE_CDRW_DD:  printf ("CD-RW/DD\n"); break;
        case DISCTYPE_DVDROM:   printf ("DVDROM\n"); break;
        case DISCTYPE_DVDRAM:   printf ("DVDRAM\n"); break;
        case DISCTYPE_DVDR:     printf ("DVD-R\n"); break;
        case DISCTYPE_DVDRW:    printf ("DVD-RW\n"); break;
        case DISCTYPE_DVDPLUSR: printf ("DVD+R\n"); break;
        case DISCTYPE_DVDPLUSRW:printf ("DVD+RW\n"); break;
        default:                printf ("Unknown\n");
        }

      if (disc_type != DISCTYPE_UNKNOWN)
        {
        // Get the write status of a disc.

        cdwriterP->GetDiscWriteStatus (&write_status);

        printf ("Disc Status            = ");

        switch (write_status)
          {
          case DISC_STATUS_EMPTY:     printf ("Empty\n"); break;
          case DISC_STATUS_WRITABLE:  printf ("Writable\n"); break;
          case DISC_STATUS_COMPLETE:  printf ("Complete\n"); break;
          default:                    printf ("Unknown\n");
          }

        // Get the manufacturer's code for a CD-R or CD-RW disc. The code is stored
        // as a time value in minutes:seconds:frames. Unique time values are assigned
        // to each disc manufacturer.

        if ((disc_type == DISCTYPE_CDR) || (disc_type == DISCTYPE_CDRW))
          {
          if (cdwriterP->GetDiscManufacturerCode (&msf))
            printf ("Disc Manufacturer Code = %02u-%02u-%02u\n",
              msf.minutes, msf.seconds, msf.frames);
          }

        // Get the maximum capacity of the disc in blocks.

        if (cdwriterP->GetDiscMaximumCapacity (&blocks))
          printf ("Maximum Disc Capacity  = %lu blocks\n", blocks);
        else
          printf ("Maximum Disc Capacity  = Unknown\n");

        // Get the current capacity of the disc in blocks.

        if (cdwriterP->GetDiscCurrentCapacity (&blocks))
          printf ("Current Read Capacity  = %lu blocks\n", blocks);
        else
          printf ("Current Read Capacity  = Unknown\n");

        // Get writable disc information.

        if ((write_status == DISC_STATUS_EMPTY) || (write_status == DISC_STATUS_WRITABLE))
          {
          // Get the amount of freespace (in blocks) remaining on the disc.

          if (cdwriterP->GetDiscFreespace (&freespace))
            printf ("Disc Freespace         = %lu blocks\n", freespace);
          else
            printf ("Disc Freespace         = Unknown\n");

          // Get the next writable address for track-at-once mode.

          status = cdwriterP->GetNextWritableAddress (WRITEMODE_TRACKATONCE, &lba);
          if (status == E_Success) printf ("Track-at-once Next LBA = %ld\n", lba);

          // Get the next writable address for disc-at-once mode.

          status = cdwriterP->GetNextWritableAddress (WRITEMODE_DISCATONCE, &lba);
          if (status == E_Success) printf ("Disc-at-once Next LBA  = %ld\n", lba);
          }
        }

      break;
      }
    }

  // Shutdown the ASPI manager.

  ASPIAdapter::ShutdownManager();

  // End exception handling.

  EXCEPTION_HANDLER_EXIT

  return (0);
}

