/****************************************************
*��Ȩ���У�
*�ļ�����interface_module.h
*����˵��������ϵͳ�����
*���ߣ������
*�������ڣ�2017/02/27
*****************************************************/
#ifndef  __INTERFACE_MODULE_H_
#define  __INTERFACE_MODULE_H_

/*****************************************************
*ͷ�ļ�����
******************************************************/
#include "stm32f0xx_hal.h"
/******************************************************
*�궨��
******************************************************/
#define ADC_OVERSAMPLING_LEN  16
#define ADC_TOTAL_LEN  5//(ADC_CH_MAX*ADC_OVERSAMPLING_LEN)

#define UART_RX_BUFFER_LEN  200
#define UART_TX_BUFFER_LEN  200

//������ͨ��
#define TRIGGER_CH  TIM_CHANNEL_1
#define ZERO_CH     TIM_CHANNEL_4
/******************************************************
*���Ͷ���
******************************************************/
//ADC_CH_FIO2, ��У׼��װ��ʹ��
typedef enum{
	ADC_CH_TEMP_REF_HIGH,
	ADC_CH_TEMP_REF_LOW,
	ADC_CH_CURR,
	ADC_CH_TEMP_NTC,
	ADC_CH_MAX
}ADC_CH;

//ͨ��
typedef enum{
	COM1,
	COM2
}COM_CNT;



/******************************************************
*�ⲿ��������
******************************************************/
/************************************************************************************
* ����ӿ�
************************************************************************************/
void interface_module_init_sys(void);
void interface_module_create_task(void);
/************************************************************************************
* Ӳ���ӿ�
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
