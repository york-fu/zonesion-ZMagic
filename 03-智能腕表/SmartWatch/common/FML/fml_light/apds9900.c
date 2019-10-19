#include "fml_light/apds9900.h"
#include "soft_iic.h"

int sensorID = 0x01;

int apds9900_readReg(unsigned char regAddr)
{
  unsigned char data;
  I2C_Start();
  I2C_WriteByte(APDS9900_ADDR & 0xFE);
  if(I2C_WaitAck()) return -1;
  I2C_WriteByte(regAddr);
  if(I2C_WaitAck()) return -1;
  I2C_Start();
  I2C_WriteByte(APDS9900_ADDR | 0x01);
  if(I2C_WaitAck()) return -1;
  data = I2C_ReadByte();
  I2C_Stop();
  return data;
}
  
int apds9900_readRegTwo(unsigned char regAddr)
{
  unsigned char data[2] = {0};
  I2C_Start();
  I2C_WriteByte(APDS9900_ADDR & 0xFE);
  if(I2C_WaitAck()) return -1;
  I2C_WriteByte(regAddr);
  if(I2C_WaitAck()) return -1;
  I2C_Start();
  I2C_WriteByte(APDS9900_ADDR | 0x01);
  if(I2C_WaitAck()) return -1;
  data[0] = I2C_ReadByte();
  I2C_Ack();
  data[1] = I2C_ReadByte();
  I2C_NoAck();
  I2C_Stop();
  return data[1] << 8 | data[0];
}

int apds9900_writeReg(unsigned char regAddr, unsigned char data)
{
  I2C_Start();
  I2C_WriteByte(APDS9900_ADDR & 0xFE);
  if(I2C_WaitAck()) return -1;
  I2C_WriteByte(regAddr);
  if(I2C_WaitAck()) return -1;
  I2C_WriteByte(data);
  if(I2C_WaitAck()) return -1;
  I2C_Stop();
  return 0;
}

void apds9900_init(void)
{
  I2C_GPIOInit();
  
  sensorID = apds9900_readReg(0x12);
  if(sensorID < 0)
  {
    apds9900_writeReg(0x00, 0x03);
    sensorID = OTHER;
  }
  else
  {
    sensorID = APDS9900;
    int v = apds9900_readReg(0x11);
    apds9900_writeReg(0x00, 0x00); 
    apds9900_writeReg(0x0E, 0x01);                               
    apds9900_writeReg(0x0F, 0x20);  
    apds9900_writeReg(0x00, 0x0F);
  }
}

int get_apds9900Lux(void)
{
  float GA = 0x48;
  float B = 2.23;
  float C = 0.7;
  float D = 1.42;
  float DF = 52;
  float ALSIT = 2.72;
  int AGAIN = 1;
  float LPC = GA * DF / (ALSIT * AGAIN);
  int CDataL = 0;
  int IRDATAL = 0;
  if(sensorID == APDS9900)
  {
    CDataL = apds9900_readRegTwo(0x14 | 0xA0);
    IRDATAL = apds9900_readRegTwo(0x16 | 0xA0);
  }
  else
  {
    CDataL = apds9900_readReg(0x0C);
    IRDATAL = apds9900_readReg(0x0D); 
  }
  if(CDataL < 0 || IRDATAL < 0) return -1;
  float IAC1 = CDataL - B * IRDATAL;
  float IAC2 = C * CDataL - D * CDataL;
  float IAC = IAC1;
  IAC = (IAC2 > IAC) ? IAC2 : IAC;
  IAC = (IAC < 0) ? 0 : IAC;
  int Lux = (int)(IAC * LPC);
  return Lux;
}

int get_apds9900PromixitStatus(void)
{
  int status = 0;
  if(sensorID == APDS9900)
    status = apds9900_readRegTwo(0x18);
  else
    status = apds9900_readReg(0x0F);
  if(status < 0) return -1;
  return status;
}







