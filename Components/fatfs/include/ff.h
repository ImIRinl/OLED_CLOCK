/*----------------------------------------------------------------------------/
/  FatFs - Generic FAT Filesystem module  R0.14                               /
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2019, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:

/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/----------------------------------------------------------------------------*/

#ifndef FF_DEFINED
#define FF_DEFINED	86606	/* Revision ID */

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_types.h"
#include "ffconf.h"		/* FatFs configuration options */





/* Definitions of volume management */

#if FF_MULTI_PARTITION		/* Multiple partition configuration */
typedef struct {
	BYTE pd;	/* Physical drive number */
	BYTE pt;	/* Partition: 0:Auto detect, 1-4:Forced partition) */
} PARTITION;
extern PARTITION VolToPart[];	/* Volume - Partition mapping table */
#endif

#if FF_STR_VOLUME_ID
#ifndef FF_VOLUME_STRS
extern const char* VolumeStr[FF_VOLUMES];	/* User defied volume ID */
#endif
#endif


/* �й�FatFs API��·�������ַ������� */

#ifndef _INC_TCHAR
#define _INC_TCHAR

#if FF_USE_LFN && FF_LFN_UNICODE == 1 	/* Unicode in UTF-16 encoding */
typedef WCHAR TCHAR;
#define _T(x) L ## x
#define _TEXT(x) L ## x
#elif FF_USE_LFN && FF_LFN_UNICODE == 2	/* Unicode in UTF-8 encoding */
typedef char TCHAR;
#define _T(x) u8 ## x
#define _TEXT(x) u8 ## x
#elif FF_USE_LFN && FF_LFN_UNICODE == 3	/* Unicode in UTF-32 encoding */
typedef DWORD TCHAR;
#define _T(x) U ## x
#define _TEXT(x) U ## x
#elif FF_USE_LFN && (FF_LFN_UNICODE < 0 || FF_LFN_UNICODE > 3)
#error Wrong FF_LFN_UNICODE setting
#else									/* ANSI/OEM code in SBCS/DBCS */
typedef char TCHAR;
#define _T(x) x
#define _TEXT(x) x
#endif

#endif





/* �ļ�ϵͳ����ṹ�� (FATFS) */

typedef struct {
	BYTE	fs_type;		/* �ļ�ϵͳ���� (0:û����) */
	BYTE	pdrv;			/* ����������� */
	BYTE	n_fats;			/* FATs��(1 or 2) */
	BYTE	wflag;			/* win[] ��־ (b0:dirty) */
	BYTE	fsi_flag;		/* FSINFO ��־ (b7:disabled, b0:dirty) */
	WORD	id;				/* Volume mount ID */
	WORD	n_rootdir;		/* ��Ŀ¼��Ŀ�� (FAT12/16) */
	WORD	csize;			/* �ش�С*/
#if FF_MAX_SS != FF_MIN_SS
	WORD	ssize;			/* ������С (512, 1024, 2048 or 4096) */
#endif
#if FF_USE_LFN
	WCHAR*	lfnbuf;			/* LFN ���������� */
#endif
#if FF_FS_EXFAT
	BYTE*	dirbuf;			/* Directory entry block scratchpad buffer for exFAT */
#endif
#if FF_FS_REENTRANT
	FF_SYNC_t	sobj;		/* Identifier of sync object */
#endif
#if !FF_FS_READONLY
	DWORD	last_clst;		/* �ϴη���Ĵ� */
	DWORD	free_clst;		/* ���д� */
#endif
#if FF_FS_RPATH
	DWORD	cdir;			/* Current directory start cluster (0:root) */
#if FF_FS_EXFAT
	DWORD	cdc_scl;		/* Containing directory start cluster (invalid when cdir is 0) */
	DWORD	cdc_size;		/* b31-b8:Size of containing directory, b7-b0: Chain status */
	DWORD	cdc_ofs;		/* Offset in the containing directory (invalid when cdir is 0) */
#endif
#endif
	DWORD	n_fatent;		/* Number of FAT entries (number of clusters + 2) */
	DWORD	fsize;			/* Size of an FAT [sectors] */
	LBA_t	volbase;		/* Volume base sector */
	LBA_t	fatbase;		/* FAT ������ */
	LBA_t	dirbase;		/* Root directory base sector/cluster */
	LBA_t	database;		/* Data base sector */
#if FF_FS_EXFAT
	LBA_t	bitbase;		/* Allocation bitmap base sector */
#endif
	LBA_t	winsect;		/* Current sector appearing in the win[] */
	BYTE	win[FF_MAX_SS];	/* Disk access window for Directory, FAT (and file data at tiny cfg) */
} FATFS;



/* ����ID�ͷ�����Ϣ (FFOBJID) */

typedef struct {
	FATFS*	fs;				/* Pointer to the hosting volume of this object */
	WORD	id;				/* Hosting volume mount ID */
	BYTE	attr;			/* Object attribute */
	BYTE	stat;			/* Object chain status (b1-0: =0:not contiguous, =2:contiguous, =3:fragmented in this session, b2:sub-directory stretched) */
	DWORD	sclust;			/* Object data start cluster (0:no cluster or root directory) */
	FSIZE_t	objsize;		/* Object size (valid when sclust != 0) */
#if FF_FS_EXFAT
	DWORD	n_cont;			/* Size of first fragment - 1 (valid when stat == 3) */
	DWORD	n_frag;			/* Size of last fragment needs to be written to FAT (valid when not zero) */
	DWORD	c_scl;			/* Containing directory start cluster (valid when sclust != 0) */
	DWORD	c_size;			/* b31-b8:Size of containing directory, b7-b0: Chain status (valid when c_scl != 0) */
	DWORD	c_ofs;			/* Offset in the containing directory (valid when file object and sclust != 0) */
#endif
#if FF_FS_LOCK
	UINT	lockid;			/* File lock ID origin from 1 (index of file semaphore table Files[]) */
#endif
} FFOBJID;



/* File object structure (FIL) */

typedef struct {
	FFOBJID	obj;			/* Object identifier (must be the 1st member to detect invalid object pointer) */
	BYTE	flag;			/* File status flags */
	BYTE	err;			/* Abort flag (error code) */
	FSIZE_t	fptr;			/* File read/write pointer (Zeroed on file open) */
	DWORD	clust;			/* Current cluster of fpter (invalid when fptr is 0) */
	LBA_t	sect;			/* Sector number appearing in buf[] (0:invalid) */
#if !FF_FS_READONLY
	LBA_t	dir_sect;		/* Sector number containing the directory entry (not used at exFAT) */
	BYTE*	dir_ptr;		/* Pointer to the directory entry in the win[] (not used at exFAT) */
#endif
#if FF_USE_FASTSEEK
	DWORD*	cltbl;			/* Pointer to the cluster link map table (nulled on open, set by application) */
#endif
#if !FF_FS_TINY
	BYTE	buf[FF_MAX_SS];	/* File private data read/write window */
#endif
} FIL;



/* Ŀ¼����ṹ�� (DIR) */

typedef struct {
	FFOBJID	obj;			/* �����ʶ�� */
	DWORD	dptr;			/* ��ǰ��/дƫ���� */
	DWORD	clust;			/* ��ǰ�� */
	LBA_t	sect;			/* ��ǰ���� (0:����������ֹ) */
	BYTE*	dir;			/* Pointer to the directory item in the win[] */
	BYTE	fn[12];			/* SFN (in/out) {body[8],ext[3],status[1]} */
#if FF_USE_LFN
	DWORD	blk_ofs;		/* Offset of current entry block being processed (0xFFFFFFFF:Invalid) */
#endif
#if FF_USE_FIND
	const TCHAR* pat;		/* Pointer to the name matching pattern */
#endif
} DIR;



/* �ļ���Ϣ�ṹ�� (FILINFO) */

typedef struct {
	FSIZE_t	fsize;			/* �ļ���С */
	WORD	fdate;			/* ���޸ĵ����� */
	WORD	ftime;			/* ���޸ĵ�ʱ�� */
	BYTE	fattrib;		/* �ļ����� */
#if FF_USE_LFN
	TCHAR	altname[FF_SFN_BUF + 1];/* ��ѡ�ļ��� */
	TCHAR	fname[FF_LFN_BUF + 1];	/* ���ļ��� */
#else
	TCHAR	fname[12 + 1];	/* �ļ��� */
#endif
} FILINFO;



/* ��ʽ�����ṹ�� (MKFS_PARM) */

typedef struct {
	BYTE fmt;			/* ��ʽѡ�� (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
	BYTE n_fat;			/* Number of FATs */
	UINT align;			/* ��������� (����) */
	UINT n_root;		/* ��Ŀ¼��Ŀ�� */
	DWORD au_size;		/* �ش�С (�ֽ�) */
} MKFS_PARM;



/* �ļ���������ֵ (FRESULT) */

typedef enum {
	FR_OK = 0,				/* (0) �ɹ� */
	FR_DISK_ERR,			/* (1) Ӳ���ײ����I/O����*/
	FR_INT_ERR,				/* (2) ����ʧ�� */
	FR_NOT_READY,			/* (3) ���������������� */
	FR_NO_FILE,				/* (4) �޷��ҵ��ļ� */
	FR_NO_PATH,				/* (5) �޷��ҵ�·�� */
	FR_INVALID_NAME,		/* (6) ·������ʽ��Ч */
	FR_DENIED,				/* (7) Ŀ¼���������ʱ��ܾ� */
	FR_EXIST,				/* (8) ���ʱ��ܾ� */
	FR_INVALID_OBJECT,		/* (9) �ļ�/Ŀ¼������Ч*/
	FR_WRITE_PROTECTED,		/* (10) ����������д������ */
	FR_INVALID_DRIVE,		/* (11) �߼�����������Ч */
	FR_NOT_ENABLED,			/* (12) ���û�й������� */
	FR_NO_FILESYSTEM,		/* (13) û����Ч��FAT��� */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs()�����κ��������ֹ */
	FR_TIMEOUT,				/* (15) �޷�����ڶ����ʱ����ڷ��ʾ�����Ȩ */
	FR_LOCKED,				/* (16) �����ļ�������ԣ��������ܾ� */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN �����������޷����� */
	FR_TOO_MANY_OPEN_FILES,	/* (18) �򿪵��ļ����� > FF_FS_LOCK */
	FR_INVALID_PARAMETER	/* (19) ������Ч */
} FRESULT;



/*--------------------------------------------------------------*/
/* FatFs ģ��Ӧ�ýӿ�API                           */

FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode);			/* ��/����һ���ļ� */
FRESULT f_close (FIL* fp);						/* �ر�һ���򿪵��ļ����� */
FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br);		/* ��ȡ�ļ����� */
FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);	/* д�����ݵ��ļ� */
FRESULT f_lseek (FIL* fp, FSIZE_t ofs);					/* �ƶ��ļ�������ļ�ָ�� */
FRESULT f_truncate (FIL* fp);						/* �ض��ļ���С */
FRESULT f_sync (FIL* fp);						/* ˢ��д���ļ��Ļ������� */
FRESULT f_opendir (DIR* dp, const TCHAR* path);				/* ��һ��Ŀ¼ */
FRESULT f_closedir (DIR* dp);						/* �ر�һ���򿪵�Ŀ¼ */
FRESULT f_readdir (DIR* dp, FILINFO* fno);				/* ��ȡĿ¼�� */
FRESULT f_findfirst (DIR* dp, FILINFO* fno, const TCHAR* path, const TCHAR* pattern);	/* ��Ŀ¼����ȡ��һ��ƥ���� */
FRESULT f_findnext (DIR* dp, FILINFO* fno);				/* ��ȡƥ�����һ�� */
FRESULT f_mkdir (const TCHAR* path);					/* ����һ����Ŀ¼ */
FRESULT f_unlink (const TCHAR* path);					/* ɾ��һ���Ѵ��ڵ��ļ�����Ŀ¼ */
FRESULT f_rename (const TCHAR* path_old, const TCHAR* path_new);	/* ������/�ƶ��ļ�����Ŀ¼ */
FRESULT f_stat (const TCHAR* path, FILINFO* fno);			/* ����ļ�����Ŀ¼�Ĵ����� */
FRESULT f_chmod (const TCHAR* path, BYTE attr, BYTE mask);		/* �����ļ�����Ŀ¼������ */
FRESULT f_utime (const TCHAR* path, const FILINFO* fno);		/* �����ļ�����Ŀ¼��ʱ��� */
FRESULT f_chdir (const TCHAR* path);					/* �ı䵱ǰĿ¼ */
FRESULT f_chdrive (const TCHAR* path);					/* �ı䵱ǰ������ */
FRESULT f_getcwd (TCHAR* buff, UINT len);				/* ��ȡ��ǰĿ¼ */
FRESULT f_getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfs);	/* ��ȡ�������Ŀ��д����� */
FRESULT f_getlabel (const TCHAR* path, TCHAR* label, DWORD* vsn);	/* ��ȡ��� */
FRESULT f_setlabel (const TCHAR* label);				/* ���þ�� */
FRESULT f_forward (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf);	/* ��ȡ�ļ����ݲ�����ת�����������豸 */
FRESULT f_expand (FIL* fp, FSIZE_t fsz, BYTE opt);			/* Ϊ�ļ�����һ�������Ŀ� */
FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt);			/* ����/ж��һ���߼������� */
FRESULT f_mkfs (const TCHAR* path, const MKFS_PARM* opt, void* work, UINT len);	/* ����һ��FAT��� */
FRESULT f_fdisk (BYTE pdrv, const LBA_t ptbl[], void* work);		/* ��һ�������������ֳɼ������� */
FRESULT f_setcp (WORD cp);						/* ���õ�ǰ����ҳ */
int f_putc (TCHAR c, FIL* fp);						/* ���ļ�������ַ� */
int f_puts (const TCHAR* str, FIL* cp);					/* ���ļ�������ַ��� */
int f_printf (FIL* fp, const TCHAR* str, ...);				/* ���ļ�����Ӹ�ʽ���ַ��� */
TCHAR* f_gets (TCHAR* buff, int len, FIL* fp);				/* ���ļ���ȡ�ַ��� */

#define f_eof(fp) ((int)((fp)->fptr == (fp)->obj.objsize))
#define f_error(fp) ((fp)->err)
#define f_tell(fp) ((fp)->fptr)
#define f_size(fp) ((fp)->obj.objsize)
#define f_rewind(fp) f_lseek((fp), 0)
#define f_rewinddir(dp) f_readdir((dp), 0)
#define f_rmdir(path) f_unlink(path)
#define f_unmount(path) f_mount(0, path, 0)

#ifndef EOF
#define EOF (-1)
#endif




/*--------------------------------------------------------------*/
/* Additional user defined functions                            */

/* RTC function */
#if !FF_FS_READONLY && !FF_FS_NORTC
DWORD get_fattime (void);
#endif

/* LFN support functions */
#if FF_USE_LFN >= 1						/* Code conversion (defined in unicode.c) */
WCHAR ff_oem2uni (WCHAR oem, WORD cp);	/* OEM code to Unicode conversion */
WCHAR ff_uni2oem (DWORD uni, WORD cp);	/* Unicode to OEM code conversion */
DWORD ff_wtoupper (DWORD uni);			/* Unicode upper-case conversion */
#endif
#if FF_USE_LFN == 3						/* Dynamic memory allocation */
void* ff_memalloc (UINT msize);			/* Allocate memory block */
void ff_memfree (void* mblock);			/* Free memory block */
#endif

/* Sync functions */
#if FF_FS_REENTRANT
int ff_cre_syncobj (BYTE vol, FF_SYNC_t* sobj);	/* Create a sync object */
int ff_req_grant (FF_SYNC_t sobj);		/* Lock sync object */
void ff_rel_grant (FF_SYNC_t sobj);		/* Unlock sync object */
int ff_del_syncobj (FF_SYNC_t sobj);	/* Delete a sync object */
#endif




/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* �ļ�����ģʽ���򿪷�����־(3rd argument of f_open) */
#define	FA_READ			0x01    //�����ȡ����
#define	FA_WRITE		0x02    //����д������
#define	FA_OPEN_EXISTING	0x00    //���ļ�������ļ������ڣ����ʧ�ܡ�(Ĭ��)
#define	FA_CREATE_NEW		0x04    //����һ�����ļ�������ļ��Ѵ��ڣ��򴴽�ʧ�ܡ�
#define	FA_CREATE_ALWAYS	0x08    //����һ�����ļ�������ļ��Ѵ��ڣ����������ضϲ�����
#define	FA_OPEN_ALWAYS		0x10   //����ļ����ڣ���򿪣����򣬴���һ�����ļ���
#define	FA_OPEN_APPEND		0x30    //���ļ�β��׷��д������

/* ����Ѱ�ҿ��� (2nd argument of f_lseek) */
#define CREATE_LINKMAP	((FSIZE_t)0 - 1)

/* ѡ���ʽ����ʽ���ã� (2nd argument of f_mkfs) */
#define FM_FAT		0x01
#define FM_FAT32	0x02
#define FM_EXFAT	0x04
#define FM_ANY		0x07
#define FM_SFD		0x08

/* �ļ�ϵͳ���� (FATFS.fs_type) */
#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3
#define FS_EXFAT	4

/* ����Ŀ¼���ļ�����λ (FILINFO.fattrib) */
#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */


#ifdef __cplusplus
}
#endif

#endif /* FF_DEFINED */
