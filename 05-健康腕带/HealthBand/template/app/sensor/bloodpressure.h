#ifndef _BLOODPRESSURE_
#define _BLOODPRESSURE_

#include <contiki.h>
#include <stdio.h>
#include "string.h"
#include <stdlib.h>

#define BP_VERSION 2


void blood_init(void);
extern void blood_stop(void);
extern void blood_start(void);
void blood_result(void);

int16_t SBP_Get(void);
int16_t DBP_Get(void);
int16_t HR_Get(void);

#endif
