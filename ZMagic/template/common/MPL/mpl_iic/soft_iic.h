#ifndef _soft_iic_
#define _soft_iic_

#include "stm32f4xx.h" 


void IIC_DelayUs(uint16_t t);

//I2C
#define I2C_SCL_RCC     RCC_AHB1Periph_GPIOA
#define I2C_SCL_GPIO    GPIOA
#define I2C_SCL_PIN     GPIO_Pin_8

#define I2C_SDA_RCC     RCC_AHB1Periph_GPIOC
#define I2C_SDA_GPIO    GPIOC
#define I2C_SDA_PIN     GPIO_Pin_9

#define I2C_SDA_PIN_NUM 9

#define SDA_IN          do{I2C_SDA_GPIO->MODER &= ~(3<<(I2C_SDA_PIN_NUM*2)); I2C_SDA_GPIO->MODER |= (0<<(I2C_SDA_PIN_NUM*2));}while(0)
#define SDA_OUT         do{I2C_SDA_GPIO->MODER &= ~(3<<(I2C_SDA_PIN_NUM*2)); I2C_SDA_GPIO->MODER |= (1<<(I2C_SDA_PIN_NUM*2));}while(0)

#define SCL_L           (I2C_SCL_GPIO->BSRRH=I2C_SCL_PIN)
#define SCL_H           (I2C_SCL_GPIO->BSRRL=I2C_SCL_PIN)

#define SDA_L           (I2C_SDA_GPIO->BSRRH=I2C_SDA_PIN)
#define SDA_H           (I2C_SDA_GPIO->BSRRL=I2C_SDA_PIN)

#define SDA_R           (I2C_SDA_GPIO->IDR&I2C_SDA_PIN)


void I2C_GPIOInit(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);
int I2C_WaitAck(void);
void I2C_WriteByte(uint8_t SendByte); //数据从高位到低位
uint8_t I2C_ReadByte(void);

/**********************************************************************************************/
//I2C1
#define I2C1_SCL_RCC     RCC_AHB1Periph_GPIOB
#define I2C1_SCL_GPIO    GPIOB
#define I2C1_SCL_PIN     GPIO_Pin_10

#define I2C1_SDA_RCC     RCC_AHB1Periph_GPIOB
#define I2C1_SDA_GPIO    GPIOB
#define I2C1_SDA_PIN     GPIO_Pin_11

#define I2C1_SDA_PIN_NUM 11

#define SDA1_IN          do{I2C1_SDA_GPIO->MODER &= ~(3<<(I2C1_SDA_PIN_NUM*2)); I2C1_SDA_GPIO->MODER |= (0<<(I2C1_SDA_PIN_NUM*2));}while(0)
#define SDA1_OUT         do{I2C1_SDA_GPIO->MODER &= ~(3<<(I2C1_SDA_PIN_NUM*2)); I2C1_SDA_GPIO->MODER |= (1<<(I2C1_SDA_PIN_NUM*2));}while(0)

#define SCL1_L           (I2C1_SCL_GPIO->BSRRH=I2C1_SCL_PIN)
#define SCL1_H           (I2C1_SCL_GPIO->BSRRL=I2C1_SCL_PIN)

#define SDA1_L           (I2C1_SDA_GPIO->BSRRH=I2C1_SDA_PIN)
#define SDA1_H           (I2C1_SDA_GPIO->BSRRL=I2C1_SDA_PIN)

#define SDA1_R           (I2C1_SDA_GPIO->IDR&I2C1_SDA_PIN)


void I2C1_GPIOInit(void);
void I2C1_Start(void);
void I2C1_Stop(void);
void I2C1_Ack(void);
void I2C1_NoAck(void);
int I2C1_WaitAck(void);
void I2C1_WriteByte(uint8_t SendByte); //数据从高位到低位
uint8_t I2C1_ReadByte(void);

#endif