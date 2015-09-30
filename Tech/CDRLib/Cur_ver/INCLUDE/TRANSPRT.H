#if !defined(CDRLIB_TRANSPRT_H)
#define CDRLIB_TRANSPRT_H

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

#define TRANSPORTER_TYPE_SCSI   0
#define TRANSPORTER_TYPE_SERIAL 1

#define DISCMATIC_AD1050        0

// Exchange modes.

#define EXCHANGE_MODE_FIRST   0
#define EXCHANGE_MODE_NEXT    1
#define EXCHANGE_MODE_LAST    2

// Discmatic AD1050 command codes

#define DISCMATIC_LOAD_DISC       0xA0
#define DISCMATIC_UNLOAD_DISC     0xA1
#define DISCMATIC_PICKUP_BAD_DISC 0xA2
#define DISCMATIC_DROP_BAD_DISC   0xA3
#define DISCMATIC_STATUS_INQUIRY  0xA7
#define DISCMATIC_INITIALIZE      0xAA
#define DISCMATIC_VERSION_INQUIRY 0xAF

// Discmatic AD1050 status codes

#define DISCMATIC_ERROR           0x82
#define DISCMATIC_DOOR_OPEN       0x87
#define DISCMATIC_HOLDER_EMPTY    0x88
#define DISCMATIC_VALID_COMMAND   0x8A
#define DISCMATIC_INVALID_COMMAND 0x8B
#define DISCMATIC_BUSY            0x8D
#define DISCMATIC_NO_HOLDER       0x8E
#define DISCMATIC_NO_BULK         0x8F

//
// Medium exchange device class
//

class CDExchanger : public ASPIDevice {
  public:
    CDExchanger(ADCB *adcbP) : ASPIDevice(adcbP) {}

    // High level functions.
    int ExchangeMedium(int mode, BOOL sig_flag);

    // Functions that execute an ASPI command.
    int ExecuteExchangeMedium(
      UWORD element, UWORD source, UWORD target1, UWORD target2,
      BOOL inv1, BOOL inv2, UBYTE vendor);

  private:
    // EXCHANGE MEDIUM command.
    struct EXCHMEDIUM {
      UBYTE cmd;
      BFLAG : 5;
      BFLAG lun : 3;
      UBYTE element[2];
      UBYTE source[2];
      UBYTE target1[2];
      UBYTE target2[2];
      BFLAG inv2 : 1;
      BFLAG inv1 : 1;
      BFLAG : 6;
      CONTROL control;
      };
  };

//
// Disc transporter base class
//

class DiscTransporter {
  public:
    DiscTransporter(CDWriter *cdwriterP);
    virtual ~DiscTransporter();

    int ExchangeDisc(int mode, BOOL log_flag, BOOL sig_flag);

    virtual int Initialize() = 0;
    virtual int ExchangeDisc(int mode) = 0;
    virtual int RemoveBadDisc() = 0;

  protected:
    CDWriter *cdwriterP;
  };

//
// SCSI based disc transporter class
//

class DiscTransporterSCSI : public DiscTransporter {
  public:
    DiscTransporterSCSI(CDExchanger *cdexchangerP, CDWriter *cdwriterP);

    virtual int Initialize();
    virtual int ExchangeDisc(int mode);
    virtual int RemoveBadDisc();

  private:
    CDExchanger *cdexchangerP;
  };

//
// SERIAL port based disc transporter class
//

#if defined(WIN32)
class DiscTransporterSerial : public DiscTransporter {
  public:
    DiscTransporterSerial(UBYTE commport, ULONG baudrate, CDWriter *cdwriterP);
    ~DiscTransporterSerial();
    
    virtual int Initialize();
    virtual int ExchangeDisc(int mode);
    virtual int RemoveBadDisc();

  private:
    int ConvertStatus(int status);
    int ExecuteCommand(UBYTE command);
    int LoadDisc();
    int UnloadDisc();
    int PickupBadDisc();
    int DropBadDisc();

  private:
    HANDLE port_handle;
    ULONG baudrate;
    UBYTE commport;
    };
#endif

// Restore previous packing alignment.

#pragma pack(pop)

#endif

