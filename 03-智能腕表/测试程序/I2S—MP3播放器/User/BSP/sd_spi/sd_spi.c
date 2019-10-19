#include "sd_spi.h"

static uint8_t SD_Type = 0;
  
void SPI_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;
 
 
  RCC_APB2PeriphClockCmd(SD_SPI_CLK, ENABLE);//使能SPI1时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOb时钟
  //GPIOFB3,4,5初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_MOSI|GPIO_SCK|GPIO_MISO;//PB3~5复用功能输出	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIO时钟
  GPIO_InitStructure.GPIO_Pin = GPIO_NSS;//NSS	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
  CS_H;
	
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1); //PB3复用为 SD_SPI
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1); //PB4复用为 SD_SPI
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1); //PB5复用为 SD_SPI
	
  SPI_I2S_DeInit(SD_SPI);
	
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
  SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
  SPI_Init(SD_SPI, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
  SPI_Cmd(SD_SPI, ENABLE); //使能SPI外设
  SPI_ReadWrite_Byte(0xff);
}
//spi读写一个字节
uint8_t SPI_ReadWrite_Byte(uint8_t TxData)
{     
  uint16_t timeout = 0;
  while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET)//等待发送区空
  {
    timeout++;
    if(timeout>9999) return 0;
  }  
	SPI_I2S_SendData(SD_SPI, TxData); //通过外设SPIx发送一个byte  数据
  timeout = 0;
  while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET) //等待接收完一个byte  
  {
    timeout++;
    if(timeout>9999) return 0;
  }  
	return SPI_I2S_ReceiveData(SD_SPI); //返回通过SPIx最近接收的数据		
}
//设置SPI速率 初始速率不能高于400k
void SD_SPI_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
  SD_SPI->CR1&=0XFFC7;//位3-5清零，用来设置波特率
  SD_SPI->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
  SPI_Cmd(SD_SPI,ENABLE); //使能SPI1
}

/*********************************************************************/
//SD卡的一些操作函数：

//取消片选 释放SPI总线
void SD_DisSelect(void)
{
  CS_H;
  SPI_ReadWrite_Byte(0xff);
}
//等待卡准备好
//准备好了返回值0；其他错误代码
uint8_t SD_WaitReady(void)
{
  uint32_t t=0;
  do
  {
	if(SPI_ReadWrite_Byte(0xff)==0XFF)return 0;//OK
	t++;		  	
  }while(t<0XFFFFFF);
  return 1;
}
//选择SD卡并等待卡准备好
//返回值 0成功，1失败
uint8_t SD_Select(void)
{
  CS_L;
  if(SD_WaitReady()==0)return 0;
  SD_DisSelect();
  return 1;
}
//获得响应
//Response 要得到的回应?
//其他响应错误
uint8_t SD_GetResponse(uint8_t Response)
{
  u16 Count=0xFFFF;   		//等待次数				  
  while ((SPI_ReadWrite_Byte(0xff)!=Response)&&Count)Count--;	  
  if (Count==0)return MSD_RESPONSE_FAILURE;  
  else return MSD_RESPONSE_NO_ERROR;
}
//从SD卡读取一个数据包的内容
//buf数据缓存区
//len读取的数据长度
uint8_t SD_RecvData(uint8_t*buf,u16 len)
{			  	  
  if(SD_GetResponse(0xFE))return 1;
  while(len--)
  {
    *buf=SPI_ReadWrite_Byte(0xff);
     buf++;
  }
  SPI_ReadWrite_Byte(0xff);
  SPI_ReadWrite_Byte(0xff);									  					    
  return 0;
}
//向SD卡写入一个数据包的内容512字节
//buf数据缓存
//命令
//返回值0成功 其他失败
uint8_t SD_SendBlock(uint8_t*buf,uint8_t cmd)
{	
  u16 t;		  	  
  if(SD_WaitReady())return 1;
  SPI_ReadWrite_Byte(cmd);
  if(cmd!=0XFD)
  {
	for(t=0;t<512;t++)SPI_ReadWrite_Byte(buf[t]);
	SPI_ReadWrite_Byte(0xFF);
	SPI_ReadWrite_Byte(0xFF);
	t=SPI_ReadWrite_Byte(0xFF);
	if((t&0x1F)!=0x05)return 2;								  					    
   }						 									  					    
  return 0;
}
//向SD卡发送一个命令
//uint8_t cmd  命令
//uint32_t arg 命令参数
uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
  uint8_t r1;	
  uint8_t Retry=0; 
  SD_DisSelect();
  if(SD_Select())return 0XFF;
    SPI_ReadWrite_Byte(cmd | 0x40);
    SPI_ReadWrite_Byte(arg >> 24);
    SPI_ReadWrite_Byte(arg >> 16);
    SPI_ReadWrite_Byte(arg >> 8);
    SPI_ReadWrite_Byte(arg);	  
    SPI_ReadWrite_Byte(crc); 
  if(cmd==CMD12) SPI_ReadWrite_Byte(0xff);//Skip a stuff byte when stop reading
	Retry=0X1F;
  do
   {
	 r1=SPI_ReadWrite_Byte(0xFF);
   }while((r1&0X80) && Retry--);	 
  return r1;
}
//获取SD卡的CID信息包括 包括制造商信息
//uint8_t *cid_data	存放CID的内存至少16个字节
//返回 0:NO_ERR
//		 1:错误														   
uint8_t SD_GetCID(uint8_t *cid_data)
{
  uint8_t r1;	   
  r1=SD_SendCmd(CMD10,0,0x01);
  if(r1==0x00)
  {
	r1=SD_RecvData(cid_data,16);	//接收16个字节的数据 
  }
  SD_DisSelect();
  if(r1)return 1;
  else return 0;
}																				  
//获取SD卡的CSD信息包括 包括容量和速度信息
//uint8_t *cid_data	存放CsD的内存至少16个字节
//返回 0:NO_ERR
//		 1:错误														   
uint8_t SD_GetCSD(uint8_t *csd_data)
{
  uint8_t r1;	 
  r1=SD_SendCmd(CMD9,0,0x01);
  if(r1==0)
  {
     r1=SD_RecvData(csd_data, 16); 
   }
  SD_DisSelect();
  if(r1)return 1;
  else return 0;
}  
//获取SD卡总扇区数
//返回值:0: 取出容量出错
//      其他:SD容量(扇区数/512字节)														  
uint32_t SD_GetSectorCount(void)
{
  uint8_t csd[16];
  uint32_t Capacity;  
  uint8_t n;
  u16 csize;  					    
  //取CSD信息
  if(SD_GetCSD(csd)!=0) return 0;	    
    
  if((csd[0]&0xC0)==0x40)	 //V2.00卡
  {	
	 csize = csd[9] + ((u16)csd[8] << 8) + 1;
	 Capacity = (uint32_t)csize << 10;//得到扇区数		   
  }else//V1.XX卡
   {	
	  n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
	  csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
	  Capacity= (uint32_t)csize << (n - 9);//得到扇区数 
   }
   return Capacity;
}
//初始化SD
uint8_t SD_Init_Config(void)
{
  uint8_t r1;      // 
  u16 retry;  // 
  uint8_t buf[4];  
  u16 i;
 
  SPI_init();	//初始化IO
  for(i=0;i<10;i++)SPI_ReadWrite_Byte(0XFF);//发送至少74个脉冲
  retry=20;
  do
  {
	 r1=SD_SendCmd(CMD0,0,0x95);//进入IDLE状态
   }while((r1!=0X01) && retry--);
   SD_Type=0;//默认无卡
  if(r1==0X01)
   {
	  if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
	  {
		for(i=0;i<4;i++)buf[i]=SPI_ReadWrite_Byte(0XFF);	//Get trailing return value of R7 resp
		if(buf[2]==0X01&&buf[3]==0XAA)//卡是否支持2.7~3.6V
		{
		  retry=0XFFFE;
		  do
		  {
			 SD_SendCmd(CMD55,0,0X01);	//发送CMD55
			 r1=SD_SendCmd(CMD41,0x40000000,0X01);//发送CMD41
		    }while(r1&&retry--);
		  if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//鉴别SD2.0开始
		  {
			 for(i=0;i<4;i++)buf[i]=SPI_ReadWrite_Byte(0XFF);//得到OCR值
			 if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC;    //检测CCS
			  else SD_Type=SD_TYPE_V2;   
		    }
		 }
		}else//SD V1.x/ MMC	V3
		{
		  SD_SendCmd(CMD55,0,0X01);		//发送CMD55
		  r1=SD_SendCmd(CMD41,0,0X01);	//发送CMD41
		  if(r1<=1)
		  {		
			SD_Type=SD_TYPE_V1;
			retry=0XFFFE;
			do 
			{
			  SD_SendCmd(CMD55,0,0X01);	//发送CMD55
			  r1=SD_SendCmd(CMD41,0,0X01);//发送CMD41
			}while(r1&&retry--);
		  }else
		     {
			   SD_Type=SD_TYPE_MMC;//MMC V3
			   retry=0XFFFE;
			   do 
			    {											    
				  r1=SD_SendCmd(CMD1,0,0X01);
			     }while(r1&&retry--);  
		   }
		  if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;
		}
    SD_DisSelect();
    SD_SPI_SetSpeed(SPI_BaudRatePrescaler_2);
   }
  if(SD_Type)return 0;
  else if(r1)return r1; 	   
  return 0xaa;
}
uint8_t SD_ReadDisk(uint8_t*buf,uint32_t sector,uint16_t cnt)
{
  uint8_t r1;
  if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;
  if(cnt==1)
  {
	r1=SD_SendCmd(CMD17,sector,0X01);
	if(r1==0)
	{
	  r1=SD_RecvData(buf,512);	   
	}
  }else
    {
	   r1=SD_SendCmd(CMD18,sector,0X01);
	   do
	   {
		  r1=SD_RecvData(buf,512);	 
		  buf+=512;  
		}while(--cnt && r1==0); 	
	   SD_SendCmd(CMD12,0,0X01);	
	 }   
  SD_DisSelect();
  return r1;
}
//uint8_t*buf数据缓存
//sector起始扇区
//扇区数
uint8_t SD_WriteDisk(uint8_t*buf,uint32_t sector,uint16_t cnt)
{
  uint8_t r1;
  if(SD_Type!=SD_TYPE_V2HC)sector *= 512;
  if(cnt==1)
  {
	 r1=SD_SendCmd(CMD24,sector,0X01);
	 if(r1==0)
	 {
		r1=SD_SendBlock(buf,0xFE);  
	  }
   }else
    {
	  if(SD_Type!=SD_TYPE_MMC)
	   {
		  SD_SendCmd(CMD55,0,0X01);	
		  SD_SendCmd(CMD23,cnt,0X01);
		}
 	  r1=SD_SendCmd(CMD25,sector,0X01);
	  if(r1==0)
	  {
		do
		{
		  r1=SD_SendBlock(buf,0xFC);	 
		  buf+=512;  
		 }while(--cnt && r1==0);
		 r1=SD_SendBlock(0,0xFD);
		}
	}   
  SD_DisSelect();
  return r1;
}


