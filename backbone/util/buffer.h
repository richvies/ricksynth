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

#ifndef BUFFER_H
#define BUFFER_H

#include "backbone/common.h"


typedef void BUFFER;
typedef void DATA;


extern void    BUFF_init(void);
extern BUFFER* BUFF_createFifo(void *arr, uint16_t n_elm, size_t sizeof_elm);
extern BUFFER* BUFF_createLifo(void *arr, uint16_t n_elm, size_t sizeof_elm);
extern bool    BUFF_delete(BUFFER *buf);

extern bool     BUFF_push(BUFFER *buf, DATA *data);
extern bool     BUFF_pop(BUFFER *buf, DATA **data);
extern bool     BUFF_write(BUFFER *buf, DATA *data, uint16_t len);
extern uint16_t BUFF_peak(BUFFER *buf, DATA **data);
extern bool     BUFF_read(BUFFER *buf, uint16_t len);


#endif
