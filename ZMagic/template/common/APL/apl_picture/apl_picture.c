#include "apl_picture.h"

/* 实际的测试需要是图像宽度的4倍即可，切记(也就是保证每个像素如果是32位数据的情况) */
#define PIC_BUFFRE_SIZE  (320*4)

typedef struct
{
	FIL *file;
	uint8_t *_acBuffer;
}IN_DATA;


 /**
  * @brief  BMP/GIF/JPEG获取外部图片文件数据的回调函数，作为GUI_xx_DrawEx的回调函数
  * @param  p: 外部调用时输入的指针，一般使用文件指针.
  * @param  NumBytesReq:请求的字节数。
  * @param 	ppData：指向到获取到数据的指针。“GetData”函数必须将指针设置到请求数据所在的有效位置。
  * @param  Off ：如果此标记为 1，应将数据指针设置到数据流的起始位置
  * @retval 返回可用的数据字节数
  */
static int _GetData_BMP(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) 
{

	int FileAddress = 0;
	UINT NumBytesRead;
	IN_DATA *in_data;
	FRESULT result;  

	
	in_data = (IN_DATA *)p;
	
	/*
	* 检查Buffer大小
	*/
	if (NumBytesReq > PIC_BUFFRE_SIZE) 
  {
    NumBytesReq = PIC_BUFFRE_SIZE;
	}

	/*
	* 根据Off参数设置位置
	*/
	if(Off == 1) 
		FileAddress = 0;
	else 
		FileAddress = Off;
	
	//对文件进行定位
	result =f_lseek(in_data->file, FileAddress);
	if (result != FR_OK)
	{
		return 0;
	}

  //printf("lseek:result->(%d) off->(%ld) address->(%d)\n",result,Off,FileAddress);
	/*
	* 读取数据到Buffer
	*/
	result = f_read(in_data->file, in_data->_acBuffer, NumBytesReq, &NumBytesRead);
  //printf("read:result->(%d) numreq->(%d),numread->(%d)\n",result,NumBytesReq,NumBytesRead);
	/*
	* 设置指针指向缓冲区
	*/
	*ppData = (const U8 *)in_data->_acBuffer;

	/*
	* 返回读取到的有效字节数
	*/
	return NumBytesRead;
}


 /**
  * @brief  显示文件系统中的BMP图片
  * @param  sFilename: 图片的完整路径
  * @param  usPOSX:图片的显示位置X
  * @param 	usPOSY：图片的显示位置Y
  * @retval 无
  */
void _ShowBMPEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) 
{	 
	FRESULT result = FR_OK;  
	IN_DATA in_data;
#if 0
	GUI_HMEM hMem,hMem2;	
  
	//从emwin内存中分配空间
	hMem = GUI_ALLOC_AllocZero(PIC_BUFFRE_SIZE);
  if(hMem==NULL) return;
	//把空间句柄转成指针
  in_data._acBuffer = GUI_ALLOC_h2p(hMem);
	
	//从emwin内存中分配空间
	hMem2 = GUI_ALLOC_AllocZero(sizeof(FIL));
  if(hMem2==NULL) 
  {
    //释放申请的空间
    GUI_ALLOC_Free(hMem);
    return;
  }
	//把空间句柄转成指针
  in_data.file = GUI_ALLOC_h2p(hMem2);
#else
  in_data.file = (FIL*)calloc(sizeof(FIL),1);
  in_data._acBuffer = (uint8_t*)calloc(PIC_BUFFRE_SIZE,1);
  if((in_data.file == NULL) || (in_data._acBuffer == NULL))
  {
    return;
  }
#endif
    /* 打开文件 */		
	result = f_open(in_data.file, sFilename, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
  
	if (result != FR_OK)
	{
    printf("Picturn file open error!\r\n");
	}
  else
  {
    GUI_BMP_DrawEx(_GetData_BMP, &in_data, usPOSX, usPOSY);
    f_close(in_data.file);
  }
#if 0
	//释放申请的空间
	GUI_ALLOC_Free(hMem);
		//释放申请的空间
	GUI_ALLOC_Free(hMem2);
#else
  free(in_data._acBuffer);
  free(in_data.file);
#endif
}
