/*********************************************************************************************************//**
 * @file    dm9051_stm.c
 * @version V1.01
 * @date    2014/6/10
 * @brief   The DM9051 driver for uCOSII+LwIP & uIP on STM32F103.
 *************************************************************************************************************
 *
 * <h2><center>Copyright (C) 2014 DAVICOM Semiconductor Inc. All rights reserved</center></h2>
 *
 ************************************************************************************************************/

/* Includes ------------------------------------------------------------------------------------------------*/
#include "dm9051_stm.h"

#ifdef uIP_NOOS
#include  "uip.h"
#include "uipopt.h"
#endif	//uIP_NOOS

/* Private typedef -----------------------------------------------------------------------------------------*/
enum DM9000_PHY_mode
{
    DM9000_10MHD   = 0,
    DM9000_100MHD  = 1,
    DM9000_10MFD   = 4,
    DM9000_100MFD  = 5,
    DM9000_10M     = 6,
    DM9000_AUTO    = 8,
    DM9000_1M_HPNA = 0x10
};

enum DM9000_TYPE
{
    TYPE_DM9000E,
    TYPE_DM9000A,
    TYPE_DM9000B,
    TYPE_DM9051
};

#if 1
struct dm9000_eth
{
    enum DM9000_TYPE type;
    enum DM9000_PHY_mode mode;

    uint8_t  imr_all;

    uint8_t  packet_cnt;                    /* packet I or II                                                 */
    uint16_t queue_packet_len;              /* queued packet (packet II)                                      */

    /* interface address info.                                                                                */
    uint8_t  dev_addr[6];                   /* hw address                                                     */

    //u32_t txb_count;	//Byte counter
    //u32_t rxb_count;

};
#endif	//0

/* Private constants ---------------------------------------------------------------------------------------*/
#define DM9000_PHY              (0x40)    /* PHY address 0x01                                               */

/* Private function prototypes -----------------------------------------------------------------------------*/

static u16 phy_read(u32 uReg);
static void phy_write(u16 uReg, u16 uValue);
static void phy_mode_set(u32 uMediaMode);

static void _dm9000_Delay(u32 uDelay);
static void _dm9000_Delay_ms(u32 uDelay);

static void dm9051_Configuration(void);
static void DM9051_Write_Reg(u8 Reg_Off, u8 spi_data);
static u8 DM9051_Read_Reg(u8 Reg_Off);
static void DM9051_Write_Mem(u8* pu8data, u16 datalen);
static void DM9051_Read_Mem(u8* pu8data, u16 datalen);

void NVIC_Configuration(void);

#if (DM9000_DEBUG == 1)
#define DM9000_TRACE  printf
#else
#define DM9000_TRACE(...)
#endif

#if (DM9000_MSG == 1)
#define DM9000_TRACE2  printf
#else
#define DM9000_TRACE2(...)
#endif

/* Global variables ----------------------------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------------------------------------*/
static struct dm9000_eth dm9000_device;

#ifndef uIP_NOOS
//static OS_EVENT *dm9000_sem_ack;		//For TX interrupt

static OS_EVENT *dm9000_sem_lock;

#ifdef DMA_INT
static OS_EVENT *dm9051_txdma_flag;
static OS_EVENT *dm9051_rxdma_flag;
#endif //DMA_INT

#endif	//uIP_NOOS

#ifdef SPI_DMA
//__IO uint32_t CurrDataCounterEnd = 0x01; /* This variable should not be initialized to 0 */
uint8_t RxComplete;
uint8_t TxComplete;
#endif	//SPI_DMA

//static u32 uGPIO_PIN = DM9K_DATA_GPIO_LO;

/* Global functions ----------------------------------------------------------------------------------------*/

/* -----------------------------------------------------*/
/* DM9051_Write_Mem()																		*/
/*																											*/
/* DM9051 burst write command: SPI_WR_BURST = 0xF8			*/
/*                   */
/*------------------------------------------------------*/
static void DM9051_Write_Mem(u8* pu8data, u16 datalen)
{
    uint32_t i;
//    uint8_t err;

    // Send the array to the slave
    u8 burstcmd = SPI_WR_BURST;

    SPI_DM9051_CS_LOW();

    SPI_DM9051_SendByte(burstcmd);
#ifndef SPI_DMA
    for(i=0; i< datalen; i++)
    {
        SPI_DM9051_SendByte(pu8data[i]);
    }
    SPI_DM9051_CS_HIGH();
#else
    TxComplete = 0;

    ((DMA_Channel_TypeDef *)SPI_MASTER_Tx_DMA_Channel)->CMAR = (uint32_t)pu8data;
    //*(uint32_t*)DMA_CNDTR3_TX_REG = (uint32_t)datalen;		//save DMA buffersize
    ((DMA_Channel_TypeDef *)SPI_MASTER_Tx_DMA_Channel)->CNDTR = (uint32_t)datalen;
    DMA_Cmd(SPI_MASTER_Tx_DMA_Channel, ENABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);	//TX DMA request, start move data

#ifdef DMA_INT
    OSSemPend(dm9051_txdma_flag, 0,&err);
    //while(!TxComplete);
#else
    while(!DMA_GetFlagStatus(SPI_MASTER_Tx_DMA_FLAG));
    //DMA_ClearFlag(SPI_MASTER_Tx_DMA_FLAG);

    //SPI_DM9051_CS_HIGH();
#endif	//DMA_INT

    DMA_ClearFlag(SPI_MASTER_Tx_DMA_FLAG);
    SPI_DM9051_CS_HIGH();

    DMA_Cmd(SPI_MASTER_Tx_DMA_Channel, DISABLE);
    SPI_DM9051_CS_HIGH();
#endif	//SPI_DMA	

}

/* -----------------------------------------------------*/
/* DM9051_Read_Mem()																		*/
/*																											*/
/* DM9051 burst read command: SPI_RD_BURST = 0x72 			*/
/*                   */
/*------------------------------------------------------*/
static void DM9051_Read_Mem(u8* pu8data, u16 datalen)
{
    u32 i;
//    uint32_t mccr;
//    uint8_t err;

    // Read SPI_Data_Array back from the slave
    u8 burstcmd = SPI_RD_BURST;

    SPI_DM9051_CS_LOW();

    SPI_DM9051_SendByte(burstcmd);
#ifndef SPI_DMA
    for(i=0; i<datalen; i++)
    {
        pu8data[i] = SPI_DM9051_SendByte(0x0);
    }
    SPI_DM9051_CS_HIGH();
#else
    RxComplete = 0;

    //DMA_CNDTR2_RX_REG = (uint32_t)datalen;	//save DMA buffersize
    ((DMA_Channel_TypeDef *)SPI_MASTER_Rx_DMA_Channel)->CMAR = (uint32_t)pu8data;
    ((DMA_Channel_TypeDef *)SPI_MASTER_Rx_DMA_Channel)->CNDTR = (uint32_t)datalen;
    DMA_Cmd(SPI_MASTER_Rx_DMA_Channel, ENABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);	//RX DMA request, start move data

    /* Dummy TX to generate SPI clock for RX*/
    mccr = ((DMA_Channel_TypeDef *)SPI_MASTER_Tx_DMA_Channel)->CCR;		//Save DMA channel 3 configuration register
    ((DMA_Channel_TypeDef *)SPI_MASTER_Tx_DMA_Channel)->CCR = (mccr & 0xffffff31);	// Peripheral & memory doesn't auto increment. Interrupt disable
    ((DMA_Channel_TypeDef *)SPI_MASTER_Tx_DMA_Channel)->CMAR = (uint32_t)pu8data;
    ((DMA_Channel_TypeDef *)SPI_MASTER_Tx_DMA_Channel)->CNDTR = (uint32_t)datalen;
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, DISABLE);		//Diable Interrupt
    DMA_Cmd(SPI_MASTER_Tx_DMA_Channel, ENABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);	//TX DMA request, start move data

#ifdef DMA_INT
    OSSemPend(dm9051_rxdma_flag, 0,&err);
    //while(!RxComplete);
#else
    while(!DMA_GetFlagStatus(SPI_MASTER_Rx_DMA_FLAG));
    while(!DMA_GetFlagStatus(SPI_MASTER_Tx_DMA_FLAG));

    //DMA_ClearFlag(SPI_MASTER_Rx_DMA_FLAG);
    //DMA_ClearFlag(SPI_MASTER_Tx_DMA_FLAG);

    //SPI_DM9051_CS_HIGH();
#endif	//DMA_INT

    DMA_ClearFlag(SPI_MASTER_Rx_DMA_FLAG);
    DMA_ClearFlag(SPI_MASTER_Tx_DMA_FLAG);

    SPI_DM9051_CS_HIGH();

    DMA_Cmd(SPI_MASTER_Rx_DMA_Channel, DISABLE);

    /* Restore Tx setting */
    DMA_Cmd(SPI_MASTER_Tx_DMA_Channel, DISABLE);
    ((DMA_Channel_TypeDef *)SPI_MASTER_Tx_DMA_Channel)->CCR = mccr;
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
#endif	//SPI_DMA 	


}

/* -----------------------------------------------------*/
/* DM9051_Write_Reg()																		*/
/*																											*/
/* SPI write command: bit7 = 1, 											*/
/*                   bit[6:0] = Reg. offset addr		*/
/*------------------------------------------------------*/
static void DM9051_Write_Reg(u8 Reg_Off, u8 spi_data)
{
    u8 cmdaddr;

    cmdaddr = (Reg_Off | 0x80);

    SPI_DM9051_CS_LOW();

    // SPI transfer DM9051 Read-Command and Reg. offset.
    SPI_DM9051_SendByte(cmdaddr);		//Read command + Register offset address
    SPI_DM9051_SendByte(spi_data);				//

    SPI_DM9051_CS_HIGH();

    return;
}

/* -----------------------------------------------------*/
/* DM9051_Read_Reg()																		*/
/*																											*/
/* SPI read command: bit7 = 0, 													*/
/*                   bit[6:0] = Reg. offset addr				*/
/*------------------------------------------------------*/
static u8 DM9051_Read_Reg(u8 Reg_Off)
{
    u8 spi_data;

    SPI_DM9051_CS_LOW();

    // SPI transfer DM9051 Read-Command and Reg. offset.
    SPI_DM9051_SendByte(Reg_Off);		//Read command + Register offset address

    spi_data = SPI_DM9051_SendByte(0x0);		//Dummy for read register value.

    SPI_DM9051_CS_HIGH();

    return spi_data;
}

#if 0
void dm9051_reset(void)
{
    printf("dm9k_reset...\n");

    DM9051_Write_Reg(DM9000_NCR, DM9000_REG_RESET);			/* 對 DM9000A 進行軟件重置 */
    _dm9000_Delay_ms(1);								/* delay 10us */
    DM9051_Write_Reg(DM9000_NCR, DM9000_REG_RESET);			/* 對 DM9000A 進行軟件重置 */
    _dm9000_Delay_ms(1);								/* delay 10us */

    /* 基本記存器相關設置 */
//	DM9051_Write_Reg(DM9000_NCR, 0x00);						/* 對 DM9000A 進行軟件重置 */
//	DM9051_Write_Reg(DM9051_IMR, DM9000_IMR_OFF); 			/* 開啟內存自環模式 */
    DM9051_Write_Reg(DM9000_TCR2, DM9000_TCR2_SET);			/* 設置 LED 顯示模式 */

    /* 清除多餘資訊 */
    DM9051_Write_Reg(DM9000_NSR, 0x2c);
    DM9051_Write_Reg(DM9000_TCR, 0x00);
    //DM9051_Write_Reg(DM9051_ISR, 0x3f);

    //iow(0x38, 0x6b);
    //iow(0x39, 0x00);

//#ifdef DM9000A_FLOW_CONTROL
    DM9051_Write_Reg(DM9000_BPTR, DM9000_BPTR_SET);			/* 半雙工流控設置 */
    DM9051_Write_Reg(DM9000_FCTR, DM9000_FCTR_SET);			/* 全雙工流控設置 */
    DM9051_Write_Reg(DM9000_FCR, DM9000_FCR_SET);			/* 開啟流控設置 */
//#endif


#ifdef  Rx_Int_enable
    iow(DM9051_REG_IMR, DM9000_IMR_SET);			/* 開啟 中斷模式 */
#else
    iow(DM9051_REG_IMR, DM9000_IMR_OFF);			/* 關閉 中斷模式 */
#endif


    DM9051_Write_Reg(DM9000_RCR, DM9000_RCR_SET);			/* 開啟 接收工能 */

    //SendPackOk = 0;
}
#endif	//0

/*********************************************************************************************************//**
  * @brief  DM9051 init function.
  * @retval -1 or 0
  ***********************************************************************************************************/
#ifdef uIP_NOOS
s32 dm9051_init()
#else
s32 dm9051_init(struct netif *netif)
#endif
{
    int i, oft, lnk;
    uint32_t value;

    dm9051_Configuration();

#if 0
    //netif->state = (void *)&dm9000_device;
    netif->txb_count = 0;
    netif->rxb_count = 0;
    netif->rx_reset = 0;
    netif->tx_reset = 0;
#endif	//0

    /* identfy DM9051 */                       
    value = DM9051_Read_Reg(DM9000_VIDL);
    value |= DM9051_Read_Reg(DM9000_VIDH) << 8;
    value |= DM9051_Read_Reg(DM9000_PIDL) << 16;
    value |= DM9051_Read_Reg(DM9000_PIDH) << 24;

    if (value != DM9051_ID)
    {
        /* one more retry */
        value  = DM9051_Read_Reg(DM9000_VIDL);
        value |= DM9051_Read_Reg(DM9000_VIDH) << 8;
        value |= DM9051_Read_Reg(DM9000_PIDL) << 16;
        value |= DM9051_Read_Reg(DM9000_PIDH) << 24;

        if (value != DM9051_ID)
        {
            DM9000_TRACE2("DM9051 id invalid: 0x%X \n", value);
            return -1;
        }
    }
    DM9000_TRACE2("DM9051 id: 0x%x \n", value);

    dm9000_device.type  = TYPE_DM9051;
#ifdef FORCE_10M
    dm9000_device.mode    = DM9000_10M;
#else
    dm9000_device.mode    = DM9000_AUTO;
#endif	//FORCE_10M

    dm9000_device.packet_cnt = 0;
    dm9000_device.queue_packet_len = 0;

    /* SRAM Tx/Rx pointer automatically return to start address                                               */
    /* Packet Transmitted, Packet Received                                                                    */
    dm9000_device.imr_all = IMR_PAR | IMR_PRM;
    //dm9000_device.imr_all = IMR_DEFAULT;
    //dm9000_device.imr_all = 0x81;

#ifdef uIP_NOOS
    dm9000_device.dev_addr[0] = emacETHADDR0;
    dm9000_device.dev_addr[1] = emacETHADDR1;
    dm9000_device.dev_addr[2] = emacETHADDR2;
    dm9000_device.dev_addr[3] = emacETHADDR3;
    dm9000_device.dev_addr[4] = emacETHADDR4;
    dm9000_device.dev_addr[5] = emacETHADDR5;
#else
    dm9000_device.dev_addr[0] = netif->hwaddr[0];
    dm9000_device.dev_addr[1] = netif->hwaddr[1];
    dm9000_device.dev_addr[2] = netif->hwaddr[2];
    dm9000_device.dev_addr[3] = netif->hwaddr[3];
    dm9000_device.dev_addr[4] = netif->hwaddr[4];
    dm9000_device.dev_addr[5] = netif->hwaddr[5];
#endif


    //dm9000_sem_ack = OSSemCreate(0);	//For TX interrupt
#ifndef uIP_NOOS
    dm9000_sem_lock = OSSemCreate(1);

#ifdef DMA_INT
    dm9051_txdma_flag = OSSemCreate(0);
    dm9051_rxdma_flag = OSSemCreate(0);
#endif	//DMA_INT

#endif	//uIP_NOOS

    /* RESET device                                                                                           */
    DM9051_Write_Reg(DM9000_NCR, DM9000_REG_RESET);
    _dm9000_Delay(1);	//1us
    DM9051_Write_Reg(DM9000_NCR, 0);

    /* RESET device                                                                                           */
    DM9051_Write_Reg(DM9000_NCR, DM9000_REG_RESET);
    _dm9000_Delay(1);
    DM9051_Write_Reg(DM9000_NCR, 0);

    DM9051_Write_Reg(DM9000_GPCR, GPCR_GEP_CNTL);
    DM9051_Write_Reg(DM9000_GPR, 0x00);		//Power on PHY

//  #if (ENABLE_LCD_DISPLAY == 1)
//  _dm9000_Delay_ms(50000/3);
//  LCD_ShowString(Line3, LEFT_WIDTH + 200 + 0 * 5, LEFT_ALIGNMENT, Color_White, FONT_12, ".");
//  _dm9000_Delay_ms(50000/3);
//  LCD_ShowString(Line3, LEFT_WIDTH + 200 + 1 * 5, LEFT_ALIGNMENT, Color_White, FONT_12, ".");
//  _dm9000_Delay_ms(50000/3);
//  LCD_ShowString(Line3, LEFT_WIDTH + 200 + 2 * 5, LEFT_ALIGNMENT, Color_White, FONT_12, ".");
//  #else
    //_dm9000_Delay_ms(50000);
    _dm9000_Delay_ms(100);
//  #endif

    /* Set PHY                                                                                                */
    phy_mode_set(dm9000_device.mode);
    //_dm9000_Delay_ms(50000);
    //printf("PHY REG_0 = 0x%4x\n", phy_read(0));
    //printf("PHY REG_4 = 0x%4x\n", phy_read(4));
    /* set mac address                                                                                        */
    for (i = 0, oft = DM9000_PAR; i < 6; i++, oft++)
    {
        DM9051_Write_Reg(oft, dm9000_device.dev_addr[i]);
    }

    /* set multicast address      */
    for(i = 0; i < 8; i++) 								 /* 清除 網卡多播設置 */
        DM9051_Write_Reg(DM9000_MAR + i, (7 == i) ? 0x80 : 0x00); /* 速設置 廣播包 設置 */

    /*
      for (i = 0, oft = DM9000_MAR; i < 8; i++, oft++)
      {
        DM9051_Write_Reg(oft, 0xff);
      }
    */

    DM9000_TRACE2("DM9051 MAC: ");
    for (i = 0, oft = DM9000_PAR; i < 6; i++, oft++)
    {
        DM9000_TRACE2("%02X:", DM9051_Read_Reg(oft));
    }
    DM9000_TRACE2("\n");

    /* Activate DM9051                                                                                        */
    /* Setup DM9051 Registers */
    DM9051_Write_Reg(DM9000_NCR, NCR_DEFAULT);
    DM9051_Write_Reg(DM9000_TCR, TCR_DEFAULT);
    //DM9051_Write_Reg(DM9000_TCR, 0x20);						//Disable underrun retry.
    DM9051_Write_Reg(DM9000_RCR, RCR_DEFAULT);
    DM9051_Write_Reg(DM9000_BPTR, BPTR_DEFAULT);
    //DM9051_Write_Reg(DM9000_FCTR, FCTR_DEAFULT);
    DM9051_Write_Reg(DM9000_FCTR, 0x3A);
    DM9051_Write_Reg(DM9000_FCR,  FCR_DEFAULT);
    //DM9051_Write_Reg(DM9000_FCR,  0x0);					//Disable Flow_Control
    DM9051_Write_Reg(DM9000_SMCR, SMCR_DEFAULT);
    DM9051_Write_Reg(DM9000_TCR2, DM9000_TCR2_SET);
    //DM9051_Write_Reg(DM9000_TCR2, 0x80);
    DM9051_Write_Reg(DM9051_INTR, 0x1);

    /* Clear status */
    DM9051_Write_Reg(DM9000_NSR, NSR_CLR_STATUS);
    DM9051_Write_Reg(DM9051_ISR, ISR_CLR_STATUS);


//  if (dm9000_device.mode == DM9000_AUTO)
    if ((dm9000_device.mode == DM9000_AUTO) || (dm9000_device.mode == DM9000_10M))
    {
        //printf("DM9051_REG_1fh = 0x%X.\n", DM9051_Read_Reg(0x1f));

        while (!(phy_read(1) & 0x20))
        {
            /* autonegation complete bit                                                                          */
            _dm9000_Delay(2);
            //printf("PHY_REG00h = 0x%04X, PHY_REG01h = 0x%04X.\n", phy_read(0), phy_read(1));

            i++;
            if (i >= 10)
            {
                DM9000_TRACE2("could not establish link \n");
                //LCD_ShowString(Line5, LEFT_WIDTH, LEFT_ALIGNMENT, Color_White, FONT_12, "Operation Mode: Connecting");
                //return 0;
                break;
            }
        }
    }


    /* see what we've got                                                                                     */
    lnk = phy_read(17) >> 12;
    DM9000_TRACE2("operating at ");
    switch (lnk)
    {
    case 1:
        DM9000_TRACE2("10M half duplex ");
        //LCD_ShowString(Line5, LEFT_WIDTH, LEFT_ALIGNMENT, Color_White, FONT_12, "Operation Mode: 10M half duplex");
        break;
    case 2:
        DM9000_TRACE2("10M full duplex ");
        //LCD_ShowString(Line5, LEFT_WIDTH, LEFT_ALIGNMENT, Color_White, FONT_12, "Operation Mode: 10M full duplex");
        break;
    case 4:
        DM9000_TRACE2("100M half duplex ");
        //LCD_ShowString(Line5, LEFT_WIDTH, LEFT_ALIGNMENT, Color_White, FONT_12, "Operation Mode: 100M half duplex");
        break;
    case 8:
        DM9000_TRACE2("100M full duplex ");
        //LCD_ShowString(Line5, LEFT_WIDTH, LEFT_ALIGNMENT, Color_White, FONT_12, "Operation Mode: 100M full duplex");
        break;
    default:
        DM9000_TRACE2("unknown: %d ", lnk);
        break;
    }
    DM9000_TRACE2("mode \n");

    DM9051_Write_Reg(DM9051_IMR, dm9000_device.imr_all);    /* Enable TX/RX interrupt mask                    */
    //DM9051_Write_Reg(DM9051_IMR, 0x82);
    DM9051_Write_Reg(DM9000_RCR, (RCR_DEFAULT | RCR_RXEN));  /* Enable RX */
    //DM9051_Write_Reg(DM9000_RCR, 0x03);		//promiscuous mode
    return 0;
}

/*********************************************************************************************************//**
  * @brief  Close DM9000.
  * @retval Always 0
  ***********************************************************************************************************/
u32 dm9000_close(void)
{
    /* RESET devie                                                                                            */
    phy_write(0, 0x8000);                  /* PHY RESET                                                       */
    DM9051_Write_Reg(DM9000_GPR, 0x01);    /* Power-Down PHY                                                  */
    DM9051_Write_Reg(DM9051_IMR, 0x80);    /* Disable all interrupt                                           */
    DM9051_Write_Reg(DM9000_RCR, 0x00);    /* Disable RX                                                      */

    return 0;
}

#ifndef uIP_NOOS
/*********************************************************************************************************//**
 * @brief   This function handles EXTI interrupt.
 * @retval  None
 ************************************************************************************************************/
void dm9051_IRQHandler(void)
{
    unsigned  int cpu_sr;
    OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR                 */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

#if 0
    if (OSIntNesting > 0)
    {
        OSIntNesting--;
    }
#endif	//0

    EXTI_ClearITPendingBit(EXTI_Line7);

    while( GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)==0 )
    {
        dm9051_isr();
    }

    //EXTI_ClearITPendingBit(EXTI_Line7);

    OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR                 */
}


/*********************************************************************************************************//**
  * @brief  Interrupt service routine.
  * @retval None
  ***********************************************************************************************************/
void dm9051_isr(void)
{
    uint16_t int_status;
    u8 temp;

    DM9051_Write_Reg(DM9051_IMR, DM9000_IMR_OFF);		//Clear interrupt flag

    /* Got DM9051 interrupt status  */
    int_status = DM9051_Read_Reg(DM9051_ISR);

    DM9051_Write_Reg(DM9051_ISR, (u8)int_status);    /* Clear ISR status                                          */

    //DM9000_TRACE("dm9000 isr: int status %04x \n", int_status);

    /* receive overflow                                                                                       */
    if (int_status & ISR_ROS)
    {
        DM9000_TRACE2("overflow \n");
    }

    if (int_status & ISR_ROOS)
    {
        DM9000_TRACE2("overflow counter overflow \n");
    }

    /* Received the coming packet                                                                             */
    if (int_status & ISR_PRS)
    {
        extern OS_EVENT* ethernetinput;

        /* disable receive interrupt                                                                            */
        dm9000_device.imr_all &= ~IMR_PRM;

        //dm9000_device.imr_all = IMR_OFF;		//Disable all interrupt.

        /* a frame has been received                                                                            */
        OSSemPost(ethernetinput);

    }

#if 0
    /* Transmit Interrupt check                                                                               */
    if (int_status & ISR_PTS)
    {
        /* transmit done                                                                                        */
        int tx_status = DM9051_Read_Reg(DM9000_NSR);    /* Got TX status                                        */

        if (tx_status & (NSR_TX2END | NSR_TX1END))
        {
#if 0
            dm9000_device.packet_cnt --;
            if (dm9000_device.packet_cnt > 0)
            {
                DM9000_TRACE("dm9000 isr: tx second packet \n");

                /* transmit packet II                                                                               */
                /* Set TX length to DM9000                                                                          */
                DM9051_Write_Reg(DM9000_TXPLL, dm9000_device.queue_packet_len & 0xff);
                DM9051_Write_Reg(DM9000_TXPLH, (dm9000_device.queue_packet_len >> 8) & 0xff);

                /* Issue TX polling command                                                                         */
                DM9051_Write_Reg(DM9000_TCR, TCR_TXREQ);    /* Cleared after TX complete                            */
            }
#endif	//0
            /* unlock DM9000 device                                                                               */
            //OSSemPost(dm9000_sem_ack);	//For TX interrupt

        }
    }
#endif	//0

    //DM9000_TRACE("dm9000 isr: imr_all %02x \n", dm9000_device.imr_all);
    //DM9051_Write_Reg(DM9051_IMR, dm9000_device.imr_all);

}
#endif	//uIP_NOOS

/*********************************************************************************************************//**
  * @brief  Tx function.
  * @param  pbuf: buffer link list
  * @retval Always 0
  ***********************************************************************************************************/
#ifdef uIP_NOOS
u32 dm9051_tx(void)
#else
u32 dm9051_tx(struct netif *netif, struct pbuf* p)
#endif	//uIP_NOOS
{

    uint16_t calc_MWR;
    uint16_t SendLength;

    //printf("dm9051_tx()....\n");

#ifndef uIP_NOOS
    uint8_t err;

    netif->txb_count += p->tot_len;		//Count TX bytes.
    SendLength = p->tot_len;
    /* lock DM9051                                                                                            */
    OSSemPend(dm9000_sem_lock, 0,&err);
#else
    SendLength = uip_len;
#endif	//uIP_NOOS

    /* disable dm9000a interrupt                                                                              */
    DM9051_Write_Reg(DM9051_IMR, IMR_PAR);

    while(DM9051_Read_Reg(DM9000_TCR) & DM9000_TCR_SET)
    {
        _dm9000_Delay(5);
    }

#ifdef FifoPointCheck
    /* 計算下一個傳送的指針位 , 若接收長度為奇數，需加一對齊偶字節。*/
    /* 若是超過 0x0bff ，則需回歸繞到 0x0000 起始位置 */
    calc_MWR = (DM9051_Read_Reg(DM9051_MWRH) << 8) + DM9051_Read_Reg(DM9051_MWRL);
    calc_MWR += SendLength;
    //printf("calc_MWR = 0x%X\n", calc_MWR);
//	if(SendLength & 0x01) calc_MWR++;
    if(calc_MWR > 0x0bff) calc_MWR -= 0x0c00;
#endif


    /* Move data to DM9000 TX RAM                                                                             */
    //_dm9000_WriteRegAddr(DM9000_MWCMD);

    {
#ifdef uIP_NOOS
        //Write data to FIFO
        DM9051_Write_Reg(DM9051_TXPLL, uip_len & 0xff);
        DM9051_Write_Reg(DM9051_TXPLH, (uip_len >> 8) & 0xff);
        DM9051_Write_Mem((u8_t*)uip_buf, SendLength);
#else
        /* q traverses through linked list of pbuf's. This list MUST consist of a single packet ONLY            */
        struct pbuf *q;
        //uint16_t pbuf_index = 0;
        //uint8_t word[2], word_index = 0;

        q = p;

#if 0
        printf("Tx Read Pointer H = 0x%02X.\n", DM9051_Read_Reg(DM9000_TRPAH));
        printf("Tx Read Pointer L = 0x%02X.\n", DM9051_Read_Reg(DM9000_TRPAL));
        printf("DM9051_MWRH H = 0x%02X.\n", DM9051_Read_Reg(DM9051_MWRH));
        printf("DM9051_MWRL L = 0x%02X.\n", DM9051_Read_Reg(DM9051_MWRL));
#endif  //0

        //Write data to FIFO
        DM9051_Write_Reg(DM9051_TXPLL, q->tot_len & 0xff);
        DM9051_Write_Reg(DM9051_TXPLH, (q->tot_len >> 8) & 0xff);
        DM9051_Write_Mem((u8_t*)q->payload, q->tot_len);

#endif	//uIP_NOOS

        /* Issue TX polling command                                                                             */
        DM9051_Write_Reg(DM9000_TCR, TCR_TXREQ);    /* Cleared after TX complete */

    }

#if 0
    if (dm9000_device.packet_cnt == 0)
    {
        DM9000_TRACE("dm9000 tx: first packet \n");

        dm9000_device.packet_cnt ++;
        /* Set TX length to DM9000                                                                              */
        DM9051_Write_Reg(DM9000_TXPLL, p->tot_len & 0xff);
        DM9051_Write_Reg(DM9000_TXPLH, (p->tot_len >> 8) & 0xff);

        /* Issue TX polling command                                                                             */
        DM9051_Write_Reg(DM9000_TCR, TCR_TXREQ);    /* Cleared after TX complete                                */
    }
    else
    {
        DM9000_TRACE("dm9000 tx: second packet \n");
        dm9000_device.packet_cnt ++;
        dm9000_device.queue_packet_len = p->tot_len;
    }
#endif	//0

#if 0
    printf("Tx control Reg = 0x%02X.\n", DM9051_Read_Reg(DM9000_TCR));
    printf("Tx Read Pointer H = 0x%02X.\n", DM9051_Read_Reg(DM9000_TRPAH));
    printf("Tx Read Pointer L = 0x%02X.\n", DM9051_Read_Reg(DM9000_TRPAL));
    printf("DM9051_MWRH H = 0x%02X.\n", DM9051_Read_Reg(DM9051_MWRH));
    printf("DM9051_MWRL L = 0x%02X.\n", DM9051_Read_Reg(DM9051_MWRL));
    printf("Tx control Reg = 0x%02X.\n", DM9051_Read_Reg(DM9000_TCR));
    printf("ISR Reg = 0x%02X.\n", DM9051_Read_Reg(DM9051_ISR));
    printf("NSR Reg = 0x%02X.\n", DM9051_Read_Reg(DM9000_NSR));
#endif  //0


#ifdef FifoPointCheck
    if(calc_MWR != ((DM9051_Read_Reg(DM9051_MWRH) << 8) + DM9051_Read_Reg(DM9051_MWRL)))
    {
#ifndef uIP_NOOS
        netif->tx_reset++;
#endif	//uIP_NOOS

#if 0 //Point_Error_Reset
        /* 若是指針出錯，等待此一封包送完 , 之後進行重置 */
        while(ior(DM9000_REG_TCR) & DM9000_TCR_SET) udelay (5);
        dm9k_reset();
        /* unlock DM9051 device                                                                                   */
        OSSemPost(dm9000_sem_lock);
        return;
#endif
        printf("DM9K MWR Error!! calc_MWR = 0x%X, SendLength = 0x%x\n", calc_MWR, SendLength);
        printf("MWRH = 0x%X, MWRL = 0x%X\n", DM9051_Read_Reg(DM9051_MWRH), DM9051_Read_Reg(DM9051_MWRL));
#if 0
        DM9051_Write_Reg(DM9051_ISR, 0x80);    /* DIsable INT                                               */
        dm9051_init(netif);
        DM9051_Write_Reg(DM9051_ISR, 0x81);    /* Enable INT */
#endif	//0

        /*若是指針出錯，將指針移到下一個傳送包的包頭位置  */
        DM9051_Write_Reg(DM9051_MWRH, (calc_MWR >> 8) & 0xff);
        DM9051_Write_Reg(DM9051_MWRL, calc_MWR & 0xff);
    }
#endif

    /* enable dm9000a interrupt    */
    DM9051_Write_Reg(DM9051_IMR, dm9000_device.imr_all);

#ifndef uIP_NOOS
    /* unlock DM9051 device                                                                                   */
    OSSemPost(dm9000_sem_lock);

    /* wait ack                                                                                               */
    //OSSemPend(dm9000_sem_ack, 0,&err);	//For TX interrupt
#endif	//uIP_NOOS

    //DM9000_TRACE("dm9000 tx done \n");

    return 0;
}

/*********************************************************************************************************//**
  * @brief  Rx function.
  * @retval pbuf
  ***********************************************************************************************************/
#ifdef uIP_NOOS
uint16_t dm9051_rx(void)
#else
struct pbuf *dm9051_rx(struct netif *netif)
#endif	//uIP_NOOS
{
    uint8_t rxbyte;

    uint16_t calc_MRR;
    uint16_t rx_len = 0;

#ifndef uIP_NOOS
    uint8_t err;
    uint8_t* databyte = 0;
    struct pbuf* p;
    //printf("dm9051_rx()..->\n");

    /* init p pointer                                                                                         */
    p = NULL;

    /* lock DM9000                                                                                            */
    OSSemPend(dm9000_sem_lock, 0,&err);
#endif	//uIP_NOOS


    /* Check packet ready or not                                                                              */
    rxbyte = DM9051_Read_Reg(DM9051_MRCMDX);
    rxbyte = DM9051_Read_Reg(DM9051_MRCMDX);

    //printf("dm9051_rx process...1st byte = 0x%x\n",rxbyte);

    if ((rxbyte != 1) && (rxbyte != 0))
    {
        printf("rxbyte = %02X.\n", rxbyte);
        DM9000_TRACE2("dm9000 rx: rx error, stop device \n");

        /* Reset RX FIFO pointer */
        DM9051_Write_Reg(DM9000_RCR, RCR_DEFAULT);	//RX disable
        DM9051_Write_Reg(DM9051_MPCR, 0x01);		//Reset RX FIFO pointer
        _dm9000_Delay(2);
        DM9051_Write_Reg(DM9000_RCR, (RCR_DEFAULT | RCR_RXEN));		//RX Enable

        /* restore receive interrupt                                                                            */
        dm9000_device.imr_all |= IMR_PRM;
        //dm9000_device.imr_all = IMR_DEFAULT;
        DM9051_Write_Reg(DM9051_IMR, dm9000_device.imr_all);

#ifndef uIP_NOOS
        netif->rx_reset++;
        /* unlock DM9051                                                                                          */
        OSSemPost(dm9000_sem_lock);
#endif	//uIP_NOOS

        return 0;
    }

    if (rxbyte)
    {
        uint16_t rx_status;
        //uint16_t* data = 0;

        uint8_t ReceiveData[4];

#ifdef FifoPointCheck
        calc_MRR = (DM9051_Read_Reg(DM9051_MRRH) << 8) + DM9051_Read_Reg(DM9051_MRRL);	//Save RX SRAM pointer
#endif	//FifoPointCheck

        DM9051_Read_Reg(DM9051_MRCMDX);		//dummy read

        DM9051_Read_Mem(ReceiveData, 4);

        rx_status = ReceiveData[0] + (ReceiveData[1] << 8);
        rx_len = ReceiveData[2] + (ReceiveData[3] << 8);

        //DM9000_TRACE("dm9051 rx: status %04x len %d \n", rx_status, rx_len);
#ifdef uIP_NOOS
        DM9051_Read_Mem((u8_t *)uip_buf, rx_len);
#else
        netif->rxb_count += rx_len;		//Count received bytes.

        /* allocate buffer           */
        p = pbuf_alloc(PBUF_LINK, rx_len, PBUF_RAM);
        if(p != NULL)
        {
            DM9051_Read_Mem((u8_t *)p->payload, rx_len);
        }
        else
        {
            DM9000_TRACE2("dm9000 rx: no pbuf \n");
            /* no pbuf, discard data from DM9000  */
            DM9051_Read_Mem(databyte, rx_len);
        }
#endif	//uIP_NOOS

#ifdef FifoPointCheck
        /* 若是超過 0x3fff ，則需回歸繞到 0x0c00 起始位置 */
        calc_MRR += (rx_len + 4);
        if(calc_MRR > 0x3fff) calc_MRR -= 0x3400;

        if(calc_MRR != ((DM9051_Read_Reg(DM9051_MRRH) << 8) + DM9051_Read_Reg(DM9051_MRRL)))
        {

            printf("DM9K MRR Error!!\n");
            printf("Predicut RX Read pointer = 0x%X, Current pointer = 0x%X\n", calc_MRR, ((DM9051_Read_Reg(DM9051_MRRH) << 8) + DM9051_Read_Reg(DM9051_MRRL)));

            /*若是指針出錯，將指針移到下一個包的包頭位置  */
            DM9051_Write_Reg(DM9051_MRRH, (calc_MRR >> 8) & 0xff);
            DM9051_Write_Reg(DM9051_MRRL, calc_MRR & 0xff);
        }
#endif

        if ((rx_status & 0xbf00) || (rx_len < 0x40) || (rx_len > DM9000_PKT_MAX) )
        {
            DM9000_TRACE2("rx error: status %04x, rx_len: %d \n", rx_status, rx_len);

            if (rx_status & 0x100)
            {
                DM9000_TRACE2("rx fifo error \n");
            }
            if (rx_status & 0x200)
            {
                DM9000_TRACE2("rx crc error \n");
            }
            if (rx_status & 0x8000)
            {
                DM9000_TRACE2("rx length error \n");
            }
            if (rx_len > DM9000_PKT_MAX)
            {
                DM9000_TRACE2("rx length too big \n");
                /* RESET device                                                                                     */
                //_dm9000_WriteReg(DM9000_NCR, NCR_RST);
                //_dm9000_Delay_ms(5);
            }
#ifndef uIP_NOOS
            pbuf_free(p);
            p = NULL;
#endif	//uIP_NOOS

        }
    }
    else
    {
        //DM9000_TRACE("dm9000 rx: No packets received. \n");

        /* clear packet received latch status                                                                   */

        /* restore receive interrupt                                                                            */
        dm9000_device.imr_all |= IMR_PRM;
        //dm9000_device.imr_all = IMR_DEFAULT;
        DM9051_Write_Reg(DM9051_IMR, dm9000_device.imr_all);

    }

#ifdef uIP_NOOS
    return rx_len;

#else
    /* unlock DM9051                                                                                          */
    OSSemPost(dm9000_sem_lock);
    return p;
#endif	//uIP_NOOS

}

/* Private functions ---------------------------------------------------------------------------------------*/

/*********************************************************************************************************//**
  * @brief  Hardware related configuration.
  * @retval None
  ***********************************************************************************************************/
void dm9051_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*DM9051 CS*/
    RCC_AHB1PeriphClockCmd(RCC_DM9051_CS, ENABLE);//妏夔GPIOB奀笘
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_DM9051_CS;//pin
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//怀堤
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//芢侺怀堤
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//奻嶺
    GPIO_Init(GPIO_DM9051_CS, &GPIO_InitStructure);//場宎趙
    GPIO_SetBits(GPIO_DM9051_CS,GPIO_Pin_DM9051_CS);
    
    /*FLASH CS*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//妏夔GPIOB奀笘
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//pin
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//怀堤
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//芢侺怀堤
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//奻嶺
    GPIO_Init(GPIOA, &GPIO_InitStructure);//場宎趙
    GPIO_SetBits(GPIOA,GPIO_Pin_15);
    
    /*SPI init*/
    SPI3_Init();
    SPI3_SetSpeed(SPI_BaudRatePrescaler_8);
    
#ifdef SPI_DMA
    RCC_AHBPeriphClockCmd(SPI_MASTER_DMA_CLK, ENABLE);
#endif	//SPI_DMA

#ifdef DMA_INT
    /* NVIC configuration */
    NVIC_Configuration();
#endif	//DMA_INT

#ifdef SPI_DMA
    /* SPI_MASTER_Rx_DMA_Channel configuration ---------------------------------*/
    DMA_DeInit(SPI_MASTER_Rx_DMA_Channel);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI_MASTER_DR_Base;
    //DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SPI_MASTER_Buffer_Rx;
#ifdef uIP_NOOS
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uip_buf;
#endif
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //DMA_InitStructure.DMA_BufferSize = BufferSize;		//RX length
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(SPI_MASTER_Rx_DMA_Channel, &DMA_InitStructure);

    /* SPI_MASTER_Tx_DMA_Channel configuration ---------------------------------*/
    DMA_DeInit(SPI_MASTER_Tx_DMA_Channel);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI_MASTER_DR_Base;
    //DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SPI_MASTER_Buffer_Tx;
#ifdef uIP_NOOS
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uip_buf;
#endif
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(SPI_MASTER_Tx_DMA_Channel, &DMA_InitStructure);

#ifdef DMA_INT
    /* Enable DMA1 Channel2,3 Transfer Complete interrupt */
    DMA_ITConfig(SPI_MASTER_Rx_DMA_Channel, DMA_IT_TC, ENABLE);
    DMA_ITConfig(SPI_MASTER_Tx_DMA_Channel, DMA_IT_TC, ENABLE);
#endif	//DMA_INT  
    //DMA_Cmd(SPI_MASTER_Rx_DMA_Channel, ENABLE);
    //DMA_Cmd(SPI_MASTER_Tx_DMA_Channel, ENABLE);
#endif	//SPI_DMA

#ifdef DM9051_INT
    /* configure PC7 as external interrupt */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Connect DM9051 EXTI Line to GPIOC Pin 7 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource7);

    /* Configure DM9051 EXTI Line to generate an interrupt on falling edge */
    EXTI_InitStructure.EXTI_Line = EXTI_Line7;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Clear DM9051 EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line7);

    /* Enable the EXTI7 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif	//DM9051_INT

}


/*********************************************************************************************************//**
  * @brief  uS level delay function.
  * @param  uDelay: Delay time
  * @retval None
  ***********************************************************************************************************/
static void _dm9000_Delay(u32 uDelay)
{
    delay_us(uDelay);
}

/*********************************************************************************************************//**
  * @brief  mS lebel delay function.
  * @param  uDelay: delay time based on ms.
  * @retval None
  ***********************************************************************************************************/
static void _dm9000_Delay_ms(u32 uDelay)
{
    delay_ms(uDelay);
}

/*********************************************************************************************************//**
  * @brief  Read function of PHY.
  * @param  uReg: PHY register
  * @retval uData: Data of register
  ***********************************************************************************************************/
static u16 phy_read(u32 uReg)
{
    u16 uData;

    /* Fill the phyxcer register into REG_0C                                                                  */
    DM9051_Write_Reg(DM9000_EPAR, DM9000_PHY | uReg);
    DM9051_Write_Reg(DM9000_EPCR, 0xc);           /* Issue phyxcer read command                               */

    _dm9000_Delay_ms(100);                        /* Wait read complete                                       */
    //while(DM9051_Read_Reg(DM9000_EPCR) & 0x1) {_dm9000_Delay(1);};		//Wait complete

    DM9051_Write_Reg(DM9000_EPCR, 0x0);           /* Clear phyxcer read command                               */
    uData = (DM9051_Read_Reg(DM9000_EPDRH) << 8) | DM9051_Read_Reg(DM9000_EPDRL);

    return uData;
}

/*******************************************************************************
* Function Name  : phy_write
* Description    : Write a word to phyxcer
* Input          : - reg: reg
*                  - value: data
* Output         : None
* Return         : None
* Attention         : None
*******************************************************************************/
static void phy_write(uint16_t reg, uint16_t value)
{
    /* Fill the phyxcer register into REG_0C                                                                */
    DM9051_Write_Reg(DM9000_EPAR, DM9000_PHY | reg);

    /* Fill the written data into REG_0D & REG_0E                                                           */
    DM9051_Write_Reg(DM9000_EPDRL, (value & 0xff));
    DM9051_Write_Reg(DM9000_EPDRH, ((value >> 8) & 0xff));
    DM9051_Write_Reg(DM9000_EPCR, 0xa);    /* Issue phyxcer write command                                   */

    //_dm9000_Delay_ms(500);                       /* Wait write complete                                   */
    while(DM9051_Read_Reg(DM9000_EPCR) & 0x1)
    {
        _dm9000_Delay(1);
    };		//Wait complete

    DM9051_Write_Reg(DM9000_EPCR, 0x0);    /* Clear phyxcer write command                                   */


}

/*********************************************************************************************************//**
  * @brief  Set PHY mode.
  * @param  uMediaMode:
  *         @DM9000_AUTO Auto negotiation
  *         @DM9000_10MHD 10MHz, Half duplex
  *         @DM9000_10MFD 10MHz, Full duplex
  *         @DM9000_100MHD 100MHz, Half duplex
  *         @DM9000_100MFD 100MHz, Full duplex
  * @retval None
  ***********************************************************************************************************/
static void phy_mode_set(u32 uMediaMode)
{
    uint16_t phy_reg4 = 0x01e1, phy_reg0 = 0x1000;
    if (!(uMediaMode & DM9000_AUTO) )
    {
        switch (uMediaMode)
        {
        case DM9000_10MHD:
        {
            phy_reg4 = 0x21;
            phy_reg0 = 0x0000;
            break;
        }
        case DM9000_10MFD:
        {
            phy_reg4 = 0x41;
            phy_reg0 = 0x1100;
            break;
        }
        case DM9000_100MHD:
        {
            phy_reg4 = 0x81;
            phy_reg0 = 0x2000;
            break;
        }
        case DM9000_100MFD:
        {
            phy_reg4 = 0x101;
            phy_reg0 = 0x3100;
            break;
        }
        case DM9000_10M:
        {
            phy_reg4 = 0x61;
            phy_reg0 = 0x1200;
            break;
        }
        }
        phy_write(4, phy_reg4);             /* Set PHY media mode                                               */

        phy_write(0, phy_reg0);             /* Tmp                                                              */
        _dm9000_Delay(10);
    }
#if 0
    DM9051_Write_Reg(DM9000_GPCR, 0x01);  /* Let GPIO0 output                                                 */
    DM9051_Write_Reg(DM9000_GPR,  0x00);  /* Enable PHY                                                       */
#endif	//0

}

#ifdef uIP_NOOS
/**************************************************/
/*uIP                                             */
/**************************************************/
uint8_t etherdev_init(void)
{
    return dm9051_init();
}

void etherdev_send(void)
{
    dm9051_tx();
}

uint16_t etherdev_read(void)
{
    return dm9051_rx();
}

#endif	//uIP_NOOS

#ifdef DMA_INT
/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable Rx DMA1 channel2 IRQ Channel */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable Tx DMA1 channel3 IRQ Channel */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

/**
  * @brief  This function handles DMA1 Channel 2 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel2_IRQHandler(void)
{
    /* Test on Rx DMA1 Channel2 Transfer Complete interrupt */
    if(DMA_GetITStatus(DMA1_IT_TC2))
    {
        /* Get Current Data Counter value after complete transfer */
        //CurrDataCounterEnd = DMA_GetCurrDataCounter(DMA1_Channel2);
        /* Clear DMA1 Channel2 Half Transfer, Transfer Complete and Global interrupt pending bits */
        DMA_ClearITPendingBit(DMA1_IT_GL2);

        SPI_DM9051_CS_HIGH();

        OSSemPost(dm9051_rxdma_flag);
        //RxComplete = 1;
    }
}

/**
  * @brief  This function handles Tx DMA1 Channel 3 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel3_IRQHandler(void)
{
    /* Test on DMA1 Channel3 Transfer Complete interrupt */
    if(DMA_GetITStatus(DMA1_IT_TC3))
    {
        /* Get Current Data Counter value after complete transfer */
        //CurrDataCounterEnd = DMA_GetCurrDataCounter(DMA1_Channel3);
        /* Clear DMA1 Channel6 Half Transfer, Transfer Complete and Global interrupt pending bits */
        DMA_ClearITPendingBit(DMA1_IT_GL3);

        SPI_DM9051_CS_HIGH();

        OSSemPost(dm9051_txdma_flag);
        //TxComplete = 1;
    }
}

#endif	//DMA_INT
