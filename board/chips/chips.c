#include "chips.h"

#ifdef PCF8575_NUM_OF
  #include "pcf8575/pcf8575.c"
#endif

#ifdef W25_NUM_OF
  #include "w25q/w25q.c"
#endif
