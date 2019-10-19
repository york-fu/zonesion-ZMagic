#ifndef __BH1750_H__
#define __BH1750_H__

void BH1750GPIOInit(void);
int  bh1750_write_cmd(char cmd);
int bh1750_init(void);
float bh1750_get(void);

#endif