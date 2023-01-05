#ifndef __COMMON_H
#define __COMMON_H


#ifdef __cplusplus
 extern "C" {
#endif


#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SIZEOF(x) (sizeof(x)/sizeof(*x))

// DBG_Printf(INFO_LVL, RAD_ID, x, ##__VA_ARGS__)
#define PRINTF_VERB(x, ...)
#define PRINTF_INFO(x, ...)
#define PRINTF_WARN(x, ...)
#define PRINTF_ERRO(x, ...)

#define MIN(x,y)  (a < b ? a : b)
#define MAX(x,y)  (a > b ? a : b)

/* Generic data queue macros, can use as FIFO or LIFO
 *
 * ATTN: must follow this naming convention
 *
 * buf    - the queue
 * tail   - tail index
 * count  - current count
 *
 *
 * e.g. structure
 * typedef struct
 * {
 *  my_type_t buf[10];
 *  uint8_t   tail = 0;
 *  uint8_t   count = 0;
 * } my_queue_t;
 *
 *
 * e.g. usage
 *
 * my_queue_t my_q;
 *
 * Q_clear(my_q);
 *
 * // adding items
 * if (false == Q_isFull(my_q))
 * {
 *    Q_getHead(my_q) = 666;
 *    Q_incHead(my_q);
 * }
 *
 * // removing FIFO
 * if (true == Q_pending(my_q))
 * {
 *  uin32_t tmp = Q_getTail(my_q);
 *  Q_incTail(my_q)
 * }
 *
 * // removing LIFO
 * if (true == Q_pending(my_q))
 * {
 *  uin32_t tmp = Q_getHead(my_q);
 *  Q_decHead(my_q)
 * }
 */

#define Q_clear(q)          ( q.tail = 0; q.count = 0; )
#define Q_pending(q)        ({ q.count > 0; })
#define Q_isFull(q)         ({ q.count >= SIZEOF(q.buf); })

#define Q_getHead(q)        ( q.buf[q.tail + q.count] )
#define Q_incHead(q)        { ++q.count; }
#define Q_getTail(q)        ( q.buf[q.tail] )
#define Q_incTail(q)        { --q.count; ++q.tail; if (q.tail >= SIZEOF(q.buf)) {q.tail = 0;} }

/* for LIFO use */
#define Q_decHead(q)        { --q.count; }


#ifdef __cplusplus
}
#endif


#endif