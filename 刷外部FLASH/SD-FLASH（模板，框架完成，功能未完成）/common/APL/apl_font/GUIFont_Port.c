/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   XBF格式字体emwin函数接口
  ******************************************************************************
  */

#include "GUIFont_Port.h"   
#include "w25qxx.h"
#include "ff.h"

/*字库存储路径*/
#if (XBF_FONT_SOURCE == USE_FLASH_FONT)

  #define FONT_FLAG_ADDR  (29*4096)
	static const uint32_t  FONT_XINSONGTI_12_ADDR	 = (30*4096);
	static const uint32_t  FONT_XINSONGTI_16_ADDR	 = (380*4096);
	static const uint32_t  FONT_XINSONGTI_24_ADDR	 = (780*4096);

#elif (XBF_FONT_SOURCE == USE_SDCARD_FONT)

	static const char  FONT_STORAGE_ROOT_DIR[] =  "0:";
	static const char FONT_XINSONGTI_12_ADDR[] =  "0:Font/ST12.xbf";
	static const char FONT_XINSONGTI_16_ADDR[] =  "0:Font/ST16.xbf";
	static const char FONT_XINSONGTI_24_ADDR[] =  "0:Font/ST24.xbf";

#elif (XBF_FONT_SOURCE == USE_FLASH_FILESYSTEM_FONT)
	
	static const char FONT_STORAGE_ROOT_DIR[] =   "1:";
	static const char FONT_XINSONGTI_12_ADDR[] =  "0:Font/ST12.xbf";
	static const char FONT_XINSONGTI_16_ADDR[] =  "0:Font/ST16.xbf";
	static const char FONT_XINSONGTI_24_ADDR[] =  "0:Font/ST24.xbf";

#endif


/*字库结构体*/
GUI_XBF_DATA 	XBF_XINSONGTI_12_Data;
GUI_FONT     	FONT_XINSONGTI_12;

GUI_XBF_DATA 	XBF_XINSONGTI_16_Data;
GUI_FONT     	FONT_XINSONGTI_16;

GUI_XBF_DATA 	XBF_XINSONGTI_24_Data;
GUI_FONT     	FONT_XINSONGTI_24;


/*字库存储在文件系统时需要使用的变量*/
#if (XBF_FONT_SOURCE == USE_SDCARD_FONT || XBF_FONT_SOURCE == USE_FLASH_FILESYSTEM_FONT)
	extern FATFS fs[2];										/* Work area (file system object) for logical drives */
	static FIL fFont_file;										/* file objects */
	static FRESULT fFont_result; 
	static UINT fFont_num;            				/* File R/W count */
#endif

 /**
  * @brief  获取字体数据的回调函数
  * @param  Offset：要读取的内容在XBF文件中的偏移位置
  * @param  NumBytes：要读取的字节数
	* @param  pVoid：自定义数据的指针
  * @param  pBuffer：存储读取内容的指针
  * @retval 0 成功, 1 失败
  */
static int _cb_FONT_XBF_GetData(U32 Offset, U16 NumBytes, void * pVoid, void * pBuffer)
{
#if (XBF_FONT_SOURCE == USE_FLASH_FONT)
	
	//从pVoid中获取字库的存储地址(pvoid的值在GUI_XBF_CreateFont中传入)
	W25QXX_Read(pBuffer,*(uint32_t *)pVoid+Offset,NumBytes);
	
	return 0;
	
#elif (XBF_FONT_SOURCE == USE_SDCARD_FONT || XBF_FONT_SOURCE == USE_FLASH_FILESYSTEM_FONT)
	
	//从pVoid中获取字库的存储地址(pvoid的值在GUI_XBF_CreateFont中传入)
	fFont_result = f_open(&fFont_file , (char *)pVoid, FA_OPEN_EXISTING | FA_READ);

	if ( fFont_result == FR_OK ) 
	{
			f_lseek (&fFont_file, Offset);		//指针偏移
		
			//读取内容
			fFont_result = f_read( &fFont_file, pBuffer, NumBytes, &fFont_num );
			
			f_close(&fFont_file);
			
			return 0;  
	}    
	else
			return 1; 

#endif

}

short LanguageDeviceInit()
{
#if (XBF_FONT_SOURCE == USE_FLASH_FONT)
  
  unsigned char buf[2]={0};
  W25QXX_Init();
  W25QXX_Read(buf,FONT_FLAG_ADDR,2);
  if((buf[0]==0xA0)&&(buf[1]==0x0A))
    return 0;
  return 1;
  
#elif (XBF_FONT_SOURCE == USE_SDCARD_FONT || XBF_FONT_SOURCE == USE_FLASH_FILESYSTEM_FONT)
	
  fFont_result = f_mount(&fs[0],FONT_STORAGE_ROOT_DIR,1);
	if ( fFont_result == FR_OK ) 
  {
    fFont_result = f_open(&fFont_file , FONT_XINSONGTI_16_ADDR, FA_OPEN_EXISTING);
    if ( fFont_result == FR_OK ) 
    {
			f_close(&fFont_file);
      return 0;
    }
  }
  return 1;
  
#endif
}



 /**
  * @brief  创建XBF字体
  * @param  无
  * @retval 无
  */
short Creat_XBF_Font(void) 
{	
  static char fontInit=0;
  if(LanguageDeviceInit()) return 1;
  if(fontInit) return 0;
	//
	// 宋体12
	//
	GUI_XBF_CreateFont(&FONT_XINSONGTI_12,    					// GUI_FONT 字体结构体指针
					   &XBF_XINSONGTI_12_Data,         						// GUI_XBF_DATA 结构体指针
					   GUI_XBF_TYPE_PROP_EXT, 										// 字体类型
					   _cb_FONT_XBF_GetData,  							// 获取字体数据的回调函数
					   (void *)&FONT_XINSONGTI_12_ADDR);        	// 要传输给回调函数的自定义数据指针，此处传输字库的地址
	//
	// 宋体16
	//
	GUI_XBF_CreateFont(&FONT_XINSONGTI_16,    					// GUI_FONT 字体结构体指针
					   &XBF_XINSONGTI_16_Data,         						// GUI_XBF_DATA 结构体指针
					   GUI_XBF_TYPE_PROP_EXT, 										// 字体类型
					   _cb_FONT_XBF_GetData,  							// 获取字体数据的回调函数
					   (void *)&FONT_XINSONGTI_16_ADDR);        	// 要传输给回调函数的自定义数据指针，此处传输字库的地址
	//
	// 宋体24
	//
	GUI_XBF_CreateFont(&FONT_XINSONGTI_24,    				// GUI_FONT 字体结构体指针
					 &XBF_XINSONGTI_24_Data,         						// GUI_XBF_DATA 结构体指针
					 GUI_XBF_TYPE_PROP_EXT, 										// 字体类型
					 _cb_FONT_XBF_GetData,  							// 获取字体数据的回调函数
					 (void *)&FONT_XINSONGTI_24_ADDR);        	// 要传输给回调函数的自定义数据指针，此处传输字库的地址
  fontInit=1;
  return 0;
}


 /**
  * @brief  GBK转UTF8
  * @param  src：输入的字符串（GBK格式）
  * @param  str：输出的字符串（UTF8格式）
  * @retval 无
  */
short GBK2UTF8(char *src, char *str)
{
	uint32_t i=0,num=0;
	uint16_t gbkdata=0;
	uint16_t UCbuffer[1024]={0};
	for(i=0,num=0;src[i]!='\0';num++)
	{
		if((uint8_t)src[i]>0x80)
		{
			gbkdata=src[i+1]+src[i]*256;
			UCbuffer[num]=ff_convert(gbkdata,1);
			i+=2;
		}
		else
		{
			UCbuffer[num]=0x00ff&src[i];
			i+=1;
		}
	}
	UCbuffer[num]='\0';
	GUI_UC_ConvertUC2UTF8(UCbuffer,2*num+2,str,num*3);
  num*=3;
	str[num]=0;
  return num;
}

/*************************************************************************************************
* 将UTF8编码转换成Unicode（UCS-2LE）编码  低地址存低位字节
* 参数：
*    char* pInput     输入字符串
*    char*pOutput   输出字符串
* 返回值：转换后的Unicode字符串的字节数，如果出错则返回-1
**************************************************************************************************/
//utf8转unicode
int Utf8ToUnicode(char* pInput, char* pOutput)
{
	int outputSize = 0; //记录转换后的Unicode字符串的字节数
 
	while (*pInput)
	{
		if (*pInput > 0x00 && *pInput <= 0x7F) //处理单字节UTF8字符（英文字母、数字）
		{
			*pOutput = *pInput;
			 pOutput++;
			*pOutput = 0; //小端法表示，在高地址填补0
		}
		else if (((*pInput) & 0xE0) == 0xC0) //处理双字节UTF8字符
		{
			char high = *pInput;
			pInput++;
			char low = *pInput;
			if ((low & 0xC0) != 0x80)  //检查是否为合法的UTF8字符表示
			{
				return -1; //如果不是则报错
			}
 
			*pOutput = (high << 6) + (low & 0x3F);
			pOutput++;
			*pOutput = (high >> 2) & 0x07;
		}
		else if (((*pInput) & 0xF0) == 0xE0) //处理三字节UTF8字符
		{
			char high = *pInput;
			pInput++;
			char middle = *pInput;
			pInput++;
			char low = *pInput;
			if (((middle & 0xC0) != 0x80) || ((low & 0xC0) != 0x80))
			{
				return -1;
			}
			*pOutput = (middle << 6) + (low & 0x3F);//取出middle的低两位与low的低6位，组合成unicode字符的低8位
			pOutput++;
			*pOutput = (high << 4) + ((middle >> 2) & 0x0F); //取出high的低四位与middle的中间四位，组合成unicode字符的高8位
		}
		else //对于其他字节数的UTF8字符不进行处理
		{
			return -1;
		}
		pInput ++;//处理下一个utf8字符
		pOutput ++;
		outputSize += 2;
	}
	//unicode字符串后面，有两个\0
	*pOutput = 0;
	 pOutput++;
	*pOutput = 0;
	return outputSize;
}

/*********************************************END OF FILE**********************/
