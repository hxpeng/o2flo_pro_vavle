/****************************************************
*版权所有：深圳XXX有限公司
*文件名：sampling_module.h
*功能说明：
*作者：
*生成日期：
*****************************************************/
#ifndef  __SAMPLING_MODULE_H_
#define  __SAMPLING_MODULE_H_

/*****************************************************
*头文件包含
******************************************************/
#include "datatype.h"
#include "common.h"
/******************************************************
*宏定义
******************************************************/
//fifo长度
#define CURR_FIFO_BUFFER_LEN  128
#define CURR_RT_AVG_LEN  5

//平均计算缓存长度
#define TEMP_BUFFER_LEN  30
#define PRESS_BUFFER_LEN 20

#define SAMAPLING_ERROR  60000
/******************************************************
*内部类型定义
******************************************************/
typedef enum{
  SPEAKER_CH        = 0x01, // only battery power.
  FLOW_CH       = 0x02, // no battery or check battery failed.
  EEPROM_CH
}IIC_CH_TYPE;

//采样
typedef struct
{
	u16 avg_flow;     //平均流量
	u16 flow;
	u8 sensor_type;		//传感器型号
	u8 sensor_err;		//传感器错误标志
	u16 cur_adc;
	u8 IS_valve_exist;  //比例阀是否存在
	u8 IS_5V_ok;        //5V是否ok
	u8 IS_12V_ok;       //12V是否ok
	int16_t temp;       //温度
}SAMPLING_MODULE;

typedef enum{
	OPT_SENSOR_ERR = 0x01,
	AIR_SENSOR_ERR = 0x02,
	OXY_SENSOR_ERR = 0x04
}SENSOR_ERR;
/******************************************************
*外部变量声明
******************************************************/
extern SAMPLING_MODULE sampling_module;

/******************************************************
*外部函数声明
******************************************************/

void sampling_task(void* pvParamemters);
#endif
