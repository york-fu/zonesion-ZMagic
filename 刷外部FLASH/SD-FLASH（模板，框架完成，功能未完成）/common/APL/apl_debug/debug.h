#ifndef _debug_h_
#define _debug_h_

#include <stdio.h>
#include "contiki.h"
#include "usart.h"

void debug_init(void);
int debug_input(char c);


#endif