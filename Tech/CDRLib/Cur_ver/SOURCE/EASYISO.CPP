// 08-Sep-04 (sjk) Support new CDFSOPTIONS: iso9660_character_set and iso9660_level. Also do some casting.

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
#include <new.h>

#include "cdrlib.h"
#include "iso9660.h"
#include "prodver.h"
#include "protect.h"

// Stack size.

extern unsigned _stklen = 32768;

// Command line parameters.

static char *pathname = NULL;
static char *image_filnam = NULL;

static char volume_id[64] = {0};

static UBYTE cdwriter_adapter, cdwriter_id, cdwriter_lun;
static BOOL cdwriter_id_specified = FALSE;

static UBYTE cdfs_type = CDFS_TYPE_ISO9660;
static UBYTE iso9660_level = ISO9660_LEVEL1;
static UBYTE iso9660_character_set = ISO9660_CHARSET_DOS;

static UWORD record_speed = SPEED_MAX;
static UWORD cache_size = 0;

static BOOL archive_only_flag = FALSE;
static BOOL beep_flag = FALSE;
static BOOL close_session_flag = TRUE;
static BOOL confirm_flag = TRUE;
static BOOL eject_flag = FALSE;
static BOOL fullpath_flag = FALSE;
static BOOL hidden_flag = FALSE;
static BOOL import_flag = FALSE;
static BOOL joliet_flag = FALSE;
static BOOL log_flag = TRUE;
static BOOL mode2_flag = FALSE;
static BOOL multisession_flag = FALSE;
static BOOL noversion_flag = FALSE;
static BOOL postgap_flag = TRUE;
static BOOL recurse_flag = FALSE;
static BOOL system_flag = FALSE;
static BOOL test_flag = FALSE;
static BOOL underrun_protect_flag = FALSE;
static BOOL verify_flag = FALSE;

// Function prototypes.

static void PrintUsage(void);
static void ParseCommandLine(int argc, char *argv[]);

//
// Print command usage.
// 

static void PrintUsage()
{
  printf ("EASYISO.EXE - Version %s (%s)\n", PRODUCT_VERSION, PRODUCT_RELEASE_DATE);
  printf ("%s\n\n", COPYRIGHT_MESSAGE);
  printf ("Usage: EASYISO <pathname or @listfile> [imagefile] [/ID=ha:id:lun]\n");
  printf ("         [/BATCH] [/BEEP] [/CACHE=n] [/CHARSET=type] [/EJECT]\n");
  printf ("         [/FILESYSTEM=type] [/FULLPATH] [/IMPORT] [/JOLIET] [/LEVEL2]\n");
  printf ("         [/MODE2] [/NOCONFIRM] [/NOFINALIZE] [/NOUNDERRUN] [/NOVERSION]\n");
  printf ("         [/RECURSE] [/SESSION] [/SPEED=n] [/TEST] [/VERIFY] [/VOLUME=label]\n");
  printf ("         [/ARCHIVE] [/HIDDEN] [/SYSTEM]\n");

  printf ("<hit any key to continue>\r");
  getch();

  printf ("pathname    - Directory pathname to backup\n");
  printf ("@listfile   - File that contains a list of pathnames to backup\n");
  printf ("imagefile   - Output image filename (optional)\n");
  printf ("/ID         - ASPI ID of CD recorder (default is first found)\n");
  printf ("/BATCH      - Disable all logging messages\n");
  printf ("/BEEP       - Issue an audible 'beep' when recording has finished\n");
  printf ("/CACHE=n    - Cache size in megabytes (32-bit version only)\n");
  printf ("/CHARSET    - Name translation character set (ISO9660, DOS, or ASCII)\n");
  printf ("/EJECT      - Eject CD when recording has finished\n");
  printf ("/FILESYSTEM - CDROM filesystem type (ISO9660, UDF, or UDFISO)\n");
  printf ("/FULLPATH   - Preserve full pathnames\n");
  printf ("/IMPORT     - Import previous session from disc\n");
  printf ("/JOLIET     - Enable Joliet long filename support (32-bit version only)\n");
  printf ("/LEVEL2     - Enable ISO9660 Level 2 filenames (31 characters long)\n");
  printf ("/MODE2      - Write sectors in MODE2 format\n");
  printf ("/NOCONFIRM  - Disable all confirmation prompts\n");
  printf ("/NOFINALIZE - Disable session/disc finalization\n");
  printf ("/NOUNDERRUN - Enable buffer underrun protection (if supported by recorder)\n");
  printf ("/NOVERSION  - Disable filename version numbers\n");
  printf ("/RECURSE    - Recurse all subdirectories\n");
  printf ("/SESSION    - Open new session after finalizing current session\n");
  printf ("/SPEED=n    - Recording speed (default is maximum)\n");
  printf ("/TEST       - Enable test recording mode\n");
  printf ("/VERIFY     - Verify disc after recording\n");
  printf ("/VOLUME     - Volume label (maximum 32 characters)\n");
  printf ("/ARCHIVE    - Include \"archive\" files only\n");
  printf ("/HIDDEN     - Include \"hidden\" files\n");
  printf ("/SYSTEM     - Include \"system\" files\n");
  printf ("e.g. COPY2CD C:\\MYFILES\\ /JOLIET /TEST /ID=0:5:0\n");
  printf ("e.g. COPY2CD @DIRLIST.TXT TEST.ISO /IMPORT /RECURSE /VOLUME=MY_FILES\n");
}

//
// Parse the command line.
//

static void ParseCommandLine(int argc, char *argv[])
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
          cdwriter_adapter = (unsigned char)adapter;
          cdwriter_id = (unsigned char)id;
          cdwriter_lun = (unsigned char)lun;
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

      else if (! strncmp (uparg, "/CHARSET", 8))
        {
        char charset_str[80];

        if (sscanf (uparg, "/CHARSET=%s", &charset_str) != 1) {
          fprintf (stderr, "Error: Illegal /CHARSET format!\n");
          exit (1);
          }

        if (! stricmp(charset_str, "ISO9660"))
          iso9660_character_set = ISO9660_CHARSET_STANDARD;
        else if (! stricmp(charset_str, "DOS"))
          iso9660_character_set = ISO9660_CHARSET_DOS;
        else if (! stricmp(charset_str, "ASCII1"))
          iso9660_character_set = ISO9660_CHARSET_ASCII;
        else
          {
          fprintf (stderr, "Illegal /CHARSET option \"%s\" specified!\n", charset_str);
          exit (1);
          }
        }

      else if (! strncmp (uparg, "/FILESYSTEM", 11))
        {
        char cdfs_str[80];

        if (sscanf (uparg, "/FILESYSTEM=%s", &cdfs_str) != 1) {
          fprintf (stderr, "Error: Illegal /FILESYSTEM format!\n");
          exit (1);
          }

        if (! stricmp(cdfs_str, "ISO9660"))
          cdfs_type = CDFS_TYPE_ISO9660;
        else if (! stricmp(cdfs_str, "UDFISO"))
          cdfs_type = CDFS_TYPE_UDFBRIDGE;
        else if (! stricmp(cdfs_str, "UDF"))
          cdfs_type = CDFS_TYPE_UDF;
        else
          {
          fprintf (stderr, "Illegal /FILESYSTEM option \"%s\" specified!\n", cdfs_str);
          exit (1);
          }
        }

      else if (! strcmp (uparg, "/EJECT"))
        eject_flag = TRUE;

      else if (! strcmp (uparg, "/FULLPATH"))
        fullpath_flag = TRUE;

      else if (! strcmp (uparg, "/IMPORT"))
        import_flag = TRUE;

      else if (! strcmp (uparg, "/JOLIET"))
        {
        #if defined(WIN32)
          joliet_flag = TRUE;
        #else
          fprintf (stderr, "Error: /JOLIET option is not allowed for 16-bit DOS.\n");
          exit (1);
        #endif
        }

      else if (! strcmp (uparg, "/LEVEL2"))
        iso9660_level = ISO9660_LEVEL2;

      else if (! strcmp (uparg, "/MODE2"))
        mode2_flag = TRUE;

      else if (! strcmp (uparg, "/NOCONFIRM"))
        confirm_flag = FALSE;

      else if (! strcmp (uparg, "/NOFINALIZE"))
        close_session_flag = FALSE;

      else if (! strcmp (uparg, "/NOUNDERRUN"))
        underrun_protect_flag = TRUE;

      else if (! strcmp (uparg, "/NOVERSION"))
        noversion_flag = TRUE;

      else if (! strcmp (uparg, "/RECURSE"))
        recurse_flag = TRUE;

      else if (! strcmp (uparg, "/SESSION"))
        multisession_flag = TRUE;

      else if (! strcmp (uparg, "/ARCHIVE"))
        archive_only_flag = TRUE;

      else if (! strcmp (uparg, "/HIDDEN"))
        hidden_flag = TRUE;

      else if (! strcmp (uparg, "/SYSTEM"))
        system_flag = TRUE;

      else if (! strcmp (uparg, "/TEST"))
        test_flag = TRUE;

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

      else if (! strcmp (uparg, "/VERIFY"))
        verify_flag = TRUE;

      else if (! strncmp (uparg, "/VOLUME", 7))
        {
        if (sscanf (uparg, "/VOLUME=%s", volume_id) != 1) {
          fprintf (stderr, "Error: Illegal /VOLUME format!\n");
          exit (1);
          }

        if (strlen(volume_id) > 32) {
          fprintf (stderr, "Error: Volume label is too long!\n");
          exit (1);
          }

        // Uppercase the volume ID.

        strupr (volume_id);
        }

      else {
        fprintf (stderr, "Invalid switch - %s\n", argP);
        exit (1);
        }
      }

    // otherwise, it's probably one of the parameters.

    else
      {
      if (pathname == NULL)
        pathname = argP;
      else if (image_filnam == NULL)
        image_filnam = argP;
      else
        {
        fprintf (stderr, "Too many parameters - %s\n", argP);
        exit (1);
        }
      }
    }

  // Check for required parameters.

  if (pathname == NULL)
    {
    fprintf (stderr, "Error: Directory pathname must be specified!\n");
    exit (1);
    }
}

//
// Main entry point
//

main(int argc, char *argv[])
{
  CDWriter *cdwriterP;

  DIRPATHDESC pathdesc_vec[100];
  int desc_count = 0;

  // Set the memory allocation failure handler.

  #if defined(WIN32)
  _set_new_handler (ConsoleNewHandler);
  #endif

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

  // Make sure the recorder supports "track-at-once" recording.

  if (! cdwriterP->IsFlagSet(CDWriter::cwfTrackAtOnce)) {
    fprintf (stderr,
      "Error: CD-Recorder does not support track-at-once recording!\n");
    exit (1);
    }

  // Build a list of pathname descriptors...

  if (pathname[0] != '@')
    {
    DIRPATHDESC *descP = &pathdesc_vec[0];

    char *new_pathnameP = new char[strlen(pathname) + 1];
    StringCopy (new_pathnameP, pathname);

    descP->pathnameP = new_pathnameP;
    descP->out_pathnameP = NULL;
    descP->fullpath_flag = fullpath_flag;
    descP->recurse_flag = recurse_flag;
    descP->hidden_flag = hidden_flag;
    descP->system_flag = system_flag;
    descP->archive_only_flag = archive_only_flag;
    
    desc_count = 1;
    }
  else
    {
    FILE *file;
    char dirname[256];

    // Open the list file.

    if ((file = fopen (&pathname[1], "r")) == NULL) {
      fprintf (stderr, "Error: Unable to open file \"%s\"\n", &pathname[1]);
      exit (1);
      }

    // Build the directory path descriptor list.

    DIRPATHDESC *descP = &pathdesc_vec[0];

    while (fgets(dirname, 256, file) != NULL)
      {
      char dummy[256];

      // Skip blank lines.

      int status = sscanf (dirname, "%s", dummy);
      if ((status == 0) || (status == EOF)) continue;

      // Too many directories?

      if (desc_count == 100) {
        fprintf (stderr, "Error: Too many directories/files specified\n");
        exit (1);
        }

      // Strip off the line terminator.

      int dirname_len = strlen(dirname) - 1;
      dirname[dirname_len] = '\0';

      // Build the directory path descriptor.

      char *new_pathnameP = new char[dirname_len + 1];
      StringCopy (new_pathnameP, dirname);

      descP->pathnameP = new_pathnameP;
      descP->out_pathnameP = NULL;
      descP->fullpath_flag = fullpath_flag;
      descP->recurse_flag = recurse_flag;
      descP->hidden_flag = hidden_flag;
      descP->system_flag = system_flag;
      descP->archive_only_flag = archive_only_flag;
      
      desc_count++;
      descP++;
      }

    // Close the list file.

    fclose (file);

    // Make sure that at least one pathname was specified.

    if (desc_count == 0) {
      fprintf (stderr, "Error: File contains no directory names\n");
      exit (1);
      }
    }

  // Initialize the ISO9660 image options.

  CDFSOPTIONS cdfs_options;
  MEMCLEAR (&cdfs_options, sizeof(CDFSOPTIONS));

  cdfs_options.filesystem_type = cdfs_type;
  cdfs_options.joliet_flag = joliet_flag;
  cdfs_options.noversion_flag = noversion_flag;
  cdfs_options.file_date_option = CDFS_FILEDATE_ORIGINAL;
  cdfs_options.iso9660_level = iso9660_level;
  cdfs_options.iso9660_character_set = iso9660_character_set;

  StringCopy (cdfs_options.volume_id, volume_id);

  // Initialize the recording options.

  RECORDOPTIONS recordopt;
  MEMCLEAR (&recordopt, sizeof(RECORDOPTIONS));

  recordopt.speed = (unsigned char)record_speed;
  recordopt.mode2_flag = mode2_flag;
  recordopt.postgap_flag = postgap_flag;
  recordopt.close_session_flag = close_session_flag;
  recordopt.multisession_flag = multisession_flag;
  recordopt.test_flag = test_flag;
  recordopt.verify_flag = verify_flag;
  recordopt.underrun_protect_flag = underrun_protect_flag;
  recordopt.beep_flag = beep_flag;
  recordopt.eject_flag = eject_flag;
  recordopt.log_flag = log_flag;

  // Enable caching?

  #if defined(WIN32)
  if (cache_size != 0)
    {
    CacheThread::SetEnabled (TRUE);
    CacheThread::SetSize ((unsigned char)cache_size);
    }
  #endif

  // Prompt user to start?

  if (confirm_flag)
    {
    if (test_flag) printf ("TEST recording mode is enabled!\n");
    printf ("Hit <ENTER> to begin recording (or CTRL/C to exit)...");
    getchar();
    printf ("\n");
    }

  // Record the disc.

  cdwriterP->RecordDirectoryPaths (
    pathdesc_vec, desc_count, image_filnam, &cdfs_options, &recordopt, import_flag);

  // Free the directory pathnames.

  for (int i = 0; i < desc_count; i++)
    delete []((char *)pathdesc_vec[i].pathnameP);

  // Success.

  if (log_flag) printf ("\nCD successfully recorded!\n");

  // Shutdown the ASPI manager.

  ASPIAdapter::ShutdownManager();

  // End exception handling.

  EXCEPTION_HANDLER_EXIT

  return (0);
}
