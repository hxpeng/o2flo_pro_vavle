/****************************************************
*��Ȩ���У�����XXX���޹�˾
*�ļ�����sampling_module.h
*����˵����
*���ߣ�
*�������ڣ�
*****************************************************/
#ifndef  __SAMPLING_MODULE_H_
#define  __SAMPLING_MODULE_H_

/*****************************************************
*ͷ�ļ�����
******************************************************/
#include "datatype.h"
#include "common.h"
/******************************************************
*�궨��
******************************************************/
//fifo����
#define CURR_FIFO_BUFFER_LEN  128
#define CURR_RT_AVG_LEN  5

//ƽ�����㻺�泤��
#define TEMP_BUFFER_LEN  30
#define PRESS_BUFFER_LEN 20

#define SAMAPLING_ERROR  60000
/******************************************************
*�ڲ����Ͷ���
******************************************************/
typedef enum{
  SPEAKER_CH        = 0x01, // only battery power.
  FLOW_CH       = 0x02, // no battery or check battery failed.
  EEPROM_CH
}IIC_CH_TYPE;

//����
typedef struct
{
	u16 avg_flow;     //ƽ������
	u16 flow;
	u8 sensor_type;		//�������ͺ�
	u8 sensor_err;		//�����������־
	u16 cur_adc;
	u8 IS_valve_exist;  //�������Ƿ����
	u8 IS_5V_ok;        //5V�Ƿ�ok
	u8 IS_12V_ok;       //12V�Ƿ�ok
	int16_t temp;       //�¶�
}SAMPLING_MODULE;

typedef enum{
	OPT_SENSOR_ERR = 0x01,
	AIR_SENSOR_ERR = 0x02,
	OXY_SENSOR_ERR = 0x04
}SENSOR_ERR;
/******************************************************
*�ⲿ��������
******************************************************/
extern SAMPLING_MODULE sampling_module;

/******************************************************
*�ⲿ��������
******************************************************/

void sampling_task(void* pvParamemters);
#endif
