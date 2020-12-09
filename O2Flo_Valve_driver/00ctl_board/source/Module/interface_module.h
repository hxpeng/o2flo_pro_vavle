/****************************************************
*版权所有：
*文件名：interface_module.h
*功能说明：创建系统任务等
*作者：软件组
*生成日期：2017/02/27
*****************************************************/
#ifndef  __INTERFACE_MODULE_H_
#define  __INTERFACE_MODULE_H_

/*****************************************************
*头文件包含
******************************************************/
#include "stm32f0xx_hal.h"
/******************************************************
*宏定义
******************************************************/
#define ADC_OVERSAMPLING_LEN  16
#define ADC_TOTAL_LEN  5//(ADC_CH_MAX*ADC_OVERSAMPLING_LEN)

#define UART_RX_BUFFER_LEN  200
#define UART_TX_BUFFER_LEN  200

//超声波通道
#define TRIGGER_CH  TIM_CHANNEL_1
#define ZERO_CH     TIM_CHANNEL_4
/******************************************************
*类型定义
******************************************************/
//ADC_CH_FIO2, 在校准工装中使用
typedef enum{
	ADC_CH_TEMP_REF_HIGH,
	ADC_CH_TEMP_REF_LOW,
	ADC_CH_CURR,
	ADC_CH_TEMP_NTC,
	ADC_CH_MAX
}ADC_CH;

//通信
typedef enum{
	COM1,
	COM2
}COM_CNT;



/******************************************************
*外部函数声明
******************************************************/
/************************************************************************************
* 软件接口
************************************************************************************/
void interface_module_init_sys(void);
void interface_module_create_task(void);
/************************************************************************************
* 硬件接口
************************************************************************************/
void interface_module_set_dac_value(uint16_t data);
uint16_t interface_module_get_16bits_adc_value(ADC_CH ch);
void interface_module_read_flash(uint32_t addr, uint32_t *pdata, uint16_t len);
void interface_module_write_flash(uint32_t addr, uint32_t *pdata, uint16_t len);
void interface_module_receive(COM_CNT com, uint8_t* prx_buffer, uint16_t len);
void interface_module_transmit(COM_CNT com, uint8_t* prx_buffer, uint16_t len);
int GetUartReceiverResidualCnt(COM_CNT com);
void interface_module_spi_write_byte(uint8_t _ucByte);
void interface_module_spi_read_byte(uint8_t* _ucByte);
void interface_module_set_valve_putout(int16_t putout);
#endif
