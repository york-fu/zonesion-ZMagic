#include "sd_spi.h"

static uint8_t SD_Type = 0;


//spi读写一个字节
#define SD_SPI_ReadWriteByte(TxData)  SPI1_ReadWriteByte(TxData)
//设置SPI速率 初始速率不能高于400k
#define SD_SPI_SetSpeed(SPI_BaudRatePrescaler)  SPI1_SetSpeed(SPI_BaudRatePrescaler)


void SD_GpioInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  GPIO_StructInit(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
  
  RCC_AHB1PeriphClockCmd(SD_SPI_NSS_RCC, ENABLE);//使能GPIO时钟
  GPIO_InitStructure.GPIO_Pin = SD_SPI_NSS_PIN;//NSS	
  GPIO_Init(SD_SPI_NSS_GPIO, &GPIO_InitStructure);//初始化
  SD_CS_H;
  
  SPI1_Init();
}

/*********************************************************************/
//SD卡的一些操作函数：

//等待卡准备好
//准备好了返回值0；其他错误代码
uint8_t SD_WaitReady(void)
{
  uint32_t t=0;
  do
  {
    if(SD_SPI_ReadWriteByte(0xff)==0XFF)return 0;//OK
    t++;		  	
  }while(t<0XFFFFFF);
  return 1;
}

//取消片选 释放SPI总线
void SD_DisSelect(void)
{
  SD_CS_H;
  SD_SPI_ReadWriteByte(0xff);
}

//选择SD卡并等待卡准备好
//返回值 0成功，1失败
uint8_t SD_Select(void)
{
  SD_CS_L;
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
  while ((SD_SPI_ReadWriteByte(0xff)!=Response)&&Count)Count--;	  
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
    *buf=SD_SPI_ReadWriteByte(0xff);
    buf++;
  }
  SD_SPI_ReadWriteByte(0xff);
  SD_SPI_ReadWriteByte(0xff);								  					    
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
  SD_SPI_ReadWriteByte(cmd);
  if(cmd!=0XFD)
  {
    for(t=0;t<512;t++)SD_SPI_ReadWriteByte(buf[t]);
    SD_SPI_ReadWriteByte(0xFF);
    SD_SPI_ReadWriteByte(0xFF);
    t=SD_SPI_ReadWriteByte(0xFF);
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
	uint8_t Retry=0X1F; 
  if(SD_Select())return 0XFF;
  SD_SPI_ReadWriteByte(cmd | 0x40);
  SD_SPI_ReadWriteByte(arg >> 24);
  SD_SPI_ReadWriteByte(arg >> 16);
  SD_SPI_ReadWriteByte(arg >> 8);
  SD_SPI_ReadWriteByte(arg);	  
  SD_SPI_ReadWriteByte(crc); 
  if(cmd==CMD12) SD_SPI_ReadWriteByte(0xff);//Skip a stuff byte when stop reading
  do
  {
    r1=SD_SPI_ReadWriteByte(0xFF);
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
  
  SD_GpioInit();	//初始化IO
  SD_SPI_SetSpeed(SPI_BaudRatePrescaler_256);
  for(i=0;i<10;i++)SD_SPI_ReadWriteByte(0XFF);//发送至少74个脉冲
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
      for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);	//Get trailing return value of R7 resp
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
          for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);//得到OCR值
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
  }
  SD_SPI_SetSpeed(SPI_BaudRatePrescaler_2);
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


