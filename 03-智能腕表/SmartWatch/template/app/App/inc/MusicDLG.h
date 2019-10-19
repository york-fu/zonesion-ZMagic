#ifndef _musicdlg_h_
#define _musicdlg_h_

#include "DIALOG.h"
#include "App.h"
#include "View.h"
#include "fml_audio/bsp_wm8978.h"  


#define MUSIC_MAX_NUM           20
#define FILE_NAME_LEN           200			
#define MUSIC_NAME_LEN          50

#define BUFFER_SIZE             (1024*16)
#define MP3BUFFER_SIZE          2304		
#define INPUTBUF_SIZE           3000	

#define LYRIC_MAX_SIZE          200				
#define MAX_LINE_LEN            200	

#define WM_USERMSG_PLAYSTART    (WM_USER+1)
#define WM_USERMSG_PLAYOVER     (WM_USER+2)
#define WM_USERMSG_KEYEVENT     (WM_USER+3)


/* WAV文件头格式 */
typedef __packed struct
{ 
	uint32_t	riff;							/* = "RIFF"	0x46464952*///4
	uint32_t	size_8;						/* 从下个地址开始到文件尾的总字节数	*///4
	uint32_t	wave;							/* = "WAVE" 0x45564157*///4
	
	uint32_t	fmt;							/* = "fmt " 0x20746d66*///4
	uint32_t	fmtSize;					/* 下一个结构体的大小(一般为16) *///4	 
	uint16_t	wFormatTag;				/* 编码方式,一般为1	*///2
	uint16_t	wChannels;				/* 通道数，单声道为1，立体声为2 *///2
	uint32_t	dwSamplesPerSec;	/* 采样率 *///4
	uint32_t	dwAvgBytesPerSec;	/* 每秒字节数(= 采样率 × 每个采样点字节数) *///4
	uint16_t	wBlockAlign;			/* 每个采样点字节数(=量化比特数/8*通道数) *///2
	uint16_t	wBitsPerSample;		/* 量化比特数(每个采样需要的bit数) *///2
																			   
	uint32_t	data;							/* = "data" 0x61746164*///4
	uint32_t	datasize;					/* 纯数据长度 *///4
} WavHead;


typedef struct
{
	uint8_t ucVolume;			      /* 当前放音音量 */
  uint8_t ucMute;             /* 静音标志 1：静音，0：有声  */
  uint8_t ucSpk;              /* 外放状态 */
	uint32_t ucFreq;			      /* 采样频率 */
  uint32_t ucbps;            /* 比特率 ：每秒传送多少个位 */
}MusicDevice_t;

/* 播放状态 */
typedef enum
{
	STA_IDLE = 0,	/* 待机状态 */
	STA_PLAYING,	/* 放音状态 */
	STA_PAUSE,	  /* 暂停状态 */
  STA_SWITCH,   /* 切歌状态 */
}MusicStatus_t;

typedef struct
{
  FIL fs_file;
  UINT fs_num;
  FRESULT fs_result;
  char pathList[MUSIC_MAX_NUM][FILE_NAME_LEN];
  char nameList[MUSIC_MAX_NUM][MUSIC_NAME_LEN];
  uint8_t num;
}MusicFile_t;

typedef struct
{
  uint32_t allTime;
  uint32_t currTime;
  uint16_t index;
  uint8_t file[FILE_NAME_LEN];
  uint8_t type;
  uint16_t flags;
  uint8_t IsReady;
  uint8_t bufFlag;
  uint16_t dataBuffer[2][BUFFER_SIZE];
}MusicPlay_t;


typedef struct
{
	uint16_t frameSize;
	uint32_t ID3V2Size;
  uint32_t frameTime;
  int32_t readOffset;
  int32_t readSurplus;
  uint8_t data[INPUTBUF_SIZE];
  uint8_t* readPtr;
  //HMP3Decoder Mp3Decoder;/* mp3解码器指针	*/
  //MP3FrameInfo Mp3FrameInfo;/* mP3帧信息  */
}Mp3Play_t;

typedef struct
{
  MusicDevice_t device;
  MusicFile_t file;
  WM_HWIN hWin;
  MusicPlay_t play;
  //Mp3Play_t mp3;
}AppMusic_t;


extern AppMusic_t* Music;
extern MusicStatus_t MusicStatus;
PROCESS_NAME(MusicProcess);

#endif