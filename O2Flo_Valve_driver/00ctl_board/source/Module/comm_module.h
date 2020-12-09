/****************************************************
*��Ȩ���У�����XXX���޹�˾
*�ļ�����comm_module.h
*����˵����
*���ߣ�
*�������ڣ�
*****************************************************/
#ifndef  __COMM_MODULE_H_
#define  __COMM_MODULE_H_

/*****************************************************
*ͷ�ļ�����
******************************************************/
#include "datatype.h"
#include "fifo.h"
#include "common.h"

#ifndef _COMM_MODULE_H
#define _COMM_MODULE_H
#include "comm_task.h"
//#include "o2flo_system_cfg.h"
#include "stm32f0xx_hal.h"
/******************************************************
*�ڲ��궨��
******************************************************/
#define   CMD_BUFFER_LENGTH         	100  //�궨���������ݰ��ĳ���
#define	  PACK_HEAD_BYTE				0xFF //ͷ�ļ���־
#define   MODULE_CMD_TYPE				0


#define  TX_FIFO_LEN  100

//--------------------ģ�鷢��----------------//
#define MODULE_SEND_FIO2_FLOW_RT_ID   	0x01 //50HZ
#define MODULE_SEND_FIO2_FLOW_PRESS_TEMP_AVG_ID   	0x02 //1HZ
#define MODULE_SEND_CALI_PATA_ID  0x10 //1HZ ��ΪУ׼��

//����
#define MODULE_SEND_FIO2_TIME_ID  0x20
#define MODULE_SEND_TEST_ID  0x80
//--------------------��������----------------//
#define HOST_SEND_TEST_ID  0x80
#define HOST_SEND_SETTING_FLOW 0x31
#define SLAVER_SEND_DATA			 0x31

#define HOST_SEND_MODIFY_PID    						 0x32
#define SLAVER_SEND_MODIFY_PID_SUCCESS 			 0x32
#define HOSET_SEND_GET_PID									 0x33
#define SLAVER_SEND_PID_GET_RESULT					 0x33      
#define HOST_SEND_MANUAL_SET_PWM_DC					 0x34      //�ֶ����ñ�����PWM��D.C
#define HOST_SEND_TEST_VALVE_CURVE				   0x35      //���Ա���������������
#define HOST_GET_DATA				   0x36

/******************************************************
*�ڲ����Ͷ���
******************************************************/


/******************************************************
*�ⲿ��������
******************************************************/

/******************************************************
*�ⲿ��������
******************************************************/
void comm_task(void* pvParamemters);










//***************������Ҫ�û��Լ�����***************************************************

//����"���������ɻص�����������Ĭ�϶�Ӧ����A��������ξͱ���˷���B
//#define SEND_TRIGGERED_BY_TXCALLBACK 

//ProcessRcvedData()�������ж���ʹ�ã�Ҳ�����ڽ���������ʹ��
//#define PROCESS_RCV_IN_TASK

//#define UART1_ENABLE 
#define UART2_ENABLE
//#define UART3_ENABLE 
//#define UART4_ENABLE

//���崮�ڽ��յ�buffer����
#define USART1_BUFFER_SIZE  100
#define USART2_BUFFER_SIZE  100
#define USART3_BUFFER_SIZE  100
#define USART4_BUFFER_SIZE  100

//send buffer�Ŀ�������,3��ʾ��3������buffer,50��ʾÿ��buffer��50�ֽ�
//����UART3��UART4�������ֱ����ˣ�ȫ��ͨ�ó�3��50
#define SEND_BUFFER_QTY     4
#define SEND_BUFFER_SIZE    100

//*****************************************************************************************

//Ŀǰ��֧�����ַ���A��B����������о����
//A-��HAL_UART_TxCpltCallback������һ֡�ķ���
//B-�ɷ��������Լ����ƣ���ʱ��ͷ��ͣ����ù�HAL_UART_TxCpltCallback
#ifdef SEND_TRIGGERED_BY_TXCALLBACK
#define UART_SEND_PLAN_A
#else
#define UART_SEND_PLAN_B
#endif


typedef struct
{
	uint8_t flg;
	uint8_t* p_Adress;
}FLG_ADRESS_CTRL;

typedef struct
{
	uint8_t ready_2_send;   //���ͱ�־λ 1-���Է���; 0-���ܷ���
	uint8_t cur_flgAdressCtrl_No;     //��ǰbuffer flg�ı��
	FLG_ADRESS_CTRL flg_address_ctrl[SEND_BUFFER_QTY];
}DATA_SEND_CTRL;

typedef struct 
{
	uint16_t recv_flg;
	uint16_t recv_length; 
	uint32_t prev_Index;
	uint32_t cur_Index;
}UART_RCV_CTRL;


////����1�����崮�ڽ��պͷ�������buffer�ĳ���
//extern DMA_HandleTypeDef hdma_usart1_rx;
//extern UART_HandleTypeDef huart1;
//extern uint8_t g_UART1_recvBuffer[USART1_BUFFER_SIZE];
//extern uint8_t g_UART1_preProcess_Buffer[USART1_BUFFER_SIZE];
//extern UART_RCV_CTRL UART1_rcv_Ctrl;
//extern DATA_SEND_CTRL UART1_send_ctrl;

//����2�����崮�ڽ��պͷ�������buffer�ĳ���
extern DMA_HandleTypeDef hdma_uart2_rx;
extern UART_HandleTypeDef huart2;
extern uint8_t g_UART2_recvBuffer[USART2_BUFFER_SIZE];
extern uint8_t g_UART2_preProcess_Buffer[USART2_BUFFER_SIZE];
extern UART_RCV_CTRL g_UART2_Ctrl;
extern DATA_SEND_CTRL UART2_send_ctrl;

////����3�����崮�ڽ��պͷ�������buffer�ĳ���
//extern DMA_HandleTypeDef hdma_usart3_rx;
//extern UART_HandleTypeDef huart3;
//extern uint8_t g_UART3_recvBuffer[USART3_BUFFER_SIZE];
//extern uint8_t g_UART3_preProcess_Buffer[USART3_BUFFER_SIZE];
//extern UART_RCV_CTRL UART3_rcv_Ctrl;
//extern DATA_SEND_CTRL UART3_send_ctrl;

////����4�����崮�ڽ��պͷ�������buffer�ĳ���
//extern DMA_HandleTypeDef hdma_uart4_rx;
//extern UART_HandleTypeDef huart4;
//extern uint8_t g_UART4_recvBuffer[USART4_BUFFER_SIZE];
//extern uint8_t g_UART4_preProcess_Buffer[USART4_BUFFER_SIZE];
//extern UART_RCV_CTRL g_UART4_Ctrl;
//extern DATA_SEND_CTRL UART4_send_ctrl;

//����ӿ�
void uart_Init(void);
void ProcessRcvedData(void);
void sendData(void);
void Process_UART_IDLE_INTERRUPT(UART_HandleTypeDef* huart);
uint8_t* SaveDataTo(DATA_SEND_CTRL* p_send_ctrl,uint8_t* pScr,int len);
void set_checkSum(uint8_t* pArray);
#endif //_COMM_MODULE_H

#endif
