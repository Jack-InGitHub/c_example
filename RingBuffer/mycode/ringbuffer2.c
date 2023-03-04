/**
 * @file    ringbuffer2.c
 * @author  oikiou (pq_liu@foxmail.com)
 * @brief   使用 usedSize 变量描述当前存储数据
 * @version 0.1
 * @date    2023-03-04 : 
 * 
 * @copyright COPYRIGHT PQ.liu®
 * 
 * SPDX-License-Identifier: GPL-3.0-only
 * @section   History
 * <table>
 *     <tr><th>Version <th>Data        <th>Author  <th>Notes
 *     <tr><td>V0.1    <td>2023-03-04  <td>oikiou  <td>First Version
 * </table>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringbuffer2.h"

/**
 * @brief   
 * @param   rb : 
 * @param   pPool : 
 * @param   size : 
 * @return  uint32_t : 
 */
uint32_t RingBuffer_InitPool(ringBuffer_t *rb, uint8_t *const pPool, uint32_t size)
{
    RB_ASSERT (rb != NULL);
    
    rb->type = RINGBUFF_MEM_POOL;
    rb->pBuffer = pPool;
    rb->bufferSize = size;
    memset(rb->pBuffer, 0, rb->bufferSize);
    rb->wIndex = rb->rIndex = 0;
    rb->usedSize = 0;
    return 0;
}

uint32_t RingBuffer_ResetPool(ringBuffer_t *rb)
{
    RB_ASSERT (rb != NULL);
    
    if (rb->type != RINGBUFF_MEM_POOL)
    {
        return 1;
    }
    rb->pBuffer = NULL;
    rb->bufferSize = 0;
    rb->wIndex = rb->rIndex = 0;
    rb->usedSize = 0;
    return 0;
}

/**
 * @brief   
 * @param   rb : 
 * @param   size : 
 * @return  uint32_t : 
 */
uint32_t RingBuffer_InitMalloc(ringBuffer_t *rb, uint32_t size)
{
    RB_ASSERT (rb != NULL);
    
    rb->type = RINGBUFF_MEM_MALLOC;
    rb->pBuffer = malloc(size);
    if (rb->pBuffer == NULL)
    {
        return 1;
    }
    memset(rb->pBuffer, 0, rb->bufferSize);
    rb->bufferSize = size;
    rb->wIndex = rb->rIndex = 0;
    rb->usedSize = 0;
    return 0;
}

uint32_t RingBuffer_ResetMalloc(ringBuffer_t *rb)
{
    RB_ASSERT (rb != NULL);
    
    if (rb->type != RINGBUFF_MEM_MALLOC)
    {
        return 1;
    }
    free(rb->pBuffer);
    rb->pBuffer = NULL;
    rb->bufferSize = 0;
    rb->wIndex = rb->rIndex = 0;
    rb->usedSize = 0;
    return 0;
}

/**
 * @brief   获取已经使用的空间量
 * @param   rb : 
 * @return  uint32_t : 
 */
static inline uint32_t RingBuffer_GetCount(ringBuffer_t *rb)
{
    RB_ASSERT (rb != NULL);
    return rb->usedSize;
}

/**
 * @brief   获取可用的空间量
 * @param   rb : 
 * @return  uint32_t : 
 */
static inline uint32_t RingBuffer_GetSpace(ringBuffer_t *rb)
{
    RB_ASSERT (rb != NULL);
    return rb->bufferSize - rb->usedSize;
}

/**
 * @brief   写入环形缓冲
 * @param   rb : 控制块
 * @param   pPutBuffer : 
 * @param   putSize : 预期存入数据量
 * @return  uint32_t : 实际存入的数据量，注意！这个数据量可能会小于预期
 */
uint32_t RingBuffer_Put(ringBuffer_t *rb, const uint8_t *const pPutBuffer, uint32_t putSize)
{
    RB_ASSERT (rb != NULL);
    RB_ASSERT (pPutBuffer != NULL);
    
    uint32_t ringWIndexToBuffEnd = rb->bufferSize - rb->wIndex;
    uint32_t freeSpace = RingBuffer_GetSpace(rb);
    
    if (freeSpace == 0)
        {return 0;}
    
    // 丢弃一些数据
    if (freeSpace < putSize)
    {
        putSize = freeSpace;
    }
    
    // 没有发生位置翻转
    if (ringWIndexToBuffEnd > putSize)
    {
        memcpy(&rb->pBuffer[rb->wIndex], pPutBuffer, putSize);
        rb->wIndex += putSize;
        rb->usedSize += putSize;
        return putSize;
    }
    
    // 发生位置翻转 记录flag 用于区分 wIndex == rIndex 时候是 full 还是 empty
    memcpy(&rb->pBuffer[rb->wIndex], &pPutBuffer[0], ringWIndexToBuffEnd);
    memcpy(&rb->pBuffer[0], &pPutBuffer[ringWIndexToBuffEnd], putSize-ringWIndexToBuffEnd);
    rb->wIndex = putSize-ringWIndexToBuffEnd;
    rb->usedSize += putSize;
    return putSize;
}

/**
 * @brief   读取环形缓冲
 * @param   rb : 控制块
 * @param   pGetBuffer : 
 * @param   getSize : 预取获取数据量
 * @return  uint32_t : 实际获取的数据量，注意！这个数据量可能会小于预期
 */
uint32_t RingBuffer_Get(ringBuffer_t *rb, uint8_t *const pGetBuffer, uint32_t getSize)
{
    RB_ASSERT (rb != NULL);
    RB_ASSERT (pGetBuffer != NULL);
    
    uint32_t ringRIndexToBuffEnd = rb->bufferSize - rb->rIndex;
    uint32_t usedSpace = RingBuffer_GetCount(rb);
    
    if(usedSpace == 0)
        {return 0;}
    
    // 只读取部分数据
    if(usedSpace < getSize)
    {
        getSize = usedSpace;
    }
    
    // 没有发生位置翻转
    if(ringRIndexToBuffEnd > getSize)
    {
        memcpy(pGetBuffer, &rb->pBuffer[rb->rIndex], getSize);
#if 0
memset(&rb->pBuffer[rb->rIndex], '0', getSize);
#endif
        rb->rIndex += getSize;
        rb->usedSize -= getSize;
        return getSize;
    }
    
    // 发生位置翻转 记录flag 用于区分 wIndex == rIndex 时候是 full 还是 empty
    memcpy(pGetBuffer, &rb->pBuffer[rb->rIndex], ringRIndexToBuffEnd);
    memcpy(&pGetBuffer[ringRIndexToBuffEnd], &rb->pBuffer[0], getSize - ringRIndexToBuffEnd);
#if 0
memset(&rb->pBuffer[rb->rIndex], '0', ringRIndexToBuffEnd);
memset(&rb->pBuffer[0], '0', getSize - ringRIndexToBuffEnd);
#endif
    rb->rIndex = getSize - ringRIndexToBuffEnd;
    rb->usedSize -= getSize;
    return getSize;
}





#define RB_SIZE 10


void showBuffer(char *str,uint8_t *buffer, uint32_t size)
{
    uint8_t tmp[RB_SIZE+1];
    memcpy(tmp, buffer, size);
    tmp[size] = 0;
    printf("%s:%s\n",str, tmp);
}

int main(int argc, char **argv)
{
    ringBuffer_t rb;
    ringBuffer_t *pRB = &rb;
    uint8_t rbBuffer[RB_SIZE]={0};
    
    uint8_t PutBuffer[RB_SIZE]={0};
    uint8_t *pPutBuffer = PutBuffer;
    uint32_t putSize = 0;
    uint32_t putResultCount = 0;
    
    uint8_t GetBuffer[RB_SIZE]={0};
    uint8_t *pGetBuffer = GetBuffer;
    uint32_t getSize = 0;
    uint32_t getResultCount = 0;
    
    const char len5[]  = "55555";
    const char len6[]  = "666666";
    const char len7[]  = "7777777";
    const char len10[]  = "++++++++++";
    
    printf("RingBuffer_Init\n");
    if ( RingBuffer_InitPool(pRB, rbBuffer, RB_SIZE) != 0 )
    {
        printf("RingBuffer_Init failed\n");
        return -1;
    }
    printf("RingBuffer_Init ed %d\n", __LINE__);
    showBuffer("pGetBuffer", pGetBuffer, RB_SIZE);
    showBuffer("pPutBuffer", pPutBuffer, RB_SIZE);
    showBuffer("rbBuffer", rbBuffer, RB_SIZE);
    
    putSize = 10;
    memcpy(pPutBuffer, len10, putSize);
    putResultCount = RingBuffer_Put(pRB, pPutBuffer, putSize);
    if( putResultCount != putSize )
    {
        printf("RingBuffer_Put failed result:%d dest:%d\n", putResultCount, putSize);
    }
    printf("\nRingBuffer_Put %d ed %d\n", putSize, __LINE__);
    showBuffer("pGetBuffer", pGetBuffer, RB_SIZE);
    showBuffer("pPutBuffer", pPutBuffer, RB_SIZE);
    showBuffer("rbBuffer", rbBuffer, RB_SIZE);
    
getSize = 10;
getResultCount = RingBuffer_Get(pRB, pGetBuffer, getSize);
if ( getResultCount != getSize )
{
    printf("RingBuffer_Get failed result:%d dest:%d\n", getResultCount, getSize);
}
printf("\nRingBuffer_Get %d ed %d\n", getSize, __LINE__);
showBuffer("pGetBuffer", pGetBuffer, RB_SIZE);
showBuffer("pPutBuffer", pPutBuffer, RB_SIZE);
showBuffer("rbBuffer", rbBuffer, RB_SIZE);
    
    putSize = 5;
    memcpy(pPutBuffer, len5, putSize);
    putResultCount = RingBuffer_Put(pRB, pPutBuffer, putSize);
    if( putResultCount != putSize )
    {
        printf("RingBuffer_Put failed result:%d dest:%d\n", putResultCount, putSize);
    }
    printf("\nRingBuffer_Put %d ed %d\n", putSize, __LINE__);
    showBuffer("pGetBuffer", pGetBuffer, RB_SIZE);
    showBuffer("pPutBuffer", pPutBuffer, RB_SIZE);
    showBuffer("rbBuffer", rbBuffer, RB_SIZE);
    
getSize = 5;
getResultCount = RingBuffer_Get(pRB, pGetBuffer, getSize);
if ( getResultCount != getSize )
{
    printf("RingBuffer_Get failed result:%d dest:%d\n", getResultCount, getSize);
}
printf("\nRingBuffer_Get %d ed %d\n", getSize, __LINE__);
showBuffer("pGetBuffer", pGetBuffer, RB_SIZE);
showBuffer("pPutBuffer", pPutBuffer, RB_SIZE);
showBuffer("rbBuffer", rbBuffer, RB_SIZE);
    
    
    putSize = 7;
    memcpy(pPutBuffer, len7, putSize);
    putResultCount = RingBuffer_Put(pRB, pPutBuffer, putSize);
    if( putResultCount != putSize )
    {
        printf("RingBuffer_Put failed result:%d dest:%d\n", putResultCount, putSize);
    }
    printf("\nRingBuffer_Put %d ed %d\n", putSize, __LINE__);
    showBuffer("pGetBuffer", pGetBuffer, RB_SIZE);
    showBuffer("pPutBuffer", pPutBuffer, RB_SIZE);
    showBuffer("rbBuffer", rbBuffer, RB_SIZE);


getSize = 2;
getResultCount = RingBuffer_Get(pRB, pGetBuffer, getSize);
if ( getResultCount != getSize )
{
    printf("RingBuffer_Get failed result:%d dest:%d\n", getResultCount, getSize);
}
printf("\nRingBuffer_Get %d ed %d\n", getSize, __LINE__);
showBuffer("pGetBuffer", pGetBuffer, RB_SIZE);
showBuffer("pPutBuffer", pPutBuffer, RB_SIZE);
showBuffer("rbBuffer", rbBuffer, RB_SIZE);


    putSize = 5;
    memcpy(pPutBuffer, len5, putSize);
    putResultCount = RingBuffer_Put(pRB, pPutBuffer, putSize);
    if( putResultCount != putSize )
    {
        printf("RingBuffer_Put failed result:%d dest:%d\n", putResultCount, putSize);
    }
    printf("\nRingBuffer_Put %d ed %d\n", putSize, __LINE__);
    showBuffer("pGetBuffer", pGetBuffer, RB_SIZE);
    showBuffer("pPutBuffer", pPutBuffer, RB_SIZE);
    showBuffer("rbBuffer", rbBuffer, RB_SIZE);
    
    
getSize = 10;
getResultCount = RingBuffer_Get(pRB, pGetBuffer, getSize);
if ( getResultCount != getSize )
{
    printf("RingBuffer_Get failed result:%d dest:%d\n", getResultCount, getSize);
}
printf("\nRingBuffer_Get %d ed %d\n", getSize, __LINE__);
showBuffer("pGetBuffer", pGetBuffer, RB_SIZE);
showBuffer("pPutBuffer", pPutBuffer, RB_SIZE);
showBuffer("rbBuffer", rbBuffer, RB_SIZE);

getSize = 5;
getResultCount = RingBuffer_Get(pRB, pGetBuffer, getSize);
if ( getResultCount != getSize )
{
    printf("RingBuffer_Get failed result:%d dest:%d\n", getResultCount, getSize);
}
printf("\nRingBuffer_Get %d ed %d\n", getSize, __LINE__);
showBuffer("pGetBuffer", pGetBuffer, RB_SIZE);
showBuffer("pPutBuffer", pPutBuffer, RB_SIZE);
showBuffer("rbBuffer", rbBuffer, RB_SIZE);
    
    
    
    
    
    
    
    
    
    
    printf("RingBuffer_ResetPool\n");
    if ( RingBuffer_ResetPool(pRB) != 0 )
    {
        printf("RingBuffer_ResetPool failed\n");
        return -1;
    }
    printf("RingBuffer_ResetPool ed %d\n", __LINE__);
    showBuffer("pGetBuffer", pGetBuffer, RB_SIZE);
    showBuffer("pPutBuffer", pPutBuffer, RB_SIZE);
    showBuffer("rbBuffer", rbBuffer, RB_SIZE);
}
