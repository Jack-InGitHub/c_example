/**
 * @file    ringbuffer2.h
 * @author  oikiou (pq_liu@foxmail.com)
 * @brief   
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

#ifndef RINGBUFFER2_H__
#define RINGBUFFER2_H__

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
    
    uint32_t rIndex;
    uint32_t wIndex;
    
    uint32_t usedSize;
    
    uint32_t bufferSize;
} ringBuffer_t;




#ifdef __cplusplus
}
#endif

#endif
