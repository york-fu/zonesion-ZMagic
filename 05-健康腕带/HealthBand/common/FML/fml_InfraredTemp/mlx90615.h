#ifndef _mlx90615_h_
#define _mlx90615_h_

#include "stm32f4xx.h"	

int mlx90615_init(void);
short mlx90615_GetTemp(float* temp);

#endif