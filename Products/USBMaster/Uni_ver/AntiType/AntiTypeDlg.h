// AntiTypeDlg.h : header file
//

#if !defined(AFX_ANTITYPEDLG_H__549E05C9_25DB_4F7D_942F_A9FC6F89845F__INCLUDED_)
#define AFX_ANTITYPEDLG_H__549E05C9_25DB_4F7D_942F_A9FC6F89845F__INCLUDED_
#define WM_MY_MESSAGE (WM_USER + 101)
#define WM_NOTIFY_ICON (WM_USER + 102)
#define WM_DEVICE_ARRIVED (WM_USER + 103)
#define WM_DEVICE_REMOVED (WM_USER + 104)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAntiTypeDlg dialog

#include "SystemTray.h"

extern CString driverName;
extern CString mirrorDriverName;
//extern BOOL g_mountOrDismount;
void SaveDriverInfo(PDRIVE_MAP pDriveMap);
void DismountDrive();
#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

//typedef enum STORAGE_BUS_TYPE {
//    BusTypeUnknown = 0x00,
//    BusTypeScsi,
//    BusTypeAtapi,
//    BusTypeAta,
//    BusType1394,
//    BusTypeSsa,
//    BusTypeFibre,
//    BusTypeUsb,
//    BusTypeRAID,
//    BusTypeMaxReserved = 0x7F
//} STORAGE_BUS_TYPE, *PSTORAGE_BUS_TYPE;

// retrieve the storage device descriptor data for a device. 
typedef struct STORAGE_DEVICE_DESCRIPTOR {
  ULONG  Version;
  ULONG  Size;
  UCHAR  DeviceType;
  UCHAR  DeviceTypeModifier;
  BOOLEAN  RemovableMedia;
  BOOLEAN  CommandQueueing;
  ULONG  VendorIdOffset;
  ULONG  ProductIdOffset;
  ULONG  ProductRevisionOffset;
  ULONG  SerialNumberOffset;
  STORAGE_BUS_TYPE  BusType;
  ULONG  RawPropertiesLength;
  UCHAR  RawDeviceProperties[1];

} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;

// retrieve the properties of a storage device or adapter. 
typedef enum STORAGE_QUERY_TYPE {
  PropertyStandardQuery = 0,
  PropertyExistsQuery,
  PropertyMaskQuery,
  PropertyQueryMaxDefined

} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;

// retrieve the properties of a storage device or adapter. 
typedef enum STORAGE_PROPERTY_ID {
  StorageDeviceProperty = 0,
  StorageAdapterProperty,
  StorageDeviceIdProperty

} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;

// retrieve the properties of a storage device or adapter. 
typedef struct STORAGE_PROPERTY_QUERY {
  STORAGE_PROPERTY_ID  PropertyId;
  STORAGE_QUERY_TYPE  QueryType;
  UCHAR  AdditionalParameters[1];

} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;

typedef struct  {
	BYTE btOriginalDriver;
	BYTE btMirrorDriver;
}ORIGINALDRIVER_MIRRORDRIVER,*PORIGINALDRIVER_MIRRORDRIVER;

typedef struct  {
	char szSaveInfoPath[MAX_PATH];
	char szOriginalDriver;
}THREAD_PARAM,*PTHREAD_PARAM;

class CAntiTypeDlg : public CDialog
{
// Construction
public:
	CAntiTypeDlg(CWnd* pParent = NULL);	// standard constructor

public:
	BOOL UpdateDriverList();
	CSystemTray *m_pTray;

// Dialog Data
	//{{AFX_DATA(CAntiTypeDlg)
	enum { IDD = IDD_ANTITYPE_DIALOG };
	CListCtrl	m_removableDriverList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAntiTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAntiTypeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEncrypt();
	afx_msg void OnDecrypt();
	afx_msg void OnButtonDismountAll();
	afx_msg void OnButtonMount();
	afx_msg void OnButtonChangePassword();
	afx_msg void OnIconShowWindow();
	afx_msg void OnIconExit();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg BOOL OnDeviceChange(UINT wParam, DWORD_PTR lParam);
	afx_msg LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceArrived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceRemoved(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANTITYPEDLG_H__549E05C9_25DB_4F7D_942F_A9FC6F89845F__INCLUDED_)
