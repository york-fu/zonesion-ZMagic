#ifndef _imageviewerdlg_h_
#define _imageviewerdlg_h_

#include "DIALOG.h"
#include "App.h"
#include "View.h"
#include "ff.h"

#define PICTURE_NUM_MAX   50
#define PICTURE_NAME_LEN  50
#define PICTURE_PATH_LEN  200
#define PIC_WINSOW_WIDTH  320
#define PIC_WINSOW_HEIGHT 240

typedef struct
{
  uint16_t bfType;
  uint32_t bfSize;
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;
}BitmapFileHeader_t;

typedef struct
{
  uint32_t biSize;
  int32_t biWidth;
  int32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  uint32_t biXPelsPerMeter;
  uint32_t biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
}BitmapInfoHeader_t;


typedef struct
{
  short num;
  short index;
  char nameList[PICTURE_NUM_MAX][PICTURE_NAME_LEN];
  char pathList[PICTURE_NUM_MAX][PICTURE_PATH_LEN];
}ImageViewer_t;

PROCESS_NAME(ImageViewerProcess);

#endif