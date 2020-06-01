#include "heap_manage.h"
#include "string.h"
#include "uart.h"

//���ܿռ�(����all_heap[ HEAP_SIZE] ����������ʵ��ռ HEAP_SIZE��2 �ֽڣ�������)
#define HEAP_SIZE  1500    
#define HEAPMEM_IN_USE             0x8000

//�ѿռ����λ��
#define HEAP_LASTBLK_IDX      ((HEAP_SIZE / HEAPMEM_HDRSZ) - 1) 
//���ֿ�λ��
#define HEAPMEM_SMALLBLK_HDRCNT   (HEAPMEM_SMALLBLK_BUCKET / HEAPMEM_HDRSZ)
//�����ʼλ��
#define HEAPMEM_BIGBLK_IDX        (HEAPMEM_SMALLBLK_HDRCNT + 1)

//�ײ���С
#define HEAPMEM_HDRSZ              sizeof(heapMemHdr_t)  
//��С���С
#define HEAPMEM_MIN_BLKSZ         (HEAPMEM_ROUND((HEAPMEM_HDRSZ * 2)))
//С���С
#define HEAPMEM_SMALL_BLKSZ       (HEAPMEM_ROUND(16))
//Ĭ�ϳ����С
#define HEAPMEM_LL_BLKSZ          (HEAPMEM_ROUND(417) + (19 * HEAPMEM_HDRSZ))

//С���ܿռ�
#define HEAPMEM_SMALLBLK_BUCKET  ((HEAPMEM_SMALL_BLKSZ * HEAPMEM_SMALL_BLKCNT) + HEAPMEM_LL_BLKSZ)
//����ܿռ�
#define HEAPMEM_BIGBLK_SZ         (HEAP_SIZE - HEAPMEM_SMALLBLK_BUCKET - HEAPMEM_HDRSZ*2)

//Ĭ��С������
#define HEAPMEM_SMALL_BLKCNT       8
// ���������ڴ��С�������������17�ֽڿռ䣬�����Ϊ18�ֽڣ�
#define HEAPMEM_ROUND(X)       ((((X) + HEAPMEM_HDRSZ - 1) / HEAPMEM_HDRSZ) * HEAPMEM_HDRSZ)

typedef struct {
  unsigned len : 15;//����ĳ������Ϊ2^16-1���ֽڣ�������ռ����С����Ϊ2���ֽ�
  unsigned inUse : 1;//��־λ��ʾ�����Ƿ��Ѿ���ʹ��
} heapMemHdrHdr_t;

typedef union {
  //��ˣ���halDataAlign\u tС��UINT16ʱ��������ǿ�ƽṹ��������Ԫ�أ���������Ŀ�����˷ѿռ䡣
  uint8 alignDummy;
  uint16 val;//�洢��һ�鳤�ȣ�in use��Ϣ
  heapMemHdrHdr_t hdr;//��ͷָ��
} heapMemHdr_t;

static __no_init heapMemHdr_t all_heap[HEAP_SIZE];//����ѿռ�����
static __no_init heapMemHdr_t *ff1;  //��һ���տ�

static uint8 heapMemStat;            // ��ɢ״̬��־ 0x01���߳�

/**************************************************************************************************
 *@����            heap_init
 *
 *@����            �ѿռ��ʼ��
 *
 *@�������  
 *
 *@����            ��
 *
 *�������
 *
 *��
 * 
 *@����           ��
 **************************************************************************************************
 */
void heap_init(void)
{
  all_heap[HEAP_LASTBLK_IDX].val = 0;//�ڶѵ�ĩβ����һ���տ飬�Ա�������п��ٱȽ�
  ff1 = all_heap;//���ù���С��ռ���ײ�
  ff1->val = HEAPMEM_SMALLBLK_BUCKET;
  //���û���С��ռ�����ռ���ײ�
  all_heap[HEAPMEM_SMALLBLK_HDRCNT].val = (HEAPMEM_HDRSZ | HEAPMEM_IN_USE);;
  // ���ù�����ռ��ײ�
  all_heap[HEAPMEM_BIGBLK_IDX].val = HEAPMEM_BIGBLK_SZ;  // Set 'len' & clear 'inUse' field.
}
/**************************************************************************************************
 *@����            heap_kick
 *
 *@����            �������ռ��С��ռ�ȥ��
 *
 *@�������  
 *
 *@����            ��
 *
 *�������
 *
 *��
 * 
 *@����           ��
 **************************************************************************************************
 */
void heap_kick(void)
{
//  uint8 intState;
  //����һ����λ�ڴ�ռ䣬��������ռ�ͻ���������ռ��
  heapMemHdr_t *tmp = heap_alloc(1);
  //�����ٽ���
  //ff1ָ��������ڴ�ռ��ײ�����ff1��ȡ���˵�һ��С����е�ַ
  ff1 = tmp - 1;   //ƫ�Ƶ��ռ��ײ�
  heap_free(tmp); //�ͷŸ�������ڴ�ռ�
  heapMemStat = 0x01; //���ó�����޳���־

  //�˳��ٽ���
}
/**************************************************************************************************
 *@����            heap_alloc
 *
 *@����            �ѿռ���亯��
 *
 *@�������  
 *
 *@����            size - �ֽڳ���
 *
 *�������
 *
 *��
 * 
 *@����           void*
 **************************************************************************************************
 */
void *heap_alloc(uint16 size)
{
  heapMemHdr_t *prev = NULL;
  heapMemHdr_t *hdr;
//  uint8 intState;
  uint8 coal = 0;
  size += HEAPMEM_HDRSZ; //����Ҫ����Ŀռ����һ�������ײ�
 //�����ٽ���
 
  //����size��С���ǿռ���루�봦�����ͱ�������أ�
  if ( sizeof( uint8 ) == 2 )//����uint8ռ��2���ֽ�
  {
    size += (size & 0x01);//����Ϊ196������sizeΪ196������Ϊ197������sizeҪ198������
  }
  else if ( sizeof( uint8 ) != 1 )
  {
    const uint8 mod = size % sizeof( uint8 );

    if ( mod != 0 )
    {
      size += (sizeof( uint8 ) - mod);
    }
  }
  //�ж�С���ڴ�ռ��Ƿ��㹻���䣬���������ڴ�ռ�����
  if ((heapMemStat == 0) || (size <= HEAPMEM_SMALL_BLKSZ))
  {
    hdr = ff1;//С���ڴ棬��ff1��ʼ����
  }
  else
  {
    hdr = (all_heap + HEAPMEM_BIGBLK_IDX);//�Ӵ�鿪ʼ����
  }
  
  //��ʼ������Ѱ���ʺϵ��ڴ�ռ�
  do
  {
    if ( hdr->hdr.inUse )//������free��
    {
      coal = 0;//������һ�飬�����free
    }
    else //����free��
    {
      if ( coal != 0 )//��һ����free��
      {
        prev->hdr.len += hdr->hdr.len;//����free��ϲ������ڴ�ռ�

        if ( prev->hdr.len >= size )  //�ϲ���Ĵ�С����size
        {
          hdr = prev;  //�õ���ĵ�ַ
          break;
        }
      }
      else //��һ���Ƿ�free��
      {
        if ( hdr->hdr.len >= size )//һ����Ĵ�С�Ϳ���������������䣬����ѭ������
        {
          break;
        }

        coal = 1;//���򣬱��coalΪ1��������һ�飬������free��
        prev = hdr; //���浱ǰ�ڴ��ַ
      }
    }
    //(uint8 *)hdr�������ʹ����2���ֽڣ�ǿ��ת����1���ֽ�
    hdr = (heapMemHdr_t *)((uint8 *)hdr + hdr->hdr.len);//����mallocʵ�ַ�ʽ��������һ��

    if ( hdr->val == 0 )//�Ѿ�����ѵײ�����ʼ��ʱ���Ѿ��öѵ�Ϊ�㣬����ʶ��
    {
      hdr = NULL;//��ָ�룬��ʾ�Ҳ�������size��
      break;
    }
  }while(1);
  
  if ( hdr != NULL )//�Ѿ��ҵ�����size��
  {
    uint16 tmp = hdr->hdr.len - size;//��ʾ��Ĵ�С��������Ĵ�Сʱ��Ϊ�˲��˷ѿռ䣬��Ҫ�ѿ��п�
    //ȷ���Ƿ���������ֵ
    if ( tmp >= HEAPMEM_MIN_BLKSZ )//ʣ�µĴ�С���Ե�����Ϊһ��free��
    {
      // Split the block before allocating it.
      heapMemHdr_t *next = (heapMemHdr_t *)((uint8 *)hdr + size);
      next->val = tmp;                     // ���ߺ�һ�����Լ�����Ϣ
      hdr->val = (size | HEAPMEM_IN_USE);  // value����ǰһ��Ĵ�С��ʹ������������൱��˫������
    }
    else
    {
      hdr->hdr.inUse = TRUE; //��Ǳ����Ѿ���ʹ��
    }
    
  //  if ((heapMemStat != 0) && (ff1 == hdr))
  //  {
   //   ff1 = (heapMemHdr_t *)((uint8 *)hdr + hdr->hdr.len);
   // }
    hdr++;   
  }
  //�˳��ٽ���
  return (void *)hdr;
}
/**************************************************************************************************
 *@����            heap_free
 *
 *@����            ���ͷź���
 *
 *@�������  
 *
 *@����            *ptr - �ڴ��ַ
 *
 *�������
 *
 *��
 * 
 *@����           ��
 **************************************************************************************************
 */
void heap_free(void *ptr)
{
  //���heapMemHdr_tΪ2���ֽڣ�������ָ���ȥ1�������ַ��ı�2
  heapMemHdr_t *hdr = (heapMemHdr_t *)ptr - 1;//��ȡ���ڴ�ռ��ײ�
 // uint8 intState;
  //�����ж��ٽ�
  hdr->hdr.inUse = FALSE; //���ʹ��״̬Ϊ��δʹ��
  
  ptr = NULL;
  
 // if (ff1 > hdr)
 // {
 //   ff1 = hdr;//����ff1λ��
 // }
  //�˳��ж��ٽ�
}