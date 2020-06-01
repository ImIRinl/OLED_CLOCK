/*---------------------------------------------------------------------------/
/  FatFs ��������
/---------------------------------------------------------------------------*/

#define FFCONF_DEF	86606	/* �汾 ID */

/*---------------------------------------------------------------------------/
/ ��������
/---------------------------------------------------------------------------*/

#define FF_FS_READONLY	0
/* ���ѡ�񿪹ء�ֻ�������� (0:��/д or 1:ֻ��)
/  ���ѡ����1���ã���ɾ���� дAPI����,��f_write(), f_sync(),
/  f_unlink(), f_mkdir(), f_chmod(), f_rename(), f_truncate(), f_getfree()
/�Ϳ�ѡ���д����. */

#define FF_FS_MINIMIZE	0
/* ���ѡ�񿪹ض�������С���𣬶��һ��Ƴ�һЩ����API���� 
/
/   0: ����������ȫʹ��.
/   1: f_stat(), f_getfree(), f_unlink(), f_mkdir(), f_truncate() and f_rename()
/      ���Ƴ�.
/   2: ��1ѡ��Աȣ������˶�f_opendir(), f_readdir() and f_closedir() ���Ƴ�
/   3: ��1ѡ��Աȣ������˶�f_lseek()���Ƴ�*/


#define FF_USE_STRFUNC	2
/* ���ѡ�񿪹����ַ�������, f_gets(), f_putc(), f_puts() and f_printf().
/
/  0: ʧ���ַ�������
/  1: ������ LF-CRLF conversion.
/  2: ���� with LF-CRLF conversion. */


#define FF_USE_FIND		1
/* ���ѡ�񿪹ع���Ŀ¼��ȡ����, f_findfirst() and
/  f_findnext(). (0:ʧ��, 1:ʹ�� 2:ʹ�ܣ�Ҳƥ��altname[] t) */


#define FF_USE_MKFS		0
/* ���ѡ�񿪹� f_mkfs() ����. (0:ʧ�� or 1:ʹ��) */


#define FF_USE_FASTSEEK	1
/* ���ѡ�񿪹ؿ���Ѱ�Һ���. (0:ʧ�� or 1:ʹ��) */


#define FF_USE_EXPAND	0
/* ���ѡ�񿪹� f_expand ����. (0:ʧ�� or 1:ʹ��) */


#define FF_USE_CHMOD	0
/* ���ѡ�񿪹� ���Դ�����, f_chmod() and f_utime().
/  (0:ʧ�� or 1:ʹ��) ͬʱ FF_FS_READONLY ��ҪΪ0����ʹ�����ѡ��. */


#define FF_USE_LABEL	1
/* ���ѡ�񿪹� ��꺯��, f_getlabel() and f_setlabel().
/  (0:ʧ�� or 1:ʹ��) */


#define FF_USE_FORWARD	0
/* ���ѡ�񿪹� f_forward() ����. (0:ʧ�� or 1:ʹ��) */


/*---------------------------------------------------------------------------/
/ �������ú������ռ�����
/---------------------------------------------------------------------------*/

#define FF_CODE_PAGE	936
/* ���ѡ��ָ����Ŀ��ϵͳ��ʹ�õ�OEM����ҳ
/  ����ȷ�Ĵ���ҳ���ÿ��ܵ��´��ļ�ʧ��
/
/   437 - ����Ӣ��
/   720 - ��������
/   737 - ϣ����
/   771 - KBL
/   775 - Baltic
/   850 - ������ 1
/   852 - ������ 2
/   855 - �������
/   857 - ��������
/   860 - ��������
/   861 - ������
/   862 - ϣ������
/   863 - ���ô���
/   864 - ��������
/   865 - ��ŷ
/   866 - ����
/   869 - ϣ���� 2
/   932 - ���� (DBCS)
/   936 - ��������(DBCS)
/   949 - ����(DBCS)
/   950 - ��������(DBCS)
/     0 - �����������д���ҳ������ f_setcp()��������
*/


#define FF_USE_LFN		0
#define FF_MAX_LFN		255
/*  FF_USE_LFN ���� ֧��LFN (���ļ���).
/
/   0: ʧ�� LFN. FF_MAX_LFN �𲻵�Ч��
/   1: ʹ�� LFN ��BSS�о�̬���������������ǲ����̰߳�ȫ����
/   2: ʹ�� LFN ��stack��ջ���ж�̬������������
/   3: ʹ�� LFN ��heap���ѣ��ж�̬������������
/
/  ʹ��LFN��ffunicode.c�ļ���Ҫ��ӵ������С�LFN������Ҫռ��ĳЩ�ض��Ĺ�����������
/  ��������СΪ(FF_MAX_LFN + 1) * 2 bytes�����ʹ��exFAT������Ҫ����(FF_MAX_LFN + 44) / 15 * 32 bytes
/  FF_MAX_LFN��������UTF-16 ���뵥Ԫ�Ĺ�����������С����ΧΪ12-255���Ƽ�����Ϊ255����ȫ֧��LFN����
/ ��ʹ��stack��ջ����Ϊ������������ע�����stack��ջ��������������ʹ��heap���ѣ��ռ���Ϊ������������
/ ��ؿռ�Ĺ���������ff_memalloc()��ff_memfree()������ffsystem.c��Ҫ��ӵ������С�*/



#define FF_LFN_UNICODE	0
/* ���ѡ�񿪹� ��LFNʹ��ʱ���ַ�����API
/
/   0: ANSI/OEM in current CP (TCHAR = char)
/   1: Unicode in UTF-16 (TCHAR = WCHAR)
/   2: Unicode in UTF-8 (TCHAR = char)
/   3: Unicode in UTF-32 (TCHAR = DWORD)
/
/ ͬʱ�ַ���I/O��������ΪҲ���ܴ�ѡ���Ӱ��*/

#define FF_LFN_BUF		255
#define FF_SFN_BUF		12
/* ����������ö�������FILINFO�ṹ������ֳ�Ա��С
/  ���������ڶ�ȡĿ¼� ��ЩֵӦ���㹻���ļ�����ȡ
/  ���Ķ�ȡ�ļ�������ȡ�����ַ����롣 depends
/  ��LFNʧ�ܣ���Щѡ��û�����á�*/

#define FF_STRF_ENCODE	0
/* �� FF_LFN_UNICODE >= 1 ����LFN ʹ��, �ַ���I/O����, f_gets(),
/  f_putc(), f_puts and f_printf() ����������ת���ַ�����.
/  ���ѡ��ٶ��ַ����� ON THE FILE ����ͨ����Щ������/д
/
/   0: ANSI/OEM in current CP
/   1: Unicode in UTF-16LE
/   2: Unicode in UTF-16BE
/   3: Unicode in UTF-8
*/

#define FF_FS_RPATH		0
/* ��ѡ������֧�����·��
/
/   0: ʧ�����·�������Ƴ���غ���
/   1: ʹ�����·�� f_chdir() �� f_chdrive() ����ʹ��
/   2: ��ѡ��1�Ļ����ϣ����ӿ���ʹ��f_getcwd()����
*/

/*---------------------------------------------------------------------------/
/ ������/�������
/---------------------------------------------------------------------------*/

#define FF_VOLUMES		1
/* ����ʹ�õľ�� (�߼�����)��Χ1-10 */


#define FF_STR_VOLUME_ID	0
#define FF_VOLUME_STRS		"RAM","NAND","CF","SD","SD2","USB","USB2","USB3"
/* FF_STR_VOLUME_ID ����֧�������ַ���volume ID
/  FF_STR_VOLUME_ID�ɱ�����Ϊ1����2, �����ַ�����������·�����е���������
/  FF_VOLUME_STRS ����volume ID�ַ�����Ϊ�߼�������
/  ��Ŀ��������������FF_VOLUMES
/  �Ϸ���volume ID strings�ַ���A-Z��a-z����0-9 however, ���ִ�Сд
/  ���FF_STR_VOLUME_ID >= 1 ���� FF_VOLUME_STRS û�ж��壬�û���Ҫ����volume�ַ�������
/  const char* VolumeStr[FF_VOLUMES] = {"ram","flash","sd","usb",...
*/

#define FF_MULTI_PARTITION	0
/* ���ѡ�񿪹�֧�����߼��������ж���
/  Ĭ��Ϊ (0), ÿ���߼���������һ����Ӧ��ͬ�������������Ų���ֻ���������������Ļᱻ��װ
/  ���������ʹ�ܣ�1���ˣ�ÿ��ÿ���߼���������һ����Ӧ��ͬ�������������룬��VolToPart[]���г�����
/  ͬʱ��f_fdisk()��������ʹ��*/
//  funciton will be available. 


#define FF_MIN_SS		512
#define FF_MAX_SS		512
/* ���ѡ������֧�������ռ�ķ�Χ��512,1024, 2048 or 4096��
/  ���ڴ����ϵͳ��ͨ�ô洢����Ӳ�̣�ͨ������512
/  �����ֵ��Ҫflash�ռ��һЩѡ��ý�����͡�
/  ��FF_MAX_SS ���� FF_MIN_SS, FatFs�������ò�ͬ��������Сģʽ
/  ʹ��disk_ioctl()������Ҫ�õ�GET_SECTOR_SIZE����*/


#define FF_LBA64		0
/*���ѡ�񿪹�֧��64λLBA(0:ʧ�� or 1:ʹ��)
/  ʹ��64λLBA��ͬʱexFAT��Ҫʹ��(FF_FS_EXFAT == 1) */


#define FF_MIN_GPT		0x100000000
/* ����GPT��ʽ��f_mkfs��f_fdisk����������������С��������0x100000000 Ϊ���
/  ���ѡ����FF_LBA64 == 0ʱ��Ч*/


#define FF_USE_TRIM		0
/* ���ѡ�񿪹�֧��(0:ʧ�� or 1:ʹ��)
/  ʹ��Trim ����, ʹ��disk_ioctl()������Ҫ�õ�CTRL_TRIM���� */



/*---------------------------------------------------------------------------/
/ ϵͳ����
/---------------------------------------------------------------------------*/

#define FF_FS_TINY		0
/* ���ѡ�񿪹�tiny��������(0:���� ���� 1:Tiny)
/  ����tiny ����, �ļ������С����С�� FF_MAX_SS bytes
/  ��ͬ��˽������������ɾ���ļ��������ļ�ϵͳ����ͨ�����������������ļ����ݴ���*/


#define FF_FS_EXFAT		0
/* ���ѡ�񿪹�֧��exFAT�ļ�ϵͳ(0:ʧ�� or 1:ʹ��)
/  ʹ�� exFAT,LFNҲҪʹ��. (FF_USE_LFN >= 1)
/  ��ע�⣬ʹ��exFAT��ʧȥANSI C (C89)����. */


#define FF_FS_NORTC	1
#define FF_NORTC_MON	1
#define FF_NORTC_MDAY	1
#define FF_NORTC_YEAR	2019
/* FF_FS_NORTC ����ʱ������ܡ����ϵͳû��RTC���ܻ��߲���Ҫʱ��������FF_FS_NORTC = 1��ʧ��ʱ�������
/  ÿ����FatFs�޸ĵĶ���Ҫһ���̶���ʱ�����������FF_NORTC_MON, FF_NORTC_MDAY and FF_NORTC_YEAR�ĵ�ǰʱ��
/  ʹ��ʱ�������FF_FS_NORTC = 0), get_fattime() ���� ��Ҫ��ӵ����̣��Ա��ȡ��ǰʱ�䡣
/  ��ʱFF_NORTC_MON,FF_NORTC_MDAY �� FF_NORTC_YEAR�𲻵����á�
/  ���ѡ����ֻ������(FF_FS_READONLY = 1)Ҳ�������� */


#define FF_FS_NOFSINFO	0
/* �����Ҫ֪����ǰFAT32�����пռ䣬�������ѡ��Ϊbit 0��f_getfree() �����ھ����غ�ĵ�һʱ����FAT��ȫɨ��
/  Bit 1 ����ʹ��������Ĵغ�
/
/  bit0=0: �����FSINFO���ã�ʹ�ÿ��дؼ���
/  bit0=1: ��ʹ����FSINFO�Ŀ��дؼ���
/  bit1=0: �����FSINFO���ã�ʹ��������Ĵغ�
/  bit1=1: ��ʹ����FSINFO��������Ĵغ�
*/


#define FF_FS_LOCK		0
/* ���ѡ�� FF_FS_LOCK ���� �ļ������������ƴ��ظ��ļ��Ͷ���ķǷ�������
/  ���ѡ�����FF_FS_READONLYΪ1
/  0:  ʧ���ļ������ܡ�Ϊ�˱������𻵣�Ӧ�ó���Ҫ����Ƿ��򿪡��Ƴ�����������������
/  >0: ʹ���ļ������ܣ���ֵ������ٸ��ļ�/��Ŀ¼�������ļ���������ͬʱ�򿪡�
/  ��ע���ļ��������Ƕ�����re-entrancy���������룩��*/


/* #include <somertos.h>	// O/S definitions */
#define FF_FS_REENTRANT	0
#define FF_FS_TIMEOUT	1000
#define FF_SYNC_t		HANDLE
/* TFF_FS_REENTRANT ����FatFsģ�鱾���re-entrancy�����������̰߳�ȫ��
/  ��ע�⣬�������ѡ�񣬽��벻ͬ�����ļ�ͨ����re-entrant�;����ƺ�����
/  f_mount(), f_mkfs()��f_fdisk() ����, ͨ������re-entrant�ġ�
/  ����ֻ���ļ�/Ŀ¼������ͬ�ľ����ܴ˿��� 
/
/   0: ʧ�� re-entrancy. FF_FS_TIMEOUT �� FF_SYNC_t ��������.
/   1: ʹ�� re-entrancy. �û��ṩ��ͬ���������,
/      ff_req_grant(), ff_rel_grant(), ff_del_syncobj() and ff_cre_syncobj()
/      ����������ӵ������У�������syscall.c��
/
/  FF_FS_TIMEOUT ���峬ʱʱ�䣨��ʱ��tickΪ��λ��
/  FF_SYNC_t ���� O/S ����sync�������ͣ�����HANDLE, ID, OS_EVENT*,
/  SemaphoreHandle_t and etc. A header file for O/S �����ͷ�ļ���Ҫ������ff.h���� */



/*--- ��������ѡ�� ---*/
