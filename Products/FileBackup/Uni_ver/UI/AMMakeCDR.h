#if !defined(AFX_AMMAKECDR_H__B87A46CA_93F7_45A5_8D47_E7BC78545BB6__INCLUDED_)
#define AFX_AMMAKECDR_H__B87A46CA_93F7_45A5_8D47_E7BC78545BB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AMMakeCDR.h : header file
//
#include "..\..\..\..\tech\cdrlib\Cur_ver\include\cdrlib.h"

#define WM_MYMESSAGE (WM_USER + 1)

//Disk Image related structures
#define SECTORS_PER_CLUSTER		32
#define PARTITION_START			0x3F
#define	HEADS					0xFF
#define SECTORS_PER_TRACK		0x3F
#define	MIN_FAT32_SECTORS		32*1024*1024/512
#define	MIN_FAT16_SECTORS		32*1024*1024/512
#define	RESERVED_SECTORS		37	// This number + PARTITION_START must be 2048 aligned
									// This is required to access files in the partition
									// using iso9660
//#define	RESERVED_SPACE			4*CDSCT+100*SECTOR_SIZE
// ISO9660 uses the first 0x16 CD sectors.
#define	RESERVED_SPACE			0x17*CDSCT

#define SECTOR_SIZE		512

#define VD_LBN				16
#define BRVD_LBN			17
#define CDSCT				2048L
#define	BOOT_IMG_LBA		0x17


#define	BOOTABLE_CD_RESERVED_SYS_SPACE	(BOOT_IMG_LBA*CDSCT+CDSCT)
#define	BOOTABLE_IMG_RESERVED_SYS_SPACE	(PARTITION_START*SECTOR_SIZE\
										+RESERVED_SECTORS*SECTOR_SIZE\
										+2*0x10000+SECTORS_PER_CLUSTER*SECTOR_SIZE\
										+SECTORS_PER_TRACK*HEADS*SECTOR_SIZE)


typedef BOOL (WINAPI *P_GDFSE)(LPCTSTR,
							   PULARGE_INTEGER,
                               PULARGE_INTEGER,
							   PULARGE_INTEGER);

int DisplayMessage(HWND hWnd,DWORD dwMessage,DWORD dwTitle,DWORD dwType);
void GetSendMessage(HWND hWnd);
BOOL MakeCD(CDWriter *cdwriterP,CString csFile);

BOOL NewMakeCD(CDWriter *cdwriterP,CString csFile);

BOOL File2CD(CDWriter *cdwriterP,CString csSelected,CProgressCtrl *pProgress,CStatic *pStateMsg);
BOOL GetSepName(CString csFile,char *szFile);
BOOL GetVolumeFreeSpace(char cDrive,__int64 *pi64Count,__int64 *pFreeBytes);

/////////////////////////////////////////////////////////////////////////////
// CAMMakeCDR dialog

class CAMMakeCDR : public CPropertyPage
{
	DECLARE_DYNCREATE(CAMMakeCDR)

// Construction
public:
	void DisableControl();
	CAMMakeCDR();
	~CAMMakeCDR();

// Dialog Data
	//{{AFX_DATA(CAMMakeCDR)
	enum { IDD = IDD_MAKE_CDR };
	CButton	m_CancleButton;
	CButton	m_RefFile;
	CButton	m_CreateCD;
	CStatic	m_StateMsg;
	CEdit	m_FileEdit;
	CProgressCtrl	m_Progress;
	CString	m_csFileName;
	BOOL	m_bBootable;
	CDWriter *m_cdWriterP;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAMMakeCDR)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAMMakeCDR)
	afx_msg void OnCreateCD();
	afx_msg void OnOpenImgFile();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	virtual void OnCancel();
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMMAKECDR_H__B87A46CA_93F7_45A5_8D47_E7BC78545BB6__INCLUDED_)
