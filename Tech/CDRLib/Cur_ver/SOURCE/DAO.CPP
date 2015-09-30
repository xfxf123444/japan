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
#include "protect.h"

// External variables.

extern unsigned _stklen = 16384;  // Stack length.

extern UBYTE ActualSpeedTable[];

// Protection information.

extern CINFO gControlFileInfo;    // Control file information.
extern BOOL gbAppUnlocked;        // Application unlocked?

// Command line parameters.

static char *cue_filnam = NULL;
static char *log_filnam = NULL;

static UBYTE cdwriter_adapter, cdwriter_id, cdwriter_lun;
static BOOL cdwriter_id_specified = FALSE;

static UWORD copy_count = 1;
static UWORD record_speed = SPEED_MAX;

static UWORD cache_size = 0;

static BOOL beep_flag = FALSE;
static BOOL confirm_flag = TRUE;
static BOOL eject_flag = FALSE;
static BOOL log_flag = TRUE;
static BOOL multisession_flag = FALSE;
static BOOL raw_flag = FALSE;
static BOOL transporter_flag = FALSE;
static BOOL test_flag = FALSE;
static BOOL underrun_protect_flag = FALSE;

// Function prototypes.

void ParseCommandLine(int argc, char *argv[]);
void PrintUsage(void);
void UnlockProgram(void);

//
// Print command line usage
//

void PrintUsage()
{
  printf ("DAO.EXE - Version %s (%s)\n", PRODUCT_VERSION, PRODUCT_RELEASE_DATE);
  printf ("%s\n\n", COPYRIGHT_MESSAGE);

  // Print registration information.

  if (gControlFileInfo.authtype == UNLOCKED)
    {
    printf ("This software is registered to...\n");
    printf ("  Name   : %s\n", gControlFileInfo.user_name);
    printf ("  Company: %s\n", gControlFileInfo.user_company);
    printf ("  Date   : %02u/%02u/%04u\n\n",
      gControlFileInfo.registration_date.month,
      gControlFileInfo.registration_date.day,
      gControlFileInfo.registration_date.year);
    }

  printf ("Usage: DAO <cuefile> [logfile] [/ID=ha:id:lun] [/BATCH] [/BEEP]\n");
  printf ("         [/CACHE=n] [/COPIES=n] [/EJECT] [/NOCONFIRM] [/NOUNDERRUN]\n");
  printf ("         [/RAW] [/SESSION] [SPEED=n] [/TEST] [/TRANSPORT] [/UNLOCK]\n");
  printf ("cuefile     - Cue sheet filename\n");
  printf ("logfile     - Disc layout log filename (optional)\n");
  printf ("/ID         - ASPI ID of CD recorder (default is first found)\n");
  printf ("/BATCH      - Disable all messages and confirmation prompts\n");
  printf ("/BEEP       - Issue an audible 'beep' when recording has finished\n");
  printf ("/CACHE=n    - Cache size in megabytes (32-bit version only)\n");
  printf ("/COPIES=n   - Number of discs to be recorded (default is 1)\n");
  printf ("/EJECT      - Eject CD when recording has finished\n");
  printf ("/NOCONFIRM  - Disable all confirmation prompts\n");
  printf ("/NOUNDERRUN - Enable buffer underrun protection (if supported by recorder)\n");
  printf ("/RAW        - Enable RAW recording mode (if supported by recorder)\n");
  printf ("/SESSION    - Enable session-at-once recording (default is disc-at-once)\n");
  printf ("/SPEED=n    - Recording speed (default is maximum)\n");
  printf ("/TEST       - Enable test recording mode\n");
  printf ("/TRANSPORT  - Use KODAK disc transporter\n");
  printf ("/UNLOCK     - Unlock all programs from demonstration mode\n");
}

//
// Parse DOS command line.
//

void ParseCommandLine(int argc, char *argv[])
{
  extern BOOL env_cdwriter_flag;
  extern UBYTE env_cdwriter_adapter;
  extern UBYTE env_cdwriter_id;
  extern UBYTE env_cdwriter_lun;

  int i;
  char *argP, uparg[80];

  UWORD adapter, id, lun;

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

      else if (! strcmp (uparg, "/BATCH")) {
        log_flag = FALSE;
        confirm_flag = FALSE;
        }

      else if (! strcmp (uparg, "/BEEP"))
        beep_flag = TRUE;

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

      else if (! strncmp (uparg, "/CACHE", 6))
        {
        if (sscanf (uparg, "/CACHE=%hu", &cache_size) != 1) {
          fprintf (stderr, "Error: Illegal /CACHE format!\n");
          exit (1);
          }

        if ((cache_size < 1)  || (cache_size > 32)) {
          fprintf (stderr, "Error: Illegal cache size specified!\n");
          exit (1);
          }
        }

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

      else if (! strcmp (uparg, "/NOUNDERRUN"))
        underrun_protect_flag = TRUE;

      else if (! strcmp (uparg, "/RAW"))
        raw_flag = TRUE;

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

      else if (! strcmp (uparg, "/TRANSPORT"))
        transporter_flag = TRUE;

      else if (! strcmp (uparg, "/UNLOCK")) {
        UnlockProgram ();
        exit (0);
        }

      else {
        fprintf (stderr, "Invalid switch - %s\n", argP);
        exit (1);
        }
      }

    // otherwise, this is probably one of the filename parameters...

    else
      {
      if (cue_filnam == NULL)
        cue_filnam = argP;
      else if (log_filnam == NULL)
        log_filnam = argP;
      else {
        fprintf (stderr, "Too many parameters - %s\n", argP);
        exit (1);
        }
      }
    }

  // Make sure that the CUE SHEET filename was specified.

  if (cue_filnam == NULL) {
    fprintf (stderr, "Error: CUE SHEET filename must be specified!\n");
    exit (1);
    }
}

//
// Attempt to unlock the program.
//

void UnlockProgram()
{
  int status;

  char name[64], company[64], keystr[64];

  AUTHCODE primarykey, secondarykey;

  // If the program is already unlocked, simply return.

  if (gControlFileInfo.authtype == 'U') {
    fprintf (stderr, "Error: Program is already unlocked!\n");
    exit (1);
    }

  // Prompt for user information.

  printf ("**************************************************************\n");
  printf ("* This procedure will UNLOCK all programs from demo mode...  *\n");
  printf ("* All data must be entered exactly as given to you or this   *\n");
  printf ("* procedure will fail!!                                      *\n");
  printf ("**************************************************************\n");
  printf ("\n");

  printf ("Username      ? ");
  gets (name);

  printf ("Company/Email ? ");
  gets (company);

  printf ("Unlock Key    ? ");
  gets (keystr);

  status = sscanf (
    keystr, "%lx-%lx-%lx-%lx",
    &primarykey.longvec[3], &primarykey.longvec[2],
    &primarykey.longvec[1], &primarykey.longvec[0]);

  if (status != 4)
    {fprintf (stderr, "\nError: Illegal key format!\n"); exit(1);}

  printf ("Check Key     ? ");
  gets (keystr);

  status = sscanf (
    keystr, "%lx-%lx-%lx-%lx",
    &secondarykey.longvec[3], &secondarykey.longvec[2],
    &secondarykey.longvec[1], &secondarykey.longvec[0]);

  if (status != 4)
    {fprintf (stderr, "\nError: Illegal key format!\n"); exit(1);}

  printf ("\n");

  // Unlock the software.

  ProtUnlock (name, company, &primarykey, &secondarykey);

  // Inform the user that the program is now unlocked.

  printf ("Welcome to DAO...\n");
  printf ("The software is now unlocked and fully functional!\n");
}

//
// Main routine
//

main(int argc, char *argv[])
{
  UWORD display_speed;
  MSF msf;

  CDWriter *cdwriterP;
  CDExchanger *exchdevP;
  auto_ptr<DiscTransporter> transporterP;

  // Enable exception handling.

  EXCEPTION_HANDLER_START

  // Initialize and check the protection.

  ProtInitialize (NULL, TRUE, TRUE);

  // Set the test recording flag if the application is still locked.

  test_flag = (! gbAppUnlocked);
  
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

  if (log_flag)
    {
    printf ("CD-Recorder device found...\n");
    printf ("  HA #%u - ASPI ID #%u - %-8s %-16s %-4s\n\n",
      cdwriterP->GetAdapter(), cdwriterP->GetId(),
      cdwriterP->GetVendorId(), cdwriterP->GetProductId(),
      cdwriterP->GetFirmwareLevel());
    }

  // Make sure the recorder supports "disc-at-once".

  if (! cdwriterP->IsFlagSet(CDWriter::cwfDiscAtOnce)) {
    fprintf (stderr,
      "Error: CD-Recorder does not support 'disc-at-once' recording!\n");
    exit (1);
    }

  // Using a disc transporter?

  if (transporter_flag)
    {
    // Find a known disc exchanger device on the SCSI bus.

    if ((exchdevP = (CDExchanger *)ASPIAdapter::FindDeviceObject (ASPI_M_DEVTYPE_CHANGER)) == NULL) {
      fprintf (stderr,
        "\nError: Unable to find a known Disc-Transporter device!\n");
      exit (1);
      }

    if (log_flag)
      {
      printf ("Disc-Transporter device found...\n");
      printf ("  HA #%u - SCSI ID #%u - %-8s %-16s %-4s\n\n",
        exchdevP->GetAdapter(), exchdevP->GetId(),
        exchdevP->GetVendorId(), exchdevP->GetProductId(),
        exchdevP->GetFirmwareLevel());
      }

    // Create a disc transporter object.

    transporterP.reset (new DiscTransporterSCSI(exchdevP, cdwriterP));
    }
  
  // Parse the cuesheet file.

  auto_ptr<CueSheet> cuesheetP(new CueSheet(cue_filnam, TRUE));

  // Display cuesheet statistics.

  if (log_flag)
    {
    printf ("\nThe cuesheet has been successfully compiled...\n");
    printf ("  Number of files     : %03u\n", cuesheetP->file_count);
    printf ("  Number of tracks    : %03u\n", cuesheetP->track_count);
    printf ("  Number of cuepoints : %03u\n", cuesheetP->cuepoint_count - 2);
    }

  // Check the disc layout.

  if (log_flag)
    {
    // Does the layout contain short tracks?

    if (! cuesheetP->disclayoutP->Check())
      {
      fprintf (stderr, "\nWARNING! Disc layout contains one or more tracks that are shorter\n");
      fprintf (stderr, "than four seconds. This is a violation of the CDROM specification.\n");
      }

    // Will the data fit on an 80 minute disc?

    if (cuesheetP->disc_high_lba > CDROM_80MIN_BLKCNT)
      {
      MSF msf;
      ConvertLBA2MSF (cuesheetP->disc_high_lba, &msf);
      fprintf (stderr, "\nWARNING! Total disc time (%02u:%02u:%02u) exceeds 80 minutes.\n",
        msf.minutes, msf.seconds, msf.frames);
      }
    }

  // Create the disc layout log file?

  if (log_filnam != NULL)
    cuesheetP->disclayoutP->Display (log_filnam, FALSE, FALSE);
  
  // Determine the actual recording speed.

  record_speed = MIN (record_speed, cdwriterP->GetMaxWriteSpeed());
  display_speed = ActualSpeedTable[record_speed];

  // Print statistics.

  if (log_flag)
    {
    ULONG write_blkcnt = cuesheetP->disc_high_lba + 20000;

    ULONG blocks_per_min = 60 * 75 * display_speed;
    UWORD record_min = CDIV (write_blkcnt, blocks_per_min);

    ConvertLBA2MSF (cuesheetP->disc_high_lba, &msf);

    printf ("\nTotal disc time is %02u:%02u:%02u\n",
      msf.minutes, msf.seconds, msf.frames);

    printf ("Approximate recording time at %ux speed is %u minutes\n",
      display_speed, record_min);
    }

  // Build a disc-at-once context block.

  auto_ptr<DAOCB> daocbP(new DAOCB);

  MEMCLEAR (daocbP.get(), sizeof(DAOCB));

  daocbP->disclayoutP = cuesheetP->disclayoutP;
  daocbP->filecb_vec = cuesheetP->filecb_vec;
  daocbP->file_count = cuesheetP->file_count;
  daocbP->recordoptions.speed = record_speed;
  daocbP->recordoptions.beep_flag = beep_flag;
  daocbP->recordoptions.eject_flag = (transporter_flag ? FALSE : eject_flag);
  daocbP->recordoptions.log_flag = log_flag;
  daocbP->recordoptions.multisession_flag = multisession_flag;
  daocbP->recordoptions.raw_flag = raw_flag;
  daocbP->recordoptions.test_flag = test_flag;
  daocbP->recordoptions.underrun_protect_flag = underrun_protect_flag;

  // Enable caching?

  #if defined(WIN32)
  if (cache_size != 0)
    {
    CacheThread::SetEnabled (TRUE);
    CacheThread::SetSize (cache_size);
    }
  #endif

  // Make copies...

  for (UWORD c = 1; c <= copy_count; c++)
    {
    // Prompt to begin...

    if (((confirm_flag || (copy_count > 1)) && (! transporter_flag)) ||
        (confirm_flag && transporter_flag && (c == 1)))
      {
      printf ("\n");
      if (test_flag) printf ("TEST recording mode is enabled!\n");
      printf ("Hit <ENTER> to begin recording (or CTRL/C to exit)...");
      getchar();
      }

    // Exchange disc?

    if (transporter_flag)
      {
      int mode = ((c == 1) ? EXCHANGE_MODE_FIRST : EXCHANGE_MODE_NEXT);
      printf ("\nLoading Disc #%u... Please Wait...", c);
      transporterP->ExchangeDisc (mode, log_flag, TRUE);
      printf ("\n");
      }

    printf ("\n");

    // Write the disc using disc-at-once recording.

    cdwriterP->RecordDiscAtOnce (daocbP.get());

    // Success.

    if (log_flag) printf ("\nCD successfully recorded!\n");
    }

  // Unload the final disc?

  if (transporter_flag)
    transporterP->ExchangeDisc (EXCHANGE_MODE_LAST, log_flag, TRUE);

  // Shutdown the ASPI manager.

  ASPIAdapter::ShutdownManager();

  // End exception handling.

  EXCEPTION_HANDLER_EXIT

  return (0);
}

