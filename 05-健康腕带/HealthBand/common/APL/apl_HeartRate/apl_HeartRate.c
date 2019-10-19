#include "apl_HeartRate/apl_HeartRate.h"
#include "exti.h"
#include "fml_HeartRate/max30102.h"
#include "fml_HeartRate/algorithm.h"

#define BUFFER_LENGTH   100 //data length

uint32_t aun_ir_buffer[BUFFER_LENGTH]; //infrared LED sensor data
uint32_t aun_red_buffer[BUFFER_LENGTH];  //red LED sensor data
int32_t n_spo2;  //SPO2 value
int8_t ch_spo2_valid;  //indicator to show if the SPO2 calculation is valid
int32_t n_heart_rate; //heart rate value
int8_t  ch_hr_valid;  //indicator to show if the heart rate calculation is valid

uint32_t BufferCount = BUFFER_LENGTH+1;

process_event_t HeartRateEvent;


void HeartRate_Get(int32_t* heart_rate)
{
  if(ch_hr_valid)
    *heart_rate = n_heart_rate;
  else
    *heart_rate = 0;
}
    
void Spo2_Get(int32_t* spo2)
{
  if(ch_spo2_valid)
    *spo2 = n_spo2;
  else
    *spo2 = 0;
}

void HeartRateIrq()
{
  if(BufferCount < BUFFER_LENGTH)
  {
    maxim_max30102_read_fifo((aun_red_buffer + BufferCount), (aun_ir_buffer + BufferCount)); //read from MAX30102 FIFO
    BufferCount++;
    if(BufferCount == 100)
      process_post_synch(&HeartRateProcess,HeartRateEvent,NULL);
  }
}

void HeartRateGpioInit()
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //上下拉
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;            //100MHz
  //irq io
  RCC_AHB1PeriphClockCmd(HEART_RATE_IRQ_RCC, ENABLE); 
  GPIO_InitStructure.GPIO_Pin = HEART_RATE_IRQ_PIN ;
  GPIO_Init(HEART_RATE_IRQ_GPIO, &GPIO_InitStructure);                        //初始化
  
  Exti15_10_Init(EXTI_PortSourceGPIOD,EXTI_PinSource12);
  Exti15_10_IrqCall_Set(12,HeartRateIrq);
}

void HeartRateProcessInit()
{
  uint8_t uch_dummy;

  HeartRateGpioInit();
  I2C2_GPIOInit();
  
  maxim_max30102_reset(); //resets the MAX30102
  maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_dummy); //Reads/clears the interrupt status register
  maxim_max30102_init();  //initialize the MAX30102
  
  BufferCount = 0;
  
//  for(uint16_t i = 0; i < BUFFER_LENGTH; i++)
//  {
//    while(HEART_RATE_IRQ == 1); //wait until the interrupt pin asserts
//    maxim_max30102_read_fifo((aun_red_buffer + i), (aun_ir_buffer + i)); //read from MAX30102 
//  }
//    //calculate heart rate and SpO2 after first 150 samples (first 3 seconds of samples)
//    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, BUFFER_LENGTH, aun_red_buffer,
//                                           &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
}

PROCESS(HeartRateProcess, "HeartRateProcess");

/*********************************************************************************************
* 名称：PROCESS_THREAD(HeartRateProcess, ev, data)
* 功能：心率传感器处理进程
* 参数：HeartRateProcess, ev, data
* 返回：无
* 修改：无
*********************************************************************************************/
PROCESS_THREAD(HeartRateProcess, ev, data)
{
  //static struct etimer et_HR;
  
  PROCESS_BEGIN();
  
  HeartRateProcessInit();
  HeartRateEvent = process_alloc_event();
  //process_post_synch(&HeartRateProcess,PROCESS_EVENT_TIMER,NULL);
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev==PROCESS_EVENT_TIMER)
    {
      //etimer_set(&et_HR,100);
    }
    if(ev == HeartRateEvent)
    {
      //calculate heart rate and SpO2 after first 150 samples (first 3 seconds of samples)
      maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, BUFFER_LENGTH, aun_red_buffer,
                                           &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
      BufferCount = 0;
    }
  }
  
  PROCESS_END();
}