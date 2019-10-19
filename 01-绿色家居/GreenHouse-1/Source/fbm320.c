#include "fbm320.h"
#include "iic.h"
#include "delay.h"
#include "hal_types.h"

long UP_S=0, UT_S=0, RP_S=0, RT_S=0, OffP_S=0;
long UP_I=0, UT_I=0, RP_I=0, RT_I=0, OffP_I=0;
float H_S=0, H_I=0;
float Rpress;
unsigned int C0_S, C1_S, C2_S, C3_S, C6_S, C8_S, C9_S, C10_S, C11_S, C12_S; 
unsigned long C4_S, C5_S, C7_S;
unsigned int C0_I, C1_I, C2_I, C3_I, C6_I, C8_I, C9_I, C10_I, C11_I, C12_I; 
unsigned long C4_I, C5_I, C7_I;
unsigned char Formula_Select=1;

/*********************************************************************************************
* 名称：fbm320_read_id()
* 功能：FBM320_ID读取
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
unsigned char fbm320_read_id(void)
{
  iic_start();                                                  //启动总线
  if(iic_write_byte(FBM320_ADDR) == 0){                         //检测总线地址
    if(iic_write_byte(FBM320_ID_ADDR) == 0){                    //监测信道状态
      //do{
      //  delay(30);                                              //延时30指令
        iic_start();	                                        //启动总线
      //}
      //while(iic_write_byte(FBM320_ADDR | 0x01) == 1);           //等待总线通信完成
      iic_write_byte(FBM320_ADDR | 0x01);
        unsigned char id = iic_read_byte(1);                      
      if(FBM320_ID == id){
        iic_stop();                                             //停止总线传输
        return 1;
      }
      
    }
  }
  iic_stop();                                                   //停止总线传输
  return 0;                                                     //地址错误返回0
}


/*********************************************************************************************
* 名称：fbm320_read_reg()
* 功能：数据读取
* 参数：
* 返回：data1 数据/0 错误返回
* 修改：
* 注释：
*********************************************************************************************/
unsigned char fbm320_read_reg(unsigned char reg)
{
  iic_start();                                                  //启动IIC总线传输
  if(iic_write_byte(FBM320_ADDR) == 0){                         //检测总线地址
    if(iic_write_byte(reg) == 0){                               //监测信道状态
      //do{
      //  delay(30);                                              //延时30
        iic_start();                                            //启动IIC总线传输	
      //}
      //while(iic_write_byte(FBM320_ADDR | 0x01) == 1);           //等待IIC总线启动成功
      iic_write_byte(FBM320_ADDR | 0x01);
      unsigned char data1 = iic_read_byte(1);                   //读取数据
      iic_stop();                                               //停止IIC总线
      return data1;                                             //返回数据
    }
  }
  iic_stop();                                                   //停止IIC总线
  return 0;                                                     //返回错误0
}

/*********************************************************************************************
* 名称：fbm320_write_reg()
* 功能：发送识别信息
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void fbm320_write_reg(unsigned char reg,unsigned char data)
{
  iic_start();                                                  //启动IIC总线
  if(iic_write_byte(FBM320_ADDR) == 0){                         //检测总线地址
    if(iic_write_byte(reg) == 0){                               //监测信道状态
      iic_write_byte(data);                                     //发送数据
    }
  }
  iic_stop();                                                   //停止IIC总线
}

/*********************************************************************************************
* 名称：fbm320_read_data()
* 功能：数据读取
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
long fbm320_read_data(void)
{
  unsigned char data[3];
  //char ret;
  iic_start();                                                  //启动总线
  if (1 == iic_write_byte(FBM320_ADDR)){                                  //总线地址设置
    return -1;
  }
  if (1 == iic_write_byte(FBM320_DATAM)){                                 //读取数据指令
    return -1;
  }//delay(30);
  iic_start();                                                  //启动总线
  iic_write_byte(FBM320_ADDR | 0x01);                           //读取数据
  data[2] = iic_read_byte(0);
  data[1] = iic_read_byte(0);
  data[0] = iic_read_byte(1);
  iic_stop();                                                   //停止总线传输
  /*
  data[2] = fbm320_read_reg(FBM320_DATAM);
  data[1] = fbm320_read_reg(FBM320_DATAC);
  
  data[0] = fbm320_read_reg(FBM320_DATAL);
  */
  return (((long)data[2] << 16) | ((long)data[1] << 8) | data[0]);
}


/*********************************************************************************************
* 名称：Coefficient()
* 功能：大气压力系数换算
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/

void Coefficient(void)                                                                                                        //Receive Calibrate Coefficient
{
  unsigned char i;
  unsigned int R[10];
  unsigned int C0=0, C1=0, C2=0, C3=0, C6=0, C8=0, C9=0, C10=0, C11=0, C12=0; 
  unsigned long C4=0, C5=0, C7=0;
  
  for(i=0; i<9; i++)
    R[i]=(unsigned int)((unsigned int)fbm320_read_reg(0xAA + (i*2))<<8) | fbm320_read_reg(0xAB + (i*2));
  R[9]=(unsigned int)((unsigned int)fbm320_read_reg(0xA4)<<8) | fbm320_read_reg(0xF1);
  
  if(((Formula_Select & 0xF0) == 0x10) || ((Formula_Select & 0x0F) == 0x01))
  {
    C0 = R[0] >> 4;
    C1 = ((R[1] & 0xFF00) >> 5) | (R[2] & 7);
    C2 = ((R[1] & 0xFF) << 1) | (R[4] & 1);
    C3 = R[2] >> 3;
    C4 = ((unsigned long)R[3] << 2) | (R[0] & 3);
    C5 = R[4] >> 1;
    C6 = R[5] >> 3;
    C7 = ((unsigned long)R[6] << 3) | (R[5] & 7);
    C8 = R[7] >> 3;
    C9 = R[8] >> 2;
    C10 = ((R[9] & 0xFF00) >> 6) | (R[8] & 3);
    C11 = R[9] & 0xFF;
    C12 = ((R[0] & 0x0C) << 1) | (R[7] & 7);
  }
  else
  {
    C0 = R[0] >> 4;
    C1 = ((R[1] & 0xFF00) >> 5) | (R[2] & 7);
    C2 = ((R[1] & 0xFF) << 1) | (R[4] & 1);
    C3 = R[2] >> 3;
    C4 = ((unsigned long)R[3] << 1) | (R[5] & 1);
    C5 = R[4] >> 1;
    C6 = R[5] >> 3;
    C7 = ((unsigned long)R[6] << 2) | ((R[0] >> 2) & 3);
    C8 = R[7] >> 3;
    C9 = R[8] >> 2;
    C10 = ((R[9] & 0xFF00) >> 6) | (R[8] & 3);
    C11 = R[9] & 0xFF;
    C12 = ((R[5] & 6) << 2) | (R[7] & 7);
  }
  C0_I = C0;
  C1_I = C1;
  C2_I = C2;
  C3_I = C3;
  C4_I = C4;
  C5_I = C5;
  C6_I = C6;
  C7_I = C7;
  C8_I = C8;
  C9_I = C9;
  C10_I = C10;
  C11_I = C11;
  C12_I = C12;        
}
/*********************************************************************************************
* 名称：Calculate()
* 功能：大气压力换算
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/

void Calculate(long UP, long UT)                //Calculate Real Pressure & Temperautre
{
  int8 C12=0;
  int16 C0=0, C2=0, C3=0, C6=0, C8=0, C9=0, C10=0, C11=0; 
  //long C0=0, C2=0, C3=0, C6=0, C8=0, C9=0, C10=0, C11=0; 
  int32 C1=0, C4=0, C5=0, C7=0;
  int32 RP=0, RT=0;
  int32 DT, DT2, X01, X02, X03, X11, X12, X13, X21, X22, X23, X24, X25, X26, X31, X32, CF, PP1, PP2, PP3, PP4;
  C0 = C0_I;
  C1 = C1_I;
  C2 = C2_I;
  C3 = C3_I;
  C4 = C4_I;
  C5 = C5_I;
  C6 = C6_I;
  C7 = C7_I;
  C8 = C8_I;
  C9 = C9_I;
  C10 = C10_I;
  C11 = C11_I;
  C12 = C12_I;
  if(((Formula_Select & 0xF0) == 0x10) || ((Formula_Select & 0x0F) == 0x01))                        //For FBM320-02
  {
    DT        =        ((UT - 8388608) >> 4) + (C0 << 4);
    X01        =        ((C1 + 4459) * DT) >> 1;
    X02        =        ((((C2 - 256) * DT) >> 14) * DT) >> 4;
    X03        =        (((((C3 * DT) >> 18) * DT) >> 18) * DT);
    RT        =        (((long)2500 << 15) - X01 - X02 - X03) >> 15;
    
    DT2        =        (X01 + X02 + X03) >> 12;
    
    X11        =        ((C5 - 4443) * DT2);
    X12        =        (((C6 * DT2) >> 16) * DT2) >> 2;
    X13        =        ((X11 + X12) >> 10) + ((C4 + 120586) << 4);
    
    X21        =        ((C8 + 7180) * DT2) >> 10;
    X22        =        (((C9 * DT2) >> 17) * DT2) >> 12;
    if(X22 >= X21)
      X23        =        X22 - X21;
    else
      X23        =        X21 - X22;
    X24        =        (X23 >> 11) * (C7 + 166426);
    X25        =        ((X23 & 0x7FF) * (C7 + 166426)) >> 11;
    if((X22 - X21) < 0)
      X26        =        ((0 - X24 - X25) >> 11) + C7 + 166426;
    else        
      X26        =        ((X24 + X25) >> 11) + C7 + 166426;
    
    PP1        =        ((UP - 8388608) - X13) >> 3;
    PP2        =        (X26 >> 11) * PP1;
    PP3        =        ((X26 & 0x7FF) * PP1) >> 11;
    PP4        =        (PP2 + PP3) >> 10;
    
    CF        =        (2097152 + C12 * DT2) >> 3;
    X31        =        (((CF * C10) >> 17) * PP4) >> 2;
    X32        =        (((((CF * C11) >> 15) * PP4) >> 18) * PP4);
    RP        =        ((X31 + X32) >> 15) + PP4 + 99880;
  }
  else                                                                                                                                                                                                                                                                                                                //For FBM320
  {
    DT        =        ((UT - 8388608) >> 4) + (C0 << 4);
    X01        =        ((C1 + 4418) * DT) >> 1;
    X02        =        ((((C2 - 256) * DT) >> 14) * DT) >> 4;
    X03        =        (((((C3 * DT) >> 18) * DT) >> 18) * DT);
    RT = (((long)2500 << 15) - X01 - X02 - X03) >> 15;
    
    DT2        =        (X01 + X02 + X03) >>12;
    
    X11        =        (C5 * DT2);
    X12        =        (((C6 * DT2) >> 16) * DT2) >> 2;
    X13        =        ((X11 + X12) >> 10) + ((C4 + 211288) << 4);
    
    X21        =        ((C8 + 7209) * DT2) >> 10;
    X22        =        (((C9 * DT2) >> 17) * DT2) >> 12;
    if(X22 >= X21)
      X23        =        X22 - X21;
    else
      X23        =        X21 - X22;
    X24        =        (X23 >> 11) * (C7 + 285594);
    X25        =        ((X23 & 0x7FF) * (C7 + 285594)) >> 11;
    if((X22 - X21) < 0) 
      X26        =        ((0 - X24 - X25) >> 11) + C7 + 285594;
    else
      X26        =        ((X24 + X25) >> 11) + C7 + 285594;
    PP1        =        ((UP - 8388608) - X13) >> 3;
    PP2        =        (X26 >> 11) * PP1;
    PP3        =        ((X26 & 0x7FF) * PP1) >> 11;
    PP4        =        (PP2 + PP3) >> 10;
    
    CF        =        (2097152 + C12 * DT2) >> 3;
    X31        =        (((CF * C10) >> 17) * PP4) >> 2;
    X32        =        (((((CF * C11) >> 15) * PP4) >> 18) * PP4);
    RP = ((X31 + X32) >> 15) + PP4 + 99880;
  }
  
  RP_I = RP;
  RT_I = RT;
}
/*********************************************************************************************
* 名称：Calculate()
* 功能：大气压力传感器初始化
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/

unsigned char fbm320_init(void)
{
  iic_init();                                                   //IIC初始化
  if(fbm320_read_id() == 0)                                     //判读初始化是否成功
    return 0;
  Coefficient();
  return 1;
}
/*********************************************************************************************
* 名称：fbm320_data_get()
* 功能：传感器数据读取函数
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
int fbm320_data_get(float *temperature,long *pressure)
{
  //Coefficient();                                                //系数换算
  fbm320_write_reg(FBM320_CONFIG,TEMPERATURE);                  //发送识别信息
   delay_ms(5);                                                 //延时5ms
  UT_I = fbm320_read_data();                                    //读取传感器数据
  fbm320_write_reg(FBM320_CONFIG,OSR8192);                      //发送识别信息
  delay_ms(20);                                                 //延时10ms
  UP_I = fbm320_read_data();                                    //读取传感器数据
  if (UT_I == -1 || UP_I == -1){
    return -1;
  }
  Calculate( UP_I, UT_I);                                       //传感器数值换算
  *temperature = RT_I * 0.01f;                                  //温度计算
  *pressure = RP_I;                                             //压力计算
  return 0;
}