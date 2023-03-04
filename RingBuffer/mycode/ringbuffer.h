/**
 * @brief   
 */

#ifndef RINGBUFFER_H__
#define RINGBUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char uint8_t;
typedef unsigned int  uint32_t;

#define RB_ASSERT(x) 

#define RINGBUFF_MEM_POOL                       1
#define RINGBUFF_MEM_MALLOC                     2

typedef struct _ringBuffer
{
    uint8_t type;
    uint8_t *pBuffer;
    
    uint32_t rFlag   :1;
    uint32_t rIndex  :31;
    uint32_t wFlag   :1;
    uint32_t wIndex  :31;
    
    uint32_t bufferSize;
} ringBuffer_t;




#ifdef __cplusplus
}
#endif

#endif
