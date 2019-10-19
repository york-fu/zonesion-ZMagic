#ifndef _apl_cpu_h_
#define _apl_cpu_h_

#include "stm32f4xx.h"
#include "contiki.h"
#include "stdio.h"

#define CPU_USAGE_EN 1

extern unsigned long IdleCount;
extern process_event_t CPUUsageEvent;

unsigned char GetCPUUsage(void);
PROCESS_NAME(CPUUsageProcess);

#endif