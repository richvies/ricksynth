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


#ifndef __STORAGE_H
#define __STORAGE_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "mcu.h"


typedef enum
{
  STG_FIRST,
  STG_EXTERNAL = STG_FIRST,
  STG_NUM_OF,
} stg_e;


extern bool     STG_init(bool first_boot);
extern bool     STG_newFile(stg_e idx, char *fname);
extern bool     STG_openFile(stg_e idx, char *fname);
extern bool     STG_writeFile(stg_e idx,
                              char *fname,
                              void *buf,
                              uint16_t len,
                              uint16_t offset);
extern bool     STG_appendFile(stg_e idx, char *fname, void *buf, uint16_t len);
extern uint16_t STG_readFile(stg_e idx, char *fname, void *buf, uint16_t len);
extern bool     STG_closeFile(stg_e idx, char *fname);


#ifdef __cplusplus
}
#endif


#endif
