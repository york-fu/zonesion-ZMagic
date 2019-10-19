#include "IIC1.h"

/*********************************************************************************************
* 名称：GPIO_init
* 功能：功能初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void  GPIO_IIC_init(void)
{    //pc5 pd3 pb4
  GPIO_InitTypeDef  GPIO_InitStructure,GPIO_InitStructure2,GPIO_InitStructure3;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能GPIOB时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //使能GPIOC时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //使能GPIOD时钟
  //先初始化PC5 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;              //PC5 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;          //输出
  GPIO_InitStructure.GPIO_OType= GPIO_OType_OD;          //开漏
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;         //上拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);                //初始化  
   //初始化PD3 IO口
  GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_3;              //PD3 
  GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_OUT;          //输出
  GPIO_InitStructure2.GPIO_OType= GPIO_OType_OD;          //开漏
  GPIO_InitStructure2.GPIO_PuPd = GPIO_PuPd_UP;          //上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure2);                //初始化  
  //初始化PB4
  GPIO_InitStructure3.GPIO_Pin = GPIO_Pin_4;              //PB4 
  GPIO_InitStructure3.GPIO_Mode = GPIO_Mode_OUT;          //输出
  GPIO_InitStructure3.GPIO_OType= GPIO_OType_OD;          //开漏
  GPIO_InitStructure3.GPIO_PuPd = GPIO_PuPd_UP ;         //上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure3);                //初始化
  
  GPIO_ResetBits(GPIOD, GPIO_Pin_3);                     //使能
  SDA_1();
  SCL_1();
}


/********************************************************
*名称：I2C_Start
*功能：开始信号
*参数：无
*返回：无
*********************************************************/
void START_bit(void)
{      
  SDA_OUT();
  delay_us(5);
  SDA_1();        // Set SDA line
  delay_us(1);	// Wait a few microseconds
  SCL_1();	// Set SCL line
  delay_us(30);	// Generate bus free time between Stop
                  // and Start condition (Tbuf=4.7us min)
  SDA_0();	// Clear SDA line
  delay_us(30);	// Hold time after (Repeated) Start
                  // Condition. After this period, the first clock is generated.
                  //(Thd:sta=4.0us min)
  SCL_0();	// Clear SCL line 
  delay_us(30);	// Wait a few microseconds		
}
/********************************************************
*名称：I2C_Stop
*功能：停止信号
*参数：无
*返回：无
*********************************************************/

void STOP_bit(void)
{       
  SDA_OUT();
  SCL_0();	// Clear SCL line
  delay_us(2);	// Wait a few microseconds
  SDA_0();	// Clear SDA line
  delay_us(2);	// Wait a few microseconds
  SCL_1();    // Set SCL line
  delay_us(2);	// Stop condition setup time(Tsu:sto=4.0us min)
  SDA_1();	// Set SDA line
}

unsigned char TX_byte(unsigned char Tx_buffer)
{
    unsigned char	Bit_counter;
    unsigned char	Ack_bit;
    unsigned char	bit_out;

    for(Bit_counter=8; Bit_counter; Bit_counter--)
    {
      if(Tx_buffer&0x80) {bit_out=1; }
      else {bit_out=0;} 
      send_bit(bit_out); // Send the current bit on SDA
      Tx_buffer<<=1;     // Get next bit for checking
    }
    Ack_bit=Receive_bit();	// Get acknowledgment bit	
    return	Ack_bit;
} 


//---------------------------------------------------------------------------------------------
void send_bit(unsigned char bit_out)
{       
  SDA_OUT();
  if(bit_out) {SDA_1();}  
  else	      {SDA_0();}
  delay_us(1);   // Tsu:dat = 250ns minimum
  SCL_1();    // Set SCL line
  delay_us(30);  // High Level of Clock Pulse------------------
  SCL_0();    // Clear SCL line
  delay_us(30);  // Low Level of Clock Pulse----------------------
  //mSDA_HIGH(); // Master release SDA line ,
  return;
}

//--------------------------------------------------------------
unsigned char Receive_bit(void)
{
    unsigned char Ack_bit;
    
    SDA_IN();						
    delay_us(3);
    SCL_1();					
    delay_us(30);			
    if(SDA_read() == 1) {Ack_bit=1;}		
    else  { Ack_bit=0;	}		
    SCL_0();						
    delay_us(20);			
    return	Ack_bit;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
unsigned char RX_byte(unsigned char ack_nack)
{
    unsigned char 	RX_buffer;
    unsigned char	Bit_Counter;
	
    for(Bit_Counter=8; Bit_Counter; Bit_Counter--)
      {
	if(Receive_bit())		
          {
            RX_buffer <<= 1;	
            RX_buffer |=0x01;
          }
        else
          {
            RX_buffer <<= 1;	
            RX_buffer &=0xfe;	
          }
      }
    send_bit(ack_nack);		
    return RX_buffer;
}

