#ifndef __PLUS_TEST_H_
#define __PLUS_TEST_H_

#include <contiki.h>
#include "stm32f4xx_gpio.h"
#include "extend.h"
#include <string.h>
#include "stm32f4xx.h"
#include "w25qxx.h"
#include "ethernet.h"
#include "Relay.h"
#include "drive_led.h"
#include "drive_key.h"
#include "test_ui.h"
#include "key.h"
#include "ethernet.h"
#include "rfUart.h"
#include "hw.h"


extern uint8_t testFlag_key1;
extern uint8_t testFlag_key2;
extern uint8_t testFlag_key3;
extern uint8_t testFlag_key4;
extern uint8_t testFlag_uart1;
extern uint8_t testFlag_flash;
extern uint8_t testFlag_ethernet;

PROCESS_NAME(test_init);
uint8_t test_getStatus(void);

#endif
