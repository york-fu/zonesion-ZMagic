#ifndef _rf_config_h_
#define _rf_config_h_

//xlabTools设置开/关
#define XLABTOOLS_ON    0

//Plus设置开/关
#define RF_PLUS_ON      0


#define RF_CONFIG_FILE
/*********************************************************************************************
zigbee rf config
*********************************************************************************************/
#define ZIGBEE_CONFIG_PANID     8224
#define ZIGBEE_CONFIG_CHANNEL   11

/*********************************************************************************************
ble rf config
*********************************************************************************************/

/*********************************************************************************************
WiFi rf config
*********************************************************************************************/
#define WIFI_CONFIG_SSID        "AndroidAP"
#define WIFI_CONFIG_KEYTYPE     0
#define WIFI_CONFIG_KEY         "123456789"
#define WIFI_CONFIG_SIP         "192.168.43.1"

/*********************************************************************************************
lora rf config
*********************************************************************************************/
//#define LORA_CONFIG_NODEID  0xE852          //节点地址:0x01-0xFE
#define LORA_CONFIG_NETID   50              //网络ID:0-254
#define LORA_CONFIG_FP      433             //基频
#define LORA_CONFIG_PV      15              //发射功率:1-20
#define LORA_CONFIG_SF      8               //扩频因子:6-12
#define LORA_CONFIG_CR      1               //编码率:1-4
#define LORA_CONFIG_PS      15              //前导码长度
#define LORA_CONFIG_BW      5               //带宽:0-9
#define LORA_CONFIG_HOP     0               //跳频开关:0/1
//跳频表
#define LORA_CONFIG_HOPTAB  "431,435,431,435,431,435,431,435,431,435"


#endif