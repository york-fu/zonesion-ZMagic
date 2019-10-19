/*********************************************************************************************
* 文件: w25qxx.c
* 作者：zonesion 2016.12.22
* 说明：外部flash驱动文件
* 修改：
* 注释：
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "w25qxx.h"
#include "spi.h"
#include "delay.h"
/*********************************************************************************************
* 全局变量
*********************************************************************************************/

u16 W25QXX_TYPE=W25Q64;	                                        //默认是W25Q64
/*********************************************************************************************
* 名称:W25QXX_Init
* 功能:初始化SPI FLASH的IO口
* 参数:无
* 返回:无
* 修改:
* 注释: 4Kbytes为一个Sector 16个扇区为1个Block 
*       W25Q64容量为8M字节,共有64个Block,2048个Sector
*********************************************************************************************/

void W25QXX_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE );        //PORTA时钟使能
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;                    //PA15输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //输出功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;            //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);                        //初始化
  W25QXX_CS=1;				                        //SPI FLASH不选中
  SPI3_Init();		   	                                //初始化SPI
  SPI3_SetSpeed(SPI_BaudRatePrescaler_2);                       //设置为18M时钟,高速模式
  W25QXX_TYPE=W25QXX_ReadID();                                  //读取FLASH ID. 
}
/*********************************************************************************************
* 名称:W25QXX_ReadSR
* 功能:读取W25QXX的状态寄存器
* 参数:无
* 返回:byte
* 修改:
* 注释: 
*       BIT7  6   5   4   3   2   1   0
*       SPR   RV  TB BP2 BP1 BP0 WEL BUSY
*       SPR:默认0,状态寄存器保护位,配合WP使用
*       TB,BP2,BP1,BP0:FLASH区域写保护设置
*       WEL:写使能锁定
*       BUSY:忙标记位(1,忙;0,空闲)
*       默认:0x004
*********************************************************************************************/
u8 W25QXX_ReadSR(void)
{
  u8 byte=0;
  W25QXX_CS=0;                                                  //使能器件
  SPI3_ReadWriteByte(W25X_ReadStatusReg);                       //发送读取状态寄存器命令
  byte=SPI3_ReadWriteByte(0Xff);                                //读取一个字节
  W25QXX_CS=1;                                                  //取消片选
  return byte;
}
/*********************************************************************************************
* 名称:W25QXX_Write_SR
* 功能:写W25QXX状态寄存器
* 参数:无
* 返回:无
* 修改:
* 注释: 只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
*********************************************************************************************/
void W25QXX_Write_SR(u8 sr)
{
  W25QXX_CS=0;                                                  //使能器件
  SPI3_ReadWriteByte(W25X_WriteStatusReg);                      //发送写取状态寄存器命令
  SPI3_ReadWriteByte(sr);               	                //写入一个字节
  W25QXX_CS=1;                                                  //取消片选
}
/*********************************************************************************************
* 名称:W25QXX_Write_Enable
* 功能:W25QXX写使能，将WEL置位
* 参数:无
* 返回:无
* 修改:
* 注释: 
*********************************************************************************************/

void W25QXX_Write_Enable(void)
{
  W25QXX_CS=0;                          	                //使能器件
  SPI3_ReadWriteByte(W25X_WriteEnable); 	                //发送写使能
  W25QXX_CS=1;                           	                //取消片选
}
/*********************************************************************************************
* 名称:W25QXX_Write_Disable
* 功能:W25QXX写禁止，将WEL清零
* 参数:无
* 返回:无
* 修改:
* 注释: 
*********************************************************************************************/

void W25QXX_Write_Disable(void)
{
  W25QXX_CS=0;                                                  //使能器件
  SPI3_ReadWriteByte(W25X_WriteDisable);                        //发送写禁止指令
  W25QXX_CS=1;                                                  //取消片选
}
//读取芯片ID
//返回值如下:

/*********************************************************************************************
* 名称:W25QXX_ReadID
* 功能:读取芯片ID
* 参数:无
* 返回:Temp：
*       0XEF13,表示芯片型号为W25Q80
*       0XEF14,表示芯片型号为W25Q16
*       0XEF15,表示芯片型号为W25Q32
*       0XEF16,表示芯片型号为W25Q64
*       0XEF17,表示芯片型号为W25Q128
* 修改:
* 注释: 
*********************************************************************************************/

u16 W25QXX_ReadID(void)
{
  u16 Temp = 0;
  W25QXX_CS=0;
  SPI3_ReadWriteByte(0x90);                                     //发送读取ID命令
  SPI3_ReadWriteByte(0x00);
  SPI3_ReadWriteByte(0x00);
  SPI3_ReadWriteByte(0x00);
  Temp|=SPI3_ReadWriteByte(0xFF)<<8;
  Temp|=SPI3_ReadWriteByte(0xFF);
  W25QXX_CS=1;
  return Temp;
}
/*********************************************************************************************
* 名称:W25QXX_ReadID
* 功能:读取SPI FLASH在指定地址开始读取指定长度的数据
* 参数:pBuffer-数据存储区  ReadAddr-开始读取的地址(24bit)
*      NumByteToRead-要读取的字节数(最大65535)
* 返回:无
* 修改:
* 注释: 
*********************************************************************************************/
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)
{
  u16 i;
  W25QXX_CS=0;                            	                //使能器件
  SPI3_ReadWriteByte(W25X_ReadData);         	                //发送读取命令
  SPI3_ReadWriteByte((u8)((ReadAddr)>>16));  	                //发送24bit地址
  SPI3_ReadWriteByte((u8)((ReadAddr)>>8));
  SPI3_ReadWriteByte((u8)ReadAddr);
  for(i=0;i<NumByteToRead;i++)
  {
    pBuffer[i]=SPI3_ReadWriteByte(0XFF);   	                //循环读数
  }
  W25QXX_CS=1;
}


/*********************************************************************************************
* 名称:W25QXX_Write_Page
* 功能:SPI在一页(0~65535)内写入少于256个字节的数据 在指定地址开始写入最大256字节的数据
* 参数:pBuffer-数据存储区 
*       NumByteToWrite-要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
*      NumByteToRead-要读取的字节数(最大65535)
* 返回:无
* 修改:
* 注释: 
*********************************************************************************************/
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
  u16 i;
  W25QXX_Write_Enable();                  	                //SET WEL
  W25QXX_CS=0;                            	                //使能器件
  SPI3_ReadWriteByte(W25X_PageProgram);      	                //发送写页命令
  SPI3_ReadWriteByte((u8)((WriteAddr)>>16)); 	                //发送24bit地址
  SPI3_ReadWriteByte((u8)((WriteAddr)>>8));
  SPI3_ReadWriteByte((u8)WriteAddr);
  for(i=0;i<NumByteToWrite;i++)SPI3_ReadWriteByte(pBuffer[i]);  //循环写数
  W25QXX_CS=1;                            	                //取消片选
  W25QXX_Wait_Busy();					        //等待写入结束
}
/*********************************************************************************************
* 名称:W25QXX_Write_NoCheck
* 功能:无检验写SPI FLASH 
*       必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
* 参数:pBuffer-数据存储区 
*       WriteAddr-开始写入的地址(24bit)
*      NumByteToRead-要读取的字节数(最大65535)
* 返回:无
* 修改:
* 注释: 
*********************************************************************************************/
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
  u16 pageremain;
  pageremain=256-WriteAddr%256;                                 //单页剩余的字节数
  if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;      //不大于256个字节
  while(1)
  {
    W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
    if(NumByteToWrite==pageremain)break;                        //写入结束了
    else //NumByteToWrite>pageremain
    {
      pBuffer+=pageremain;
      WriteAddr+=pageremain;
      
      NumByteToWrite-=pageremain;			        //减去已经写入了的字节数
      if(NumByteToWrite>256)pageremain=256;                     //一次可以写入256个字节
      else pageremain=NumByteToWrite; 	                        //不够256个字节了
    }
  };
}
/*********************************************************************************************
* 名称:W25QXX_BUFFER
* 功能:写SPI FLASH
*       在指定地址开始写入指定长度的数据，该函数带擦除操作!
* 参数:pBuffer-数据存储区 
*       WriteAddr-开始写入的地址(24bit)
*      NumByteToRead-要读取的字节数(最大65535)
* 返回:无
* 修改:
* 注释: 
*********************************************************************************************/
u32 num;
u8 W25QXX_BUFFER[4096];
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u32 NumByteToWrite)
{
  u32 secpos;
  u16 secoff;
  u32 secremain;
  u16 i;
  u8 * W25QXX_BUF;
  W25QXX_BUF=W25QXX_BUFFER;
  secpos=WriteAddr/4096;                                        //扇区地址
  secoff=WriteAddr%4096;                                        //在扇区内的偏移
  secremain=4096-secoff;                                        //扇区剩余空间大小
  if(NumByteToWrite<=secremain)secremain=NumByteToWrite;        //不大于4096个字节
  while(1)
  {
    W25QXX_Read(W25QXX_BUF,secpos*4096,4096);                   //读出整个扇区的内容
    for(i=0;i<secremain;i++)                                    //校验数据
    {
      if(W25QXX_BUF[secoff+i]!=0XFF)break;                      //需要擦除
    }
    if(i<secremain)                                             //需要擦除
    {
      W25QXX_Erase_Sector(secpos);		                //擦除这个扇区
      for(i=0;i<secremain;i++)	   		                //复制
      {
        W25QXX_BUF[i+secoff]=pBuffer[i];
      }
      W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);        //写入整个扇区
      
    }else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);    //写已经擦除了的,直接写入扇区剩余区间.
    if(NumByteToWrite==secremain)break;                         //写入结束了
    else                                                        //写入未结束
    {
      secpos++;                                                 //扇区地址增1
      secoff=0;                                                 //偏移位置为0
      
      pBuffer+=secremain;  				        //指针偏移
      WriteAddr+=secremain;				        //写地址偏移
      NumByteToWrite-=secremain;			        //字节数递减
      num=282744-NumByteToWrite;
      if(NumByteToWrite>4096)secremain=4096;                    //下一个扇区还是写不完
      else secremain=NumByteToWrite;		                //下一个扇区可以写完了
    }
  };
}
/*********************************************************************************************
* 名称:W25QXX_Erase_Chip
* 功能:擦除整个芯片
* 参数:无
* 返回:无
* 修改:
* 注释: 
*********************************************************************************************/

void W25QXX_Erase_Chip(void)
{
  W25QXX_Write_Enable();                 	 	        //SET WEL
  W25QXX_Wait_Busy();
  W25QXX_CS=0;                            	                //使能器件
  SPI3_ReadWriteByte(W25X_ChipErase);        	                //发送片擦除命令
  W25QXX_CS=1;                            	                //取消片选
  W25QXX_Wait_Busy();   				        //等待芯片擦除结束
}
/*********************************************************************************************
* 名称:W25QXX_Erase_Sector
* 功能:擦除一个扇区
*       擦除一个扇区的最少时间:150ms
* 参数:Dst_Addr-扇区地址（根据实际容量设置）
* 返回:无
* 修改:
* 注释: 
*********************************************************************************************/
void W25QXX_Erase_Sector(u32 Dst_Addr)
{
  Dst_Addr*=4096;
  W25QXX_Write_Enable();                  	                //SET WEL
  W25QXX_Wait_Busy();
  W25QXX_CS=0;                            	                //使能器件
  SPI3_ReadWriteByte(W25X_SectorErase);      	                //发送扇区擦除指令
  SPI3_ReadWriteByte((u8)((Dst_Addr)>>16));  	                //发送24bit地址
  SPI3_ReadWriteByte((u8)((Dst_Addr)>>8));
  SPI3_ReadWriteByte((u8)Dst_Addr);
  W25QXX_CS=1;                            	                //取消片选
  W25QXX_Wait_Busy();   				        //等待擦除完成
}
/*********************************************************************************************
* 名称:W25QXX_Wait_Busy
* 功能:等待空闲
* 参数:
* 返回:无
* 修改:
* 注释: 
*********************************************************************************************/
void W25QXX_Wait_Busy(void)
{
  while((W25QXX_ReadSR()&0x01)==0x01);  		        // 等待BUSY位清空
}
/*********************************************************************************************
* 名称:W25QXX_PowerDown
* 功能:进入掉电模式
* 参数:
* 返回:无
* 修改:
* 注释: 
*********************************************************************************************/
void W25QXX_PowerDown(void)
{
  W25QXX_CS=0;                           	 	        //使能器件
  SPI3_ReadWriteByte(W25X_PowerDown);                           //发送掉电命令
  W25QXX_CS=1;                            	                //取消片选
  delay_us(3);                                                  //等待TPD
}

/*********************************************************************************************
* 名称:W25QXX_WAKEUP
* 功能:唤醒
* 参数:
* 返回:无
* 修改:
* 注释: 
*********************************************************************************************/
void W25QXX_WAKEUP(void)
{
  W25QXX_CS=0;                            	                //使能器件
  SPI3_ReadWriteByte(W25X_ReleasePowerDown);	                //send W25X_PowerDown command 0xAB
  W25QXX_CS=1;                            	                //取消片选
  delay_us(3);                            	                //等待TRES1
}
