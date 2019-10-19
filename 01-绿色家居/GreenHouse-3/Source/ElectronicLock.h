#ifndef _electronic_lock_
#define _electronic_lock_

#include <ioCC2530.h>                                           //引入CC2530所对应的头文件（包含各SFR的定义）

#define ElectronicLockIO  P1_2

void ElectronicLock_init(void);
void ElectronicLock_on(void);
void ElectronicLock_off(void);

#endif