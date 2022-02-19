/****************************************************************************

RickSynth
----------

MIT License

Copyright (c) [2021] [Richard Davies]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
****************************************************************************/


#include "common.h"
#include "config.h"

#include "buffer.h"


typedef enum type_e
{
  TYPE_LIFO,
  TYPE_FIFO,
  TYPE_U8,
  NUM_OF_TYPES,
} type_t;


typedef struct buffer_s
{
  uint16_t num_elm;
  uint16_t sizeof_elm;
  void *arr;

  bool isFree;
  type_t type;
  uint16_t head;
  uint16_t tail;
} buffer_t;


static buffer_t buffers[NUM_OF_BUFFERS];
static void *buffers_end;


static uint16_t _getLevel(BUFFER *buf);
static BUFFER* _create(void *arr, uint16_t n_elm, size_t size_elm, type_t type);
static void _cpy(void *dst, const void *src, size_t len);


void    BUFF_init(void)
{
  uint8_t i;
  buffer_t *buf;

  memset(buffers, 0, sizeof(buffers));
  for (i = 0; i < NUM_OF_BUFFERS; i++)
  {
    buf = &buffers[i];
    buf->isFree = true;
  }

  buffers_end = &buffers[NUM_OF_BUFFERS] + sizeof(buffers[0]);
}

BUFFER* BUFF_createFifo(void *arr, uint16_t n_elm, size_t sizeof_elm)
{
  return _create(arr, n_elm, sizeof_elm, TYPE_FIFO);
}

BUFFER* BUFF_createLifo(void *arr, uint16_t n_elm, size_t sizeof_elm)
{
  return _create(arr, n_elm, sizeof_elm, TYPE_LIFO);
}

bool    BUFF_delete(BUFFER *buf)
{
  buffer_t *_buf = (buffer_t*)buf;
  void *address = (void*)buf;

  if ((address < (void*)buffers)
  ||  (address > buffers_end))
  {
    return false;
  }
  else
  {
    _buf->isFree = false;
    return true;
  }

}


bool     BUFF_push(BUFFER *buf, DATA *data)
{
  buffer_t *b = (buffer_t*)buf;

  switch (b->type)
  {
  case TYPE_U8:
    *(uint8_t*)(b->arr + b->head) = *(uint8_t*)data;
    break;

  default:
    break;
  }

  return true;
}

bool     BUFF_pop(BUFFER *buf, DATA **data)
{
  bool ret = true;

  ret &= BUFF_peak(buf, data);
  ret &= BUFF_read(buf, 1);

  return ret;
}


bool     BUFF_write(BUFFER *buf, DATA *data, uint16_t len)
{
  buffer_t *b = (buffer_t*)buf;

  void *dst;
  uint32_t num_bytes;
  uint16_t len_available;
  uint16_t len_before_wrap;

  len_available = b->num_elm - 1 - _getLevel(buf);
  if (len > len_available)
  {
    return false;
  }

  dst = b->arr + (b->head * b->sizeof_elm);
  len_before_wrap = b->num_elm - 1 - b->head;

  if (len <= len_before_wrap)
  {
    num_bytes = len * b->sizeof_elm;
    _cpy(dst, (const void*)data, num_bytes);
    b->head += len;
  }
  else
  {
    num_bytes = len_before_wrap * b->sizeof_elm;
    _cpy(dst, (const void*)data, num_bytes);

    dst = b->arr;
    data += num_bytes;
    len -= len_before_wrap;
    num_bytes = len * b->sizeof_elm;
    _cpy(dst, (const void*)data, num_bytes);
    b->head = len - 1;
  }

  return true;
}

uint16_t BUFF_peak(BUFFER *buf, DATA **data)
{
  buffer_t *b = (buffer_t*)buf;
  uint16_t len;

  *data = b->arr + (b->tail + b->sizeof_elm);

  if (b->head < b->tail)
  {
    len = b->num_elm - b->tail;
  }
  else
  {
    len = b->head - b->tail;
  }

  return len;
}

bool     BUFF_read(BUFFER *buf, uint16_t len)
{
  buffer_t *b = (buffer_t*)buf;
  uint16_t level = _getLevel(buf);

  if (len >= level)
  {
    b->tail += level;
  }
  else
  {
    b->tail += len;
  }

  if (b->tail >= b->num_elm)
  {
    b->tail -= b->num_elm;
  }

  return true;
}


static uint16_t _getLevel(BUFFER *buf)
{
  buffer_t *b = (buffer_t*)buf;
  int32_t level = b->head - b->tail;

  if (level < 0)
  {
    level += b->num_elm;
  }

  return level;
}

static BUFFER* _create(void *arr, uint16_t n_elm, size_t size_elm, type_t type)
{
  uint8_t i;
  buffer_t *buf;

  for (i = 0; i < NUM_OF_BUFFERS; i++)
  {
    buf = &buffers[i];

    if (buf->isFree)
    {
      buf->isFree = false;
      buf->num_elm = n_elm;
      buf->sizeof_elm = size_elm;
      buf->arr = arr;
      buf->head = 0;
      buf->tail = 0;
      buf->type = type;
      break;
    }
  }

  return (BUFFER*)buf;
}

static void _cpy(void *dst, const void *src, size_t len)
{
  while(len--)
  {
    *(char*)dst++ = *(const char*)src++;
  }
}
