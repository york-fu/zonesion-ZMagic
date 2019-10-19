
#include "rtimer-arch.h"

void rtimer_arch_set(rtimer_clock_t t)
{
}
clock_time_t
clock_time(void);
rtimer_clock_t rtimer_arch_now(void)
{
  return clock_time();
}