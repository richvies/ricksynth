#ifndef __CHIPS_H
#define __CHIPS_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "config_board.h"

#ifdef PCF8575_NUM_OF
  #include "pcf8575/pcf8575.h"
#endif

#ifdef W25_NUM_OF
  #include "w25q/w25q.h"
#endif


#ifdef __cplusplus
}
#endif


#endif
