#include "stm32f4xx.h"

#define LEDS_RCC        RCC_AHB1Periph_GPIOA
#define LEDS_GPIO       GPIOA
#define LEDS_PIN        GPIO_Pin_12

#define LEDV_RCC        RCC_AHB1Periph_GPIOD
#define LEDV_GPIO       GPIOD
#define LEDV_PIN        GPIO_Pin_11


static unsigned char led_status = 0;
void leds_arch_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(LEDS_RCC | LEDV_RCC, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin =  LEDS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(LEDS_GPIO, &GPIO_InitStructure);  
  
  GPIO_InitStructure.GPIO_Pin =  LEDV_PIN;
  GPIO_Init(LEDV_GPIO, &GPIO_InitStructure);  
  
  GPIO_SetBits(LEDS_GPIO,  LEDS_PIN);
  GPIO_SetBits(LEDV_GPIO,  LEDV_PIN);
} 
unsigned char leds_arch_get(void)
{
  return led_status;
}
void leds_arch_set(unsigned char leds)
{
  led_status = leds;
  if (leds & 0x01) 
    GPIO_ResetBits(LEDS_GPIO, LEDS_PIN);
  else 
    GPIO_SetBits(LEDS_GPIO, LEDS_PIN);
  if (leds & 0x02) 
    GPIO_SetBits(LEDV_GPIO, LEDV_PIN);
  else 
    GPIO_ResetBits(LEDV_GPIO, LEDV_PIN);
  //if (leds & 0x04) pin_on |= GPIO_Pin_5;
  //else pin_off |= GPIO_Pin_5;
  //if (leds & 0x08) pin_on |= GPIO_Pin_6;
  //else pin_off |= GPIO_Pin_6;
//  GPIO_ResetBits(LEDS_GPIO, pin_on); //on
//  GPIO_SetBits(LEDS_GPIO, pin_off); //off
  /*
  pin_on = pin_off = 0;
  if (leds & 0x04) pin_on |= GPIO_Pin_0;
  else pin_off |= GPIO_Pin_0;
  if (leds & 0x08) pin_on |= GPIO_Pin_1;
  else pin_off |= GPIO_Pin_1;
  if (leds & 0x10) pin_on |= GPIO_Pin_2;
  else pin_off |= GPIO_Pin_2;  
  GPIO_ResetBits(GPIOB, pin_on); //on
  GPIO_SetBits(GPIOB, pin_off); //off  
  */
}